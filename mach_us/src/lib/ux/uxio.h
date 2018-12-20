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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio.h,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxio.h,v $
 * Revision 1.4  94/07/08  16:01:58  mrt
 * 	Updated copyrights.
 * 
 * Revision 1.3  91/05/05  19:28:42  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:01:58  dpj]
 * 
 * 	Use sequential versions of I/O operations when appropriate.
 * 	[91/04/28  10:31:34  dpj]
 * 
 * Revision 1.2  89/05/17  16:45:51  dorr
 * 	include file cataclysm
 * 
 * Revision 1.1  89/02/21  21:55:00  dorr
 * Initial revision
 * 
 * Revision 1.1.1.1  89/02/20  21:11:55  dorr
 * 	?
 * 
 * Revision 1.1  88/10/20  20:21:50  dorr
 * Initial revision
 * 
 *
 */

#ifndef	_UXIO_H
#define	_UXIO_H

#include <mach_error.h>

#define	KERNEL_FILE_IO	1

/* file flags */
typedef	unsigned int	file_flag_t;	
#define	FILE_FLAG_NONE		(0)
#define	FILE_NDELAY		(0x01)
#define	FILE_APPEND		(0x02)
#define	FILE_ASYNC		(0x04)
#define	FILE_SEQUENTIAL		(0x08)	/* dummy flag for sequential I/O
					   objects -- not settable by users */


#endif	_UXIO_H



