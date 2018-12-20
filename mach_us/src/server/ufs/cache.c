/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they made and grant Carnegie Mellon the
 * rights to redistribute these changes.
 */
/*
 * HISTORY:
 * $Log:	cache.c,v $
 * Revision 2.6  94/07/21  11:57:33  mrt
 * 	updated copyright
 * 
 * Revision 2.5  93/01/20  17:39:49  jms
 * 	Updated with code from roy@osf with MP bug fixes
 * 	[93/01/18  17:43:13  jms]
 * 
 * Revision 2.5  92/06/08  18:22:58  pjg
 * 	Insert missing mutex_unlock in cache_read_conflict_detect. (roy)
 * 
 * Revision 2.4  92/05/18  12:31:03  roy
 * 	Use assertions where appropriate.
 * 	[92/05/14            roy]
 * 
 * Revision 2.3  92/03/15  14:40:33  roy
 * 	91/03/09  10:23:35  roy
 * 	Added REPLY_HASH_ALIAS support.
 * 
 * 	92/03/03  17:00:33  roy
 * 	Increase INITIAL_WRITE_BUFS.
 * 
 * 	92/02/21  17:49:02  roy
 * 	Separate free lists for read and write descriptors.
 * 
 * 	92/02/19  10:48:17  roy
 * 	Must start over in cache_read_conflict_detect when conflict detected.
 * 
 * Revision 2.2  91/12/10  21:34:00  roy
 * 	91/10/07  20:09:50  roy
 * 	Major overhaul: implemented a state machine for buffer
 * 	descriptors, added a hash table and separately threaded
 * 	reclaim list, new per-buffer descriptor lock protecting
 * 	its 'state' field. 
 * 
 * Revision 2.1  91/09/23  10:23:35  roy
 * Created.
 * 
 */

/*
 * Module supporting synchronization of file data reads-aheads and 
 * write-behinds (aka write-pending).  
 */

#include <mach.h>
#include <base.h>
#include <sys/types.h>
#include <queue.h>
#include <zalloc.h>
#include "device_reply_hdlr.h"

/*
 * This module maintains a hash table and three lists:
 *   hash table - contains buffer descriptors for read-ahead buffers
 *   free list - list of free buffer descriptors
 *   write list - list of buffer descriptors for writes pending (in-progress)
 *   reclaim list - list of buffer descriptors for read-ahead buffers that are
 *     also available for reclaimation
 * 
 * When a new buffer descriptor is needed the free list is checked first and
 * then the cached list.
 *
 * The hash table keys off of block numbers rounded down to a boundary,
 * as determinted by CACHE_BLKS_PER_REGION.  The implication is that 
 * requests to this module must not straddle region boundaries.
 */
#define	NCACHE_HASH		37	/* number of entries in hash table */
#define CACHE_BLKS_PER_REGION  	128
#define CACHE_MASK	 	0xffffff80   /* corresponds to 128 */

#define CACHE_HASH(blkno) 		\
	(((daddr_t)blkno & CACHE_MASK) % NCACHE_HASH)

typedef struct cache_hash {
	struct mutex	lock;
	queue_head_t	queue;
	int 		count;
} cache_hash_t;
cache_hash_t cache_table[NCACHE_HASH];

#define	INITIAL_READ_BUFS	32      /* initial # of read descriptors */
#define	INITIAL_WRITE_BUFS	256     /* initial # of write descriptors */
#define MIN_READ_CACHE_BUFS	16      /* min # of bufs in read-ahead cache */

queue_head_t 	rfree_head;    	/* head of read free list */
queue_head_t 	wfree_head;    	/* head of write free list */
queue_head_t	write_head;   	/* head of write-pending list */
queue_head_t  	reclaim_head; 	/* head of reclaimable read_ahead buf list */

struct mutex	rfreel;	      	/* synchronizes access to read free list */
struct mutex	wfreel;	      	/* synchronizes access to write free list */
struct mutex	writel;      	/* synchronizes access to write-pending list */
struct mutex	reclaiml;    	/* synchronizes access to reclaimable list */

/*
 * Buffer descriptors move through a state machine.  Valid states are:
 */
#define	RFREE_LIST		0x0001		/* on read free list */
#define	WFREE_LIST		0x0002		/* on write free list */
#define CACHED_NOT_VALID	0x0004		/* in cache, data not vaild */
#define CACHED_VALID		0x0008		/* in cache, data vaild */
#define CLAIMED_NOT_VALID	0x0010		/* claimed, data not valid */
#define CLAIMED_VALID		0x0020		/* claimed, data valid */
#define MUST_FREE		0x0040		/* must free the buf */
#define WRITE_LIST		0x0080		/* on write-pending list */
#define LIMBO			0x0100		/* intermediate state to
						 * prevent access */
