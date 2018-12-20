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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/exception_error.h,v $
 *
 * Purpose: Definitions of signal numbers
 *
 * HISTORY:
 * $Log:	exception_error.h,v $
 * Revision 2.3  94/07/08  15:51:11  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  90/07/09  17:00:44  dorr
 * 	Initial revision.
 * 	[90/01/07  16:17:48  dorr]
 * 
 * 	created.
 * 	[89/08/11  14:57:27  dorr]
 * 	No Further Changes
 * 	[90/07/06  13:30:04  jms]
 * 
 */

#ifndef	_exception_error_h
#define	_exception_error_h

#include <mach/exception.h>

#define	except_sub	(err_server|err_sub(13))


/* the first range of signals consists of mach exceptions */
#define	EXCEPT_ACCESS		(except_sub|EXC_BAD_ACCESS)
#define	EXCEPT_BAD_INSTRUCTION	(except_sub|EXC_BAD_INSTRUCTION)
#define	EXCEPT_ARITHMETIC	(except_sub|EXC_ARITHMETIC)
#define	EXCEPT_EMULATION	(except_sub|EXC_EMULATION)
#define	EXCEPT_SOFTWARE		(except_sub|EXC_SOFTWARE)
#define	EXCEPT_BREAKPOINT	(except_sub|EXC_BREAKPOINT)

#endif _exception_error_h
