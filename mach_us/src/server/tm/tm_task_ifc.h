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
 * File:  tm_task_ifc.h
 *
 * Purpose:  Mach Task Master task object definition
 *
 * J. Mark Stevenson	21-Mar-1990
 * Michael B. Jones	07-Jul-1988
 */

/*
 * HISTORY:
 * $Log:	tm_task_ifc.h,v $
 * Revision 2.13  94/10/27  12:02:03  jms
 * 	Add tm_get_shared_info method for accessing process info shared between the
 * 	client emulator and the task_master.
 * 	[94/10/26  14:58:45  jms]
 * 
 * Revision 2.12  94/07/13  17:33:42  mrt
 * 	Updated copyright
 * 
 * Revision 2.11  94/05/17  13:36:13  jms
 * 	Add pointers to session object.
 * 	[94/05/11  14:56:26  modh]
 * 
 * Revision 2.10  94/05/16  16:38:38  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 
 * Revision 2.9  93/01/20  17:39:36  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  17:39:08  jms]
 * 
 * Revision 2.8  92/07/05  23:35:51  dpj
 * 	Use new us_tm_task_ifc.h interface for the C++ taskmaster.
 * 	Translate to G++
 * 	Implement as dir entries tm/TASK/<tid> and in the tm/GROUPS/<gid>/<tid>
 * 	[92/06/24  18:17:23  jms]
 * 
 * Revision 2.7  92/03/05  15:13:25  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	Switch mach_types.h => mach/mach_types.h
 * 	[92/02/26  19:35:56  jms]
 * 
 * Revision 2.6  91/07/01  14:15:28  jms
 * 	Add tm_hurtme.
 * 	[91/06/25  11:54:55  jms]
 * 
 * Revision 2.5  90/11/27  18:22:12  jms
 * 	Modify task setup for the pre/post fork forms of initialization.
 * 	[90/11/20  15:00:00  jms]
 * 
 * Revision 2.4  90/08/13  15:45:15  jjc
 * 	Added methods for interval timer.
 * 	[90/07/19            jjc]
 * 
 * Revision 2.3  90/07/09  17:12:39  dorr
 * 	Add emul_status field/methods
 * 	[90/07/09  11:23:44  jms]
 * 
 * Revision 2.2  90/03/21  17:30:05  jms
 * 	Comment and agent fixes
 * 	[90/03/16  17:40:03  jms]
 * 
 * 	first checkin object based TM
 * 	[89/12/19  16:25:22  jms]
 * 
 */

#ifndef	_tm_task_ifc_h
#define	_tm_task_ifc_h

#include	<mach/mach_types.h>
#include	<us_tm_task_ifc.h>
#include	<us_tm_tgrp_ifc.h>
#include	<us_item_ifc.h>
#include	<us_name_ifc.h>
#include	<us_event_ifc.h>
#include	<vol_agency_ifc.h>
#include	<null_pager_ifc.h>
#include	<sys/wait.h>

#include	"timer.h"
#include	"tm_types.h"
#include	<dll.h>

#define	TM_FLAG_NONE		(0)
#define	TM_DEBUG_CHILDREN	(0x01)
#define	TM_SUSPEND_ON_EXEC	(0x02)

typedef struct timer	*timer_t;
typedef int tm_flag_t;
class tm_root;
class tm_task_group;
class tm_task;
class tm_session;
extern void timer_start_friend(tm_task *);

class tm_task: public vol_agency, public usTMTask {
      public:
	DECLARE_MEMBERS(tm_task);

      private:
	/* 
	 * Class variables
	 */

	static timer_id_t	timer_id;
					/* ID number to use for next timer */
	static struct mutex	timer_lock;
					/* lock on the queue of timers */
	static mach_port_t	timer_port;
					/* port that server listens to */
	static dll_head_t	timer_queue;
					/* sorted queue of timers */
	static int		timer_up;
					/* timer module up? */

	friend void timer_start_friend(tm_task *);	/* hack to start the timer loop */

	/*
	 * Instance variables
	 */
	struct mutex    lock;
	tm_root		* tm_root_obj;
	tm_task_id_t	task_id;
	task_t		kernel_port;
	usEvent		*post_event_obj;
	tm_flag_t	flags;

	std_cred	*task_cred;
	int		protlen;

	tm_task_group	* tgrp;			/* tm_task_group */
	tm_task		*parent;		/* tm_task */
	tm_session      *session;               /* tm_session */

	int		last_tm_post_id;	/* timeout tm request identifier */
	int		last_hurtme_id;		/* last post id "hurt" */
	int		state;			/* State the task is in */
	union wait	emul_status;		/* goes to parent on SIGCHLD */

#if SHARED_DATA_TIMING_EQUIVALENCE
	null_pager		shared_pager;
	vm_address_t		shared_local_addr;	/* address of shared mem */
	tm_shared_info_t	shared_info;		/* recast of shared_local_addr */
#endif SHARED_DATA_TIMING_EQUIVALENCE

      /***********************
       *
       * EXTERNALLY AVAILABLE METHODS
       *
       ***********************/

      public:
	mach_error_t tm_get_task_id(
	    tm_task_id_t *);	/* out task_id */

	mach_error_t tm_get_kernel_port(
	    mach_port_t *);	/* out kernel_port */

	mach_error_t tm_change_task_auth (
	    ns_token_t);	/* task_auth_token */

