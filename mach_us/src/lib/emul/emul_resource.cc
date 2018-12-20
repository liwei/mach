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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/emul/emul_resource.cc,v $
 *
 * Purpose:
 * 
 * HISTORY
 * $Log:	emul_resource.cc,v $
 * Revision 2.3  94/07/08  16:57:18  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/06  11:30:29  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:35:15  pjg]
 * 
 * Revision 2.2  91/07/01  14:06:56  jms
 * 	First version.
 * 	[91/06/16  21:00:47  dpj]
 * 
 */

#include "emul_base.h"

extern "C" {
#include <base.h>
#include <cthreads.h>
#include <errno.h>
#include <syscall_val.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <us_error.h>
#include <mach/time_value.h>
#include <mach/thread_info.h>
}

mach_error_t
emul_getrusage(int who, struct rusage *rusage, syscall_val_t *rv)
{
	struct rusage			*irusage;
	struct thread_basic_info	bi;
	unsigned int			bi_count;
	mach_error_t			err;

	SyscallTrace("getrusage");

	DEBUG2(emul_debug, (Diag, "getrusage(%x %x %x)", who, rusage, rv));

	COPYOUT_INIT(irusage, rusage, 1, struct rusage, rv, EFAULT);

	switch(who) {
	case RUSAGE_SELF:
		bi_count = THREAD_BASIC_INFO_COUNT;
		err = thread_info(mach_thread_self(),	/* XXX rwd-threads ? */
				   THREAD_BASIC_INFO,
				   (thread_info_t)&bi,
				   &bi_count);
		if (err != ERR_SUCCESS) {
			us_internal_error("thread_info()",err);
			goto finish;
		}

		irusage->ru_utime.tv_sec = bi.user_time.seconds;
		irusage->ru_utime.tv_usec = bi.user_time.microseconds;
		irusage->ru_stime.tv_sec = bi.system_time.seconds;
		irusage->ru_stime.tv_usec = bi.system_time.microseconds;

		irusage->ru_maxrss = 0;
		irusage->ru_ixrss = 0;	/* integral shared memory size */
		irusage->ru_idrss = 0;	/* integral unshared data " */
		irusage->ru_isrss = 0;	/* integral unshared stack " */
		irusage->ru_minflt = 0;	/* page reclaims */
		irusage->ru_majflt = 0;	/* page faults */
		irusage->ru_nswap = 0;	/* swaps */
		irusage->ru_inblock = 0;/* block input operations */
		irusage->ru_oublock = 0;/* block output operations */
		irusage->ru_msgsnd = 0;	/* messages sent */
		irusage->ru_msgrcv = 0;	/* messages received */
		irusage->ru_nsignals = 0;/* signals received */
		irusage->ru_nvcsw = 0;	/* voluntary context switches */
		irusage->ru_nivcsw = 0;	/* involuntary " */
		break;

	case RUSAGE_CHILDREN:
		/*
		 * XXX Need to call on the task master.
		 */
		DEBUG0(emul_debug,(Diag,"getrusage(!SELF) not implemented"));
		err = US_NOT_IMPLEMENTED;
		goto finish;

	default:
		err = unix_err(EINVAL);
		goto finish;
	}

	err = ERR_SUCCESS;
finish:
	COPYOUT(irusage, rusage, 1, struct rusage, rv, EFAULT);

	if (err) {
		rv->rv_val1 = emul_error_to_unix(err);
	} else {
		rv->rv_val1 = 0;
	}
	return(err);
}