typedef struct buf_desc {	
	queue_chain_t	chain;          /* next and prev links for hash table */
	queue_chain_t	reclaim_chain;  /* next and prev links for recl. list */
	mach_port_t	devport;	/* device that data is from */
	daddr_t		blkno;          /* disk block number of data */
	int		numblks;        /* number of blocks of data */
	int		blksize;        /* block (sector) size */
	vm_address_t	buf;		/* address of buffer */
	struct mutex	lock;		/* protects 'state' field */
	short		state;          /* state of the buffer desc */
	struct condition done;		/* signalled when buffer exists */
	kern_return_t	error;		/* error code */
	mach_port_t	reply_port;     /* port used for async. i/o replies */
} buf_desc_t;

int		cache_read_alloc = 0;   /* total num read descriptors */
int		cache_write_alloc = 0;  /* total num write descriptors */
int		cache_readl_cnt = 0;	/* num descriptors in read cache */
int		cache_writel_cnt = 0;	/* num descriptors on write list */
int		cache_rfreel_cnt = 0;   /* num descriptors on read free list */
int		cache_wfreel_cnt = 0;   /* num descriptors on write free list */
int		cache_reclaiml_cnt = 0; /* num descriptors on reclaim list */

/*
 * Debugging statistics.
 */
#define	CACHE_DEBUG	1

#if	CACHE_DEBUG
#define	debug_incr_counter(x)	(x)++
#else
#define	debug_incr_counter(x)	
#endif

#if	CACHE_DEBUG
int		cache_acq_read = 0;
int		cache_acq_read_free = 0;
int		cache_acq_read_reclaim = 0;
int		cache_acq_write = 0;
int		cache_acq_write_free = 0;
int		cache_num_read_hits = 0;
int		cache_num_read_misses = 0;
int		cache_num_read_dups = 0;
int		cache_num_read_waits = 0;
int		cache_num_read_wakeups = 0;
int		cache_num_shrink_bufs = 0;
int		cache_num_reinstalls = 0;
int		cache_num_read_finishes = 0;
int		cache_num_read_aborts = 0;
int		cache_num_read_must_frees = 0;
int		cache_num_read_conflicts = 0;
int		cache_num_read_wait_writes = 0;
int		cache_num_writes = 0;
int		cache_num_write_waits = 0;
int		cache_num_write_waits_multi = 0;
int		cache_num_write_wakeups = 0;
int		cache_num_write_conflicts = 0;
int		cache_num_write_conflicts_multi = 0;
int		cache_num_write_aborts = 0;
int		cache_num_write_dups = 0;
#endif	/* CACHE_DEBUG */

/*
 * Used to determine if a buffer overlaps with (blkno, numblks).
 */
#define cache_buf_overlap(bp, bno, numb) 			        \
  (((bno <= bp->blkno) && (bno + numb > bp->blkno)) || 		   	\
  ((bno > bp->blkno) && (bno < bp->blkno + bp->numblks)))		\

/*
 * Used to determine if a buffer can satisfy a read request (blkno, numblks).
 */
#define cache_read_hit(bp, bno, numb)	 				\
	((bno == bp->blkno) &&						\
	 (bno + numb <= bp->blkno + bp->numblks))			\

/*
 * Used for synchronizing on buffer descriptors.
 */
#define buf_wait(bp, lockptr)						\
		condition_wait(&bp->done, lockptr)	

#define buf_wakeup_one(bp)						\
		condition_signal(&bp->done)

#define buf_wakeup_all(bp)						\
		condition_broadcast(&bp->done)

/*
 * The following cache_link_* macros must be called with the 
 * appropriate lock held.
 */
#define cache_link_read(bp, q)					\
	cache_readl_cnt++;					\
	queue_enter_first(q, bp, buf_desc_t *, chain);		

#define cache_unlink_read(bp, q)				\
	cache_readl_cnt--;					\
	queue_remove(q, bp, buf_desc_t *, chain);	

#define cache_link_write(bp)					\
	cache_writel_cnt++;					\
	queue_enter_first(&write_head, bp, buf_desc_t *, chain);	

#define cache_unlink_write(bp)					\
	cache_writel_cnt--;					\
	queue_remove(&write_head, bp, buf_desc_t *, chain);

#define cache_link_reclaim(bp)					\
	cache_reclaiml_cnt++;					\
	queue_enter_first(&reclaim_head, bp, buf_desc_t *, reclaim_chain);	

#define cache_unlink_reclaim(bp)			      	\
	cache_reclaiml_cnt--;					\
	queue_remove(&reclaim_head, bp, buf_desc_t *, reclaim_chain);


/* internal routines */
void 		cache_init();
void		cache_init_buf();
void 		cache_free_read();
void 		cache_free_write();
buf_desc_t *	cache_reclaim();
buf_desc_t *	cache_acquire_read();	
buf_desc_t *	cache_acquire_write();	
void		cache_reinstall();

zone_t		buf_desc_zone;
kr_fn_t		callback_read_func;
kr_fn_t		callback_write_func;