	mach_error_t tm_debug_children_of(
	    boolean_t);		/* on_off */

	mach_error_t tm_get_parent(
	    ns_access_t,	/* access */
	    usItem **);		/* out: usTMTask **parent_agent */

	mach_error_t tm_get_tgrp(
	    ns_access_t,	/* access */
	    usItem **);		/* out: usTMTgrp **tgrp_agent */

	mach_error_t tm_set_tgrp(
	    usItem *);		/* (usTMTgrp *) tgrp_agent */

	mach_error_t tm_get_task_emul_status(
	    union wait *);	/* out status */

	mach_error_t tm_set_task_emul_status(
	    union wait,		/* status */
	    union wait);	/* mask */

	mach_error_t tm_hurtme(
	    int,		/* event */
	    union wait,		/* status */
	    int);		/* tm_post_id */

	mach_error_t tm_event_to_task_thread(
	    mach_port_t,	/* thread */
	    mach_error_t,	/* event */
	    int,		/* code */
	    int);		/* subcode */

	mach_error_t tm_event_to_task(
	    mach_error_t,		/* event */
	    int,			/* code */
	    int);			/* subcode */

	mach_error_t tm_timer_get(
	    timer_id_t,			/* id */
	    timer_type_t,		/* which */
	    timer_value_t);		/* out value */

	mach_error_t tm_timer_set(
	    timer_type_t,		/* which */
	    int,			/* event */
	    int,			/* code */
	    int,			/* subcode */
	    timer_id_t *,		/* inout id */
	    timer_value_t);		/* value */

	mach_error_t tm_timer_delete(
	    timer_id_t,			/* id */
	    timer_value_t);		/* out ovalue */

#if SHARED_DATA_TIMING_EQUIVALENCE
	mach_error_t tm_touch_shared(
	    int);			/* id */

	mach_error_t tm_get_shared_info(
	tm_task_id_t*,			/* out id */
	int*,				/* out touch */
	tm_task_id_t*,			/* out parent_id */
	char *);		/* out exec_string[SHARED_EXEC_STR_MAX] */
#endif SHARED_DATA_TIMING_EQUIVALENCE

      /***********************
       *
       * INTRA-SERVER ONLY METHODS
       *
       ***********************/
      public:

	virtual char* remote_class_name() const;

	tm_task();		/* Dummy for method setup */

	tm_task(		/* initial/non-forked task creation */
	    tm_root *,		/* tm_root_obj */
	    ns_mgr_id_t,	/* mgr_id */
	    access_table *,	/* access_table */
	    std_cred *,		/* credobj */

	    task_t,		/* kernel_port */
	    usEvent *,		/* post_event_obj */

	    ns_token_t,		/* task_auth_token */

#if SHARED_DATA_TIMING_EQUIVALENCE
	    vm_address_t *,	/* in/out: shared space address */
#endif SHARED_DATA_TIMING_EQUIVALENCE
	    tm_task_group **,	/* out id_tgrp */
	    tm_task_id_t,	/* task_id */
	    mach_error_t *);	/* out return */

	tm_task(		/* forked task creation */
	    tm_root *,		/* tm_root_obj */
	    ns_mgr_id_t,	/* mgr_id */
	    access_table *,	/* access_table */
	    task_t,		/* kernel_port */
	    tm_task *,		/* parent_task */
	    tm_task_id_t,	/* task_id */
	    mach_error_t *);	/* out return */

	virtual ~tm_task();

	boolean_t tm_task_not_dead(void);

	mach_error_t tm_post_fork_task(
#if SHARED_DATA_TIMING_EQUIVALENCE
	    vm_address_t *,	/* in/out: shared space address */
#endif SHARED_DATA_TIMING_EQUIVALENCE
	    usEvent *);	/* post_event_obj */

	mach_error_t tm_task_death();

	mach_error_t tm_get_task_cred(
	    std_cred **);	/* out *task_cred */

#if SHARED_DATA_TIMING_EQUIVALENCE
	mach_error_t tm_setup_shared(vm_address_t *); /* shared_addr */
	mach_error_t tm_get_shared_info_internal(void**);
#endif SHARED_DATA_TIMING_EQUIVALENCE

	mach_error_t timer_init();

	mach_error_t timer_shutdown();

	/*********************
	 *
 	 * TIMER INTERNAL METHODS
	 *
	 *********************/

	/*
	 *	Remove and free all the interval timer records for the given
	 *	task
	 */
	mach_error_t timer_task_delete();

	/*
	 *      Find the interval timer record with the given task and ID
	 */
	timer_addr_t timer_find(
	    timer_id_t);      /* id */

	/*
	 *	Insert interval timer record into queue of records sorted 
	 *	in increasing order by time left until expiration
	 */
	mach_error_t timer_insert(
		timer_t);

	/*
	 *	Server loop which dequeues the timer at the head of the queue,
	 *	sleeps until it expires, resets the timer, requeues it,  and
	 *	waits for next timer to expire.  If the queue is empty, we
	 *	sleep in msg_receive(),	waiting for a message to arrive and
	 *	signal that the queue is no longer empty.
	 */
	mach_error_t timer_server();

};

/* various states a task can be in */
#define TM_TASK_RUNNING		0
#define TM_TASK_STOPPED		1
#define TM_TASK_DEAD		2

#endif	_tm_task_ifc_h
