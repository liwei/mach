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
 * HISTORY:
 * $Log:	emul_signal.cc,v $
 * Revision 2.4  94/07/08  16:57:20  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  92/07/05  23:25:30  dpj
 * 	Use new us_tm_{root,task,tgrp}_ifc.h interfaces for the C++ taskmaster.
 * 	[92/06/24  14:34:14  jms]
 * 
 * Revision 2.2  91/11/06  11:32:39  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:35:48  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:28:45  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:30:03  pjg]
 * 
 * 	Create mbj_signal branch
 * 	[89/03/31  16:03:51  mbj]
 * 
 * 	Check in initial revision.
 * 	[89/03/30  16:43:22  mbj]
 * 
 * Revision 2.12  91/07/01  14:06:58  jms
 * 	Modify self signaling logic so that it nolonger involves the TM
 * 	except when it request the TM to actually stop/kill it (tm_hurt_me).
 * 	Add appropriate logic to do same when signalling a group which contains oneself
 * 	[91/06/24  16:28:29  jms]
 * 
 * Revision 2.11  91/04/12  18:47:43  jjc
 * 	Replaced Paul Neves' EFAULT handling with macros for copying
 * 	arguments in and out of system calls to figure out whether
 * 	they're good or not.
 * 	[91/04/01            jjc]
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:48:38  jjc]
 * 
 * Revision 2.10.1.2  91/02/05  15:14:27  neves
 * 	Added do_self_signal routine.
 * 
 * Revision 2.10.1.1  91/02/05  15:11:04  neves
 * 	Inserted VALID_ADDRESS macro where appropriate.
 * 
 * Revision 2.10  90/12/21  13:52:20  jms
 * 	Fixed emul_sigsetmask - Too many arguments.
 * 	[90/12/17  14:59:26  neves]
 * 
 * 	Fixed default signal handlers to return proper exit code.
 * 	[90/12/17  14:55:04  neves]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:15:26  jms]
 * 
 * Revision 2.9  90/11/27  18:18:33  jms
 * 	Bug Fix.
 * 	[90/11/20  12:54:39  jms]
 * 
 * 	Setup for pure kernel execution
 * 	Take out HTG_SIGNAL dook
 * 	[90/08/20  17:24:09  jms]
 * 
 * Revision 2.8  90/10/02  11:35:50  mbj
 * 	Added missing mach_object_dereference call.
 * 	[90/10/01  14:49:40  mbj]
 * 
 * Revision 2.7  90/07/09  17:02:32  dorr
 * 	add htg_signal stuff for hybrid emulation.
 * 	[90/03/01  14:53:26  dorr]
 * 
 * 	add thread argument to event_post.  get rid of emul_signal_deliver().
 * 	[90/02/23  14:44:24  dorr]
 * 
 * 	first working signal delivery.
 * 	gut htg_kill and htg_killpg.  fill in signal_* routines.
 * 	[90/01/11  11:39:35  dorr]
 * 	signal_* routines filled in (no core dumped).
 * 	Add self signal syncronization.
 * 	Add emulation unix process  status logic
 * 	Add sig_pause.
 * 	Misc bug fixes.
 * 	[90/07/06  15:02:46  jms]
 * 
 * Revision 2.6  90/03/21  17:21:14  jms
 * 	Mods for useing the objectified Task Master and TM->emul signal support.
 * 	[90/03/16  16:33:02  jms]
 * 
 * Revision 2.5  90/01/02  21:50:19  dorr
 * 	initial signal work.
 * 
 * Revision 2.4.1.2  89/12/19  17:05:38  dorr
 * 	checkin before christmas
 * 
 * Revision 2.4.1.1  89/12/18  15:50:57  dorr
 * 	fill out misc stack/mask/vec syscalls.
 * 
 */
 

#include <uxsignal_ifc.h>
#include <us_tm_task_ifc.h>

#include "emul_base.h"
#include "emul_proc.h"

extern "C" {
#include <mach/mach_types.h>
#include <mach_error.h>
#include <hash.h>

#include <sig_error.h>

#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/wait.h>
}

uxsignal* uxsignal_obj;

/* location of _sigreturn as supplied by sigvec. */
int (*sigreturnaddr)();

extern "C" void signal_terminate(int, int);
extern "C" void signal_core(int, int);
extern "C" void signal_stop(int, int);
extern "C" void signal_ignore(int);


