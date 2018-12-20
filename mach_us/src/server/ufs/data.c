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
 * $Log:	data.c,v $
 * Revision 2.4  94/07/21  11:57:38  mrt
 * 	updated copyright
 * 
 * Revision 2.3  93/01/20  17:39:55  jms
 * 	Updated for MP bug fixes with code from roy@osf.org
 * 	[93/01/18  17:45:55  jms]
 * 
 * Revision 2.4  92/05/24  14:47:28  pjg
 * 	Added comments related to NCPUS == 1 in the read/write finish routines.
 * 	[92/05/20            roy]
 * 
 * Revision 2.3  92/03/15  14:41:32  roy
 * 	Added calls to ux_server_thread_blocking/unblocking.
 * 
 * Revision 2.2  91/12/10  21:37:09  roy
 * 	91/10/07  20:04:44  roy
 * 	Much improved error handling; better isolation from 
 * 	cache module.
 * 
 * 	91/10/02  16:35:35  roy
 * 	Cache interface converted to use device port instead
 * 	of dev_t.
 * 
 * 	91/09/23  10:24:23  roy
 * 	Initial revision for OSF/1.
 * 
 * Revision 2.1  91/09/23  10:24:06  roy
 * Created.
 * 
 */

/*
 * Routines implementing file data abstraction.
 */

/*
 * Note on the synchronization design in this layer:
 *
 * This layer only guarantees the following:
 *   data_read() and data_write() invocations for a data range that
 *   overlaps with a prior, completed data_read() or data_write()
 *   invocation, are guaranteed to be executed after the prior invocation.
 *
 * This implies that all other ordering guarantees (to clients) must 
 * be provided at a higher level.  For example, if a client task
 * writes and then reads the same data, and in-order execution is desired,
 * it is the responsibility of a higher layer in the file server to 
 * ensure that the data_read() invocation to this layer is not executed 
 * until the data_write() invocation has completed.
 *
 * There are two aspects that require us to synchronize at this layer:
 * read-ahead and write-behind.  
 * (1) Read-ahead.  Incoming reads and writes to this layer must 
 * synchronize with asynchronously initiated read-ahead operations.  
 * The possibility of conflict arises here because the higher layer has 
 * no knowledge of asynchronous read-aheads.
 *
 * (2) Write-behind.  This layer may notify the high layer that a
 * write operation has completed when in reality the disk operation
 * has not yet occurred.  Because of this, this layer must 
 * synchronize incoming requests with outstanding write-behind operations
 * to guarantee in-order execution.
 * 
 * Synchronization Design:
 * Conceptually, there are two lists of outstanding operations, one for
 * read-aheads and one for pending writes (write-behinds).  The 
 * implementation of these lists is fully abstracted by the underlying
 * cache_* layer.  These lists allow incoming requests to be synchronized
 * with the outstanding operations.
 *
 * Read Algorithm:
 * - Try to satisfy the request from the read-ahead list.
 * - If can, get the data and return.
 * - If can't, wait until all conflicting write-behinds finish.
 * - Perform disk read (synchronously).
 * - Kick off read-ahead if needed.
 * - Return data.
 * 
 * Write Algorithm:
 * - Wait for any conflicting write operations to complete.
 * - Insert an entry in the write list (in order to block later
 *   incoming conflicting writes).
 * - Abort any conflicting read-aheads in progress (don't want stale
 *   data in the read-ahead cache).
 * - Perform the disk write (asynchronously).
 * - Return.
 *
 * Write Completion Algorithm:
 * - Remove entry from the write list.
 * - Wakeup any waiters.
 * 
 * Read-Ahead Algorithm:
 * - Insert an entry in the read-ahead list.
 * - Give up if there are any conflicting writers.
 * - Perform the disk operation (asynchronously).
 * - Return.
 * 
 * Read-Ahead Completion Algorithm:
 * - Mark data in the corresponding entry in the read-ahead list
 *   as valid (unless there was an error or a writer invalidated
 *   the entry).
 * - Wakeup any waiters.
 */

#include <mach.h>
#include <base.h>
#include <sys/types.h>
#include "disk.h"

#ifdef USE_DEVICE_RW
#include <device/device.h>
#endif USE_DEVICE_RW

/* statistics */
int			data_read_sync_num = 0;
int			data_read_try_again_set = 0;
int			data_read_try_again_got = 0;
int			data_read_async_num = 0;
int			data_write_sync_num = 0;
int			data_write_async_num = 0;

#define	ROUNDUP(size, blksize)		      	\
  ((size + blksize - 1) & ~(blksize - 1))
#define	NUMBLKS(size, blksize) 		   	\
  (size / blksize)

#define TRY_AGAIN	0xabcefd92

void 
data_init()
{
	/*
	 * Initialize the file data cache, providing it with the
	 * functions to be called upon completion of disk reads
	 * and writes.  (The callback functions are associated with
	 * the cache buffers.)
	 */
	kern_return_t	data_read_finish(), data_write_finish();

	cache_init(data_read_finish, data_write_finish);
}


