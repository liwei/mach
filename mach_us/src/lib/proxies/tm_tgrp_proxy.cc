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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/tm_tgrp_proxy.cc,v $
 *
 * 
 * Purpose:  client proxy for a task group
 * 
 * HISTORY:
 * $Log:	tm_tgrp_proxy.cc,v $
 * Revision 2.3  94/07/07  17:59:34  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:27  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:54:34  jms]
 * 
 * Revision 2.2  92/07/05  23:29:18  dpj
 * 	Converted for new RPC package.
 * 	Fixed-up the mess with the RPC constructor.
 * 	[92/07/05  18:56:49  dpj]
 * 
 * 	Use new us_tm_tgrp_ifc.h interfaces for the C++ taskmaster to implement
 * 	logic that had been in tm_jgrp_proxy.cc.
 * 	[92/06/24  16:10:54  jms]
 * 
 * Revision 2.2  91/11/06  13:48:34  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:39:26  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:37:54  pjg]
 * 
 */

#ifndef lint
char * tm_tgrp_proxy_rcsid = "$Header: tm_tgrp_proxy.cc,v 2.3 94/07/07 17:59:34 mrt Exp $";
#endif	lint

#include <tm_tgrp_proxy_ifc.h>


#define BASE usTMTgrp_proxy
_DEFINE_CLASS(tm_tgrp_proxy);
_DEFINE_INSTANTIATION(tm_tgrp_proxy);
_DEFINE_CASTDOWN(tm_tgrp_proxy);
_DEFINE_CONVERTER(tm_tgrp_proxy);


void tm_tgrp_proxy::init_class(usClass* class_obj)
{
        BASE::init_class(class_obj);

        BEGIN_SETUP_METHOD_WITH_ARGS(tm_tgrp_proxy);
        SETUP_METHOD_WITH_ARGS(tm_tgrp_proxy,tm_get_tgrp_id);
        END_SETUP_METHOD_WITH_ARGS;
}

tm_tgrp_proxy::tm_tgrp_proxy()
: 
	tgrp_id(NULL_TGRP_ID)
{
}

tm_tgrp_proxy::tm_tgrp_proxy(void* p, const usClass& c)
: 
	tgrp_id(NULL_TGRP_ID),
	usTMTgrp_proxy(p,c)
{
}

mach_error_t tm_tgrp_proxy::tm_get_tgrp_id(tm_tgrp_id_t* id)
{
	mach_error_t	ret = ERR_SUCCESS;

	if (NULL_TGRP_ID == tgrp_id) {
		ret = outgoing_invoke(mach_method_id(tm_get_tgrp_id),
				&tgrp_id);
	}

	*id = tgrp_id;
	return ret;
}
