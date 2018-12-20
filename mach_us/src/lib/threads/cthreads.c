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
 *
 * File: us/lib/threads/cthreads.c
 *
 * Implementation of cthread_fork, cthread_join, cthread_exit, etc.
 *
 * HISTORY
 * $Log:	cthreads.c,v $
 * Revision 1.9  94/07/08  14:14:29  mrt
 * 	Updated copyright
 * 
 * Revision 1.8  94/05/17  14:06:35  jms
 * 	Make spin_locks spin_locks instead of int
 * 	Make stuff volatile as needed
 * 	[94/04/28  18:25:17  jms]
 * 
 * Revision 1.7  91/11/06  13:33:15  jms
 * 	Changed uses of "catch" to "t_catch" ("catch" is a reserved work 
 * 	in C++).
 * 	[91/09/27  15:12:10  pjg]
 * 
 * Revision 1.6  91/07/01  14:10:56  jms
 * 	No Further Changes
 * 	[91/06/24  17:08:35  jms]
 * 
 * 	Add cthread_bind_phantom to tell cthreads that a given phanton has a given
 * 	kernel thread.
 * 
 * 	Add cthread_bind to bind a given cthread to its current kernel thread.
 * 	returns the "id" (port_t) of that thread.
 * 	[91/04/15  17:54:06  jms]
 * 
 * Revision 1.5  90/10/30  17:25:01  mbj
 * 	Pick up setjmp definitions from <setjmp.h>.
 * 	[90/10/30  17:02:52  mbj]
 * 
 * Revision 1.4  90/07/26  12:41:56  dpj
 * 	Fixed cthread_init() to return 0, for compatibility
 * 	with new crt0.
 * 	[90/07/24  15:03:00  dpj]
 * 
 * Revision 1.3  90/07/09  17:05:33  dorr
 * 	Dorr/mbj added cthread_create_phantom
 * 	[90/07/06  16:51:23  jms]
 * 
 * 24-Mar-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Implement fork() for multi-threaded programs.  The child's
 *	cthread state is re-initialized to correctly describe its
 *	(single) cthread.  Acquires necessary locks to insure that
 *	cthread library data structures aren't being modified in
 *	critical sections during the fork by other threads.
 *	
 *	Made MTASK version work correctly again.
 *
 * 31-Dec-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Changed cthread_exit() logic for the case of the main thread,
 *	to fix thread and stack memory leak found by Camelot group.
 *
 * 21-Aug-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Added consistency check in beginning of cthread_body().
 *
 * 11-Aug-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Removed cthread_port() and cthread_set_port().
 *	Removed port deallocation from cthread_free().
 *	Minor changes to cthread_body(), cthread_exit(), and cthread_done().
 *
 * 10-Aug-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Changed call to mig_init() in cthread_init() to pass 1 as argument.
 *
 * 31-Jul-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Added call to mig_init() from cthread_init().
 */


#include <cthreads.h>
#include "cthread_internals.h"

/*
 * C Threads imports:
 */
extern void cproc_init(), cproc_create();
extern void mig_init();

/*
 * Mach imports:
 */

/*
 * C library imports:
 */
#include <setjmp.h>

/*
 * Thread status bits.
 */
#define	T_MAIN		0x1
#define	T_RETURNED	0x2
#define	T_DETACHED	0x4

#ifdef	DEBUG
int cthread_debug = FALSE;
#endif	DEBUG

private volatile struct cthread_queue cthreads = QUEUE_INITIALIZER;
private volatile struct mutex cthread_lock = MUTEX_INITIALIZER;
private volatile struct condition cthread_needed = CONDITION_INITIALIZER;
private volatile struct condition cthread_idle = CONDITION_INITIALIZER;
private volatile int n_cprocs = 0;
private volatile int n_cthreads = 0;
private volatile int max_cprocs = 0;

private volatile cthread_t free_cthreads = NO_CTHREAD;	/* free list */
private spin_lock_t cthread_free_lock = 0;	/* unlocked */

private volatile struct cthread initial_cthread = { 0 };

cthread_t
cthread_alloc(func, arg)
	cthread_fn_t func;
	any_t arg;
{
	register cthread_t t = NO_CTHREAD;

	if (free_cthreads != NO_CTHREAD) {
		/*
		 * Don't try for the lock unless
		 * the list is likely to be nonempty.
		 * We can't be sure, though, until we lock it.
		 */
		spin_lock(&cthread_free_lock);
		t = free_cthreads;
		if (t != NO_CTHREAD)
			free_cthreads = t->next;
		spin_unlock(&cthread_free_lock);
	}
	if (t == NO_CTHREAD) {
		/*
		 * The free list was empty.
		 * We may have only found this out after
		 * locking it, which is why this isn't an
		 * "else" branch of the previous statement.
		 */
		t = (cthread_t) malloc(sizeof(struct cthread));
	}
	*t = initial_cthread;
	t->func = func;
	t->arg = arg;
	return t;
}


