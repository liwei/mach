/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/pager_base.cc,v $
 *
 * Purpose: General-purpose pager for I/O objects.
 *
 * Note: The current implementation of this class relies heavily on earlier
 * pagers written by Richard Sanzi and Joseph Barrera.
 *
 * HISTORY
 * $Log:	pager_base.cc,v $
 * Revision 2.5  94/07/07  17:23:58  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/05/17  14:07:22  jms
 * 	Cast args to cthread_fork
 * 	[94/04/28  18:48:44  jms]
 * 
 * Revision 2.3  93/01/20  17:38:05  jms
 * 	Fix logic for "destroying" memory objects.
 * 	[93/01/18  16:48:21  jms]
 * 
 * Revision 2.2  92/07/05  23:28:17  dpj
 * 	First working version.
 * 	[92/06/24  17:00:08  dpj]
 * 
 * Revision 1.19  92/03/05  15:06:46  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:43:00  jms]
 * 
 * Revision 1.18  91/11/13  17:18:43  dpj
 * 	No changes.
 * 	[91/11/12  17:55:46  dpj]
 * 
 * Revision 1.17  91/10/06  22:30:08  jjc
 * 	Added memory_object_change_completed().
 * 	Picked up from Dan Julin:
 * 	Added memory_object_supply_completed() and memory_object_data_return();
 * 	dummy routines for now.
 * 	Use mach_port_destroy() instead of mach_mach_port_deallocate() when dealing
 * 	with receive rights.
 * 	[91/10/03            jjc]
 * 
 * Revision 1.16  91/07/01  14:12:23  jms
 * 	Removed max_count arg in call to io_pagein().
 * 	[91/06/25  12:03:13  roy]
 * 
 * 	- new iop_flush method to flush pages from memory
 * 	- iop_deactivate now takes an arg to forcibly destroy 
 * 	  the memory object
 * 	- re-enable memory object caching
 * 	[91/06/05  13:57:32  roy]
 * 
 * 	- pageout buffer is consumed by io_pageout
 * 	- remove read-ahead logic at this level
 * 	- temporarily disable caching until truncate is
 * 	  implemented properly
 * 	[91/05/29  10:33:33  roy]
 * 
 * Revision 1.15  91/05/05  19:27:08  dpj
 * 	From roy@osf.org: disabled caching.
 * 	[91/04/29            dpj]
 * 
 * Revision 1.14  90/12/21  13:53:57  jms
 * 	Add a new state to the pager object to fix a race 
 * 	condition between iop_deactivate() and mo_init().  This
 * 	functionality to be replaced with use of no-more-senders.
 * 	[90/12/19  11:51:54  roy]
 * 
 * 	Add statistics gathering.
 * 	[90/12/18  12:03:50  roy]
 * 
 * 	Initial state of a pager object is frozen.  Becomes ready
 * 	when it receives the memory_object_init() message.
 * 	[90/12/15  15:07:11  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:20:10  jms]
 * 
 * Revision 1.13  90/11/27  18:20:28  jms
 * 	Add new ipc "rights" fix from Paul Roy for pager_iop_get_port
 * 	[90/11/20  14:29:06  jms]
 * 
 * 
 * Revision 1.12.2.1  90/11/05  18:27:50  roy
 * 	Use the new io_pagein() and io_pageout() calls.
 * 	When allocating a port must explicitly insert send
 * 	rights if not MACH3_UNIX.
 * 
 * 
 * Revision 1.12  89/11/28  19:11:51  dpj
 * 	Brand-new implementation.
 * 	[89/11/20  20:55:21  dpj]
 * 
 */
 
#include	<pager_base_ifc.h>

extern "C" {
#include 	<mach_error.h>
#include 	<mig_errors.h>
#include	<mach/message.h>
#include	<hash.h>
#include	<mach/memory_object.h>
#include	<us_error.h>
#include	<io_types.h>
extern mach_error_t mach_msg_receive();
}

#define	Local(foo)	this->foo


/*
 * Configuration parameters. Switch at will.
 */
int				pager_debug = TRUE;
memory_object_copy_strategy_t	pager_copy_strategy = MEMORY_OBJECT_COPY_DELAY;

/*
 * Statistics
 */
int	pager_num_inits = 0;
int	pager_num_terminates = 0;
int	pager_num_data_requests = 0;
int	pager_num_data_writes = 0;
int	pager_num_setups = 0;
int	pager_num_takedowns = 0;
int	pager_num_cleans = 0;
int	pager_num_deactivate_attempts = 0;
int	pager_num_deactivate_succeeds = 0;
int	pager_num_destroys = 0;
int	pager_num_flushes = 0;

/*
 * Verify that the arguments for a paging request make sense.
 */
#if	_DEBUG_
#define	VERIFY(memory_control,offset,length) {				\
	if (memory_control == MACH_PORT_NULL) {				\
		ERROR((Diag,"VERIFY: null memory_control\n"));		\
		return(US_INTERNAL_ERROR);				\
	}								\
	if (memory_control != Local(memory_control)) {			\
		ERROR((Diag,"VERIFY: invalid memory_control\n"));	\
		return(US_INTERNAL_ERROR);				\
	}								\
	if (length % vm_page_size) {					\
		ERROR((Diag,"VERIFY: invalid length %d\n",length));	\
		return(US_INTERNAL_ERROR);				\
	}								\
	if (offset % vm_page_size) {					\
		ERROR((Diag,"VERIFY: invalid offset %d\n",offset));	\
		return(US_INTERNAL_ERROR);				\
	}								\
}
#else	_DEBUG_
#define	VERIFY(memory_control,offset,length) /* */
#endif	_DEBUG_


