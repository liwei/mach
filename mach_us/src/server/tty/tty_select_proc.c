/*
 **********************************************************************
 * Mach Operating System
 * Copyright (c) 1994,1993 Carnegie Mellon University
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
 * File: tty_select_proc.c
 *
 * Purpose: Probe/select call into the tty code.
 *
 * Author: J. Mark Stevenson
 */

/* 
 * HISTORY:
 * $Log:	tty_select_proc.c,v $
 * Revision 2.3  94/07/21  16:15:06  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  94/01/11  18:12:07  jms
 * 	Initial Version
 * 	[94/01/10  13:52:03  jms]
 * 
 * Revision 1.3  90/10/02  11:37:52  mbj
 * 	Made it work under any of MACH3_{UNIX,VUS,US} configurations.
 * 	[90/10/01  15:26:13  mbj]
 * 
 * Revision 1.2.1.1  90/09/10  17:59:34  mbj
 * 	Paul Neves' pure kernel tty_server changes
 * 
 * Revision 1.2  88/11/01  07:28:43  mbj
 * Move selwakeup() into tty library.
 * 
 * Revision 1.1  88/10/28  01:29:37  mbj
 * Initial revision
 * 
 * 25-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

#include <base.h>
#include <mach/mach_types.h>
#include <io_types.h>
#include <exception_error.h>

#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/uio.h>
#include <sys/tty.h>
#include <sys/conf.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/systm.h>

#define SYSCALL_ENTRY						\
restartsys:							\
	u.u_error = 0;						/**/

#if US
#define SYSCALL_EXIT						\
	if ((u.u_procp)->p_cursig || ISSIG((u.u_procp)))	\
		psig();						/**/
#else US
#define SYSCALL_EXIT						\
	tty_release_thread_info();
#endif US

int unselect();
int nselcoll;



mach_error_t tty_select_proc(dev, flag)
/* Flag == FREAD or FWRITE or 0 */
    dev_t dev; int flag;
{
    int s, ncoll, ni;
    label_t lqsave;
    int result;

    SYSCALL_ENTRY;
retry:
    ncoll = nselcoll;
    u.u_procp->p_flag |= SSEL;

    if (_setjmp(&u.u_qsave)) {
#if US
        DEBUG1(1,(Diag,"tty_select_proc INTR th:0x%x\n", mach_thread_self()));
	tty_release_thread_info();
	return EXCEPT_SOFTWARE;
#else US
	if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0) {
	    tty_release_thread_info();
	    return EINTR;
	}
	goto restartsys;
#endif US
    }

    result = (*cdevsw[major(dev)].d_select)(dev, flag);

    if (u.u_error || result)
	goto done;
    s = splhigh();

    if ((u.u_procp->p_flag & SSEL) == 0 || nselcoll != ncoll) {
	u.u_procp->p_flag &= ~SSEL;
	splx(s);
	goto retry;
    }
    u.u_procp->p_flag &= ~SSEL;

    gotosleep(&selwait_lock);

    splx(s);
    goto retry	;
done:

    SYSCALL_EXIT;

    if (u.u_error) {
	/* *error = u.u_error; */
	return -1;
	
    }
    else {
	/* return result; */
	return(ERR_SUCCESS);
    }
}
