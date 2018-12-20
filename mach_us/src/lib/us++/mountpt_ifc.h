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
 * ObjectClass: mountpt
 *	Server-side object that is used to provide mount points
 *	within the name space.
 *
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/mountpt_ifc.h,v $
 *
 * Purpose: Generic mount point object.
 *
 * HISTORY:
 * $Log:	mountpt_ifc.h,v $
 * Revision 2.4  94/07/07  17:23:46  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  94/05/17  14:07:17  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/28  18:47:05  jms]
 * 
 * Revision 2.2  91/11/06  13:47:05  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:54:37  pjg]
 * 
 * Revision 2.3.1.2  91/04/14  18:29:41  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.3.1.1  90/11/14  17:09:45  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.3  89/10/30  16:34:31  dpj
 * 	Added setup method.
 * 	Use PublicLocal().
 * 	Derive from vol_agency.
 * 	[89/10/27  19:06:11  dpj]
 * 
 * Revision 2.2  89/06/30  18:35:15  dpj
 * 	Initial revision.
 * 	[89/06/29  00:35:14  dpj]
 * 
 */

#ifndef	_mountpt_ifc_h
#define	_mountpt_ifc_h

#include <us_name_ifc.h>
#include <vol_agency_ifc.h>

class mountpt: public vol_agency, public usName {
	usItem* fwd_obj;
      public:
	DECLARE_MEMBERS(mountpt);
	mountpt(ns_mgr_id_t =null_mgr_id, access_table* =0, 
		usItem* =0, mach_error_t* =0);
	virtual ~mountpt();
//	static void init_class(usClass*);

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
REMOTE	virtual mach_error_t ns_read_forwarding_entry(usItem**, char*);

	/*
	 * From usName but not implemented.
	 */
	virtual mach_error_t ns_resolve(char*, ns_mode_t, ns_access_t, 
					 usItem** , ns_type_t*, char*, 
					 int*, ns_action_t*);
	virtual mach_error_t ns_create(char*, ns_type_t, ns_prot_t, int,
					ns_access_t, usItem**);
	virtual mach_error_t ns_create_anon(ns_type_t, ns_prot_t, int,
					     ns_access_t, char*, usItem**);
	virtual mach_error_t ns_create_transparent_symlink(ns_name_t, ns_prot_t,
							  int, char *);
	virtual mach_error_t ns_insert_entry(char*, usItem*);
	virtual mach_error_t ns_insert_forwarding_entry(char*, ns_prot_t, int,
							 usItem*, char*);
	virtual mach_error_t ns_remove_entry(char*);
	virtual mach_error_t ns_rename_entry(char*, usItem*, char*);
	virtual mach_error_t ns_list_entries(ns_mode_t, ns_name_t**,
					      unsigned int*, ns_entry_t*,
					      unsigned int*);
	virtual mach_error_t ns_list_types(ns_type_t**, int*);
	virtual mach_error_t ns_allocate_unique_name(char*);
};

#endif	_mountpt_ifc_h

