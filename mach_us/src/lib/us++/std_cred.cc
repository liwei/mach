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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_cred.cc,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	std_cred.cc,v $
 * Revision 2.4  94/07/07  17:24:23  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:44  dpj
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:57:39  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:44  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:09:24  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:30:35  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:40:07  pjg]
 * 
 * Revision 2.2  89/03/17  12:49:16  sanzi
 * 	Fixed compilation errors.
 * 	[89/02/24  18:40:27  dpj]
 * 	
 * 	Added ns_get_cred_ptr().
 * 	[89/02/22  23:02:01  dpj]
 * 	
 * 	Added ns_get_principal_id().
 * 	[89/02/21  17:56:54  dpj]
 * 	
 * 	Created.
 * 	[89/02/15  15:37:01  dorr]
 * 
 */

#ifndef lint
char * std_credpp_rcsid = "$Header: std_cred.cc,v 2.4 94/07/07 17:24:23 mrt Exp $";
#endif	lint

#include	<us_error.h>
#include	<std_cred_ifc.h>
#include	<std_auth_ifc.h>

#define BASE usTop
DEFINE_LOCAL_CLASS(std_cred);

std_cred::std_cred() : authenticator(0)
{
	ns_cred = NS_CRED_NULL;
}

/*
 * Setup the internal state from a standard credentials structure.
 */
std_cred::std_cred(std_auth *authenticator, ns_cred_t cred,
		   unsigned int credlen, mach_error_t* ret) 
{
        unsigned int	org_credlen = credlen;

	this->authenticator = authenticator;
	mach_object_reference(authenticator);

	/* credlen is the size, in int's of the entire credentials structure */

	if ( (credlen - (sizeof(cred->head) / sizeof(int))) < 1) {
		*ret = NS_UNSUPPORTED_CRED;
		return;
	}

	/*
	 * copy in the real credentials
	 */
	_Local(ns_cred) = (ns_cred_t)malloc(NS_CRED_SIZE(cred));
	if (_Local(ns_cred) == NS_CRED_NULL) {
		*ret = US_OUT_OF_MEMORY;
		return;
	}
	bcopy(cred,_Local(ns_cred),NS_CRED_SIZE(cred));
	_Local(ns_cred_len) = org_credlen;

	*ret = ERR_SUCCESS;
}

std_cred::~std_cred()
{
	DESTRUCTOR_GUARD();
	mach_object_dereference(_Local(authenticator));
	if (_Local(ns_cred))
		free(_Local(ns_cred));
}


/*
 * Contact the authenticator to translate an authentication token.
 *
 * This call is simply forwarded to the authenticator.
 */
mach_error_t
std_cred::ns_translate_token(ns_token_t token, std_cred **newobj)
{
	mach_error_t		ret;

	ret = authenticator->ns_translate_token(token, newobj);
	return(ret);
}


/*
 * Get the credentials for a given object handle.
 *
 * The credentials are represented using a standard format.
 */
mach_error_t std_cred::ns_get_cred(ns_cred_t cred, int *credlen)
{
	if (_Local(ns_cred) == NS_CRED_NULL) {
		*credlen = 0;
		return(NS_UNSUPPORTED_CRED);
	}

	if (*credlen <= _Local(ns_cred_len)) {
		*credlen = 0;
		return(NS_NOT_ENOUGH_ROOM);
	}

	bcopy(_Local(ns_cred),cred,NS_CRED_SIZE(_Local(ns_cred)));
	*credlen = _Local(ns_cred_len);

	return(ERR_SUCCESS);
}


/*
 * Get a pointer to the internal cred structure.
 */
mach_error_t std_cred::ns_get_cred_ptr(ns_cred_t *cred_ptr)
{
	*cred_ptr = _Local(ns_cred);

	return(ERR_SUCCESS);
}


/*
 * Get an authentication ID characterizing the main identity
 * of the client associated with the current credentials.
 */
mach_error_t std_cred::ns_get_principal_id(ns_authid_t *id)
{
	*id = _Local(ns_cred)->authid[0];

	return(ERR_SUCCESS);
}

