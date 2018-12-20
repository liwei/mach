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
 * any improvements or extensions that they made and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/agent.cc,v $
 *
 * Purpose: Body for the agent class, including the
 *	remote-invocations filter. Instances of this class are used
 *	at the Base pointer for all operations on agencies.
 *
 * HISTORY:
 * $Log:	agent.cc,v $
 * Revision 2.8  94/07/07  17:22:46  mrt
 * 	Updated copyright.
 * 
 * Revision 2.7  94/05/17  14:07:10  jms
 * 	Static variables bug in 2.3.3 g++ compiler require declaration of 
 * 	object name -modh
 * 	[94/04/28  18:41:50  jms]
 * 
 * Revision 2.5.2.1  94/02/18  11:27:01  modh
 * 	Static variables bug in 2.3.3 g++ compiler require declaration of object name
 * 
 * 
 * Revision 2.6  94/01/11  17:49:53  jms
 * 	If ns_register_agent fails at time of agent creation, assume that the
 * 	(bad) agent is about to be killed, and dissociate the agency to prevent
 * 	extra agency death slime
 * 	[94/01/09  19:36:07  jms]
 * 
 * Revision 2.5  92/07/05  23:26:39  dpj
 * 	Removed ns_get_agency_ptr() (replaced by ns_get_item_ptr()).
 * 	[92/07/05  18:52:25  dpj]
 * 
 * 	Replace ns_get_agency_ptr with ns_get_item_ptr.
 * 	[92/06/24  15:28:55  jms]
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:48:42  dpj]
 * 
 * Revision 2.3  91/11/13  17:17:44  dpj
 * 	Fixed call path for no_remote_senders().
 * 	[91/11/13  14:51:10  dpj]
 * 
 * Revision 2.2  91/11/06  13:44:33  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:24:08  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:19:10  pjg]
 * 
 * 	Initial C++ revision.
 * 	Merged classes agent and agent. Agent now has a pointer to its
 * 	agency. The current agent pointer is stored in the per-thread data.
 * 	[90/11/14  15:19:04  pjg]
 * 
 * Revision 2.2  89/10/30  16:31:03  dpj
 * 	First version.
 * 	[89/10/27  17:22:09  dpj]
 * 
 */

#define	FAST_CASTDOWN


#include <agent_ifc.h>
#include <access_table_ifc.h>

extern "C" {
#include	<stdarg.h>
#include	<us_error.h>
}


#define BASE usItem
_DEFINE_CLASS(agent)
_DEFINE_INSTANTIATION(agent)
_DEFINE_CONVERTER(agent)
_DEFINE_CONSTRUCTOR(agent)
_DEFINE_CASTDOWN(agent)


#ifdef GXXBUG_STATIC_MEM
static int _5agent$object_key;
#endif


void agent::init_class(usClass* class_obj)
{
        BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(agent);
	SETUP_METHOD_WITH_ARGS(agent,ns_authenticate);
	SETUP_METHOD_WITH_ARGS(agent,ns_duplicate);
	SETUP_METHOD_WITH_ARGS(agent,ns_get_access);
	END_SETUP_METHOD_WITH_ARGS;
}

agent::agent() : my_agency(0),access(0),cred_obj(0),acctab(0)
{
}

agent::agent(char* s) 
	: my_agency(0),access(0),cred_obj(0),acctab(0)
{
	((usClass*)is_a())->set_remote_class_name(s);
}

agent::agent(agency *agency_obj, std_cred *credobj, access_table* acct, 
	     ns_access_t acc, mach_error_t* ret) 
	:
	my_agency(agency_obj), access(acc), cred_obj(credobj), 
	acctab(acct)
{
	DEBUG1((1), (0, "_setup_agent: agency=%x\n", agency_obj));

	mach_object_reference(cred_obj);
	mach_object_reference(acctab);
	mach_object_reference(my_agency);

	*ret = my_agency->ns_register_agent(access);
	if (*ret == MACH_OBJECT_NO_SUCH_OPERATION) {
		*ret = ERR_SUCCESS;	
	}
	
	/* 
         * If agency refused registration, then assume that we will be killed
	 * soon and dissassociate the agency.  It has refused us.
	 */
	if (ERR_SUCCESS != *ret) {
		mach_object_dereference(my_agency);
		my_agency = NULL;
	}
}

agent::~agent()
{
	DESTRUCTOR_GUARD();
	if (my_agency) {
		DEBUG1(TRUE,(0,"ns_unregister_agent: access=0x%x\n", access));
		(void) my_agency->ns_unregister_agent(access);
	}
	mach_object_dereference(cred_obj);
	mach_object_dereference(acctab);
	mach_object_dereference(my_agency);
}

/*
 * Get the credentials object corresponding to this agent.
 */
