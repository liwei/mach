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
 * stack.c
 *
 * File: us/lib/threads/stack.c
 *
 * C Thread stack allocation.
 *
 * HISTORY:
 * $Log:	stack.c,v $
 * Revision 1.13  94/07/08  14:14:42  mrt
 * 	Updated copyright
 * 
 * Revision 1.12  94/05/17  14:06:56  jms
 * 	Make spin_locks spin_locks instead of int
 * 	Make stuff volatile as needed
 * 	[94/04/28  18:36:58  jms]
 * 
 * Revision 1.11  92/03/05  15:05:16  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:28:25  jms]
 * 
 * Revision 1.10  91/10/06  22:29:51  jjc
 * 	Changed stack_init() (again) to assume that the stack size is 128K
 * 	because this is the biggest that poe can handle.  With probing the 
 * 	stack to determine its size, each successive cthread fork ends up
 * 	giving us half as much stack space because we bump into the red zone
 * 	that cthreads puts at the bottom of the stack.  Just use a fixed sized
 * 	stack until we pick up the new, better cthreads.
 * 	[91/09/30            jjc]
 * 
 * Revision 1.9  91/05/05  19:25:24  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:52:17  dpj]
 * 
 * 	Made cthread_stack_size visible to the outside (i.e. not "private"). 
 * 	[91/04/28  09:55:29  dpj]
 * 
 * Revision 1.8  91/03/25  14:14:55  jjc
 * 	Changed stack_init() to probe for the bottom of the initial stack
 * 	instead of assuming the stack is 512K.
 * 	[90/01/21            jjc]
 * 
 * Revision 1.7  90/10/29  17:32:02  dpj
 * 	Merged-up to U25
 * 	[90/09/02  20:02:00  dpj]
 * 
 * 	Picked-up changes from jms to avoid calling getrlimit().
 * 	[90/08/15  14:37:10  dpj]
 * 
 * Revision 1.6  90/08/22  18:12:05  roy
 * 	Constant stack size to avoid unix calls.
 * 	[90/08/14  12:12:19  roy]
 * 
 * Revision 1.5  90/07/09  17:06:10  dorr
 * 	add cproc_stack().
 * 	[90/02/23  14:53:02  dorr]
 * 	Bug fix: Zero out the "round up" pages on stack allocation
 * 	[90/07/06  17:13:59  jms]
 * 
 * Revision 1.5  89/05/19  13:09:50  mbj
 * 	Don't deallocate the signal trampoline code, the args or the initial
 * 	environment even though they're on a stack.  They're actually per-task
 * 	rather than per-thread data.
 * 
 * Revision 1.4  89/05/04  17:48:56  mbj
 * 	Add an assertion to catch wraparound on stack allocation.
 * 	[89/05/04            mbj]
 * 
 * Revision 1.3  89/04/04  18:22:13  dorr
 * 	add HISTORY:
 * 
 * 31-Mar-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Use external definition of cthread_stack_base as starting
 *	address for stack allocation.  Normally defined as 0, but
 *	defined externally to allow for other values.
 *
 * 24-Mar-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Implement fork() for multi-threaded programs.
 *	Made MTASK version work correctly again.
 *
 * 01-Dec-87  Eric Cooper (ecc) at Carnegie Mellon University
 *	Changed cthread stack allocation to use aligned stacks
 *	and store self pointer at base of stack.
 *	Added inline expansion for cthread_sp() function.
 */


#include <cthreads.h>
#include "cthread_internals.h"

#include <sys/time.h>
#include <sys/resource.h>

#if	MTASK
#undef	mach_task_self  /* Must call the function since the variable is shared */
#endif	MTASK

/*
 * C library imports:
 */
#if OLDWAY
extern getrlimit(), perror();
#endif OLDWAY

#define	BYTES_TO_PAGES(b)	(((b) + vm_page_size - 1) / vm_page_size)

int cthread_stack_mask;

vm_size_t cthread_stack_size = 0;

#if	MTHREAD || COROUTINE

extern vm_address_t cthread_stack_base;	/* Base for stack allocation */

vm_address_t next_stack_base;

vm_address_t initial_stack_boundary;
private volatile vm_address_t initial_stack_deallocated = 0;
private spin_lock_t initial_stack_lock = 0;

#endif	MTHREAD || COROUTINE

/*
 * Set up a stack segment for a thread.
 * Segment has a red zone (invalid page)
 * for early detection of stack overflow.
 * The cproc_self pointer is stored at the base.
 *
 *	--------- (high address)
 *	|	|
 *	|  ...	|
 *	|	|
 *	| stack	|
 *	|	|
 *	|  ...	|
 *	|	|
 *	--------- (stack base)
 *	|	|
 *	|invalid|
 *	|	|
 *	---------
 *	|	|
 *	|	|
 *	| self	|
 *	--------- (low address)
 */

