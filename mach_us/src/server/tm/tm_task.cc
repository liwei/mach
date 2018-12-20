/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tm/tm_task.cc,v $
 *
 * Purpose: Task object implementation.  Interface to the task master for 
 *		manipulating / querying specific tasks
 *
 * HISTORY:
 * $Log:	tm_task.cc,v $
 * Revision 2.10  94/10/27  12:01:57  jms
 * 	Add "tm_get_shared_info method" (and support methods) to access process
 * 	info hsared between the taskmaster and the client process emulator.
 * 	[94/10/26  14:53:10  jms]
 * 
 * Revision 2.9  94/07/13  17:33:34  mrt
 * 	Updated copyright
 * 
 * Revision 2.8  94/05/17  13:36:09  jms
 * 	Add task session calls.
 * 	[94/05/11  14:55:58  modh]
 * 
 * Revision 2.7  94/05/16  16:39:47  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 
 * 	Add hack in constructor to permit the calling of a method in a superclass
 * 		this_hack = this; this_hack->method();
 * 
 * Revision 2.6  94/04/29  15:48:09  jms
 * 	Debugging info changes
 * 	[94/04/29  15:01:57  jms]
 * 
 * Revision 2.5  94/01/11  18:11:34  jms
 * 	Use first ACL entry as task owner.
 * 	[94/01/10  13:41:22  jms]
 * 
 * Revision 2.4  93/01/20  17:39:32  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  17:38:46  jms]
 * 
 * Revision 2.3  92/07/06  16:37:17  dpj
 * 	Fixed a locking bug involved with killing suspended processes,
 * 	and another for dead processes.
 * 
 * 
 * Revision 2.2  92/07/05  23:35:43  dpj
 * 	Converted for new C++ RPC package.
 * 	[92/07/05  19:01:59  dpj]
 * 
 * 	Ensure that events are not sent to dead processes.
 * 	[92/06/25  11:22:30  jms]
 * 
 * 	Use new us_tm_{root,task,tgrp}_ifc.h interfaces for the C++ taskmaster.
 * 	Translated from tm_task.c
 * 	[92/06/24  18:05:16  jms]
 * 
 * Revision 1.14  92/03/05  15:13:12  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:33:21  jms]
 * 
 * Revision 1.13  91/11/13  17:21:22  dpj
 * 	Removed the call to tm_deregister_task() in terminate().
 * 	Use mach_port_destroy() to get rid of the kernel_port.
 * 	[91/07/13  22:28:50  dpj]
 * 
 * Revision 1.12  91/07/01  14:15:20  jms
 * 	Add tm_hurtme to stop/kill an emulated process.  Called remotely by the
 * 	emulation itself or called by TM after timeout whe posting a "hard" signal.
 * 
 * 	Remove old "self sig via TM" logic.
 * 	Add "state" field to task so we we know when we are sleeping, i
 * 	we know when we're awake...
 * 	[91/06/25  11:51:35  jms]
 * 
 * Revision 1.11  90/11/27  18:22:02  jms
 * 	Modify task creation for the pre/post forms of registration of tasks.
 * 	[90/11/20  14:58:47  jms]
 * 
 * 	Put switches around MACH3_UNIX specific code
 * 	[90/08/20  17:42:40  jms]
 * 
 * Revision 1.10  90/08/13  15:45:05  jjc
 * 	Added methods for interval timer.
 * 	[90/07/19            jjc]
 * 
 * Revision 1.9  90/07/09  17:12:07  dorr
 * 	Add emulation status manipulation methods.
 * 	Add parent notification logic.
 * 	Take out some HTG stuff
 * 	[90/07/09  11:21:02  jms]
 * 
 * Revision 1.8  90/03/21  17:29:36  jms
 * 	Misc initial bug fixes.
 * 	[90/03/16  17:36:47  jms]
 * 
 * 	first checkin objectified version
 * 	[89/12/19  16:23:51  jms]
 * 
 */

extern "C" {
#include	<mach_error.h>
}

#include	<us_error.h>
#include	<tm_task_ifc.h>
#include	<tm_root_ifc.h>
#include        <tm_session_ifc.h>
#include	<tm_task_group_ifc.h>

#include	<us_event_ifc.h>
#include	<us_item_ifc.h>
#include	<us_name_ifc.h>
	
#include	<std_prot_ifc.h>
#include	<ns_types.h>
#include	<agent_ifc.h>
#include	<timer.h>