/*
 * Record to enqueue paging requests.
 */
typedef struct iop_request {
	dll_chain_t		chain;
	vm_offset_t		offset;
	vm_size_t		length;
	vm_prot_t		desired_access;
	boolean_t		needs_data;
} *iop_request_t;


/*
 * For now, use a dedicated paging thread for the memory object management,
 * instead of the standard MachRemoteObject facility.
 */
#define	PAGING_THREAD	1
#if	PAGING_THREAD
extern mach_error_t 	paging_thread_init();
extern mach_error_t 	paging_thread_export(mach_port_t,pager_base*);
extern mach_error_t 	paging_thread_terminate(mach_port_t);
#endif	PAGING_THREAD


pager_base::pager_base()
	:
	started(FALSE)
{}


pager_base::~pager_base()
{
	if (!started) return;

	DEBUG1(pager_debug,(Diag,"pager_terminate()\n"));
		
	if (! dll_empty(&Local(requests))) {
		ERROR((Diag,
		"pager_terminate() called with non-null requests list\n"));
	}

	if (Local(memory_control) != MACH_PORT_NULL) {
		ERROR((Diag,
	"pager_terminate() called with non-null memory_control port\n"));
		(void) mach_port_destroy(mach_task_self(),Local(memory_control));
	}
	if (Local(memory_object) != MACH_PORT_NULL) {
		ERROR((Diag,
	"pager_terminate() called with non-null memory_object port\n"));
		(void) mach_port_destroy(mach_task_self(),Local(memory_object));
	}

	pager_num_takedowns++;  /* statistics */
}


mach_error_t pager_base::pager_base_start(boolean_t _pager_may_cache)
{
	mutex_init(&Local(lock));
	Local(state) = O_STATE_READY;
	Local(memory_object) = MACH_PORT_NULL;
	Local(memory_control) = MACH_PORT_NULL;
	Local(min_offset) = 0xffffffff;
	Local(max_offset) = 0;
	dll_init(&Local(requests));
	condition_init(&Local(cond));
	Local(done_pagein) = FALSE;
	Local(done_pagein_write) = FALSE;
	Local(done_pageout) = FALSE;
	Local(pager_may_cache) = _pager_may_cache;
	Local(destroying) = FALSE;

	pager_num_setups++;  /* statistics */

	started = TRUE;

	return(ERR_SUCCESS);
}


/*
 * Get the memory object port, to be used in vm_map().
 *
 * Delay the actual allocation of the memory object
 * as long as possible.
 */
mach_error_t pager_base::iop_get_port(
	mach_port_t*			memory_object)		/* OUT */
{
	mach_error_t			ret = ERR_SUCCESS;
	mach_port_t			port;

    	mutex_lock(&Local(lock));

	if (Local(memory_object) == MACH_PORT_NULL) {
#if	PAGING_THREAD
		(void) paging_thread_init();
#if 	!defined(MACH3_UNIX) 
		if ((ret = mach_port_allocate(mach_task_self(), 
			MACH_PORT_RIGHT_RECEIVE, (mach_port_t *) &port))
		    != ERR_SUCCESS)
			return(ret);

		if ((ret = mach_port_insert_right(mach_task_self(), 
						  (mach_port_t) port, 
						  (mach_port_t) port, 
						  MACH_MSG_TYPE_MAKE_SEND))
		    != ERR_SUCCESS) {
			mach_port_destroy(mach_task_self(),(mach_port_t)port);
			return(ret);
		}

		Local(memory_object) = port;
		
#else 	!defined(MACH3_UNIX) 
		(void) port_allocate(mach_task_self(),&Local(memory_object));
#endif 	!defined(MACH3_UNIX) 
		ret = paging_thread_export(Local(memory_object),this);
#else	PAGING_THREAD
		not implemented
#endif	PAGING_THREAD

		/*
		 * This indicates that we are expecting a mo_init message,
		 * and will prevent iop_deactivation from succeeding.  The 
		 * problem was that we could succeed at iop_deactivate'ing
		 * a paging object (because the last agent had gone away)
		 * although a mo_init message was still on the way.  See the
		 * comment in pager_ifc.h.  Note that we only do this
		 * for the first invocation of this routine.
		 */
		Local(state) = O_STATE_EXPECT_INIT;
	}

	*memory_object = Local(memory_object);

	mutex_unlock(&Local(lock));	
	return(ret);
}


/*
 * Clean all dirty pages currently in the object.
 *
 * This call blocks waiting for the dirty pages to be written back.
 *
 * The pager is left frozen on exit.
 */