mach_error_t
do_self_signal(int sig)
{
	int		event = unix_sig_err(sig);
        mach_error_t	err;
	tm_task_id_t	my_task_id;

	err = tm_task_obj->tm_get_task_id(&my_task_id);
	if (err != ERR_SUCCESS)  return (err);

	err = uxsignal_obj->event_post(MACH_PORT_NULL, 
				       event, 0, (int)my_task_id);
	return(err);
}

emul_kill(int pid, int sig, syscall_val_t *rv)
{
    mach_error_t 	err;
    tm_task_id_t	my_task_id;

    SyscallTrace("kill");

    err = tm_task_obj->tm_get_task_id(&my_task_id);
    if (err) {
        rv->rv_val1 = emul_error_to_unix(err);
	return(err);
    }
    
    /*
     * Are we dealing with myself or not
     */
    if ((! pid) || (my_task_id == pid_to_task_id(pid))) {
	/* It is me */
	if (ERR_SUCCESS != (err = do_self_signal(sig))) {
		return(err);
	}
    }
    else {
	/* Asking about someone else */
	usTMTask* tm_not_me_task_obj =0;
	usItem* tm_not_me_task_item =0;

        err = tm_obj->tm_task_id_to_task(pid_to_task_id(pid),
					 TM_DEFAULT_ACCESS, 
					 &tm_not_me_task_item);
	if (err) {
	    rv->rv_val1 = emul_error_to_unix(err);
	    return(err);
	}
	tm_not_me_task_obj = usTMTask::castdown(tm_not_me_task_item);

        err = tm_not_me_task_obj->tm_event_to_task(unix_sig_err(sig), 0, 0);
	if (err) {
	    mach_object_dereference(tm_not_me_task_obj);
            rv->rv_val1 = emul_error_to_unix(err);
            return(err);
	}
	mach_object_dereference(tm_not_me_task_obj);
    }

    rv->rv_val1 = 0;
    return(ERR_SUCCESS);
}
emul_killpg(int pgrp_id, int sig, syscall_val_t *rv)
{
    mach_error_t 	err;

    tm_tgrp_id_t	req_jgrp_id;
    usTMTgrp*		tm_jgrp_obj = NULL;
    usItem*		tm_jgrp_item = NULL;
    boolean_t		contained_self = FALSE;
    tm_task_id_t	my_task_id;

    SyscallTrace("killpg");

    err = tm_task_obj->tm_get_task_id(&my_task_id);
    if (err) {
	rv->rv_val1 = emul_error_to_unix(err);
	return(err);
    }
    
    /*
     * Find The job group
     */
    if (! pgrp_id) {
	/* Default, it is me */
	tm_task_obj->tm_get_tgrp(TM_DEFAULT_ACCESS, &tm_jgrp_item);
	if (err) {
            rv->rv_val1 = emul_error_to_unix(err);
            return(err);
	}
	tm_jgrp_obj = usTMTgrp::castdown(tm_jgrp_item);
    }
    else {
	/* job_group_id was given */
        req_jgrp_id = pgrp_to_job_group(pgrp_id);
        err = tm_obj->tm_find_tgrp(req_jgrp_id, TM_DEFAULT_ACCESS, &tm_jgrp_item);
	if (err) {
	    rv->rv_val1 = emul_error_to_unix(err);
	    return(err);
	}
	tm_jgrp_obj = usTMTgrp::castdown(tm_jgrp_item);
    }

    err = tm_jgrp_obj->tm_event_to_tgrp(unix_sig_err(sig), 0, 0,
					my_task_id, &contained_self);

    if (err) {
	mach_object_dereference(tm_jgrp_obj);
	rv->rv_val1 = emul_error_to_unix(err);
	return(err);
    }

    if (contained_self) {
	err = do_self_signal(sig);
	if (err) {
	    mach_object_dereference(tm_jgrp_obj);
	    rv->rv_val1 = emul_error_to_unix(err);
	    return(err);
	}
    }

    mach_object_dereference(tm_jgrp_obj);
    rv->rv_val1 = 0;
    return(ERR_SUCCESS);
}

void signal_terminate(int sig, int tm_post_id)
{
	union wait	status;

	DEBUG0(TRUE, (Diag,"signal_terminate:\n"));
	status.w_status = 0;
	status.w_termsig = sig;	
	status.w_coredump = 0;
	status.w_retcode = 0;
        (void)tm_task_obj->tm_hurtme(UNIX_SIG_KILL, status, tm_post_id);
	drop_dead();
}

