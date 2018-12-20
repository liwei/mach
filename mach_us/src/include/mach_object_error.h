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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/mach_object_error.h,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	mach_object_error.h,v $
 * Revision 1.8  94/07/08  15:51:22  mrt
 * 	Updated copyright.
 * 
 * Revision 1.7  91/07/01  14:05:59  jms
 * 	No further changes
 * 	[91/06/24  15:20:58  jms]
 * 
 * 	Add errors MACH_OBJECT_INVOKE_NOT_FOUND and MACH_OBJECT_MAX_INVOKES
 * 	[91/04/15  17:03:08  jms]
 * 
 * Revision 1.6  89/07/09  14:16:39  dpj
 * 	Updated error codes for new unified scheme.
 * 	Moved to err_server system.
 * 	[89/07/08  12:18:31  dpj]
 * 
 * Revision 1.5  89/05/17  15:58:51  dorr
 * 	include file cataclysm
 * 
 * Revision 1.4  88/11/04  11:19:26  dorr
 * add mach_object_bad_message.
 * 
 * Revision 1.3  88/10/18  12:01:24  sanzi
 * some new error codes.  remove oops dependency.
 * 
 * Revision 1.2  88/10/14  10:49:31  dorr
 * *** empty log message ***
 * 
 * Revision 1.1  88/10/14  10:45:19  dorr
 * Initial revision
 * 
 */

#ifndef	_MACH_OBJECT_ERROR_H
#define	_MACH_OBJECT_ERROR_H

#include <mach_error.h>

#define	MACH_OBJECT_MODULE		(err_server|err_sub(11))
#define	MACH_OBJECT_NOT_FOUND		(MACH_OBJECT_MODULE| 1)
#define	MACH_OBJECT_NO_SUCH_OPERATION	(MACH_OBJECT_MODULE| 2)
#define	MACH_OBJECT_UNDEFINED_ARGS	(MACH_OBJECT_MODULE| 3)
#define	MACH_OBJECT_MAXARGS		(MACH_OBJECT_MODULE| 4)
#define	MACH_OBJECT_BAD_MESSAGE		(MACH_OBJECT_MODULE| 5)
#define MACH_OBJECT_INVOKE_NOT_FOUND	(MACH_OBJECT_MODULE| 6)
#define MACH_OBJECT_MAX_INVOKES		(MACH_OBJECT_MODULE| 7)

#endif	_MACH_OBJECT_ERROR_H
