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
 *
 * Purpose: Agency for symbolic links for vnode-based servers.
 *
 * HISTORY:
 * $Log:	vn_symlink.cc,v $
 * Revision 2.3  94/07/07  17:26:02  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:32:13  dpj
 * 	First working version.
 * 	[92/06/24  17:27:59  dpj]
 * 
 * Revision 2.3  91/07/01  14:13:11  jms
 * 	Convert to using vn_reference.
 * 	[91/06/07  10:40:59  roy]
 * 
 * 	Changed aot_change_state call to aot_set_state.
 * 	[91/06/06  17:03:22  roy]
 * 	ns_reference=>vn_reference
 * 	[91/06/24  17:37:26  jms]
 * 
 * Revision 2.2  90/12/21  14:15:02  jms
 * 	Initial revision.
 * 	(Equivalent to old fs_symlink object.)
 * 	[90/12/15  15:14:21  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:22:18  jms]
 * 
 * Revision 2.1  90/12/15  15:13:47  roy
 * Created.
 * 
 */

#include	<vn_symlink_ifc.h>
#include	<vn_mgr_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<fs_types.h>
}

DEFINE_CLASS_MI(vn_symlink);
DEFINE_CASTDOWN2(vn_symlink,usName,vn_agency);

void vn_symlink::init_class(usClass* class_obj)
{
	usName::init_class(class_obj);
	vn_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(vn_symlink);
	SETUP_METHOD_WITH_ARGS(vn_symlink,ns_read_forwarding_entry);
	END_SETUP_METHOD_WITH_ARGS;

}


char* vn_symlink::remote_class_name() const
{
	return "usName_proxy";
}

vn_symlink::vn_symlink()
{
}

vn_symlink::vn_symlink(
	fs_id_t			fsid,
	fs_access*		fs_access_obj,
	ns_mgr_id_t		mgr_id,
	access_table*		access_table_obj,
	vn_mgr*			vn_mgr_obj)
	:
	vn_agency(fsid,fs_access_obj,mgr_id,access_table_obj,vn_mgr_obj)
{
}


/*
 * Return the contents of a forwarding entry.
 * The newobj returned is always unauthenticated.
 */
mach_error_t vn_symlink::ns_read_forwarding_entry(
	usItem**		newobj,
	char*			newpath)
{
	mach_error_t		ret;
	unsigned int		len;
	DECLARE_UIO(uio1);
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
	*newobj = NULL;

	SETUP_UIO(uio1,newpath,sizeof(ns_path_t),0);
	ret = fss_readlink(fsid,&uio1,fs_cred_ptr);

	len = sizeof(ns_path_t) - uio1.uio_resid;
	newpath[len] = '\0';

	return(ret);
}


