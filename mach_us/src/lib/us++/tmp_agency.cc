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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tmp_agency.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Base agency for objects in the volatile name space,
 *	that disappear from that name space when not in use.
 *
 * HISTORY
 * $Log:	tmp_agency.cc,v $
 * Revision 2.4  94/07/07  17:24:51  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:29:24  dpj
 * 	Remove the "real" temporary agency code putting it into the new "tmp_prop"
 * 	mechinism.  Then use this new mechinism to implement tmp_agency.
 * 	[92/06/24  16:16:40  jms]
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  01:01:19  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:49  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:40:54  pjg]
 * 
 * Revision 2.2  91/05/05  19:27:53  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:09  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:21:29  dpj]
 * 
 */

#include	<tmp_agency_ifc.h>
#include	<dir_ifc.h>

#define BASE vol_agency
DEFINE_CLASS(tmp_agency)
/*
 * Methods.
 */

DEFINE_TMP_PROP(tmp_agency, tmp_obj)

void tmp_agency::init_class(usClass* class_obj)
{
    vol_agency::init_class(class_obj);
}

tmp_agency::tmp_agency(ns_mgr_id_t mgr_id, access_table *access_table_obj)
	:
	vol_agency(mgr_id, access_table_obj)
{
}

tmp_agency::~tmp_agency()
{
	DESTRUCTOR_GUARD();
}

mach_error_t tmp_agency::ns_tmp_cleanup_for_shutdown()	
	{vol_agency::ns_tmp_cleanup_for_shutdown();}

/*
 * Simple override of ns_get_attributes from the base class, to
 * add the link_count attribute.
 */
mach_error_t tmp_agency::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

	ret = vol_agency::ns_get_attributes(attr,attrlen);
	if (ERR_SUCCESS != ret) return(ret);

	ret = tmp_obj.ns_get_nlinks_attribute(attr, attrlen);
	
	return(ret);
}
