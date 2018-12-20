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
 * HISTORY
 * $Log:	pager_base_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:03  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  93/01/20  17:38:09  jms
 * 	Add "pager_may_cache" args.
 * 	Add count to io_page{in,out}
 * 	Add deallocate flag to io_pagein.  Should the user deallocate the mem?
 * 	[93/01/18  17:01:28  jms]
 * 
 * Revision 2.2  92/07/05  23:28:20  dpj
 * 	First working version.
 * 	[92/06/24  17:00:37  dpj]
 * 
 * Revision 1.12  92/03/05  15:06:57  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:43:37  jms]
 * 
 * Revision 1.11  91/11/13  17:18:52  dpj
 * 	No changes.
 * 	[91/11/12  17:56:02  dpj]
 * 
 * Revision 1.10  91/10/06  22:30:15  jjc
 * 	Added memory_object_change_completed().
 * 	Picked up from Dan Julin:
 * 	Added memory_object_supply_completed() and memory_object_data_return().
 * 	[91/10/03            jjc]
 * 
 * Revision 1.9  91/07/01  14:12:29  jms
 * 	New iop_flush method.
 * 	[91/06/05  13:58:18  roy]
 * 
 * Revision 1.8  90/12/21  13:54:11  jms
 * 	Add a new state to paging objects.  Eventually, use of
 * 	no-more-senders will obviate this functionality.
 * 	[90/12/19  11:56:42  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:20:41  jms]
 * 
 * Revision 1.7  89/11/28  19:12:03  dpj
 * 	Brand-new implementation.
 * 	[89/11/20  20:56:01  dpj]
 * 
 */
#ifndef	_pager_base_ifc_h
#define	_pager_base_ifc_h


extern "C" {
#include	<mach.h>
#include	<cthreads.h>
#include	<base.h>
#include	<dll.h>
#include	<io_types.h>
}


/*
 *  The object is normally in READY state.
 *  After a clean or a flush, it moves to RECLAIMING
 *  until all pages have been flushed or cleaned,
 *  at which point it moves to FROZEN.
 *  When in FROZEN state, an object may be resumed,
 *  in which case it moves back to READY.
 *
 *  In either of these states, the pager may be active
 *  or inactive, depending on the value of memory_control.
 *
 *  A pager is put into the O_STATE_EXPECT_INIT state when
 *  pager_iop_get_port() is called.  This is our cue that 
 *  a mo_init() message is expected (because the client is
 *  mapping the file), and hence prevents iop_deactivate() from 
 *  succeeding.  This was needed to fix the race condition 
 *  whereby iop_deactivate() could succeed (it is called when 
 *  there are no active agents for the file) even though
 *  a mo_init() message is on the way.  This scenario can
 *  happen because vm_map() does not wait for mo_init() to
 *  complete.
 * 
 *  Unfortunately, sometimes iop_get_port() is called and a
 *  subsequent mo_init() never comes in so we end up with 
 *  paging objects that can never go away...
 *
 *  The eventual solution here is to either make vm_map() wait for
 *  mo_init() to complete, or use no-more-senders notification
 *  on the pager port to indicate when iop_deactivate() on the
 *  corresponding paging object should succeed.
 *
 */
#define	O_STATE_READY		0
#define	O_STATE_RECLAIMING	1
#define	O_STATE_FROZEN		2
#define O_STATE_EXPECT_INIT	3

class pager_base {
	boolean_t		started;
	struct mutex		lock;
	int			state;
	mach_port_t		memory_object;
	mach_port_t		memory_control;
	vm_offset_t		min_offset;
	vm_offset_t		max_offset;
	dll_head_t		requests;
	struct condition	cond;
	boolean_t		done_pagein;
	boolean_t		done_pagein_write;
	boolean_t		done_pageout;
	boolean_t		pager_may_cache;
	boolean_t		destroying;

	/*
	 * Internal methods.
	 */
	mach_error_t		iop_satisfy_request(
					vm_offset_t,
					vm_size_t,
					vm_prot_t,
					boolean_t);
	mach_error_t		iop_enqueue_request(
					vm_offset_t,
					vm_size_t,
					vm_prot_t,
					boolean_t);

      public:
				pager_base();
	virtual			~pager_base();

	mach_error_t		pager_base_start(boolean_t);

	/*
	 * Virtual functions to be provided by the derived class,
	 * representing the backing store.
	 */
	virtual mach_error_t	io_pagein(
					vm_offset_t,
					vm_address_t*,
					vm_size_t*,
					boolean_t*) =0;
	virtual mach_error_t	io_pageout(
					vm_offset_t,
					vm_address_t,
					vm_size_t*) =0;
	virtual mach_error_t	io_get_size(io_size_t*) =0;
	virtual mach_error_t	io_set_size(io_size_t) =0;

	/*
	 * Methods for external control.
	 */
	mach_error_t		iop_get_port(mach_port_t*);
	mach_error_t		iop_clean(boolean_t*);
	mach_error_t		iop_resume();
	mach_error_t		iop_deactivate(boolean_t,boolean_t*);
	mach_error_t		iop_flush(vm_size_t);

	/*
	 * Methods called only from the kernel, through Mig.
	 *
	 * They must be public to be accessible from Mig and C.
	 */
	mach_error_t		mo_init(
					mach_port_t,
					mach_port_t,
					vm_size_t);
	mach_error_t		mo_terminate(
					mach_port_t,
					mach_port_t);
	mach_error_t		mo_data_request(
					mach_port_t,
					vm_offset_t,
					vm_size_t,
					vm_prot_t);
	mach_error_t		mo_data_unlock(
					mach_port_t,
					vm_offset_t,
					vm_size_t,
					vm_prot_t);
	mach_error_t		mo_data_write(
					mach_port_t,
					vm_offset_t,
					vm_address_t,
					vm_size_t);
	mach_error_t		mo_lock_completed(
					mach_port_t,
					vm_offset_t,
					vm_size_t);
	mach_error_t		mo_copy(
					mach_port_t,
					vm_offset_t,
					vm_size_t,
					memory_object_t);
	mach_error_t		mo_create();
	mach_error_t		mo_supply_completed(
					memory_object_control_t,
					vm_offset_t,
					vm_size_t,
					kern_return_t,
					vm_offset_t);
	mach_error_t		mo_data_return(
					memory_object_control_t,
					vm_offset_t,
					pointer_t,
					boolean_t,
					boolean_t);
	mach_error_t		mo_change_completed(
					boolean_t,
					memory_object_copy_strategy_t);

};


#endif	_pager_base_ifc_h

