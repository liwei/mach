/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 *
 * Purpose: Manager of Vnode-based active objects.
 *
 * HISTORY
 * $Log:	vn_mgr.cc,v $
 * Revision 2.5  94/07/07  17:25:54  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/05/17  14:08:25  jms
 * 	Cast args to cthread_fork
 * 	[94/04/28  18:56:08  jms]
 * 
 * Revision 2.3  93/01/20  17:38:37  jms
 * 	Increase the time between syncs.
 * 	[93/01/18  17:10:27  jms]
 * 
 * Revision 2.2  92/07/05  23:32:04  dpj
 * 	First working version.
 * 	[92/06/24  17:27:01  dpj]
 * 
 * Revision 2.1  91/09/27  14:04:10  pjg
 * Created.
 * 
 * Revision 2.3  91/07/01  14:13:07  jms
 * 	Clean every 30 sec., destroy every 10 sec.
 * 	[91/06/12  09:16:00  roy]
 * 
 * Revision 2.2  90/12/21  13:54:46  jms
 * 	Increase destroy rate.
 * 	[90/12/18  12:01:54  roy]
 * 
 * 	Initial revision.
 * 	(Equivalent functionality to old agency_mgr object.)
 * 	[90/12/15  15:16:00  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:21:48  jms]
 * 
 * Revision 2.1  90/12/15  15:15:24  roy
 * Created.
 * 
 */

#include	<vn_mgr_ifc.h>

#include	<vn_agency_ifc.h>


#define	BASE	usTop
DEFINE_LOCAL_CLASS(vn_mgr);


/*
 * Dummy routine to start the scanning cycle.
 */
extern "C" _c_vn_mgr_startup(vn_mgr* obj)
{
	obj->scanner();
	/* never returns */
}


vn_mgr::vn_mgr()
{
#if SHORT_CYCLE
	Local(cycle_time) = 10000;		/* msec between syncs */
#else
	Local(cycle_time) = 20000;		/* msec between syncs */
#endif SHORT_CYCLE
	Local(destroy_interval) = 1;		/* cycles between destroys */
	Local(clean_interval) = 3;		/* cycles between cleans */

	int 			i;
	aot_queue_head_t	qh;
	mutex_init(&queuel);
	for (i = 0; i < AOT_QUEUE_NUM_QUEUES; i++) {
		qh = &aot_queues[i];
		queue_init(&qh->head);
		qh->count = 0;
	}
	for (i = 0; i < AOT_NUM_BUCKETS; i++) 
		queue_init(&aot_buckets[i]);

	/*
	 * Start the scanner.
	 */
	mach_object_reference(this);
	Local(scanner_thread) = cthread_fork((cthread_fn_t)_c_vn_mgr_startup,this);
	cthread_detach(Local(scanner_thread));
}


vn_mgr::~vn_mgr()
{
	DESTRUCTOR_GUARD();

	/*
	 * Since the entire server must be shutting down, we
	 * don't bother freeing up the malloc'd entries.
	 */
}


void vn_mgr::scanner()
{
	mach_error_t		ret;
	mach_msg_header_t	msg;
	unsigned int		timeout;
	unsigned int		destroy_countdown, clean_countdown;

	/*
	 * Prepare a dummy message for sleeping.
	 */
	bzero(&msg, sizeof(msg));
	ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
					&msg.msgh_local_port);
	if (ret != ERR_SUCCESS) {
		mach_error("vn_mgr::scanner.mach_port_allocate",ret);
		return;
	}
	msg.msgh_size = sizeof(mach_msg_header_t);

	/*
	 * Get the initial destroy and clean countdowns.
	 */
	destroy_countdown = Local(destroy_interval);
	clean_countdown = Local(clean_interval);

