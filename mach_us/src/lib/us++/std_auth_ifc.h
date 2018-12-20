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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_auth_ifc.h,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	std_auth_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:20  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:41  dpj
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:12:04  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:41  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:55:44  pjg]
 * 
 * Revision 2.3.1.2  91/04/14  18:30:24  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.3.1.1  90/11/14  17:10:14  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.3  89/05/17  16:43:56  dorr
 * 	add ns_create_identity
 * 	[89/05/15  12:16:39  dorr]
 * 
 * Revision 2.2  89/03/17  12:49:02  sanzi
 * 	Created.
 * 	[89/02/13  16:51:08  dorr]
 * 
 */

#ifndef	_std_auth_ifc_h
#define	_std_auth_ifc_h

#include <std_cred_ifc.h>
#include <std_ident_ifc.h>

extern "C" {
#include	<ns_types.h>
}

class std_auth: public usTop {
	mach_port_t	as_port;	/* port to the auth server */
	std_cred*	anon_cred;	/* anonymous access obj */
      public:
	DECLARE_LOCAL_MEMBERS(std_auth);
	std_auth();
	virtual ~std_auth();
	mach_error_t ns_translate_token (ns_token_t, std_cred**);
	mach_error_t ns_create_identity (std_ident*, ns_cred_t,
					 std_ident**);
};


#endif	_std_auth_ifc_h