extern "C"{
#include	<sig_error.h>
#include	<sys/signal.h>
}

/* The global access object for the task master */
extern int tm_debug;

DEFINE_CLASS_MI(tm_task)
DEFINE_CASTDOWN2(tm_task, vol_agency, usTMTask)

void tm_task::init_class(usClass* class_obj)
{
    usTMTask::init_class(class_obj);
    vol_agency::init_class(class_obj);

    BEGIN_SETUP_METHOD_WITH_ARGS(tm_task);

    SETUP_METHOD_WITH_ARGS(tm_task, tm_get_task_id);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_get_kernel_port);

    SETUP_METHOD_WITH_ARGS(tm_task, tm_change_task_auth);

    SETUP_METHOD_WITH_ARGS(tm_task, tm_debug_children_of);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_get_parent);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_get_tgrp);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_set_tgrp);

    SETUP_METHOD_WITH_ARGS(tm_task, tm_get_task_emul_status);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_set_task_emul_status);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_event_to_task);
//    SETUP_METHOD_WITH_ARGS(tm_task, tm_event_to_task_thread);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_hurtme);
//    SETUP_METHOD_WITH_ARGS(tm_task, tm_task_death);

//    SETUP_METHOD_WITH_ARGS(tm_task, ns_create_agent);

    SETUP_METHOD_WITH_ARGS(tm_task, tm_timer_get);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_timer_set);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_timer_delete);
#if SHARED_DATA_TIMING_EQUIVALENCE
    SETUP_METHOD_WITH_ARGS(tm_task, tm_touch_shared);
    SETUP_METHOD_WITH_ARGS(tm_task, tm_get_shared_info);
#endif SHARED_DATA_TIMING_EQUIVALENCE
    END_SETUP_METHOD_WITH_ARGS;
}

char* tm_task::remote_class_name() const
{
	return "tm_task_proxy";
}

/*
 * Some initialization macros 
 */
#define DEFAULT_EMUL_STATUS(status) {\
    (status).w_status = 0; \
    (status).w_retcode = (unsigned short)(-1);}

#define default_task_fields \
    vol_agency(mgr_id,acctab), \
\
    tm_root_obj(tm_root_obj_a), \
    task_id(task_id_a), \
    kernel_port(kernel_port_a), \
    flags(TM_FLAG_NONE), \
\
    last_tm_post_id(0), \
    last_hurtme_id(0), \
    state(TM_TASK_RUNNING)

/*
 * Dummy for method setup
 */
tm_task::tm_task():
    tm_root_obj(NULL),
    post_event_obj(NULL),
    parent(NULL)
{
};

/*
 * Create an initial task (no parents)
 */
tm_task::tm_task(
    tm_root		*tm_root_obj_a,
    ns_mgr_id_t		mgr_id,
    access_table	*acctab,
    std_cred		*credobj,

    task_t		kernel_port_a,
    usEvent		*post_event_obj_a,

    ns_token_t		task_auth_token,

#if SHARED_DATA_TIMING_EQUIVALENCE
    vm_address_t	*shared_addr,
#endif SHARED_DATA_TIMING_EQUIVALENCE

    tm_task_group	**tgrp,
    tm_task_id_t	task_id_a,
    mach_error_t	*ret)
    :
    default_task_fields,

    post_event_obj(post_event_obj_a),

    parent(this)
{
    DEBUG2(tm_debug, (0, "tm_task(initial):this=0x%x, post_event_obj =0x%x\n",
	this, post_event_obj));

    mach_error_t session_ret;
    session = new tm_session();

    mach_object_reference(session);
    mach_object_reference(this);	/* reference for "tm_task_death" */
    mach_object_reference(tm_root_obj);
    mach_object_reference(post_event_obj);
    mach_object_reference(parent);
    DEFAULT_EMUL_STATUS(emul_status);
    mutex_init(&(this->lock));

    if (NULL == *tgrp) {
	tm_root_obj->tm_find_tgrp_internal(task_id, tgrp);
    }
    tm_set_tgrp(*tgrp);

    *ret = credobj->ns_translate_token(task_auth_token, &task_cred);

    /* set protection on first task? */

#if SHARED_DATA_TIMING_EQUIVALENCE
    tm_setup_shared(shared_addr);
#endif SHARED_DATA_TIMING_EQUIVALENCE

    session_ret = session->tasks_increment();
    if (session_ret != ERR_SUCCESS)
      *ret = session_ret;
    
    return;
}

