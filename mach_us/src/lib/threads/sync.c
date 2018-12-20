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
 * File: us/lib/threads/sync.c,v $
 *
 * Purpose: Spin locks and mutexes.
 *
 * HISTORY:
 * $Log:	sync.c,v $
 * Revision 1.5  94/07/08  14:14:45  mrt
 * 	Updated copyright
 * 
 * Revision 1.4  94/05/17  15:55:52  jms
 * 	Be sure to get the "cthreads.h" from this directory.
 * 
 * Revision 1.3  90/07/09  17:06:48  dorr
 * 	Header fixed
 * 	[90/07/06  17:23:00  jms]
 * 
 */


#include "cthreads.h"
#include "cthread_internals.h"

/*
 * Spin locks.
 * Use test and test-and-set logic on all architectures.
 */

void
spin_lock(p)
	register int *p;
{
	while (*p != 0 || !mutex_try_lock((mutex_t) p))
		;	/* spin */
}

void
spin_unlock(p)
	int *p;
{
	mutex_unlock((mutex_t) p);
}

/*
 * Mutex objects.
 *
 * Mutex_wait_lock() is implemented in terms of mutex_try_lock().
 * Mutex_try_lock() and mutex_unlock() are machine-dependent,
 * except in the COROUTINE implementation.
 *
 * Mutex_try_lock() relies on the fact that the first word pointed to
 * by its argument is the lock word, and does not reference any other fields;
 * hence it can be used by both spin_lock() (with an int * argument)
 * and mutex_wait_lock() (with a mutex_t argument).
 */

int mutex_spin_limit = 0;

void
mutex_wait_lock(m)
	register mutex_t m;
{
	register int i;

	TRACE(printf("[%s] lock(%s)\n", cthread_name(cthread_self()), mutex_name(m)));
	for (i = 0; i < mutex_spin_limit; i += 1)
		if (m->lock == 0 && mutex_try_lock(m))
			return;
		else
			/* spin */;
	for (;;)
		if (m->lock == 0 && mutex_try_lock(m))
			return;
		else
			cthread_yield();
}

#if	COROUTINE

int
mutex_try_lock(m)
	register mutex_t m;
{
	if (m->lock) {
		return FALSE;
	} else {
		m->lock = 1;
		return TRUE;
	}
}

void
mutex_unlock(m)
	register mutex_t m;
{
	m->lock = 0;
}

#endif	COROUTINE
