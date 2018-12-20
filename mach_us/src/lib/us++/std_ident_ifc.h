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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_ident_ifc.h,v $
 *
 * Purpose: generic login identity holder
 *
 * HISTORY:
 * $Log:	std_ident_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:28  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:49  dpj
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:12:20  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:53  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:56:07  pjg]
 * 
 * Revision 2.1.3.1  90/11/14  17:10:57  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.1  89/05/15  14:46:20  dorr
 * Created.
 * 
 * 
 */

#ifndef	_std_ident_ifc_h
#define	_std_ident_ifc_h

#include <top_ifc.h>

extern "C" {
#include	<ns_types.h>
}

class std_ident: public usTop {
	ns_identity_t  		ident;	/* our identity */
	ns_token_t     		token;	/* token we can give away */
      public:
	DECLARE_LOCAL_MEMBERS(std_ident);
	virtual ~std_ident();

	std_ident(ns_identity_t =0, ns_token_t =0);

	virtual mach_error_t clone_init(mach_port_t);

	mach_error_t ns_get_token(ns_token_t*);
	mach_error_t ns_get_identity(ns_identity_t*);
};

#endif	_std_ident_ifc_h

