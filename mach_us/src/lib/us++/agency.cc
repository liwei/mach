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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/agency.cc,v $
 *
 * Purpose: Coordinator for all the various agents for a given object within
 * a server.
 *
 * This class should only be used as a base for a specialized agency class for
 * each specific server. It should not be directly instantiated.
 *
 * HISTORY:
 * $Log:	agency.cc,v $
 * Revision 2.6  94/07/07  17:22:38  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  94/05/17  14:07:03  jms
 * 	Needed implementations of methods in agency class for 2.3.3 g++ compiler -modh
 * 	[94/04/28  18:40:20  jms]
 * 
 * Revision 2.4.1.1  94/02/18  11:24:42  modh
 * 	Needed implementations of methods in agency class for 2.3.3 g++ compiler
 * 
 * Revision 2.4  92/07/05  23:26:34  dpj
 * 	Waste ns_get_agency_ptr, replaced with usItem::ns_get_item_ptr
 * 	[92/06/24  15:26:07  jms]
 * 	Eliminated active_table/active_object mechanism.
 * 	[92/06/24  16:00:25  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:47:10  dpj]
 * 
 * Revision 2.2  91/11/06  13:44:22  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:22:23  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:18:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:15:23  pjg]
 * 
 * Revision 1.9  91/07/01  14:11:16  jms
 * 	Added ns_get_manager().
 * 	[91/06/21  17:15:37  dpj]
 * 
 * Revision 1.8  91/05/05  19:25:35  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:52:31  dpj]
 * 
 * 	Added ns_create_agent_same_cred().
 * 	[91/04/07            dpj]
 * 
 * Revision 1.7  89/10/30  16:30:35  dpj
 * 	Reorganized to use setup methods, PublicLocal() macros
 * 	and active_table. Cleaned-up initial setup.
 * 	[89/10/27  17:16:30  dpj]
 * 
 * Revision 1.6  89/06/30  18:33:11  dpj
 * 	Check that the active_table is not NULL before generating
 * 	an act_idle_hint().
 * 	Added ns_get_agency_ptr(). I'm not using it right now,
 * 	but hey, it looks good!
 * 	[89/06/29  00:14:20  dpj]
 * 
 * Revision 1.5  89/03/30  12:05:27  dpj
 * 	Updated to new syntax for invoke_super().
 * 	[89/03/26  18:48:16  dpj]
 * 
 * Revision 1.4  89/03/17  12:36:03  sanzi
 * 	Added some methods to allow objects that derive from this class to access
 * 	our local variables.  These should be removed when we enable children to
 * 	access parents variables directly. 
 * 	[89/03/01  10:41:06  sanzi]
 * 	
 * 	Modified ns_create_agent() to not register the agent
 * 	explcitly. Registration is now handled by the new agent itself.
 * 	[89/02/25  00:25:48  dpj]
 * 	
 * 	Added access_table to the agency state.
 * 	[89/02/24  19:08:43  dpj]
 * 	
 * 	Remove bogus mach_object_dereference(*newobj) in 
 * 	agency_ns_create_agency() in error case after call
 * 	to ns_register_agent().
 * 	[89/02/24  17:18:40  sanzi]
 * 	
 * 	don't crap out if delegate doesn't support [un]register.
 * 	[89/02/15  15:24:25  dorr]
 * 	
 * 	don't call delegate operations uless you have a delegate.
 * 	[89/02/10  13:56:00  dorr]
 * 	
 * 	ns_register_agent -> agency_ns_register_agent
 * 	[89/02/08  16:12:09  dorr]
 * 	
 * 	fix compilation errors.
 * 	[89/02/08  13:51:18  dorr]
 * 	
 * 	Removed agent_count stuff.
 * 	Fixed setup method.
 * 	Added ns_deactivate().
 * 	Fixed ns_unregister_agent() to generate an act_idle_hint when appropriate.
 * 	[89/02/07  13:24:25  dpj]
 * 	
 * 	Checkpoint before removing the agent_count
 * 	[89/02/06  14:37:56  dpj]
 * 	
 * 	Initial version from dpj_5
 * 	[89/01/12  17:29:19  dpj]
 * 
 * Revision 1.3.1.2  88/12/14  20:48:05  dpj
 * 	Last checkin before Xmas break
 * 
 * Revision 1.3.1.1  88/11/18  10:29:09  dpj
 * 	Checkin for us tree reorganization.
 * 
 * Revision 1.3  88/11/01  01:41:27  dpj
 * Added support for remote invocation.
 * 
 * Revision 1.2  88/10/24  19:53:25  dpj
 * Fixed a few syntax error.
 * Do not allow export() on a shutdown agency.
 * 
 * Revision 1.1  88/10/21  11:56:30  dpj
 * Initial revision
 *
 */


#include <agent_ifc.h>
#include <agency_ifc.h>
#include <std_auth_ifc.h>

extern "C" {
#include	<ns_types.h>

extern mach_error_t netname_check_in(mach_port_t, char*, 
					mach_port_t, mach_port_t);
}

/*
 * Class methods
 */

#define BASE usItem
DEFINE_ABSTRACT_CLASS(agency);

const ns_mgr_id_t null_mgr_id = {0, 0};

agency::agency() : mgr_id(null_mgr_id), access_tab(0)
{}

