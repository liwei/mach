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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/fs_auth.cc,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	fs_auth.cc,v $
 * Revision 2.3  94/07/07  17:23:18  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:27:27  dpj
 * 	Defined as a LOCAL_CLASS.
 * 	Replaced fs_cred with fs_cred_data to keep new compiler happy.
 * 	Converted to new IPC.
 * 	[92/06/24  16:20:10  dpj]
 * 
 * Revision 2.1  91/09/27  11:34:14  pjg
 * Created.
 * 
 * Revision 2.5  90/12/19  11:05:05  jjc
 * 	Fixed fs_auth_ns_translate_token() to setup groups list
 * 	correctly.
 * 	[90/11/30            jjc]
 * 
 * Revision 2.4  89/10/30  16:32:26  dpj
 * 	Use fs_access for authid translations.
 * 	[89/10/27  17:34:54  dpj]
 * 
 * Revision 2.3  89/07/09  14:18:58  dpj
 * 	Fixed calls to MACH_ERROR -> mach_errror.
 * 	[89/07/08  12:54:28  dpj]
 * 
 * Revision 2.2  89/03/17  12:40:37  sanzi
 * 	Use method invocation instead of direct procedure calls to access
 * 	internal methods in this class.
 * 	[89/02/16  17:21:45  dpj]
 * 	
 * 	Renamed a few fs_cred variables to avoid conflicts with the
 * 	fs_cred factory object.
 * 	fs_export.h -> fs_types.h.
 * 	[89/02/15  22:01:36  dpj]
 * 	
 * 	Fixed length of credentials structure passed into fs_cred objects.
 * 	[89/02/13  16:15:52  dpj]
 * 	
 * 	Reorganized translations for UNIX-like anonymous and privileged IDs.
 * 	Fixed credlen argument for setting up new cred objects.
 * 	[89/02/12  20:48:36  dpj]
 * 	
 * 	uid's are shorts.
 * 	[89/02/10  13:56:34  dorr]
 * 	
 * 	fix the size of cred structs; uid's are shorts...be careful.
 * 	[89/02/09  18:00:00  dorr]
 * 	
 * 	fix mismatched parameter in fs_setup_fs_cred.
 * 	[89/02/08  16:59:04  dorr]
 * 	
 * 	Added the "authenticator" argument to the setup call for fs_cred.
 * 	Added a couple of methods include files.
 * 	[89/02/08  15:19:22  dpj]
 * 	
 * 	change some names around to get it to compile.
 * 	[89/02/08  11:53:42  dorr]
 * 	
 * 	fix the name of fs_auth_ns_translate
 * 	[89/02/08  09:38:08  dorr]
 * 	
 * 	first reasonable version
 * 	[89/02/07  17:13:18  dorr]
 * 	
 * 	change some names
 * 	[89/02/07  10:38:57  dorr]
 * 
 */

#ifndef lint
char * fs_auth_rcsid = "$Header: fs_auth.cc,v 2.3 94/07/07 17:23:18 mrt Exp $";
#endif	lint

#include	<fs_auth_ifc.h>
#include	<fs_cred_ifc.h>
#include	<fs_access_ifc.h>

extern "C" {
#include	<us_error.h>
#include	<servers/auth.h>
#include	<servers/netname.h>
}

#define BASE usTop
DEFINE_LOCAL_CLASS(fs_auth)


/*
 * Default values for special authentication IDs.
 *
 * These values can be patched with a debugger.
 */
ns_authid_t	default_anon_authid = 2;



//fs_auth::fs_auth()
//:
// anon_authid(default_anon_authid), as_port(MACH_PORT_NULL),
// fs_access_obj(0), anon_cred(0)
//{}


