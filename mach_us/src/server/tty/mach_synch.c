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
 * 
 * Purpose:  Support BSDesk sync primatives used by derived code within the
 *		TTY server.
 * 
 * HISTORY
 * $Log:	mach_synch.c,v $
 * Revision 2.8  94/07/21  16:14:41  mrt
 * 	Updated copyright
 * 
 * Revision 2.7  94/01/11  18:11:42  jms
 * 	More debugging stuff.
 * 	[94/01/10  13:42:27  jms]
 * 
 * Revision 2.6  92/07/05  23:36:13  dpj
 * 	Removed include of mach_object.h.
 * 	[92/05/10  01:33:27  dpj]
 * 
 * Revision 2.5  92/03/05  15:15:12  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:46:02  jms]
 * 
 * Revision 2.4  91/07/01  14:15:30  jms
 * 	make the tty server interruptable in the mach_objects/interrupt.h sense.
 * 	[91/06/25  13:22:00  jms]
 * 
 * 	Change so that "gotosleep" will be interrupted when the remote invocation
 * 	mechinism wants to interrupt an active tty call.
 * 	[91/04/15  18:06:39  jms]
 * 
 * Revision 2.3  91/03/25  14:15:00  jjc
 * 	Put priority level in C thread specific global variable.
 * 	[91/03/06            jjc]
 * 
 * Revision 2.2  90/10/02  11:37:19  mbj
 * 	Dropped annoying nyi debugging printout.
 * 	[90/10/01  15:07:58  mbj]
 * 
 * Revision 2.6  90/03/14  21:26:44  rwd
 * 	Change reference of PINOD+1 to PSPECL.
 * 	[90/02/16            rwd]
 * 
 * Revision 2.5  90/01/19  14:35:03  rwd
 * 	Call ux_server_thread_busy on rmt_* sleep calls (prio=PINOD+1)
 * 	[90/01/12            rwd]
 * 
 * Revision 2.4  89/12/08  20:14:30  rwd
 * 	Change SLEEP_HASH function to be more appropriate for a
 * 	multi-cthreaded server.
 * 	[89/12/04            rwd]
 * 	Fix uthread_wakeup.
 * 	[89/11/29            rwd]
 * 	Returned spl locking to original form.
 * 	[89/11/27            rwd]
 * 	Changed locking on spls and changed while (foo) condition_wait
 * 	to if (foo) condition_wait because of new locking.
 * 	[89/11/20            rwd]
 * 
 * 	Changed master_lock to mutex.  Changed waits to condition waits.
 * 	Timeouts for condition waits are implimented through the timeout
 * 	mechanism.  Use special timeout and untimeout.
 * 	[89/10/30            rwd]
 * 
 * Revision 2.2  89/10/17  11:26:10  rwd
 * 	Added DEBUG code
 * 	[89/09/26            rwd]
 * 
 * Revision 2.1.1.1  90/09/10  17:51:30  mbj
 * 	Paul Neves' pure kernel tty_server changes
 * 
 * Revision 2.1.1.2  89/09/26  14:57:14  dbg
 * 	Add check for exit/stop in tsleep.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.1.1.1  89/09/21  20:37:04  dbg
 * 	Make serialization on master_queue explicit.
 * 	[89/09/21            dbg]
 * 
 */
/*
 * Replacements for sleep, wakeup, tsleep, spl*.
 */

#include <base.h>

#include <sys/param.h>
#include <sys/types.h>
#include <mach/machine/boolean.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/queue.h>
#include <sys/synch.h>
#include <sys/systm.h>

#include <uxkern/import_mach.h>
#include <cthreads.h>
#include <interrupt.h>

static cthread_key_t	priority_key = CTHREAD_KEY_INVALID;

#define	set_priority_level(level) \
	if (cthread_setspecific(priority_key, (any_t)level) == -1) { \
		ERROR((Diag, "set_priority_level: cthread_setspecific() failed\n")); \
	}

