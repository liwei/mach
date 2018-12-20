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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxsignal_ifc.h,v $
 *
 * Purpose: BSD style User signals
 *
 * HISTORY: 
 * $Log:	uxsignal_ifc.h,v $
 * Revision 2.9  94/07/08  16:02:24  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.8  94/05/17  14:08:53  jms
 * 	Need dummy implementations of virtual methods in class 
 * 		uxsignal for 2.3.3 g++ -modh
 * 	[94/04/28  19:01:05  jms]
 * 
 * Revision 2.7.1.1  94/02/18  11:34:25  modh
 * 	Need to declare virtual funtions in uxsignal for 2.3.3 g++
 * 
 * Revision 2.7  92/07/05  23:33:00  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	Added a few include files for good luck with the compiler...
 * 	[92/06/24  17:32:47  dpj]
 * 
 * Revision 2.6  91/11/06  14:12:59  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:21:09  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:11:58  pjg]
 * 
 * Revision 2.5  91/07/01  14:13:21  jms
 * 	Remove old (via TM) self signal routines
 * 	Add event_post_with_timeout
 * 	[91/06/25  11:38:50  jms]
 * 
 * Revision 2.4  91/05/05  19:29:07  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:41  dpj]
 * 
 * 	Fixed broken cloning logic (from Rich Draves @ CMU).
 * 	[91/04/28  10:35:03  dpj]
 * 
 * Revision 2.3  90/07/09  17:11:37  dorr
 * 	move exception handlers into cthreads.
 * 	[90/03/22  13:59:40  dorr]
 * 
 * 	add event handler stacks.
 * 	[90/03/01  15:39:02  dorr]
 * 
 * 	add signal_return, signal_pause, signal_wait.
 * 	clean up staack switch. get rid of alerts.
 * 	[90/02/23  14:56:00  dorr]
 * 
 * 	move mask, handler and flags into unix sigvec.
 * 	execption_post -> event_post.  add signal_set_vec,
 * 	signal_set_stack, signal_pause and signal_wait.
 * 	[90/01/11  11:46:18  dorr]
 * 	Add child information and self sync. fields/methods to the object
 * 	[90/07/09  10:44:18  jms]
 * 
 * Revision 2.2  90/01/02  22:17:24  dorr
 * 	first crack at signals.
 * 
 * Revision 2.1.1.3  90/01/02  14:22:12  dorr
 * 	signal_post -> exception_post
 * 
 * Revision 2.1.1.2  89/12/19  17:14:10  dorr
 * 	initial checkin
 * 
 * Revision 2.1.1.1  89/08/10  16:13:23  dorr
 * 	created.
 * 
 */

#ifndef	_uxsignal_ifc_h
#define	_uxsignal_ifc_h

extern "C" {
#include <base.h>
#include <signal.h>
#include <dll.h>
}

#include <method_info_ifc.h>
#include <top_ifc.h>
#include <us_event_ifc.h>
#include <clone_ifc.h>

typedef	int	signal_t;		/* signals are small ints */
typedef	int	signal_set_t;		/* their range fits into an int */

#define	SIG_STATE_PAUSED	0x1
#define	SIG_STATE_USE_OLD_MASK	0x2
#define	SIG_STATE_ON_STACK	0x4
typedef	int  sig_state_t;

/*
 * XXX have to split out per-thread signals
 */

#ifdef	GXXBUG_CLONING2
class uxsignal: public usEvent {
#else	GXXBUG_CLONING2
class uxsignal: public usEvent, public usClone {
#endif	GXXBUG_CLONING2
	struct mutex		lock;

	signal_set_t		pending;	/* currently pending signals */
	signal_set_t		blocked;	/* currently blocked signals */
	signal_set_t		prev_blocked;	/* previous set of blocked signals */
	sig_state_t		state;
	condition_t		pause;			/* wait for a signal */

	struct sigvec		vec[NSIG+1];
	int			ux_code[NSIG+1];	/* pending code */
	int			tm_post_ids[NSIG+1];
	vm_address_t		sigstack;		/* signal stack */

	thread_t		primary_thread;

	struct mutex		child_info_lock;
	int			child_count;
	dll_head_t		child_info_list;
	struct condition	child_info_arrived;

      public:
	uxsignal();
	virtual ~uxsignal();
	DECLARE_MEMBERS(uxsignal);
	virtual char* remote_class_name() const;

	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_abort(mach_port_t);
	virtual mach_error_t clone_complete();

REMOTE	virtual mach_error_t event_post(thread_t, mach_error_t, int, int);
REMOTE	virtual mach_error_t event_post_with_timeout(thread_t, mach_error_t, int, int, int);

	mach_error_t signal_register(thread_t);
	mach_error_t signal_reset_locked(thread_t);
	mach_error_t signal_reset(thread_t);
	boolean_t    xlate_exception(int, int, int, int*, int*);
	mach_error_t deliver(thread_t, boolean_t);
	mach_error_t signal_return(int, int);
	mach_error_t signal_set_vec(int, struct sigvec*, struct sigvec*);
	mach_error_t signal_set_handler(int, mach_error_fn_t, 
					 mach_error_fn_t*);
	mach_error_t signal_block(signal_set_t, signal_set_t*);
	mach_error_t signal_set_mask(signal_set_t, signal_set_t*);
	mach_error_t signal_get_mask(signal_set_t*);
	mach_error_t signal_set_stack(struct sigstack*, struct sigstack*);
	int          signal_pause(signal_set_t);
	int          signal_wait();
	mach_error_t get_child_info(int*, int*, boolean_t);

      private:
	void child_info_init();
	void child_info_post_fork();
	void set_child_info(int, int);
	
	int nxt_bit(int);
	mach_error_t signal_post(thread_t, int, int, boolean_t);
	mach_error_t event_post_internal(thread_t, mach_error_t, int,
					 int, int);

      public:
	virtual mach_error_t _notdef();

        virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
        virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
        virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
        virtual mach_error_t ns_set_times(time_value_t, time_value_t);
        virtual mach_error_t ns_get_protection(ns_prot_t, int*);
        virtual mach_error_t ns_set_protection(ns_prot_t, int);
        virtual mach_error_t ns_get_privileged_id(int*);
        virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);
        virtual mach_error_t ns_get_manager(ns_access_t, usItem **);

};
#endif	_uxsignal_ifc_h
