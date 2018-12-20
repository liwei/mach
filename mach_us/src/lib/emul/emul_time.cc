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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/emul/emul_time.cc,v $
 *
 * Purpose:
 *
 * HISTORY
 * $Log:	emul_time.cc,v $
 * Revision 2.4  94/07/08  16:57:23  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  92/07/05  23:25:36  dpj
 * 	Use new us_tm_{root,task,tgrp}_ifc.h interfaces for the C++ taskmaster.
 * 	[92/06/24  14:36:08  jms]
 * 
 * Revision 2.2  91/11/06  11:32:58  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:37:20  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:29:39  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:30:26  pjg]
 * 
 * Revision 2.6  91/07/01  14:07:01  jms
 * 	Fixed error code returns for copyin/copyout problems.
 * 	Added emul_gettimeofday().
 * 	[91/06/16  21:02:18  dpj]
 * 
 * Revision 2.5  91/04/12  18:47:48  jjc
 * 	Replaced Paul Neves' EFAULT handling with macros for copying
 * 	arguments in and out of system calls to figure out whether
 * 	they're good or not.
 * 	[91/04/01            jjc]
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:48:45  jjc]
 * 
 * Revision 2.4.1.1  91/02/05  15:19:26  neves
 * 	Inserted VALID_ADDRESS macro where appropriate.
 * 
 * Revision 2.4  90/12/21  13:52:28  jms
 * 	Clear old timer in emul_setitimer for very first timer.
 * 	[90/12/17  17:47:00  neves]
 * 
 * 	Fixed emul_setitimer problem returning old timer values.
 * 	[90/12/17  15:41:36  neves]
 * 
 * 	Emul_getitimer now checks for null argument.
 * 	[90/12/17  15:28:37  neves]
 * 
 * 	Added check for invalid interval in emul_setitimer.
 * 	[90/12/17  15:20:45  neves]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:15:43  jms]
 * 
 * Revision 2.3  90/11/27  18:18:38  jms
 * 	No Change
 * 	[90/11/20  13:48:15  jms]
 * 
 * Revision 2.2  90/08/13  15:44:10  jjc
 * 	Created.
 * 	[90/07/21            jjc]
 * 
 *
 */

#include <us_tm_task_ifc.h>
#include "emul_base.h"

extern "C" {
#include <base.h>
#include <cthreads.h>
#include <errno.h>
#include <sig_error.h>
#include <syscall_val.h>
#include <sys/time.h>
#include <mach/time_value.h>
#include <us_error.h>
#include "timer.h"
}

#define	TIMER_MAX	3

timer_id_t		timer_id[TIMER_MAX];
struct mutex		timer_lock;

extern usTMTask*	tm_task_obj;

void timer_init(void)
{
	int	i;

	for (i = 0; i < TIMER_MAX; i++)
		timer_id[i] = TIMER_INVALID_ID;
	mutex_init(&timer_lock);
}

mach_error_t
emul_getitimer(int which, struct itimerval *value, syscall_val_t *rv)
{
	mach_error_t		err;
	struct itimerval	*itv;

	SyscallTrace("getitimer");

	COPYOUT_INIT(itv, value, 1, struct itimerval, rv, EFAULT);

	DEBUG2(emul_debug, (Diag, "getitimer(%x %x %x)", which, itv, rv));

	if (itv == NULL) {
                rv->rv_val1 = EFAULT;
		return(unix_err(EFAULT));
	}

	mutex_lock(&timer_lock);
	if (timer_id[which])
		err = tm_task_obj->tm_timer_get(timer_id[which], which, 
						(timer_value_t)itv);
	else {
		timerclear(&itv->it_interval);
		timerclear(&itv->it_value);
		err = ERR_SUCCESS;
	}
	mutex_unlock(&timer_lock);
	if (err == US_OBJECT_NOT_FOUND)
		err = ERR_SUCCESS;	/* BSD Unix always returns success */
        if ( err )
                rv->rv_val1 = emul_error_to_unix(err);
        else
                rv->rv_val1 = ESUCCESS;

	COPYOUT(itv, value, 1, struct itimerval, rv, EFAULT);
	return(err);
}

