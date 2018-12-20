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
 * File: us/lib/threads/unix.c
 *
 * Purpose:
 *	Simulate blocking UNIX system calls in the presence
 *	of other coroutines.
 *
 * HISTORY:
 * $Log:	unix.c,v $
 * Revision 1.3  94/07/08  14:14:48  mrt
 * 	Updated copyright
 * 
 * Revision 1.2  90/07/09  17:06:55  dorr
 * 	Header Fixed
 * 	[90/07/06  17:27:20  jms]
 * 
 */


#include <cthreads.h>
#include "cthread_internals.h"
#include <sys/time.h>

struct timeval unix_select_timeout = {
	0,	/* seconds */
	10000,	/* microseconds */
};

#if	COROUTINE

#include <sys/types.h>
#include <syscall.h>

/*
 * C Threads imports:
 */
extern int time_compare();
extern void time_plus();

/*
 * C library imports:
 */
extern syscall();

#define	FD_BYTES(n)	(howmany((n), NFDBITS) * sizeof(fd_mask))

select(nfds, readfds, writefds, exceptfds, timeout)
	int nfds;
	fd_set *readfds, *writefds, *exceptfds;
	struct timeval *timeout;
{
	int n;
	struct timeval tval, deadline;
	fd_set r, w, e;
	register fd_set *rp = (readfds == 0 ? 0 : &r);
	register fd_set *wp = (writefds == 0 ? 0 : &w);
	register fd_set *ep = (exceptfds == 0 ? 0 : &e);

	if (timeout != 0)
		time_plus((struct timeval *) 0, timeout, &deadline);
	for (;;) {
		if (cthread_count() <= 1) {
			/*
			 * No other threads are runnable.
			 * Go ahead and do the possibly blocking version.
			 */
			return syscall(SYS_select, nfds, readfds, writefds, exceptfds, timeout);
		}
		/*
		 * Only copy in portion of fd_set required by value of nfds.
		 */
		if (rp != 0)
			bcopy((char *) readfds, (char *) rp, FD_BYTES(nfds));
		if (wp != 0)
			bcopy((char *) writefds, (char *) wp, FD_BYTES(nfds));
		if (ep != 0)
			bcopy((char *) exceptfds, (char *) ep, FD_BYTES(nfds));
		tval = unix_select_timeout;
		n = syscall(SYS_select, nfds, rp, wp, ep, &tval);
		switch (n) {
		    case -1:
			return -1;
		    case 0:
			if (timeout != 0 &&
			    time_compare(&deadline, (struct timeval *) 0) <= 0)
				return 0;
			TRACE(printf("[%s] select()\n", cthread_name(cthread_self())));
			cthread_yield();
			continue;
		    default:
			/*
			 * Only copy out portion of fd_set required by value of nfds.
			 */
			if (rp != 0)
				bcopy((char *) rp, (char *) readfds, FD_BYTES(nfds));
			if (wp != 0)
				bcopy((char *) wp, (char *) writefds, FD_BYTES(nfds));
			if (ep != 0)
				bcopy((char *) ep, (char *) exceptfds, FD_BYTES(nfds));
			return n;
		}
	}
}
#endif	COROUTINE