mach_error_t pager_base::iop_clean(
	boolean_t*		dirty)			/* OUT */
{
	mach_error_t		ret;
	
	DEBUG1(pager_debug,(Diag,"iop_clean() called\n"));

	mutex_lock(&Local(lock));

	if (Local(state) != O_STATE_READY) {
		mutex_unlock(&Local(lock));
		DEBUG1(pager_debug,(Diag,
				"iop_clean(): object not in ready state\n"));
		return(US_OBJECT_BUSY);
	}

	if ((! Local(done_pagein_write)) ||
				(Local(memory_control) == MACH_PORT_NULL)) {
		Local(state) = O_STATE_FROZEN;
		*dirty = FALSE;
		mutex_unlock(&Local(lock));
		DEBUG1(pager_debug,(Diag,"iop_clean(): object not dirty\n"));
		return(ERR_SUCCESS);
	}

	DEBUG1(pager_debug,(Diag,"iop_clean(): reclaiming dirty pages\n"));

	Local(state) = O_STATE_RECLAIMING;

	mutex_unlock(&Local(lock));
	ret = memory_object_lock_request(Local(memory_control),
				Local(min_offset),
				Local(max_offset) - Local(min_offset),
				TRUE,FALSE,VM_PROT_WRITE,Local(memory_object));
	mutex_lock(&Local(lock));
	if (ret != ERR_SUCCESS) {
		ERROR((Diag,
			"iop_clean.memory_object_lock_request() failed: %s\n",
						mach_error_string(ret)));
		Local(state) = O_STATE_READY;
		mutex_unlock(&Local(lock));
		return(ret);
	}

	while (Local(state) != O_STATE_FROZEN) {
		condition_wait(&Local(cond),&Local(lock));
	}

	DEBUG2(pager_debug,(Diag,"iop_clean -> SUCCESS"));

	*dirty = Local(done_pageout);
	Local(done_pagein_write) = FALSE;
	Local(done_pageout) = FALSE;

	mutex_unlock(&Local(lock));

	pager_num_cleans++;  /* statistics */

	return(ERR_SUCCESS);
}


/*
 * Restore access to the data after iop_clean().
 */
mach_error_t pager_base::iop_resume()
{
	mach_error_t		ret;
	iop_request_t		req;

	DEBUG1(pager_debug,(Diag,"iop_resume() called\n"));

	mutex_lock(&Local(lock));

	if (Local(state) != O_STATE_FROZEN) {
		mutex_unlock(&Local(lock));
		return(US_OBJECT_BUSY);
	}

	/*
	 * XXX Consider releasing the lock before scanning the
	 * list. The problem is there is that new requests may come
	 * in before we are done, including mo_data_write()'s.
	 */
	req = (iop_request_t) dll_first(&Local(requests));
	while (! dll_end(&Local(requests),(dll_entry_t)req)) {
		ret = iop_satisfy_request(req->offset,req->length,
					req->desired_access,req->needs_data);
		if (ret != ERR_SUCCESS) {
			ERROR((Diag,
			"iop_resume.iop_satisfy_request() failed: %s\n",
						mach_error_string(ret)));
		}
		dll_remove(&Local(requests),req,iop_request_t,chain);
		free(req);
		req = (iop_request_t) dll_first(&Local(requests));
	}

	Local(state) = O_STATE_READY;

	mutex_unlock(&Local(lock));
	return(ERR_SUCCESS);
}


/*
 * Test whether we are in a state from which we can exit cleanly.
 * Destroy flag says to forcibly destroy the underlying memory object.
 */
mach_error_t pager_base::iop_deactivate(
	boolean_t		destroy,
	boolean_t*		dirty)			/* OUT */
{
	mach_error_t		ret;
	int			old_state;

	mutex_lock(&Local(lock));

	/*
	 * Destroy flag indicates that we should tell the kernel to
	 * destroy the memory object.  This will result in a terminate
	 * msg from the kernel.  memory_control==MACH_PORT_NULL implies that
	 * the memory object has already been terminated.
	 */
	pager_num_deactivate_attempts++;  /* statistics */
	
	DEBUG2(pager_debug,(Diag,"iop_deactivate called"));

	if ((Local(state) != O_STATE_READY) ||
	    (Local(memory_control) != MACH_PORT_NULL)) {
		if (destroy && (Local(memory_control)) != MACH_PORT_NULL) {
			DEBUG0(pager_debug,(Diag,"attempting_destroy: 0x%x\n",
					Local(memory_control)));
			pager_num_destroys++;  /* statistics */

			Local(destroying) = TRUE;
#if DISCARD_BEFORE_DESTROY
			(void)memory_object_lock_request(Local(memory_control),
				Local(min_offset),
				Local(max_offset) - Local(min_offset),
				MEMORY_OBJECT_RETURN_NONE, TRUE,
				VM_PROT_ALL, MACH_PORT_NULL);

			cthread_yield();
#endif DISCARD_BEFORE_DESTROY
			(void)memory_object_destroy(Local(memory_control), 
						KERN_SUCCESS);
		}
		mutex_unlock(&Local(lock));
		DEBUG2(pager_debug,(Diag,"iop_deactivate -> BUSY"));
		return(US_OBJECT_BUSY);
	}

	pager_num_deactivate_succeeds++;  /* statistics */

	if (Local(memory_object) == MACH_PORT_NULL) {
		mutex_unlock(&Local(lock));
		*dirty = FALSE;
		Local(done_pageout) = FALSE;
		DEBUG2(pager_debug,(Diag,"iop_deactivate -> SUCCESS"));
		return(ERR_SUCCESS);
	}

	*dirty = Local(done_pageout);
	Local(done_pageout) = FALSE;

#if	PAGING_THREAD
	ret = paging_thread_terminate(Local(memory_object));
	if (ret != ERR_SUCCESS) {
		ERROR((Diag,
		"iop_deactivate.paging_thread_terminate() failed: %s\n",
						mach_error_string(ret)));
		mutex_unlock(&Local(lock));
		return(US_OBJECT_BUSY);
	}
#else	PAGING_THREAD
	not implemented
#endif	PAGING_THREAD

	ret = mach_port_destroy(mach_task_self(),Local(memory_object));
	if (ret != ERR_SUCCESS) {
		ERROR((Diag,
		"iop_deactivate.mach_port_destroy(memory_object) failed: %s\n",
						mach_error_string(ret)));
		mutex_unlock(&Local(lock));
		return(US_OBJECT_BUSY);
	}
	Local(memory_object) = MACH_PORT_NULL;

	mutex_unlock(&Local(lock));
	DEBUG2(pager_debug,(Diag,"iop_deactivate -> SUCCESS 2"));
	return(ERR_SUCCESS);
}


