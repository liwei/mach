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
 * HISTORY
 * $Log:	cthread_internals.h,v $
 * Revision 1.7  94/07/08  14:14:27  mrt
 * 	Updated copyright
 * 
 * Revision 1.6  94/05/17  14:06:33  jms
 * 	Make cproc state volatile
 * 	[94/04/28  18:23:52  jms]
 * 
 * Revision 1.5  92/03/05  15:03:39  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:20:54  jms]
 * 
 * Revision 1.4  91/07/01  14:10:54  jms
 * 	No Further Changes
 * 	[91/06/24  17:08:03  jms]
 * 
 * 	Add inclusion guards
 * 	[91/04/15  17:48:46  jms]
 * 
 * Revision 1.3  90/07/09  17:05:29  dorr
 * 	No Further Changes
 * 	[90/07/06  16:48:19  jms]
 * 
 * Revision 1.5  89/06/21  13:03:01  mbj
 * 	Removed the old (! IPC_WAIT) form of condition waiting/signalling.
 * 
 * Revision 1.4  89/05/19  13:02:58  mbj
 * 	Add cproc flags.
 * 
 * Revision 1.3  89/05/05  18:48:17  mrt
 * 	Cleanup for Mach 2.5
 * 
 * 24-Mar-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Implement fork() for multi-threaded programs.
 *	Made MTASK version work correctly again.
 */
/*
 * cthread_internals.h - by Eric Cooper
 *
 * File us/lib/threads/cthread_internals.h
 *
 *
 * Private definitions for the C Threads implementation.
 *
 * The cproc structure is used for different implementations
 * of the basic schedulable units that execute cthreads.
 *
 * The cproc implementation is determined by defining exactly
 * one of the following options:
 *
 *	MTHREAD		MACH threads; single address space,
 *			kernel-mode preemptive scheduling
 *
 *	COROUTINE	coroutines; single address space,
 *			user-mode non-preemptive scheduling
 *
 *	MTASK		MACH tasks; multiple address spaces,
 *			shared memory for global data,
 *			kernel-mode preemptive scheduling
 */
/*
 * HISTORY:
 * 24-Mar-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Implement fork() for multi-threaded programs.
 *	Made MTASK version work correctly again.
 */


#ifndef _CTHREAD_INTERNALS_
#define _CTHREAD_INTERNALS_ 1
#include "options.h"

/*
 * Low-level thread implementation.
 * This structure must agree with struct ur_cthread in cthreads.h
 */
typedef struct cproc {
	struct cproc *next;		/* for lock, condition, and ready queues */
	cthread_t incarnation;		/* for cthread_self() */
	volatile int state;
	mach_port_t reply_port;		/* for mig_get_reply_port() */

#if	COROUTINE
	int context;
#endif	COROUTINE

#if	MTHREAD
	mach_port_t wait_port;
#endif	MTHREAD

#if	MTHREAD || MTASK
	int id;
#endif	MTHREAD || MTASK
	struct cproc *link;		/* for finding cproc_self() when MTASK;
					   also so all cprocs can be found
					   after a fork() */
#if	MTHREAD || COROUTINE
	int flags;
#endif	MTHREAD || COROUTINE

	unsigned int stack_base;
	unsigned int stack_size;

} *cproc_t;

#define	NO_CPROC		((cproc_t) 0)
#define	cproc_self()		((cproc_t) ur_cthread_self())

/*
 * Possible cproc states.
 */
#define	CPROC_RUNNING		0
#define	CPROC_SPINNING		1
#define	CPROC_BLOCKED		2

#if	MTHREAD || COROUTINE
/*
 * The cproc flag bits.
 */
#define CPROC_INITIAL_STACK	0x1

#endif	MTHREAD || COROUTINE

/*
 * C Threads imports:
 */
extern char *malloc();

/*
 * Mach imports:
 */
extern void mach_error();

/*
 * C library imports:
 */
extern exit();

/*
 * Macro for MACH kernel calls.
 */
#define	MACH_CALL(expr, ret)	if (((ret) = (expr)) != KERN_SUCCESS) { \
					mach_error("expr", (ret)); \
					ASSERT(SHOULDNT_HAPPEN); \
					exit(1); \
				} else

/*
 * Debugging support.
 */
#ifdef	DEBUG

#define	private
#define	TRACE(x)	if (cthread_debug) x ; else
extern int cthread_debug;

/*
 * C library imports:
 */
extern printf(), fprintf(), abort();

#else	DEBUG

#define	private static
#define	TRACE(x)

#endif	DEBUG
#endif _CTHREAD_INTERNALS_
