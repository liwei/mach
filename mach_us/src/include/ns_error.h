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
 * File: us/include/ns_error.h
 *
 */

/*
 * Mach Name Server.
 *
 * Return codes for NS operations.
 */

/*
 * HISTORY: 
 * $Log:	ns_error.h,v $
 * Revision 1.13  94/07/08  15:51:27  mrt
 * 	Updated copyright.
 * 
 * Revision 1.12  91/05/05  19:23:46  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:43:43  dpj]
 * 
 * 	Added NS_INVALID_LINK_COUNT, NS_TFORWARD_FAILURE.
 * 	[91/04/28  09:27:23  dpj]
 * 
 * Revision 1.11  89/10/30  16:28:00  dpj
 * 	Added NS_ENTRY_NOT_RESERVED.
 * 	[89/10/27  16:31:05  dpj]
 * 
 * Revision 1.10  89/07/09  14:16:42  dpj
 * 	Updated error codes for new unified scheme.
 * 	[89/07/08  12:30:50  dpj]
 * 
 * Revision 1.9  89/06/30  18:30:12  dpj
 * 	Added NS_CANNOT_INSERT.
 * 	[89/06/28  23:56:42  dpj]
 * 
 * 	Merged with mainline.
 * 	[89/05/31  17:33:54  dpj]
 * 
 * 	Added NS_PREFIX_OVERFLOW.
 * 	[89/05/26  16:13:35  dpj]
 * 
 * Revision 1.8  89/05/17  16:01:11  dorr
 * 	include file cataclysm
 * 
 * Revision 1.7  89/03/17  12:18:38  sanzi
 * 	add ns_set_prot_failed.
 * 	[89/02/14  11:31:46  dorr]
 * 
 */

#ifndef	_NS_ERROR_H_
#define	_NS_ERROR_H_

#include	<mach_error.h>
#include	<us_error.h>

#define	NS_MODULE		(err_server|err_sub(6))

#define	NS_INVALID_HANDLE	(NS_MODULE |	1)
#define	NS_NAME_NOT_FOUND	(NS_MODULE |	2)
#define	NS_NAME_EXISTS		(NS_MODULE |	3)
#define	NS_NAME_TOO_LONG	(NS_MODULE |	4)
#define	NS_PATH_TOO_LONG	(NS_MODULE |	5)
#define	NS_INVALID_NAME		(NS_MODULE |	6)
#define	NS_NOT_DIRECTORY	(NS_MODULE |	7)
#define	NS_IS_DIRECTORY		(NS_MODULE |	8)
#define	NS_DIR_NOT_EMPTY	(NS_MODULE |	9)
#define	NS_INFINITE_RETRY	(NS_MODULE |   10)
#define	NS_INFINITE_FORWARD	(NS_MODULE |   11)
#define	NS_INVALID_PREFIX	(NS_MODULE |   12)
#define	NS_PREFIX_OVERFLOW	(NS_MODULE |   13)
#define	NS_BAD_DIRECTORY	(NS_MODULE |   14)
#define	NS_UNKNOWN_ENTRY_TYPE	(NS_MODULE |   15)
#define	NS_INVALID_GENERATION	(NS_MODULE |   16)
#define	NS_ENTRY_NOT_RESERVED	(NS_MODULE |   17)
#define	NS_INVALID_LINK_COUNT	(NS_MODULE |   18)
#define	NS_TFORWARD_FAILURE	(NS_MODULE |   19)


/*
 * Obsolete error definitions.
 */
#define	NS_SUCCESS		ERR_SUCCESS
#define	NS_NOT_FOUND		NS_NAME_NOT_FOUND
#define	NS_INVALID_ACCESS	US_ACCESS_DENIED
#define	NS_NOT_AUTHENTICATED	US_NOT_AUTHENTICATED
#define	NS_ENTRY_EXISTS		NS_NAME_EXISTS
#define	NS_INVALID_ENTRY_DATA	US_INVALID_ARGS
#define	NS_UNKNOWN_FAILURE	US_UNKNOWN_ERROR
#define	NS_BUSY			US_OBJECT_BUSY
#define	NS_UNSUPPORTED_CRED	US_INVALID_FORMAT
#define	NS_UNSUPPORTED_PROT	US_INVALID_FORMAT
#define	NS_NOT_ENOUGH_ROOM	US_RESOURCE_EXHAUSTED
#define	NS_SET_PROT_FAILED	NS_INVALID_GENERATION
#define	NS_CANNOT_INSERT	US_UNSUPPORTED


#endif	_NS_ERROR_H_