private void
setup_stack(p, base)
	register cproc_t p;
	register vm_address_t base;
{
	register kern_return_t r;

	/*
	 * Check alignment.
	 */
	ASSERT((base & cthread_stack_mask) == base);
	ASSERT(((base + cthread_stack_size - 1) & cthread_stack_mask) == base);
	/*
	 * Stack base is two pages from bottom.
	 */
	p->stack_base = (unsigned int) (base + 2*vm_page_size);
	/*
	 * Stack size is segment size minus two pages.
	 */
#if	MTHREAD || COROUTINE
	if (p->flags & CPROC_INITIAL_STACK) {
		p->stack_size = initial_stack_boundary - p->stack_base;
	} else {
		p->stack_size = cthread_stack_size - 2*vm_page_size;
	}
#else	MTHREAD || COROUTINE
	p->stack_size = cthread_stack_size - 2*vm_page_size;
#endif	MTHREAD || COROUTINE
	/*
	 * Protect red zone.
	 */
	MACH_CALL(vm_protect(mach_task_self(), base + vm_page_size, vm_page_size, FALSE, VM_PROT_NONE), r);
	/*
	 * Store self pointer.
	 */
	*((cproc_t *) base) = p;
}

vm_offset_t
cproc_stack(p)
	cproc_t		p;
{
	return p->stack_base + p->stack_size;
}

#if	MACH3
vm_offset_t
addr_range_check(start_addr, end_addr, desired_protection)
	vm_offset_t	start_addr, end_addr;
	vm_prot_t	desired_protection;
{
	register vm_offset_t	addr;

	addr = start_addr;
	while (addr < end_addr) {
	    vm_offset_t		r_addr;
	    vm_size_t		r_size;
	    vm_prot_t		r_protection,
				r_max_protection;
	    vm_inherit_t	r_inheritance;
	    boolean_t		r_is_shared;
	    memory_object_name_t	r_object_name;
	    vm_offset_t		r_offset;

	    r_addr = addr;
	    if (vm_region(mach_task_self(),
			  &r_addr,
			  &r_size,
			  &r_protection,
			  &r_max_protection,
			  &r_inheritance,
			  &r_is_shared,
			  &r_object_name,
			  &r_offset)
		    != KERN_SUCCESS
		  ||
		    r_addr > addr	/* gap */
		  ||
		    (r_protection & desired_protection)
			!= desired_protection
					/* not readable */
	       )
	    {
		return (0);
	    }
	    addr = r_addr + r_size;
	}
	return (addr);
}

/*
 * Probe for bottom and top of stack.
 * Assume:
 * 1. stack grows DOWN
 * 2. There is an unallocated region below the stack.
 */
void
probe_stack(stack_bottom, stack_top)
	vm_offset_t	*stack_bottom;
	vm_offset_t	*stack_top;
{
	/*
	 * Since vm_region returns the region starting at
	 * or ABOVE the given address, we cannot use it
	 * directly to search downwards.  However, we
	 * also want a size that is the closest power of
	 * 2 to the stack size (so we can mask off the stack
	 * address and get the stack base).  So we probe
	 * in increasing powers of 2 until we find a gap
	 * in the stack.
	 */
	vm_offset_t	start_addr, end_addr;
	vm_offset_t	last_start_addr, last_end_addr;
	vm_size_t	stack_size;

	/*
	 * Start with a page
	 */
	start_addr = cthread_sp() & ~(vm_page_size - 1);
	end_addr   = start_addr + vm_page_size;

	stack_size = vm_page_size;

	/*
	 * Increase the tentative stack size, by doubling each
	 * time, until we have exceeded the stack (some of the
	 * range is not valid).
	 */
	do {
	    /*
	     * Save last addresses
	     */
	    last_start_addr = start_addr;
	    last_end_addr   = end_addr;

	    /*
	     * Double the stack size
	     */
	    stack_size <<= 1;
	    start_addr = end_addr - stack_size;

	    /*
	     * Check that the entire range exists and is writable
	     */
	} while (end_addr = (addr_range_check(start_addr,
				  end_addr,
				  VM_PROT_READ|VM_PROT_WRITE)));
	/*
	 * Back off to previous power of 2.
	 */
	*stack_bottom = last_start_addr;
	*stack_top = last_end_addr;
}
#endif	MACH3

