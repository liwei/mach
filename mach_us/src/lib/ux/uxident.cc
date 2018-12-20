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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxident.cc,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxident.cc,v $
 * Revision 2.5  94/07/08  16:01:50  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  94/05/17  14:08:39  jms
 * 	Need implementations of virtual methods in class uxident for 2.3.3 g++ -modh
 * 	[94/04/28  18:57:45  jms]
 * 
 * Revision 2.3.1.1  94/02/18  11:32:45  modh
 * 	Need implementations of virtual methods in class uxident for 2.3.3 g++
 * 
 * Revision 2.3  92/07/05  23:32:41  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:24:45  dpj]
 * 
 * Revision 2.2  91/11/06  14:11:40  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:14:38  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:08:13  pjg]
 * 
 * 	add set_groups, get_groups & set_regid.
 * 	create_unix_identity() takes group list along with
 * 	primary uid & gid.  make some confusing name changes
 * 	in set_re[ug]id.  
 * 	[90/01/02  14:17:35  dorr]
 * 
 * 	Initial checkin.
 * 	[89/12/28  10:59:22  dorr]
 * 
 * Revision 2.3  90/12/19  11:05:58  jjc
 * 	Fixed bug in uxident_set_groups() where I forgot to use the
 * 	new groups to create the new identities.
 * 	[90/11/30            jjc]
 * 	Changed uxident_set_re{uid,gid} to return US_OBJECT_EXISTS
 * 	if the IDs are set already, so the caller can avoid doing
 * 	any work.
 * 	[90/11/14            jjc]
 * 	Fixed create_unix_identity() not to test for uid being added
 * 	to group list, since uid and gid spaces don't overlap.
 * 	[90/09/26            jjc]
 * 	Fixed uxident_setup() to initialize groups and ngroups.
 * 	Changed to keep track of only the real and effective identity objects.
 * 	Made routines to add and remove a group from the groups list.
 * 	Changed uxident_get_groups() to return number of groups.
 * 	Cleaned up.
 * 	[90/08/29            jjc]
 * 
 * Revision 2.2  90/01/02  22:14:21  dorr
 * 	move identity related operations and state into this
 * 	object.
 * 
 *
 */

#ifndef lint
char * uxident_rcsid = "$Header: uxident.cc,v 2.5 94/07/08 16:01:50 mrt Exp $";
#endif	lint

#include "uxident_ifc.h"

extern "C" {
#include <base.h>
#include <debug.h>
#include <us_error.h>

#include <ns_types.h>

/*
#include <uxident_methods.h>
#include <cat_methods.h>
#include <ns_methods.h>
#include <ns_internal.h>
#include <fs_internal.h>
*/
}


#define	ID_RU_RG	0		/* real uid, real gid */
#define	ID_EU_EG	1		/* eff uid, eff gid */

/*
 *	define the unix protection specific operations
 */
/*
 * uxident_setup(): set up our initial identity
 */
uxident::uxident(fs_access* acc, std_auth* auth, std_ident* ai)
	: r_uid(-1), e_uid(-1), r_gid(-1), e_gid(-1), ngroups(0)
{
	int		i;

	access_obj = acc;
	mach_object_reference(access_obj);

	auth_obj = auth;
	mach_object_reference(auth_obj);

	for (i=0; i<Count(_Local(as_ids)); i++) {
		mach_object_reference(ai);
		_Local(as_ids)[i] = ai;
	}

	if (ai == 0) {
		return;
	}

	std_cred*	cred;
	ns_cred_t	credp;
	ns_token_t	token;
	mach_error_t	err;

	/* establish our real and effective id's (the same) */
	ai->ns_get_token(&token);
	err = auth_obj->ns_translate_token(token, &cred);
	if (err != ERR_SUCCESS) {
		DEBUG1(TRUE,(0,
			"setup: ns_translate_token failed: %s\n",
			(char *)mach_error_string(err)));
		return;
	}

	err = cred->ns_get_cred_ptr(&credp);
	if (err != ERR_SUCCESS) {
		DEBUG1(TRUE,(0,
			"setup: ns_get_cred_ptr failed: %s\n",
			(char *)mach_error_string(err)));
		return;
	}

	_Local(e_uid) = _Local(r_uid) = credp->authid[0];
	_Local(e_gid) = _Local(r_gid) = credp->authid[1];

	DEBUG1(TRUE,(0,"setup: uid %d, gid %d\n", _Local(r_uid),
		_Local(r_gid)));

	groups[0] = r_gid;
	ngroups = 1;
}