forever:
	/*
	 * Destroy inactive objects in the active object table.
	 */
	if (--destroy_countdown == 0) {
		aot_destroy_inactive();  
		destroy_countdown = Local(destroy_interval);
	}

	/*
	 * Sync the objects in the active object table and then sync
	 * the underlying file system data structures to disk.
	 */
	if (--clean_countdown == 0) {
		aot_clean();  
		fss_sync();
		clean_countdown = Local(clean_interval);
	}

	timeout = Local(cycle_time);
	(void) mach_msg(&msg, MACH_RCV_MSG|MACH_RCV_TIMEOUT, 0,
			sizeof(msg), msg.msgh_local_port, 
			timeout, MACH_PORT_NULL);

	goto forever;
}



/*
 * Active Object Table
 *
 * The active object table (aot) is a table for storing active objects.
 * Objects can be installed in the table with an associated 'key' thus
 * allowing subsequent lookups.  A routine is also exported to allow the
 * the "state" of an object to be changed.  The allowable states
 * are currently defined statically to support vnode-based active objects,
 * but it would be possible to make this object more generic by
 * allowing the number of states and their "behavior" to be dynamic.
 * The current states are:
 *	AOT_STATE_DIR_ACTIVE	  - active non-file object (dirs, symlinks)
 *	AOT_STATE_FILE_ACTIVE     - active file 
 *	AOT_STATE_FILE_MUST_CLEAN - active file, must be cleaned 
 *	AOT_STATE_INACTIVE        - previously active object, now inactive
 *
 * The aot also exports routines for cleaning dirty files, and destroying
 * (i.e., freeing) inactive objects.  aot_clean() cleans all the files
 * in the AOT_STATE_FILE_MUST_CLEAN state, and aot_destroy_inactive() 
 * attempts to destroy all the objects (files, dirs, symlinks) in the 
 * AOT_STATE_INACTIVE state. 
 *
 * To perform its job, the aot invokes methods on the objects it contains,
 * including:
 *   vn_reference() - take a reference to an object to ensure that it won't
 *     	be deactivated.
 *   vn_clean() - clean an object.
 *   vn_destroy() - attempt to destroy an inactive object.
 *
 * The aot collaborates with active objects to maintain object reference
 * counts as follows:  the knowledge of how many "referencers" an object has
 * is maintained by the object itself.  This allows an object to take
 * appropriate action (e.g., changes its state in the aot) as its reference
 * count changes. An object's "language" reference count (i.e., the count
 * that when it goes to zero causes the object's destructor to be called) is
 * always 1.  Active objects are initially created by directory objects but
 * once the object is installed in the aot, its language reference becomes
 * owned by the aot.  When the aot successfully calls vn_destroy() on 
 * an object, it may then free the object.
 *
 * The aot is also the object where potentially conflicting operations
 * synchronize.  Mainly, we need to synchonize lookups and cleaning
 * with destroying (deactivation).  That is, lookups and cleaning must
 * prevent deactivation from occuring, and vice versa.  This is accomplished
 * via aot entry locks and object reference counts:  attempts to destroy
 * require holding the aot entry lock, but these attempts will fail
 * if the object reference count is not zero; successfully looking up or
 * cleaning on object requires obtaining an object reference, but 
 * obtaining such requires holding the aot entry lock.  Hence, diddling
 * or examining object reference counts requires that the aot entry
 * lock first be acquired.
 */

/*
 * Macros
 */
#define ENTRY_LOCK(entry)					\
	entry->locked = TRUE;

#define ENTRY_UNLOCK(entry, lockptr)				\
	mutex_lock(lockptr);					\
	entry->locked = FALSE;					\
	mutex_unlock(lockptr);					\
	condition_broadcast(&entry->ready);

#define ENTRY_IS_LOCKED(entry)					\
	(entry->locked)

#define ENTRY_WAIT(entry, lockptr)				\
	condition_wait(&entry->ready, lockptr)

#define BUCKET_HASH(key)	&aot_buckets[(int)(key) & (AOT_NUM_BUCKETS-1)]


/*
 * Internal types.
 */
