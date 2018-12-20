/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * Network Errors
 *
 */

/*
 * HISTORY: 
 * $Log:	net_error.h,v $
 * Revision 2.3  94/07/08  15:51:24  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/05/05  19:23:37  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:43:29  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  09:22:51  dpj]
 * 
 */

#ifndef	_NET_ERROR_H_
#define	_NET_ERROR_H_

#include	<mach_error.h>

#define	NET_MODULE		(err_server|err_sub(16))

#define	NET_INVALID_ADDR_FLAVOR	(NET_MODULE |    1)
#define	NET_INVALID_ADDR_VALUE	(NET_MODULE |    2)
#define	NET_IS_CONNECTED	(NET_MODULE |	 3)
#define	NET_NOT_CONNECTED	(NET_MODULE |	 4)
#define	NET_CONNECTION_REFUSED	(NET_MODULE |	 5)

#endif	_NET_ERROR_H_


