/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/net_dir_base_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for network service endpoints.
 *
 * HISTORY
 * $Log:	net_dir_base_ifc.h,v $
 * Revision 2.3  94/07/07  17:23:48  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:47:11  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:54:51  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:49  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:13  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:13:27  dpj]
 * 
 */

#ifndef	_net_dir_base_ifc_h
#define	_net_dir_base_ifc_h

#include <us_net_name_ifc.h>
#include <dir_ifc.h>


//class net_dir_base: public virtual usNetName, public virtual dir {

class net_dir_base: public dir {
      protected:
	net_info_t		netinfo;
      public:
	DECLARE_MEMBERS(net_dir_base);
	net_dir_base(ns_mgr_id_t =null_mgr_id, access_table * =0, 
		     net_info_t * =0);
	virtual ~net_dir_base();
	virtual char* remote_class_name() const;

#ifdef notdef
	virtual mach_error_t ns_create_agent(ns_access_t, std_cred *,agent **);
#endif notdef

REMOTE	virtual mach_error_t ns_create(ns_name_t, ns_type_t, ns_prot_t,
				       int, ns_access_t, usItem **);
REMOTE	virtual mach_error_t ns_create_transparent_symlink(ns_name_t,ns_prot_t,
							   int, char *);
REMOTE	virtual mach_error_t ns_insert_entry(char *, usItem*);
REMOTE	virtual mach_error_t ns_insert_forwarding_entry(ns_name_t, ns_prot_t, 
							int, usItem *, char*);
REMOTE	virtual mach_error_t ns_rename_entry(ns_name_t, usItem*,ns_name_t);
REMOTE	virtual mach_error_t ns_list_types(ns_type_t **, int *);

REMOTE	virtual mach_error_t net_lookup(net_addr_t*, ns_access_t, usItem **,
					ns_type_t*, net_info_t *);
REMOTE	virtual mach_error_t net_cots_lookup(net_addr_t *, net_addr_t *,
					     ns_access_t, usItem **,
					     ns_type_t *, net_info_t *);
	virtual mach_error_t net_create(net_addr_t *, int *, ns_prot_t, int,
					ns_access_t, usItem **, ns_type_t *,
					net_info_t *);
};


#endif	_net_dir_base_ifc_h