struct aot_entry {
	queue_chain_t		queue_chain;	/* queue link */
	queue_chain_t		bucket_chain;	/* hash table bucket link */
	queue_head_t		*bucket_head;	/* ptr to head of bucked list */
	aot_key_t		key;		/* lookup key */
	vn_agency*		obj;		/* active object */
	aot_qnum_t		queue_num;	/* queue number */
	boolean_t		locked;		/* entry locked? */
	struct condition	ready;		/* cond var for waiting */
};


/* 
 * Get a free aot_entry.  Must be called with the bucket lock held.
 * Guaranteed to succeed.
 */
#define aot_get_free_entry_internal(e)					\
{									\
	aot_queue_head_t	qh;					\
		                                                        \
	qh = &aot_queues[AOT_QUEUE_FREE];				\
		                                                        \
	if (qh->count > 0) {						\
		queue_remove_first(&qh->head, e, aot_entry_t, queue_chain);  \
		qh->count--;						\
	} else 								\
		e = (aot_entry_t) Malloc(sizeof(struct aot_entry));	\
}


/*
 * Free an aot entry.  Must be called with the entry locked.
 *
 * Note that if there is an obj associated with the object then the
 * mach_object_reference to it will be released.
 */
void vn_mgr::aot_free_entry_internal(
	aot_entry_t		entry)
{
	mutex_lock(&queuel);
	
	if (! ENTRY_IS_LOCKED(entry))
		CRITICAL((0,"aot_free_entry_internal"));

	/*
	 * Remove from the hash table.
	 */
	queue_remove(entry->bucket_head, entry, aot_entry_t, bucket_chain);
	mutex_unlock(&queuel);

	ENTRY_UNLOCK(entry, &queuel);  /* could be threads waiting... */
	
	/*
	 * Release the (sole) mach_object_reference to the object
	 * that was given to the aot when the object was aot_install'ed.
	 * (entry->obj == NULL if we're being called on
	 * behalf of aot_cancel()).
	 */
	if (entry->obj != NULL)
		mach_object_dereference(entry->obj);

	/*
	 * Move the entry to the free list.
	 * (noone has access to the entry, but lock it anyways just to 
	 * keep aot_move_entry_internal() happy)
	 */
	ENTRY_LOCK(entry);
	aot_move_entry_internal(entry, AOT_QUEUE_FREE);
}


/*
 * Find the entry corresponding to the given key. If the entry does
 * not exist and okcreate is TRUE, create a new entry.
 *
 * A returned entry is always locked.
 */
mach_error_t vn_mgr::aot_find_entry_internal(
	aot_key_t		key,
	boolean_t		okcreate,
	aot_entry_t*		entry)	/* OUT */
{
	queue_head_t		*bh;
	aot_entry_t		e;

	/*
	 * Search the hash table for the specified key.  If we find
	 * the key but the entry is locked, wait for the lock to clear
	 * and retry the search from the beginning (because the entry 
	 * may have been removed from the hash table).
	 */
	bh = BUCKET_HASH(key);

	mutex_lock(&queuel);
retry:
	for (e = (aot_entry_t) queue_first(bh);
	     !queue_end(bh, (queue_entry_t) e);
	     e = (aot_entry_t) queue_next((queue_entry_t) &e->bucket_chain) ) 
		if (key == e->key)
			if (ENTRY_IS_LOCKED(e)) {
				ENTRY_WAIT(e, &queuel);
				goto retry;
			} else
				break;  /* found an entry */

	if (queue_end(bh, (queue_entry_t) e)) 
		if (okcreate) {
			/*
			 * Must create a new entry in the aot.
			 * Acquire a free entry, initialize it, and
			 * put it into the hash table.
			 */
			aot_get_free_entry_internal(e);
			condition_init(&e->ready);
			e->bucket_head = bh;
			e->key = key;
			e->queue_num = AOT_QUEUE_NULL;
			e->obj = NULL;
			queue_enter(bh, e, aot_entry_t, bucket_chain);
		} else {
			mutex_unlock(&queuel);
			*entry = NULL;
			return(US_OBJECT_NOT_FOUND);
		}

	/* have an entry - lock it */
	ENTRY_LOCK(e);

	mutex_unlock(&queuel);
	*entry = e;
	return(ERR_SUCCESS);
}


