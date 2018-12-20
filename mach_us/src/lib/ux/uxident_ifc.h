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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxident_ifc.h,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxident_ifc.h,v $
 * Revision 2.7  94/07/08  16:01:51  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.6  94/05/17  14:08:42  jms
 * 	Need implementations of virtual methods in class uxident for 2.3.3 g++ -modh
 * 	[94/04/28  18:58:03  jms]
 * 
 * Revision 2.5.1.1  94/02/18  11:33:26  modh
 * 	Need to declare virtual functions in class uxident for 2.3.3 g++
 * 
 * Revision 2.5  92/07/05  23:32:44  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:31:03  dpj]
 * 
 * Revision 2.4  91/11/06  14:11:45  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:14:53  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:08:35  pjg]
 * 
 * Revision 2.3  90/12/19  11:06:05  jjc
 * 	Only keep track of two IDs (real and effective)
 * 	[90/08/29            jjc]
 * 
 * Revision 2.2  90/01/02  22:14:58  dorr
 * 	add set_regid set_groups & get_groups.
 * 
 * Revision 2.1.1.2  90/01/02  14:19:28  dorr
 * 	add groups list and ngroups.
 * 	declare methods for set_regid, get_groups & set_groups.
 * 
 * Revision 2.1.1.1  89/12/28  11:00:30  dorr
 * 	Initial version.
 * 
 * Revision 2.2  89/06/30  18:36:45  dpj
 * 	Moved here from lib/us.
 * 	[89/06/29  00:49:29  dpj]
 * 
 * Revision 2.3  89/05/17  16:44:55  dorr
 * 	add fields to keep track of current identity.
 * 	add methods to set current identity.
 * 	[89/05/15  12:19:31  dorr]
 * 
 * Revision 2.2  89/03/17  12:55:20  sanzi
 * 	Created.
 * 	[89/03/02  13:49:50  dorr]
 * 
 *
 */


#ifndef	uxident_ifc_h
#define	uxident_ifc_h

#include <top_ifc.h>
#include <clone_ifc.h>
#include <std_auth_ifc.h>
#include <fs_access_ifc.h>
#include <std_ident_ifc.h>

extern "C" {
#include <sys/param.h>
}

/*
 *	define the unix protection specific operations
 */

class uxident: public usClone {
	std_auth*	auth_obj;
	fs_access*	access_obj;
	std_ident*	as_ids[2];	/* unforgeable id's corresponding to real/eff uid/gid's */
	ns_authid_t	r_uid;		/* current real/eff uid & gid's */
	ns_authid_t	e_uid;
	ns_authid_t	r_gid;
	ns_authid_t	e_gid;
	int		ngroups;	/* all of the groups you belong to */
	ns_authid_t	groups[NGROUPS];
      public:
	uxident(fs_access* =0, std_auth* =0, std_ident* =0);
	virtual ~uxident();

	uxident_set_reuid(int, int);
	uxident_set_regid(int, int);
	uxident_set_groups(int*, int);
	uxident_get_token(ns_token_t*);
	uxident_get_eids(ns_authid_t*, ns_authid_t*);
	uxident_get_rids(ns_authid_t*, ns_authid_t*);
	uxident_get_groups(int*, int*);

	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_abort(mach_port_t) { return ERR_SUCCESS; }
	virtual mach_error_t clone_complete() { return ERR_SUCCESS; }

        virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
        virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
        virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
        virtual mach_error_t ns_set_times(time_value_t, time_value_t);
        virtual mach_error_t ns_get_protection(ns_prot_t, int*);
        virtual mach_error_t ns_set_protection(ns_prot_t, int);
        virtual mach_error_t ns_get_privileged_id(int*);
        virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);
        virtual mach_error_t ns_get_manager(ns_access_t, usItem **);
      private:
	mach_error_t create_unix_identity(ns_authid_t, ns_authid_t,
						   ns_authid_t*, int, 
						   std_ident**);
	mach_error_t add_group(ns_authid_t);
	mach_error_t remove_group(ns_authid_t);
};

#endif	uxident_ifc_h
