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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/us_item_proxy.cc,v $

 *
 * Purpose:  Base proxy for items returned to a client process from a server
 *		All other proxies are specializations of this one.
 *
 * HISTORY:
 * $Log:	us_item_proxy.cc,v $
 * Revision 2.3  94/07/07  17:59:37  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:30  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:55:49  jms]
 * 
 * Revision 2.4  92/07/05  23:30:00  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:18:53  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:19:25  dpj]
 * 
 * Revision 2.3  92/03/05  15:05:54  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:34:22  jms]
 * 
 * Revision 2.2  91/11/06  13:49:47  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:52:55  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:33  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:27  pjg]
 * 
 */

#ifndef lint
char * us_item_proxy_rcsid = "$Header: us_item_proxy.cc,v 2.3 94/07/07 17:59:37 mrt Exp $";
#endif	lint

#include <us_item_proxy_ifc.h>


boolean_t cat1_debug = 1;

#ifdef	GXXBUG_CLONING1
#define BASE usClone
DEFINE_PROXY_CLASS(usItem_proxy)
_DEFINE_CASTDOWN(usItem_proxy)
#else	GXXBUG_CLONING1
DEFINE_PROXY_CLASS(usItem_proxy)
DEFINE_CASTDOWN2(usItem_proxy,usItem,usClone)
#endif	GXXBUG_CLONING1


void usItem_proxy::init_class(usClass* class_obj)
{
#ifdef	GXXBUG_CLONING1
        BASE::init_class(class_obj);
#else	GXXBUG_CLONING1
        usItem::init_class(class_obj);
        usClone::init_class(class_obj);
#endif	GXXBUG_CLONING1

        BEGIN_SETUP_METHOD_WITH_ARGS(usItem_proxy);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_authenticate);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_duplicate);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_get_attributes);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_set_times);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_get_protection);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_set_protection);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_get_privileged_id);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_get_access);
        SETUP_METHOD_WITH_ARGS(usItem_proxy,ns_get_manager);
        END_SETUP_METHOD_WITH_ARGS;

	class_obj->set_remote_class_name("usItem_proxy");
}

mach_error_t usItem_proxy::clone_init(mach_port_t child)
{
	mach_error_t	ret = ERR_SUCCESS;;
	mach_port_t obj_port = object_port();

	if (obj_port != MACH_PORT_NULL) {
		/*
		 * Give the remote port to the child.
		 */
		DEBUG2(cat1_debug,(0,"clone_init - insert %#d into %#d\n",
				  Local(object_port), child));
		ret = mach_port_insert_right(child, obj_port, obj_port, 
					     MACH_MSG_TYPE_COPY_SEND);
		if (ret == KERN_NAME_EXISTS) {
			/* XXX - temp until no more senders */
			ret = ERR_SUCCESS;
		}
		if (ret != ERR_SUCCESS) {
			DEBUG1(cat1_debug, 
			       (0,"%s::clone_init - insert fails %#x\n", 
				class_name(), ret));
			return(ret);
		}
	}
	return(ERR_SUCCESS);
}

mach_error_t usItem_proxy::clone_abort(mach_port_t child)
{
	/* 
	 * Note: object port references will automagically go away 
	 *	when the child task dies
	 */
	return(ERR_SUCCESS);
}

mach_error_t usItem_proxy::clone_complete()
{
	return ERR_SUCCESS;
}

mach_error_t 
usItem_proxy::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
	return (outgoing_invoke(mach_method_id(ns_authenticate),access,t,obj));
}

mach_error_t 
usItem_proxy::ns_duplicate(ns_access_t access, usItem** newobj)
{
	return (outgoing_invoke(mach_method_id(ns_duplicate),access,newobj));
}

mach_error_t usItem_proxy::ns_get_attributes(ns_attr_t atr, int* atrlen)
{
	return (outgoing_invoke(mach_method_id(ns_get_attributes),atr,atrlen));
}

mach_error_t usItem_proxy::ns_set_times(time_value_t atime, time_value_t mtime)
{
	return (outgoing_invoke(mach_method_id(ns_set_times),atime,mtime));
}

mach_error_t usItem_proxy::ns_get_protection(ns_prot_t prot, int* protlen)
{
	return (outgoing_invoke(mach_method_id(ns_get_protection),prot,protlen));
}

mach_error_t usItem_proxy::ns_set_protection(ns_prot_t prot, int protlen)
{
	return (outgoing_invoke(mach_method_id(ns_set_protection),prot,protlen));
}

mach_error_t usItem_proxy::ns_get_privileged_id(int* id)
{
	return (outgoing_invoke(mach_method_id(ns_get_privileged_id),id));
}

mach_error_t 
usItem_proxy::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
	return (outgoing_invoke(mach_method_id(ns_get_access),access,cred,credlen));
}

mach_error_t 
usItem_proxy::ns_get_manager(ns_access_t access, usItem **newobj)
{
	return (outgoing_invoke(mach_method_id(ns_get_manager),access,newobj));
}