/*
 * Construction of a forked task
 */
tm_task::tm_task(
    tm_root		*tm_root_obj_a,
    ns_mgr_id_t		mgr_id,
    access_table	*acctab,
    task_t		kernel_port_a,
    tm_task		*parent_task,	/* tm_task */
    tm_task_id_t	task_id_a,
    mach_error_t	*ret)
    :
    default_task_fields,

    post_event_obj(NULL),

    task_cred(parent_task->task_cred),

    parent(parent_task)
{
    int			prot_data[DEFAULT_NS_PROT_LEN];
    ns_prot_t		prot = (ns_prot_t)prot_data;
    int			prot_len;
    tm_task		*this_hack;

    DEBUG2(tm_debug, (0, "tm_task(forked): this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

    /* retreive session from parent */
    session = parent_task->session;

    mach_object_reference(session);
    mach_object_reference(this);	/* reference for "tm_task_death" */
    mach_object_reference(tm_root_obj);
    mach_object_reference(parent);
    mach_object_reference(task_cred);
    DEFAULT_EMUL_STATUS(emul_status);
    parent_task->ns_get_protection(prot, &prot_len);
    this_hack = this;
    this_hack->ns_set_protection(prot, prot_len);
    tgrp = NULL;

    mutex_init(&(this->lock));
    tm_set_tgrp(parent->tgrp);
    
    *ret = session->tasks_increment();
} 

/*
 * tm_post_fork_task: Finish initialization of new(ish)
 *
 * Parameters:
 *
 * Results:
 *
 * Side effects:
 *	Initialization of a new(ish) task object completed
 *
 * Note:
 *	Must be called on all tasks that were "setup_pre_fork"ed.
 *
 */
mach_error_t tm_task::tm_post_fork_task(
#if SHARED_DATA_TIMING_EQUIVALENCE
    vm_address_t	*shared_addr,
#endif SHARED_DATA_TIMING_EQUIVALENCE
    usEvent		*post_event_obj_a)
{
    if (NULL == post_event_obj_a) {
	return(US_INVALID_ARGS);
    }

    post_event_obj = post_event_obj_a;
    mach_object_reference(post_event_obj);

    DEBUG2(tm_debug, (0, "tm_post_fork_task: this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

#if SHARED_DATA_TIMING_EQUIVALENCE
    (void) tm_setup_shared(shared_addr);
#endif SHARED_DATA_TIMING_EQUIVALENCE

    return(ERR_SUCCESS);
}

tm_task::~tm_task()
{
    int dummy_val = 1;	/* XXX Dummy break point line */

#if ABSTRACT_DESTRUCTOR
    printf("tm_task::~tm_task: this = 0x%x, size = 0x%x\n", this, sizeof(*this));
#endif ABSTRACT_DESTRUCTOR
}

boolean_t tm_task::tm_task_not_dead(void)
{
    return(TM_TASK_DEAD != state); /* No phoenix processes (no need to lock) */
}

/*****************************************************************************\
*									      *
*			   Task Port and ID Calls			      *
*									      *
\*****************************************************************************/

/*
 * tm_get_task_id: Get the task_id of a task.
 *
 * Parameters:
 *
 * Results:
 *	task_id [tm_task_id_t *]: Its task id
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_task_id(
    tm_task_id_t	*task_id_a)
{
    DEBUG2(tm_debug, (0, "tm_get_task_id: this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

    *task_id_a = task_id;
    return(ERR_SUCCESS);
}

/*
 * tm_get_kernel_port: Get the Mach kernel port (mach_port_t port) for a task.
 *
 * Parameters:
 *
 * Results:
 *	kernel_port [mach_port_t *]: Its kernel port
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_kernel_port(
    mach_port_t	*kernel_port_a)
{
    DEBUG2(tm_debug, (0, "get_kernel_port: this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

//    mutex_lock(&(this->lock));
    *kernel_port_a = kernel_port;
//    mutex_unlock(&(this->lock));
    return(ERR_SUCCESS);
}

/*****************************************************************************\
*									      *
*		    Task Creation and Manipulation Calls		      *
*									      *
\*****************************************************************************/

/*
 * tm_debug_children_of: 
 *
 * Parameters:
 *	on_off [boolean_t]:		Should the children be debugged
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_debug_children_of(
    boolean_t	on_off)
{
    mutex_lock(&(this->lock));

    if (on_off)
	flags |= TM_DEBUG_CHILDREN;
    else
	flags &= ~TM_DEBUG_CHILDREN;

    mutex_unlock(&(this->lock));
    return ERR_SUCCESS;
}


/*****************************************************************************\
*									      *
*			    User and Group Calls			      *
*									      *
\*****************************************************************************/
/*
 * tm_get_task_cred: Get the tasks std_cred_obj
 *
 * Parameters:
 *
 * Results:
 *	task_cred_obj [**std_cred]
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_task_cred(
	std_cred	**task_cred_obj)
{
    DEBUG2(tm_debug, (0, "tm_get_task_cred: this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

    mutex_lock(&(this->lock));
    *task_cred_obj = task_cred;
    mach_object_reference(task_cred);
    mutex_unlock(&(this->lock));
    return(ERR_SUCCESS);
}

/*
 * tm_change_task_auth: Change the authorization/authentication of a task.
 *	Used when a new user identity is needed (login, ...)
 *
 * Parameters:
 *	task_auth_token [ns_token_t]:	The new auth token
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_change_task_auth(
    ns_token_t		task_auth_token)
{
    std_cred		*credobj;
    ns_cred_t		cred_ptr;
    mach_error_t	ret;
    int			prot_data[DEFAULT_NS_PROT_LEN];
    ns_prot_t		prot = (ns_prot_t)prot_data;
    int			prot_len;
    mach_error_t        session_ret;

    DEBUG2(tm_debug, (0, "tm_change_task_auth: this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

    ret = agent::base_object()->ns_get_cred_obj(&credobj);
    if (ret != ERR_SUCCESS) {
	return(ret);
    }

    /* dereference the old session */
    session_ret = session->tasks_decrement();
    mach_object_dereference(session);

    mutex_lock(&(this->lock));
    mach_object_dereference(task_cred);
    ret = credobj->ns_translate_token(task_auth_token, &task_cred);
    mach_object_dereference(credobj);
    ret = task_cred->ns_get_cred_ptr(&cred_ptr);


    /*
     * create the default protection
     */
    ns_get_protection(prot, &prot_len);

    /* XXX Assumes that acl[0] is "owner", acl[1] is a group. Bug? */
    prot->acl[0].authid = cred_ptr->authid[0];  
    prot->acl[1].authid = cred_ptr->authid[1];
    ns_set_protection(prot, prot_len);

    mutex_unlock(&(this->lock));

    /* create new session and increment the #of tasks */
    session = new tm_session();
    session_ret = session->tasks_increment();
    if (session_ret != ERR_SUCCESS)
      ret = session_ret;
    return(ret);
}

/*
 * tm_get_parent: Get parent task.
 *
 * Parameters:
 *	access [ns_access_t]:	Desired access rights
 *
 * Results:
 *	parent_agent [usTMTask * (tm_task_agent *)]:	The parent process of the task
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_parent(
    ns_access_t		access,
    usItem		**parent_agent)	/* (usTMTask **) */
{
    std_cred		*credobj;
    mach_error_t	ret;

    DEBUG2(tm_debug, (0, "tm_get_parent: this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

    *parent_agent = NULL;

    ret = agent::base_object()->ns_get_cred_obj(&credobj);
    if (ret != ERR_SUCCESS) {
	return(ret);
    }

    mutex_lock(&(this->lock));
    agent *agent_obj;
    ret = parent->ns_create_agent(access, credobj, &agent_obj);
    mutex_unlock(&(this->lock));
    mach_object_dereference(credobj);

    if (ERR_SUCCESS == ret) *parent_agent = agent_obj;
    return(ret);
}

/*
 * tm_get_tgrp: Get task task group.
 *
 * Parameters:
 *	access [ns_access_t]:	Desired access rights
 *
 * Results:
 *	tgrp_agent [ usTMTgrp ** (tm_tgrp_agent *)]:	The task's task group
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_tgrp(
    ns_access_t		access,
    usItem		**tgrp_agent)	/* (usTMTgrp **) */
{
    std_cred		*credobj;
    mach_error_t	ret;

    DEBUG2(tm_debug, (0, "tm_get_tgrp: this = 0x%x, post_event_obj = 0x%x\n",
	this, post_event_obj));

    *tgrp_agent = NULL;
    
    ret = agent::base_object()->ns_get_cred_obj(&credobj);
    if (ret != ERR_SUCCESS) {
	return(ret);
    }

    mutex_lock(&(this->lock));
    agent *agent_obj;
    ret = tgrp->ns_create_agent(access, credobj, &agent_obj);
    mutex_unlock(&(this->lock));
    mach_object_dereference(credobj);

    if (ERR_SUCCESS == ret) *tgrp_agent = agent_obj;
    return(ret);
}

/*
 * tm_set_tgrp: Set task task group.
 *	The task is added to the task group (and removed from the previous task
 *	group) if need be.
 *
 * Parameters:
 *	tgrp_agent [usTMTgrp]:	The desired task group
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *	Taken from tm_procs.c:tm_set_tgrp 10/26/89
 */
mach_error_t tm_task::tm_set_tgrp(
    usItem	*tgrp_agent)	/* (usTMTgrp *) */ 
{
    mach_error_t	ret;
    tm_tgrp_id_t	tgrp_id;
    tm_task_group	*next_tgrp = NULL;

    DEBUG2(tm_debug, (0, "tm_set_tgrp: this = 0x%x, post_event_obj = 0x%x\n",
	 this, post_event_obj));

    if (NULL != tgrp_agent) {
	usItem *tmp_item;
	ret = tgrp_agent->ns_get_item_ptr(&tmp_item);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(tmp_item);
		return(ret);
	}
	next_tgrp = tm_task_group::castdown(tmp_item);
	if (NULL == next_tgrp) {
	    DEBUG0(tm_debug, (0, "tm_task_tm_set_tgrp: NULL next_tgrp(%d, %d)\n",
		this, tmp_item));
	    mach_object_dereference(tmp_item);
	    return(MACH_OBJECT_NO_SUCH_OPERATION);
	}
    }

    mutex_lock(&(this->lock));

    if (NULL != tgrp) {
	if (ERR_SUCCESS != (ret = tgrp->tm_remove_tgrp_task(this))) {

	    ERROR((this, "tm_task.tm_set_tgrp: task could not be removed from its old task group"));
	    mutex_unlock(&(this->lock));
	    mach_object_dereference(next_tgrp);
	    return(ret);
	}
	mach_object_dereference(tgrp);
	tgrp = NULL;
    }

    if (NULL == next_tgrp) {
	mutex_unlock(&(this->lock));
	return(ERR_SUCCESS);
    }

    if (ERR_SUCCESS != (ret = next_tgrp->tm_add_tgrp_task(this))) {
	DEBUG0(tm_debug, (this, "tm_task.tm_set_tgrp: task could not be added to its new task group" ));
	mutex_unlock(&(this->lock));
	mach_object_dereference(next_tgrp);
	return(ret);
    }

    tgrp = next_tgrp;
    mutex_unlock(&(this->lock));

    return(ret);
}

		
/*****************************************************************************\
*									      *
*			       Emul Value Calls				      *
*									      *
\*****************************************************************************/
/*
 * tm_get_task_emul_status:  Get the value which represents the emulation 
 *	status of the process.  This is the status received by the "wait" 
 *	system call of unix.
 *
 * Parameters:
 *
 * Results:
 *      status [union wait *]:		The (partial) status to be assigned
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_task_emul_status(
    union wait *status)
{
    DEBUG2(tm_debug, (0, "tm_get_emul_status: this = 0x%x, post_event_obj = 0x%x\n",
	 this, post_event_obj));

    mutex_lock(&(this->lock));
    status->w_status = emul_status.w_status;
    mutex_unlock(&(this->lock));
    return(ERR_SUCCESS);
}

/*
 * tm_set_task_emul_status:  Set the value which represents the emulation 
 *	status of the process.  Only use those bits from the status that 
 *	are set in the mask.
 *
 * Parameters:
 *      status [union wait]:		The (partial) status to be assigned
 *      mask [union wait]:		Mask for part of status to be used
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_set_task_emul_status(
    union wait	status,
    union wait	mask)
{
    DEBUG2(tm_debug, (0, "tm_set_emul_status: this = 0x%x, post_event_obj = 0x%x\n",
	 this, post_event_obj));

    mutex_lock(&(this->lock));
    emul_status.w_status = 
	((emul_status.w_status & (~ mask.w_status)) | 
	 (status.w_status & mask.w_status));
    mutex_unlock(&(this->lock));
    return(ERR_SUCCESS);
}

/*****************************************************************************\
*									      *
*			     Task Event Calls   			      *
*									      *
\*****************************************************************************/

/* 
 * tm_hurtme: 
 *	Suspend/terminate the current task.  It is possible that the
 *	task is stopped, don't stop twice.
 *
 * Parameters:
 *	event [mach_error_t]:		Event being convied to the task
 *	status				emul_status to use (for "wait").
 *	tm_post_id [int]			Id used to prevent redundant calls
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_hurtme(
    int		event,
    union wait	status,
    int		tm_post_id)
{
    mach_error_t	err = ERR_SUCCESS;

    DEBUG2(tm_debug, (0, "tm_hurtme: this = 0x%x, post_event_obj = 0x%x\n",
	 this, post_event_obj));

    mutex_lock(&(this->lock));
    if (tm_post_id && (tm_post_id <= last_hurtme_id)) {
	mutex_unlock(&(this->lock));
	return(ERR_SUCCESS);
    }
    if (tm_post_id) last_hurtme_id = tm_post_id;
    emul_status.w_status = status.w_status;
    
    if (UNIX_SIG_STOP == event) {
	if (TM_TASK_RUNNING == state) {
	    /* XXX setup queue for posts to suspended task */

	    err = task_suspend(kernel_port);
	    state = TM_TASK_STOPPED;

	    /* send sigchild to parent if possible */
	    (void)(parent->tm_event_to_task(UNIX_SIG_CHLD, 
		task_id, emul_status.w_status));
	}
    }
    else {
	err = task_terminate(kernel_port);
    }
    mutex_unlock(&(this->lock));
    return(err);
}

/* 
 * tm_event_to_task_thread: Post an event to/for a task.  
 *	Catchable events are sent to the task uninterpreted.
 *	If the task is stopped, the event is queued for later delivery.
 *	Uncatchable events are applied directly to the task.
 *	If the thread arg is non-null then that thread is suspended
 *
 * Parameters:
 *	thread [mach_port_t]:		Thread in question (MACH_PORT_NULL if none)
 *	event [mach_error_t]:		Event being convied to the task
 *	code [int]:			It's event/error/exception code
 *	subcode [int]:			It's subcode
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_event_to_task_thread(
    mach_port_t		thread,
    mach_error_t	event,
    int			code,
    int			subcode)
{
    mach_error_t	ret;
    int			w_stat;
    tm_task		*child_task;	/* for use with sigchld */

    mutex_lock(&(this->lock));

    /* 
     * If it is already dead, do nothing.
     */
    if (TM_TASK_DEAD == state) {
        mutex_unlock(&(this->lock));
	return(TM_INVALID_TASK);
    }

    /* suspend the thread (to be unsuspended by the emul_lib as handling the exception */

    if (MACH_PORT_NULL != thread) {
	if (KERN_SUCCESS != thread_suspend(thread)) {
	    DEBUG0(tm_debug, (this, "tm_task.tm_event_to_task: failed to suspend thread 0x%x", thread ));
	}
    }

    /* 
     * Deliver the signal
     */
    switch (event) {
	case UNIX_SIG_KILL:
	case UNIX_SIG_STOP:
	{
	    union wait	status;
	    int		tm_post_id;
	    boolean_t	hurt_me = FALSE;

	    last_tm_post_id++;
	    tm_post_id = last_tm_post_id;

	    /* Try and stop/kill it in an orderly fashion, as needed */
	    if (TM_TASK_RUNNING == state) {
	        mutex_unlock(&(this->lock));
		DEBUG2(tm_debug, (this, "tm_task::tm_event_to_task_thread: calling event_post_with_timeout(%d,%d,%d,%d,%d,%d)\n", this, thread, event, code, subcode, tm_post_id ));
	        ret = post_event_obj->event_post_with_timeout(
			thread, event,code,subcode,tm_post_id);
		hurt_me = (ERR_SUCCESS == ret);
	    }
	    else {
	        mutex_unlock(&(this->lock));
	        hurt_me = TRUE;
	    }

	    /* if something unusual, force the "hurtme" action */
	    if (hurt_me) {
	        if (UNIX_SIG_KILL == event) {
		    status.w_status = 0;
		    status.w_termsig = SIGKILL;
	        }
	        else {
		    status.w_status = 0;
		    status.w_stopval = WSTOPPED;
		    status.w_stopsig = SIGSTOP;
	        }
		ret = tm_hurtme(event, status, tm_post_id);
	    }
	    return(ret);
	}

	case UNIX_SIG_CONT:

	    /* reset the status */
	    emul_status.w_status = 0;
	    state = TM_TASK_RUNNING;

	    if (KERN_SUCCESS != (ret = task_resume(kernel_port))) {
		ERROR((this, "Task_Master: SIGCONT kill error"));
	    }

	    /* XXX post the pending events */

	    break;

	case UNIX_SIG_CHLD:
	    /* 
	     * Set the emul_status of the child (caller) and then post
	     * the event.  The pid of the child should be in the
	     * "code" and the "subcode" contains the wait status
	     * if the event is sent from the childs emulation lib.
	     * Otherwise the "code" is 0 and nothing special need be done.
	     */
	     if (0 != code) {
		/* we have an auto-generated sigchild.  
		   Fix its status unless we were just called from SIG_STOP */
		/* find the child object */
		union wait status;
		status.w_status = subcode;
		if (WSTOPPED != status.w_stopval)
		    if (ERR_SUCCESS == (ret = 
			tm_root_obj->tm_task_id_to_task_internal(
				code, &child_task))) {
		    /* Not a stop and got the child object */
		    union wait mask;
		    mask.w_status = -1;

		    union wait tmp_wait;
		    tmp_wait.w_status = subcode;
		    child_task->tm_set_task_emul_status(tmp_wait, mask);

		    /* XXX add child to "wait" pending queue here */

		    mach_object_dereference(child_task);
		}
	    }
	    break;
    }

    /*
     * Lets send this one to the task to handle
     */

    /* XXX Deal with queueing stopped process signals */

    mutex_unlock(&(this->lock));
    DEBUG2(tm_debug, (this, "tm_task::tm_event_to_task_thread: calling event_post(%d,%d,%d,%d,%d)\n", this, thread, event, code, subcode ));
    if (ERR_SUCCESS != (ret = post_event_obj->event_post(
				thread, event,code,subcode))) {
        DEBUG1(tm_debug, (this, "tm_task.tm_event_to_task: event post failed" ));
    }
    return(ret);
}

