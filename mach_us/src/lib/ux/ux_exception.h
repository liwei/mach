/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988,1987 Carnegie Mellon University
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
 **********************************************************************
 * HISTORY
 * $Log:	ux_exception.h,v $
 * Revision 2.4  94/07/08  16:01:48  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  91/11/06  14:11:01  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:14:04  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:05:59  pjg]
 * 
 * Revision 2.2  90/07/09  16:59:03  dorr
 * 	No Further Changes
 * 	[90/07/09  10:32:40  jms]
 * 
 * Revision 2.2  90/06/02  15:26:20  rpd
 * 	Cleaned up conditionals; removed MACH, CMU, CMUCS, MACH_NO_KERNEL.
 * 	[90/04/28            rpd]
 * 	Converted to new IPC.
 * 	Added EXC_UNIX_ABORT.
 * 
 * 	Out-of-kernel version.
 * 	[89/01/06            dbg]
 * 
 * Revision 2.1  89/08/04  14:46:33  rwd
 * Created.
 * 
 * Revision 2.2  88/08/24  02:52:12  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:27:27  mwyoung]
 * 
 *
 * 29-Sep-87  David Black (dlb) at Carnegie-Mellon University
 *	Created.
 *
 **********************************************************************
 */

#ifndef	_SYS_UX_EXCEPTION_H_
#define	_SYS_UX_EXCEPTION_H_

/*
 *	Codes for Unix software exceptions under EXC_SOFTWARE.
 */


#define EXC_UNIX_BAD_SYSCALL	0x10000		/* SIGSYS */

#define EXC_UNIX_BAD_PIPE	0x10001		/* SIGPIPE */

#define EXC_UNIX_ABORT		0x10002		/* SIGABRT */

#ifdef	KERNEL
#include <uxkern/import_mach.h>

/*
 *	Kernel data structures for Unix exception handler.
 */

struct mutex		ux_handler_init_lock;
mach_port_t		ux_exception_port;

#endif	KERNEL
#endif	_SYS_UX_EXCEPTION_H_
