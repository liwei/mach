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
 * ObjectClass: fs_cred
 *	Server-side object that is used to define the unix specific 
 *	protection for an object.
 *
 * SuperClass: base
 *
 * Delegated Objects: none
 *
 * ClassMethods:
 *	Exported: ns_setup_access(), ns_get_cred(), ns_get_priviledged_id()
 *	fs_get_cred_ptr()
 *
 *	//ns_setup_std_access_like(), 
 *	ns_store_acl(), ns_fetch_acl(), ns_fetch_credentials(),
 *	ns_check_access(), ns_list_access()//
 *
 *	Internal: none
 *
 * Notes:
 *	The fs_cred object is matched against an object's protection
 *	when creating a new agent to determine what methods the
 *	user is allowed to perform.  Its arguments are unix specific,
 *	as an optimiziation for servers and clients who both understand
 *	and use the Unix protection model.
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/fs_cred_ifc.h,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	fs_cred_ifc.h,v $
 * Revision 2.3  94/07/07  17:23:23  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:27:33  dpj
 * 	Derive from std_cred instead of usTop.
 * 	Use fs_auth instead of std_auth.
 * 	[92/06/24  16:22:48  dpj]
 * 
 * 	Initial version from dpj_5
 * 	[89/01/12  18:17:46  dpj]
 * 
 * Revision 2.1  91/09/27  14:51:44  pjg
 * Created.
 * 
 * Revision 2.3  89/06/30  18:34:25  dpj
 * 	Added ns_get_cred_ptr(), ns_get_principal_id().
 * 	[89/06/29  00:28:54  dpj]
 * 
 * Revision 2.2  89/03/17  12:41:18  sanzi
 * 	fs_export.h -> fs_types.h.
 * 	[89/02/15  21:50:03  dpj]
 * 	
 * 	Added a field to store a reference to the authenticator responsible
 * 	for this object.
 * 	[89/02/08  15:16:44  dpj]
 * 	
 * 	Created.
 * 	[89/02/07  17:14:53  dorr]
 * 	
 * 	Created.
 * 	[89/02/07  09:44:42  dorr]
 * 	
 * 	Last checkin before Xmas break
 * 	[88/12/14  20:55:33  dpj]
 * 	
 * 	Checkpoint to let people look at this
 * 	[88/12/12  10:14:50  dpj]
 * 
 */

#ifndef	_fs_cred_ifc_h
#define	_fs_cred_ifc_h

#include	<std_cred_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<fs_types.h>
}

class fs_auth;

class fs_cred: public std_cred {
	fs_auth*		authenticator;
	struct fs_cred_data	fs_cred_struct;
	int			groups_count;
	ns_cred_t		ns_cred;
	int			ns_cred_len;
      public:
	DECLARE_LOCAL_MEMBERS(fs_cred);
//				fs_cred();
				fs_cred(
				fs_auth*,
				ns_cred_t,
				unsigned int,
				fs_cred_t);
	virtual			~fs_cred();
	virtual	mach_error_t	ns_get_cred(ns_cred_t,int*);
	virtual	mach_error_t	ns_get_cred_ptr(ns_cred_t*);
	virtual	mach_error_t	ns_get_principal_id(ns_authid_t*);

	mach_error_t		fs_translate_token(ns_token_t,fs_cred**);
	mach_error_t		ns_translate_token(ns_token_t,std_cred**);
	mach_error_t		fs_get_cred_ptr(fs_cred_t*);
};

#endif	_fs_cred_ifc_h

