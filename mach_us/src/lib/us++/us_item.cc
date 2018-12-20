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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_item.cc,v $
 *
 * usItem: abstract class defining the generic operations available for
 *	   all objects.
 *
 *
 * HISTORY
 * $Log:	us_item.cc,v $
 * Revision 2.5  94/07/07  17:25:10  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/06  07:53:39  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.3  92/07/05  23:29:58  dpj
 * 	Add ns_get_item_ptr
 * 	[92/06/24  16:35:16  jms]
 * 	Made ns_(un)register_agent() into real functions instead of pure
 * 	virtuals, to get around a bug in gcc-2.0.
 * 	[92/03/10  20:32:09  dpj]
 * 
 * Revision 2.2  91/11/06  13:49:39  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:52:33  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:22  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:04:54  pjg]
 * 
 */

#include <us_item_ifc.h>

#define BASE usRemote
DEFINE_ABSTRACT_CLASS(usItem);

DEFINE_METHOD_ARGS(ns_authenticate,"rpc K<100001>: IN int; IN p(COPY_SEND); OUT * object<usItem>;");
DEFINE_METHOD_ARGS(ns_duplicate,"rpc K<100002>: IN int; OUT * object<usItem>;");
DEFINE_METHOD_ARGS(ns_get_attributes,"rpc K<100003>: OUT * int[*:100]; IN OUT * int;");
DEFINE_METHOD_ARGS(ns_set_times,"rpc K<100004>: IN int[2]; IN int[2];");
DEFINE_METHOD_ARGS(ns_get_protection,"rpc K<100005>: OUT * int[*:100]; IN OUT * int;");
DEFINE_METHOD_ARGS(ns_set_protection, "rpc K<100006>: IN * int[*]; IN int;");
DEFINE_METHOD_ARGS(ns_get_privileged_id, "rpc K<100007>: OUT * int;");
DEFINE_METHOD_ARGS(ns_get_access,"rpc K<100008>: OUT * int; OUT * int[*:100]; IN OUT * int;");
DEFINE_METHOD_ARGS(ns_get_manager,"rpc K<100009>: IN int; OUT * object<usItem>;");


/*
 * Return a pointer to this item.  Dummy routine, needed when
 * the caller does not know if he is talking to an agent or to
 * the agency directly.  Agent implementation must overide definition
 * to return the agency.
 */
mach_error_t usItem::ns_get_item_ptr(usItem **item)
{
	mach_object_reference(this);
	*item = this;
	return(ERR_SUCCESS);
}

mach_error_t usItem::ns_register_agent(ns_access_t access)
{
	return(ERR_SUCCESS);
}

mach_error_t usItem::ns_unregister_agent(ns_access_t access)
{
	return(ERR_SUCCESS);
}

