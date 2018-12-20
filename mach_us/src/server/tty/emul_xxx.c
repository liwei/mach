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
 * HISTORY:
 * $Log:	emul_xxx.c,v $
 * Revision 1.7  94/07/21  16:14:37  mrt
 * 	Updated copyright
 * 
 * Revision 1.6  92/07/05  23:36:09  dpj
 * 	Eliminated diag_format().
 * 	[92/05/10  01:32:28  dpj]
 * 
 * Revision 1.5  90/10/02  11:37:10  mbj
 * 	Made it work under MACH3_US.
 * 	[90/10/01  15:06:21  mbj]
 * 
 * Revision 1.4  90/08/14  22:03:28  mbj
 * 	Changed some printfs to debugging output.
 * 	[90/08/14  14:47:12  mbj]
 * 
 * Revision 1.3  89/07/09  14:22:09  dpj
 * 	Added include of base.h.
 * 	[89/07/08  13:19:19  dpj]
 * 
 * Revision 1.2  89/03/21  14:42:08  mbj
 * 	Merge mbj_pgrp branch onto mainline.
 * 
 * Revision 1.1.1.1  89/03/07  13:48:24  mbj
 * 	Removed gsignal().
 * 
 * 26-Sep-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Created to hold emulation routines.
 */

#include <base.h>

#include <sys/types.h>
#include <sys/thread.h>
#include <sys/signal.h>
#include <sys/user.h>
#include <sys/proc.h>

int copyin(from, to, size) char *from; char *to; int size;
{
    bcopy(from, to, size);
    return 0;
}

int copyout(from, to, size) char *from; char *to; int size;
{
    bcopy(from, to, size);
    return 0;
}

char *panicstr;

void panic(msg) char *msg;
{
    panicstr = msg;
    CRITICAL((Diag,"panic: %s\n", msg));
#if	! MACH3_US
    kill(getpid(), SIGQUIT);	/* Dump core! */
#endif	! MACH3_US
    exit(1);
}

void psig()
{
    DEBUG0(1,(Diag,"psig()\n"));			/* XXX */
    u.u_procp->p_cursig = 0;
}

void forceclose(dev) dev_t dev;
{
    DEBUG0(1,(Diag,"forceclose not yet implemented\n"));
}

/*
 * Is p an inferior of the current process?
 */
inferior(p)
    register struct proc *p;
{

    DEBUG0(1,(Diag,"inferior not yet implemented\n"));
    return (1);
}

struct proc *
pfind(pid)
    int pid;
{
    DEBUG0(1,(Diag,"pfind not yet implemented\n"));
    return ((struct proc *)0);
}
