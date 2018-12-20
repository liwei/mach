/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/usint_mf_proxy_ifc.h,v $
 *
 * usint_mf_proxy: proxy for mapped-file I/O.
 *
 * 
 * Purpose:  Mapped file proxy interface
 * 
 * HISTORY:
 * $Log:	usint_mf_proxy_ifc.h,v $
 * Revision 2.3  94/07/07  17:59:40  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:36  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:57:22  jms]
 * 
 * Revision 2.2  92/07/05  23:31:40  dpj
 * 	First working version.
 * 	[92/06/24  17:24:54  dpj]
 * 
 */

#ifndef	_usint_mf_proxy_ifc_h
#define	_usint_mf_proxy_ifc_h

#include <us_byteio_proxy_ifc.h>

#include <mf_user_ifc.h>

class usint_mf_proxy: public VIRTUAL5 usByteIO_proxy {
      public:
	DECLARE_PROXY_MEMBERS(usint_mf_proxy);
			usint_mf_proxy();

	mach_error_t	clone_complete();

	DECLARE_MF_USER_PROP(mf_prop);
};

#endif	_usint_mf_proxy_ifc_h

