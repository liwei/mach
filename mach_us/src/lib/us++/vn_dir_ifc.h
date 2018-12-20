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
 * ObjectClass: vn_dir
 *
 *	Agency for directories in a vnode-based file server.
 *
 * SuperClass: fs_agency
 *
 * Delegated Objects
 *
 * ClassMethods:
 *	Exported:
 *
 *	All the ns_ methods dealing with directories.
 *
 *	Internal:
 *
 *	fs_fsid_export: export an agent for a given FS ID.
 *
 * Notes:
 *
 *	This class implements all the operations for directory objects.
 */ 
/*
 * HISTORY:
 * $Log:	vn_dir_ifc.h,v $
 * Revision 2.5  94/07/07  17:25:50  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/06/29  14:56:55  mrt
 * 	Added mount option to the new fs_access.
 * 	[94/06/29  13:53:40  grm]
 * 
 * Revision 2.3  94/05/17  14:08:18  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/28  18:55:40  jms]
 * 
 * Revision 2.2  92/07/05  23:31:55  dpj
 * 	First working version.
 * 	[92/06/24  17:26:25  dpj]
 * 
 * Revision 2.1  91/09/27  15:02:05  pjg
 * Created.
 * 
 * Revision 2.4  91/07/01  14:12:58  jms
 * 	Convert to using vn_reference.
 * 	[91/06/07  10:42:25  roy]
 * 	ns_reference=>vn_reference
 * 	[91/06/24  17:34:52  jms]
 * 
 * Revision 2.3  91/05/05  19:28:12  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:42  dpj]
 * 
 * 	Support transparent symlinks.
 * 	[91/04/28  10:24:46  dpj]
 * 
 * Revision 2.2  90/12/21  14:14:34  jms
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:21:05  jms]
 * 
 * Revision 2.1  90/12/15  15:13:20  roy
 * Created.
 * 
 * 
 */

#ifndef	_vn_dir_ifc_h
#define	_vn_dir_ifc_h

#include	<us_name_ifc.h>
#include	<vn_agency_ifc.h>


class vn_dir: public vn_agency, public usName {
	mach_error_t		fsint_setup_list(
					int,
					vm_address_t,
					vm_address_t,
					int,
					vm_address_t*,
					vm_address_t*);
	void			fsint_cleanup_list(
					int,
					int,
					vm_address_t,
					vm_address_t);
	mach_error_t		fs_fsid_export(
				   	fs_id_t,
					ns_type_t,
				 	ns_access_t,
					fs_cred*,
					usItem**);

      public:
	DECLARE_MEMBERS(vn_dir);
				vn_dir();
				vn_dir(fs_id_t,fs_access*,ns_mgr_id_t,
					access_table*,vn_mgr*);
				vn_dir(fs_id_t,ns_mgr_id_t,int,mach_error_t*);
	virtual char*		remote_class_name() const;

	/*
	 * Methods exported remotely
	 */
REMOTE	virtual mach_error_t ns_resolve(ns_path_t, ns_mode_t, ns_access_t, 
					usItem** , ns_type_t*, ns_path_t, 
					int*, ns_action_t*);
REMOTE	virtual mach_error_t ns_create(ns_name_t, ns_type_t, ns_prot_t, int,
				       ns_access_t, usItem**);
REMOTE	virtual mach_error_t ns_create_transparent_symlink(ns_name_t, ns_prot_t,
							  int, char *);
REMOTE	virtual mach_error_t ns_insert_entry(char*, usItem*);
REMOTE	virtual mach_error_t ns_insert_forwarding_entry(ns_name_t, ns_prot_t, 
							int, usItem*, char*);
REMOTE	virtual mach_error_t ns_remove_entry(ns_name_t);
REMOTE	virtual mach_error_t ns_list_entries(ns_mode_t, ns_name_t**,
					     unsigned int*, ns_entry_t*,
					     unsigned int*);
REMOTE	virtual mach_error_t ns_list_types(ns_type_t**, int*);

	/*
	 * From usName but not implemented.
	 */
REMOTE	virtual mach_error_t ns_rename_entry(ns_name_t a, 
						usItem* b, ns_name_t c)
				{ return _notdef(); }
REMOTE	virtual mach_error_t ns_read_forwarding_entry(usItem**, char*)
				{ return _notdef(); }
REMOTE	virtual mach_error_t ns_create_anon(unsigned int a, ns_prot_t b,
				int c, unsigned int d, char* e, usItem** f)
				{ return _notdef(); }
REMOTE	virtual mach_error_t ns_allocate_unique_name(char* a)
				{ return _notdef(); }

};


#endif	_vn_dir_ifc_h

