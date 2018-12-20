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
 * File:  tm_root_ifc.h
 *
 * Purpose:  Mach Task Master agency object definition
 *
 * J. Mark Stevenson
 *
 * 12/19/89
 */

/*
 * HISTORY
 * $Log:	tm_root_ifc.h,v $
 * Revision 2.5  94/10/27  12:01:53  jms
 * 	Add tm_dump_exec_strings method (not exported)
 * 	[94/10/26  14:51:28  jms]
 * 
 * Revision 2.4  94/07/13  17:33:26  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  93/01/20  17:39:29  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  17:37:40  jms]
 * 
 * Revision 2.2  92/07/05  23:35:40  dpj
 * 	Removed active_table.
 * 	Added compiler work-around for remote_class_name() (GXXBUG_VIRTUAL1).
 * 	[92/07/05  19:01:38  dpj]
 * 
 * 	Use new us_tm_root_ifc.h interfaces for the C++ taskmaster.
 * 	Translated from tm_agency.c.
 * 	[92/06/24  18:03:52  jms]
 * 
 * Revision 2.5  90/11/27  18:21:50  jms
 * 	pre_regsiter_forked_task/post_regsiter_forked_task split
 * 	[90/11/20  14:53:53  jms]
 * 
 * 	Add "last_task_id" field to tm_root_object
 * 	[90/08/20  17:40:37  jms]
 * 
 * Revision 2.4  90/08/13  15:44:54  jjc
 * 	Added methods for interval timer.
 * 	[90/07/19            jjc]
 * 
 * Revision 2.3  90/07/09  17:11:54  dorr
 * 	Adda field to keep the childs exception port.  A method for delivering kernel
 * 	exceptions.
 * 	[90/07/09  11:05:05  jms]
 * 
 * Revision 2.2  90/03/21  17:28:40  jms
 * 	Mods for useing the objectified Task Master
 * 	[90/03/16  17:13:48  jms]
 * 
 * 	First objectified Task Master checkin
 * 	[89/12/19  16:22:30  jms]
 * 
 */
#ifndef	_tm_root_ifc_h
#define	_tm_root_ifc_h

extern "C" {
#include	<mach/mach_types.h>
#include	<dll.h>
}
#include	<agency_ifc.h>
#include	<us_tm_root_ifc.h>
#include	<us_tm_task_ifc.h>
#include	<us_tm_tgrp_ifc.h>
#include	<us_event_ifc.h>
#include	<dir_ifc.h>
#include	<ns_types.h>

#include	"tm_types.h"
#include	"tm_mapping.h"

#define TASK_DIR_PATH "/TM/TASKS/"
#define TASK_DIR_PATH_LEN 10


class tm_task;
class tm_task_group;

class tm_root:  public dir, public usTMRoot {
    private:
	struct mutex		lock;
	dir			*task_dir;
	dir			*group_dir;
	port_mapping_table_t	port_table;
	mach_port_t		child_exception_port;
	mach_port_t		death_watch_port;
	tm_task_id_t		last_task_id;

	/* 
         * Once you have a task and a taskid, put it into the right
	 * taskmaster global places.
	 */
	mach_error_t introduce_task(
	    tm_task *, 		/* task */
	    tm_task_id_t, 	/* task_id */
	    mach_port_t, 	/* kernel port */
	    int);		/* ns_reserve_entry tag */

    /*
     * For server internal use only!
     */	
    public:
	/*
	 * Find an existing job group or create a new job group with the 
	 * given id
	 */
	mach_error_t tm_find_tgrp_internal(
	    tm_tgrp_id_t,	/* task_group_id */
	    tm_task_group **);	/* out *task_group */

	mach_error_t tm_deliver_kernel_exception(
	    mach_port_t,	/* task_port */
	    mach_port_t,	/* thread_port */
	    int,		/* exception */
	    int,		/* code */
	    int);		/* subcode */

