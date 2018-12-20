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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_tm_task_ifc.h,v $
 *
 * usTMRoot: abstract class defining the Task Master root protocol.
 * usTMTask: abstract class defining the task protocol.
 * usTMTgrp: abstract class defining the task group protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY:
 * $Log:	us_tm_task_ifc.h,v $
 * Revision 2.5  94/10/27  12:01:18  jms
 * 	Add the tm_get_shared_info method for getting process info shared between
 * 	the client and the task_master, most notably the exec string.
 * 	[94/10/26  14:37:53  jms]
 * 
 * Revision 2.4  94/07/08  15:52:02  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  93/01/20  17:36:24  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timimg of such sharing.
 * 	[93/01/18  15:49:47  jms]
 * 
 * Revision 2.2  92/07/05  23:24:07  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/29            dpj]
 * 
 * 	External interface for a task object.  Replaces the task specific portion
 * 	of us_task_ifc.h (dead file).
 * 	[92/06/24  13:52:01  jms]
 * 
 * Revision 2.1  91/04/15  14:41:08  pjg
 * Created.
 * 
 * 
 */

#ifndef	_us_tm_task_h
#define	_us_tm_task_h

#include <us_item_ifc.h>
#include <us_event_ifc.h>

extern "C" {
#include <tm_types.h>
#include <timer.h>
#include <sys/wait.h>
}


class usTMTask: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usTMTask);
	static void initClass(usClass*);

#if ABSTRACT_DESTRUCTOR
	~usTMTask();
#endif ABSTRACT_DESTRUCTOR


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
REMOTE	virtual mach_error_t tm_get_task_id(
	    tm_task_id_t *) = 0;	/* out task_id */

/*
 * tm_get_kernel_port: Get the Mach kernel port (task_t port) for a task.
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
REMOTE	virtual mach_error_t tm_get_kernel_port(
	    mach_port_t *) = 0;		/* out kernel_port */

/*
 * tm_change_task_auth: Change the authorization/authentication of a task.
 *      Used when a new user identity is needed (login, ...)
 *
 * Parameters:
 *      task_auth_token [ns_token_t]:   The new auth token
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE  virtual mach_error_t tm_change_task_auth(
            ns_token_t) = 0;            /* task_auth_token */

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
REMOTE	virtual mach_error_t tm_debug_children_of(
	    boolean_t) = 0;

/*
 * tm_get_parent: Get parent task.
 *
 * Parameters:
 *
 * Results:
 *	parent [usTMTask **]:		The parent process of the task
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_get_parent(
	    ns_access_t,	/* access */
	    usItem **) = 0;	/* out: (usTMTask **) parent */

/*
 * tm_get_tgrp: Get a task's tgrp.
 *
 * Parameters:
 *      access  [ns_access_t]:		Desired access to result tgrp
 *
 * Results:
 *	tgrp [usTMTgrp **]:	The task's tgrp
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_get_tgrp(
	    ns_access_t,	/* access */
	    usItem **) = 0;	/* out: (usTMTgrp **)tgrp */

/*
 * tm_set_tgrp: Set task's tgrp.
 *	The task is added to the tgrp (and removed from the previous job
 *	tgrp) if need be.
 *
 * Parameters:
 *	tgrp [usTMTgrp *]:	The desired tgrp
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_set_tgrp(
	    usItem *) = 0;	/* (usTMTgrp *)tgrp */

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
REMOTE	virtual mach_error_t tm_get_task_emul_status(
	    union wait *) = 0;	/* out status */

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
REMOTE	virtual mach_error_t tm_set_task_emul_status(
	    union wait,		/* status */
	    union wait) = 0;	/* mask */

/*
 * tm_hurtme: Do a "hard" event/action (STOP/KILL) to the task.
 *	Called by an emulation library in order to cause the
 *	taskmaster to actually STOP/KILL the task running in
 *	said library.
 *
 * Parameters:
 *	event [mach_error_t]:		The "hard" event (eg UNIX_SIG_{STOP,KILL})
 *	status [union wait]:		The unix "wait" status for the process
 *	tm_post_id [int]:		Id to prevent redundant "hurts" with 
 *					timeout event postings. Zero otherwise.
 *					
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_hurtme(
	    int,		/* event */
	    union wait,		/* status */
	    int) = 0;		/* tm_post_id */