/*
 * Move an entry to the end of the specified queue.
 * This routine must be invoked with the entry locked.
 * 
 * If the entry is already on the specified queue then this
 * routine guarantees that the entry will not be moved (e.g.,
 * unlinked from the queue and linked back in again).  It's
 * also important that new entries are added to the *end*.
 * The process of cleaning relies on both these facts.
 */
void vn_mgr::aot_move_entry_internal(
	aot_entry_t		entry,
	aot_qnum_t		newq)
{
	aot_queue_head_t	oqh;
	aot_queue_head_t	nqh;

	mutex_lock(&queuel);

	if (! ENTRY_IS_LOCKED(entry))
		CRITICAL((0,"aot_move_entry_internal"));

	/*
	 * Nothing to do if entry is already on the proper queue.
	 */
	if (newq == entry->queue_num) {
		mutex_unlock(&queuel);
		return;
	}

	/*
	 * Remove from current queue.
	 */
	if (entry->queue_num != AOT_QUEUE_NULL) {
		oqh = &aot_queues[entry->queue_num];
		queue_remove(&oqh->head, entry, aot_entry_t, queue_chain);
		oqh->count--;
	}

	nqh = &aot_queues[newq];
	queue_enter(&nqh->head, entry, aot_entry_t, queue_chain);
	nqh->count++;
	entry->queue_num = newq;

	mutex_unlock(&queuel);
}


/*
 * Lock an aot entry corresponding to the given key.
 */
mach_error_t vn_mgr::aot_lock(
	aot_key_t		key,
	void**			entry)
{
	/*
	 * Entry is left locked by aot_find_entry_internal.
	 */
	return(aot_find_entry_internal(key, FALSE, (aot_entry_t*) entry));
}


/*
 * Unlock an aot entry.  Companion to aot_lock().
 */
void vn_mgr::aot_unlock(
	void*			entry)
{
	ENTRY_UNLOCK(((aot_entry_t) entry), &queuel);
}


/*
 * Get the state of an entry.
 * Must be called with a lock held on the entry.
 */
aot_state_t vn_mgr::aot_get_state(
	void*			entry)
{
	return((aot_state_t)((aot_entry_t)entry)->queue_num);
}


/*
 * Set the state of an entry.
 * Must be called with a lock held on the entry.
 */
void vn_mgr::aot_set_state(
	void*			entry,
	aot_state_t		state)
{
	aot_move_entry_internal((aot_entry_t) entry,(aot_qnum_t) state);
}


/*
 * Look-up an object corresponding to a given key,
 * or reserve a new entry it if it does not already exist.
 *
 * If the "obj" argument is NULL on exit, then a new entry designated
 * by "tag" has been reserved.  Otherwise, "tag" should be ignored.
 * 
 * We assume that either aot_cancel() or aot_install() will subsequently 
 * be called.  If an entry was found, an vn_reference for the object is
 * automatically obtained.
 */
mach_error_t vn_mgr::aot_lookup_or_reserve(
	aot_key_t		key,
	vn_agency**		obj,		/* OUT */
	int*			tag)		/* OUT */
{
	mach_error_t		ret;
	aot_entry_t		entry;

	*obj = NULL;
	*tag = 0;

	if ((ret = aot_find_entry_internal(key, TRUE, &entry))  
	    != ERR_SUCCESS)
		return(ret);

	/*
	 * If we found an existing entry, obtain a vn_reference for the caller.
	 * Otherwise, we are reserving a new entry and we just leave it locked.
	 */
	if (entry->obj != NULL) {
		entry->obj->vn_reference();  /* get a reference */
		*obj = entry->obj;
		ENTRY_UNLOCK(entry, &queuel); 
	} else {
		*tag = (int) entry;
	}

	return(ERR_SUCCESS);
}