agency::agency(ns_mgr_id_t m_id, access_table* acctab)
	:
	mgr_id(m_id),access_tab(acctab)
{	
	mach_object_reference(acctab);
}

agency::~agency()
{
	DESTRUCTOR_GUARD();
	mach_object_dereference(_Local(access_tab));
}

char* agency::remote_class_name() const
{
	return "INVALID_CLASS";
}


/*
 * Create a new agent for this agency.
 */
mach_error_t 
agency::ns_create_agent(ns_access_t access, std_cred* cred_obj, 
			agent** newobj)
{
	mach_error_t		ret;

	DEBUG1((1), (0, "agency: ns_create_agent\n"));

	if (access == 0) {
		*newobj = 0;
		return(ERR_SUCCESS);
	}
       	ret = ns_check_access(access, cred_obj);
	if (ret != ERR_SUCCESS) {
		*newobj = 0;
		return(ret);
	}
	/*
	 * Create the agent.
	 */
//	new_object(*newobj,agent);
//	*newobj = new_agent();
//	mach_object_reference(this);
//	ret = invoke(*newobj,mach_method_id(setup_agent),this,access,
//		     cred_obj,_Local(access_tab));

	mach_object_reference(this);
	*newobj = new agent(this, cred_obj, access_tab, access, &ret);
	mach_object_dereference(this);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(*newobj);
		*newobj = 0;
		return(ret);
	}
	return(ERR_SUCCESS);
}

mach_error_t 
agency::ns_create_agent_same_cred(ns_access_t access, agent** newobj)
{
	std_cred*		cred;
	mach_error_t		ret;

	/*
	 * No agent with 0 access.
	 */
	if (access == 0) {
		*newobj = NULL;
		return(ERR_SUCCESS);
	}

	/*
	 * Get current credentials.
	 */
	ret = agent::base_object()->ns_get_cred_obj(&cred);
	if (ret != ERR_SUCCESS) {
		us_internal_error(
		"ns_create_agent_same_cred: cannot get current credentials",
		ret);
		*newobj = NULL;
		return(US_INTERNAL_ERROR);
	}

	/*
	 * Create the agent and exit.
	 */
	ret = ns_create_agent(access,cred,newobj);
	mach_object_dereference(cred);

	return(ret);
}


/*
 * Create a new initial agent for this agency, with anonymous credentials.
 */
mach_error_t agency::ns_create_initial_agent(agent** newobj)
{
	mach_error_t	ret;
	std_cred	*anon_cred;

	std_auth *authenticator = new std_auth;

	*newobj = NULL;
	ret = authenticator->ns_translate_token(MACH_PORT_NULL, &anon_cred);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(authenticator);
		return(ret);
	}
	ret = ns_create_agent(NSR_REFERENCE, anon_cred, newobj);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(anon_cred);
		mach_object_dereference(authenticator);
		return(ret);
	}
	mach_object_dereference(anon_cred);
	mach_object_dereference(authenticator);

	return(ERR_SUCCESS);
}

/*
 * Create an initial agent, and export it through the netname service.
 */
mach_error_t agency::ns_netname_export(ns_name_t name)
{
	mach_error_t	ret;
	agent*		agent_obj;

	ret = ns_create_initial_agent(&agent_obj);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	int porttype;
	mach_port_t p  = agent_obj->get_transfer_port(&porttype);
	ret = netname_check_in(name_server_port, name, MACH_PORT_NULL, p);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}
	INFO((0,"Agent checked-in as \"%s\"\n",name));

	return(ERR_SUCCESS);
}


/*
 * Return a handle for a "manager object" exporting operations to
 * control the whole set of agencies in one server or subtree.
 *
 * XXX For now, we just return a pointer to the Diag object, with
 * XXX no further mediation. This should be replaced with a real
 * XXX manager object, exported through an agent, etc.
 */
mach_error_t agency::ns_get_manager(ns_access_t access, usItem **newobj)
{
	extern diag *_us_diag_object;

//	mach_object_reference(_us_diag_object);
//	*newobj = _us_diag_object;
	*newobj = 0;
	return(ERR_SUCCESS);
}

#ifdef notdef
mach_error_t agency::_invoke(int mid, obj_arg_list_t arglist)
{
	mach_error_t ret;

	DEBUG2(TRUE,(0, "agency::invoke: obj=0x%0x, mid=%d\n", this, mid));
	us_table_entry_t entryp = is_a()->_lookup(mid);
	if (entryp != 0) {
		Pftype functionp = entryp->pfunc;
		usClass* class_desc = (usClass*) entryp->cl;
		void* thisp = _castdown(*class_desc);
		DEBUG2(TRUE,(0,"agency::invoke on agency obj=0x%0x, mid=%d\n",thisp, mid));
		ret = (*functionp)(thisp, arglist);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	DEBUG2(TRUE,(0, "agency::invoke: ret=%d\n", ret));
	return ret;
}
#endif notdef


mach_error_t agency::ns_register_agent(ns_access_t access)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}
mach_error_t agency::ns_unregister_agent(ns_access_t access)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}
mach_error_t agency::ns_check_access(ns_access_t access, std_cred *cred_obj)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t agency::ns_reference()
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t agency::ns_dereference()
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t 
agency::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t 
agency::ns_duplicate(ns_access_t access, usItem** newobj)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t 
agency::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t 
agency::ns_set_times(time_value_t atime, time_value_t mtime)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t 
agency::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}