void 
cache_init(callback_read, callback_write)
	kr_fn_t			callback_read;
	kr_fn_t			callback_write;
{
	register buf_desc_t	*bp;
	int			i;

	callback_read_func = callback_read; 	/* may be needed later */ 
	callback_write_func = callback_write;  

	mutex_init(&rfreel);
	mutex_init(&wfreel);
	mutex_init(&writel);
	mutex_init(&reclaiml);
	queue_init(&rfree_head);
	queue_init(&wfree_head);
	queue_init(&write_head);
	queue_init(&reclaim_head);

	for (i = 0; i < NCACHE_HASH; i++) {
		mutex_init(&cache_table[i].lock);
		queue_init(&cache_table[i].queue);
		cache_table[i].count = 0;
	}

	/* expandable by default */
	buf_desc_zone = zinit((vm_size_t)sizeof(buf_desc_t),
			      (vm_size_t)sizeof(buf_desc_t) * 
			             (INITIAL_READ_BUFS + INITIAL_WRITE_BUFS),
			      vm_page_size,
			      "cache buf desc");
	
	for (i = 0; i < INITIAL_READ_BUFS; i++) {
		bp = (buf_desc_t *) zalloc(buf_desc_zone);
		condition_init(&bp->done);
		mutex_init(&bp->lock);
#ifdef	REPLY_PORT_ALIAS
		device_callback_enter(&bp->reply_port, (char *) bp, 
				    callback_read_func, callback_write_func);
#else
		bp->reply_port = mach_reply_port();
		device_callback_enter(bp->reply_port, (char *) bp, 
				    callback_read_func, callback_write_func);
#endif
		bp->buf = NULL;
		cache_read_alloc++;
		cache_free_read(bp);
	}

	for (i = 0; i < INITIAL_WRITE_BUFS; i++) {
		bp = (buf_desc_t *) zalloc(buf_desc_zone);
		condition_init(&bp->done);
		mutex_init(&bp->lock);
#ifdef	REPLY_PORT_ALIAS
		device_callback_enter(&bp->reply_port, (char *) bp, 
				    callback_read_func, callback_write_func);
#else
		bp->reply_port = mach_reply_port();
		device_callback_enter(bp->reply_port, (char *) bp, 
				    callback_read_func, callback_write_func);
#endif
		bp->buf = NULL;
		cache_write_alloc++;
		cache_free_write(bp);
	}
}


/*
 * Initialize the state of a buffer descriptor. 
 */
void 
cache_init_buf(bp)
	buf_desc_t		*bp;
{
	kern_return_t 		err;

	/*
	 * Deallocate any data associated with the buf desc.
	 */

	if (bp->buf != NULL) {
	        /* printf("cache_init_buf deallocating: buf=0x%x, size=%d\n",
			     bp->buf, bp->numblks*bp->blksize); */
		if ((err = vm_deallocate(mach_task_self(), bp->buf,
					 bp->numblks*bp->blksize))
		    != KERN_SUCCESS) {
			printf("vm_deallocate failure: 0x%x\n", err);
			panic("cache_init_buf");
		}
		bp->buf = NULL;
	}

	bp->numblks = bp->blksize = 0;
	bp->error = KERN_SUCCESS;
}


/* 
 * Put a buffer desccriptor on the read free list.
 */
void 
cache_free_read(bp)
	buf_desc_t		*bp;
{
	cache_init_buf(bp);  /* initialize the buf desc */
	bp->state = RFREE_LIST;
	mutex_lock(&rfreel);
	cache_rfreel_cnt++;					
	queue_enter(&rfree_head, bp, buf_desc_t *, chain);
	mutex_unlock(&rfreel);
}


/* 
 * Put a buffer desccriptor on the write free list.
 */
void 
cache_free_write(bp)
	buf_desc_t		*bp;
{
	cache_init_buf(bp); 
	bp->state = WFREE_LIST;
	mutex_lock(&wfreel);
	cache_wfreel_cnt++;					
	queue_enter(&wfree_head, bp, buf_desc_t *, chain);
	mutex_unlock(&wfreel);
}


/*
 * Reclaim a buffer from the cache.  The reclaim list is searched 
 * on an LRI basis (Least Recently Inserted into the cache).  
 */
