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
 * File:        sync.c
 * Purpose:
 *	user space process syncronization (just like in the kernel)
 *	
 *
 * HISTORY
 * $Log:	sync.c,v $
 * Revision 1.6  94/07/21  11:58:58  mrt
 * 	updated copyright
 * 
 * Revision 1.5  94/05/17  14:11:03  jms
 * 	Change private to capital PRIVATE for 2.3.3 g++ -modh
 * 	[94/04/29  13:49:11  jms]
 * 
 * Revision 1.4.1.1  94/02/18  11:41:34  modh
 * 	change private to capital PRIVATE for 2.3.3 g++
 * 
 * Revision 1.4  92/07/05  23:36:53  dpj
 * 	private -> PRIVATE to avoid C++ problem.
 * 
 * 	  03-Mar-1988  Douglas Orr (dorr) at Carnegie-Mellon University
 * 	[92/06/24  17:45:27  dpj]
 * 
 *	Created.
 */

#include <base.h>

#ifdef KERNEL
#define _KERNEL KERNEL
#undef KERNEL
#endif KERNEL

#include <cthreads.h>
#include <stdio.h>

#ifdef _KERNEL
#define	KERNEL _KERNEL
#endif _KERNEL

/*
 *	XXX big plan:  The big plan is to replace sleeps and 
 *  wakeups with individual condition variables appropriate to
 *  whatever action is being taken.  For the short run, these
 *  definitions of sleep and wakeup should provide enough syncronization
 *  to get going.
 *
 *  If we ever wanted to do this seriously, we should cache and hash.
 */

PRIVATE struct mutex sync_lock = MUTEX_INITIALIZER;

#define	COND_NULL	(struct cond_list *)0
PRIVATE struct cond_list {
	condition_t		cond;
	vm_offset_t		val;
	struct cond_list	* next;
} * cond_list = COND_NULL;


/*
 *	find_condition:  find any existing condition that
 *  matches the given val.  If none exists, create one.
 *  return the new condition.  Always called with the sync_lock
 *  on.
 */
PRIVATE condition_t
find_condition( val )
{
	register struct cond_list * cl;

	for( cl = cond_list; cl; cl = cl->next ) {
		if( cl->val == val ) break;
	}

	if( cl == COND_NULL ) {
		cl = New( struct cond_list );
		cl->cond = condition_alloc();
		cl->val = val;
		cl->next = cond_list;
		cond_list = cl;
	}

	return( cl->cond );
}

/*
 *	free_condition:  free the condition corresponding to
 *  the given value.
 */
PRIVATE
free_condition( val )
{
	register struct cond_list * cl, * prev;

	for( prev = COND_NULL, cl = cond_list; 
	     cl; 
	     prev = cl, cl = cl->next ) {
		if( cl->val == val ) break;
	}

	if( !cl ) panic( "free_condition" );

	/* unlink */
	if ( prev )
		prev->next = cl->next;
	else
		cond_list = cl->next;

#undef	free	
	condition_free( cl->cond );
	free( cl );

}


sleep( val )
	vm_offset_t	val;
{

	mutex_lock( &sync_lock );

	/* see if this is in our list of existing conditions */
	condition_wait( find_condition( val ), &sync_lock );

	mutex_unlock( &sync_lock );

}


wakeup( val )
	vm_offset_t	val;
{

	mutex_lock( &sync_lock );

	condition_broadcast( find_condition( val ) );	/* wake everybody up everyone who's asleep */
	free_condition( val );				/* and ditch this condition */

	mutex_unlock( &sync_lock );

}
