/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/vol_agency.cc,v $
 *
 * Purpose: Base agency for objects in the volatile name space.
 *
 * HISTORY
 * $Log:	vol_agency.cc,v $
 * Revision 2.4  94/07/07  17:26:10  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:32:24  dpj
 * 	Add the ns_tmp_xxx methods to enable the tmp_prop.
 * 	ns_register_tmplink only acts on dirs now.
 * 	[92/06/24  17:01:22  jms]
 * 	Eliminated ambiguity with local member "lock".
 * 	[92/05/10  01:24:08  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:17:27  dpj]
 * 
 * Revision 2.2  91/11/06  14:09:59  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:05:16  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:44:48  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:06:42  pjg]
 * 
 * Revision 2.4  91/05/05  19:28:24  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:56  dpj]
 * 
 * 	Reworked to support explicit link count, temporary and strong links. 
 * 	[91/04/28  10:26:16  dpj]
 * 
 * Revision 2.3  90/11/10  00:38:37  dpj
 * 	Replaced ns_set_attributes() with ns_set_times().
 * 	[90/11/08  22:19:16  dpj]
 * 
 * 	Added ns_set_attributes method.
 * 	[90/10/24  15:32:17  neves]
 * 
 * Revision 2.2  89/10/30  16:37:09  dpj
 * 	First version.
 * 	[89/10/27  19:28:59  dpj]
 * 
 *
 */

#include <vol_agency_ifc.h>

extern "C" {
#include <io_types.h>
}

#define BASE std_prot
DEFINE_CLASS(vol_agency);

/*
 * Class methods
 */

void vol_agency::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(vol_agency);
	SETUP_METHOD_WITH_ARGS(vol_agency,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(vol_agency,ns_set_times);
	END_SETUP_METHOD_WITH_ARGS;
}

vol_agency::vol_agency() : link_count(0)
{
	mutex_init(&Local(lock));
}

vol_agency::vol_agency(ns_mgr_id_t mgr_id, access_table *access_tab)
	:
	std_prot(mgr_id, access_tab),
	link_count(0)
{
	mutex_init(&Local(lock));
}

mach_error_t vol_agency::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

	*attrlen = sizeof(*attr) / sizeof(int);
	attr->version = NS_ATTR_VERSION;
	attr->valid_fields = (NS_ATTR_TYPE|NS_ATTR_ID|NS_ATTR_NLINKS);
	attr->type = NST_INVALID;
	attr->mgr_id = this->mgr_id;
	/*
	 * Try to get an object ID that will not conflict with
	 * anything else likely to be used in the same server...
	 */
	attr->obj_id = 0x10000000 | (ns_obj_id_t) this;

	CLEAR_TV(attr->access_time);	/* XXX */
	CLEAR_TV(attr->modif_time);	/* XXX */
	CLEAR_TV(attr->creation_time);	/* XXX */
	INT_TO_IO_SIZE(0,&attr->size);

	ret = ns_get_protection_ltd(&attr->prot_ltd);
	if (ret == ERR_SUCCESS) {
		attr->valid_fields |= NS_ATTR_PROT;
	}

	mutex_lock(&Local(lock));
	attr->nlinks = Local(link_count);
	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}

mach_error_t 
vol_agency::ns_set_times(time_value_t atime, time_value_t mtime)
{
	return(ERR_SUCCESS);
}


mach_error_t 
vol_agency::ns_register_tmplink(dir* parent, int tag)
{
	mutex_lock(&Local(lock));
	Local(link_count)++;
	mutex_unlock(&Local(lock));

	mach_object_reference(this);

	return(ERR_SUCCESS);
}


mach_error_t vol_agency::ns_unregister_tmplink(int tag)
{
	mutex_lock(&Local(lock));
	Local(link_count)--;
	mutex_unlock(&Local(lock));

	mach_object_dereference(this);

	return(ERR_SUCCESS);
}


mach_error_t vol_agency::ns_reference_tmplink()
{
	mach_object_reference(this);

	return(ERR_SUCCESS);
}


mach_error_t vol_agency::ns_register_stronglink()
{
	mutex_lock(&Local(lock));
	Local(link_count)++;
	mutex_unlock(&Local(lock));

	mach_object_reference(this);

	return(ERR_SUCCESS);
}


mach_error_t vol_agency::ns_unregister_stronglink()
{
	mutex_lock(&Local(lock));
	Local(link_count)--;
	mutex_unlock(&Local(lock));

	mach_object_dereference(this);

	return(ERR_SUCCESS);
}

/*
 * Dummies for temporary object state call backs.
 */
mach_error_t vol_agency::ns_tmp_last_link()		{return(ERR_SUCCESS);}
mach_error_t vol_agency::ns_tmp_last_chance()		{return(ERR_SUCCESS);}
mach_error_t vol_agency::ns_tmp_cleanup_for_shutdown()	{return(ERR_SUCCESS);}

// Work-around bug in g++
mach_error_t 
vol_agency::ns_check_access(ns_access_t access, std_cred *cred_obj)
{
	DEBUG0(TRUE, (0, "vol_agency::ns_check_access: this=%x\n", this));
	return std_prot::ns_check_access(access, cred_obj);
}