uxident::~uxident()
{
	int			i;

	for(i=0; i<Count(as_ids); i++)
		mach_object_dereference(as_ids[i]);

	mach_object_dereference(auth_obj);
	mach_object_dereference(access_obj);
}
/*
 *	clone_init(): insert your identity into children
 */
mach_error_t uxident::clone_init(mach_port_t child)
{
	mach_error_t		ret;
	int			i;

	for(i=0; i<Count(_Local(as_ids)); i++) {

		/* 
		 * usually, all as_ids will be the same object 
		 * do a simple test for the typical case
		 */
		if ((i > 0) && _Local(as_ids)[i] == _Local(as_ids)[i-1])
			continue;

		ret = (as_ids[i])->clone_init(child);
		if (ret != ERR_SUCCESS
		    && ret != MACH_OBJECT_NO_SUCH_OPERATION) {
			return(ret);
		}
	}

	/*
	 * XXX C++ Is this ever used, or does it always return
	 * MACH_OBJECT_NO_SUCH_OPERATION ?
	 */
//	ret = auth_obj->clone_init(child);
//	if (ret == MACH_OBJECT_NO_SUCH_OPERATION)
		ret = ERR_SUCCESS;

	return ret;
}

/*
 *	add_group:  add group to list of groups that we're a member of
 */
mach_error_t uxident::add_group(ns_authid_t gid)
{
	register int	i;
	register int	ng;

	ng =  ngroups;
	for (i = 0; i < ng; i++)
		if (groups[i] == gid)	/* in the list already */
			return(ERR_SUCCESS);
	if (ng < NGROUPS) {			/* add to end of list */
		groups[ng++] = gid;
		ngroups = ng;
		return(ERR_SUCCESS);
	}
	return(US_RESOURCE_EXHAUSTED);
}

/*
 *	remove_group:  remove group to list of groups that we're a member of
 */
mach_error_t uxident::remove_group(ns_authid_t gid)
{
	register int i, j, ng;

	ng = ngroups;
	for (i = 0; i < ng; i++)
		if (groups[i] == gid) {
			for (j = i; j < ng-1; j++)
				groups[j] = groups[j+1];
			ngroups = --ng;
			return(ERR_SUCCESS);
		}
	return(US_OBJECT_NOT_FOUND);
}

/*
 *	create_unix_identity():  fill in our credentials with our user and
 *	group authids, and then have the authorization agent make an 
 *	identity object for us.
 */
mach_error_t 
uxident::create_unix_identity(ns_authid_t uid, ns_authid_t gid, 
			       ns_authid_t *groups_p, int ngroups_p, 
			       std_ident** as_id)
{
	int			cred_data[NS_CRED_LEN(NGROUPS+1)];
	ns_cred_t		cred = (ns_cred_t)cred_data;
	register int		i;

	DEBUG1(TRUE,(0,"create_unix_id: uid=%d gid=%d id[RU_RG]=0x%x\n",
		uid, gid, as_ids[0]));

	cred->head.version = NS_CRED_VERSION;
	cred->head.cred_len = 2;
	cred->authid[0] = uid;
	cred->authid[1] = gid;

	/* tack on our ancillary groups */
	for( i=0; i < ngroups_p; i++) {
		if (groups_p[i] != gid) {
			cred->authid[cred->head.cred_len++] =
				groups_p[i];
		}

	}

	/*
	 * ask the authorization agent to create a new identity.
	 * pass to it our real identity, batman.
	 * the identity object will contain two ports, one being our private
	 * port for talking to the authentication server and the other being
	 * an authentication token.
	 */
	return auth_obj->ns_create_identity(as_ids[ID_RU_RG],cred,as_id);
}