buf_desc_t *
cache_reclaim()
{
	register buf_desc_t	*bp;
	register cache_hash_t	*c;
	
	/*
	 * Search for an unclaimed buf, starting from the
	 * end of the list.  But, make sure we leave a minimum
	 * number of bufs in the cache.
	 */
	mutex_lock(&reclaiml);
	if (cache_reclaiml_cnt <= MIN_READ_CACHE_BUFS) {
		mutex_unlock(&reclaiml);
		return(NULL);
	}
		
	for (bp = (buf_desc_t *) queue_last(&reclaim_head);  
	     !queue_end(&reclaim_head, (queue_entry_t) bp);
	     bp = (buf_desc_t *) queue_prev(&bp->reclaim_chain)) {
		mutex_lock(&bp->lock);
		if (bp->state == CACHED_VALID) {
			/*
			 * The buf is valid (meaning we're not waiting for
			 * its disk op to complete) and is not claimed.
			 * Do the job, repo man.
			 */
			bp->state = LIMBO;  	/* keep others away */
			mutex_unlock(&bp->lock);
			cache_unlink_reclaim(bp);
			mutex_unlock(&reclaiml);
			c = &cache_table[CACHE_HASH(bp->blkno)];
			mutex_lock(&c->lock);
			cache_unlink_read(bp, &c->queue);
			mutex_unlock(&c->lock);
			cache_init_buf(bp);  /* disassociate old buf contents */
			return(bp);
		} else
			mutex_unlock(&bp->lock);
	}
	mutex_unlock(&reclaiml);
	return(NULL);
}

/*
 * Acquire a free buffer descriptor to be used for reading.
 */
buf_desc_t *
cache_acquire_read()
{
	register buf_desc_t	*bp;

	debug_incr_counter(cache_acq_read);
	mutex_lock(&rfreel);
	if (!(queue_empty(&rfree_head))) {
		queue_remove_first(&rfree_head, bp, buf_desc_t *, chain);
		cache_rfreel_cnt--;					
		mutex_unlock(&rfreel);
		debug_incr_counter(cache_acq_read_free);
	} else {
		mutex_unlock(&rfreel);
		if ((bp = cache_reclaim()) == NULL) {
			/*
			 * Allocate a new buf descriptor.
			 */
			bp = (buf_desc_t *) zalloc(buf_desc_zone);
			if (bp == (buf_desc_t *) NULL)
				panic("cache_acquire_read: buf desc exhausted");
			condition_init(&bp->done);
			mutex_init(&bp->lock);
#ifdef	REPLY_PORT_ALIAS
			device_callback_enter(&bp->reply_port, (char *) bp, 
				       callback_read_func, callback_write_func);
#else
			bp->reply_port = mach_reply_port();
			device_callback_enter(bp->reply_port, (char *) bp, 
				       callback_read_func, callback_write_func);
#endif
			bp->buf = NULL;
			cache_read_alloc++;
		} else
			debug_incr_counter(cache_acq_read_reclaim);
	}

	return(bp);
}


/*
 * Acquire a free buffer descriptor to be used for writing.
 */
buf_desc_t *
cache_acquire_write()
{
	register buf_desc_t	*bp;

	debug_incr_counter(cache_acq_write);
	mutex_lock(&wfreel);
	if (!(queue_empty(&wfree_head))) {
		queue_remove_first(&wfree_head, bp, buf_desc_t *, chain);
		cache_wfreel_cnt--;					
		mutex_unlock(&wfreel);
		debug_incr_counter(cache_acq_write_free);
	} else {
		mutex_unlock(&wfreel);
		/*
		 * Allocate a new buf descriptor.
		 */
		bp = (buf_desc_t *) zalloc(buf_desc_zone);
		if (bp == (buf_desc_t *) NULL)
			panic("cache_acquire_write: buf desc exhausted");
		condition_init(&bp->done);
		mutex_init(&bp->lock);
#ifdef	REPLY_PORT_ALIAS
		device_callback_enter(&bp->reply_port, (char *) bp, 
				 callback_read_func, callback_write_func);
#else
		bp->reply_port = mach_reply_port();
		device_callback_enter(bp->reply_port, (char *) bp, 
				 callback_read_func, callback_write_func);
#endif
		bp->buf = NULL;
		cache_write_alloc++;
	}

	return(bp);
}


/*
 * Search for a buffer satisfying (devport, blkno, numblks).
 * If found, return a pointer to it, else return NULL.
 */
