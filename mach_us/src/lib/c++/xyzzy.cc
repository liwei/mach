
/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 *//*
 * HISTORY
 * $Log:	xyzzy.cc,v $
 * Revision 2.3  94/07/07  17:46:21  mrt
 * 	Added copyright
 * 
 * Revision 2.2  91/11/06  11:29:06  jms
 * 	Created.
 * 	[91/09/26  18:51:32  pjg]
 * 
 */
// from tiemann

/* Needed, in case there are no other objects which
   need static initialization and cleanup.  */
struct __xyzzy__
{
  __xyzzy__ () {}
  ~__xyzzy__ () {}
} __1xyzzy__;