	/*
	 * Get a Task Master tm_task from its task_id
	 */
	mach_error_t tm_task_id_to_task_internal(
	    tm_task_id_t,	/* task_id */
	    tm_task **);	/* task */

	/*
	 * De-register a task: to be used internally by the task master to free
	 * bindings in misc tables.  To be called by the terminate proc of the
	 * task.
	 */
	mach_error_t tm_deregister_task(
	    mach_port_t);	/* kernel_port */
    
#if SHARED_DATA_TIMING_EQUIVALENCE
	/*
	 * Dump all of the exec strings for system processes do the diag
	 * server.
	 */
	mach_error_t tm_dump_exec_strings();
#endif SHARED_DATA_TIMING_EQUIVALENCE
    
    public:
	DECLARE_MEMBERS(tm_root);
	tm_root();
	tm_root(ns_mgr_id_t, mach_error_t*);
	~tm_root();

#ifdef	GXXBUG_VIRTUAL1
	virtual char* remote_class_name() const;
#endif	GXXBUG_VIRTUAL1

	/*
	 * Get a Task Master tm_task from its task_id
	 */
REMOTE	virtual mach_error_t tm_task_id_to_task(
	    tm_task_id_t,	/* task_id */
	    ns_access_t,	/* access */
	    usItem **);		/* task_agent */

	/*
	 * Get a Task Master tm_task from its Mach kernel port.
	 */
REMOTE	virtual mach_error_t tm_kernel_port_to_task(
	    mach_port_t,	/* kernel_port */
	    ns_access_t,	/* access */
	    usItem **);		/* task_agent */

	mach_error_t tm_reserve_task_id(
	    tm_task_id_t *,	/* task_id */
	    int *);		/* tag */

	/*
	 * Initiate/complete registration of a task with the task master.
	 * The given task_id will be used if possible when specified.  The 
	 * task's Task Master notification port is also sent back to the task.
	 */
REMOTE	virtual mach_error_t tm_pre_register_forked_task(
	    mach_port_t,	/* kernel_port */
	    usItem  *,		/* (usTMTask *)parent_task_agent */
	    tm_task_id_t *);	/* task_id */

REMOTE	virtual mach_error_t tm_post_register_forked_task(
	    mach_port_t,	/* kernel_port */
	    usItem *,		/* (usEvent *)notify_obj */
#if SHARED_DATA_TIMING_EQUIVALENCE
	    vm_address_t *,	/* in/out: shared space address */
#endif SHARED_DATA_TIMING_EQUIVALENCE
	    ns_access_t,	/* access */
	    tm_task_id_t *,	/* task_id */
	    usItem **);		/* task_agent */

	/*
	 * Register a task which was created by an entity other than the 
	 * Task Master. This task is the first of a task tree (has no parent
	 * in the task master).  The given task_id will be used if possible
	 * when specified. The task's Task Master notification port is also
	 * sent back to the task.
	 */
REMOTE	virtual	mach_error_t tm_register_initial_task(
	    task_t,		/* kernel_port */
	    usItem *,		/* (usEvent*) notify_obj */
	    ns_token_t,		/* task_auth_token */
#if SHARED_DATA_TIMING_EQUIVALENCE
	    vm_address_t *,	/* in/out: shared space address */
#endif SHARED_DATA_TIMING_EQUIVALENCE
	    ns_access_t,	/* task_group_access */
	    ns_access_t,	/* task_access */
	    usItem **,		/* out: (usTMTgrp **) task_group_agent */
	    tm_task_id_t *,	/* task_id */
	    usItem **);		/* out: (usTMTask **) task_agent */

	/*
	 * Find an existing job group or create a new job group with the 
	 * given id
	 */
REMOTE	virtual mach_error_t tm_find_tgrp(
	    tm_tgrp_id_t,	/* task_group_id */
	    ns_access_t,	/* access */
	    usItem **);		/* task_group_agent */
};

#endif	_tm_root_ifc_h