#define	get_priority_level(level) \
	if (cthread_getspecific(priority_key, (any_t *)level) == -1) { \
		ERROR((Diag, "get_priority_level: cthread_getspecific() failed\n")); \
	}

static struct mutex	sleep_mutex = MUTEX_INITIALIZER;

/*
 * Stack of spl locks - one for each priority level.
 */
struct spl_lock {
    cthread_t		holder;
    struct mutex	lock;
    struct condition	condition;
};

static struct spl_lock	spl_locks[SPL_COUNT];

void
spl_init()
{
	register struct spl_lock *sp;

	for (sp = &spl_locks[SPL_COUNT-1]; sp >= &spl_locks[0]; sp--) {
	    sp->holder = NO_CTHREAD;
	    mutex_init(&sp->lock);
	    condition_init(&sp->condition);
	}
	if (cthread_keycreate(&priority_key) == -1)
		ERROR(("Diag, spl_init: cthread_keycreate() failed\n"));
}

int
spl_n(new_level)
	int	new_level;
{
	int	old_level;
	register int	i;
	register struct spl_lock *sp;
	register cthread_t	self = cthread_self();

	get_priority_level(&old_level);

	if (new_level > old_level) {
	    /*
	     * Raising priority
	     */
	    for (i = old_level + 1; i <= new_level; i++) {
		sp = &spl_locks[i];

		mutex_lock(&sp->lock);
		while (sp->holder != self && sp->holder != NO_CTHREAD)
		    condition_wait(&sp->condition, &sp->lock);
		sp->holder = self;
		mutex_unlock(&sp->lock);
	    }
	}
	else if (new_level < old_level) {
	    /*
	     * Lowering priority
	     */
	    for (i = old_level; i > new_level; i--) {
		sp = &spl_locks[i];

		mutex_lock(&sp->lock);
		sp->holder = NO_CTHREAD;
		mutex_unlock(&sp->lock);
		condition_signal(&sp->condition);
	    }
	}
	set_priority_level(new_level);
	return (old_level);
}

#undef spl0
int spl0()
{
    return (spl_n(0));
}

#undef splsoftclock
int splsoftclock()
{
    return (spl_n(SPLSOFTCLOCK));
}

#undef splnet
int splnet()
{
    return (spl_n(SPLNET));
}

#undef splbio
int splbio()
{
    return (spl_n(SPLBIO));
}

#undef spltty
int spltty()
{
    return (spl_n(SPLTTY));
}

#undef splimp
int splimp()
{
    return (spl_n(SPLIMP));
}

#undef splhigh
int splhigh()
{
    return (spl_n(SPLHIGH));
}

#undef splx
int splx(s)
    int s;
{
    return(spl_n(s));
}

int spl5()	{ return spl_n(5); }

/*
 * Interrupt routines start at a given priority.  They may interrupt other
 * threads with a lower priority (unlike non-interrupt threads, which must
 * wait).
 */
void
interrupt_enter(level)
	int level;
{
	register cthread_t	self = cthread_self();
	register struct spl_lock *sp = &spl_locks[level];

	/*
	 * Grab the lock for the interrupt priority.
	 */

	mutex_lock(&sp->lock);
	while (sp->holder != self && sp->holder != NO_CTHREAD)
	    condition_wait(&sp->condition, &sp->lock);
	sp->holder = self;
	mutex_unlock(&sp->lock);

	set_priority_level(level);
}

void
interrupt_exit(level)
	int level;
{
	register cthread_t	self = cthread_self();
	register struct spl_lock *sp;
	register int		i;
	int			priority_level;

	/*
	 * Release the lock for the interrupt priority.
	 */
	get_priority_level(&priority_level);

	for (i = priority_level; i >= level; i--) {
	    sp = &spl_locks[i];
	    mutex_lock(&sp->lock);
	    sp->holder = NO_CTHREAD;
	    mutex_unlock(&sp->lock);
	    condition_signal(&sp->condition);
	}
	set_priority_level(-1);
}


int
issig()
{
/*  printf("issig not yet implemented\n"); */
    return 0;
}