/*
 * Flush all pages beyond 'size'.
 */
mach_error_t pager_base::iop_flush(
	vm_size_t		size)
{
	mach_error_t		ret = ERR_SUCCESS;
	
	DEBUG1(pager_debug,(Diag,"iop_flush() called\n"));

	mutex_lock(&Local(lock));

	if (Local(memory_control) != MACH_PORT_NULL && 
	    Local(max_offset) > size) {
		ret = memory_object_lock_request(Local(memory_control),
				size, Local(max_offset) - size,
				FALSE,TRUE,VM_PROT_WRITE,MACH_PORT_NULL);
		if (ret != ERR_SUCCESS) 
			ERROR((Diag,
			  "iop_flush.memory_object_lock_request() failed: %s\n",
			       mach_error_string(ret)));
	}

	mutex_unlock(&Local(lock));

	pager_num_flushes++;  /* statistics */

	return(ret);
}


/*
 * Satisfy a pagein request (INTERNAL routine).
 *
 * The object must be locked throughout this call.
 */
mach_error_t pager_base::iop_satisfy_request(
	vm_offset_t		offset,
	vm_size_t		length,
	vm_prot_t		desired_access,
	boolean_t		needs_data)
{
	mach_error_t		ret = ERR_SUCCESS;
	mach_error_t		ret2;
	vm_prot_t		lock_value;
	vm_address_t		data;
	io_offset_t		start;
	vm_size_t		count, total_len;
	boolean_t		deallocate;

	if (desired_access & VM_PROT_WRITE) {
		lock_value = VM_PROT_NONE;
		Local(done_pagein_write) = TRUE;
	} else {
		lock_value = VM_PROT_WRITE;
	}
	Local(done_pagein) = TRUE;

	if (Local(min_offset) > offset) {
		Local(min_offset) = offset;
	}
	if (Local(max_offset) < (offset + length)) {
		Local(max_offset) = offset + length;
	}

	if (needs_data) {
		count = length;

		/*
                 * Read 'count' bytes.  Data is returned in 'data.'
                 * Size of data area is guaranteed to be a multiple
		 * of 'count.'
                 */

        	DEBUG1(pager_debug,(Diag,"iop_satisfy_request() calling io_pagein(). offset = %d, count = %d\n",offset,count));

                ret = io_pagein(offset,&data,&count, &deallocate);

		if (ret != ERR_SUCCESS) {
			ERROR((Diag,
				"iop_satisfy_request.io_pagein() failed: %s\n",
						mach_error_string(ret)));
			(void) vm_deallocate(mach_task_self(),data,count);
		} else {
			DEBUG1(pager_debug,(Diag,
		"iop_satisfy_request().mem_obj_data_provided: offset=%d, count=%d, dataptr=0x%x, lock_val=%d\n",offset,count,data,lock_value));
			DEBUG1(pager_debug,(Diag,
		"iop_satisfy_request().mem_obj_data_provided: port=0x%x\n",Local(memory_control)));

			ret = memory_object_data_supply(
						Local(memory_control),
						offset,data,count,deallocate,
						lock_value,FALSE,
						MACH_PORT_NULL);
			if (ret != ERR_SUCCESS) {
				ERROR((Diag,
	"iop_satisfy_request.memory_object_data_supply() failed: 0x%x, %s\n",
					     ret, mach_error_string(ret)));
			}
		}
	} else {
		DEBUG1(pager_debug,(Diag,
		"iop_satisfy_request(): locking range 0x%x 0x%x 0x%x\n",
						offset,length,lock_value));
		ret = memory_object_lock_request(Local(memory_control),
			offset,length,FALSE,FALSE,lock_value,MACH_PORT_NULL);
		if (ret != ERR_SUCCESS) {
			ERROR((Diag,
	"iop_satisfy_request.memory_object_lock_request() failed: %s\n",
						mach_error_string(ret)));
		}
	}

out:
	if (ret != ERR_SUCCESS) {
		DEBUG0(pager_debug,(Diag,
			"iop_satisfy_request(): signalling error 0x%x 0x%x\n",
							offset,length));
		ret2 = memory_object_data_error(Local(memory_control),
							offset,length,ret);
		if (ret2 != ERR_SUCCESS) {
			ERROR((Diag,
		"iop_satisfy_request.memory_object_data_error() failed: %s\n",
						mach_error_string(ret2)));
		}
	}

	return(ret);
}


