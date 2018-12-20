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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/vn_tsymlink.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: FS-based transparent symlink
 *
 * HISTORY
 * $Log:	vn_tsymlink.cc,v $
 * Revision 2.3  94/07/07  17:26:05  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:32:17  dpj
 * 	First working version.
 * 	[92/06/24  17:28:23  dpj]
 * 
 * Revision 2.2  91/05/05  19:28:17  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:49  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:25:25  dpj]
 * 
 */

#ifndef lint
char * vn_tsymlink_rcsid = "$Header: vn_tsymlink.cc,v 2.3 94/07/07 17:26:05 mrt Exp $";
#endif	lint

#include	<vn_tsymlink_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<fs_types.h>
}

#define	BASE	vn_symlink
DEFINE_CLASS(vn_tsymlink);

void vn_tsymlink::init_class(usClass* class_obj)
{
	usName::init_class(class_obj);
	vn_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(vn_tsymlink);
	SETUP_METHOD_WITH_ARGS(vn_tsymlink,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(vn_tsymlink,ns_read_forwarding_entry);
	END_SETUP_METHOD_WITH_ARGS;

}

vn_tsymlink::vn_tsymlink()
{
}

vn_tsymlink::vn_tsymlink(
	fs_id_t			fsid,
	fs_access*		fs_access_obj,
	ns_mgr_id_t		mgr_id,
	access_table*		access_table_obj,
	vn_mgr*			vn_mgr_obj)
	:
	vn_symlink(fsid,fs_access_obj,mgr_id,access_table_obj,vn_mgr_obj)
{
}

mach_error_t vn_tsymlink::ns_get_attributes(
	ns_attr_t		attr,
	int*			attrlen)
{
	mach_error_t		ret;

	ret = vn_symlink::ns_get_attributes(attr,attrlen);
	attr->type = NST_TRANSPARENT_SYMLINK;

	return(ret);
}


mach_error_t vn_tsymlink::ns_read_forwarding_entry(
	usItem**		newobj,		/* out */
	ns_path_t		newpath)	/* out */
{
	mach_error_t		ret;
	ns_path_t		internal_path;

	ret = vn_symlink::ns_read_forwarding_entry(newobj,internal_path);
	if (ret != ERR_SUCCESS) {
		newpath[0] = '\0';
		return(ret);
	}

	if (! bcmp(internal_path," TRANSPARENT ",13)) {
		us_internal_error("Corrupted transparent symlink",
							US_INTERNAL_ERROR);
		newpath[0] = '\0';
		return(US_INTERNAL_ERROR);
	}

	strcpy(newpath,&internal_path[13]);

	return(ERR_SUCCESS);
}