mach_error_t tm_task::tm_event_to_task(
    mach_error_t	event,
    int			code,
    int			subcode)
{
    return (tm_event_to_task_thread(MACH_PORT_NULL, event, code, subcode));
}

/* 
 * tm_task_death:
 *	Tell the task object that its task is dead
 *	Called from the tm_root object
 *
 * Parameters:
 *
 * Results:
 *	return TM_INVALID_TASK iff the task is already dead
 *		other problems are ignored.
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_task_death()
{
    mach_error_t	ret;
#if SHARED_DATA_TIMING_EQUIVALENCE
    int			i;
    boolean_t		dirty;
#endif SHARED_DATA_TIMING_EQUIVALENCE

    DEBUG2(tm_debug, (0, "tm_task_death: this = 0x%x, post_event_obj = 0x%x\n",
	 this, post_event_obj));

    mutex_lock(&this->lock);
    if (state == TM_TASK_DEAD) {
	mutex_unlock(&this->lock);
	return(TM_INVALID_TASK);
    }

    state = TM_TASK_DEAD;
    mutex_unlock(&this->lock);

    /* Remove it from its tgrp */
    (void)tm_set_tgrp(NULL);

    /* Waste timers */
    (void) timer_task_delete();

    /* fix the status */
    if (WSTOPPED == (Local(emul_status)).w_stopval) {
	/* Stopped when we died, do default death */
	DEFAULT_EMUL_STATUS(Local(emul_status));
    }

    /* Tell mom-dad that we are done/dead (timeout? XXX) */
    if ((this != parent) && parent->tm_task_not_dead()) {
	(void) parent->tm_event_to_task(UNIX_SIG_CHLD, 
			task_id, emul_status.w_status);
    }

    /* Tell session object task is dead */
    session->tasks_decrement();

    mach_object_dereference(tm_root_obj);
    mach_object_dereference(post_event_obj);
    mach_object_dereference(parent);
    mach_object_dereference(session);