/*
 * Enqueue a pagein request, to be serviced later (INTERNAL routine).
 *
 * The object must be locked throughout this call.
 */
mach_error_t pager_base::iop_enqueue_request(
	vm_offset_t		offset,
	vm_size_t		length,
	vm_prot_t		desired_access,
	boolean_t		needs_data)
{
	iop_request_t		req;

	DEBUG1(pager_debug,(Diag,"iop_enqueue_request(): 0x%x 0x%x 0x%x %s\n",
						offset,length,desired_access,
						needs_data?"data":"lock"));

	req = (iop_request_t) malloc(sizeof(struct iop_request));
	req->offset = offset;
	req->length = length;
	req->desired_access = desired_access;
	req->needs_data = needs_data;
	dll_enter(&Local(requests),req,iop_request_t,chain);

	return(ERR_SUCCESS);
}



/*
 * Memory object operations, invoked from the kernel.
 */

mach_error_t pager_base::mo_init(
	mach_port_t		memory_control,
	mach_port_t		memory_object_name,
	vm_size_t		page_size)
{
    	mach_error_t 		ret;

	DEBUG0(pager_debug,(Diag,"mo_init(): 0x%x 0x%x 0x%x\n",
				memory_control,memory_object_name,page_size));

	if (page_size != vm_page_size) {

		ERROR((Diag,"mo_init(): invalid page size: %d (expected %d)\n",
						page_size,vm_page_size));
		return(US_INVALID_ARGS);
	}

	mutex_lock(&Local(lock));

	if (Local(memory_control) != MACH_PORT_NULL) {
		ERROR((Diag,"mo_init(): duplicate call\n"));
		mutex_unlock(&Local(lock));
		return(US_INTERNAL_ERROR);
	}

	Local(memory_control) = memory_control;

	ret = memory_object_set_attributes(memory_control,
				TRUE,pager_may_cache,pager_copy_strategy);
	DEBUG1(pager_debug,
		(Diag,"mo_init.memory_object_set_attributes(): TRUE,%s,%d\n",
				pager_may_cache?"caching":"not-caching\n",
				pager_copy_strategy));
	if (ret != ERR_SUCCESS)  {
		ERROR((Diag,
			"mo_init.memory_object_set_attributes() failed: %s\n",
						mach_error_string(ret)));
	}

	Local(min_offset) = 0xffffffff;
	Local(max_offset) = 0;
	Local(done_pagein) = FALSE;
	Local(done_pagein_write) = FALSE;
	Local(state) = O_STATE_READY;

	pager_num_inits++;  /* statistics */
	
	mutex_unlock(&Local(lock));
	return(ret);
}


mach_error_t pager_base::mo_terminate(
	mach_port_t		memory_control,
	mach_port_t		memory_object_name)
{
    	mach_error_t		ret;
	iop_request_t		req;

	DEBUG0(pager_debug,(Diag,"mo_terminate(): 0x%x 0x%x\n",
					memory_control,memory_object_name));

	if (! Local(destroying)) {
	    mutex_lock(&Local(lock));
	}

	req = (iop_request_t) dll_first(&Local(requests));
	while (! dll_end(&Local(requests),(dll_entry_t)req)) {
		DEBUG1(pager_debug,(Diag,
		"mo_terminate(): cancelling request 0x%x 0x%x 0x%x %s\n",
				req->offset,req->length,req->desired_access,
				req->needs_data?"data":"lock"));
		dll_remove(&Local(requests),req,iop_request_t,chain);
		free(req);
		req = (iop_request_t) dll_first(&Local(requests));
	}

	ret = mach_port_destroy(mach_task_self(),memory_control);
	if (ret != ERR_SUCCESS) {
		ERROR((Diag,
		"mo_terminate.mach_port_destroy(memory_control) failed: %s\n",
						mach_error_string(ret)));
	}

	ret = mach_port_destroy(mach_task_self(),memory_object_name);
	if (ret != ERR_SUCCESS) {
		ERROR((Diag,
		"mo_terminate.mach_port_destroy(memory_object_name) failed: %s\n",
						mach_error_string(ret)));
	}

	Local(memory_control) = MACH_PORT_NULL;
	Local(done_pagein) = FALSE;
	Local(done_pagein_write) = FALSE;

	if (Local(state) == O_STATE_RECLAIMING) {
		Local(state) = O_STATE_FROZEN;
		mutex_unlock(&Local(lock));
		condition_signal(&Local(cond));
	} 
	else if (! Local(destroying)) {
		mutex_unlock(&Local(lock));
	}

	pager_num_terminates++;  /* statistics */

	return(ERR_SUCCESS);
}


