/*
 **********************************************************************
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
 **********************************************************************
 */
/*
 * tty_utils.c
 *
 * Various utility routines used by the tty server.
 *
 * Michael B. Jones  --  6-Oct-1988
 */

/*
 * HISTORY: $Log:	tty_utils.c,v $
 * Revision 1.2  94/07/21  16:26:48  mrt
 * 	Updated copyright
 * 
 * $Log:	tty_utils.c,v $
 * Revision 1.2  94/07/21  16:26:48  mrt
 * 	Updated copyright
 * 
 * Revision 1.1  88/10/28  01:31:34  mbj
 * Initial revision
 * 
 *  6-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */


extern char *malloc();

char *newstr(str) char *str;
{
    register char *result = malloc(strlen(str) + 1);
    if (result) strcpy(result, str);
    return result;
}

