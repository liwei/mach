/* 
 * Mach Operating System
 * Copyright (c) 1994-1987 Carnegie Mellon University
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
 * File: us/include/io_error.h
 *
 */

/*
 * Mach I/O Interface.
 *
 * Return codes for I/O operations.
 */

/*
 * HISTORY
 * $Log:	io_error.h,v $
 * Revision 1.10  94/07/08  15:51:17  mrt
 * 	Updated copyright.
 * 
 * Revision 1.9  91/05/05  19:23:19  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:43:14  dpj]
 * 
 * 	Added IO_INVALID_RECNUM and IO_NOT_ENOUGH_DATA.
 * 	[91/04/28  09:18:29  dpj]
 * 
 * Revision 1.8  89/11/28  19:08:02  dpj
 * 	Added IO_INVALID_STRATEGY and IO_REJECTED, needed for bytestreams.
 * 	Removed IO_WAIT and IO_NOTIFY, no longer used anywhere.
 * 	[89/11/20  20:18:33  dpj]
 * 
 * Revision 1.7  89/07/09  14:16:27  dpj
 * 	Updated error codes for new unified scheme.
 * 	[89/07/08  12:16:09  dpj]
 * 
 * Revision 1.6  89/05/17  15:55:17  dorr
 * 	include file cataclysm
 * 
 * Revision 1.5  89/03/17  12:17:35  sanzi
 * 	IO_INVALID_RECNUM becomes IO_INVALID_OFFSET.
 * 	[89/02/15  14:50:00  sanzi]
 * 
 * Revision 1.4  88/10/17  10:33:48  dpj
 * Added IO_MULTIPLE_ACTIVATION.
 * 
 * Revision 1.3  88/09/01  16:55:58  dpj
 * Unify NS and I/O interfaces
 * 
 * Revision 1.2.1.1  88/08/25  11:48:15  dpj
 * Unify NS and I/O interfaces
 * 
 * Revision 1.2  88/08/10  11:05:02  dorr
 * change the error subsystem number to 7.
 * 
 * Revision 1.1  88/07/19  18:37:28  dpj
 * Initial revision
 * 
 */

#ifndef	_IO_ERROR_H_
#define	_IO_ERROR_H_

#include	<mach_error.h>
#include	<us_error.h>


#define	IO_MODULE		(err_server|err_sub(7))

#define	IO_INVALID_OFFSET			(IO_MODULE |	1)
#define	IO_INVALID_SIZE				(IO_MODULE |	2)
#define	IO_INVALID_MODE				(IO_MODULE |	3)
#define	IO_INVALID_STRATEGY			(IO_MODULE |	4)
#define	IO_REJECTED				(IO_MODULE |	5)
#define	IO_INVALID_RECNUM			(IO_MODULE |	6)
#define	IO_NOT_ENOUGH_DATA			(IO_MODULE |	7)


/*
 * Obsolete error definitions.
 */
#define	IO_OK			ERR_SUCCESS
#define	IO_WOULD_WAIT		US_OBJECT_BUSY
#define	IO_INVALID_ACCESS	US_ACCESS_DENIED
#define	IO_BAD_ARGS		US_INVALID_ARGS
#define	IO_NOT_IDLE		US_EXCLUSIVE_ACCESS
#define	IO_DEAD			US_OBJECT_DEAD
#define	IO_BUSY			US_OBJECT_BUSY
#define	IO_INTERNAL_ERROR	US_INTERNAL_ERROR
#define	IO_NOT_ACTIVATED	US_OBJECT_NOT_STARTED

#endif	_IO_ERROR_H_
