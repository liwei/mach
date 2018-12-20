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
 * File: us/lib/threads/stack_base.c
 *
 * C Thread stack allocation base address.
 *
 * HISTORY
 * $Log:	stack_base.c,v $
 * Revision 1.4  94/07/08  14:14:44  mrt
 * 	Updated copyright
 * 
 * Revision 1.3  90/07/09  17:06:19  dorr
 * 	Unchanged
 * 	[90/07/06  17:16:10  jms]
 * 
 * 31-Mar-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Provide external definition of cthread_stack_base as starting
 *	address for stack allocation.  Normally defined as 0, but
 *	another definition could be loaded to allow for other values.
 */


#include <cthreads.h>
#include "cthread_internals.h"

#if	MTHREAD || COROUTINE

vm_address_t cthread_stack_base = 0;	/* Base for stack allocation */

#endif	MTHREAD || COROUTINE