mach_error_t pager_base::mo_data_request(
	mach_port_t		memory_control,
	vm_offset_t		offset,
	vm_size_t		length,
	vm_prot_t		desired_access)
{
	mach_error_t		ret;

	DEBUG1(pager_debug,(Diag,"mo_data_request(): 0x%x 0x%x 0x%x 0x%x\n",
				memory_control,offset,length,desired_access));

	VERIFY(memory_control,offset,length);

	mutex_lock(&Local(lock));

	if (Local(state) == O_STATE_READY) {
		ret = iop_satisfy_request(offset,length,desired_access,TRUE);
	} else {
		ret = iop_enqueue_request(offset,length,desired_access,TRUE);
	}

	mutex_unlock(&Local(lock));

	pager_num_data_requests++;  /* statistics */

	return(ret);
}


mach_error_t pager_base::mo_data_unlock(
	mach_port_t		memory_control,
	vm_offset_t		offset,
	vm_size_t		length,
	vm_prot_t		desired_access)
{
	mach_error_t		ret;

	DEBUG1(pager_debug,(Diag,"mo_data_unlock(): 0x%x 0x%x 0x%x 0x%x\n",
				memory_control,offset,length,desired_access));

	VERIFY(memory_control,offset,length);

	mutex_lock(&Local(lock));

	if (Local(state) == O_STATE_READY) {
		ret = iop_satisfy_request(offset,length,desired_access,FALSE);
	} else {
		ret = iop_enqueue_request(offset,length,desired_access,FALSE);
	}

	mutex_unlock(&Local(lock));
	return(ret);
}


mach_error_t pager_base::mo_data_write(
	mach_port_t		memory_control,
	vm_offset_t		offset,
	vm_address_t		data,
	vm_size_t		length)
{
	mach_error_t		ret;
	io_offset_t		start;
	vm_size_t		count;

	DEBUG1(pager_debug,(Diag,"mo_data_write(): 0x%x 0x%x 0x%x 0x%x\n",
					memory_control,offset,data,length));

	/* If the we are dieing, just toss the memory and return */
	if (Local(destroying)) {
		ret = vm_deallocate(mach_task_self(),data,length);
		return(ret);
	}

	VERIFY(memory_control,offset,length);

	mutex_lock(&Local(lock));

	if(Local(state) == O_STATE_FROZEN) {
		mutex_unlock(&Local(lock));
		ERROR((Diag,"mo_data_write() invalid in frozen state\n"));
		return(US_INTERNAL_ERROR);
	}

	/*
	 * Note that the data is "consumed" by io_pageout meaning this
	 * routine doesn't need to vm_deallocate it.
	 */
	count = length;
	ret = io_pageout(offset,data,&count);
	if (ret != ERR_SUCCESS) {
		ERROR((Diag,"mo_data_write.io_pageout() failed: %s\n",
						mach_error_string(ret)));
	}

	Local(done_pageout) = TRUE;

	mutex_unlock(&Local(lock));

	pager_num_data_writes++;  /* statistics */

	return(ret);
}


mach_error_t pager_base::mo_lock_completed(
	mach_port_t		memory_control,
	vm_offset_t		offset,
	vm_size_t		length)
{
	DEBUG1(pager_debug,(Diag,"mo_lock_completed(): 0x%x 0x%x 0x%x\n",
					memory_control,offset,length));

	VERIFY(memory_control,offset,length);

	mutex_lock(&Local(lock));

	if (Local(state) != O_STATE_RECLAIMING) {
		mutex_unlock(&Local(lock));
		ERROR((Diag,
			"mo_lock_completed() not in reclaiming state: %d\n",
							Local(state)));
		return(US_INTERNAL_ERROR);
	}

	Local(state) = O_STATE_FROZEN;

	mutex_unlock(&Local(lock));
	condition_signal(&Local(cond));

	return(ERR_SUCCESS);
}


mach_error_t pager_base::mo_copy(
	mach_port_t		old_memory_control,
	vm_offset_t		offset,
	vm_size_t		length,
	memory_object_t		new_memory_object)
{
	ERROR((Diag,"memory_object_copy() called\n"));
	return(US_INTERNAL_ERROR);
}


mach_error_t pager_base::mo_create()
{
	ERROR((Diag,"memory_object_create() called\n"));
	return(US_INTERNAL_ERROR);
}


mach_error_t pager_base::mo_supply_completed(
	memory_object_control_t	memory_control,
	vm_offset_t		offset,
	vm_size_t		length,
	kern_return_t		result,
	vm_offset_t		error_offset)
{
	ERROR((Diag,"memory_object_supply_completed() called\n"));
	return(US_INTERNAL_ERROR);
}


mach_error_t pager_base::mo_data_return(
	memory_object_control_t	memory_control,
	vm_offset_t		offset,
	pointer_t		data,
	boolean_t		dirty,
	boolean_t		kernel_copy)
{
	ERROR((Diag,"memory_object_data_return() called\n"));
	return(US_INTERNAL_ERROR);
}


mach_error_t pager_base::mo_change_completed(
	boolean_t			may_cache,
	memory_object_copy_strategy_t	copy_strategy)
{
	ERROR((Diag,"memory_object_change_completed() called\n"));
	return(US_INTERNAL_ERROR);
}



#if	PAGING_THREAD
/*
 * Glue code to connect to the kernel memory object interface.
 */

int		 mo_debug;

#define	MO_DEBUG(cmd)	\
	if (mo_debug) cmd;