/*
 * Internal routine used for read-ahead.
 */
void 
data_read_async(devport, blkno, size, blksize)
	mach_port_t	devport;
	daddr_t 	blkno;
	int		size;
	int		blksize;
{
	register void	*tag;
	register int	numblks, roundsize;		
	mach_port_t	reply_port;
	kern_return_t 	err;

	data_read_async_num++;			/* statistics */

	/*
	 * Convert from length in bytes to length in number of sectors.
	 */
	roundsize = ROUNDUP(size, blksize);
	numblks = NUMBLKS(roundsize, blksize);

	/*
	 * Prepare the cache for a read operation.  If tag == NULL then 
	 * just give up.
	 */
	if ((tag = (void *) cache_read_setup(devport, blkno, numblks, blksize,
					     &reply_port)) == NULL) 
		return;

	/*
	 * If there is a conflicting write outstanding then abort this
	 * read-ahead operation.  The error code is set to TRY_AGAIN in
	 * case another reader has already had a cache hit on this buffer.
	 */
	if (cache_write_conflict_detect(devport, blkno, numblks, FALSE)) {
		/* need better err code */
		data_read_try_again_set++;
		cache_read_abort(tag, TRY_AGAIN);
		return;
	}

#ifndef USE_DEVICE_RW
	err = disk_read_async(devport, blkno, roundsize, reply_port);
#else
	/*
	 * Perform the asynchronous read. 
	 * Data returned is a page size multiple and is guaranteed
	 * to be zero'd beyond the 'size' bytes requested.
	 */
	err = device_read_request(devport, reply_port, D_READ, 
			(recnum_t) blkno, roundsize);
#endif USE_DEVICE_RW

	if (err = KERN_SUCCESS) {
		/*
		 * Read request failed => no aysnc. reply expected.
		 */
		printf("Error: disk read request, block=%d size=%d err=0x%x\n", 
		       blkno, roundsize, err);
		cache_read_abort(tag, err);
	        return;
	}
}


int 
data_read(dev, blkno, size, rablkno, rasize, data)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	daddr_t 	rablkno;
	int 		rasize;
	vm_address_t 	*data;    /* out */
{
	register int	numblks, blksize, roundsize;
#ifdef USE_DEV_LOOKUP
	dev_rawinfo_t	*dev_rawinfo;
#endif USE_DEV_LOOKUP
	mach_port_t	devport;
	void		*tag;
	unsigned int	count;
	kern_return_t 	err;

	data_read_sync_num++;			/* statistics */

	/* Debug(printf("data_read: blkno=%d, size=%d, rablkno=%d, rasize=%d\n",
		     blkno, size, rablkno, rasize)); */

#ifdef USE_DEV_LOOKUP
	/*
	 * Find the request port and block size for the device.
	 */
	dev_rawinfo = (dev_rawinfo_t *) dev_lookup(dev, BLOCK_DEV);
	if (dev_rawinfo == NULL)
		panic("data_read null block dev");

	devport = dev_rawinfo->devport;
	blksize = dev_rawinfo->blksize;
#else
	devport = disk_device(dev);
	blksize = disk_blksize(dev);
#endif USE_DEV_LOOKUP

	/*
	 * Convert from length in bytes to length in number of sectors.
	 */
	roundsize = ROUNDUP(size, blksize);
	numblks = NUMBLKS(roundsize, blksize);
	
 try_again:
	/*
	 * See if the request can be satisfied by the cache.
	 */
	if (((void *) tag = cache_read_search(devport, blkno, numblks)) 
	    == NULL) {
		/*
		 * No buffer present in cache.  Do the following:
		 * - wait for any write conflicts to clear
		 * - perform the read operation
		 * - kick off a read-ahead if necessary.
		 */
		cache_write_conflict_detect(devport, blkno, numblks, TRUE);

#ifndef USE_DEVICE_RW
		err = disk_read(devport, blkno, roundsize, data, &count);
#else
		/*
		 * 'data' returned is a page size multiple and is guaranteed
		 * to be zero'd beyond the 'size' bytes requested.
		 */
		err = device_read(devport, D_READ, (recnum_t) blkno,
				  roundsize, (io_buf_ptr_t *) data, &count);
#endif USE_DEVICE_RW

		if (err != KERN_SUCCESS || roundsize != count) {
			printf("Error: disk read, ");
			printf("block=%d size=%d count=%d err=0x%x\n", 
			       blkno, size, count, err);
			if (*data != NULL) {
				(void) vm_deallocate(mach_task_self(), 
						     *data, count);
				*data = NULL;
			}
		        if (err == KERN_SUCCESS)
				err = KERN_FAILURE;
			return(err);
		}

		/*
		 * Zero end of sector if nec.
		 */
		if (roundsize > size)
			bzero(((char *)*data)+size, roundsize - size);
			       
		if (rablkno) 
			data_read_async(devport, rablkno, rasize, blksize);
		
		return(KERN_SUCCESS);
	} else {
		/*
		 * Got a buffer.  Kick off a read-ahead first, if necessary.
		 */
		if (rablkno) 
			data_read_async(devport, rablkno, rasize, blksize);

		/*
		 * We have a buffer, now ask for the data (may involve 
		 * waiting until the data is valid).  Note that this
		 * routine also "returns" the tag to the cache.  
		 */

		if ((err = cache_get_data(tag, blkno, numblks, data))
		    == TRY_AGAIN) {
			/*
			 * The data we hit on was aborted;
			 * see data_read_async().
			 */
			data_read_try_again_got++;
			rablkno = 0;
			goto try_again;
		} else
			return(err);
	}
}


