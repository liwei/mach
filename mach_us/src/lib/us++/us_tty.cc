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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_tty.cc,v $
 *
 * 
 * Purpose:  Support for "us_tty" interface
 * 
 * HISTORY
 * $Log:	us_tty.cc,v $
 * Revision 2.3  94/07/07  17:25:32  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  94/01/11  17:50:19  jms
 * 	Initial Version
 * 	[94/01/09  19:43:25  jms]
 * 
 * Revision 2.4  92/07/06  07:54:13  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.3  92/07/05  23:29:43  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:12:51  dpj]
 * 
 * Revision 2.2  91/11/06  13:49:13  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:42:40  pjg]
 * 
 * 	Created.
 * 	[91/04/14  18:39:37  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:27  pjg]
 * 
 */

#ifndef lint
char * us_tty_rcsid = "$Header: us_tty.cc,v 2.3 94/07/07 17:25:32 mrt Exp $";
#endif	lint

#include <us_tty_ifc.h>


#define BASE usByteIO
DEFINE_ABSTRACT_CLASS(usTTY)

DEFINE_METHOD_ARGS(tty_bsd_ioctl,"rpc intr K<101401>: IN int; IN OUT * char[128];");
