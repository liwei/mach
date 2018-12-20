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
 * tty_threads.c
 *
 * Thread management routines for the bsd tty emulation server.
 *
 * Michael B. Jones  --  12-Oct-1988
 */

/*
 * HISTORY:
 * $Log:	tty_threads.c,v $
 * Revision 1.3  94/07/21  16:15:12  mrt
 * 	Updated copyright
 * 
 * Revision 1.2  90/10/02  11:38:52  mbj
 * 	Made it work under any of MACH3_{UNIX,VUS,US} configurations.
 * 	[90/10/01  15:39:34  mbj]
 * 
 * 	Made it work again under MACH3_UNIX after Paul Neves' pure kernel
 * 	tty_server changes.
 * 	[90/09/12  13:24:02  mbj]
 * 
 * Revision 1.1  88/10/28  01:31:07  mbj
 * Initial revision
 * 
 * 12-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

#include <cthreads.h>

#include <sys/types.h>
#include <sys/kernel.h>

#if	! MACH3_US
extern void bsd_tty_select_thread();
#endif	! MACH3_US
#if	! MACH3_UNIX
extern void device_reply_loop();
#endif	! MACH3_UNIX

int lbolt_insulation = 0;	/* Turn on for debugging to inhibit lbolt */

/*ARGSUSED*/
void lbolt_thread(arg) int arg;
/*
 * Set off the lightning bolt interrupt once every second.
 */
{
    while (1) {
	if (! lbolt_insulation) wakeup(&lbolt_lock);
	sleep(1);
    }
}

void start_tty_threads()
/*
 * Start the threads which are always present in the tty server.
 */
{
    cthread_detach(cthread_fork(lbolt_thread, 0));
#if	! MACH3_US
    cthread_detach(cthread_fork(bsd_tty_select_thread, 0));
#endif	! MACH3_US
#if	! MACH3_UNIX
    cthread_detach(cthread_fork(device_reply_loop, 0));
#endif	! MACH3_UNIX
}