/*
 *	uxident_set_reuid():  set the real and effective unix id's.
 *	it is always legal to set the real id to the value
 *	of the effective id, or vice versa.
 *
 *	privileged users may change their real or effective
 *	user id's, which will require generation of a new
 *	identity and deallocation of the old identity.
 *
 *	won't change IDs if creation of either effective or real ID fails.
 */
uxident::uxident_set_reuid(int ux_ruid, int ux_euid)
{
	std_ident*		as_id[2];
	mach_error_t		err;
	ns_authid_t		ruid, euid;
	ns_authid_t		o_ruid = r_uid;
	ns_authid_t		o_euid = e_uid;
	boolean_t		suser;


	/*
	 * convert uids to authids
	 */
	if (ux_ruid == -1) {
		ruid = o_ruid;
	} else {
		(void)access_obj->fs_uid_to_authid(ux_ruid, &ruid);
	}

	if (ux_euid == -1) {
		euid = o_euid;
	} else {
		(void)access_obj->fs_uid_to_authid(ux_euid, &euid);
	}

	/*
	 * nothing to do?
	 * return error code to that effect,
	 * so caller can avoid lots of work.
	 */
	if (ruid == o_ruid && euid == o_euid) {
		DEBUG1(TRUE,(0, "set_reuid: nothing to do\n"));
		return(US_OBJECT_EXISTS);
	}

	suser = access_obj->fs_is_root(r_uid) == ERR_SUCCESS;

	DEBUG1(TRUE,(0,"set_reuid: real=ux%d(%d) eff=ux%d(%d) suser %d\n",
		ux_ruid, ruid, ux_euid, euid, suser));

	/*
	 * must be swapping real and effective uids 
	 * or be root to do anything
	 */
	if (ruid != o_ruid && ruid != o_euid && !suser)
		return(US_INVALID_ACCESS);
	
	if (euid != o_ruid && euid != o_euid && !suser)
		return(US_INVALID_ACCESS);

	/*
	 * Try to create new IDs.
	 * Bail out and return an error if we can't create them.
	 */

	as_id[ID_EU_EG] = NULL;
	as_id[ID_RU_RG] = NULL;

	if (euid != o_euid) {
		err = create_unix_identity(euid, e_gid,
					    groups, ngroups,
					    &as_id[ID_EU_EG]);
		if (err != ERR_SUCCESS) {
			DEBUG1(TRUE,(0, 
			  "set_reuid: can't create effective ID: %s\n",
			  (char*)mach_error_string(err)));
			mach_object_dereference(as_id[ID_EU_EG]);
			return(err);
		}
	}
	if (ruid != o_ruid) {
		err = create_unix_identity(ruid, r_gid,
					    groups, ngroups,
					    &as_id[ID_RU_RG]);
		if (err != ERR_SUCCESS) {
			DEBUG1(TRUE,(0,
			  "set_reuid: can't create real ID: %s\n",
			  (char*)mach_error_string(err)));
			mach_object_dereference(as_id[ID_EU_EG]);
			mach_object_dereference(as_id[ID_RU_RG]);
			return(err);
		}
	}

	/*
	 * Dereference old IDs and change to new ones
	 * now that we're sure that we can create them.
	 */
	if (euid != o_euid) {
		mach_object_dereference(as_ids[ID_EU_EG]);
		as_ids[ID_EU_EG] = as_id[ID_EU_EG];
		e_uid = euid;
	}
	if (ruid != o_ruid) {
		mach_object_dereference(as_ids[ID_RU_RG]);
		as_ids[ID_RU_RG] = as_id[ID_RU_RG];
		r_uid = ruid;
	}

	return(ERR_SUCCESS);
}

/*
 *	uxident_set_regid:  set our real and effective gids
 *	won't change IDs if creation of either effective or real ID fails.
 */