void
stack_init(p)
	cproc_t p;
{
	vm_offset_t	stack_bottom, stack_top;

	if (cthread_stack_size == 0) {	/* Written by emul_init ? */
#if	1
		cthread_stack_size = 128 * 1024;	/* XXX */
#else	1
#if	MACH3
		/*
		 * Probe for bottom and top of stack, as a power-of-2 size.
		 */
		probe_stack(&stack_bottom, &stack_top);

		cthread_stack_size = stack_top - stack_bottom;
#else	MACH3
		cthread_stack_size = 512 * 1024;	/* XXX dpj */
#endif	MACH3
#endif	1
	}
	cthread_stack_mask = ~(cthread_stack_size - 1);

#if	MTHREAD || COROUTINE
	/*
	 * Guess at first available region for stack.
	 */
	if (next_stack_base == 0)	/* Written by emul_init */
		next_stack_base = (cthread_stack_base + cthread_stack_size-1) &
				cthread_stack_mask;

	/*
	 * This is a pessimistic approximation of the top of the initial stack
	 * not including the thread trampoline code, the arguments and the
	 * initial environment.  It's pessimistic in that it contains a few
	 * stack frames which will be wasted when the stack is reallocated.
	 */
	initial_stack_boundary = (vm_address_t) cthread_sp();
#ifdef USE_CPROC_INITIAL_STACK
	p->flags |= CPROC_INITIAL_STACK;	/* We're "special" */
#endif USE_CPROC_INITIAL_STACK

#endif	MTHREAD || COROUTINE

	/*
	 * Set up stack for main thread.
	 */
	setup_stack(p, (vm_address_t) (cthread_sp() & cthread_stack_mask));
}


#if	defined(cthread_sp)
#else	/* not defined(cthread_sp) */
int
cthread_sp()
{
	int x;

	return (int) &x;
}
#endif	defined(cthread_sp)

#if	MTHREAD || COROUTINE

/*
 * Allocate a stack segment for a thread.  Stacks are never deallocated
 * except by the child process of a fork() during re-initialization.
 *
 * The variable next_stack_base is used to align stacks.
 * It may be updated by several threads in parallel,
 * but mutual exclusion is unnecessary: at worst,
 * the vm_allocate will fail and the thread will try again.
 */

void
alloc_stack(p)
	cproc_t p;
{
	vm_address_t base = next_stack_base;

	if (initial_stack_deallocated) {
		vm_address_t boundary_page;
		register kern_return_t r;

		spin_lock(&initial_stack_lock);
		if (! initial_stack_deallocated) {
			spin_unlock(&initial_stack_lock);
			goto normal_alloc_stack;
		}
		initial_stack_deallocated = 0;
		spin_unlock(&initial_stack_lock);

#ifdef USE_CPROC_INITIAL_STACK
		p->flags |= CPROC_INITIAL_STACK;
#endif USE_CPROC_INITIAL_STACK

		boundary_page = initial_stack_boundary &~ (vm_page_size - 1);
		base = initial_stack_boundary & cthread_stack_mask;

		if (initial_stack_boundary != boundary_page)
			bzero(boundary_page,
				initial_stack_boundary - boundary_page);

		MACH_CALL(vm_deallocate(mach_task_self(), base, boundary_page - base), r);
		MACH_CALL(vm_allocate(mach_task_self(), &base, boundary_page - base, FALSE), r);

		setup_stack(p, base);
		return;
	}

normal_alloc_stack:
	for (base = next_stack_base; ; ) {
	    ASSERT(cthread_stack_base == 0 || base != 0);
	    if (vm_allocate(mach_task_self(), &base, cthread_stack_size, FALSE)
		== KERN_SUCCESS) break;
	    base += cthread_stack_size;
	}

	next_stack_base = base + cthread_stack_size;
	setup_stack(p, base);
}

void dealloc_stack(p)
	cproc_t p;
{
	register kern_return_t r;

	if (p->flags & CPROC_INITIAL_STACK) {
		/*
		 * Don't deallocate the signal trampoline code, the args or
		 * the initial environment even though they're on a stack.
		 * They're actually per-task rather than per-thread data.
		 */
		initial_stack_deallocated = p->stack_base;
	} else {
		MACH_CALL(vm_deallocate(mach_task_self(), p->stack_base & cthread_stack_mask, cthread_stack_size), r);
	}
}

#endif	MTHREAD || COROUTINE

void stack_fork_child()
/*
 * Called in the child after a fork().  Resets stack data structures to
 * coincide with the reality that we now have a single cproc and cthread.
 */
{
#if	MTHREAD || COROUTINE
	/*
	 * Allocate stacks from the beginning of cthread_stack_base again.
	 */
	next_stack_base = (cthread_stack_base + cthread_stack_size-1) &
				cthread_stack_mask;
#endif	MTHREAD || COROUTINE
}
