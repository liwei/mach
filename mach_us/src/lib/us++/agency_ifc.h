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
 * ObjectClass: agency
 * 	Coordinator for all the various agents for a given [externally visible]
 *	object within a server.
 * 
 * Delegated Objects: any object needed for this particular agency.
 * 
 * ClassMethods:
 *
 * Notes:
 * 	This class should only be used as a base for a specialized agency
 *	class for each specific server. It should not be directly instantiated.
 *
 * Bugs:
 * 
 * Features:
 * 
 * Transgressions:
 */
 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/agency_ifc.h,v $
 *
 * HISTORY:
 * $Log:	agency_ifc.h,v $
 * Revision 2.5  94/07/07  17:22:42  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/05/17  14:07:05  jms
 * 	Needed to delcare virtual functions in agency class for 2.3.3 g++ compiler -modh
 * 	[94/04/28  18:40:59  jms]
 * 
 * Revision 2.3.1.1  94/02/18  11:25:27  modh
 * 	Needed to delcare virtual functions in agency class for 2.3.3 g++ compiler
 * 
 * Revision 2.3  92/07/05  23:26:37  dpj
 * 	Waste ns_get_agency_ptr(agency**).  See usItem::ns_get_item_ptr.
 * 	[92/06/24  15:27:30  jms]
 * 	Conditionalized virtual base class specifications.
 * 	Eliminated active_table/active_object mechanism.
 * 	Made ns_create_agent() virtual.
 * 	[92/06/24  16:01:12  dpj]
 * 
 * 	No changes.
 * 	[92/05/10  00:47:26  dpj]
 * 
 * Revision 2.2  91/11/06  13:44:28  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:48:57  pjg]
 * 
 * Revision 1.7.1.2  91/04/14  18:18:18  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 1.7.1.1  90/11/14  17:01:25  pjg
 * 	Initial C++ revision.
 * 
 * Revision 1.9  91/07/01  14:11:18  jms
 * 	Added ns_get_manager().
 * 	[91/06/21  17:16:48  dpj]
 * 
 * Revision 1.8  91/05/05  19:25:38  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:52:38  dpj]
 * 
 * 	Added ns_create_agent_same_cred().
 * 	[91/04/07            dpj]
 * 
 * Revision 1.7  89/10/30  16:30:40  dpj
 * 	Reorganized to use setup methods, PublicLocal() macros
 * 	and active_table.
 * 	[89/10/27  17:17:56  dpj]
 * 
 * Revision 1.6  89/06/30  18:33:17  dpj
 * 	Added ns_get_agency_ptr().
 * 	[89/06/29  00:14:44  dpj]
 * 
 * Revision 1.5  89/03/17  12:36:22  sanzi
 * 	Added some methods to allow objects that
 * 	derive from this class to access our local
 * 	variables.  These should be removed when we
 * 	enable children to access parents variables
 * 	directly. 
 * 	[89/03/01  10:38:45  sanzi]
 * 	
 * 	Added a pointer to the access_table in the local variables.
 * 	[89/02/24  18:29:00  dpj]
 * 	
 * 	#include act_internal.h; get rid of NULL declaration.
 * 	[89/02/08  13:54:11  dorr]
 * 	
 * 	Removed agent_count stuff.
 * 	[89/02/07  13:22:02  dpj]
 * 	
 * 	Checkpoint before removing the agent_count
 * 	[89/02/06  14:37:14  dpj]
 * 	
 * 	Added comments.
 * 	[89/02/03  10:15:30  sanzi]
 * 	
 * 	Initial version from dpj_5
 * 	[89/01/12  17:27:15  dpj]
 * 
 * Revision 1.3.1.2  88/12/14  20:46:26  dpj
 * 	Last checkin before Xmas break
 * 
 * Revision 1.3.1.1  88/11/18  10:41:21  dpj
 * 	Checkin for us tree reorganization
 * 
 * Revision 1.3  88/11/01  01:44:05  dpj
 * Added support for remote invocation.
 * 
 * Revision 1.2  88/10/24  19:45:52  dpj
 * Fixed a syntax error.
 * 
 * Revision 1.1  88/10/21  11:49:06  dpj
 * Initial revision
 * 
 * Revision 1.2  88/10/18  18:11:36  dorr
 * change some names.
 * 
 * Revision 1.1  88/10/17  23:51:45  dpj
 * Initial revision
 * 
 *
 */

#ifndef	_agency_ifc_h
#define	_agency_ifc_h

#include <top_ifc.h>
#include <access_table_ifc.h>
#include <std_cred_ifc.h>
#include <us_item_ifc.h>
#include <diag_ifc.h>

class agent;

extern const ns_mgr_id_t null_mgr_id;

class agency: public VIRTUAL2 usItem {
      protected:
	ns_mgr_id_t	mgr_id;
	access_table*	access_tab;
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(agency);
	agency();
	agency(ns_mgr_id_t, access_table*);
	virtual ~agency();
	virtual char* remote_class_name() const;
//	virtual mach_error_t _invoke (int, obj_arg_list_t);

	virtual mach_error_t ns_create_agent(ns_access_t, std_cred*, agent**);
	virtual mach_error_t ns_create_agent_same_cred(ns_access_t, agent**);
	virtual mach_error_t ns_create_initial_agent(agent**);
	virtual mach_error_t ns_netname_export(ns_name_t);
	virtual mach_error_t ns_get_manager(ns_access_t, usItem **);

	virtual mach_error_t ns_register_agent(ns_access_t);
	virtual mach_error_t ns_unregister_agent(ns_access_t);

	virtual mach_error_t ns_check_access(ns_access_t, std_cred*);

	/*
	 * From usItem and intercepted by agent.
	 * Just say no.
	 */
	virtual mach_error_t ns_reference();
	virtual mach_error_t ns_dereference();
	virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
	virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
	virtual mach_error_t ns_get_attributes(ns_attr_t, int *);
	virtual mach_error_t ns_set_times(time_value_t, time_value_t);
	virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);

};

#endif	_agency_ifc_h

