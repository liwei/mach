/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/syscall_val.h,v $
 *
 * Purpose: return value structure for system calls.
 *
 * This file was extracted from kern/syscall_emulation.h in the
 * Mach 3.0 kernel sources.
 *
 * HISTORY
 * $Log:	syscall_val.h,v $
 * Revision 2.3  94/07/08  15:51:40  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  90/10/29  17:23:00  dpj
 * 	Created by extracting some definitions out of
 * 	kern/syscall_emulation.h in the kernel sources,
 * 	which is not exported to applications.
 * 	[90/08/02  10:18:50  dpj]
 * 
 */

#ifndef	_syscall_val_h
#define	_syscall_val_h

/*
 *	Structure for calling emulated system calls in other OS
 */
typedef struct syscall_val {
	int	rv_val1;
	int	rv_val2;
} syscall_val_t;

#endif	_syscall_val_h
