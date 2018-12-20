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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_auth.cc,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	std_auth.cc,v $
 * Revision 2.5  94/07/07  17:24:19  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/05  23:28:39  dpj
 * 	Fixed compiler warnings.
 * 	[92/06/24  17:06:52  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:11:43  dpj]
 * 
 * Revision 2.3  92/03/05  15:05:41  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:32:44  jms]
 * 
 * Revision 2.2  91/11/06  13:47:37  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:07:32  pjg]
 * 
 * 	Upgraded to US38
 * 	k<<<log message for ./lib/us/std_auth.cc>>>
 * 	[91/04/14  18:30:13  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:39:39  pjg]
 * 
 * Revision 2.5  90/12/19  11:05:24  jjc
 * 	Changed std_auth_ns_create_identity() to call as_create_token_group()
 * 	which is the same as as_create_token() except that it will 
 * 	create any nonexistent groups that it encounters.
 * 	[90/12/05            jjc]
 * 	Changed ns_create_identity() to call as_login_create() to
 * 	login instead of as_login_priv().  as_login_create() will
 * 	create the user if he/she doesn't exist.
 * 	[90/10/09            jjc]
 * 
 * Revision 2.4  89/07/09  14:19:36  dpj
 * 	Fixed debugging statements for new DEBUG and ERROR macros.
 * 	[89/07/08  12:58:53  dpj]
 * 
 * Revision 2.3  89/05/17  16:43:20  dorr
 * 	add std_auth_create_identity().
 * 	[89/05/15  12:16:19  dorr]
 * 
 * Revision 2.2  89/03/17  12:48:49  sanzi
 * 	Changed auth_anon_id to 2 (was 1).
 * 	[89/02/24  18:39:58  dpj]
 * 	
 * 	fix ns_setup_std_cred stuff.
 * 	[89/02/15  15:36:10  dorr]
 * 	
 * 	Created.
 * 	[89/02/13  15:32:34  dorr]
 * 
 */

#ifndef lint
char * std_auth_rcsid = "$Header: std_auth.cc,v 2.5 94/07/07 17:24:19 mrt Exp $";
#endif	lint


#include <std_auth_ifc.h>

extern "C" {
#include	<auth_defs.h>
#include	<servers/auth.h>
#include	<servers/netname.h>
}

#define BASE usTop
DEFINE_LOCAL_CLASS(std_auth);

/*
 * Anonymous ID for this authenticator.
 */
static int	auth_anon_id = 2;

std_auth::std_auth()
{
	mach_error_t	ret;
	int			cred_data[NS_CRED_LEN(2)];
	ns_cred_t		cred = (ns_cred_t) cred_data;
	
	cred->head.version = NS_CRED_VERSION;
	cred->head.cred_len = 1;
	cred->authid[0] = auth_anon_id;

	//  new_object(anon_cred,std_cred);
	anon_cred = new std_cred(this, cred, NS_CRED_LEN(1), &ret);
	if (ret != ERR_SUCCESS) {
		mach_error("std_auth_initialize.setup_std_cred(anon)",ret);
	}

	ret = netname_look_up(name_server_port,"",LAS_NAME,&as_port);
	if (ret != ERR_SUCCESS) {
		ret = netname_look_up(name_server_port,"", CAS_NAME, &as_port);
		if (ret != ERR_SUCCESS) {
			mach_error(
			   "std_auth: cannot find authentication server",ret);
			as_port = MACH_PORT_NULL;
		}
	}
}

std_auth::~std_auth()
{
	mach_object_dereference(anon_cred);
	(void) mach_port_deallocate(mach_task_self(),as_port);
}

/*
 * Find a std_cred object corresponding to the specified
 * authentication token.
 *
 * XXX This call currently contacts the authentication server each time.
 * We should keep a cache of previously seen tokens.
 */
mach_error_t 
std_auth::ns_translate_token(ns_token_t token, std_cred **newobj)
{
	mach_error_t		ret;
	group_id_t		user_id;
	group_id_list_t		group_ids;
	unsigned int		count;
	int			xfer_count;
	int			cred_data[NS_CRED_LEN(16)];
	ns_cred_t		cred = (ns_cred_t) cred_data;
	int			i;

	if ((token == MACH_PORT_NULL) || (as_port == MACH_PORT_NULL)) {
		/*
		 * Anonymous access. Get default credentials.
		 */
		mach_object_reference(anon_cred);
		*newobj = anon_cred;
		return(ERR_SUCCESS);
	}

	ret = as_verify_token_ids(as_port,token,&user_id,&group_ids,&count);
	if (ret != ERR_SUCCESS) {
		*newobj = 0;
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

	for (i = 0; i < xfer_count; i++) {
		cred->authid[i + 1] = group_ids[i];
	}

	(void)vm_deallocate(mach_task_self(),(vm_offset_t)group_ids,count * sizeof(group_id_t));

	*newobj = new std_cred(this, cred, NS_CRED_LEN((xfer_count + 1)), &ret);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(*newobj);
		*newobj = NULL;
		return(ret);
	}
		
	return(ERR_SUCCESS);
}


/*
 * ns_create_identity(): return an identifier and a token, representing
 * the desired set of credentials
 */
mach_error_t 
std_auth::ns_create_identity(std_ident* me, ns_cred_t cred, std_ident** as_id)
{
	mach_error_t			err;
	mach_port_t			private_port;
	mach_port_t			my_ident;
	mach_port_t			token;

	/*
	 * generate a fresh private port.  this will only succeed
	 * if the user is in the sys_adm group.
	 */
	err = me->ns_get_identity(&my_ident);
	if (err) goto finish;

	DEBUG0(TRUE,(0, "ns_create_identity: me=%d them=%d,%d len=%d\n",
		my_ident, cred->authid[0], cred->authid[1], 
		cred->head.cred_len));

	err = as_login_create(as_port,my_ident,(group_id_list_t) cred->authid,
			      cred->head.cred_len,&private_port);
	if (err) {
		mach_error("as_login_create", err);
		goto finish;
	}

	err = as_create_token_group(as_port,private_port,
				    (group_id_list_t) &cred->authid[1],
				    cred->head.cred_len-1,&token);
	if (err) {
		mach_error("as_create_token", err);
		goto finish;
	}

//	new_object(*as_id,std_ident);
//	err = ns_setup_identity(*as_id,private_port,token);
	*as_id = new std_ident(private_port, token);

    finish:
	if (err != ERR_SUCCESS)
		mach_error("ns_create_identity", err);
	return(err);
}



