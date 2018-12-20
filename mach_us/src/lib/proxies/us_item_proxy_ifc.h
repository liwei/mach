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
 * ObjectClass: us_item_proxy
 *	Server-side object that is used to construct a hierarchical
 *	volatile name space.
 *
 * SuperClass: dir
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/us_item_proxy_ifc.h,v $
 *
 * Purpose: Generic us_item_proxy object.
 *
 * HISTORY:
 * $Log:	us_item_proxy_ifc.h,v $
 * Revision 2.3  94/07/07  17:59:38  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:32  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:56:17  jms]
 * 
 * Revision 2.3  92/07/05  23:30:04  dpj
 * 	Conditionalized virtual base class specifications.
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:19:18  dpj]
 * 
 * 	Removed virtual destructor.
 * 	[92/05/10  01:19:50  dpj]
 * 
 * Revision 2.2  91/11/06  13:49:53  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:58:53  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:43  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:16:14  pjg]
 * 
 */

#ifndef	_us_item_proxy_ifc_h
#define	_us_item_proxy_ifc_h

#include <clone_ifc.h>
#include <us_item_ifc.h>


#ifdef	GXXBUG_CLONING1
class usItem_proxy: public VIRTUAL2 usClone {
#else	GXXBUG_CLONING1
class usItem_proxy: public VIRTUAL2 usItem, public VIRTUAL1 usClone {
#endif	GXXBUG_CLONING1
      public:
	DECLARE_PROXY_MEMBERS(usItem_proxy);
	usItem_proxy() {}

	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_abort(mach_port_t);
	virtual mach_error_t clone_complete();

	/*
	 * Methods exported remotely
	 */
	virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
	virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
	virtual mach_error_t ns_set_times(time_value_t, time_value_t);
	virtual mach_error_t ns_get_protection(ns_prot_t, int*);
	virtual mach_error_t ns_set_protection(ns_prot_t, int);
	virtual mach_error_t ns_get_privileged_id(int*);
	virtual mach_error_t ns_get_access(ns_access_t*, ns_cred_t, int*);
	virtual mach_error_t ns_get_manager(ns_access_t, usItem **);
};

#endif	_us_item_proxy_ifc_h