#if SHARED_DATA_TIMING_EQUIVALENCE
    ret = vm_deallocate(mach_task_self(), shared_local_addr, SHARED_SIZE);
    shared_info = NULL;
    if (ERR_SUCCESS != ret) {
	ERROR((this, "Task_Master: failed to deallocate shared space, err = %s", mach_error_string(ret)));
    }

    /* iop_deactivate loop */
    for (i=0; i<100; i++) {
	ret = shared_pager.iop_deactivate(TRUE, &dirty);
	if (ERR_SUCCESS == ret) break;
	cthread_yield();
    }
    if (i >= 100) {
	ERROR((this, "Task_Master: iop_deactivate failed"));
    }

#endif SHARED_DATA_TIMING_EQUIVALENCE

    mach_object_dereference(this);

    return(ERR_SUCCESS);
}
#if SHARED_DATA_TIMING_EQUIVALENCE
/* 
 * tm_touch_shared: contact TM when shared data touched
 *	Part of a hack to emulate the timing involved to do actual sharing
 *	concerneced with the subcase of seek keys.
 *	XXX SHARED_DATA_TIMING_EQUIVALENCE XXX
 *
 * Parameters:
 *	id [int]:		ID of shared data
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_touch_shared(int id) {
    DEBUG1(tm_debug, (0, "tm_touch_shared: id= %d\n", id));
    shared_info->touch = id;
    return(ERR_SUCCESS);
}

/* 
 * tm_setup_shared: Initialize a shared page between the TM and a new task.
 *	XXX SHARED_DATA_TIMING_EQUIVALENCE XXX
 *
 * Parameters:
 *	id [int]:		ID of shared data
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_setup_shared(vm_address_t *shared_client_addr) {
	mach_error_t	err;
	vm_prot_t	prot;
	mach_port_t	pager_port;


	shared_pager.null_pager_start();

	prot = VM_PROT_READ | VM_PROT_WRITE,
	shared_local_addr = NULL;
	shared_info = NULL;
	shared_pager.iop_get_port(&pager_port);
	err = vm_map(mach_task_self(), &shared_local_addr, 
		SHARED_SIZE, 0, TRUE,
		pager_port, 0, FALSE,
		prot, prot, VM_INHERIT_NONE);

	if (ERR_SUCCESS != err) {
		ERROR((this, "Task_Master: failed shared page allocation"));
		return(err);
	}

	err = vm_map(kernel_port, shared_client_addr, SHARED_SIZE, 0, TRUE,
		pager_port, 0, FALSE,
		prot, prot, VM_INHERIT_NONE);

	if (ERR_SUCCESS != err) {
		ERROR((this, "Task_Master: failed shared page client_map"));
		return(err);
	}
	shared_info = (tm_shared_info_t)shared_local_addr;
	shared_info->id = task_id;
	shared_info->touch = 900;
//	mutex_init(&(shared_info->lock));
	shared_info->parent_id = parent->task_id;
	shared_info->exec_string[0] = (char)0;

	tm_touch_shared(976);
	return(ERR_SUCCESS);
}

/* 
 * tm_get_shared_info_internal: Get th pointer to the shared info record.
 *	XXX SHARED_DATA_TIMING_EQUIVALENCE XXX
 *
 * Parameters:
 *	shared_info_arg [shared_info_t *]:		OUT for info pointer.
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_shared_info_internal(void** shared_info_arg) {
	
	*shared_info_arg = (void*)shared_info;
	return(ERR_SUCCESS);
}
/* 
 * tm_get_shared_info: Get the tm_shared_info for this task.
 *
 * Parameters:
 *
 * Results:
 *	id [tm_task_id_t *]:		OUT task id
 *	touch [int *]:			OUT shared "touch" value/id
 *	parent_id [tm_task_id_t *]	OUT parent task id
 *	exec_string [char[SHARED_EXEC_STR_MAX]]
 *					OUT string for last exec
 * Side effects:
 *
 * Note:
 *
 */
mach_error_t tm_task::tm_get_shared_info(
	tm_task_id_t	*id,
	int		*touch, 
	tm_task_id_t	*parent_id,
	char		*exec_string)	/* out char[SHARED_EXEC_STR_MAX] */
{
//	mutex_lock(&(shared_info->lock));
	*id = shared_info->id;
	*touch = shared_info->touch;
	*parent_id = shared_info->parent_id;
	strcpy(exec_string, shared_info->exec_string);
//	mutex_unlock(&(shared_info->lock));
	
	return(ERR_SUCCESS);
}
#endif SHARED_DATA_TIMING_EQUIVALENCE

#if ABSTRACT_DESTRUCTOR
usTMTask::~usTMTask(){
    printf("usTMTask::~usTMTask: this = 0x%x, size = 0x%x\n", this, sizeof(*this));
}
#endif ABSTRACT_DESTRUCTOR