cproc_t cthread_create_phantom()
{
	cproc_t ur;
	extern cproc_t cproc_alloc();

	mutex_lock(&cthread_lock);

	alloc_stack(ur = cproc_alloc());
	ur->incarnation = cthread_alloc(0,0);

	mutex_unlock(&cthread_lock);

	return ur;
}

/*
 * cthread_bind_phantom: fillin the "id" field of the cproc for the given
 * thread.  Return the cthread_thread_stack of the thread.
 */
extern vm_offset_t cthread_thread_stack(/* thread */);

vm_offset_t
cthread_bind_phantom(thread)
	thread_t	thread;
{
	/* XXX Need some locks? */
	vm_offset_t	ret;
	ret = cthread_thread_stack(thread);
	((cproc_t)(* (ur_cthread_t *)ret))->id = (int)thread;
	return(ret);
}

/* cthread_bind: bind the cproc to its current id (task), return that id */
int
cthread_bind(ur)
	ur_cthread_t	ur;
{
	/* XXX Will need to do something other that just return the id */
	return(((cproc_t)ur)->id);
}

private void
cthread_free(t)
	register cthread_t t;
{
	spin_lock(&cthread_free_lock);
	t->next = free_cthreads;
	free_cthreads = t;
	spin_unlock(&cthread_free_lock);
}

int
cthread_init()
{
	static int cthreads_started = FALSE;
	register cthread_t t;

	if (cthreads_started)
		return;
	cproc_init();
	n_cprocs = 1;
	t = cthread_alloc((cthread_fn_t) 0, (any_t) 0);
	n_cthreads = 1;
	t->state |= T_MAIN;
	cthread_set_name(t, "main");
	cthread_assoc(cproc_self(), t);
	cthreads_started = TRUE;
	mig_init(1);			/* enable multi-threaded mig interfaces */
	return(0);			/* no new stack */
}

/*
 * Used for automatic initialization by crt0.
 * Cast needed since too many C compilers choke on the type void (*)().
 */
int (*_cthread_init_routine)() = (int (*)()) cthread_init;

/*
 * Procedure invoked at the base of each cthread.
 */
void
cthread_body(self)
	cproc_t self;
{
	register cthread_t t;

	ASSERT(cproc_self() == self);
	TRACE(printf("[idle] cthread_body(%x)\n", self));
	mutex_lock(&cthread_lock);
	for (;;) {
		/*
		 * Dequeue a thread invocation request.
		 */
		cthread_queue_deq(&cthreads, cthread_t, t);
		if (t != NO_CTHREAD) {
			/*
			 * We have a thread to execute.
			 */
			mutex_unlock(&cthread_lock);
			cthread_assoc(self, t);		/* assume thread's identity */
			if (_setjmp(t->t_catch) == 0) {	/* catch for cthread_exit() */
				/*
				 * Execute the fork request.
				 */
				t->result = (*(t->func))(t->arg);
			}
			/*
			 * Return result from thread.
			 */
			TRACE(printf("[%s] done()\n", cthread_name(t)));
			mutex_lock(&t->lock);

			if (t->state & T_MAIN) {
				/*
				 * Could have become main thread if we're the
				 * child of a thread that did a fork() call.
				 */
				mutex_unlock(&t->lock);
				cthread_exit(t->result);
			}

			if (t->state & T_DETACHED) {
				mutex_unlock(&t->lock);
				cthread_free(t);
			} else {
				t->state |= T_RETURNED;
				mutex_unlock(&t->lock);
				condition_signal(&t->done);
			}
			cthread_assoc(self, NO_CTHREAD);
			mutex_lock(&cthread_lock);
			n_cthreads -= 1;
		} else {
			/*
			 * Queue is empty.
			 * Signal that we're idle in case the main thread
			 * is waiting to exit, then wait for reincarnation.
			 */
			condition_signal(&cthread_idle);
#if	MTASK
			if (n_cthreads == 0) {
				/* The next broadcast should be redundant */
				condition_broadcast(&cthread_needed);
				mutex_unlock(&cthread_lock);
				_exit(0);  /* Don't clean up stdio again! */
			} else
				condition_wait(&cthread_needed, &cthread_lock);
#else	MTASK
			condition_wait(&cthread_needed, &cthread_lock);
#endif	MTASK
		}
	}
}

cthread_t
cthread_fork(func, arg)
	cthread_fn_t func;
	any_t arg;
{
	register cthread_t t;

	TRACE(printf("[%s] fork()\n", cthread_name(cthread_self())));
	mutex_lock(&cthread_lock);
	t = cthread_alloc(func, arg);
	cthread_queue_enq(&cthreads, t);
	if (++n_cthreads > n_cprocs && (max_cprocs == 0 || n_cprocs < max_cprocs)) {
		n_cprocs += 1;
		cproc_create();
	}
	mutex_unlock(&cthread_lock);
	condition_signal(&cthread_needed);
	return t;
}

