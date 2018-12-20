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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/tm_tgrp_proxy_ifc.h,v $
 *
 * Purpose: Proxy for task objects.
 *
 * HISTORY:
 * $Log:	tm_tgrp_proxy_ifc.h,v $
 * Revision 2.3  94/07/07  17:59:36  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:29  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:55:00  jms]
 * 
 * Revision 2.2  92/07/05  23:29:20  dpj
 * 	Use non-inline default constructor, with initalization for local cache.
 * 	[92/07/05  18:57:25  dpj]
 * 
 * 	Use new us_tm_tgrp_ifc.h interface for the C++ taskmaster to implement
 * 	functionality that had been found in tm_jgrp_proxy_ifc.h
 * 	[92/06/24  16:14:25  jms]
 * 
 * Revision 2.2  91/11/06  13:48:38  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:57:13  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:38:11  pjg]
 * 
 */

#ifndef	_tm_tgrp_proxy_ifc_h
#define	_tm_tgrp_proxy_ifc_h

#include <us_tm_tgrp_proxy_ifc.h>

class tm_tgrp_proxy: public usTMTgrp_proxy {
	tm_tgrp_id_t tgrp_id;
      public:
	DECLARE_PROXY_MEMBERS(tm_tgrp_proxy);
	tm_tgrp_proxy();

REMOTE	virtual mach_error_t tm_get_tgrp_id(tm_tgrp_id_t*);
};
	
#endif	_tm_tgrp_proxy_ifc_h
