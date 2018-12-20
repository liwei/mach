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
 * syscall_common.c
 *
 * Architecture-independent system call routines.
 *
 * Michael B. Jones
 *
 * 14-Sep-1989
 */
/*
 * HISTORY:
 * $Log:	syscall_common.cc,v $
 * Revision 2.3  94/07/08  16:57:43  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/06  11:33:44  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:40:30  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:33:36  pjg]
 * 
 * Revision 2.5  91/10/06  22:27:11  jjc
 * 	Changed emul_generic() to be a switch statement which returns EINVAL
 * 	for the table system call and returns 0 for everything else.
 * 	[91/09/06            jjc]
 * 
 * Revision 2.4  91/07/01  14:07:14  jms
 * 	Changed the message in emul_generic to be at debug level 0 instead
 * 	of "critical".
 * 	[91/06/16  21:02:55  dpj]
 * 	Shut off "emul_generic" messages unless debugging
 * 	[91/06/24  16:32:29  jms]
 * 
 * Revision 2.3  90/11/27  18:19:30  jms
 * 	Return val bugfixes
 * 	[90/11/20  14:07:22  jms]
 * 
 * Revision 2.2  89/10/06  14:00:48  mbj
 * 	Wrote "generic" routine to catch syscalls which are not yet emulated.
 * 	[89/10/06  00:31:16  mbj]
 * 
 */

#include <emul_base.h>
#include <sys/errno.h>

int emul_generic(int syscall, int *args, syscall_val_t *rval)
{
	DEBUG1(emul_debug,(Diag, "emul_generic(%d) called\n", syscall));
	/*
	 *	System call 63 (nosys) vectors here.
	 *	XXX Use the normal semantics of nosys() and return EINVAL for
	 *	    the table call.  For all the other system calls that aren't
	 *	    implemented, return 0 and pretend that everything worked.
	 *	    NOTE: jms said to credit dpj with this. XXX
	 */
	switch (syscall) {
		case -6:	/* table system call */
			rval->rv_val1 = EINVAL;
			return unix_err(EINVAL);
		default:
			rval->rv_val1 = 0;
			rval->rv_val2 = 0;
			return 0;
	}
}