void
cthread_detach(t)
	cthread_t t;
{
	TRACE(printf("[%s] detach(%s)\n", cthread_name(cthread_self()), cthread_name(t)));
	mutex_lock(&t->lock);
	if (t->state & T_RETURNED) {
		mutex_unlock(&t->lock);
		cthread_free(t);
	} else {
		t->state |= T_DETACHED;
		mutex_unlock(&t->lock);
	}
}

any_t
cthread_join(t)
	cthread_t t;
{
	any_t result;

	TRACE(printf("[%s] join(%s)\n", cthread_name(cthread_self()), cthread_name(t)));
	mutex_lock(&t->lock);
	ASSERT(! (t->state & T_DETACHED));
	while (! (t->state & T_RETURNED))
		condition_wait(&t->done, &t->lock);
	result = t->result;
	mutex_unlock(&t->lock);
	cthread_free(t);
	return result;
}

void
cthread_exit(result)
	any_t result;
{
	register cthread_t t = cthread_self();

	TRACE(printf("[%s] exit()\n", cthread_name(t)));
	t->result = result;
	if (t->state & T_MAIN) {
		mutex_lock(&cthread_lock);
		while (n_cthreads > 1)
			condition_wait(&cthread_idle, &cthread_lock);
#if 	MTASK
		n_cthreads = 0;		/* Tell other tasks to exit */
		condition_broadcast(&cthread_needed);
#endif	MTASK
		mutex_unlock(&cthread_lock);
		exit((int) result);
	} else {
		_longjmp(t->t_catch, TRUE);
	}
}

/*
 * Used for automatic finalization by crt0.  Cast needed since too many C
 * compilers choke on the type void (*)().
 */
int (*_cthread_exit_routine)() = (int (*)()) cthread_exit;

void
cthread_set_name(t, name)
	cthread_t t;
	char *name;
{
	t->name = name;
}

char *
cthread_name(t)
	cthread_t t;
{
	return (t == NO_CTHREAD
		? "idle"
		: (t->name == 0 ? "?" : t->name));
}

int
cthread_limit()
{
	return max_cprocs;
}

void
cthread_set_limit(n)
	int n;
{
	max_cprocs = n;
}

int
cthread_count()
{
	return n_cthreads;
}

/*
 * Routines for supporting fork() of multi-threaded programs.
 */

extern void malloc_fork_prepare(), malloc_fork_parent(), malloc_fork_child();
extern void cproc_fork_child(), stack_fork_child();

void cthread_fork_prepare()
/*
 * Prepare cthreads library to fork() a multi-threaded program.  All cthread
 * library critical section locks are acquired before a fork() and released
 * afterwards to insure no cthread data structure is left in an inconsistent
 * state in the child, which comes up with only the forking thread running.
 */
{
	mutex_lock(&cthread_lock);
	spin_lock(&cthread_free_lock);
	malloc_fork_prepare();
}

void cthread_fork_parent()
/*
 * Called in the parent process after a fork syscall.
 * Releases locks acquired by cthread_fork_prepare().
 */
{
	malloc_fork_parent();
	spin_unlock(&cthread_free_lock);
	mutex_unlock(&cthread_lock);
}

void cthread_fork_child()
/*
 * Called in the child process after a fork syscall.  Releases locks acquired
 * by cthread_fork_prepare().  Deallocates cthread data structures which
 * described other threads in our parent.  Makes this thread the main thread.
 * 
 * The mach_init() routine must be called in the child before this routine.
 */
{
	cthread_t t;

	malloc_fork_child();
	spin_unlock(&cthread_free_lock);
	mutex_unlock(&cthread_lock);

	condition_init(&cthread_needed);
	condition_init(&cthread_idle);
	max_cprocs = 0;

	/*
	 * Reinit other modules.
	 */

	stack_fork_child();
	cproc_fork_child();

	while (TRUE) {		/* Free cthread runnable list */
		cthread_queue_deq(&cthreads, cthread_t, t);
		if (t == NO_CTHREAD) break;
		free((char *) t);
	}

	while (free_cthreads != NO_CTHREAD) {	/* Free cthread free list */
		t = free_cthreads;
		free_cthreads = free_cthreads->next;
		free((char *) t);
	}

	/*
	 * From cthread_init() ...
	 */

	t = cthread_self();

	n_cprocs = 1;
	n_cthreads = 1;
	t->state = T_MAIN;
	cthread_set_name(t, "main");
	mig_init(1);		/* enable multi-threaded mig interfaces */
	
}
