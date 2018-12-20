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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_name.cc,v $
 *
 * usName: abstract class defining the naming protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY
 * $Log:	us_name.cc,v $
 * Revision 2.4  94/07/07  17:25:11  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/06  07:53:42  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.2  91/11/06  13:49:55  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:53:13  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:53  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 */

#include <us_name_ifc.h>

#define BASE usItem
DEFINE_ABSTRACT_CLASS(usName);

DEFINE_METHOD_ARGS(ns_resolve,"rpc K<100101>: IN string; IN int; IN int; OUT * object<usItem>; OUT * int; OUT * char[1024]; OUT * int; OUT * int;");
DEFINE_METHOD_ARGS(ns_create,"rpc K<100102>: IN string; IN int; IN * int[*]; IN int; IN int; OUT * object<usItem>;");
DEFINE_METHOD_ARGS(ns_create_anon,"rpc K<100103>: IN int; IN * int[*]; IN int; IN int; OUT * char[256]; OUT * object<usItem>;");
DEFINE_METHOD_ARGS(ns_create_transparent_symlink,"rpc K<100104>: IN string; IN * int[*]; IN int; IN string;");
DEFINE_METHOD_ARGS(ns_insert_entry,"rpc K<100105>: IN string; IN object<usItem>;");
DEFINE_METHOD_ARGS(ns_insert_forwarding_entry,"rpc K<100106>: IN string; IN * int[*]; IN int; IN object<usItem>; IN string;");
DEFINE_METHOD_ARGS(ns_read_forwarding_entry,"rpc K<100107>: OUT * object<usItem>; OUT * char[1024];");
DEFINE_METHOD_ARGS(ns_remove_entry,"rpc K<100108>: IN string;");
DEFINE_METHOD_ARGS(ns_rename_entry,"rpc K<100109>: IN string; IN object<usItem>; IN string;");
DEFINE_METHOD_ARGS(ns_list_entries,"rpc K<100110>: IN int; OUT COPY DEALLOC ** char[*][256]; OUT * int; OUT COPY DEALLOC ** int[*][2]; OUT * int;");
DEFINE_METHOD_ARGS(ns_list_types,"rpc K<100111>: OUT COPY DEALLOC ** int[*]; OUT * int;");
DEFINE_METHOD_ARGS(ns_allocate_unique_name,"rpc K<100112>: OUT string;");