/*
 * tm_event_to_task: Post an event to/for a task.  
 *	Catchable events are sent to the task uninterpreted.
 *	If the task is stopped, the event is queued for later delivery.
 *	Uncatchable events are applied directly to the task.
 *
 * Parameters:
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
REMOTE	virtual mach_error_t tm_event_to_task(
	    mach_error_t,		/* event */
	    int,			/* code */
	    int) = 0;			/* subcode */

/* 
 * tm_timer_get: Get value (time left and interval) of timer with the
 *	given ID for the given task.
 *
 * Parameters:
 *	id [timer_id_t]:		ID number of timer
 *	which [timer_type_t]:		timer type (real, virtual, profiled)
 *
 * Results:
 *	value [timer_value_t]:		returned as value of timer
 *
 *	Returns ERR_SUCCESS, US_INVALID_ARGS, or US_OBJECT_NOT_FOUND
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_timer_get(
	    timer_id_t,			/* id */
	    timer_type_t,		/* which */
	    timer_value_t) = 0;		/* out value */

/* 
 * tm_timer_set: Set timer
 *
 * Parameters:
 *	which [timer_type_t]:		timer type (real, virtual, profiled)
 *	event [int]:			Event to send to the task
 *	code [int]:			Its event/error/exception code
 *	subcode [int]:			Its subcode
 *	id [timer_id_t]:		ID number of timer(for reset)
 *	value [timer_value_t]:		value of timer
 *
 * Results:
 *	id [timer_id_t]:		ID number of timer(new timer)
 *
 *	Returns US_INVALID_ARGS, US_RESOURCE_EXHAUSTED, or ERR_SUCCESS
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_timer_set(
	    timer_type_t,		/* which */
	    int,			/* event */
	    int,			/* code */
	    int,			/* subcode */
	    timer_id_t *,		/* inout id */
	    timer_value_t) = 0;	/* value */

/* 
 * tm_timer_delete: Delete timer
 *
 * Parameters:
 *	id [timer_id_t]:		ID number of timer
 *
 * Results:
 *	ovalue [timer_value_t]:	returned as old value of timer
 *
 *	Returns US_OBJECT_NOT_FOUND or ERR_SUCCESS
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_timer_delete(
	    timer_id_t,			/* id */
	    timer_value_t) = 0;		/* out ovalue */

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
REMOTE	virtual mach_error_t tm_touch_shared(
	    int) = 0;		/* id */

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
REMOTE	virtual mach_error_t tm_get_shared_info(
	    tm_task_id_t *,		/* out id */
	    int *,			/* out touch */
	    tm_task_id_t *,		/* out parent_id */
	    char *) = 0;	/* out exec_string[SHARED_EXEC_STR_MAX] */

#endif SHARED_DATA_TIMING_EQUIVALENCE

}; /* end class usTMTask */

/*
 * Export those methods
 */
EXPORT_METHOD(tm_get_task_id);
EXPORT_METHOD(tm_get_kernel_port);
EXPORT_METHOD(tm_change_task_auth);
EXPORT_METHOD(tm_debug_children_of);
EXPORT_METHOD(tm_get_parent);
EXPORT_METHOD(tm_get_tgrp);
EXPORT_METHOD(tm_set_tgrp);
EXPORT_METHOD(tm_get_task_emul_status);
EXPORT_METHOD(tm_set_task_emul_status);
EXPORT_METHOD(tm_hurtme);
EXPORT_METHOD(tm_event_to_task);
EXPORT_METHOD(tm_timer_get);
EXPORT_METHOD(tm_timer_set);
EXPORT_METHOD(tm_timer_delete);
#if SHARED_DATA_TIMING_EQUIVALENCE
EXPORT_METHOD(tm_touch_shared);
EXPORT_METHOD(tm_get_shared_info);
#endif SHARED_DATA_TIMING_EQUIVALENCE

#endif	_us_tm_task_h
