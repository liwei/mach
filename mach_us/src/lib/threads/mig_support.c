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
 * mig_support.c
 *
 * File us/lib/threads/mig_support.c
 *
 * Routines to set and deallocate the mig reply port for the current thread.
 * Called from mig-generated interfaces.
 *
 * HISTORY:
 * $Log:	mig_support.c,v $
 * Revision 1.7  94/07/08  14:14:39  mrt
 * 	Updated copyright
 * 
 * Revision 1.6  93/01/20  17:37:34  jms
 * 	Fix reply_port_lock initialization for forking
 * 	[93/01/18  16:23:51  jms]
 * 
 * Revision 1.5  92/03/05  15:05:12  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:27:28  jms]
 * 
 * Revision 1.4  90/07/09  17:06:02  dorr
 * 	No Further Changes
 * 	[90/07/06  17:10:49  jms]
 * 
 * Revision 1.4  89/06/08  18:28:26  mrt
 * 	Changed task_data() to thread_reply()
 * 	[89/06/08            mrt]
 * 
 * Revision 1.3  90/01/02  22:07:01  dorr
 * 	mmmm?
 * 
 * Revision 1.3  89/05/05  18:54:23  mrt
 * 	Cleanup for Mach 2.5
 * 
 * Revision 1.2.1.1  89/12/19  17:07:13  dorr
 * 	checkin before christmas
 * 
 * 24-Mar-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Made MTASK version work correctly again.
 *
 * 27-Aug-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Changed mig_support.c to avoid deadlock that can occur
 *	if tracing is turned on	during calls to mig_get_reply_port().
 *
 * 10-Aug-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Changed mig_support.c to use MACH_CALL.
 *	Changed "is_init" to "multithreaded" and reversed its sense.
 *
 * 30-Jul-87  Mary Thompson (mrt) at Carnegie Mellon University
 *	Created.
 */


#include <mach.h>
#include <cthreads.h>
#include "cthread_internals.h"

#if	MTASK
#undef	mach_task_self  /* Must call the function since the variable is shared */
#endif	MTASK

private struct mutex reply_port_lock = MUTEX_INITIALIZER;
private int multithreaded = 0;

/*
 * Called by mach_init with 0 before cthread_init is
 * called and again with 1 at the end of cthread_init.
 */
void
mig_init(init_done)
	int init_done;
{
	multithreaded = init_done;
	if (init_done) {
		mutex_init(&reply_port_lock);
	}
}

/*
 * Called by mig interface code whenever a reply port is needed.
 * Tracing is masked during this call; otherwise, a call to printf()
 * can result in a call to malloc() which eventually reenters
 * mig_get_reply_port() and deadlocks.
 */
mach_port_t
mig_get_reply_port()
{
	register cproc_t self;
	register kern_return_t r;
	mach_port_t port;
#ifdef	DEBUG
	int d = cthread_debug;
#endif	DEBUG

	if (! multithreaded)
		return mach_reply_port();
#ifdef	DEBUG
	cthread_debug = FALSE;
#endif	DEBUG
	self = cproc_self();
	if (self->reply_port == MACH_PORT_NULL) {
		mutex_lock(&reply_port_lock);
		self->reply_port = mach_reply_port();
		MACH_CALL(mach_port_allocate(mach_task_self(), 
				MACH_PORT_RIGHT_RECEIVE, &port), r);
		self->reply_port = port;
		mutex_unlock(&reply_port_lock);
	}
#ifdef	DEBUG
	cthread_debug = d;
#endif	DEBUG
	return self->reply_port;
}

/*
 * Called by mig interface code after a timeout on the reply port.
 * May also be called by user.
 */
void
mig_dealloc_reply_port()
{
	register cproc_t self;
	register mach_port_t port;
#ifdef	DEBUG
	int d = cthread_debug;
#endif	DEBUG

	if (! multithreaded)
		return;
#ifdef	DEBUG
	cthread_debug = FALSE;
#endif	DEBUG
	self = cproc_self();
	ASSERT(self != NO_CPROC);
	port = self->reply_port;
	if (port != MACH_PORT_NULL && port != mach_reply_port()) {
		mutex_lock(&reply_port_lock);
		self->reply_port = mach_reply_port();
		(void) mach_port_destroy(mach_task_self(), port);
		self->reply_port = MACH_PORT_NULL;
		mutex_unlock(&reply_port_lock);
	}
#ifdef	DEBUG
	cthread_debug = d;
#endif	DEBUG
}
