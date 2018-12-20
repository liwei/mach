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
 * ObjectClass: agent
 * 	Generic representative for a client for a given object
 * 	within a server. This object contains the port that the client
 * 	sends requests to, and keeps track of when this port is no longer
 *	in use.
 */
 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/agent_ifc.h,v $
 *
 * HISTORY:
 * $Log:	agent_ifc.h,v $
 * Revision 2.4  94/07/07  17:22:48  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:26:42  dpj
 * 	Replace ns_get_agency_ptr with ns_get_item_ptr.
 * 	[92/06/24  15:29:18  jms]
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:50:23  dpj]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:02:13  pjg]
 * 
 * Revision 2.2  91/11/06  13:44:39  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:49:11  pjg]
 * 
 * Revision 1.5.1.1  91/04/14  18:19:22  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.2  89/10/30  16:31:12  dpj
 * 	First version.
 * 	[89/10/27  17:22:54  dpj]
 * 
 */

#ifndef	_agent_ifc_h
#define	_agent_ifc_h

#include <agency_ifc.h>

extern "C" {
#include <ns_types.h>
}

class agent: public usItem {
	agency*			my_agency;
	std_cred*		cred_obj;
	access_table*		acctab;
	ns_access_t		access;

	static int		object_key;

      public:

	static class agent*	base_object();
	static void		set_base_object(class agent*);

	DECLARE_MEMBERS(agent);
				agent();
				agent(char*);
				agent(
					agency*,
					std_cred*,
					access_table*,
					ns_access_t,
					mach_error_t*);
	virtual			 ~agent();

	virtual mach_error_t	invoke(mach_method_id_t, ...);
	virtual mach_error_t	invoke(mach_method_id_t);
	virtual mach_error_t	invoke(mach_method_id_t,void*);
	virtual mach_error_t	invoke(mach_method_id_t,void*,void*);

 	virtual char*		remote_class_name() const;

	mach_error_t		ns_get_cred_obj(std_cred **);
	mach_error_t		ns_get_item_ptr(usItem**);

	/*
	 * Methods exported remotely
	 */
REMOTE	virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
REMOTE	virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
REMOTE	virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);

	/*
	 * From usItem but implemented by agencies
	 */
	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
	virtual mach_error_t ns_set_times(time_value_t, time_value_t);
	virtual mach_error_t ns_get_protection(ns_prot_t, int*);
	virtual mach_error_t ns_set_protection(ns_prot_t, int);
	virtual mach_error_t ns_get_privileged_id(int*);
	virtual mach_error_t ns_register_agent(ns_access_t);
	virtual mach_error_t ns_unregister_agent(ns_access_t);
	virtual mach_error_t ns_get_manager(ns_access_t, usItem **);
};

#endif	_agent_ifc_h
