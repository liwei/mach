/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/clone.cc,v $
 *
 * usClone: abstract class for objects that can be cloned
 *	    across address spaces.
 *
 * HISTORY
 * $Log:	clone.cc,v $
 * Revision 2.3  94/07/07  17:23:01  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:26:54  dpj
 * 	First working version.
 * 	[92/06/24  16:07:29  dpj]
 * 
 */

#include <clone_ifc.h>

#ifdef	GXXBUG_CLONING1
#define	BASE	usItem
#else	GXXBUG_CLONING1
#define	BASE	usRemote
#endif	GXXBUG_CLONING1

DEFINE_ABSTRACT_CLASS(usClone);

