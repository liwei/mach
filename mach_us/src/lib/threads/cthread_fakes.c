/* 
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
 */
/*
 *
 * File: us/lib/threads/cthread_fakes.c
 *
 * Fake implementations of features from "newer" versions of cthreads.
 *
 * 
 * Purpose:  Faked oup routines to make this threads package smell like the
 *	newer Mach threads package
 * 
 * HISTORY
 * $Log:	cthread_fakes.c,v $
 * Revision 2.3  94/07/08  14:14:26  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  94/01/11  17:49:48  jms
 * 	Initial Version
 * 	[94/01/09  19:31:29  jms]
 * 
 */

#include <cthreads.h>
#include "cthread_internals.h"
static int limit = 0x200;

int cthread_kernel_limit(){return(limit);}

void cthread_set_kernel_limit(val) 
	int val;
{
	limit = val;
}

void cthread_wire(){0;}
