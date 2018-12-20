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
 * File:        emul_proc.cc
 *
 * Purpose:
 *	User space emulation of unix process management primitives
 *
 * HISTORY:
 * $Log:	emul_proc.cc,v $
 * Revision 2.10  94/10/27  12:01:32  jms
 * 	Setup for the "shared_info" area shared with the task_master.
 * 	[94/10/26  14:44:31  jms]
 * 
 * Revision 2.9  94/07/08  16:57:13  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.8  94/05/17  13:35:49  jms
 * 	If error is TM_TOO_MANY_TASK then do not suspend process when forking.
 * 	[94/05/11  14:41:57  modh]
 * 
 * Revision 2.7  94/01/11  17:49:02  jms
 * 	Misc bug fixes.
 * 	[94/01/09  18:39:17  jms]
 * 
 * Revision 2.6  93/01/20  17:36:48  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 
 * 	Misc. debugging code to make it easier to determine when a child dies durring
 * 	the forking process.
 * 	[93/01/18  15:59:31  jms]
 * 
 * Revision 2.5  92/07/05  23:25:23  dpj
 * 	Use new us_tm_{root,task,tgrp}_ifc.h interfaces for the C++ taskmaster.
 * 	Can you say "castdown"?
 * 	[92/06/24  14:32:06  jms]
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	Removed call to _init_task_id(). This is now done lazily.
 * 	[92/06/24  15:55:08  dpj]
 * 
 * 	Eliminated diag_format().
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:36:30  dpj]
 * 
 * Revision 2.4  92/03/05  14:56:02  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:23:30  jms]
 * 
 * Revision 2.3  91/11/13  16:43:27  dpj
 * 	Cleaned-up.
 * 	[91/11/12  17:48:54  dpj]
 * 
 * Revision 2.2  91/11/06  11:30:20  jms
 * 	Bug fix in child counts when killing stopped processes.
 * 	[91/11/04  17:06:18  jms]
 * 
 * 	Checking before c++ tty merge
 * 	[91/10/29  11:46:56  jms]
 * 
 *
 * Revision 2.1.2.1  91/09/17  14:08:23  jms
 * 	Fix bug in child count when stopping/continuing jobs.
 * 
 * Revision 2.1.1.3  91/09/26  19:34:16  pjg
 * 	Upgraded to US41.
 * 
 * 
 * Revision 2.1.1.2  91/04/16  18:27:39  pjg
 * 	Upgraded to US39.
 * 
 * Revision 2.1.1.1  91/04/15  10:29:28  pjg
 * 	Initial C++ revision.
 * 
 * Revision 1.56  91/07/01  14:06:51  jms
 * 	Added statistics gathering logic.
 * 	[91/06/21  17:09:39  dpj]
 * 
 * 	Clear the rusage structure returned in wait3(), instead
 * 	of returning garbage.
 * 	Added a call to reinitialize the diag system after fork,
 * 	without using the standard cloning system (we may want diag
 * 	to be ready before that).
 * 	[91/06/16  21:00:28  dpj]
 * 	Add child_count so "wait" knows where there are none.
 * 	[91/06/24  16:23:43  jms]
 * 
 * Revision 1.55  91/04/12  18:47:34  jjc
 * 	Replaced Paul Neves' EFAULT handling with macros for copying
 * 	arguments in and out of system calls to figure out whether
 * 	they're good or not.
 * 	[91/04/01            jjc]
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:48:27  jjc]
 * 
 * Revision 1.54.1.5  91/02/05  14:58:55  neves
 * 	Local variable clean up.
 * 
 * Revision 1.54.1.4  91/02/05  14:44:57  neves
 * 	Fixed emul_getpgrp to return ESRCH correctly.
 * 
 * Revision 1.54.1.3  91/02/05  14:40:07  neves
 * 	Fixed emul_getpgrp bug when getting another task's info.
 * 
 * Revision 1.54.1.2  91/02/05  14:26:42  neves
 * 	Changed KERN_FAILURE error returns to the actual errors.
 * 
 * Revision 1.54.1.1  91/02/05  12:31:41  neves
 * 	Inserted VALID_ADDRESS macro where appropriate.
 * 
 * Revision 1.54  90/11/27  18:18:23  jms
 * 	Split forking into separate routines run in parent xor child for all configurations.
 * 	Add pure kernel forking.
 * 	[90/11/20  13:46:09  jms]
 * 
 * 	Setup for pure kernel execution by adding "MACH3_xxx" switches and arranging
 * 	child to start from a separate routine call as well as other "task_create"
 * 	changes in forking land.
 * 	[90/08/20  17:22:48  jms]
 * 
 * Revision 1.53  90/07/26  12:29:55  dpj
 * 	Picked-up a fix from jms:
 * 		Revision 1.52.1.1  90/07/13  17:22:00  jms
 * 		Fix the reaping of zombies in the underlying 2.5MachOS.
 * 	[90/07/24  14:07:47  dpj]
 * 
 * Revision 1.52.1.1  90/07/13  17:22:00  jms
 * 	Fix the reaping of zombies in the underlying 2.5MachOS.
 * 
 * Revision 1.52  90/07/11  17:29:39  dorr
 * 	Reap 2.5 zombies only when needed.
 * 
 * Revision 1.51  90/07/09  17:02:18  dorr
 * 	register the initial thread.  fix a little debug output.
 * 	[90/03/01  14:52:26  dorr]
 * 
 * 	Debug -> DEBUG[012]
 * 	Add clone registration for newly forked task.  
 * 	Add unix process "status" logic".
 * 	Add emul_wait and friends.
 * 	[90/07/06  14:54:33  jms]
 * 
 * Revision 1.50  90/03/21  17:21:00  jms
 * 	Mods for useing the objectified Task Master and TM->emul signal support.
 * 	[90/03/16  16:27:49  jms]
 * 
 * 	first taskmaster mods
 * 	[89/12/19  16:07:58  jms]
 * 
 * Revision 1.49  90/03/14  17:28:34  orr
 * 	for some reason on the 386, the values returned through
 * 	the hosed up return path of htg_fork() are different. 
 * 	use a more robust, but more expensive method for
 * 	determining who is the parent and who is the child (do
 * 	some extra htg_getpid()'s).  this all goes away when
 * 	we have task_create().
 * 	[90/03/14  16:53:00  orr]
 * 
 * Revision 1.48  90/01/02  21:49:45  dorr
 * 	gak.  move all but the process mgt related code
 * 	into separate modules.
 *
 * Revision 1.47.1.3  90/01/02  14:11:42  dorr
 * 	nc
 *
 * Revision 1.47.1.2  89/12/19  17:05:21  dorr
 * 	checkin before christmas
 * 
 * Revision 1.47.1.1  89/12/18  15:50:13  dorr
 * 	move all process management stuff to its own area.
 * 
 * Revision 1.47.1.2  89/12/19  17:05:21  dorr
 * 	checkin before christmas
 * 
 * Revision 1.47.1.1  89/12/18  15:50:13  dorr
 * 	move all process management stuff to its own area.
 * 
 */


#include <clone_ifc.h>
#include <us_name_ifc.h>
#include <us_byteio_ifc.h>
#include <uxsignal_ifc.h>
#include <clone_master_ifc.h>

#include "emul_base.h"
#include "emul_proc.h"

extern "C" {
#include <base.h>
#include <debug.h>
#include <errno.h>
#include <us_ports.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

extern char *mach_error_string();
extern void
	cthread_fork_prepare(),
	cthread_fork_parent(),
	cthread_fork_child();

extern mach_error_t emul_wait_common(union wait*, int, struct rusage*, syscall_val_t*);

mach_error_t emul_child_init(void);
}

extern mach_error_t insert_ports(task_t);
extern void emul_stack_fork_child();
#if SHARED_DATA_TIMING_EQUIVALENCE
extern vm_address_t vm_allocate_base_address;
#endif SHARED_DATA_TIMING_EQUIVALENCE

int fork_debug = 0;

#undef	mach_task_self

#ifndef USE_DBG_PRINT
#define dbg_print(str)
#else
/*
 * Define a way to print (for debugging) that will print something somewhere
 * no matter how poorly initialized we are.
 */
#define dbg_print(str) \
    if (fork_debug) \
	printf(str);
#endif USE_DBG_PRINT

static struct mutex	fork_child_lock = MUTEX_INITIALIZER;
static tm_task_id_t 	emul_fork_task_id;	/* Task ID for this task */
static int	 	child_count = 0;

extern "C" mach_error_t emul_fork_common(thread_state_t, unsigned int,
					 syscall_val_t*);


#define CHILD_PROBE(str,child)

#ifndef CHILD_PROBE
int probe_child = 1;
mach_port_t probe_child_port = MACH_PORT_NULL;

#define CHILD_PROBE(str,child) \
	child_probe(str, child)

static void child_probe(char *str, mach_port_t child)
{
    if (probe_child) {
	mach_error_t	err;
	int		retry;

	if (MACH_PORT_NULL == probe_child_port) {
		(void) mach_port_allocate(mach_task_self(),
				MACH_PORT_RIGHT_RECEIVE, &probe_child_port);
		(void) mach_port_insert_right(mach_task_self(), 
				probe_child_port, probe_child_port,
				MACH_MSG_TYPE_MAKE_SEND);
	}
	/* Try a few times, may not stick the first time */
	for (retry=0; retry<20;retry++) {
	    err = mach_port_insert_right((child),
			probe_child_port, probe_child_port,
			MACH_MSG_TYPE_COPY_SEND);

	    if ((ERR_SUCCESS == err) || (KERN_NAME_EXISTS == err)) {
		if (0 != retry) printf("\n");
		break;
	    }
	    else {
		if (0 == retry) printf("ploop %s:",str);
		printf("%d ", retry);
		cthread_yield();
	    }
	}

	if ((KERN_SUCCESS != err) && (KERN_NAME_EXISTS != err)){
		printf("fork: Probe_error '%s', err 0x%x,'%s'. suspending.\n", 
			(str), err, mach_error_string(err));
		task_suspend(mach_task_self());
	}

    }
}
#endif CHILD_PROBE

mach_error_t emul_fork_common(thread_state_t child_state,
			      unsigned int child_state_count,
			      syscall_val_t* rv)
{
    mach_error_t 	err = ERR_SUCCESS;
    kern_return_t	result;

    tm_task_id_t 	post_fork_id;

    task_t		child_task;
    thread_t		child_thread;

#if MACH3_UNIX
    int child_pid;
#endif MACH3_UNIX

    emul_fork_task_id = NULL_TASK_ID;
    rv->rv_val1 = -1;

    mutex_lock(&fork_child_lock);
    SyscallTrace("fork");

    /* Setup for child to tell its task_id to parent */

    cthread_fork_prepare();	/* Grab cthread critical section locks */
    result = task_create(mach_task_self(), TRUE, &child_task);

    cthread_fork_parent();		/* Release cthread locks */

     CHILD_PROBE("1", child_task);

    if (result != KERN_SUCCESS) {
	DEBUG0(emul_debug, (Diag, "emul_fork:task create failure %d\n", mach_task_self() ));
	return(result);
    }
    CHILD_PROBE("2", child_task);
    result = thread_create(child_task, &child_thread);

    CHILD_PROBE("3", child_task);

    if (result != KERN_SUCCESS) {
	(void) task_terminate(child_task);
	(void) mach_port_deallocate(mach_task_self(), child_task);
	DEBUG0(emul_debug, (Diag, "emul_fork:thread create failure %d\n", mach_task_self() ));
	return(result);
    }

    CHILD_PROBE("4", child_task);

    if (! emul_fork_set_thread_state(child_thread, 
		child_state, child_state_count, 
		task_id_to_pid(emul_fork_task_id), 1)) {

	(void) task_terminate(child_task);
	(void) mach_port_deallocate(mach_task_self(), child_thread);
	(void) mach_port_deallocate(mach_task_self(), child_task);
	DEBUG0(emul_debug, (Diag, "emul_fork:thread set state failure %d\n", mach_task_self() ));
	return(err);
    }

    CHILD_PROBE("5", child_task);

    err = tm_obj->tm_pre_register_forked_task(child_task, 
					      tm_task_obj, 
					      &emul_fork_task_id);

    CHILD_PROBE("5.1", child_task);

    if (err) {
	DEBUG0(emul_debug, (Diag, "tm_pre_register_forked_task: %#x %s\n", err,
			mach_error_string(err)));

	if (err != TM_TOO_MANY_TASKS_IN_SESSION) {
	  printf("Pre_register_fork failed, suspending. %#x %s\n", err,
		 mach_error_string(err));
	  task_suspend(mach_task_self());
	}

	task_terminate(child_task);
	rv->rv_val1 = emul_error_to_unix(err);
	
	return(err);

    } else {
	DEBUG1(emul_debug, (Diag, "tm_pre_register_forked_task: %d\n",
		emul_fork_task_id));
    }
    rv->rv_val1 = (int)emul_fork_task_id;
    rv->rv_val2 = 0;

    CHILD_PROBE("6", child_task);

    if (fork_debug & 2) task_suspend(mach_task_self());
    
    dbg_print("fork_debug: parent:3\n");
    insert_ports(child_task);  /* Insert well-known ports into child */

    CHILD_PROBE("7", child_task);
    dbg_print("fork_debug: parent:4\n");
    err = emul_pre_fork(child_task);

    CHILD_PROBE("8", child_task);

    if (err != ERR_SUCCESS) {
    	task_terminate(child_task);
    	rv->rv_val1 = emul_error_to_unix(err);
    	DEBUG0(emul_debug,(Diag,"emul_io_pre_fork: failed 0x%x\n",err));
    	goto finish;
    }

    /* Mark that we have a new child and start it running */
    child_count++;
    err = thread_resume(child_thread);

#if KEEP_RESUMING
    while (err != KERN_SUCCESS) {
    	if (err != KERN_FAILURE) {
	    DEBUG0(1,(Diag,
		"task_resume failed with 0x%x (%s)\n",
		err, mach_error_string(err)));
		
	    rv->rv_val1 = emul_error_to_unix(err);
	    break;
    	}
    	swtch_pri(127);		/* Be a good citizen */
	err = task_resume(child_task);
    }
#else KEEP_RESUMING
    if (err != KERN_SUCCESS) {
	DEBUG0(1,(Diag,
	    "task_resume failed with 0x%x (%s)\n",
	    err, mach_error_string(err)));
	rv->rv_val1 = emul_error_to_unix(err);
	child_count--;
    }
#endif KEEP_RESUMING

finish:		
    (void) mach_port_deallocate(mach_task_self(), child_thread);
    (void) mach_port_deallocate(mach_task_self(), child_task);
    mutex_unlock(&fork_child_lock);

    DEBUG1(emul_debug,(Diag,
		"return from fork: err = %d, val1 = %d, val2 = %d\n",
		err, rv->rv_val1, rv->rv_val2));

    return err;
}

mach_error_t
emul_child_init(void)
{
	mach_error_t	err;
#if SHARED_DATA_TIMING_EQUIVALENCE
	vm_address_t	shared_addr;	/* the address of the shared space */
#endif SHARED_DATA_TIMING_EQUIVALENCE

	mach_init();		/* Get standard mach port set */

	/* Note "fork_child_lock" already held */

//	mach_object_statistics_post_fork();

	diag_fork_child();

	dbg_print("fork_debug: child:1\n");
	cthread_fork_child();	/* Reinit child cthread state
				   (including the reply port) */

	dbg_print("fork_debug: child:2\n");
	emul_stack_fork_child();   /* Reset list of emul stacks */

	if (fork_debug & 1) {
		dbg_print("fork_debug: Additional debug suspend 'suspend'\n");
		task_suspend(mach_task_self());
	}

#if MACH3_UNIX || MACH3_VUS || MACH3_US_SUSPEND_CHILD
	dbg_print("child about to suspend\n");
	/*
	 * Wait for parent to resume us
	 */
	if ((err = task_suspend(mach_task_self())) != ERR_SUCCESS) {
		drop_dead();
	} 
	dbg_print("child resumed\n");
#endif MACH3_UNIX || MACH3_VUS || MACH3_US_SUSPEND_CHILD

	open_debug_connection(emul_fork_task_id);

	if ((err = emul_post_fork()) != ERR_SUCCESS) {
		dbg_print("child failed post_fork\n");
		DEBUG0(1,(Diag, "fork: reinitialization failed" ));
		drop_dead();
	}

	dbg_print("child post_forked\n");
	DEBUG1(emul_debug, (Diag, "fork: Child Resumed. mach_task_self = %d\n", mach_task_self()));

	/*
	 * Get into to register the task with the task master and then
	 * register it.
	 */
/*	mach_object_dereference(tm_parent_obj);  XXX */
	tm_parent_obj = tm_task_obj;
	clone_master_obj->list_delete(usClone::castdown(tm_task_obj));
	tm_task_obj = NULL;


#if MACH3_UNIX || MACH3_VUS
	err = tm_obj->tm_pre_register_forked_task(mach_task_self(),
				tm_parent_obj,&emul_fork_task_id);
#endif MACH3_UNIX || MACH3_VUS

	usItem	*tm_task_item;
	if (ERR_SUCCESS == err) {
#if SHARED_DATA_TIMING_EQUIVALENCE
	    shared_addr = vm_allocate_base_address;
#endif SHARED_DATA_TIMING_EQUIVALENCE
	    err = tm_obj->tm_post_register_forked_task(mach_task_self(), 
					   uxsignal_obj, 
#if SHARED_DATA_TIMING_EQUIVALENCE
					   &shared_addr,
#endif SHARED_DATA_TIMING_EQUIVALENCE
					   TM_SELF_ACCESS, 
					   &emul_fork_task_id, &tm_task_item);
#if SHARED_DATA_TIMING_EQUIVALENCE
	    shared_info = (tm_shared_info_t)shared_addr;
#endif SHARED_DATA_TIMING_EQUIVALENCE
	}

#if MACH3_US
	open_debug_connection(emul_fork_task_id);
#endif MACH3_US

        if (err) {
                DEBUG1(emul_debug, (Diag, "tm_register_forked_task: %#x %s\n", err,
                                  mach_error_string(err)));
                drop_dead();
        } else {
                DEBUG1(emul_debug, (Diag, "tm_register_forked_task: (%x, %x, %d)\n",
                    tm_task_item, tm_parent_obj, emul_fork_task_id));
#if SHARED_DATA_TIMING_EQUIVALENCE
		shared_info->touch = 697;
#endif SHARED_DATA_TIMING_EQUIVALENCE
        }
	tm_task_obj = usTMTask::castdown(tm_task_item);
	clone_master_obj->list_add(usClone::castdown(tm_task_obj));

#if	Signals
	/*
	 * XXX with task_create, can rename thread port instead of registering
	 * new thread
	 */
	(void)uxsignal_obj->signal_register(mach_thread_self());
#endif	Signals

	mutex_unlock(&fork_child_lock);
	return(ERR_SUCCESS);
}

/* alias by which it is known in libc.a */
mach_error_t emul__exit(int rc, int rv)	
{
	return emul_rexit(rc,(syscall_val_t*)rv);
}


mach_error_t emul_rexit(int rc, syscall_val_t *rv)
{
	union wait		status;
	union wait		mask;

	SyscallTrace("rexit");

	DEBUG1(emul_debug,(Diag,"exit (%d), tm_task_obj=0x%0x\n", rc, tm_task_obj));

	status.w_status = 0;
	status.w_retcode = rc;
	mask.w_status = -1;
	DEBUG1(emul_debug,(Diag,"exit 1 (%d), tm_task_obj=0x%0x\n", rc, tm_task_obj));
	mach_error_t ret = tm_task_obj->tm_set_task_emul_status(status, mask);
	DEBUG1(emul_debug,(Diag,"tm_task_obj->tm_set_task_emul_status=0x%0x\n", ret));

	emul_cleanup_on_exit();
    
	drop_dead();
	return(ERR_SUCCESS);
}

/*
 * Called by "emul_wait" to implement the wait system call in a machine
 * independent fashion (without the argument brain death).
 * the args use the same semantics as "wait3" if it were actually
 * a syscall (it ain't)
 */
mach_error_t
emul_wait_common(union wait *_status, int options, struct rusage *_rusage, 
		 syscall_val_t *rv)
{
	mach_error_t	err = ERR_SUCCESS;
	tm_task_id_t	task_id = 0;
	union wait *status;
	struct rusage *rusage;

	SyscallTrace("wait3");

	COPYOUT_INIT(status, _status, 1, union wait, rv, EFAULT);
	if (_rusage) {
		COPYOUT_INIT(rusage, _rusage, 1, struct rusage, rv, EFAULT);
	}

	/* Return error if no kids */
	mutex_lock(&fork_child_lock);
	if (! child_count) {
		rv->rv_val1 = ECHILD;
		mutex_unlock(&fork_child_lock);
		return(unix_err(ECHILD));
	}
	mutex_unlock(&fork_child_lock);

	if (WUNTRACED & options) {
		/* get both stops and terminations */
		if (ERR_SUCCESS != 
		    (err = uxsignal_obj->get_child_info(&task_id,(int*)status, 
						     !(WNOHANG & options)))) {
			/* something wrong (interrupted) split */
			return(err);
		}
	}
	else {
		/* terminations, loop while we have stops */
		while(TRUE) {
			status->w_status = 0;
			if (ERR_SUCCESS != 
			    (err = uxsignal_obj->get_child_info( 
			      &task_id, (int*)status, !(WNOHANG & options)))) {
				/* something wrong (interrupted) split */
				return(err);
			}

			if (0 == task_id) break;		/* No one home, we are done */

			if (! WIFSTOPPED(*status)) 	break;	/* Not Stopped, must be dead */
		}
	}

	if ((0 != task_id) && (! WIFSTOPPED(*status))) {
		mutex_lock(&fork_child_lock);
		child_count--;
		mutex_unlock(&fork_child_lock);
	}
	rv->rv_val1 = task_id_to_pid(task_id);
	
	COPYOUT(status, _status, 1, union wait, rv, EFAULT);
	if (_rusage) {
		bzero(rusage,sizeof(struct rusage));	/* XXX */
		COPYOUT(rusage, _rusage, 1, struct rusage, rv, EFAULT);
	}
	return err;
}


mach_error_t emul_getpid(syscall_val_t *rv)
{
    tm_task_id_t	task_id;
    tm_task_id_t	parent_id;
    mach_error_t	err;

    SyscallTrace("getpid");

    err = tm_task_obj->tm_get_task_id(&task_id);
    if (err) {
        rv->rv_val1 = emul_error_to_unix(err);
        rv->rv_val2 = 0;
        return(err);
    }
    
    err = tm_parent_obj->tm_get_task_id(&parent_id);
    if (err) {
        rv->rv_val1 = emul_error_to_unix(err);
        rv->rv_val2 = 0;
        return(err);
    }
    
    rv->rv_val1 = task_id_to_pid(task_id);
    rv->rv_val2 = task_id_to_pid(parent_id);
    return (ERR_SUCCESS);
}

mach_error_t emul_getpgrp(int pid, syscall_val_t *rv)
{
    int			pgrp_id;
    mach_error_t 	err;
    tm_tgrp_id_t	my_jgrp_id;
    tm_tgrp_id_t	not_me_jgrp_id;
    tm_task_id_t	my_task_id;

    SyscallTrace("getpgrp");

    err = tm_task_obj->tm_get_task_id(&my_task_id);
    if (err) {
        rv->rv_val1 = emul_error_to_unix(err);
        rv->rv_val2 = 0;
        return(err);
    }
    
    /*
     * Are we dealing with me or not
     */
    if ((! pid) || (my_task_id == pid_to_task_id(pid))) {
	/* It is me */
	usTMTgrp*	tm_tgrp_obj = 0;
	usItem		*tm_tgrp_item;
        err = tm_task_obj->tm_get_tgrp(TM_DEFAULT_ACCESS, 
					     &tm_tgrp_item);
	if (err) {
	    rv->rv_val1 = emul_error_to_unix(err);
	    rv->rv_val2 = 0;
	    return(err);
	}
	tm_tgrp_obj = usTMTgrp::castdown(tm_tgrp_item);

        err = tm_tgrp_obj->tm_get_tgrp_id(&my_jgrp_id);
	if (err) {
	    mach_object_dereference(tm_tgrp_obj);
            rv->rv_val1 = emul_error_to_unix(err);
            rv->rv_val2 = 0;
            return(err);
	}
	pgrp_id = job_group_to_pgrp(my_jgrp_id);
	mach_object_dereference(tm_tgrp_obj);
    }
    else {
	/* Asking about someone else */
	usTMTask* tm_not_me_task_obj = 0;
	usTMTgrp* tm_not_me_jgrp_obj = 0;

	usItem	*tm_not_me_task_item;
        err = tm_obj->tm_task_id_to_task(pid_to_task_id(pid),
					 TM_DEFAULT_ACCESS, 
					 &tm_not_me_task_item);
	if (err) {
		if (err == TM_INVALID_TASK_ID)
			err = unix_err(ESRCH);
	    rv->rv_val1 = emul_error_to_unix(err);
	    rv->rv_val2 = 0;
	    return(err);
	}
	tm_not_me_task_obj = usTMTask::castdown(tm_not_me_task_item);

	usItem *tm_not_me_jgrp_item;
        err = tm_not_me_task_obj->tm_get_tgrp(TM_DEFAULT_ACCESS, 
						   &tm_not_me_jgrp_item);
	if (err) {
	    mach_object_dereference(tm_not_me_task_obj);
	    rv->rv_val1 = emul_error_to_unix(err);
	    rv->rv_val2 = 0;
	    return(err);
	}
	tm_not_me_jgrp_obj = usTMTgrp::castdown(tm_not_me_jgrp_item);

        err = tm_not_me_jgrp_obj->tm_get_tgrp_id(&not_me_jgrp_id);
	if (err) {
            rv->rv_val1 = emul_error_to_unix(err);
            rv->rv_val2 = 0;
            return(err);
	}
	pgrp_id = job_group_to_pgrp(not_me_jgrp_id);
	mach_object_dereference(tm_not_me_task_obj);
	mach_object_dereference(tm_not_me_jgrp_obj);
    }

    rv->rv_val1 = pgrp_id;
    rv->rv_val2 = 0;
    return ERR_SUCCESS;
}

mach_error_t emul_setpgrp(int pid, int pgrp, syscall_val_t *rv)
{
    boolean_t		is_own_pid;
    mach_error_t	err;
    usTMTask*	tm_local_task_obj =0;
    usTMTgrp*	tm_local_jgrp_obj =0;
    tm_task_id_t	task_id;

    SyscallTrace("setpgrp");

    /*
     * Are we dealing with me or not
     */
    err = tm_task_obj->tm_get_task_id(&task_id);
    if (err) {
        rv->rv_val1 = emul_error_to_unix(err);
        rv->rv_val2 = 0;
        return(err);
    }
    
    if ((! pid) || (task_id == pid_to_task_id(pid))) {
	/* It is me */
	mach_object_reference(tm_task_obj);
	tm_local_task_obj = tm_task_obj;
	is_own_pid = TRUE;
    }
    else {
	/* Talking about someone else */
	usItem *tm_local_task_item;
        err = tm_obj->tm_task_id_to_task(pid_to_task_id(pid),
					  TM_DEFAULT_ACCESS, 
					  &tm_local_task_item);
	if (err) {
	    rv->rv_val1 = emul_error_to_unix(err);
	    rv->rv_val2 = 0;
	    return(err);
	}
	tm_local_task_obj = usTMTask::castdown(tm_local_task_item);
	is_own_pid = FALSE;
    }

    /*
     * Find the job group.
     */
    usItem *tgrp_item;
    err = tm_obj->tm_find_tgrp(pgrp_to_job_group(pgrp),
				     TM_DEFAULT_ACCESS,&tgrp_item);
    if (err) {
	mach_object_dereference(tgrp_item);
	rv->rv_val1 = emul_error_to_unix(err);
	rv->rv_val2 = 0;
	return(err);
    }
    tm_local_jgrp_obj = usTMTgrp::castdown(tgrp_item);

    /*
     * Assign set the job group of the task
     */
    err = tm_local_task_obj->tm_set_tgrp(tm_local_jgrp_obj);
    if (err) {
	mach_object_dereference(tm_local_task_obj);
	mach_object_dereference(tm_local_jgrp_obj);
	rv->rv_val1 = emul_error_to_unix(err);
	rv->rv_val2 = 0;
	return(err);
    }

    mach_object_dereference(tm_local_task_obj);
    mach_object_dereference(tm_local_jgrp_obj);
    rv->rv_val1 = 0;
    rv->rv_val2 = 0;
    return ERR_SUCCESS;
}
