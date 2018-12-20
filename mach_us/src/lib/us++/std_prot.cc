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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_prot.cc,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	std_prot.cc,v $
 * Revision 2.5  94/07/07  17:24:38  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/05/17  14:08:04  jms
 * 	ns_check_access: Ensure for special ids (root) that NSR_EXECUTE permission is
 * 	not granted unles someone has execute permission
 * 	[94/04/28  18:54:11  jms]
 * 
 * Revision 2.3  92/07/05  23:29:02  dpj
 * 	Define as an abstract class instead of a concrete class.
 * 	Eliminated active_table/active_object mechanism.
 * 	[92/06/24  17:09:21  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:58:10  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:07  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:12:35  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:35:31  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:41:54  pjg]
 * 
 * Revision 2.5  91/05/05  19:27:32  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:58:19  dpj]
 * 
 * 	Corrected computation of protlen.
 * 	[91/04/28  10:20:00  dpj]
 * 
 * Revision 2.4  90/12/19  11:05:47  jjc
 * 	Added ns_is_priv() to check to see whether the user is privileged.
 * 	[90/10/30            jjc]
 * 
 * Revision 2.3  89/06/30  18:35:41  dpj
 * 	Removed AUTH_ID_WILD. Use NS_AUTHID_WILDCARD instead, imported
 * 	from ns_types.h.
 * 	[89/06/29  00:42:17  dpj]
 * 
 * Revision 2.2  89/03/17  12:50:16  sanzi
 * 	Fixed ns_set_protection() to accept generation numbers of 0.
 * 	Fixed size of bcopy() for ns_get_protection_ltd().
 * 	[89/02/24  18:42:01  dpj]
 * 	
 * 	Added privileged_id handling.
 * 	Added default initial protection.
 * 	Fixed ns_check_access() to conform to the standard
 * 	protection model, and to use ns_get_cred_ptr().
 * 	Added ns_get_protection_ltd().
 * 	[89/02/22  22:53:38  dpj]
 * 	
 * 	add ns_get_protection_ptr.
 * 	[89/02/16  13:20:59  dorr]
 * 	
 * 	add authorization wildcards to verification scheme.
 * 	[89/02/15  15:36:39  dorr]
 * 	
 * 	check_access takes an object as an argument.
 * 	[89/02/08  09:39:12  dorr]
 * 	
 * 	first shot at compiling.
 * 	[89/02/07  11:00:13  dorr]
 * 	
 * 	standard protection object
 * 	[89/02/03  15:44:28  dorr]
 * 
 */

#ifndef lint
char * std_prot_rcsid = "$Header: std_prot.cc,v 2.5 94/07/07 17:24:38 mrt Exp $";
#endif	lint

#include <std_prot_ifc.h>
#include <std_cred_ifc.h>
#include <agent_ifc.h>

#define BASE agency
DEFINE_ABSTRACT_CLASS_MI(std_prot);
_DEFINE_CASTDOWN(std_prot);


/*
 * Privileged ID, for which all access checking operations always
 * succeed.
 *
 * This is a variable, so that it can be patched to a different value
 * for various applications if needed. Note that this should only be
 * patched before any object is instantiated.
 */
ns_authid_t	std_prot_priv_id = 1;


/*
 * Default initial protection.
 *
 * This structure can be patched to accomodate special applications.
 */
struct ns_prot	std_prot_default_prot = {
	{
		NS_PROT_VERSION,
		1,
		1
	},
	{
		NS_AUTHID_WILDCARD,
		NSR_REFERENCE
	}
};


void std_prot::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(std_prot);
	SETUP_METHOD_WITH_ARGS(std_prot,ns_get_protection);
	SETUP_METHOD_WITH_ARGS(std_prot,ns_set_protection);
	SETUP_METHOD_WITH_ARGS(std_prot,ns_get_privileged_id);
	END_SETUP_METHOD_WITH_ARGS;
}

std_prot::std_prot() 
	: 
	privid(std_prot_priv_id),
	prot(&std_prot_default_prot),
	protlen(NS_PROT_SIZE(&std_prot_default_prot)/sizeof(int))
{}

std_prot::std_prot(ns_mgr_id_t m_id, access_table *acctab)
	:
	agency(m_id, acctab),
	privid(std_prot_priv_id),
	prot(&std_prot_default_prot),
	protlen(NS_PROT_SIZE(&std_prot_default_prot)/sizeof(int))
{}

std_prot::~std_prot()
{
	DESTRUCTOR_GUARD();
	if (_Local(prot) != &std_prot_default_prot) {
	       /* XXX Getting a "free: object has bad free list pointer" here*/
		free(_Local(prot));  
	}
}


/*
 * Match a set of credentials against the protection info,
 * and see if a given access is allowed.
 */