mach_error_t uxident::uxident_set_regid(int ux_rgid, int ux_egid)
{
	std_ident*		as_id[2];
	mach_error_t		err;
	ns_authid_t		rgid, egid;
	ns_authid_t		o_rgid = r_gid;
	ns_authid_t		o_egid = e_gid;
	boolean_t		suser;


	/*
	 * do some quick conversions into the authid id space
	 */

	if (ux_rgid == -1) {
		rgid = o_rgid;
	} else {
		(void)access_obj->fs_gid_to_authid(ux_rgid, &rgid);
	}

	if (ux_egid == -1) {
		egid = o_egid;
	} else {
		(void)access_obj->fs_gid_to_authid(ux_egid, &egid);
	}

	/*
	 * nothing to do?
	 * return error code to that effect,
	 * so caller can avoid lots of work.
	 */
	if (rgid == o_rgid && egid == o_egid) {
		DEBUG1(TRUE,(0, "set_regid: nothing to do\n"));
		return(US_OBJECT_EXISTS);
	}
	
	suser = access_obj->fs_is_root(r_uid) == ERR_SUCCESS;

	DEBUG1(TRUE,(0,"set_regid: real=ux%d(%d) eff=ux%d(%d) suser %d\n",
		ux_rgid, rgid, ux_egid, egid, suser));

	/*
	 * must be swapping real and effective gids
	 * or be root to do anything
	 */
	if (rgid != o_rgid && rgid != o_egid && !suser)
		return(US_INVALID_ACCESS);
	
	if (egid != o_rgid && egid != o_egid && !suser)
		return(US_INVALID_ACCESS);

	/*
	 * Try to create new IDs.
	 * Bail out and return an error if we can't create them.
	 */
	as_id[ID_EU_EG] = NULL;
	as_id[ID_RU_RG] = NULL;

	if (egid != o_egid) {
		err = create_unix_identity(e_uid, egid,
					    groups, ngroups,
					    &as_id[ID_EU_EG]);
		if (err != ERR_SUCCESS) {
			DEBUG1(TRUE,(0,
			  "set_regid: can't create effective ID: %s\n",
			  (char*)mach_error_string(err)));
			mach_object_dereference(as_id[ID_EU_EG]);
			return(err);
		}
	}
	if (rgid != o_rgid) {
		err = create_unix_identity(r_uid, rgid,
					    groups, ngroups,
					    &as_id[ID_RU_RG]);
		if (err != ERR_SUCCESS) {
			DEBUG1(TRUE,(0,
			  "set_regid: can't create real ID: %s\n",
			  (char*)mach_error_string(err)));
			mach_object_dereference(as_id[ID_EU_EG]);
			mach_object_dereference(as_id[ID_RU_RG]);
			return(err);
		}
	}

	/*
	 * Dereference old IDs and change to new ones
	 * now that we're sure that we can create them.
	 */
	if (rgid != o_rgid) {
		/*
		 * Remove old real group and add new real group 
		 * to group list.
		 */
		err = remove_group(o_rgid);
		if (err != ERR_SUCCESS) {
			/*
			 * BSD Unix ignores errors.  Just print warning.
			 */
			DEBUG1(TRUE,(0, 
			  "set_regid: can't remove group: %s\n",
			  (char*)mach_error_string(err)));
		}
		err = add_group(rgid);
		if (err != ERR_SUCCESS) {
			/*
			 * BSD Unix ignores errors.  Just print warning.
			 */
			DEBUG1(TRUE,(0,
			  "set_regid: can't add group: %s\n",
			  (char*)mach_error_string(err)));
		}
		mach_object_dereference(as_ids[ID_RU_RG]);
		as_ids[ID_RU_RG] = as_id[ID_RU_RG];
		r_gid = rgid;
	}
	if (egid != o_egid) {
		mach_object_dereference(as_ids[ID_EU_EG]);
		as_ids[ID_EU_EG] = as_id[ID_EU_EG];
		e_gid = egid;
	}

	return(ERR_SUCCESS);
}

/*
 *	uxident_set_groups:  set ancillary group set.  if this involves
 *	new groups, re-calculate tokens for our real and effective uid/gid's
 */