int
cpu_number()
{
    return 0;		/* XXX Should be thread-specific */
}

/*
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process
 * enters the scheduling queue at priority pri.
 * The most important effect of pri is that when
 * pri<=PZERO a signal cannot disturb the sleep;
 * if pri>PZERO signals will be processed.
 * Callers of this routine must be prepared for
 * premature return, and check that the reason for
 * sleeping has gone away.
 */
void
gotosleep(cond) condition_t cond;
{
    register struct proc *rp;
    register s;
    int	ret;

    rp = u.u_procp;
    s = splhigh();
    if (panicstr) {
	/*
	 * After a panic, just give interrupts a chance,
	 * then just return; don't run any other procs 
	 * or panic below, in case this is the idle process
	 * and already asleep.
	 * The splnet should be spl0 if the network was being used
	 * by the filesystem, but for now avoid network interrupts
	 * that might cause another panic.
	 */
/*	(void) splnet(); */
	splx(s);
	return;
    }
    rp->p_slptime = 0;

    /*
     * If wakeup occurs while in issig, thread_block()
     * below is a no-op.  If ISSIG finds a signal, clear
     * sleep condition before going to process it.
     */
    if (ISSIG(rp)) {
	(void) spl0();
        DEBUG0(1,(Diag,"mach_sync ISSIG1 th:0x%x\n", mach_thread_self()));
	goto psig;
    }
    (void) spl0();
/*	if (cpu_number() != master_cpu) {
/*	    printf("unix sleep: on slave?");
/*	} */
    mutex_lock(&sleep_mutex);
    ret = intr_cond_wait(cond, &sleep_mutex);
    mutex_unlock(&sleep_mutex);

    if (ret) {
        DEBUG0(1,(Diag,"mach_sync INTR th:0x%x\n", mach_thread_self()));
	goto psig;
    }

    if (ISSIG(rp)) {
        DEBUG0(1,(Diag,"mach_sync ISSIG2 th:0x%x\n", mach_thread_self()));
	goto psig;
    }

    splx(s);
    return;

    /*
     * If priority was low (>PZERO) and
     * there has been a signal, execute non-local goto through
     * u.u_qsave, aborting the system call in progress (see trap.c)
#if CMUCS
     * (or finishing a tsleep, see below)
#endif  CMUCS
     */
psig:
    DEBUG0(1,(Diag,"ttyserver: gotosleep _longjmp: u_qsave 0x%x th:0x%x\n", &u.u_qsave,mach_thread_self()));

    _longjmp(&u.u_qsave, 1);
    /*NOTREACHED*/
}

/*
 * Wake up all processes sleeping on chan.
 */
void
wakeup(cond)
    register condition_t cond;
{
    mutex_lock(&sleep_mutex);
    condition_broadcast(cond);
    mutex_unlock(&sleep_mutex);
}

int nselcoll;

void
selwakeup(p, coll)
    register struct proc *p;
    int coll;
{
    if (coll) {
	nselcoll++;
#if US
	wakeup(&selwait_lock);
#else US
	wakeup((caddr_t)&selwait);
#endif US
    }
    if (p) {
	int s = splhigh();
#if US
#else US
	clear_wait((us_thread_t)p, THREAD_AWAKENED, TRUE);
#endif US
	/* proc[((us_thread_t)p)->task->proc_index].p_flag &= ~SSEL; */
	((us_thread_t)p)->u_address.utask->uu_procp->p_flag &= ~SSEL;
#if US
	/* 
	 * Wake up all threads waiting on select even though we know
	 * there's only one really waiting on this event since there's
	 * no way to condition_signal a specific cthread.  This all
	 * probably gets reworked anyway.
	 */
	wakeup(&selwait_lock);
#endif US
	splx(s);
    }
}

/*
 * Arrange to run the function fun with argument arg after a delay of t ticks.
 */

void
timeout(fun, arg, t) int (*fun)(); caddr_t arg; int t;
{
    printf("timeout called\n");
    (*fun)(arg);		/* XXX no delay */
}