mach_error_t std_prot::ns_check_access(ns_access_t access, std_cred *cred_obj)
{
	register int		i;
	register int		j;
	ns_cred_t		cred_ptr;
	mach_error_t		err;
	ns_authid_t		cur_authid;
	ns_access_t		max_access = 0;

	err = cred_obj->ns_get_cred_ptr(&cred_ptr);
	if (err != ERR_SUCCESS) {
		return(err);
	}

	/*
	 * See if the given principal has the indicated access in our
	 * protection structure.
	 *
	 * XXX Is it better to look for principals in the ACL, or
	 * to look for ACL entries in the credentials list?
	 */

	for (j = 0; j < cred_ptr->head.cred_len; j++) {
		cur_authid = cred_ptr->authid[j];

		/*
		 * Check for privileged ID.
		 */
		if (cur_authid == _Local(privid)) {
			/* 
			 * if we need NSR_EXECUTE permision, be sure
			 * someone could get it
			 */
			if (! (access & NSR_EXECUTE)) {
				/* Root no NSR_EXECUTE always OK */
				return(ERR_SUCCESS);
			}
			else {
				for(i=0; i < _Local(prot)->head.acl_len; i++) {
					if (_Local(prot)->acl[i].rights & NSR_EXECUTE) {
						return(ERR_SUCCESS);
					}
				}
				return(US_INVALID_ACCESS);
			}
		}

		/*
		 * Find this credential in the acl (or a wildcard).
		 *
		 * XXX Should keep track of the wildcard and not look
		 * for it each time.
		 */
		for( i = 0; i < _Local(prot)->head.acl_len; i++) {
			if ((_Local(prot)->acl[i].authid == cur_authid) ||
			(_Local(prot)->acl[i].authid == NS_AUTHID_WILDCARD)) {

				max_access |= _Local(prot)->acl[i].rights;
				if ((max_access & access) == access) {
					return(ERR_SUCCESS);
				}
			}
		}
	}
	return(US_INVALID_ACCESS);
}


/*
 * Get and set the protection on an object, using the standard ACL
 * format.
 */
mach_error_t std_prot::ns_set_protection(ns_prot_t xprot, int xprotlen)
{
	if ((xprot->head.generation != 0) &&
		(xprot->head.generation != _Local(prot)->head.generation+1)) {
		return(NS_SET_PROT_FAILED);
	}

	if (_Local(prot) != &std_prot_default_prot) {
		free(_Local(prot));
	}

	_Local(prot) = (ns_prot_t)malloc(NS_PROT_SIZE(xprot));
	if (_Local(prot) == NS_PROT_NULL) {
		return(US_OUT_OF_MEMORY);
	}

	bcopy(xprot,_Local(prot),NS_PROT_SIZE(xprot));
	_Local(protlen) = xprotlen;

	return(ERR_SUCCESS);
}

mach_error_t std_prot::ns_get_protection(ns_prot_t xprot, int *xprotlen)
{
	if (_Local(prot)) {
		bcopy(_Local(prot),xprot,NS_PROT_SIZE(_Local(prot)));
		*xprotlen = _Local(protlen);
		return(ERR_SUCCESS);
	} else {
		*xprotlen = 0;
		return(US_OBJECT_NOT_FOUND);
	}
}


/*
 *	internal routine used to pass protection around
 *	efficiently
 */
mach_error_t std_prot::ns_get_protection_ptr(ns_prot_t *xprot)
{
	*xprot = _Local(prot);
	return(ERR_SUCCESS);
}


/*
 * Find out the privileged authentication ID that has full access to
 * this object.
 */
mach_error_t std_prot::ns_get_privileged_id(int *xprivid)
{
	*xprivid = _Local(privid);
	return(ERR_SUCCESS);
}


/*
 * Make a copy of the beginning of the protection ACL, for
 * use in a standard attributes structure.
 */
mach_error_t std_prot::ns_get_protection_ltd(ns_prot_ltd_t prot_ltd)
{
	bzero(prot_ltd,sizeof(struct ns_prot_ltd));
	bcopy(_Local(prot)->acl,prot_ltd,
			MIN(_Local(prot)->head.acl_len,4) *
				sizeof(struct ns_acl_entry));

	return(ERR_SUCCESS);
}

/*
 * Check to see if the user is privileged.
 */
mach_error_t std_prot::ns_is_priv()
{
	std_cred*	cred_obj;
	ns_cred_t		cred_ptr;
	ns_authid_t		cur_authid;
	mach_error_t		err;
	register int		i;

	(void) agent::base_object()->ns_get_cred_obj(&cred_obj);
	
	err = cred_obj->ns_get_cred_ptr(&cred_ptr);
	if (err != ERR_SUCCESS) {
		return(US_INVALID_ACCESS);
	}

	for (i = 0; i < cred_ptr->head.cred_len; i++) {
		cur_authid = cred_ptr->authid[i];

		/*
		 * Check for privileged ID.
		 */
		if (cur_authid == privid) {
			return(ERR_SUCCESS);
		}
	}
	return(US_INVALID_ACCESS);
}