void *
cache_read_search(devport, blkno, numblks)
	mach_port_t		devport;
	daddr_t 		blkno;
	unsigned int		numblks;
{
	register buf_desc_t	*bp;
	register cache_hash_t	*c;
	kern_return_t		err;

	/* printf("cache_read_search'ing for blkno = %d, size = %d\n",
		     blkno, size); */

	/*
	 * We don't make this check here because frag reallaocation 
	 * (in realloccg_nbc()) may do a read that straddles region
	 * boundaries.  That's ok because the subsequent write won't
	 * straddle.
	 if ((blkno & CACHE_MASK) != ((blkno + numblks - 1) & CACHE_MASK)) {
		printf("Error: blkno=%d numblks=%d\n", blkno, numblks);
		panic("cache_read_search");
	 }
	 */
	c = &cache_table[CACHE_HASH(blkno)];
	mutex_lock(&c->lock);
	for (bp = (buf_desc_t *) queue_first(&c->queue);
	     !queue_end(&c->queue, (queue_entry_t) bp);
	     bp = (buf_desc_t *) queue_next(&bp->chain)) {
		if ((devport == bp->devport) &&
		    (cache_read_hit(bp, blkno, numblks))) {
			mutex_lock(&bp->lock);
			if ((bp->state & (CACHED_VALID | CACHED_NOT_VALID))) {
				/*
				 * Found a buf.  Remove it from the lists.
				 * If only part of the buffer is needed,
				 * it will be reinstalled into the cache 
				 * via cache_get_data().
				 */
				bp->state = (bp->state == CACHED_VALID) ?
					CLAIMED_VALID : CLAIMED_NOT_VALID;
				mutex_unlock(&bp->lock);
				cache_unlink_read(bp, &c->queue);
				mutex_unlock(&c->lock);
				mutex_lock(&reclaiml);
				cache_unlink_reclaim(bp);
				mutex_unlock(&reclaiml);

				/* printf("cache HIT.blkno=%d,numblks=%d\n",
				     blkno, numblks); */
				debug_incr_counter(cache_num_read_hits);
				return((void *) bp);
			} else 
				mutex_unlock(&bp->lock);
		}
	}
	mutex_unlock(&c->lock);

	/* printf("cache_search MISS. blkno=%d, numblks=%d\n",
				     blkno, numblks); */
	debug_incr_counter(cache_num_read_misses);
	return (NULL);
}


/*
 * Given a tag, return the data associated with it.
 * May require waiting until the data is valid.  After invocation,
 * callers of this routine may NOT use the tag again (it effectively
 * has been exchanged for the data).
 */
kern_return_t 
cache_get_data(tag, blkno, numblks, data)
	void 			*tag;
	daddr_t			blkno;
	int			numblks;
	vm_address_t		*data;  /* out */
{
	register buf_desc_t	*bp = (buf_desc_t *) tag;
	register cache_hash_t	*c;
	kern_return_t		err;
	int			round_size;
	
	mutex_lock(&bp->lock);
	ASSERT(bp->state & (CLAIMED_VALID | CLAIMED_NOT_VALID | MUST_FREE));

	/*
	 * The state of the buf desc reflects three possible situations:
	 * (1) the operation completed successfully (CLAIMED_VALID)
	 * (2) the operation encountered an error (MUST_FREE)
	 * (3) the operation hasn't completed (CLAIMED_NOT_VALID => wait)
	 */
	if (bp->state == CLAIMED_NOT_VALID) {
		/* 
		 * Operation has not yet completed.
		 * Note that we don't need to wait in a loop because we're
		 * the only one with access to the buffer.
		 */
		debug_incr_counter(cache_num_read_waits);
		buf_wait(bp, &bp->lock);
		ASSERT(bp->state & (CLAIMED_VALID | MUST_FREE));
	}

	/*
	 * At this point, the operation is complete and this thread
	 * has the buffer claimed.  Lock is held.
	 */
	if (bp->state == CLAIMED_VALID) {
		/*
		 * Data is valid.  Check for partial cache hit.  If yes,
		 * then fix up the buf descriptor's state and reinstall it 
		 * in the cache.  Otherwise, free the buf descriptor.
		 */
		mutex_unlock(&bp->lock);
		*data = bp->buf;  	/* set return argument */

		if (numblks < bp->numblks &&
		    (round_size = (numblks*bp->blksize + vm_page_size - 1) & 
		     ~(vm_page_size-1)) < bp->numblks*bp->blksize) {
			/* printf("cache_get_data: shrinking buf\n"); */
			bp->blkno = bp->blkno + round_size/bp->blksize;
			bp->numblks -= round_size/bp->blksize;
			bp->buf = (vm_address_t)((char *) bp->buf + round_size);
			bp->state = CACHED_VALID;    /* fix up state */
			debug_incr_counter(cache_num_shrink_bufs);
			cache_reinstall(bp);  /* reinstall in the cache */
		} else {
			/* don't want cache_free_read to vm_dealloc the data */
			bp->buf = NULL;  
			cache_free_read(bp);         
		}

		return(KERN_SUCCESS);

	} else {
		/*
		 * state == MUST_FREE.  Something went wrong.
		 */
		mutex_unlock(&bp->lock);
		*data = NULL;
		err = bp->error;
		cache_free_read(bp);
		return(err);
	}
}


/*
 * Called to "setup" the cache with a read operation.  We do not
 * allow overlapping reads ops to be in the cache at the same time.
 *
 * A new buffer descriptor is allocated, filled in, inserted into the cache.
 * A tag (descriptor pointer) and a reply port to be used for async reads are
 * returned to the caller.
 *
 * Either cache_read_abort() or cache_read_finish() must be called 
 * subsequently in order to "return" the buffer handle to the cache.
 */
