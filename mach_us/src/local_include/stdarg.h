/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992-1988 Carnegie Mellon University
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
 **********************************************************************
 * HISTORY
 * $Log:	stdarg.h,v $
 * Revision 2.3  94/07/08  18:46:23  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  92/07/05  23:33:15  dpj
 * 	First version. Installed here for easier compilation.
 * 	[92/05/10  01:26:56  dpj]
 * 
 * Revision 1.3  89/12/24  16:52:20  bww
 * 	Corrected definition of va_start().  Enabled support
 * 	for BYTE_MSF machines, and included support for PMAX.
 * 	Removed declaration for _doprnt().  Utilized #error.
 * 	[89/12/24  16:51:32  bww]
 * 
 * Revision 1.2  89/05/26  12:29:53  bww
 * 	CMU CS as of 89/05/15
 * 	[89/05/26  09:46:50  bww]
 * 
 * Revision 2.3  89/01/20  15:43:52  gm0w
 * 	Cleaned up module a bit.
 * 	[88/12/27            gm0w]
 * 
 * Revision 2.2  88/12/14  23:34:32  mja
 * 	Created.
 * 	[88/01/06            jjk]
 * 
 **********************************************************************
 */

/*
 *  This is the ANSI C version of varargs.h.  If you are using
 *  a non-ANSI compiler you should include <varargs.h> instead.
 */
#if !__STDC__
#error You MUST have an ANSI compatible compiler to use stdarg.h
#endif

#ifndef _STDARG_H_
#define _STDARG_H_ 1

/*
 * USAGE:
 *	f( arg-declarations, ... ) {
 *		va_list ap;
 *		va_start(ap, parmN);	// parmN == last named arg
 *		// ...
 *		type arg = va_arg(ap, type);
 *		// ...
 *		va_end(ap);
 *	}
 */

typedef char *va_list;
#if	mips
/* PMAX pushes floats on the stack only on double longword boundaries */
# define va_start(list,parmN) list = ((char *)(&parmN) +\
				(sizeof(parmN) > 4 ? (2*8 - 1) & -8 \
						   : (2*4 - 1) & -4))
#else
# define va_start(list,parmN) list = ((char *)(&parmN) +\
				((sizeof(parmN)+3)&(-4)))
#endif
# define va_end(list)
#if	BYTE_MSF
# define va_arg(list,mode) ((mode *)((list += (sizeof(mode)+3)&(-4))\
		-((sizeof(mode)<4)?sizeof(mode):(sizeof(mode)+3)&(-4))))[0]
#else	/* BYTE_MSF */
#if	mips
/* PMAX pushes floats on the stack only on double longword boundaries */
# define va_arg(list,mode) ((mode *)((list = \
        (char *) (sizeof(mode) > 4 ? ((int)list + 2*8 - 1) & -8 \
                                   : ((int)list + 2*4 - 1) & -4))\
		-((sizeof(mode)+3)&(-4))))[0]
#else
# define va_arg(list,mode) ((mode *)((list += (sizeof(mode)+3)&(-4))\
		-((sizeof(mode)+3)&(-4))))[0]
#endif
#endif	/* BYTE_MSF */

#include <stdio.h>
extern int vprintf(const char *, va_list);
extern int vfprintf(FILE *, const char *, va_list);
extern int vsprintf(char *, const char *, va_list);
#endif	/* not _STDARG_H_ */
