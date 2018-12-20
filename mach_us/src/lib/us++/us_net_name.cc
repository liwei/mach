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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_net_name.cc,v $
 *
 * usName: abstract class defining the naming protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY
 * $Log:	us_net_name.cc,v $
 * Revision 2.4  94/07/07  17:25:24  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/06  07:54:09  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.2  91/11/06  14:07:47  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:58:06  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:53  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 */

#include <us_net_name_ifc.h>

DEFINE_ABSTRACT_CLASS_MI(usNetName);

void* usNetName::_castdown(const usClass& c) const
{
	if (&c == desc()) return (void*) this;
	void* p = usName::_castdown(c);
	void* q = p;
	if (p = usNetBase::_castdown(c)) ambig_check(p, q, c);
	return q;
}

void usNetName::init_class(usClass* class_obj) {
	usName::init_class(class_obj);
	usNetBase::init_class(class_obj);
}

DEFINE_METHOD_ARGS(net_create, "rpc K<101201>: IN OUT * int[3]; IN OUT * int; IN * int[*]; IN int; IN int; OUT * object<usItem>; OUT * int; OUT * int");
DEFINE_METHOD_ARGS(net_lookup, "rpc K<101202>: IN OUT * int[3]; IN int; OUT * object<usItem>; OUT * int; OUT * int");
DEFINE_METHOD_ARGS(net_cots_lookup, "rpc K<101203>: IN OUT * int[3]; IN * int[3]; IN int; OUT * object<usItem>; OUT * int; OUT * int");