void *
cache_read_setup(devport, blkno, numblks, blksize, reply_port)
	mach_port_t		devport;
	daddr_t			blkno;
	int			numblks;
	int			blksize;
	mach_port_t		*reply_port;  /* out */
{
	register cache_hash_t	*c;
	register buf_desc_t	*bp, *newbp;

	/*
	 * Acquire a buffer descriptor from the cache,
	 * and fill it in.
	 */
	newbp = cache_acquire_read();  
	newbp->devport = devport;
	newbp->blkno = blkno;
	newbp->numblks = numblks;
	newbp->blksize = blksize;
	newbp->state = CACHED_NOT_VALID;  /* no data yet */

	/* check to ensure request doesn't straddle region boundary */
	ASSERT((blkno & CACHE_MASK) == ((blkno + numblks - 1) & CACHE_MASK));

	/*
	 * Scan the read-ahead cache.  If we conflict with any existing
	 * bufs then abort.
	 */
	c = &cache_table[CACHE_HASH(blkno)];
	mutex_lock(&c->lock);
	for (bp = (buf_desc_t *) queue_first(&c->queue);
	     !queue_end(&c->queue, (queue_entry_t) bp);
	     bp = (buf_desc_t *) queue_next(&bp->chain)) {
		if ((devport == bp->devport) &&
		    (cache_buf_overlap(bp, blkno, numblks))) {
			mutex_unlock(&c->lock);
			debug_incr_counter(cache_num_read_dups);
			cache_free_read(newbp);
			return(NULL);
		}
	}

	/* Link into the read-ahead cache and the reclaimation list. */
	cache_link_read(newbp, &c->queue);
	mutex_unlock(&c->lock);
	mutex_lock(&reclaiml);
	cache_link_reclaim(newbp);
	mutex_unlock(&reclaiml);
	*reply_port = newbp->reply_port;
	return((void *) newbp);
}

/*
 * Similar to cache_read_setup except we already have a filled in
 * buffer descriptor (including attached data).
 */
void
cache_reinstall(newbp)
	buf_desc_t		*newbp;
{
	register cache_hash_t	*c;
	register buf_desc_t	*bp;

	/*
	 * Scan the read-ahead cache.  If we conflict with any existing
	 * bufs then abort.
	 */
	c = &cache_table[CACHE_HASH(newbp->blkno)];
	mutex_lock(&c->lock);
	for (bp = (buf_desc_t *) queue_first(&c->queue);
	     !queue_end(&c->queue, (queue_entry_t) bp);
	     bp = (buf_desc_t *) queue_next(&bp->chain)) {
		if ((newbp->devport == bp->devport) &&
		    (cache_buf_overlap(bp, newbp->blkno, newbp->numblks))) {
			mutex_unlock(&c->lock);
			debug_incr_counter(cache_num_read_dups);
			cache_free_read(newbp);
			return;
		}
	}

	/* Link into the read-ahead cache and the reclaimation list. */
	cache_link_read(newbp, &c->queue);
	mutex_unlock(&c->lock);
	mutex_lock(&reclaiml);
	cache_link_reclaim(newbp);
	mutex_unlock(&reclaiml);
	debug_incr_counter(cache_num_reinstalls);
}


/*
 * Called when a read operation needs to be aborted from the cache.
 *
 * The buffer was originally setup (and cached) by cache_read_setup().
 * Now we must remove and free the buffer.  Its state will be one of
 * CACHED_NOT_VALID, CLAIMED_NOT_VALID, or MUST_FREE.  If CLAIMED_NOT_VALID, 
 * then we must wakeup the waiting thread to do the free.  If MUST_FREE, 
 * then that's easy because we're freeing anyways.
 */
void 
cache_read_abort(tag, return_code)
	void			*tag;
	kern_return_t		return_code;
{
	register buf_desc_t	*bp = (buf_desc_t *) tag;
	register cache_hash_t	*c;

	debug_incr_counter(cache_num_read_aborts);

	mutex_lock(&bp->lock);
	ASSERT(bp->state & (CACHED_NOT_VALID | CLAIMED_NOT_VALID | MUST_FREE));

	/*
	 * If not claimed, then we unlink and free the buffer.
	 * Otherwise, the claimer must do it.
	 */
	if (bp->state & (CACHED_NOT_VALID | MUST_FREE)) {
		bp->state = LIMBO;  	/* keep others away */
		mutex_unlock(&bp->lock);
		c = &cache_table[CACHE_HASH(bp->blkno)];
		mutex_lock(&c->lock);
		cache_unlink_read(bp, &c->queue);
		mutex_unlock(&c->lock);
		mutex_lock(&reclaiml);
		cache_unlink_reclaim(bp);
		mutex_unlock(&reclaiml);
		cache_free_read(bp);
	} else { 
		/*
		 * A thread has claimed the buffer.
		 * That thread is either blocked waiting for it to be filled,
		 * or is just about to block.  In either case, it will see
		 * that bp->state = MUST_FREE.
		 */
		bp->state = MUST_FREE;
		bp->error = return_code;   /* put error code in the buf desc */
		buf_wakeup_one(bp);
		mutex_unlock(&bp->lock);
		debug_incr_counter(cache_num_read_must_frees);
		debug_incr_counter(cache_num_read_wakeups);
	}
}	


