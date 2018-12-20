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
 * $Log:	disk.h,v $
 * Revision 2.4  94/07/21  11:57:50  mrt
 * 	updated copyright
 * 
 * Revision 2.3  93/01/20  17:40:02  jms
 * 	Hooks for bug fixes from roy@osf.org
 * 	[93/01/18  17:47:46  jms]
 * 
 * Revision 2.2  91/07/03  19:06:59  jms
 * 	Merge forward to new branch.
 * 	[91/05/29  11:00:44  roy]
 * 
 * Revision 2.1.2.1  90/11/05  18:46:07  roy
 * 	No change.
 * 
 * 
 * Revision 2.1.1.1  90/10/29  15:34:08  roy
 * 	Initial Revision.
 * 
 * 
 *
 */

/*
 * Header for disk device abstraction.
 */

#ifndef	_DISK_
#define	_DISK_

#include <mach.h>

mach_port_t		disk_device();
int			disk_blksize();

mach_error_t		disk_open();
mach_error_t		disk_read();
mach_error_t		disk_write();
#if !defined(MACH3_UNIX)
mach_error_t		disk_read_async();
mach_error_t		disk_write_async();
#endif !defined(MACH3_UNIX)

#endif 	_DISK_

