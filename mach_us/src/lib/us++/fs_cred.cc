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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/fs_cred.cc,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	fs_cred.cc,v $
 * Revision 2.3  94/07/07  17:23:21  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:27:31  dpj
 * 	Derive from std_cred instead of usTop.
 * 	Use fs_auth instead of std_auth.
 * 	[92/06/24  16:22:09  dpj]
 * 
 * Revision 2.1  91/09/27  11:36:11  pjg
 * Created.
 * 
 * Revision 2.3  89/06/30  18:34:19  dpj
 * 	Added ns_get_cred_ptr(), ns_get_principal_id(). This class
 * 	must be a superset of std_cred.
 * 	It should be derived from std_cred.
 * 	[89/06/29  00:28:26  dpj]
 * 
 * Revision 2.2  89/03/17  12:41:04  sanzi
 * 	Removed check for minimal size for standard credentials.
 * 	Removed one unused variable.
 * 	[89/02/12  20:50:16  dpj]
 * 	
 * 	Added a reference to the authenticator responsible for this object.
 * 	Forward ns_translate_token() to the authenticator.
 * 	[89/02/08  15:22:44  dpj]
 * 	
 * 	fix some compiler errors.
 * 	[89/02/08  11:26:17  dorr]
 * 	
 * 	Created.
 * 	[89/02/07  17:13:54  dorr]
 * 
 */

#ifndef lint
char * fs_cred_rcsid = "$Header: fs_cred.cc,v 2.3 94/07/07 17:23:21 mrt Exp $";
#endif	lint

#include	<fs_cred_ifc.h>
#include	<fs_auth_ifc.h>


#define BASE std_cred
DEFINE_LOCAL_CLASS(fs_cred);

//fs_cred::fs_cred()
//:
// authenticator(0)
//{}

fs_cred::fs_cred(fs_auth *_authenticator, ns_cred_t cred, 
		 unsigned int credlen, fs_cred_t _fs_cred_struct)
:
 authenticator(_authenticator)
{
	unsigned int			org_credlen = credlen;

	mach_object_reference(authenticator);

	/* credlen is the size, in int's of the entire credentials structure */

#ifdef	notdef
	if ( (credlen - (sizeof(cred->head) / sizeof(int))) < 2) {
		return(NS_UNSUPPORTED_CRED);
	}
#endif	notdef

	/*
	 * copy in the fs credentials
	 */
	Local(fs_cred_struct) = *_fs_cred_struct;

	/*
	 * copy in the standard credentials
	 */
	Local(ns_cred) = (ns_cred_t)Malloc(NS_CRED_SIZE(cred));
	bcopy(cred,Local(ns_cred),NS_CRED_SIZE(cred));
	Local(ns_cred_len) = org_credlen;
}


fs_cred::~fs_cred()
{
	DESTRUCTOR_GUARD();
	mach_object_dereference(Local(authenticator));
}

/*
 * Contact the authenticator to translate an authentication token.
 *
 * This call is simply forwarded to the authenticator.
 */
mach_error_t fs_cred::fs_translate_token(ns_token_t token, fs_cred** newobj)
{
	mach_error_t		ret;

	ret = authenticator->fs_translate_token(token,newobj);
	return(ret);
}

mach_error_t fs_cred::ns_translate_token(ns_token_t token, std_cred** newobj)
{
	mach_error_t		ret;
	fs_cred*		cr;

	ret = authenticator->fs_translate_token(token,&cr);
	*newobj = fs_cred::castdown(cr);
	return(ret);
}


/*
 * Get the credentials for a given object handle.
 *
 * The credentials are represented using a standard format.
 */
mach_error_t fs_cred::ns_get_cred(ns_cred_t cred, int *credlen)
{
	if (*credlen <= Local(ns_cred_len)) {
		*credlen = 0;
		return(NS_NOT_ENOUGH_ROOM);
	}

	bcopy(Local(ns_cred),cred,NS_CRED_SIZE(Local(ns_cred)));
	*credlen = Local(ns_cred_len);

	return(ERR_SUCCESS);
}


/*
 * Get a pointer to the internal standard cred structure.
 */
mach_error_t fs_cred::ns_get_cred_ptr(ns_cred_t *cred_ptr)
{
	*cred_ptr = Local(ns_cred);

	return(ERR_SUCCESS);
}


/*
 * Get an authentication ID characterizing the main identity
 * of the client associated with the current credentials.
 */
mach_error_t fs_cred::ns_get_principal_id(ns_authid_t *id)
{
	*id = Local(ns_cred)->authid[0];

	return(ERR_SUCCESS);
}


/*
 * Get a pointer to the internal fs_cred structure.
 */
mach_error_t fs_cred::fs_get_cred_ptr(fs_cred_t *_fs_cred_struct)
{
	*_fs_cred_struct = &Local(fs_cred_struct);

	return(ERR_SUCCESS);
}