/*
 * Called upon successful completion of a read operation.
 *
 * The buffer was originally setup (and cached) by cache_read_setup().
 * Now we try to indicate that it is valid.  The buffer's state
 * will one of CACHED_NOT_VALID, CLAIMED_NOT_VALID, or MUST_FREE.  
 * If CLAIMED_NOT_VALID, then we must wakeup the waiting thread.  
 * If MUST_FREE, then there was a conflicting writer, meaning we must 
 * make sure that data is not used and the buf desc is freed.
 */
void 
cache_read_finish(tag, data, count)
	void			*tag;
	vm_address_t    	data;
	unsigned int		count;
{
	register buf_desc_t	*bp = (buf_desc_t *) tag;
	register cache_hash_t	*c;

	if (count != bp->blksize*bp->numblks || data == NULL) {
		printf("cache_read_finish Error: ");
		printf("expected=%d got=%d dataptr=0x%x\n",
		       bp->blksize*bp->numblks, count, data);
		if (data != NULL)
			(void) vm_deallocate(mach_task_self(), data, count);
		cache_read_abort(tag, KERN_FAILURE);
	}

	debug_incr_counter(cache_num_read_finishes);

	mutex_lock(&bp->lock);
	ASSERT(bp->state & (CACHED_NOT_VALID | CLAIMED_NOT_VALID | MUST_FREE));

	bp->buf = data;  	/* Set this now to ensure data is deallocated 
				 * in case cache_free_read() is called.
				 */
	if (bp->state == CACHED_NOT_VALID) {
		/*
		 * No conflict or claim took place.  Mark the buf valid.
		 */
		bp->state = CACHED_VALID;  
		mutex_unlock(&bp->lock);
	} else if (bp->state == CLAIMED_NOT_VALID) {
		bp->state = CLAIMED_VALID;
		buf_wakeup_one(bp);
		mutex_unlock(&bp->lock);
		debug_incr_counter(cache_num_read_wakeups);
	} else {
		/* state == MUST_FREE */
		mutex_unlock(&bp->lock);
		c = &cache_table[CACHE_HASH(bp->blkno)];
		mutex_lock(&c->lock);
		cache_unlink_read(bp, &c->queue);
		mutex_unlock(&c->lock);
		mutex_lock(&reclaiml);
		cache_unlink_reclaim(bp);
		mutex_unlock(&reclaiml);
		cache_free_read(bp);
	}			
}


/*
 * Remove any read-ahead buffers that conflict with (devport, blkno, numblks).
 */
void 
cache_read_conflict_remove(devport, blkno, numblks)
	mach_port_t		devport;
	daddr_t			blkno;
	int			numblks;
{
	register buf_desc_t	*bp;
	register cache_hash_t	*c;

	/* check to ensure request doesn't straddle region boundary */
        ASSERT((blkno & CACHE_MASK) == ((blkno + numblks - 1) & CACHE_MASK));

	/*
	 * Scan the read-ahead cache.
	 */
	c = &cache_table[CACHE_HASH(blkno)];
 try_again:
	mutex_lock(&c->lock);
	for (bp = (buf_desc_t *) queue_first(&c->queue);
	     !queue_end(&c->queue, (queue_entry_t) bp);
	     bp = (buf_desc_t *) queue_next(&bp->chain)) {
		if ((devport == bp->devport) &&
		    (cache_buf_overlap(bp, blkno, numblks))) {
			/*
			 * Found a conflict.  If data is not yet valid (meaning
			 * the read operation is not complete) then just change
			 * the state to 'must_free'.  Otherwise, unlink and free
			 * the buffer here.  
			 */
			debug_incr_counter(cache_num_read_conflicts);
			mutex_lock(&bp->lock);
			if (bp->state == CACHED_NOT_VALID) {
				bp->state = MUST_FREE;
				mutex_unlock(&bp->lock);
				bp->error = KERN_FAILURE; /* must set */
				debug_incr_counter(cache_num_read_must_frees);
			} else if (bp->state == CACHED_VALID) {
				bp->state = LIMBO; /* keep others away */
				mutex_unlock(&bp->lock);
				cache_unlink_read(bp, &c->queue);
				mutex_unlock(&c->lock);
				mutex_lock(&reclaiml);
				cache_unlink_reclaim(bp);
				mutex_unlock(&reclaiml);
				cache_free_read(bp);
				goto try_again; /* start over at beg. of list */
			} else {
				if (bp->state != MUST_FREE) 
				    panic("cache_read_conflict_remove HOSED");
				mutex_unlock(&bp->lock);
			}				
		}
	}
	mutex_unlock(&c->lock);
}