void signal_core(int sig, int tm_post_id)
{
	union wait	status;

	DEBUG0(TRUE, (Diag,"signal_core:\n"));

	status.w_status = 0;
	status.w_termsig = sig;	
	status.w_coredump = 1;
	status.w_retcode = 0;
	
	/* Dump core here XXX */

        (void)tm_task_obj->tm_hurtme(UNIX_SIG_KILL, status, tm_post_id);
	drop_dead();
}

void signal_stop(int sig, int tm_post_id)
{
	mach_error_t	err;
	union wait	status;
	tm_task_id_t	task_id;

	DEBUG0(TRUE, (Diag,"signal_stop:\n"));
	(void)tm_task_obj->tm_get_task_id(&task_id);
	status.w_stopval = WSTOPPED;
	status.w_stopsig = (unsigned short)sig;
        err = tm_task_obj->tm_hurtme(UNIX_SIG_STOP, status, tm_post_id);
	if (err) {
		DEBUG0(TRUE, (Diag,"signal_stop failed:\n"));
	}
}

void signal_ignore(int sig)
{
	mach_error_t	err;
	union wait	status;
	tm_task_id_t	task_id;

	DEBUG0(TRUE, (Diag,"signal_ignore:\n"));
	NULL;
}


#define sigreturn_flag 0x80000000

emul_sigvec(int sig, struct sigvec *newvec, struct sigvec *oldvec, 
	    syscall_val_t *rv)
{
	mach_error_t		err;
	struct sigvec		* newsig = (struct sigvec *)0;
	struct sigvec		* oldsig = (struct sigvec *)0;

	SyscallTrace("sigvec");

	if (newvec) {
		COPYIN(newvec, newsig, 1, struct sigvec, rv, EFAULT);
	}
	if (oldvec) {
		COPYOUT_INIT(oldsig, oldvec, 1, struct sigvec, rv, EFAULT);
	}

	/* Do magic to get location of sigreturn */
	if (sigreturn_flag & rv->rv_val2) {
		sigreturnaddr = (int (*)())((unsigned long)(rv->rv_val2) & ~sigreturn_flag);
	}

	/* XXX hash thread->signal object */
	if ( (err = uxsignal_obj->signal_set_vec(sig,newsig,oldsig)) ) {
		rv->rv_val1 = emul_error_to_unix(err);
	} else {
		rv->rv_val1 = 0;
	}
	if (newvec) {
		COPYIN_DONE(newvec, newsig, 1, struct sigvec, rv, EFAULT);
	}
	if (oldvec) {
		COPYOUT(oldsig, oldvec, 1, struct sigvec, rv, EFAULT);
	}

	return err;
	
}

emul_sigblock(int mask, syscall_val_t *rv)
{
	int			oldmask;

	SyscallTrace("sigblock");

	(void)uxsignal_obj->signal_block(mask, &rv->rv_val1);
	return ERR_SUCCESS;
}

emul_sigsetmask(int mask, syscall_val_t *rv)
{
	SyscallTrace("sigsetmask");

	/* XXX hash thread->signal object */
	rv->rv_val1 = uxsignal_obj->signal_set_mask(mask, &rv->rv_val1);

	return ERR_SUCCESS;
}

emul_sigpause(int mask, syscall_val_t *rv)
{
	mach_error_t		err;

	SyscallTrace("sigpause");

	if ( err = uxsignal_obj->signal_pause(mask) ) {
		rv->rv_val1 = emul_error_to_unix(err);	/* always EINTR */
	} else {
		rv->rv_val1 = 0;
	}

	return err;
}

emul_sigstack(struct sigstack *newstack, struct sigstack *oldstack, 
	      syscall_val_t *rv)
{
	mach_error_t		err;
	struct sigstack		* news = (struct sigstack *)0;
	struct sigstack		* olds = (struct sigstack *)0;

	SyscallTrace("sigstack");

	if (newstack) {
		COPYIN(newstack, news, 1, struct sigstack, rv, EFAULT);
	}
	if (oldstack) {
		COPYOUT_INIT(olds, oldstack, 1, struct sigstack, rv, EFAULT);
	}

	/* XXX hash thread->signal object */
	if ( err = uxsignal_obj->signal_set_stack(newstack, oldstack) ) {
		rv->rv_val1 = emul_error_to_unix(err);
	} else {
		rv->rv_val1 = 0;
	}
	if (newstack) {
		COPYIN_DONE(newstack, news, 1, struct sigstack, rv, EFAULT);
	}
	if (oldstack) {
		COPYOUT(olds, oldstack, 1, struct sigstack, rv, EFAULT);
	}

	return err;
}