fs_auth::fs_auth(fs_access* _fs_access_obj)
:
 anon_authid(default_anon_authid), fs_access_obj(_fs_access_obj)
{
	mach_error_t		ret;
	int			cred_data[NS_CRED_LEN(2)];
	ns_cred_t		cred = (ns_cred_t) cred_data;
	struct fs_cred_data	fs_cred_rec;
	int			uid, gid;

	mach_object_reference(fs_access_obj);

	(void)fs_access_obj->fs_authid_to_uid(Local(anon_authid), &uid);
	(void)fs_access_obj->fs_authid_to_gid(Local(anon_authid), &gid);
	fs_cred_rec.cr_ruid = fs_cred_rec.cr_uid = uid;
	fs_cred_rec.cr_rgid = fs_cred_rec.cr_gid = gid;
	fs_cred_rec.cr_groups[0] = 0;
	fs_cred_rec.cr_ref = 1;

	cred->head.version = NS_CRED_VERSION;
	cred->head.cred_len = 1;
	cred->authid[0] = Local(anon_authid);

	anon_cred = new fs_cred(this, cred, NS_CRED_LEN(1), &fs_cred_rec);

	ret = netname_look_up(name_server_port,"",LAS_NAME,&Local(as_port));
	if (ret != ERR_SUCCESS) {
		ret = netname_look_up(name_server_port,"",CAS_NAME,
							&Local(as_port));
		if (ret != ERR_SUCCESS) {
			mach_error(
		"fs_auth: cannot find authentication server",ret);
			Local(as_port) = MACH_PORT_NULL;
		}
	}
}


fs_auth::~fs_auth()
{
	DESTRUCTOR_GUARD();

	mach_object_dereference(Local(fs_access_obj));
	mach_object_dereference(Local(anon_cred));
	(void) mach_port_deallocate(mach_task_self(),Local(as_port));
}


/*
 * Find a fs_cred object corresponding to the specified
 * authentication token.
 *
 * XXX This call currently contacts the authentication server each time.
 * We should keep a cache of previously seen tokens.
 */
mach_error_t  fs_auth::fs_translate_token(ns_token_t token, fs_cred **newobj)
{
	mach_error_t		ret;
	group_id_t		user_id;
	group_id_list_t		group_ids;
	unsigned int		count;
	int			xfer_count;
	struct fs_cred_data	fs_cred_rec;
	int			cred_data[NS_CRED_LEN(16)];
	ns_cred_t		cred = (ns_cred_t) cred_data;
	int			i;
	int			uid, gid;

	if ((token == MACH_PORT_NULL) || (Local(as_port) == MACH_PORT_NULL)) {
		/*
		 * Anonymous access. Get default credentials.
		 */
		mach_object_reference(Local(anon_cred));
		*newobj = Local(anon_cred);
		return(ERR_SUCCESS);
	}

	ret = as_verify_token_ids(Local(as_port),token,&user_id,&group_ids,&count);
	if (ret != ERR_SUCCESS) {
		*newobj = NULL;
		return(ret);
	}

	if (count <= 0) {
		*newobj = NULL;
		return(US_INVALID_ACCESS);
	}

	if (count > (DEFAULT_NS_CRED_LEN - 1)) {
		/*
		 * Silently ignore the extra credentials.
		 *
		 * XXX Should deal with this better.
		 */
		xfer_count = DEFAULT_NS_CRED_LEN - 1;
	} else {
		xfer_count = count;
	}

	cred->head.version = NS_CRED_VERSION;
	cred->head.cred_len = xfer_count + 1;
	cred->authid[0] = user_id;
	(void)Local(fs_access_obj)->fs_authid_to_uid(user_id, &uid);
	fs_cred_rec.cr_uid = uid;

	if (xfer_count > 0) {
		cred->authid[1] = group_ids[0];
		(void)Local(fs_access_obj)->fs_authid_to_gid(group_ids[0], &gid);
		fs_cred_rec.cr_gid = gid;
	}

	for (i = 1; i < xfer_count; i++) {

		cred->authid[i + 1] = group_ids[i];
		if (i < NGROUPS-1) {
			(void)Local(fs_access_obj)->fs_authid_to_gid(
							group_ids[i], &gid);
			fs_cred_rec.cr_groups[i-1] = gid;
		}
			
	}

	/* end of group list */
	if (xfer_count == 0)
		fs_cred_rec.cr_groups[0] = 0;
	else
		fs_cred_rec.cr_groups[xfer_count-1] = 0;
	fs_cred_rec.cr_ruid = fs_cred_rec.cr_uid;
	fs_cred_rec.cr_rgid = fs_cred_rec.cr_gid;
	fs_cred_rec.cr_ref = 1;

	(void)vm_deallocate(mach_task_self(),(vm_offset_t)group_ids,
				count * sizeof(group_id_t));

//	new_object(*newobj,fs_cred);
//	ret = fs_setup_fs_cred(*newobj,Self,cred,NS_CRED_LEN(xfer_count + 1),
//								&fs_cred_rec);
	*newobj = new fs_cred(this, cred,NS_CRED_LEN(xfer_count + 1),
			      &fs_cred_rec);
		
	return(ERR_SUCCESS);
}

