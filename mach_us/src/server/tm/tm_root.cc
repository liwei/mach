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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tm/tm_root.cc,v $
 *
 * Purpose: Top level task_master interface implementation.
 *
 * HISTORY:
 * $Log:	tm_root.cc,v $
 * Revision 2.7  94/10/27  12:01:51  jms
 * 	Add tm_dump_exec_strings method (not exported)
 * 	[94/10/26  14:50:29  jms]
 * 
 * Revision 2.6  94/07/13  17:33:24  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/04/29  15:48:06  jms
 * 	Mild debugging additions
 * 	[94/04/29  14:57:52  jms]
 * 
 * Revision 2.4  94/01/11  18:11:31  jms
 * 	Fix server directory protections.
 * 	[94/01/10  13:31:01  jms]
 * 
 * Revision 2.3  93/01/20  17:39:26  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 
 * 	Misc deactivated debugging code.
 * 	[93/01/18  17:37:02  jms]
 * 
 * Revision 2.2  92/07/05  23:35:36  dpj
 * 	Converted for new C++ RPC package.
 * 	[92/07/05  19:00:59  dpj]
 * 
 * 	G++ translation from tm_agency for the "root" object of the taskmaster.
 * 	Use new us_tm_root_ifc.h interface for the C++ taskmaster.
 * 	[92/06/24  18:02:33  jms]
 * 
 * Revision 2.8  92/03/05  15:12:56  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:30:57  jms]
 * 
 * Revision 2.7  91/11/13  17:21:14  dpj
 * 	Clean-up the previous notification right returned when setting-up
 * 	dead-name notifications.
 * 	Hacked around a reference-counting problem for tm_task objects.
 * 	[91/07/13  22:24:49  dpj]
 * 
 * Revision 2.6  91/07/01  14:15:11  jms
 * 	Fixed a bug in task_id allocation code, that was causing us
 * 	to scan the entire task_id space each time...
 * 	[91/06/16  21:10:12  dpj]
 * 	Cause a thread which receives a mech error to stay suspend after
 * 	"catching" the exception from the mach kernel.
 * 	[91/06/25  11:45:29  jms]
 * 
 * Revision 2.5  90/11/27  18:21:43  jms
 * 	Split forked task registration into pre/post forked forms.
 * 	Update listening for process death and kernel exceptions to work with new IPC.
 * 	Misc other 3.0 changes
 * 	[90/11/20  14:52:26  jms]
 * 
 * 	Generate task_ids when not supplied by emul_lib.
 * 	[90/08/20  17:39:29  jms]
 * 
 * Revision 2.4  90/08/13  15:44:46  jjc
 * 	Added methods for the interval timer to the class declaration
 * 	of tm_root and added call to timer_init() to tm_root_initialize().
 * 	Included timer module, since a class can't be split into more than one
 * 	file.
 * 	[90/07/19            jjc]
 * 
 * Revision 2.3  90/07/09  17:11:46  dorr
 * 	Add mechinisms to deliver kernel exceptions
 * 	Add mechinisms to deliver "sigchld" to parents at the appropriate times
 * 	[90/07/09  10:49:04  jms]
 * 
 * Revision 2.2  90/03/21  17:28:32  jms
 * 	Mods for useing the objectified Task Master and TM->emul signal support.
 * 	[90/03/16  17:13:00  jms]
 * 
 * 	Mods for object based task master.
 * 	[89/12/19  16:22:18  jms]
 * 
 * Revision 2.4  89/10/30  16:38:50  dpj
 * 	Use the new generic active_table.
 * 	[89/10/27  19:41:08  dpj]
 * 
 * 	Reorganized to use a standard active_table.
 * 	[89/08/27  20:17:21  dpj]
 * 
 * Revision 2.3  89/07/09  14:21:12  dpj
 * 	Updated error statements for new ERROR macros.
 * 	[89/07/08  13:11:57  dpj]
 * 
 * Revision 2.2  89/06/30  18:39:00  dpj
 * 	Initial revision.
 * 	[89/06/21  22:29:25  dpj]
 * 
 *
 */

extern "C" {
#include	<us_error.h>
#include	<exception_error.h>
#include	"tm_mapping.h"
#include	<mach/notify.h>
#include	<cthreads.h>
#include	<stdio.h>
}

#include	<tm_root_ifc.h>
#include	<access_table_ifc.h>
#include	<tm_task_ifc.h>
#include	<tm_task_group_ifc.h>
#include	<agent_ifc.h>
#include	<tm_types.h>
#include	<us_event_proxy_ifc.h>