/*
 * Called to "setup" the cache with a write operation.  A new buffer
 * descriptor is allocated, filled in, and inserted into the cache.
 * A tag (descriptor pointer) and reply port to be used for async writes
 * are returned to the caller.
 *
 * Either cache_write_abort() or cache_write_finish() must be called 
 * subsequently in order to "return" the buffer handle to the cache.
 */
void * 
cache_write_setup(devport, blkno, numblks, blksize, data, reply_port)
	mach_port_t		devport;
	daddr_t			blkno;
	int			numblks;
	int			blksize;
	vm_address_t		data;
	mach_port_t		*reply_port;  /* out */
{
	register buf_desc_t	*bp, *newbp;
	boolean_t		conflict = FALSE;    

	debug_incr_counter(cache_num_writes);
	/*
	 * Acquire a buffer descriptor from the cache,
	 * and initialize it.
	 */
	newbp = cache_acquire_write();  
	newbp->devport = devport;
	newbp->blkno = blkno;
	newbp->numblks = numblks;
	newbp->blksize = blksize;
	newbp->buf = data;
	newbp->state = WRITE_LIST;  /* this state only used for debugging */

	/*
	 * Scan the write list, waiting for a conflict to clear.
	 */
	mutex_lock(&writel);
 try_again:
	for (bp = (buf_desc_t *) queue_first(&write_head);
	     !queue_end(&write_head, (queue_entry_t) bp);
	     bp = (buf_desc_t *) queue_next(&bp->chain)) {
		if ((devport == bp->devport) &&
		    (cache_buf_overlap(bp, blkno, numblks))) {
			debug_incr_counter(cache_num_write_waits);
			if (conflict)
				debug_incr_counter(cache_num_write_waits_multi);
			buf_wait(bp, &writel);  
			/*
			 * It's possible that this write conflicted
			 * with multiple write-pending ops.
			 */
			conflict = TRUE;
			goto try_again; 	
		}
	}

	/* Link into the write list. */
	cache_link_write(newbp);	
	mutex_unlock(&writel);
	*reply_port = newbp->reply_port;
	return((void *) newbp);
}


/*
 * Called upon successful completion of a write operation.
 */
void
cache_write_finish(tag, count)
	void			*tag;
	unsigned int		count;
{
	buf_desc_t		*bp = (buf_desc_t *) tag;
	kern_return_t		err;

	if (count != bp->blksize*bp->numblks)
		printf("Error: incomplete disk write, expected=%d wrote=%d\n",
		       bp->blksize*bp->numblks, count);
        /* 
	 * Unlink and free the buf desc, and wakeup any waiters.
	 */
	mutex_lock(&writel);
	cache_unlink_write(bp);  /* must hold lock */
	buf_wakeup_all(bp);
	mutex_unlock(&writel);
	debug_incr_counter(cache_num_write_wakeups);
	
	cache_free_write(bp);  	/* A waiter on bp is not allowed to use bp
				 * after being woken up!
				 */
}


/*
 * Called when a write operation needs to be aborted from the cache.
 */
void
cache_write_abort(tag)
	void			*tag;
{
	register buf_desc_t	*bp = (buf_desc_t *) tag;

	debug_incr_counter(cache_num_write_aborts);
	cache_write_finish(tag, bp->blksize*bp->numblks);
}


/*
 * Called by readers wishing to detect conflicting writes outstanding.
 * If wait flag is true, then this routine waits for a conflict to 
 * clear before reporting that no conflicts exist.
 */
int
cache_write_conflict_detect(devport, blkno, numblks, wait)
	mach_port_t		devport;
	daddr_t			blkno;
	int			numblks;
	boolean_t		wait;
{
	register buf_desc_t	*bp;
	boolean_t 		conflict = FALSE;

	/*
	 * Scan the write list.
	 */
	mutex_lock(&writel);
try_again:
	for (bp = (buf_desc_t *) queue_first(&write_head);
	     !queue_end(&write_head, (queue_entry_t) bp);
	     bp = (buf_desc_t *) queue_next(&bp->chain)) {
		if ((devport == bp->devport) &&
		    (cache_buf_overlap(bp, blkno, numblks))) {
			/*
			 * Found a conflicting entry.
			 */
			debug_incr_counter(cache_num_write_conflicts);
			if (conflict)
			   debug_incr_counter(cache_num_write_conflicts_multi);
			if (wait) {
				debug_incr_counter(cache_num_read_wait_writes);
				buf_wait(bp, &writel);  
				/*
				 * It's possible that this read conflicted
				 * with multiple write-pending ops.
				 */
				conflict = TRUE;
			        goto try_again;  
			} else {
				mutex_unlock(&writel);
				return(1);  /* conflict dectected */
			}
		}
	}
	
	mutex_unlock(&writel);
	return(0);  /* no conflicts */
}
