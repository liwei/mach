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
 * options.h
 *
 * File: us/lib/threads/options.h
 *
 * This file normalizes the option values
 * to 1 for the one in use, 0 for the others.
 * If none of the options is defined,
 * it will cause a compile-time error.
 */
/*
 * HISTORY
 * $Log:	options.h,v $
 * Revision 1.3  94/07/08  14:14:40  mrt
 * 	Updated copyright
 * 
 * Revision 1.2  90/07/09  17:06:06  dorr
 * 	No Further Changes
 * 	[90/07/06  17:11:43  jms]
 * 
 * Revision 1.3  89/06/21  13:03:21  mbj
 * 	Removed the old (! IPC_WAIT) form of condition waiting/signalling.
 * 
 * Revision 1.2  89/05/05  18:59:43  mrt
 * 	Cleanup for Mach 2.5
 * 
 */


#if defined(MTHREAD)
#	if defined(COROUTINE) || defined(MTASK)
		compile_time_check() {
			undefined(UNIQUE_IMPLEMENTATION_OPTION);
		}
#	else
#		undef	MTHREAD
#		undef	COROUTINE
#		undef	MTASK
#		define	MTHREAD		1
#		define	COROUTINE	0
#		define	MTASK		0
#	endif
#else
#if defined(COROUTINE)
#	if defined(MTASK)
		compile_time_check() {
			undefined(UNIQUE_IMPLEMENTATION_OPTION);
		}
#	else
#		undef	MTHREAD
#		undef	COROUTINE
#		undef	MTASK
#		define	MTHREAD		0
#		define	COROUTINE	1
#		define	MTASK		0
#	endif
#else
#if defined(MTASK)
#	undef	MTHREAD
#	undef	COROUTINE
#	undef	MTASK
#	define	MTHREAD		0
#	define	COROUTINE	0
#	define	MTASK		1
#else
	compile_time_check() {
		undefined(IMPLEMENTATION_OPTION);
	}
#endif
#endif
#endif

/*
 * The following should be defined
 * as 1 to enable, 0 to disable.
 */
#define	SCHED_HINT	0
