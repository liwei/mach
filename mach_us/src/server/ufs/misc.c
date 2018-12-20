
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
 * File:        misc.c
 * Purpose:
 *	miscellaneous routines.
 *	
 *
 * HISTORY:
 * $Log:	misc.c,v $
 * Revision 1.13  94/07/21  11:58:49  mrt
 * 	updated copyright
 * 
 * Revision 1.12  92/03/05  15:15:56  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:50:28  jms]
 * 
 * Revision 1.11  91/12/20  17:45:20  jms
 * 	Add "suspend_on_panic" debugging feature.
 * 	[91/12/20  16:26:47  jms]
 * 
 * Revision 1.10  89/05/18  10:51:38  dorr
 * 	include file cataclysm
 * 
 * Revision 1.9  89/03/17  13:07:12  sanzi
 * 	include debug.h
 * 	[89/03/12  20:25:09  dorr]
 * 
 *
 *   03-Mar-1988  Douglas Orr (dorr) at Carnegie-Mellon University
 *	Created.
 */

#include "base.h"
#include <debug.h>

#include <stdio.h>

#include <mach/kern_return.h>
#define	EXPORT_BOOLEAN
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>

int suspend_on_panic = 0;

/*
 * copystr: copy a null terminated string from one point to another
 * Returns a nonzero error code if the last character copied
 * was not the null character.
 *
 *	copystr(from, to, maxlength, &lencopied)
 */
copystr( from, to, maxlength, lencopied )
	char		* from;
	char		* to;
	unsigned	maxlength;
	unsigned	* lencopied;
{
	char * org_to = to;

	while( maxlength-- > 0 )
		if( (*to++ = *from++) == '\0' ) break;

	if( lencopied != (unsigned *)0 )
		*lencopied = org_to - to;

	return( ! ((to > org_to) && (to[-1] == '\0')) );
}

/*VARARGS*/
panic( fmt, a0, a1, a2, a3, a4, a5, a6 )
	char		* fmt;
{
	printf( "panic: " );
	printf( fmt, a0, a1, a2, a3, a4, a5, a6 );
	printf( "\n" );
	if (suspend_on_panic) printf("Suspending upon panic.\n");
	fflush( stdout );
	fflush( stderr );
	if (suspend_on_panic) {
		task_suspend(mach_task_self());
	}
	exit( 1 );
}


vm_offset_t
kmem_alloc( map, size )
	int		map;
	unsigned	size;
{
	vm_offset_t	data = (vm_offset_t)0;

	/* ignore the map... */
	if( vm_allocate( mach_task_self(), &data, size, TRUE ) != KERN_SUCCESS )
		return( (vm_offset_t)0 );

	return( data );

}

kmem_free( map, space, size )
	unsigned	map;
	vm_offset_t	space;
	vm_size_t	size;
{
	(void)vm_deallocate( mach_task_self(), space, size );
}



#if	CS_RPAUSE
/* 
 *  rpsleep - perform a resource pause sleep
 *
 *  rsleep = function to perform resource specific sleep
 *  arg1   = first function parameter
 *  arg2   = second function parameter
 *  mesg1  = first component of user pause message
 *  mesg2  = second component of user pause message
 *
 *  Display the appropriate pause message on the user's controlling terminal.
 *  Save the current non-local goto information and establish a new return
 *  environment to transfer here.  Invoke the supplied function to sleep
 *  (possibly interruptably) until the resource becomes available.  When the
 *  sleep finishes (either normally or abnormally via a non-local goto caused
 *  by a signal), restore the old return environment and display a resume
 *  message on the terminal.
 *
 *  Return: true if the resource has now become available, or false if the wait
 *  was interrupted by a signal.
 */

boolean_t
rpsleep(rsleep, arg1, arg2, mesg1, mesg2)
int (*rsleep)();
int arg1;
int arg2;
char *mesg1;
char *mesg2;
{
    uprintf("[%s: %s%s, you lose ...]\r\n", "uck", mesg1, mesg2);
    exit( 1 );
}
#endif	CS_RPAUSE