mach_error_t
emul_setitimer(int which, struct itimerval *value, struct itimerval *ovalue,
	       syscall_val_t *rv)
{
	mach_error_t		err;
	register int		event;
	struct timer_value	itval;
	struct itimerval	*itv;
	struct itimerval	*oitv = (struct itimerval *)0;

	SyscallTrace("setitimer");

	COPYIN(value, itv, 1, struct itimerval, rv, EFAULT);
	if (ovalue)  {
		COPYOUT_INIT(oitv, ovalue, 1, struct itimerval, rv, EFAULT);
	}

	DEBUG2(emul_debug, (Diag, "setitimer(%x, %x %x %x)", which, itv, oitv, rv));


	if (itv->it_interval.tv_sec < 0 ||
	    itv->it_interval.tv_usec < 0) {
                rv->rv_val1 = EINVAL;
		return(unix_err(EINVAL));
	}
	    
	err = ERR_SUCCESS;
	mutex_lock(&timer_lock);

	if (oitv) {
		timerclear(&oitv->it_value);
		timerclear(&oitv->it_interval);
	}

	if (timer_id[which]) {
		if (!oitv) {
			err = tm_task_obj->tm_timer_delete(timer_id[which], 
							    &itval);
			timer_id[which] = TIMER_INVALID_ID;
		}
		else if (!timerisset(&itv->it_value)) {
			(void)tm_task_obj->tm_timer_get(timer_id[which], 
						which, (timer_value_t)oitv);
			err = tm_task_obj->tm_timer_delete(timer_id[which], 
							  (timer_value_t)oitv);
			timer_id[which] = TIMER_INVALID_ID;
		}
		else
			err = tm_task_obj->tm_timer_get(timer_id[which], 
							which, 
							(timer_value_t)oitv);
	}
	if (err == US_OBJECT_NOT_FOUND)
		err = ERR_SUCCESS;

	if (!err && timerisset(&itv->it_value)) {
		switch (which) {
			case TIMER_REAL:
				event = UNIX_SIG_ALRM;
				break;
			case TIMER_VIRTUAL:
				event = UNIX_SIG_VTALRM;
				break;
			case TIMER_PROFILED:
				event = UNIX_SIG_PROF;
				break;
			default:
				err = US_INVALID_ARGS;
		}
		err = tm_task_obj->tm_timer_set(which, event, 0, 0,
						&timer_id[which], 
						(timer_value_t)itv);
	}
	mutex_unlock(&timer_lock);


        if ( err )
                rv->rv_val1 = emul_error_to_unix(err);
        else
                rv->rv_val1 = ESUCCESS;

	COPYIN_DONE(value, itv, 1, struct itimerval, rv, EFAULT);
	if (ovalue) {
		COPYOUT(oitv, ovalue, 1, struct itimerval, rv, EFAULT);
	}

	return(err);
}

mach_error_t
emul_gettimeofday(struct timeval *tp, struct timezone *tzp,
		  syscall_val_t *rv)
{
	struct timeval		*itp;
	struct timezone		*itzp;
	time_value_t		time_value;
	mach_error_t		err;

	SyscallTrace("gettimeofday");

	DEBUG2(emul_debug, (Diag, "gettimeofday(%x %x %x)", tp, tzp, rv));

	/*
	 * XXX Obtain the time zone from the configuration server?
	 */
	if (tzp != NULL) {
		COPYOUT_INIT(itzp, tzp, 1, struct timezone, rv, EFAULT);
		itzp->tz_minuteswest = 300;
		itzp->tz_dsttime = DST_USA;
		COPYOUT(itzp, tzp, 1, struct timezone, rv, EFAULT);
	}

	if (tp != NULL) {
		COPYOUT_INIT(itp, tp, 1, struct timeval, rv, EFAULT);

		err = host_get_time(emul_host_port(),&time_value);
		if (err != ERR_SUCCESS) {
			goto finish;
		}

		itp->tv_sec = time_value.seconds;
		itp->tv_usec = time_value.microseconds;

		COPYOUT(itp, tp, 1, struct timeval, rv, EFAULT);
	}

	err = ERR_SUCCESS;
finish:
	if (err) {
		rv->rv_val1 = emul_error_to_unix(err);
	} else {
		rv->rv_val1 = 0;
	}
	return(err);
}
