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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/usint_mf_proxy.cc,v $
 *
 * usint_mf_proxy: proxy for mapped-file I/O.
 *
 * 
 * Purpose:  Mapped file proxy
 * 
 * HISTORY
 * $Log:	usint_mf_proxy.cc,v $
 * Revision 2.3  94/07/07  17:59:39  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:34  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:56:50  jms]
 * 
 * Revision 2.2  92/07/05  23:31:38  dpj
 * 	First working version.
 * 	[92/06/24  17:24:40  dpj]
 * 
 */

#ifndef lint
char * usint_mf_proxy_rcsid = "$Header: usint_mf_proxy.cc,v 2.3 94/07/07 17:59:39 mrt Exp $";
#endif	lint

#include <usint_mf_proxy_ifc.h>

#define	BASE	usByteIO_proxy
//DEFINE_PROXY_CLASS(usint_mf_proxy);
_DEFINE_CLASS(usint_mf_proxy);
_DEFINE_INSTANTIATION(usint_mf_proxy);
_DEFINE_CONVERTER(usint_mf_proxy);
_DEFINE_CASTDOWN(usint_mf_proxy);


void usint_mf_proxy::init_class(usClass* class_obj)
{
        usByteIO_proxy::init_class(class_obj);

        BEGIN_SETUP_METHOD_WITH_ARGS(usint_mf_proxy);
	SETUP_MF_USER_PROP(usint_mf_proxy);
        END_SETUP_METHOD_WITH_ARGS;
}

usint_mf_proxy::usint_mf_proxy()
	:
	mf_prop(this)
{}

usint_mf_proxy::usint_mf_proxy(void* p, const usClass&)
	:
	mf_prop(this)
{
	set_object_port((mach_port_t)p);
}

mach_error_t usint_mf_proxy::clone_complete()
{
	(void) mf_prop.clone_complete();
	return usByteIO_proxy::clone_complete();
}
