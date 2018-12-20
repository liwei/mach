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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/base.h,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	base.h,v $
 * Revision 1.14  94/07/08  15:49:24  mrt
 * 	Updated copyright.
 * 
 * Revision 1.13  92/07/05  23:23:03  dpj
 * 	Added extern declarations for C++.
 * 	[92/05/10  00:14:24  dpj]
 * 
 * Revision 1.12  91/11/13  16:30:44  dpj
 * 	Converted for C++ usage. Replaced "private" keyword with "PRIVATE".
 * 	[91/11/06  20:39:08  dpj]
 * 
 * Revision 1.11  89/10/30  16:27:33  dpj
 * 	Added Malloc() and NULL.
 * 	[89/10/15  20:07:58  dpj]
 * 
 * Revision 1.10  89/07/19  11:38:53  dorr
 * 	add ZeroNew and Endof.
 * 
 * Revision 1.9  89/07/09  14:16:18  dpj
 * 	Added include of debug.h.
 * 	[89/07/08  12:13:36  dpj]
 * 
 * Revision 1.8.1.1  89/07/06  14:06:34  dorr
 * 	add Endof()
 * 
 * Revision 1.8  89/05/17  15:54:00  dorr
 * 	include file cataclysm
 * 
 * Revision 1.7  88/12/20  11:35:40  mbj
 * Modified MIN, MAX macros so cpp won't complain about their "redefinition".
 * 
 * Revision 1.6  88/10/30  15:04:03  dorr
 * add NewStr
 * 
 * Revision 1.5  88/10/27  16:10:13  dorr
 * define private.
 * 
 * Revision 1.4  88/10/14  21:20:20  dpj
 * Added MAX and MIN.
 * 
 * Revision 1.3  88/09/21  15:59:22  sanzi
 * add declaration of malloc().
 * 
 * Revision 1.2  88/08/28  16:38:10  dorr
 * add array operations.
 * 
 * Revision 1.1  88/08/18  16:18:17  dorr
 * Initial revision
 * 
 * Revision 1.9  89/07/09  14:16:18  dpj
 * 	Added include of debug.h.
 * 	[89/07/08  12:13:36  dpj]
 */

#ifndef	_BASE_H
#define	_BASE_H

#ifdef	__cplusplus
extern "C" {
#include <mach_error.h>
#include <debug.h>
}
#else	__cplusplus
#include <mach_error.h>
#include <debug.h>
#endif	__cplusplus

/*
 * Space allocator.
 */
extern char *malloc();
extern void free();
#define	Malloc(size)		malloc(size)
#define	New(typ)		(typ *)malloc(sizeof(typ))
#define NewArray(typ,cnt)	(typ *)malloc(sizeof(typ)*(cnt))
#define NewStr(str)		(char *)strcpy(malloc(strlen(str)+1),str)
#define ZeroNew(cnt,typ)	(typ *)calloc(cnt,sizeof(typ))
#define	Free(ptr)		free(ptr)

/*
 * Array operations
 */
#define	Count(arr)		(sizeof(arr)/sizeof(arr[0]))
#define	Lastof(arr)		(Count(arr)-1)
#define	Endof(arr)		(&(arr)[Count(arr)])

/*
 * Trivial macros.
 */
#define	MIN(a,b) (((a)<(b))?(a):(b))
#define	MAX(a,b) (((a)>(b))?(a):(b))
#ifndef	NULL
#define	NULL	0
#endif	NULL
#ifndef	TRUE
#define	TRUE	1
#define	FALSE	0
#endif	TRUE

#if	_DEBUG_
#define	PRIVATE
#else
#define	PRIVATE static
#endif  _DEBUG_

/*
 * Random extern declarations.
 */
#ifdef	__cplusplus
extern "C" {
extern void bcopy();
extern int bcmp();
extern void bzero();
#include	<strings.h>
}
#else	__cplusplus
extern void bcopy();
extern int bcmp();
extern void bzero();
#include	<strings.h>
#endif	__cplusplus


#endif	_BASE_H

