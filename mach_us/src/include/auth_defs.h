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
 * auth_defs.h
 *
 *	Public definitions for the authentication service.
 *
 */
/*
 * HISTORY:
 * $Log:	auth_defs.h,v $
 * Revision 1.6  94/07/08  15:53:56  mrt
 * 	Updated copyright
 * 
 * Revision 1.5  92/03/05  14:55:06  jms
 * 	Change US_UNKNOWN_FAILURE => US_UNKNOWN_ERROR
 * 	[92/02/26  16:39:55  jms]
 * 
 * Revision 1.4  89/07/09  14:16:14  dpj
 * 	Updated error codes for new unified scheme.
 * 	[89/07/08  12:13:02  dpj]
 * 
 * Revision 1.3  89/05/17  15:53:25  dorr
 * 	include file cataclysm
 * 
 * 17-Feb-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#ifndef	_AUTH_DEFS_
#define	_AUTH_DEFS_

#include	<mach_error.h>
#include	<us_error.h>


#define GROUP_NAME_SIZE	64
#define FULL_NAME_SIZE	256
#define PASS_WORD_SIZE	16

typedef char		group_name_t[GROUP_NAME_SIZE];
typedef char		full_name_t[FULL_NAME_SIZE];
typedef char		pass_word_t[PASS_WORD_SIZE];
typedef unsigned long	group_id_t;
typedef group_id_t	*group_id_list_t;
typedef group_name_t	*group_name_list_t;
typedef int		group_type_t;

#define GROUP_ID_MAX	4096

/*
 * Group types.
 */
#define AS_PRIMARY	0
#define AS_SECONDARY	1

/*
 * Standard group IDs.
 */
#define NULL_ID		-1
#define DEFAULT_ID	0
#define SYS_ADM_ID	1

/*
 * Error Codes.
 */
#define	AS_MODULE		(err_server|err_sub(8))

#define AS_BAD_PRIVATE_PORT	(AS_MODULE | 1)
#define AS_BAD_NAME		(AS_MODULE | 2)
#define AS_NOT_PRIMARY		(AS_MODULE | 3)
#define AS_BAD_PASS_WORD	(AS_MODULE | 4)
#define AS_BAD_GROUP		(AS_MODULE | 5)
#define AS_DUPLICATE_ID		(AS_MODULE | 6)
#define AS_DUPLICATE_NAME	(AS_MODULE | 7)
#define AS_NOT_SECONDARY	(AS_MODULE | 8)

#define AS_ERROR_MIN		(AS_MODULE | 1)
#define AS_ERROR_MAX		(AS_MODULE | 8)

/*
 * Obsolete error definitions.
 */
#define AS_SUCCESS		ERR_SUCCESS
#define AS_FAILURE		US_UNKNOWN_ERROR
#define AS_TIMEOUT		US_TIMEOUT
#define AS_NOT_ALLOWED		US_ACCESS_DENIED


/*
 * Names that the servers use.
 */
#define CAS_NAME	"CENTRAL_AUTHENTICATION_SERVER"
#define LAS_NAME	"LOCAL_AUTHENTICATION_SERVER"


#endif	_AUTH_DEFS_
