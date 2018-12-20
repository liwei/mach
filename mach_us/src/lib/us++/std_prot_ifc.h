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
 * ObjectClass: std_prot
 *	Server-side object that is used to define the protection for
 *	an object.
 *
 * SuperClass: base
 *
 * Delegated Objects: none
 *
 * ClassMethods:
 *	Exported: 
 *	ns_check_access(), ns_set_protection(), ns_get_protection(),
 *	ns_get_priv_id()
 *	Internal: none
 *
 * Notes:
 *	The std_prot object is matched against the user's credentials
 *	when creating a new agent to determine what methods the
 *	user is allowed to perform.
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_prot_ifc.h,v $
 *
 * Purpose: Generic protection object
 *
 * HISTORY:
 * $Log:	std_prot_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:39  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:29:04  dpj
 * 	Define as an abstract class instead of a concrete class.
 * 	Eliminated active_table/active_object mechanism.
 * 	[92/06/24  17:10:00  dpj]
 * 
 * 	Added a couple of include files.
 * 	[92/05/10  00:58:32  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:11  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:56:34  pjg]
 * 
 * Revision 2.2.2.2  91/04/14  18:35:42  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.2.2.1  90/11/14  17:11:30  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.3  90/12/19  11:05:52  jjc
 * 	Added ns_is_priv() which checks to see if the user is privileged.
 * 	[90/10/30            jjc]
 * 
 * Revision 2.2  89/03/17  12:50:30  sanzi
 * 	Added ns_get_privileged_id() and ns_get_protection_ltd().
 * 	Added a local variable for the privileged ID.
 * 	[89/02/22  23:03:34  dpj]
 * 	
 * 	add ns_get_protection_ptr.
 * 	[89/02/16  13:24:13  dorr]
 * 	
 * 	get rid of ns_get_priv_id.
 * 	[89/02/15  15:23:24  dorr]
 * 	
 * 	change acl -> protection operations
 * 	[89/02/07  10:59:37  dorr]
 * 
 */

#ifndef	_std_prot_ifc_h
#define	_std_prot_ifc_h

#include <top_ifc.h>
#include <std_cred_ifc.h>
#include <agency_ifc.h>
#include <access_table_ifc.h>


//class std_prot: public agency {

class std_prot: public agency {
	ns_prot_t	prot;		/* this agency's protection */
	int		protlen;
	ns_authid_t	privid;		/* privileged ID */
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(std_prot);
	std_prot();
	std_prot(ns_mgr_id_t, access_table*);
	virtual ~std_prot();
//	static void init_class(usClass*);

	virtual mach_error_t ns_check_access(ns_access_t, std_cred*);
REMOTE	virtual mach_error_t ns_set_protection(ns_prot_t, int);
REMOTE	virtual mach_error_t ns_get_protection(ns_prot_t, int*);
	virtual mach_error_t ns_get_protection_ptr(ns_prot_t*);
REMOTE	virtual mach_error_t ns_get_privileged_id(int*);
	mach_error_t ns_get_protection_ltd(ns_prot_ltd_t);
	mach_error_t ns_is_priv();
};

#endif	_std_prot_ifc_h

