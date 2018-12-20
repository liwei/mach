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
 *
 * File: us/include/us_error.h
 *
 */

/*
 * Mach Generic Errors
 *
 */

/*
 * HISTORY: 
 * $Log:	us_error.h,v $
 * Revision 1.8  94/07/08  15:51:48  mrt
 * 	Updated copyright.
 * 
 * Revision 1.7  92/07/05  23:23:28  dpj
 * 	Added US_NO_REMOTE_MGR for mapped-file control.
 * 	[92/06/24  13:25:33  dpj]
 * 
 * Revision 1.6  89/07/19  11:39:16  dorr
 * 	merge.
 * 
 * Revision 1.3.2.2  89/07/10  15:38:22  dorr
 * 	get rid of extra crap produced by clumsy mergin
 * 
 * Revision 1.3.2.1  89/07/06  14:06:57  dorr
 * 	update with dan's new generic errors
 * 
 * Revision 1.5  89/07/09  14:16:50  dpj
 * 	Updated error codes for new unified scheme.
 * 	[89/07/08  12:32:27  dpj]
 * 
 * Revision 1.4  89/06/30  18:30:45  dpj
 * 	Added several generic error codes.
 * 	Fixed the module number to avoid a conflict with
 * 	the authentication server.
 * 	[89/06/21  22:15:03  dpj]
 * 
 * Revision 1.3  89/05/17  16:08:29  dorr
 * 	include file cataclysm
 * 
 * Revision 1.2  89/03/17  12:21:18  sanzi
 * 	add US_OUT_OF_MEMORY
 * 	add object_not_found, object_exists and invalid access codes.
 * 
 * Revision 1.1.1.2  89/02/08  11:28:52  dorr
 * 	add US_OUT_OF_MEMORY
 * 
 * Revision 1.1.1.1  89/02/03  13:50:14  dorr
 * 	add object_not_found, object_exists and invalid access codes.
 * 
 * Revision 1.1  89/02/03  13:29:02  dorr
 * Initial revision
 * 
 */

#ifndef	_US_ERROR_H_
#define	_US_ERROR_H_

#include	<mach_error.h>

#define	US_MODULE		(err_server|err_sub(9))

#define	US_UNKNOWN_ERROR	(US_MODULE |    1)
#define	US_OBJECT_NOT_FOUND	(US_MODULE |    2)
#define	US_OBJECT_EXISTS	(US_MODULE |    3)
#define	US_OBJECT_BUSY		(US_MODULE |    4)
#define	US_OBJECT_NOT_STARTED	(US_MODULE |	5)
#define	US_OBJECT_DEAD		(US_MODULE |	6)
#define	US_INVALID_ARGS		(US_MODULE |    7)
#define	US_INVALID_ACCESS	(US_MODULE |	8)
#define	US_INVALID_FORMAT	(US_MODULE |    9)
#define	US_INVALID_BUFFER_SIZE	(US_MODULE |   10)
#define	US_ACCESS_DENIED	(US_MODULE |   11)
#define	US_RESOURCE_EXHAUSTED	(US_MODULE |   12)
#define	US_QUOTA_EXCEEDED	(US_MODULE |   13)
#define	US_LIMIT_EXCEEDED	(US_MODULE |   14)
#define	US_NOT_IMPLEMENTED	(US_MODULE |   15)
#define	US_UNSUPPORTED		(US_MODULE |   16)
#define	US_HARDWARE_ERROR	(US_MODULE |   17)
#define	US_RETRY_REQUIRED	(US_MODULE |   18)
#define	US_NOT_AUTHENTICATED	(US_MODULE |   19)
#define	US_EXCLUSIVE_ACCESS	(US_MODULE |   20)
#define	US_TIMEOUT		(US_MODULE |   21)
#define	US_BAD_REFCOUNT		(US_MODULE |   22)
#define	US_INTERNAL_ERROR	(US_MODULE |   23)
#define	US_NO_REMOTE_MGR	(US_MODULE |   24)

/*
 * Obsolete error definitions.
 */
#define	US_OUT_OF_MEMORY	US_RESOURCE_EXHAUSTED

#endif	_US_ERROR_H_