/*
 * Cancel a reservation for an entry.  The entry was obtained
 * via a call to aot_lookup_or_reserve() and must be locked.
 * Upon exit, the tag is invalid.
 */
void vn_mgr::aot_cancel(
	int			tag)
{
	aot_free_entry_internal((aot_entry_t) tag);
}


/*
 * Install an object in a previously reserved entry.  The entry
 * was obtained via a call to aot_lookup_or_reserve() and must be locked.
 * Upon exit, the entry is no longer locked.
 */
void vn_mgr::aot_install(
	int			tag,
	vn_agency*		obj,
	aot_state_t		state)
{
	((aot_entry_t)tag)->obj = obj;
	aot_move_entry_internal((aot_entry_t) tag, (aot_qnum_t) state);
	ENTRY_UNLOCK(((aot_entry_t) tag), &queuel);

}


/*
 * Enter a given object in the table, provided its key is not already 
 * registered. 
 */
mach_error_t vn_mgr::aot_enter(
	aot_key_t		key,
	vn_agency*		obj,
	aot_state_t		state)
{
	mach_error_t		ret;
	aot_entry_t		entry;

	ret = aot_find_entry_internal(key, TRUE, &entry);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	if (entry->obj != NULL) {
		ENTRY_UNLOCK(entry, &queuel);
		return(US_OBJECT_EXISTS);
	}

	entry->obj = obj;
	aot_move_entry_internal(entry, (aot_qnum_t) state);
	ENTRY_UNLOCK(entry, &queuel);

	return(ret);
}


/*
 * Clean all writable files.
 */
void vn_mgr::aot_clean()
{
	aot_queue_head_t	qh;
	aot_entry_t		entry, last_entry;
	unsigned int		clean_count;

	/*
	 * This routine relies on the fact that the only way an
	 * entry on the MUST_CLEAN queue can move to another queue
	 * is via the act of cleaning (i.e., during the execution
	 * of vn_clean()).  Hence, when this routine begins 
	 * executing it knows the exact number of entries that need
	 * to be cleaned.  As each entry is processed it is removed
	 * from the front of the queue, added to the end, and then
	 * cleaned.  It is imperative that any new entries added to
	 * MUST_CLEAN queue are added to the end.
	 */

	qh = &aot_queues[AOT_QUEUE_FILE_MUST_CLEAN];

	mutex_lock(&queuel);
	clean_count = qh->count;  	/* number of entries to clean */
	last_entry = (aot_entry_t) queue_last(&qh->head);  /* sanity checking */

	while (clean_count--) {
		/*
		 * Move the current entry to the end of the queue and
		 * then clean.  
		 */
		queue_remove_first(&qh->head, entry, aot_entry_t,
			     queue_chain);
		queue_enter(&qh->head, entry, aot_entry_t, 
			     queue_chain);

		if (clean_count == 0 && entry != last_entry)  
			/* sanity check */
			CRITICAL((0,"aot_clean"));
		
		mutex_unlock(&queuel);
		entry->obj->vn_clean();  	/* womp! */
		mutex_lock(&queuel);
	}
	mutex_unlock(&queuel);

}


short		aot_destroy_50_bucket = 0;  	/* statistics */
short		aot_destroy_100_bucket = 0;
short		aot_destroy_150_bucket = 0;
short		aot_destroy_200_bucket = 0;
short		aot_destroy_300_bucket = 0;
short		aot_destroy_big_bucket = 0;

short		aot_destroy_100_le50_bucket = 0;
short		aot_destroy_100_gt50_bucket = 0;
short		aot_destroy_200_le50_bucket = 0;
short		aot_destroy_200_gt50_bucket = 0;
short		aot_destroy_300_le50_bucket = 0;
short		aot_destroy_300_gt50_bucket = 0;
short		aot_destroy_big_le50_bucket = 0;
short		aot_destroy_big_gt50_bucket = 0;
/*
 * Garbage collect inactive objects.  
 */
