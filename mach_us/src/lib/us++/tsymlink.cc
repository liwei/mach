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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tsymlink.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Transparent symlink for the "volatile name space".
 *
 * HISTORY
 * $Log:	tsymlink.cc,v $
 * Revision 2.3  94/07/07  17:25:01  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:49:00  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:41:53  pjg]
 * 
 * Revision 2.2  91/05/05  19:27:57  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:16  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:22:04  dpj]
 * 
 */

#ifndef lint
char * tsymlink_rcsid = "$Header: tsymlink.cc,v 2.3 94/07/07 17:25:01 mrt Exp $";
#endif	lint

#include	<tsymlink_ifc.h>


#define BASE symlink
DEFINE_CLASS(tsymlink);

tsymlink::tsymlink(ns_mgr_id_t mgr_id, access_table* access_table_obj,
		   ns_path_t fwdpath, mach_error_t* ret)
	:
	symlink(mgr_id, access_table_obj, fwdpath, ret)
{}

void tsymlink::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(tsymlink);
	SETUP_METHOD_WITH_ARGS(tsymlink,ns_get_attributes);
	END_SETUP_METHOD_WITH_ARGS;

}


mach_error_t tsymlink::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

	ret = symlink::ns_get_attributes(attr,attrlen);
	attr->type = NST_TRANSPARENT_SYMLINK;
	return(ret);
}

