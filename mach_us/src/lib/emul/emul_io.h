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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/emul/emul_io.h,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	emul_io.h,v $
 * Revision 1.15  94/07/08  16:57:03  mrt
 * 	Updated copyrights.
 * 
 * Revision 1.14  92/07/05  23:25:12  dpj
 * 	No changes.
 * 	[92/06/29  22:44:59  dpj]
 * 
 * 	Removed include of mach_object.h.
 * 	[92/05/10  00:32:10  dpj]
 * 
 * 	Removed FD_* flags and MAX_OPEN_FILES definitions (not used).
 * 	[92/03/10  20:30:01  dpj]
 * 
 * Revision 1.13  90/01/02  21:44:29  dorr
 * 	get rid of KERNEL_FILE_IO
 * 
 * Revision 1.12.1.2  90/01/02  14:10:52  dorr
 * 	get rid of KERNEL_FILE_IO (now KernelFileIO)
 * 
 * Revision 1.12.1.1  89/12/18  15:56:40  dorr
 * 	get rid of dll.h
 * 
 * Revision 1.12  89/07/19  11:35:52  dorr
 * 	change the way file table flags work.
 * 	clean up.
 * 
 * Revision 1.11.1.1  89/06/21  15:54:48  dorr
 * 	make file table an object.  move file table specific stuff into
 * 	ftab implementation files.
 * 
 * Revision 1.11  89/05/17  16:14:31  dorr
 * 	include file cataclysm
 * 
 * Revision 1.10  88/10/28  13:34:29  dorr
 * make list of objects that gets cloned on fork global.
 * add routines to deal with them.
 * 
 * Revision 1.9  88/10/25  16:40:09  dorr
 * new wave i/o operations.
 * 
 * Revision 1.8  88/10/14  16:28:29  dorr
 * working machtalk objects.
 * 
 *
 */

#ifndef	_EMUL_IO_H
#define	_EMUL_IO_H

#include <cthreads.h>
#include <mach_error.h>

#endif	_EMUL_IO_H