extern int tm_debug;
extern boolean_t forward_kernel_exceptions;
extern std_auth *auth_obj;
extern ns_authid_t fs_access_default_root_authid;

/*
 * Class initialization.
 */
DEFINE_CLASS_MI(tm_root)
DEFINE_CASTDOWN2(tm_root, dir, usTMRoot)

void tm_root::init_class(usClass* class_obj) {
	usTMRoot::init_class(class_obj);
	dir::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(tm_root);
	SETUP_METHOD_WITH_ARGS(tm_root, tm_task_id_to_task);
	SETUP_METHOD_WITH_ARGS(tm_root, tm_kernel_port_to_task);
	SETUP_METHOD_WITH_ARGS(tm_root, tm_pre_register_forked_task);
	SETUP_METHOD_WITH_ARGS(tm_root, tm_post_register_forked_task);
	SETUP_METHOD_WITH_ARGS(tm_root, tm_register_initial_task);
	SETUP_METHOD_WITH_ARGS(tm_root, tm_find_tgrp);

//	SETUP_METHOD_WITH_ARGS(tm_root,ns_create);
//	SETUP_METHOD_WITH_ARGS(tm_root,ns_list_types);
	END_SETUP_METHOD_WITH_ARGS;
}

#ifdef	GXXBUG_VIRTUAL1
char* tm_root::remote_class_name() const
	{ return dir::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1

/*
 * Protection for task/group subdirectories.
 */
ns_prot_t	subdir_prot = NULL;


/* 
 * Self handle for kernel exception handeling.
 */
static	tm_root		*tm_root_obj;		
extern	mach_error_fn_t	exc_server;

static tm_death_watch(
    mach_port_t		*death_watch_port)
{
    mach_dead_name_notification_t	notify_msg;
    kern_return_t	kr;

#if	LOCK_THREADS
    mutex_lock(thread_lock);
#endif	LOCK_THREADS

    kr = mach_port_allocate(mach_task_self(), 
		MACH_PORT_RIGHT_RECEIVE, death_watch_port);
    if (kr != KERN_SUCCESS) {
	ERROR((tm_root_obj, "tm_root.tm_death_watch: allocate notify port" ));
	return((int)kr);
    }

    kr = mach_port_insert_right(mach_task_self(), *death_watch_port,
		*death_watch_port, MACH_MSG_TYPE_MAKE_SEND);

    if (kr != KERN_SUCCESS) {
	ERROR((tm_root_obj, "tm_root.tm_death_watch: add send right notify port" ));
	return((int)kr);
    }

    bzero(&notify_msg, sizeof(mach_msg_header_t));
    while (TRUE) {
	notify_msg.not_header.msgh_size = sizeof(notify_msg);
	notify_msg.not_header.msgh_local_port = *death_watch_port;
	if (MACH_MSG_SUCCESS == mach_msg_receive(&notify_msg)) {
	    if (MACH_NOTIFY_DEAD_NAME == notify_msg.not_header.msgh_id) {

		tm_root_obj->tm_deregister_task(notify_msg.not_port);
	    }
	}
    }
   return(ERR_SUCCESS);
}

/*
 * Routine used to start a watch for exception from registered processes.
 * This routine is used in the initialization of the tm_root.
 */
static tm_exception_watch(
    mach_port_t		*child_exception_port)
{
    kern_return_t	kr;

    kr = mach_port_allocate(mach_task_self(), 
		MACH_PORT_RIGHT_RECEIVE, child_exception_port);
    if (kr != KERN_SUCCESS) {
	ERROR((tm_root_obj, "tm_root.tm_exception_watch: allocate child_exception port" ));
	return((int)kr);
    }

    kr = mach_port_insert_right(mach_task_self(), *child_exception_port,
		*child_exception_port, MACH_MSG_TYPE_MAKE_SEND);
    if (kr != KERN_SUCCESS) {
	ERROR((tm_root_obj, "tm_root.tm_exception_watch: add send right child_exception port" ));
	return((int)kr);
    }
    
    (void)server_loop(NULL, 1, &exc_server, NULL, *child_exception_port);
    return((int)kr);
}


/*
 * House keeping procs
 */
tm_root::tm_root():
    task_dir(NULL),
    group_dir(NULL)
{};

tm_root::tm_root(ns_mgr_id_t mgr_id, mach_error_t* ret)
    :
    dir(mgr_id, ret)
{
    cthread_t		new_thread;
    int			acl_len = 2;

    mutex_init(&(this->lock));

    /*
     * Initialize protection for tm/task/group subdirectories.
     */
    subdir_prot = (ns_prot_t)malloc(sizeof(struct ns_prot_head) +
				(acl_len * sizeof(struct ns_acl_entry)));

    subdir_prot->head.version = NS_PROT_VERSION;
    subdir_prot->head.generation = 0;
    subdir_prot->head.acl_len = acl_len;

    subdir_prot->acl[0].authid = fs_access_default_root_authid;
    subdir_prot->acl[0].rights = NSR_ADMIN |
		NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;
    subdir_prot->acl[1].authid = NS_AUTHID_WILDCARD;
    subdir_prot->acl[1].rights =
		NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;

    *ret = ns_set_protection(subdir_prot,
                                     NS_PROT_SIZE(subdir_prot) / sizeof(int));
    if (*ret != ERR_SUCCESS) {
        us_internal_error("ns_set_protection(ROOT)",*ret);
    }

    /*
     * Setup the directorys of task_ids and task_groups
     */
    task_dir = new dir(mgr_id, access_tab);
    *ret = task_dir->ns_set_protection(subdir_prot,
                                     NS_PROT_SIZE(subdir_prot) / sizeof(int));
    if (*ret != ERR_SUCCESS) {
        us_internal_error("ns_set_protection(TASKS)",*ret);
    }
    *ret = ns_insert_entry("TASKS",task_dir);
    if (*ret != ERR_SUCCESS) {
        us_internal_error("ns_insert_entry(TASKS)",*ret);
    }

    group_dir = new dir(mgr_id, access_tab);
    *ret = group_dir->ns_set_protection(subdir_prot,
                                     NS_PROT_SIZE(subdir_prot) / sizeof(int));
    if (*ret != ERR_SUCCESS) {
        us_internal_error("ns_set_protection(TGROUPS)",*ret);
    }
    *ret = ns_insert_entry("GROUPS",group_dir);
    if (*ret != ERR_SUCCESS) {
        us_internal_error("ns_insert_entry(GROUPS)",*ret);
    }

    /*
     * Setup the (hash) table of ports => task_ids
     */
     port_table = new_port_mapping_table();

    /*
     * setup to handle child exceptions
     */
    new_thread = cthread_fork((cthread_fn_t)tm_exception_watch, 
    				(any_t)(&child_exception_port));
    cthread_set_name(new_thread, "tm_exception_watch");
    cthread_detach(new_thread);

    /*
     * Listen for death
     */
    new_thread = cthread_fork((cthread_fn_t)tm_death_watch, 
				(any_t)(&death_watch_port));
    cthread_set_name(new_thread, "tm_death_watch");
    cthread_detach(new_thread);

    last_task_id = ((tm_task_id_t)0);

    /* Hide it away for kernel exception handling */
    tm_root_obj = this;
    mach_object_reference(tm_root_obj);
    
    tm_task	dummy_task;
    dummy_task.timer_init();		/* initialize interval timer */

    return;
}

tm_root::~tm_root()
{
    mach_object_dereference(task_dir);
    mach_object_dereference(group_dir);

    tm_task	dummy;
    dummy.timer_shutdown();
}

/*
 * Get a Task Master tm_task from its task_id
 */
mach_error_t tm_root::tm_task_id_to_task_internal(
    tm_task_id_t	task_id,
    tm_task		**task)		/* out: tm_task */
{
    mach_error_t        ret;
    ns_type_t		ns_type;
    agency		*task_agency;
    char		task_id_str[TASK_ID_STR_MAX];

    sprintf(task_id_str, "%d", task_id);
    ret = task_dir->ns_lookup_entry(task_id_str, strlen(task_id_str),
			&task_agency, &ns_type);

    if ((ERR_SUCCESS != ret) || (NST_TASK != ns_type)) {
	mach_object_dereference(task_agency);
	return(TM_INVALID_TASK_ID);
    }
    *task = tm_task::castdown(task_agency);
    if (NULL == *task) {
	DEBUG0(tm_debug, (0, "tm_root::tm_task_id_to_task_internal: NULL task castdown(%d, %d)\n",
		this, task_agency));
	mach_object_dereference(task_agency);
	return(TM_INVALID_TASK_ID);
    }

    return(ret);
}
/*
 * Get a Task Master tm_task_agent from its task_id
 */
mach_error_t tm_root::tm_task_id_to_task(
    tm_task_id_t	task_id,
    ns_access_t         access,
    usItem		**task_agent)		/* out: tm_task_agent */
{
    tm_task		*task;
    std_cred		*credobj;
    mach_error_t        ret;

    *task_agent = NULL;

    if (ERR_SUCCESS != (ret =
	tm_task_id_to_task_internal(task_id, &task))) {
	return(ret);
    }

    ret = agent::base_object()->ns_get_cred_obj(&credobj);
    if (ret != ERR_SUCCESS) {
        mach_object_dereference(task);
        return(ret);
    }

    agent *agent_obj;
    ret = task->ns_create_agent(access, credobj, &agent_obj);

    mach_object_dereference(task);
    mach_object_dereference(credobj);

    if (ERR_SUCCESS == ret) *task_agent = agent_obj;
    return(ret);
}

/*
 * Get a Task Master tm_task_agent from its Mach kernel port.
 */
mach_error_t tm_root::tm_kernel_port_to_task(
    mach_port_t		kernel_port,
    ns_access_t         access,
    usItem		**task_agent)		/* tm_task_agent */
{
    tm_task_id_t	task_id;
    port_types_t	kport;
    mach_error_t        ret;

    *task_agent = NULL;


    if (! lookup_port_mapping(port_table, kernel_port, &kport, &task_id)) {
	return(TM_INVALID_KERNEL_PORT);
    }
    ret = tm_task_id_to_task(task_id, access, task_agent);

    return(ret);
}

/*
 * Internal method used to reserve a task id from the Local(task_id_table) 
 * active table.  Used when registering new tasks.
 *
 * IF the given "task_id" was non zero then reserve that value
 *	(TM_INVALID_TASK_ID if in use).
 * ELSE, reserve the next available value.
 *
 * The "tag" must be used to "cancel" or "install" the reservation via
 * "Local(task_id_table)" active table.
 */

#define RET_CHECK(str,err) 0

#ifndef RET_CHECK
#define RET_CHECK(str,err) \
	if (ERR_SUCCESS != (err)) { \
	    DEBUG1(1,(0,"tm_root::Register_Forked error: %s. err 0x%x, %s\n", \
			(str),(err),mach_error_string(err))); \
	}
#endif RET_CHECK

#ifndef RET_CHECK
#define RET_CHECK(str,err) \
	if (ERR_SUCCESS != (err)) {	\
	    printf("Register_forked error: %s, err 0x%x,'%s'. suspending.\n", \
			(str), err, mach_error_string(err)); \
	    task_suspend(mach_task_self()); \
	}
#endif RET_CHECK

#define TM_MAX_TASK_ID ((tm_task_id_t)0xffff)

mach_error_t tm_root::tm_reserve_task_id(
    tm_task_id_t	*task_id,	/* New task id */
    int			*tag)		/* task_id_table access table reservation tag */
{
    mach_error_t	ret;
    char		task_id_str[TASK_ID_STR_MAX];

    if (NULL_TASK_ID != *task_id) {
	/*
	 * Caller (emul-lib) supplied one (thinks he knows better 
	 * and does on top of 2.5)
	 */
	sprintf(task_id_str, "%d", *task_id);

	if (ERR_SUCCESS != (ret = 
	        task_dir->ns_reserve_entry(task_id_str,	tag))) {

	    if (NS_ENTRY_EXISTS == ret) {
		/* Task is already there punt */
		return(TM_INVALID_TASK_ID);
	    }

	    mach_error("Error using task_id directory",ret);
	    return(ret);
	}
	return(ERR_SUCCESS);
    }

    /* 
     * Get the next available new task id
     */
    mutex_lock(&(this->lock));
    *task_id = Local(last_task_id) + 1;

    while (*task_id != Local(last_task_id)) {
	/* Try to reserve this one */
	sprintf(task_id_str, "%d", *task_id);
	ret = task_dir->ns_reserve_entry(task_id_str, tag);

	if ((NS_ENTRY_EXISTS != ret) && (ERR_SUCCESS != ret)) {
	    /* We have a bad one */
	    mach_error("Error finding new task_id in directory",ret);
	    mutex_unlock(&(this->lock));
	    return(ret);
	}

	/* Got one? */
        if (NS_ENTRY_EXISTS != ret) {
	    last_task_id = *task_id;
	    mutex_unlock(&(this->lock));
	    return(ERR_SUCCESS);
	}

	/* Next task id value */
	(*task_id)++;
	if (TM_MAX_TASK_ID < (*task_id)) *task_id = 1;
    }
    mutex_unlock(&(this->lock));
    return(TM_NO_MORE_TASK_IDS);
}

mach_error_t tm_root::introduce_task(
	tm_task		*task,
	tm_task_id_t	task_id,
	mach_port_t	kernel_port,
	int		tag)
{
    mach_error_t	ret;

    if (ERR_SUCCESS != (ret = 
	    task_dir->ns_install_entry(tag, task, NST_TASK))) {
	ERROR((this, "tm_root::introduce_task: failed to enter task in task dir" ));

	return(ret);
    }

    enter_port_mapping(port_table, kernel_port, MACH_KERNEL_PORT, task_id);

    /* setup to get the task exceptions */
    if (forward_kernel_exceptions) {
	ret = task_set_special_port(kernel_port, TASK_EXCEPTION_PORT, 
					child_exception_port);
	if (ret != ERR_SUCCESS) {
	    return(ret);

	}
    }

    /* setup to learn of port death */
    {
	mach_port_t	previous_port_dummy = MACH_PORT_NULL;
	mach_port_request_notification(mach_task_self(),
	    kernel_port, MACH_NOTIFY_DEAD_NAME, 1,
	    death_watch_port, MACH_MSG_TYPE_MAKE_SEND_ONCE,
	    &previous_port_dummy);
	    if (previous_port_dummy != MACH_PORT_NULL) {
		    mach_port_deallocate(mach_task_self(),
			    previous_port_dummy);
	    }
    }
    return(ERR_SUCCESS);
}


/*
 * Initiate registration of a forked task.
 * The given task_id will be used if possible when specified.
 * Tm_post_register_forked_task must be called after 
 *	tm_pre_register_forked_task.
 */
mach_error_t tm_root::tm_pre_register_forked_task(
    mach_port_t		kernel_port,
    usItem		*parent_task_agent,	/* tm_task_agent */
    tm_task_id_t	*task_id)		/* in/out parameter */
{
    mach_error_t	ret;
    int			tag;
    tm_task		*task = NULL;
    tm_task		*parent_task = NULL;

    DEBUG2(1,(0,"tm_root::tm_pre_register_forked_task 1\n"));

    usItem *tmp_item;
    ret = parent_task_agent->ns_get_item_ptr(&tmp_item);
    if (ret != ERR_SUCCESS) {
	RET_CHECK("1",ret);
    	return(ret);
    }

    parent_task = tm_task::castdown(tmp_item);
    if (NULL == parent_task) {
	DEBUG1(1,(0,"tm_root::tm_pre_register_forked_task err 1\n"));
	RET_CHECK("2",MACH_OBJECT_NO_SUCH_OPERATION);
	return(MACH_OBJECT_NO_SUCH_OPERATION);
    }

    if (ERR_SUCCESS != (ret = tm_reserve_task_id(task_id, &tag))) {
	DEBUG1(1,(0,"tm_root::tm_pre_register_forked_task err 2\n"));
	RET_CHECK("3",ret);
	return(ret);
    }

    /* Get a new task */
    task = new tm_task(this, mgr_id, access_tab,
			kernel_port, parent_task, *task_id, &ret);
    if (ERR_SUCCESS != ret) {
	task_dir->ns_cancel_entry(tag);
	mach_object_dereference(parent_task);
	mach_object_dereference(task);
	RET_CHECK("4",ret);
	return(ret);
    }

    ret = introduce_task(task, *task_id, kernel_port, tag);
    if (ERR_SUCCESS != ret) {
	task_dir->ns_cancel_entry(tag);
    }

    mach_object_dereference(parent_task);
    mach_object_dereference(task);

    RET_CHECK("5",ret);
    return(ret);
}

/*
 * Complete registration of a forked task.
 * Tm_post_register_forked_task must be called after 
 *	tm_pre_register_forked_task.
 */
mach_error_t tm_root::tm_post_register_forked_task(
    mach_port_t		kernel_port,
    usItem		*notify_item,	/* tm_notify_obj */
//    usEvent		*notify_obj,
#if SHARED_DATA_TIMING_EQUIVALENCE
    vm_address_t	*shared_addr,
#endif SHARED_DATA_TIMING_EQUIVALENCE
    ns_access_t		access,
    tm_task_id_t	*task_id,
    usItem		**task_agent)	/* tm_task_agent */
{
    mach_error_t	ret;
    port_types_t	port_type;
    tm_task		*task = NULL;
    std_cred		*credobj;
    char		task_id_str[TASK_ID_STR_MAX];

    /* Get the task object */
    if (! lookup_port_mapping(port_table, kernel_port, 
				&port_type, task_id)) {
	/* kernel_port not found */
	return(US_INVALID_ARGS);
    }
    if (MACH_KERNEL_PORT != port_type) {
	return(US_INVALID_ARGS);
    }

    if (ERR_SUCCESS != (ret =
	tm_task_id_to_task_internal(*task_id, &task))) {
	return(ret);
    }

    usEvent *notify_obj;
    notify_obj = usEvent_proxy::castdown(notify_item);

    /* Set the task notify object iff not yet set */
#if SHARED_DATA_TIMING_EQUIVALENCE
    ret = task->tm_post_fork_task(shared_addr, notify_obj);
//    tm_dump_exec_strings();
#else
    ret = task->tm_post_fork_task(notify_obj);
#endif SHARED_DATA_TIMING_EQUIVALENCE
    if (ERR_SUCCESS != ret) {
	mach_object_dereference(task);
	return(ret);
    }

    /* Get our credobj */
    ret = agent::base_object()->ns_get_cred_obj(&credobj);
    if (ret != ERR_SUCCESS) {
	mach_object_dereference(task);
	mach_object_dereference(credobj);
	return(ret);
    }

    agent *agent_obj;
    ret = task->ns_create_agent(access, credobj, &agent_obj);
    if (ERR_SUCCESS == ret) *task_agent = agent_obj;

    /* Go Home */
    mach_object_dereference(task);
    mach_object_dereference(credobj);

    return(ret);
}

/*
 * Register a task which an initial task which otheres will be forked from.
 * The task was created by an entity other than the Task Master.
 * The given task_id will  be used if possible when specified.
 */
mach_error_t tm_root::tm_register_initial_task(
    task_t		kernel_port,
    usItem		*notify_item,
//    usEvent		*notify_obj,
    ns_token_t		task_auth_token,
#if SHARED_DATA_TIMING_EQUIVALENCE
    vm_address_t	*shared_addr,
#endif SHARED_DATA_TIMING_EQUIVALENCE
    ns_access_t		task_group_access,
    ns_access_t		task_access,
    usItem		**task_group_agent,
    tm_task_id_t	*task_id,
    usItem		**task_agent)		/* tm_task_agent */
{
    mach_error_t	ret;
    int			tag;
    tm_task		*task = NULL;
    tm_task_group	*task_group = NULL;
    std_cred		*credobj;
    agent		*agent_obj;
    agency		*task_agency;
    usEvent		*notify_obj;

    /* Get our credobj */
    ret = agent::base_object()->ns_get_cred_obj(&credobj);
    if (ret != ERR_SUCCESS) {
	mach_object_dereference(credobj);
	return(ret);
    }

    if (ERR_SUCCESS != (ret = tm_reserve_task_id(task_id, &tag))) {
	mach_object_dereference(credobj);
	return(ret);
    }

    notify_obj = usEvent::castdown(notify_item);

    /* Get a new task */
    task = new tm_task(this, mgr_id, access_tab, credobj,
		kernel_port, notify_obj, task_auth_token,
#if SHARED_DATA_TIMING_EQUIVALENCE
		shared_addr,
#endif SHARED_DATA_TIMING_EQUIVALENCE
		&task_group, *task_id, &ret);

    if (ERR_SUCCESS != ret) {
	task_dir->ns_cancel_entry(tag);
	mach_object_dereference(credobj);
	return(ret);
    }
    task_agency = (agency *)task;
    task = tm_task::castdown(task_agency);

    ret = introduce_task(task, *task_id, kernel_port, tag);
    if (ERR_SUCCESS != ret) {
	task_dir->ns_cancel_entry(tag);
	mach_object_dereference(task_group);
	mach_object_dereference(task);
    }

    /* if we dont need to return anything new, go home */
    if (((ns_access_t)0 == task_access) &&
	(((ns_access_t)0 == task_group_access) ||
	 (NULL != *task_group_agent))) {
	mach_object_dereference(task);
	mach_object_dereference(task_group);
	return(ERR_SUCCESS);
    }

    /* Get the task_agent */
    if ((ns_access_t)0 != task_access) {
	agent_obj = NULL;
	ret = task->ns_create_agent(task_access, credobj, &agent_obj);
	if (ret != ERR_SUCCESS) {
	    mach_object_dereference(task);
	    mach_object_dereference(task_group);
	    mach_object_dereference(credobj);
	    return(ret);
	}
	*task_agent = agent_obj;
    }

    /* Get the task_group_agent */
    if (((ns_access_t)0 != task_group_access) &&
	(NULL == *task_group_agent)) {

	agent_obj = NULL;
	ret = task_group->ns_create_agent(task_group_access, credobj, &agent_obj);
	if (ret != ERR_SUCCESS) {
	    mach_object_dereference(task);
	    mach_object_dereference(task_group);
	    mach_object_dereference(credobj);
	    if (NULL != *task_agent) {
		mach_object_dereference(*task_agent);
		*task_agent = NULL;
	    }
	    return(ret);
	}
	*task_group_agent = agent_obj;
    }

    /* Let's go home */
    mach_object_dereference(task);
    mach_object_dereference(task_group);
    mach_object_dereference(credobj);

    return(ERR_SUCCESS);
}

/*
 * De-register a task: to be useed internally by the task master to free
 * bindings in misc tables.  To be called upon the death of the task
 * kernel_port
 */
mach_error_t tm_root::tm_deregister_task(
    mach_port_t		kernel_port)
{
    tm_task_id_t	task_id = NULL_TASK_ID;
    tm_task		*this_task = NULL;
    port_types_t	port_type;
    mach_error_t	ret;

    /* 
     * Get the taskid from the kernal port if possible
     */
    if (! lookup_port_mapping(port_table, kernel_port, 
		&port_type, &task_id)) {
	/* kernel_port not found */
	DEBUG0(tm_debug, (0, "tm_root::tm_deregister_task: Bad kernel_port"));
	return(ERR_SUCCESS);
    }
    if (MACH_KERNEL_PORT != port_type) {
	DEBUG0(tm_debug, (0, "tm_root::tm_deregister_task: Bad kernel_port type"));
	return(ERR_SUCCESS);
    }


    /* Get the task itself */
    if (ERR_SUCCESS != (ret = 
		tm_task_id_to_task_internal(task_id, &this_task))) {
	DEBUG0(tm_debug, (0, "tm_root::tm_deregister_task: Bad task_id"));
	return(ret);
    }

    /*
     * Notify the task obj of its impending death (stop if already dead)
     */
    ret = this_task->tm_task_death();
    if (ERR_SUCCESS != ret) {
	return(ret);
    }

    /* Take it out of the port mapping */
    (void) remove_port_mapping(port_table, kernel_port);

    /* Take it out of the task space */
    char	task_id_str[TASK_ID_STR_MAX];
    sprintf(task_id_str, "%d", task_id);
    (void) task_dir->ns_remove_entry(task_id_str);

    mach_object_dereference(this_task);
    
    /* waste the kernel port */
    mach_port_destroy(mach_task_self(), kernel_port);
    return(ret);
}
#if SHARED_DATA_TIMING_EQUIVALENCE
mach_error_t tm_root::tm_dump_exec_strings()
{
    mach_error_t	ret;
    dir_iterator	*iterator;
    dir_entry_t		entry;
    tm_task		*task;
    tm_shared_info_t	sh_info;

    iterator = new dir_iterator(task_dir);
    printf("ID: Exec string\n");
    while (1) {
	ret = iterator->dir_iterate(&entry);
	if ((ERR_SUCCESS != ret) || (NULL == entry) || (NULL == entry->obj)) {
	    Free(iterator);
	    return(ret);
	}

	task = tm_task::castdown(entry->obj);
	ret = task->tm_task::tm_get_shared_info_internal((void**)&sh_info);

	printf("%d: addr=0x%x, touch=%d, %s\n",sh_info->id, sh_info, sh_info->touch, sh_info->exec_string);
    }
    return(ERR_SUCCESS);
}
#endif SHARED_DATA_TIMING_EQUIVALENCE


/*
 * Find an existing job group or create a new job group with the given id
 * This routine is only to be called from inside the Task Master
 */
mach_error_t tm_root::tm_find_tgrp_internal(
    tm_tgrp_id_t	task_group_id,
    tm_task_group	**task_group)
{
    mach_error_t	ret;
    int			tag;
    ns_type_t		ns_type;
    agency		*tgrp_agency;
    char		tgrp_id_str[TASK_GROUP_ID_STR_MAX];
    int			tgrp_id_str_len;

    sprintf(tgrp_id_str, "%d", task_group_id);
    tgrp_id_str_len = strlen(tgrp_id_str);
    
    /* Loop till we find/make the taskgroup in question */
    while (1) {
	ret = group_dir->ns_reserve_entry(tgrp_id_str, &tag);

	/* Die on unexpected error */
	if ((ERR_SUCCESS != ret) && (NS_ENTRY_EXISTS != ret)) {
		return(ret);
	}

	if (ERR_SUCCESS == ret) {
	    /* Got it reserved build the new group dir and put it in place */
	    *task_group = new tm_task_group(task_group_id, mgr_id, access_tab);
	    (void) (*task_group)->ns_set_protection(subdir_prot,
                                NS_PROT_SIZE(subdir_prot) / sizeof(int));
//	    (void) (*task_group)->std_prot::ns_set_protection(subdir_prot,
//				NS_PROT_SIZE(subdir_prot) / sizeof(int));
	    group_dir->ns_install_entry(tag, *task_group, NST_DIRECTORY);
	    break;
	}

	if (NS_ENTRY_EXISTS == ret) {
	    /* the group already exists, lets get it */
	    ret = group_dir->ns_lookup_entry(tgrp_id_str, tgrp_id_str_len,
			&tgrp_agency, &ns_type);

	    /* Try again if it went away */
	    if (NS_NOT_FOUND == ret) continue;

	    if (ERR_SUCCESS != ret) return(ret);

	    if (NST_DIRECTORY != ns_type) {
		mach_object_dereference(tgrp_agency);
		return(TM_INVALID_TASK_GROUP_ID);
	    }
	    *task_group = tm_task_group::castdown(tgrp_agency);
	    if (NULL == *task_group) {
		DEBUG0(tm_debug, (0, "tm_root::tm_find_tgrp_internal: NULL task_group castdown(%d, %d)\n",
			this, tgrp_agency));
		mach_object_dereference(tgrp_agency);
		return(TM_INVALID_TASK_GROUP_ID);
	    }

	    /* Got a good one. */
	    break;
	}
    }
    return(ERR_SUCCESS);
}

/*
 * Find an existing job group or create a new job group with the given id
 * This method is primarily designed for external use.
 */
mach_error_t tm_root::tm_find_tgrp(
    tm_tgrp_id_t	task_group_id,
    ns_access_t		access,
    usItem		**task_group_agent)	/* tm_task_group_agent */
{
    mach_error_t	ret;
    std_cred		*credobj;
    tm_task_group	*task_group = NULL;

    *task_group_agent = NULL;

    ret = tm_find_tgrp_internal(task_group_id, &task_group);
    if (ret != ERR_SUCCESS) {
	return(ret);
    }

    /* Get our credobj */
    ret = agent::base_object()->ns_get_cred_obj(&credobj);
    if (ret != ERR_SUCCESS) {
	mach_object_dereference(credobj);
	mach_object_dereference(task_group);
	return(ret);
    }

    agent *agent_obj = NULL;
    ret = task_group->ns_create_agent(access, credobj, &agent_obj);
    if (ret != ERR_SUCCESS) {
	mach_object_dereference(credobj);
	mach_object_dereference(task_group);
	return(ret);
    }
    *task_group_agent = agent_obj;

    mach_object_dereference(task_group);
    mach_object_dereference(credobj);

    return(ret);
}

mach_error_t
tm_root::tm_deliver_kernel_exception(
    mach_port_t	task_port,
    mach_port_t	thread_port,
    int		exception,
    int		code,
    int		subcode)
{
    tm_task_id_t	task_id;
    port_types_t	kport;
    tm_task		*task_obj;

    mach_error_t        ret;

    DEBUG0(tm_debug, (this, "tm_root.deliver_kernel_exception: task_port 0x%x, thread_port 0x%x, exception 0x%x, code 0x%x, subcode 0x%x",
	task_port, thread_port, exception, code, subcode));


    if (! lookup_port_mapping(port_table, task_port, &kport, &task_id)) {
	ERROR((this, "tm_root.deliver_kernel_exception 1: failed to find task which received exception (kport 0x%x)", task_port ));
	return(TM_INVALID_KERNEL_PORT);
    }

    /* get the task object */
    ret = tm_task_id_to_task_internal(task_id, &task_obj);
    if (ERR_SUCCESS != ret) {
	ERROR((this, "tm_root.deliver_kernel_exception 2: failed to find task which received exception (kport 0x%x)", task_port ));
	return(ret);
    }

    /* Make it stay suspended after we return */
    thread_suspend(thread_port);

    /* invoke the exception upon it */
    if (ERR_SUCCESS != (ret = task_obj->tm_event_to_task_thread(thread_port, 
					(except_sub | exception), code, subcode))) {
	ERROR((this, "tm_root.deliver_kernel_exception: failed to post mach exception to task (kport 0x%x)", task_port ));
    }
    mach_object_dereference(task_obj);

    return(ERR_SUCCESS);
}

extern "C" {

kern_return_t
catch_exception_raise(
    mach_port_t	exception_port,
    mach_port_t	thread,
    mach_port_t	task,
    int		exception,
    int		code,
    int		subcode)
{
    DEBUG0(tm_debug, (tm_root_obj, "tm_root.catch_exception_raise: task 0x%x, thread 0x%x, exception 0x%x, code 0x%x, subcode 0x%x",
	task, thread, exception, code, subcode));

    tm_root_obj->tm_deliver_kernel_exception(task, thread, exception, code, subcode);
    return (KERN_SUCCESS);
}

} /* extern "C" end */