void vn_mgr::aot_destroy_inactive()
{
	aot_queue_head_t	qh;
	aot_entry_t		entry;
	unsigned int		destroy_count;
	mach_error_t		ret;
	short			attempts = 0;
	short			succeeds = 0;

	/*
	 * Unlike the process of cleaning, this routine cannot rely
	 * on entries remaining on the INACTIVE queue for the 
	 * duration of time that vn_destroy() is executing.
	 * However, it's ok to just attempt to destroy the number
	 * entries that exist at the beginning of the destroy process.
	 * The worst that can happen is that we'll attempt to destroy
	 * some entries more than once.
	 */

	qh = &aot_queues[AOT_QUEUE_INACTIVE];

	mutex_lock(&queuel);
	destroy_count = qh->count;  	/* number of entries to destroy */

	while (destroy_count--) {
		/*
		 * Move the current entry to the end of the queue and
		 * then attempt to destroy it.  But, must wait if it's
		 * locked (and possibly being moved to a different queue).
		 * There's a possibility that an entry is moved off our
		 * queue and then back again while we wait, but that's
		 * harmless.
		 */
		entry = (aot_entry_t) queue_first(&qh->head);
		while (ENTRY_IS_LOCKED(entry)) 
			ENTRY_WAIT(entry, &queuel);

		if (entry->queue_num != AOT_QUEUE_INACTIVE)
			/* entry moved to a different queue while we waited */
			continue;

		queue_remove(&qh->head, entry, aot_entry_t,
			     queue_chain);
		queue_enter(&qh->head, entry, aot_entry_t, 
			     queue_chain);

		ENTRY_LOCK(entry);
		mutex_unlock(&queuel);

		attempts++;  /* statistics */
		if (entry->obj->vn_destroy() == ERR_SUCCESS) {
	                /*
			 * Gonzo!  Finish the job.
			 *
			 * Note that the following call will cause
			 * the sole mach_object_reference for entry->obj
			 * to be released.
			 */
			succeeds++;  /* statistics */
			aot_free_entry_internal(entry);
		} else {
			/*
			 * The entry shouldn't have changed queues because 
			 * it is locked.
			 */
			if (entry->queue_num != AOT_QUEUE_INACTIVE) 
				CRITICAL((0,"aot_destroy_inactive"));
			ENTRY_UNLOCK(entry, &queuel);
		}
		mutex_lock(&queuel);

	}

	mutex_unlock(&queuel);

	/*
	 * Gather statistics.
	 */
	if (attempts <= 50) {
		aot_destroy_50_bucket++;
		if (attempts/2 < succeeds)
			aot_destroy_100_gt50_bucket++;
		else
			aot_destroy_100_le50_bucket++;
	} else if (attempts <= 100) {
		aot_destroy_100_bucket++;
		if (attempts/2 < succeeds)
			aot_destroy_100_gt50_bucket++;
		else
			aot_destroy_100_le50_bucket++;
	} else if (attempts <= 150) {
		aot_destroy_150_bucket++;
		if (attempts/2 < succeeds)
			aot_destroy_200_gt50_bucket++;
		else
			aot_destroy_200_le50_bucket++;
	} else if (attempts <= 200) {
		aot_destroy_200_bucket++;
		if (attempts/2 < succeeds)
			aot_destroy_200_gt50_bucket++;
		else
			aot_destroy_200_le50_bucket++;
	} else if (attempts <= 300) {
		aot_destroy_300_bucket++;
		if (attempts/2 < succeeds)
			aot_destroy_300_gt50_bucket++;
		else
			aot_destroy_300_le50_bucket++;
	} else {
		if (attempts/2 < succeeds)
			aot_destroy_big_gt50_bucket++;
		else
			aot_destroy_big_le50_bucket++;
		aot_destroy_big_bucket++;
	}
}