mach_error_t agent::ns_get_cred_obj(std_cred **crd_obj)
{
	*crd_obj = this->cred_obj;
	mach_object_reference(this->cred_obj);
	return(ERR_SUCCESS);
}

char* agent::remote_class_name() const
{
	if (my_agency) {
		return my_agency->remote_class_name();
	} else {
		return is_a()->remote_class_name();
	}
}


/*
 * Authentication calls.
 */
/*
 * Return a new handle for the same object, with a new access
 * and authentication.
 *
 * The normal primitive specifies a single token for authentication.
 * Other primitives may specify many parameters, like effective ID,
 * security level, etc.
 */
mach_error_t 
agent::ns_authenticate(ns_access_t access, ns_token_t token, usItem** newobj)
{
	mach_error_t	ret;
	std_cred*	new_cred_obj;
	agent*		agent_obj;

	DEBUG1((1), (0, "agent::_ns_authenticate\n"));

	ret = cred_obj->ns_translate_token(token, &new_cred_obj);
	if (ret != ERR_SUCCESS) {
		*newobj = NULL;
		return(ret);
	}
	ret = my_agency->ns_create_agent(access, new_cred_obj, &agent_obj);
	*newobj = agent_obj;
	mach_object_dereference(new_cred_obj);
	return(ret);
}


/*
 * Return a new handle for the same object, with a new access
 * and the same authentication.
 */
mach_error_t 
agent::ns_duplicate(ns_access_t access, usItem** newobj)
{
	mach_error_t	ret;
	agent*		agent_obj;

	DEBUG1((1), (0, "agent::_ns_duplicate\n"));
	ret = my_agency->ns_create_agent(access, cred_obj, &agent_obj);
	*newobj = agent_obj;
	return(ret);
}


/*
 * Get the access and credentials for a given object handle.
 *
 * The credentials are represented using a standard format.
 */
mach_error_t agent::ns_get_access(
	ns_access_t		*access,
	ns_cred_t		cred,
	int			*credlen)
{
	mach_error_t		ret;

	*access = this->access;
	ret = cred_obj->ns_get_cred(cred, credlen);

	return(ret);
}


/*
 * Return a pointer to the agency.
 */
mach_error_t agent::ns_get_item_ptr(usItem** item)
{
	mach_object_reference(my_agency);
	*item = my_agency;
	return(ERR_SUCCESS);
}

mach_error_t agent::invoke(mach_method_id_t mid, ...)
{
	mach_error_t 		ret;
	ns_access_t		req_access;
	va_list			ap;

	va_start(ap, mid);

	/*
	 * Find out the access rights required for the current method
	 * (mid).
	 */
	ret = acctab->ns_find_required_access(mid,&req_access);
	if (ret != ERR_SUCCESS) {
		va_end(ap);
		return(ret);
	}

	/*
	 * Check that the access is allowed, and forward the operation.
	 *
	 * Note that everybody always has NSR_REFERENCE access.
	 */
	if (((access | NSR_REFERENCE) & req_access) != req_access) {
		va_end(ap);
		return(US_INVALID_ACCESS);
	}

	/*
	 * Store a pointer to this agent, to serve as a context for
	 * operations inside the agency.
	 */
	agent::set_base_object(this);

	/*
	 * Find the right method and call it.
	 */
	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset,
				*(arg_list_ptr)ap);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				_castdown(*entryp->cl),
				*(arg_list_ptr)ap);
#endif	FAST_CASTDOWN
	}
	else if ((entryp = my_agency->is_a()->lookup_method(mid)) != 0) {
		/*
		 * No check for my_agency != 0.
		 */
#ifdef	FAST_CASTDOWN
#ifdef	notdef
	ERROR((Diag,"agent forwarding: entryp=0x%x, entryp->pfunc=0x%x, entryp->cl=0x%x, entryp->offset=0x%x, agency=0x%x, (usTop*)agency=0x%x, agency->_castdown(*entryp->cl)=0x%x, dir::desc()=0x%x, dir::castdown(agency)=0x%x, dir::castdown((usTop*)agency)=0x%x",
			entryp,
			entryp->pfunc,
			entryp->cl,
	       		entryp->offset,
			my_agency, 
			(usTop*)my_agency, 
			my_agency->_castdown(*entryp->cl), 
			dir::desc(),
			dir::castdown(my_agency), 
			dir::castdown((usTop*)my_agency)));
#endif	notdef
		ret = (*entryp->pfunc)(
				((void*)((usTop*)my_agency)) + entryp->offset,
				*(arg_list_ptr)ap);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				my_agency->_castdown(*entryp->cl),
				*(arg_list_ptr)ap);
#endif	FAST_CASTDOWN
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	va_end(ap);

	return ret;
}


