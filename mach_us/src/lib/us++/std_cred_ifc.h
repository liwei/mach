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
 * ObjectClass: std_cred
 *	Server-side object that is used to define the generic
 *	protection for an object.
 *
 * SuperClass: base
 *
 * Delegated Objects: none
 *
 * ClassMethods:
 *	Exported: ns_setup_access(), ns_get_cred()
 *
 *	Internal: ns_get_cred_ptr().
 *
 * Notes:
 *	The std_cred object is matched against an object's protection
 *	when creating a new agent to determine what methods the
 *	user is allowed to perform.  
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_cred_ifc.h,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	std_cred_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:25  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:46  dpj
 * 	Made main methods virtual.
 * 	[92/06/24  17:07:51  dpj]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:30:47  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:10:37  pjg]
 * 
 * Revision 2.2  91/11/06  13:47:47  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:55:57  pjg]
 * 
 * Revision 2.2  89/03/17  12:49:30  sanzi
 * 	Added ns_get_cred_ptr().
 * 	[89/02/22  23:02:46  dpj]
 * 	
 * 	Added ns_get_principal_id().
 * 	[89/02/21  17:58:02  dpj]
 * 	
 * 	convert from fs_cred.
 * 	[89/02/15  15:14:38  dorr]
 * 
 */

#ifndef	_std_cred_ifc_h
#define	_std_cred_ifc_h


#include <top_ifc.h>

extern "C" {
#include	<ns_types.h>
}

class std_auth;

class std_cred: public usTop {
	std_auth	*authenticator;
	ns_cred_t	ns_cred;
	int		ns_cred_len;
      public:
	DECLARE_LOCAL_MEMBERS(std_cred);
				std_cred();
				std_cred(std_auth*,ns_cred_t,
					unsigned int,mach_error_t*);
	virtual			~std_cred();

	virtual mach_error_t	ns_translate_token(ns_token_t, std_cred**);
	virtual mach_error_t	ns_get_cred(ns_cred_t, int*);
	virtual mach_error_t	ns_get_cred_ptr(ns_cred_t*);
	virtual mach_error_t	ns_get_principal_id(ns_authid_t*);
};


#endif	_std_cred_ifc_h