typedef struct {
	mach_msg_header_t	head;
	char		body[8192-sizeof(mach_msg_header_t)];
} msg_t;


extern "C" boolean_t	memory_object_server();
extern hash_table_t 	port_to_paging_obj;

struct mutex		port_to_paging_obj_lock;

#define	PAGING_OBJ_LOOKUP(obj, port, str) 				\
	{								\
		mutex_lock(&port_to_paging_obj_lock);			\
		obj = (pager_base*) hash_lookup(port_to_paging_obj,	\
							memory_object);	\
		if (obj == NULL) {					\
	    		MO_DEBUG(printf("%s lookup error on port 0x%x\n",\
							memory_object));\
			mutex_unlock(&port_to_paging_obj_lock);		\
	    		return(KERN_FAILURE);				\
		}							\
		MO_DEBUG(printf("mem_obj: invoke %s on obj 0x%x\n",str,obj));\
/*		mach_object_reference(obj);	*/			\
		mutex_unlock(&port_to_paging_obj_lock);			\
	}

#define	PAGING_OBJ_DONE(obj)						\
/*		mach_object_dereference(obj); */

/*
 * Handler routines for all paging requests.
 */

extern "C" mach_error_t
memory_object_init(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	memory_object_name_t memory_object_name,
	vm_size_t page_size)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base*	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object, "memory_object_init");
	
	ret = obj->mo_init( memory_control, memory_object_name, page_size);

	PAGING_OBJ_DONE(obj);

	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"init",
			mach_error_string(ret),
			ret));
			
	
	return(ret);
}


extern "C" mach_error_t
memory_object_copy(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	vm_offset_t offset,
	vm_size_t length,
	memory_object_t nmo)
{
    	mach_error_t 	ret = KERN_SUCCESS;

	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object, "memory_object_copy");
	
	ret = obj->mo_copy( memory_control, offset, length, nmo);

	PAGING_OBJ_DONE(obj);

	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"init",
			mach_error_string(ret),
			ret));
	
	printf("memory_object_copy called!\n");
	fflush(stdout);
	
	return(ret);
}


#ifdef notdef
/*
 * The kernel is not supposed to call this for non-default pagers.
 */
extern "C" mach_error_t
pager_create(opo, npo, nrp, nn, nps)
{
    nyi("io_pager/pager_create called");
    return(KERN_FAILURE);
}
#endif	notdef

extern "C" mach_error_t
memory_object_data_request(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	vm_offset_t	offset,
	vm_size_t	length,
	vm_prot_t	access)
{
	mach_error_t	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object, "memory_object_request");

	ret = obj->mo_data_request( memory_control, offset, length, access);

	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"data_request",
			mach_error_string(ret),
			ret));

	return(ret);
}

extern "C" mach_error_t
memory_object_data_unlock(
	memory_object_t memory_object,
	memory_object_control_t pager_request_port,
	vm_offset_t	offset,
	vm_size_t	length,
	vm_prot_t	access)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object, "memory_object_data_unlock");
	
	ret = obj->mo_data_unlock( pager_request_port, offset, length, access);

	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"data_unlock",
			mach_error_string(ret),
			ret));

	return(ret);
}

extern "C" mach_error_t
memory_object_data_write(
	memory_object_t memory_object,
	memory_object_control_t	pager_request_port,
	vm_offset_t	offset,
	vm_address_t	data,
	vm_size_t	length)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object, "memory_object_data_write");
	
	ret = obj->mo_data_write( pager_request_port, offset, data, length);

	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"data_write",
			mach_error_string(ret),
			ret));
	return(ret);	
}

extern "C" mach_error_t
memory_object_lock_completed(
	memory_object_t memory_object,
	memory_object_control_t pager_request_port,
	vm_offset_t	offset,
	vm_size_t	length)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object, "memory_object_lock_completed");

	ret = obj->mo_lock_completed( pager_request_port, offset, length);

	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"lock_completed",
			mach_error_string(ret),
			ret));
	
	return(ret);	
}

extern "C" mach_error_t
memory_object_supply_completed(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	vm_offset_t	offset,
	vm_size_t	length,
	kern_return_t	result,
	vm_offset_t	error_offset)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object,
					"memory_object_supply_completed");

	ret = obj->mo_supply_completed( memory_control,
					offset, length, result, error_offset);

	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"supply_completed",
			mach_error_string(ret),
			ret));
	
	return(ret);	
}

extern "C" mach_error_t
memory_object_data_return(
	memory_object_t memory_object,
	memory_object_control_t memory_control,
	vm_offset_t	offset,
	pointer_t	data,
	boolean_t	dirty,
	boolean_t	kernel_copy)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object,
					"memory_object_data_return");

	ret = obj->mo_data_return( memory_control,
					offset, data, dirty, kernel_copy);

	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"data_return",
			mach_error_string(ret),
			ret));
	
	return(ret);	
}

extern "C" mach_error_t
memory_object_change_completed(
	memory_object_t memory_object,
	boolean_t may_cache,
	memory_object_copy_strategy_t copy_strategy)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object,
					"memory_object_change_completed");

	ret = obj->mo_change_completed(may_cache,copy_strategy);
	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"change_completed",
			mach_error_string(ret),
			ret));
	
	return(ret);
}