mach_error_t agent::invoke(mach_method_id_t mid)
{
	mach_error_t 		ret;
	ns_access_t		req_access;

	/*
	 * Find out the access rights required for the current method
	 * (mid).
	 */
	ret = acctab->ns_find_required_access(mid,&req_access);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Check that the access is allowed, and forward the operation.
	 *
	 * Note that everybody always has NSR_REFERENCE access.
	 */
	if (((access | NSR_REFERENCE) & req_access) != req_access) {
		return(US_INVALID_ACCESS);
	}

	/*
	 * Store a pointer to this agent, to serve as a context for
	 * operations inside the agency.
	 */
	agent::set_base_object(this);

	/*
	 * Find the right method and call it.
	 */
	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				_castdown(*entryp->cl));
#endif	FAST_CASTDOWN
	}
	else if ((entryp = my_agency->is_a()->lookup_method(mid)) != 0) {
		/*
		 * No check for my_agency != 0.
		 */
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)my_agency)) + entryp->offset);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				my_agency->_castdown(*entryp->cl));
#endif	FAST_CASTDOWN
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return ret;
}


mach_error_t agent::invoke(mach_method_id_t mid, void* arg0)
{
	mach_error_t 		ret;
	ns_access_t		req_access;

	/*
	 * Find out the access rights required for the current method
	 * (mid).
	 */
	ret = acctab->ns_find_required_access(mid,&req_access);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Check that the access is allowed, and forward the operation.
	 *
	 * Note that everybody always has NSR_REFERENCE access.
	 */
	if (((access | NSR_REFERENCE) & req_access) != req_access) {
		return(US_INVALID_ACCESS);
	}

	/*
	 * Store a pointer to this agent, to serve as a context for
	 * operations inside the agency.
	 */
	agent::set_base_object(this);

	/*
	 * Find the right method and call it.
	 */
	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset,
				arg0);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				_castdown(*entryp->cl),arg0);
#endif	FAST_CASTDOWN
	}
	else if ((entryp = my_agency->is_a()->lookup_method(mid)) != 0) {
		/*
		 * No check for my_agency != 0.
		 */
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)my_agency)) + entryp->offset,
				arg0);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				my_agency->_castdown(*entryp->cl),
				arg0);
#endif	FAST_CASTDOWN
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return ret;
}


mach_error_t agent::invoke(mach_method_id_t mid, void* arg0, void* arg1)
{
	mach_error_t 		ret;
	ns_access_t		req_access;

	/*
	 * Find out the access rights required for the current method
	 * (mid).
	 */
	ret = acctab->ns_find_required_access(mid,&req_access);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Check that the access is allowed, and forward the operation.
	 *
	 * Note that everybody always has NSR_REFERENCE access.
	 */
	if (((access | NSR_REFERENCE) & req_access) != req_access) {
		return(US_INVALID_ACCESS);
	}

	/*
	 * Store a pointer to this agent, to serve as a context for
	 * operations inside the agency.
	 */
	agent::set_base_object(this);

	/*
	 * Find the right method and call it.
	 */
	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset,
				arg0,arg1);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				_castdown(*entryp->cl),arg0,arg1);
#endif	FAST_CASTDOWN
	}
	else if ((entryp = my_agency->is_a()->lookup_method(mid)) != 0) {
		/*
		 * No check for my_agency != 0.
		 */
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)my_agency)) + entryp->offset,
				arg0,arg1);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				my_agency->_castdown(*entryp->cl),
				arg0,arg1);
#endif	FAST_CASTDOWN
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return ret;
}


/* The following methods not defined for agents */

mach_error_t agent::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	return _notdef();
}

mach_error_t agent::ns_set_times(time_value_t atime, time_value_t mtime)
{
	return _notdef();
}
mach_error_t agent::ns_get_protection(ns_prot_t prot, int* protlen)
{
	return _notdef();
}

mach_error_t agent::ns_set_protection(ns_prot_t prot, int protlen)
{
	return _notdef();
}

mach_error_t agent::ns_get_privileged_id(int* id)
{
	return _notdef();
}

mach_error_t agent::ns_get_manager(ns_access_t, usItem **)
{
	return _notdef();
}

/* XXX temporary, to work around bug in g++ */
mach_error_t agent::ns_register_agent(ns_access_t access)
{
	return _notdef();
}
mach_error_t agent::ns_unregister_agent(ns_access_t access)
{
	return _notdef();
}

class agent* agent::base_object()
{
	class agent*	obj;
	(void) cthread_getspecific(agent::object_key, (any_t *)(&obj));
	return(obj);
}

void agent::set_base_object(class agent* obj)
{
	if (agent::object_key == 0) {
		cthread_keycreate(&agent::object_key);
	}
	(void) cthread_setspecific(agent::object_key, obj);
}
