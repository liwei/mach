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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_tm_root_ifc.h,v $
 *
 * usTMRoot: abstract class defining the Task Master root protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY:
 * $Log:	us_tm_root_ifc.h,v $
 * Revision 2.4  94/07/08  15:51:59  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  93/01/20  17:36:22  jms
 * 	Add the "shared space address" as an out arg to task registration.  Space
 * 	used for sharing between TM and task for SHARED_DATA_TIMING_EQUIVALENCE.
 * 	[93/01/18  15:46:25  jms]
 * 
 * Revision 2.2  92/07/05  23:24:05  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/29            dpj]
 * 
 * 	C++ interface of the task_master "root" functionallity.  Code translate
 * 	from tm_agency_ifc.h.
 * 	[92/06/24  13:49:53  jms]
 * 
 * Revision 2.1  91/04/15  14:41:08  pjg
 * Created.
 * 
 * 
 */

#ifndef	_us_tm_root_h
#define	_us_tm_root_h

#include <us_item_ifc.h>
#include <us_event_ifc.h>

extern "C" {
#include <tm_types.h>
}

/*
 * Some predefined access values.
 */
#define TM_DEFAULT_ACCESS NSR_REFERENCE
#define TM_SELF_ACCESS NSR_REFERENCE


/*
 * Operations upon the Task Master itself not directed at a specific task
 * or tgrp of tasks.
 */
class usTMRoot: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usTMRoot);

/*
 * tm_task_id_to_task: Get a Task Master tm_task from its task_id
 *
 * Parameters:
 *	task_id [tm_task_id_t]:			Id of desired task
 *	access	[ns_access_t]:			desired access to result obj
 *
 * Results:
 *	task [usTMTask **]:	Task of the given ID
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_task_id_to_task(
	    tm_task_id_t,	/* task_id */
	    ns_access_t,	/* access */
	    usItem **) = 0;	/* out: (usTMTask **) task */

/*
 * tm_kernel_port_to_task: Get a Task Master tm_task from its Mach kernel port.
 *
 * Parameters:
 *	kernel_port [mach_port_t]:		Kernel port for the desired task
 *	access	[ns_access_t]:		desired access to result obj
 *
 * Results:
 *	task [usTMTask]:		Task of the kernel port
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_kernel_port_to_task(
	    mach_port_t,	/* kernel_port */
	    ns_access_t,	/* access */
	    usItem **) = 0;	/* out: (usTMTask **) task */

/*
 * tm_pre_register_forked_task: Register a new task with
 * the Task Master which was forked from a previously registered task.
 * The given task_id will be used if possible when specified.
 *
 * Parameters:
 *	kernel_port [mach_port_t]:			The kernel port for the task.
 *	parent_task [usTMTask *]:		The task of the parent
 *      task_id [tm_task_id_t *]:		The desired id for the task
 *
 * Results:
 *      task_id [tm_task_id_t *]:		The actual id of the task.
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_pre_register_forked_task(
	    task_t,			/* kernel_port */
	    usItem *,			/* (usTMTask *) parent_task */
	    tm_task_id_t *) = 0;	/* inout task_id */

/*
 * tm_post_register_forked_task: Complete the registration of a preregistered task
 *
 * Parameters:
 *	kernel_port [mach_port_t]:			The kernel port for the task.
 *	post_event_obj  [usItem *]		The tasks event obj
 *      access  [ns_access_t]:                  desired access to result obj
 *
 * Results:
 *      task_id [tm_task_id_t *]:		The actual id of the task.
 *	task [usTMTask **]:		The task object for the
 *						registered task
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_post_register_forked_task(
	    task_t,		/* kernel_port */
	    usItem *,		/* (usEvent *) notify_obj */
#if SHARED_DATA_TIMING_EQUIVALENCE
	    vm_address_t *,	/* in/out: shared space address (aka vm_map) */
#endif SHARED_DATA_TIMING_EQUIVALENCE
	    ns_access_t,	/* access */
	    tm_task_id_t *,	/* out task_id */
	    usItem **) = 0;	/* out: (usTMTask **) task */

/*
 * tm_register_initial_task: Register a task which was created by an entity 
 * other than the Task Master.  This task is a root a a task tree/ was forked
 * The given task_id will be used if possible when specified.
 *
 * Parameters:
 *	kernel_port [mach_port_t]:	The kernel port for the task.
 *	post_event_obj  [usItem *]:	The tasks event obj
 *	task_auth_token	[ns_token_t]:	Authentication token for the task
 *      tgrp_access  [ns_access_t]:     Desired access to result obj
 *      task_access  [ns_access_t]:     Desired access to result obj
 *	tgrp [usTMTgrp **]		
 *		The desired tgrp, if null, new tgrp is returned 
 *		with same id as the task.
 *      task_id [tm_task_id_t *]:	The desired id for the task
 *
 * Results:
 *	tgrp [usTMTgrp **]		New tgrp iff needed
 *      task_id [tm_task_id_t *]:	The actual id of the task.
 *	task [usTMTask **]:		The task object for the
 *						registered task
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_register_initial_task(
	    task_t,		/* kernel_port */
	    usItem *,		/* (usEvent *) notify_obj */
	    ns_token_t,		/* task_auth_token */
#if SHARED_DATA_TIMING_EQUIVALENCE
	    vm_address_t *,	/* in/out: shared space address (aka vm_map) */
#endif SHARED_DATA_TIMING_EQUIVALENCE
	    ns_access_t,	/* group_access */
	    ns_access_t,	/* task_access */
	    usItem **,		/* out: (usTMTgrp **) tgrp */
	    tm_task_id_t *,	/* out task_id */
	    usItem **) = 0;	/* out: (usTMTask **) task */

/*
 * tm_find_tgrp:  Find an existing tgrp or create a new tgrp
 *	 with the given id
 *
 * Parameters:
 *      tgrp_id [tm_group_id_t]: The desired id for the tgrp
 *      access  [ns_access_t]:        Desired access to result obj
 *
 * Results:
 *	tgrp [usTMTgrp]:	The tgrp object for the
 *						registered tgrp
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_find_tgrp(
	    tm_tgrp_id_t,		/* tgrp_id */
	    ns_access_t,		/* access */
	    usItem **) = 0;		/* out: (usTMTgrp **) tgrp */
}; /* end class usTMRoot
 */

/*
 * Export those methods
 */
EXPORT_METHOD(tm_task_id_to_task);
EXPORT_METHOD(tm_kernel_port_to_task);
EXPORT_METHOD(tm_pre_register_forked_task);
EXPORT_METHOD(tm_post_register_forked_task);
EXPORT_METHOD(tm_register_initial_task);
EXPORT_METHOD(tm_find_tgrp);
#endif	_us_tm_root_h