mach_error_t uxident::uxident_set_groups(int* gidset, int ngroups_p)
{
	std_ident*		as_id[2];
	ns_authid_t		groups[NGROUPS];
	int			i;
	mach_error_t		err = ERR_SUCCESS;

	DEBUG1(TRUE,(0,"set_groups: gidset 0x%x, ngroups %d\n",
		gidset, ngroups_p));
	/*
	 * gotta be root ...
	 */
	/*
	 * XXX C++ Where was this operation exported ?
	 * Is is (or should it be) part of the IO protocol ? 
	 * The naming protocol ?
	 */
//	if (fs_is_root(Local(access_obj), Local(r_uid)) != ERR_SUCCESS)
//		return(US_INVALID_ACCESS);

	if (ngroups_p > Count(_Local(groups)))
		return(US_INVALID_ARGS);

	DEBUG1(TRUE,(0,"set_groups: changing groups\n"));

	/*
	 * add each group authid
	 * Note: BSD Unix allows a group to appear more than once in the list.
	 */
	for (i=0; i<ngroups_p; i++) {
		ns_authid_t	gid;

		(void)access_obj->fs_gid_to_authid(gidset[i], &gid);
		groups[i] = gid;
	}

	/*
	 * Try to create new IDs.
	 * Bail out and return an error if we can't create them.
	 */
	as_id[ID_EU_EG] = NULL;
	as_id[ID_RU_RG] = NULL;

	err = create_unix_identity(e_uid, e_gid,
				    groups, ngroups_p, &as_id[ID_EU_EG]);
	if (err != ERR_SUCCESS) {
		DEBUG1(TRUE,(0, 
		  "set_groups: can't create effective ID: %s\n",
		  (char*)mach_error_string(err)));
		mach_object_dereference(as_id[ID_EU_EG]);
		return(err);
	}

	err = create_unix_identity(r_uid, r_gid,
				    groups, ngroups_p, &as_id[ID_RU_RG]);
	if (err != ERR_SUCCESS) {
		DEBUG1(TRUE,(0, 
		  "set_groups: can't create real ID: %s\n",
		  (char*)mach_error_string(err)));
		mach_object_dereference(as_id[ID_EU_EG]);
		mach_object_dereference(as_id[ID_RU_RG]);
		return(err);
	}

	/*
	 * Change group list and IDs now that
	 * we've made it through without error
	 */
	_Local(ngroups) = ngroups_p;
	for (i = 0; i < ngroups_p; i++)
		groups[i] = groups[i];

	mach_object_dereference(as_ids[ID_EU_EG]);
	as_ids[ID_EU_EG] = as_id[ID_EU_EG];

	mach_object_dereference(as_ids[ID_RU_RG]);
	as_ids[ID_RU_RG] = as_id[ID_RU_RG];

	return(ERR_SUCCESS);
}

/*
 *	uxident_get_token: return our effective token
 */
uxident::uxident_get_token(ns_token_t* token)
{

	/*
	 * create a token for our effective identity (effective user and group ids)
	 */
	return (as_ids[ID_EU_EG])->ns_get_token(token);
}


/*
 * uxident_get_eids: return authid's for our effective uid and gid
 */
uxident::uxident_get_eids(ns_authid_t *euid, ns_authid_t *egid)
{
	*euid = _Local(e_uid);
	*egid = _Local(e_gid);

	return ERR_SUCCESS;
}


/*
 * uxident_get_rids: return authid's for our real uid and gid
 */
uxident::uxident_get_rids(ns_authid_t *ruid, ns_authid_t *rgid)
{

	*ruid = _Local(r_uid);
	*rgid = _Local(r_gid);
	return ERR_SUCCESS;

}



/*
 *	uxident_get_groups:  return the current ancillary group set
 */
mach_error_t uxident::uxident_get_groups(int* gidset, int* ngroups_p)
{
	register int		i;
	int			ng;

	DEBUG1(TRUE,(0,"get_groups: gidset 0x%x, ngroups 0x%x\n",
		gidset, ngroups_p));

	ng = MIN(_Local(ngroups),*ngroups_p);
	for( i=0; i < ng; i++) {
		(void)access_obj->fs_authid_to_gid(groups[i],&gidset[i]);
	}

	for( ; i< *ngroups_p; i++)
		gidset[i] = -1;

	*ngroups_p = ng;
	return(ERR_SUCCESS);
}

mach_error_t
uxident::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_duplicate(ns_access_t access, usItem** newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_set_times(time_value_t atime, time_value_t mtime)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_get_protection(ns_prot_t prot, int* protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_set_protection(ns_prot_t prot, int protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_get_privileged_id(int* id)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxident::ns_get_manager(ns_access_t access, usItem **newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

