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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/local_include/debug.h,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	debug.h,v $
 * Revision 1.13  94/07/08  18:42:47  mrt
 * 	Updated copyright
 * 
 * Revision 1.12  92/07/05  23:33:06  dpj
 * 	Fixed some declarations for C++.
 * 	[92/05/10  01:25:45  dpj]
 * 
 * Revision 1.11  92/03/05  15:07:42  jms
 * 	Evaluate the ASSERT args even when not debugging.  They may have required
 * 	side effects.
 * 	[92/02/26  18:53:58  jms]
 * 
 * Revision 1.10  91/11/13  17:19:08  dpj
 * 	Added ASSERT_RETCODE and ASSERT_EXPR.
 * 	[91/11/06  20:43:37  dpj]
 * 
 * Revision 1.9  91/11/06  14:13:10  jms
 * 	Ifdef'd the constructs related to C++, so that this file could
 * 	still be included by C files.
 * 	[91/10/03  15:02:11  pjg]
 * 
 * 	Changed to work with C++ objects.
 * 	[91/09/26  18:23:09  pjg]
 * 
 * Revision 1.8  91/05/05  19:29:13  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:52  dpj]
 * 
 * 	Added us_internal_error().
 * 	[91/04/28  10:36:42  dpj]
 * 
 * Revision 1.7  91/02/25  17:50:59  mal
 * 	no change
 * 	[91/02/25  17:44:28  mal]
 * 
 * Revision 1.6  90/10/29  17:33:04  dpj
 * 	Merged-up to U25
 * 	[90/09/02  20:02:37  dpj]
 * 
 * 	Picked-up fixes from roy to clean-up printf and co.
 * 	[90/08/15  14:53:32  dpj]
 * 
 * Revision 1.5  90/10/02  11:33:00  mbj
 * 	Made statement-like macros legal in all statement contexts.
 * 	[90/10/01  14:51:49  mbj]
 * 
 * Revision 1.4  90/08/22  18:13:14  roy
 * 	Removed #define of printf.
 * 	[90/08/14  12:01:27  roy]
 * 
 * Revision 1.3  89/07/09  14:20:36  dpj
 * 	Reorganized to work with new Diag objects, and uniform
 * 	DEBUG and ERROR macros.
 * 	[89/07/08  13:07:45  dpj]
 * 
 * Revision 1.2  89/03/17  12:59:01  sanzi
 * 	redefine printf if debugging.
 * 	[89/03/12  20:33:00  dorr]
 * 	
 * 	get rid of manditory DEBUG setting.  add NoDebug flag.
 * 	[89/03/11  23:23:16  dorr]
 * 
 */

#ifndef	_DEBUG_H
#define	_DEBUG_H

#include	<mach/error.h>
#include	<us_error.h>

#include <macro_help.h>

#define	Dbg_Level_Min		0
#define	Dbg_Level_Max		0x7fffffff

#define	Dbg_Level_Critical	-3
#define	Dbg_Level_Error		-2
#define	Dbg_Level_Info		-1
#define	Dbg_Level_0		0
#define	Dbg_Level_1		1
#define	Dbg_Level_2		2


/*
 * Global variables controlling debugging.
 */
extern int			us_debug_level;
extern int			us_debug_force;


/*
 * Global DIAG object.
 *
 * This is defined as a char * to avoid a dependency on mach_object.h.
 */
extern char			*us_diag_object;
#define Diag			((char*) us_diag_object)


/*
 * Global functions exported by the DIAG system.
 */
extern void			us_init_diag();
extern void			diag_critical();
extern void			diag_error();
extern void			diag_info();
extern void			diag_debug0();
extern void			diag_debug1();
extern void			diag_debug2();
extern void			diag_dprintf();
extern mach_error_t		diag_startup();

/*
 * Basic macros.
 *
 * _DEBUG_ controls debugging statements.
 */
#define	CRITICAL(args)						\
    MACRO_BEGIN							\
	if (us_debug_level > Dbg_Level_Critical)		\
		diag_critical args;				\
    MACRO_END

#define	ERROR(args)						\
    MACRO_BEGIN							\
	if (us_debug_level > Dbg_Level_Error)			\
		diag_error args;				\
    MACRO_END

#define	INFO(args)						\
    MACRO_BEGIN							\
	if (us_debug_level > Dbg_Level_Info)			\
		diag_info args;					\
    MACRO_END

#if	_DEBUG_

#define	DEBUG0(cond,args)					\
    MACRO_BEGIN							\
	if ((us_debug_level > Dbg_Level_0) &&			\
				(us_debug_force || (cond)))	\
		diag_debug0 args;				\
    MACRO_END

#define	DEBUG1(cond,args)					\
    MACRO_BEGIN							\
	if ((us_debug_level > Dbg_Level_1) &&			\
				(us_debug_force || (cond)))	\
		diag_debug1 args;				\
    MACRO_END

#define	DEBUG2(cond,args)					\
    MACRO_BEGIN							\
	if ((us_debug_level > Dbg_Level_2) &&			\
				(us_debug_force || (cond)))	\
		diag_debug2 args;				\
    MACRO_END

#else	_DEBUG_

#define	DEBUG0(cond,args)
#define	DEBUG1(cond,args)
#define	DEBUG2(cond,args)

#endif	_DEBUG_


/*
 * Obsolete macros, kept here for compatibility.
 */
#if	_DEBUG_
#define	Debug(stmt)						\
	if (us_debug_level > Dbg_Level_0)			\
		stmt
#else	_DEBUG_
#define	Debug(stmt)
#endif	_DEBUG_


/*
 * Replacements for Mach error routines.
 */
#define	mach_error(str,err)					\
	ERROR((Diag,"%s: %s\n",str,mach_error_string(err)))
#define	mach_error_critical(str,err)				\
	CRITICAL((Diag,"%s: %s\n",str,mach_error_string(err)))


/*
 * Standard reporting of US problems.
 */
#ifndef __cplusplus
extern struct debug_self { int a, b, c; }     self;
extern void                                   diag_us_internal_error();

#define	us_internal_error(str,err)				\
	(diag_us_internal_error(self,str,err,__FILE__,__LINE__),\
	 US_INTERNAL_ERROR)
#else
void diag_us_internal_error(char *, mach_error_t, char *, int); 
#define	us_internal_error(str,err)				\
	(diag_us_internal_error(str,err,__FILE__,__LINE__),	\
	 US_INTERNAL_ERROR)
#endif /* __cplusplus */

#if	_DEBUG_

#define	ASSERT_RETCODE(_msg,_ret)					\
	MACRO_BEGIN							\
    		mach_error_t	_localret = (_ret);			\
		if (_localret != ERR_SUCCESS) {				\
			us_internal_error((_msg),_localret);		\
		}							\
	MACRO_END

#define	ASSERT_EXPR(_msg,_expr)						\
	MACRO_BEGIN							\
		if (! (_expr)) {					\
			us_internal_error((_msg),US_INTERNAL_ERROR);	\
		}							\
	MACRO_END

#else	_DEBUG_

#define	ASSERT_RETCODE(_msg,_ret)	(_ret)
#define	ASSERT_EXPR(_msg,_expr)		(_expr)

#endif	_DEBUG_

#endif	_DEBUG_H
