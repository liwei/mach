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
 * ObjectClass: vn_symlink
 *
 *	Agency for symbolic links in a vnode-based file server.
 *
 * SuperClass: vn_agency
 *
 * Delegated Objects
 *
 * ClassMethods:
 *	Exported:
 *
 *	ns_read_forwarding_entry: return the contents of a symlink.
 *
 *	Internal:
 *
 *	vn_setup_agency: initialize the object.
 *
 * Notes:
 *
 *	This class implements all the operations for symlink objects.
 */ 
/*
 *
 * HISTORY:
 * $Log:	vn_symlink_ifc.h,v $
 * Revision 2.4  94/07/07  17:26:04  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  94/05/17  14:08:28  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/28  18:56:22  jms]
 * 
 * Revision 2.2  92/07/05  23:32:15  dpj
 * 	First working version.
 * 	[92/06/24  17:28:11  dpj]
 * 
 * Revision 2.3  91/07/01  14:13:13  jms
 * 	Convert to using vn_reference.
 * 	[91/06/07  10:41:23  roy]
 * 	ns_reference=>vn_reference
 * 	[91/06/24  17:37:54  jms]
 * 
 * Revision 2.2  90/12/21  14:14:44  jms
 * 	Initial revision.
 * 	[90/12/15  15:15:03  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:22:40  jms]
 * 
 * Revision 2.1  90/12/15  15:14:49  roy
 * Created.
 * 
 */

#ifndef	_vn_symlink_ifc_h
#define	_vn_symlink_ifc_h

#include <us_name_ifc.h>
#include <vn_agency_ifc.h>

class vn_mgr;

class vn_symlink: public vn_agency, public usName {
      public:
	DECLARE_MEMBERS(vn_symlink);
				vn_symlink();
				vn_symlink(
					fs_id_t,
					fs_access*,
					ns_mgr_id_t,
					access_table*,
					vn_mgr*);
	virtual char*		remote_class_name() const;

REMOTE	virtual mach_error_t ns_read_forwarding_entry(usItem**, char*);

	/*
	 * Routines that do not exist for a symlink.
	 */
	virtual mach_error_t ns_resolve(char* a, ns_mode_t b, ns_access_t c, 
					usItem** d, ns_type_t* e, char* f, 
					int* g, ns_action_t* h)
			{ return _notdef(); }
	virtual mach_error_t ns_create(char* a, ns_type_t b, ns_prot_t c,
					int d, ns_access_t e, usItem** f)
			{ return _notdef(); }
	virtual mach_error_t ns_create_anon(ns_type_t a, ns_prot_t b, int c,
					ns_access_t d, char* e, usItem** f)
			{ return _notdef(); }
	virtual mach_error_t ns_create_transparent_symlink(ns_name_t a,
					ns_prot_t b, int c, char* d)
			{ return _notdef(); }
	virtual mach_error_t ns_insert_entry(char* a, usItem* b)
			{ return _notdef(); }
	virtual mach_error_t ns_insert_forwarding_entry(char* a, ns_prot_t b,
					int c, usItem* d, char* e)
			{ return _notdef(); }
	virtual mach_error_t ns_remove_entry(char* a)
			{ return _notdef(); }
	virtual mach_error_t ns_rename_entry(char* a, usItem* b, char* c)
			{ return _notdef(); }
	virtual mach_error_t ns_list_entries(ns_mode_t a, ns_name_t** b,
					     unsigned int* c, ns_entry_t* d,
					     unsigned int* e)
			{ return _notdef(); }
	virtual mach_error_t ns_list_types(ns_type_t** a, int* b)
			{ return _notdef(); }
	virtual mach_error_t ns_allocate_unique_name(char* a)
			{ return _notdef(); }

};

#endif	_vn_symlink_ifc_h

