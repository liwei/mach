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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/fs_auth_ifc.h,v $
 *
 * Purpose: 
 *
 * HISTORY:
 * $Log:	fs_auth_ifc.h,v $
 * Revision 2.3  94/07/07  17:23:20  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:27:29  dpj
 * 	Make the new compiler happy.
 * 	[92/06/24  16:21:01  dpj]
 * 
 * 	Initial version from dpj_5
 * 	[89/01/12  18:17:57  dpj]
 * 
 * Revision 2.1  91/09/27  14:51:28  pjg
 * Created.
 * 
 * Revision 2.3  89/10/30  16:32:31  dpj
 * 	Use fs_access for authid translations.
 * 	[89/10/27  17:35:44  dpj]
 * 
 * Revision 2.2  89/03/17  12:40:51  sanzi
 * 	declare conversion methods.  auth_access -> auth_cred.
 * 	[89/02/08  13:56:00  dorr]
 * 	
 * 	created.
 * 	[89/02/07  17:30:47  dorr]
 * 	
 * 	Last checkin before Xmas break
 * 	[88/12/14  20:55:56  dpj]
 * 
 */

#ifndef	_fs_auth_ifc_h
#define	_fs_auth_ifc_h

#include	<top_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<fs_types.h>
}

class fs_cred;
class fs_access;


class fs_auth: public usTop {
	mach_port_t		as_port;	/* port to the auth server */
	class fs_access*	fs_access_obj;	/* access manager */
	ns_authid_t		anon_authid;	/* anonymous ID */
	class fs_cred*		anon_cred;	/* anonymous access obj */
      public:
	DECLARE_LOCAL_MEMBERS(fs_auth);
//			fs_auth();
			fs_auth(class fs_access*);
			~fs_auth();
	mach_error_t	fs_translate_token(ns_token_t,class fs_cred**);
};

#endif	_fs_auth_ifc_h

