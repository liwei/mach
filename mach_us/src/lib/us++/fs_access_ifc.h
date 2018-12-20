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
 * ObjectClass: fs_access
 *
 *	Object handling all protection operations for FS-based systems.
 *
 * SuperClass: base
 *
 * Delegated Objects:
 *
 * ClassMethods:
 *	Exported: 
 *
 *	fs_authid_to_uid: convert standard authentication ID to FS-style uid.
 *	fs_authid_to_gid: convert standard authentication ID to FS-style gid.
 *	fs_uid_to_authid: convert FS-style uid to standard authentication ID.
 *	fs_gid_to_authid: convert FS-style gid to standard authentication ID.
 *	fs_is_root: return whether or not uid has root priveledges
 *	fs_convert_access_fs2ns: convert FS-format access rights into
 *		standard format.
 *	fs_convert_access_ns2fs: convert standard-format access rights into
 *		FS format.
 *	fs_convert_type_fs2ns: convert FS type designation into standard
 *		designation.
 *	fs_convert_prot_ns2fs: convert standard protection info into
 *		FS attributes.
 *	fs_convert_prot_f2ns: convert protection info from FS attributes
 *		into standard format.
 *	fs_check_access_from_data: match FS-style protection against
 *		FS-style credentials.
 *	ns_get_privileged_id: return the authentication ID that has full
 *		privileges for the agencies managed by this object.
 *
 * Notes:
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/fs_access_ifc.h,v $
 *
 * HISTORY:
 * $Log:	fs_access_ifc.h,v $
 * Revision 2.5  94/07/07  17:23:17  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/06/29  14:56:49  mrt
 * 	Fixed code to support access to read only mounted devices.
 * 	[94/06/29  13:51:34  grm]
 * 
 * Revision 2.3  92/07/05  23:27:24  dpj
 * 	Defined as a LOCAL_CLASS.
 * 	Changed some signed ints to unsigned ints and vice-versa.
 * 	[92/06/24  16:17:56  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:10:33  dpj]
 * 
 * 	add fs_is_root().
 * 	[90/01/02  14:14:45  dorr]
 * 
 * Revision 2.2  91/11/06  13:46:29  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:51:05  pjg]
 * 
 * Revision 2.4.1.1  91/04/14  18:27:33  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.4  90/12/19  11:04:48  jjc
 * 	Added fs_is_priv() which checks to see if the user is root or in a
 * 	privileged group.
 * 	[90/10/29            jjc]
 * 
 * Revision 2.3  90/01/02  22:12:28  dorr
 * 	add fs_is_root.
 * 
 * Revision 2.2  89/10/30  16:32:10  dpj
 * 	First version.
 * 	[89/10/27  17:31:16  dpj]
 * 
 */

#ifndef	_fs_access_ifc_h
#define	_fs_access_ifc_h

#include <top_ifc.h>

extern "C" {
#include <ns_types.h>
#include <fs_types.h>
}

class fs_access: public usTop {
	hash_table_t		authid_to_uid;
	hash_table_t		authid_to_gid;
	hash_table_t		uid_to_authid;
	hash_table_t		gid_to_authid;
	ns_authid_t		priv_authid;
	ns_authid_t		root_authid;
	int			priv_uid;
	int			priv_gid;
	int			dev_mode;

      public:
	DECLARE_LOCAL_MEMBERS(fs_access);
	                fs_access();
			fs_access(int);
	virtual		~fs_access();
	mach_error_t	fs_set_authid_map(char*, unsigned int);
	mach_error_t 	fs_authid_to_uid(ns_authid_t, int*);
	mach_error_t	fs_authid_to_gid(ns_authid_t, int*);
	mach_error_t	fs_uid_to_authid(int, ns_authid_t*);
	mach_error_t	fs_gid_to_authid(int, ns_authid_t*);
	ns_access_t	fs_convert_access_fs2ns(int);
	mach_error_t	fs_is_root(ns_authid_t);
	int		fs_convert_access_ns2fs(ns_access_t);
	mach_error_t	fs_convert_prot_ns2fs(
				ns_prot_t,
				unsigned int,
				fs_attr_t);
	mach_error_t	fs_convert_prot_fs2ns(
				fs_attr_t,
				ns_prot_t,
				unsigned int*);
	mach_error_t	fs_check_access_from_data(
				fs_attr_t,
				ns_access_t,
				fs_cred_t);
	mach_error_t	ns_get_privileged_id(ns_authid_t*);
	mach_error_t	fs_is_priv(ns_cred_t);
};

#endif	_fs_access_ifc_h