/*
 * Callback routine on completion of asynchronous disk reads.
 */
kern_return_t 
data_read_finish(tag, return_code, data, count)
	void		*tag;
	kern_return_t	return_code;
	vm_address_t 	data;
	unsigned int 	count;
{
#if	NCPUS == 1
	/*
	 * We don't need to take the master mutex at this point because
	 * the underlying code is using cthreads mutex's (which maintain
	 * their purported semantics even if NCPUS == 1).
	 */
#endif
	if (return_code != KERN_SUCCESS) {
		printf("Error: disk read completion, err=0x%x\n", return_code);
		if (data != NULL)
			(void) vm_deallocate(mach_task_self(), data, count);
		cache_read_abort(tag, return_code);
	} else
		cache_read_finish(tag, data, count);

	return(KERN_SUCCESS);
}


int
data_write(dev, blkno, data, size, synchronous)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	vm_address_t 	data;
	boolean_t	synchronous;	
{
	register int	numblks, blksize;
#ifdef USE_DEV_LOOKUP
	dev_rawinfo_t	*dev_rawinfo;
#endif USE_DEV_LOOKUP
	mach_port_t	devport, reply_port;
	void		*tag;
	unsigned int	count;
	kern_return_t 	err;

	if (synchronous)
		data_write_sync_num++;	   	/* statistics */
	else
		data_write_async_num++;	   	/* statistics */

#ifdef USE_DEV_LOOKUP
	/*
	 * Find the request port and block size for the device.
	 */
	dev_rawinfo = (dev_rawinfo_t *) dev_lookup(dev, BLOCK_DEV);
	if (dev_rawinfo == NULL)
		panic("data_read null block dev");

	devport = dev_rawinfo->devport;
	blksize = dev_rawinfo->blksize;
#else
	devport = disk_device(dev);
	blksize = disk_blksize(dev);
#endif USE_DEV_LOOKUP

	/*
	 * Convert from length in bytes to length in number of sectors.
	 */
/*	ASSERT(size % blksize == 0); 	/% must be writing sector multiple */
	numblks = NUMBLKS(size, blksize);

	/*
	 * Prepare the cache for a write operation
	 * (synchronizing with any pending write conflicts).
	 */
	if (((void *) tag = cache_write_setup(devport, blkno, numblks, blksize,
					      data, &reply_port)) == NULL) {
		printf("Error: disk data write can't setup - not written!!\n");
		return(KERN_FAILURE);
	}

	/*
	 * Abort any conflicting read-ahead operations.
	 */
	cache_read_conflict_remove(devport, blkno, numblks);

        /*
	 * Perform the write either synchronously or asynchronously. 
	 */
	if (synchronous) {
#ifndef USE_DEVICE_RW
		err = disk_write(dev, blkno, size, data, &count);
#else
		err = device_write(devport, D_WRITE, (recnum_t) blkno, 
				   (io_buf_ptr_t) data, size, &count);
#endif USE_DEVICE_RW

		if (err != KERN_SUCCESS) {
			printf("Error: disk write, ");
			printf("block=%d size=%d err=0x%x\n", blkno, size, err);
			cache_write_abort(tag);
		} else
			cache_write_finish(tag, count);
	} else {
#ifndef USE_DEVICE_RW
		err = disk_write_async(dev, blkno, size, data, reply_port);
#else
		err = device_write_request(devport, reply_port, 
					   D_WRITE, (recnum_t) blkno, 
					   (io_buf_ptr_t) data, size);
#endif USE_DEVICE_RW

		if (err != KERN_SUCCESS) {
			/*
			 * Write request failed => no aysnc. reply expected.
			 */
			printf("Error: disk write request, ");
			printf("block=%d size=%d err=0x%x\n", blkno, size, err);
			cache_write_abort(tag);
		}
	}
	return(err);
}
	

/*
 * Callback routine on completion of asynchronous disk writes.
 */
kern_return_t 
data_write_finish(tag, return_code, count)
	void		*tag;
	kern_return_t	return_code;
	unsigned int 	count;
{
#if	NCPUS == 1
	/*
	 * We don't need to take the master mutex at this point because
	 * the underlying code is using cthreads mutex's (which maintain
	 * their purported semantics even if NCPUS == 1).
	 */
#endif
	if (return_code != KERN_SUCCESS) {
		printf("Error: disk write completion, err=0x%x\n", return_code);
		cache_write_abort(tag);
	} else
		cache_write_finish(tag, count);

	return(KERN_SUCCESS);
}
