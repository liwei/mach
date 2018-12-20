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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tmp_dir.cc,v $
 * Author: J. Mark Stevenson
 *
 * Purpose: temporary directory object.  Will dissapear from a parent directory
 *		when it nolonger has any agents, stronglinks, or entries.
 *
 * HISTORY:
 * $Log:	tmp_dir.cc,v $
 * Revision 2.3  94/07/07  17:24:54  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:29:29  dpj
 * 	Initial tmp_dir.  This is a directory which automatically/atomically
 * 	disappears when it nolonger has agents/stronglinks/entries just as a
 * 	tmp_agency goes away when it has not agents/stronglinks.
 * 
 * 	Implemented using the "tmp_prop" and overiding some additional "dir" methods
 * 	where needed.
 * 	[92/06/24  16:22:23  jms]
 * 
 */

#ifndef lint
char * tmp_dir_rcsid = "$Header: tmp_dir.cc,v 2.3 94/07/07 17:24:54 mrt Exp $";
#endif	lint

#include <tmp_dir_ifc.h>

extern "C" {
#include <ns_types.h>
}

#define BASE dir
DEFINE_CLASS(tmp_dir);
DEFINE_CASTDOWN(tmp_dir, BASE);
DEFINE_TMP_PROP(tmp_dir, tmp_prop_obj)

void tmp_dir::init_class(usClass* class_obj)
{
    dir::init_class(class_obj);

    BEGIN_SETUP_METHOD_WITH_ARGS(tmp_dir);
    SETUP_METHOD_WITH_ARGS(tmp_dir,ns_get_attributes);
    SETUP_METHOD_WITH_ARGS(tmp_dir,ns_remove_entry);
    END_SETUP_METHOD_WITH_ARGS;
}

tmp_dir::tmp_dir(ns_mgr_id_t mgr_id, access_table *access_table_obj)
	:
	dir(mgr_id, access_table_obj)
{
}

tmp_dir::~tmp_dir()
{
}

mach_error_t tmp_dir::ns_tmp_last_link()
{
    if (0 != dir_entry_count()) {
	return(NS_DIR_NOT_EMPTY);
    }
    return(ERR_SUCCESS);
}
mach_error_t tmp_dir::ns_tmp_last_chance()	{return(ns_tmp_last_link());}

mach_error_t tmp_dir::ns_remove_entry(ns_name_t name)
{
    mach_error_t	ret;

    ret = dir::ns_remove_entry(name);
    if (ERR_SUCCESS != ret) {
	return(ret);
    }

    if (ERR_SUCCESS ==  tmp_prop_obj.ns_reference_tmplink()) {
	/* Not dead yet */
	mach_object_dereference(this);
    }

    return(ERR_SUCCESS);
}

mach_error_t tmp_dir::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
    mach_error_t	ret;

    ret = dir::ns_get_attributes(attr, attrlen);
    if (ERR_SUCCESS != ret) return(ret);

    ret = tmp_prop_obj.ns_get_nlinks_attribute(attr, attrlen);

    return(ret);
}
