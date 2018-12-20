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
 * File: us/include/sunrpc_error.h
 *
 */

/*
 * Mach Generic Errors
 *
 */

/*
 * HISTORY: 
 * $Log:	sunrpc_error.h,v $
 * Revision 2.3  94/07/08  15:51:35  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  89/06/30  18:30:39  dpj
 * 	Initial revision
 * 	[89/06/21  22:14:03  dpj]
 * 
 */

#ifndef	_SUNRPC_ERROR_H_
#define	_SUNRPC_ERROR_H_

#include	<mach_error.h>

#define	SUNRPC_MODULE		(err_server|err_sub(10))

#define	sunrpc_error(n)		(SUNRPC_MODULE | ((mach_error_t)(n)))

#endif	_SUNRPC_ERROR_H_