extern "C" mach_error_t
memory_object_terminate(
	memory_object_t memory_object,
	memory_object_control_t mc,
	memory_object_name_t mn)
{
    	mach_error_t 	ret = KERN_SUCCESS;
	pager_base* 	obj;

	PAGING_OBJ_LOOKUP(obj, memory_object, "memory_object_terminate");

	ret = obj->mo_terminate( mc, mn);
	
	PAGING_OBJ_DONE(obj);
	
	if (ret != ERR_SUCCESS)
		MO_DEBUG(printf("memory_object_%s error %s (0x%x)\n",
			"termiate",
			mach_error_string(ret),
			ret));
	return(ret);	    
}

cthread_t	paging_thread = NULL;
extern "C" int	handle_paging();
mach_port_t	paging_set = MACH_PORT_NULL;
hash_table_t	port_to_paging_obj;


mach_error_t paging_thread_init()
{
    	int i;
	mach_error_t 	err;
	if (paging_set == MACH_PORT_NULL) {
		err = mach_port_allocate(mach_task_self(), 
			MACH_PORT_RIGHT_PORT_SET, &paging_set);
		mutex_init(&port_to_paging_obj_lock);
	}

	
	if (paging_thread == NULL)
			paging_thread = cthread_fork((cthread_fn_t)handle_paging, 0);
	
	return(err);    
}

mach_error_t paging_thread_export(
	mach_port_t	port,
	pager_base* 	obj)
{
    	mach_error_t err;
	err = mach_port_move_member(mach_task_self(), port, paging_set);
	if (err)
		MO_DEBUG(printf("paging_thread_export: port_set_add %s [0x%x]\n", mach_error_string(err), err));

	if (port_to_paging_obj == (hash_table_t) 0)
		port_to_paging_obj = hash_init(0,0,64);

	mutex_lock(&port_to_paging_obj_lock);
	hash_enter(port_to_paging_obj, port, obj);
	mutex_unlock(&port_to_paging_obj_lock);
	
//	mach_object_reference(obj);

	return(err);
}

mach_error_t paging_thread_terminate(
	mach_port_t		port)
{
	pager_base*	obj;

	mutex_lock(&port_to_paging_obj_lock);
	obj = (pager_base*) hash_lookup(port_to_paging_obj, port);
	mutex_unlock(&port_to_paging_obj_lock);
	
	if (obj == NULL)
		MO_DEBUG(printf("paging_thread_terminate: uh oh!\n"));

	mutex_lock(&port_to_paging_obj_lock);		
	hash_remove(port_to_paging_obj,port);
	mutex_unlock(&port_to_paging_obj_lock);
	
//	mach_object_dereference(obj);

	return(ERR_SUCCESS);
}    

/*
 *	request_handler: take incoming requests and dispatch them through
 *	the user-provided dispatch routine
 */

extern "C" handle_paging()
{
	mach_error_t		err;
	boolean_t		replying;
	msg_t			msg1, msg2;
	msg_t			*in_msg, *tmp_msg;

	typedef struct {
		mach_msg_header_t head;
		mach_msg_type_t RetCodeType;
		kern_return_t RetCode;
	} Reply;

	Reply 			*out_msg;

	/* Setup for first request */
	in_msg = &msg1;
	out_msg = ((Reply*)(&msg2));
	bzero(&msg1, sizeof(msg_t));
	bzero(&msg2, sizeof(msg_t));
	replying = FALSE;

	/* Go into a send/receive loop */
	for (;;) {
		if (replying) {
			out_msg->head.msgh_local_port = MACH_PORT_NULL;
			out_msg->head.msgh_bits = in_msg->head.msgh_bits;
			out_msg->head.msgh_size = sizeof(Reply);

			if (memory_object_server( in_msg, out_msg)) {
				if ((out_msg->head.msgh_remote_port == MACH_PORT_NULL)
				    || (out_msg->RetCode == MIG_NO_REPLY)) {
					replying = FALSE;
				}
			} else {
			    	MO_DEBUG(printf("handle_paging: bad msg id 0x%x\n", in_msg->head.msgh_id));
				replying = FALSE;
			}
		}
		
		if (replying) {
			err = mach_msg((mach_msg_header_t*)out_msg, 
					MACH_RCV_MSG|MACH_SEND_MSG,
					sizeof(Reply), sizeof(msg_t),
					paging_set,
					MACH_MSG_TIMEOUT_NONE,
					MACH_PORT_NULL);

			tmp_msg = (msg_t *)out_msg;
			out_msg = (Reply *)in_msg;
			in_msg = tmp_msg;
		} else {
			in_msg->head.msgh_local_port = paging_set;
			in_msg->head.msgh_size = sizeof(msg_t);
			in_msg->head.msgh_bits = MACH_MSGH_BITS_ZERO;
			err = mach_msg_receive(in_msg);
		}

		/* Belch on non-send error */
		replying = TRUE;
		if ((MACH_MSG_SUCCESS != err) &&
		    ((err_get_system(err) != err_mach_ipc) ||
		    (err_get_sub(err) != 0))) {
			mach_error("paging object", err);
			replying = FALSE;
		}
	
	}
}



#endif	PAGING_THREAD
