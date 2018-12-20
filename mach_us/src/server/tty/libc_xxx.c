/*
 **********************************************************************
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
 **********************************************************************
 */
/*
 * libc_xxx.c
 *
 * Reimplementations of things in libc which won't work in this environment.
 *
 * Michael B. Jones  --  12-Oct-1988
 */

/*
 * HISTORY: 
 * $Log:	libc_xxx.c,v $
 * Revision 1.6  94/07/21  16:14:39  mrt
 * 	Updated copyright
 * 
 * Revision 1.5  92/07/05  23:36:11  dpj
 * 	Fixed some declarations for C++.
 * 	[92/05/10  01:33:11  dpj]
 * 
 * Revision 1.4  92/03/05  15:15:09  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:45:39  jms]
 * 
 * Revision 1.3  90/10/02  11:37:15  mbj
 * 	Added libc rindex() function.
 * 	[90/10/01  15:06:54  mbj]
 * 
 * Revision 1.2.1.1  90/09/10  17:50:33  mbj
 * 	Paul Neves' pure kernel tty_server changes
 * 
 * Revision 1.2  89/05/18  10:32:54  dorr
 * 	include file cataclysm
 * 
 * Revision 1.1  88/10/28  01:26:04  mbj
 * Initial revision
 * 
 * 12-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

#include <mach.h>
#include <mach/message.h>
#include <mach/port.h>
#include <cthreads.h>

#define	__cplusplus	/*XXXXXXXXX to avoid bad version of ASSERT*/
#include <debug.h>
#undef	__cplusplus

#include <base.h>

void sleep(seconds) int seconds;
/*
 * An implementation of sleep(3) which works in multi-threaded programs.
 */
{
    static int sleep_key = CTHREAD_KEY_INVALID;
    mach_port_t sleep_port;
    mach_msg_header_t msg;
    mach_error_t ret;
    
    if (CTHREAD_KEY_INVALID == sleep_key) {
	sleep_key = cthread_keycreate(&sleep_key);
    }
    (void) cthread_getspecific(sleep_key, &sleep_port);

    if (CTHREAD_DATA_VALUE_NULL == (any_t)sleep_port) {
	ret = mach_port_allocate(mach_task_self(), 
			MACH_PORT_RIGHT_RECEIVE, &sleep_port);
	ASSERT_RETCODE("TTY sleep port allocate died", ret);

	(void) cthread_setspecific(sleep_key, sleep_port);
    }

    bzero(&msg, sizeof(msg));
    msg.msgh_size = sizeof(msg);
    msg.msgh_local_port = sleep_port;
    mach_msg(&msg, MACH_RCV_MSG|MACH_RCV_TIMEOUT,
			0, sizeof(msg), msg.msgh_local_port,
			seconds * 1000, MACH_PORT_NULL);
}

char *
rindex(sp, c)
register char *sp, c;
{
	register char *r;

	r = 0;
	do {
		if (*sp == c)
			r = sp;
	} while (*sp++);
	return(r);
}

#ifdef TESTCASE
main()
{
    printf("Sleeping for 10 seconds\n");
    sleep(10);
    printf("Done sleeping\n");
}
#endif TESTCASE
