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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_net_clts.cc,v $
 *
 * usNetCLTS: abstract class defining the basic connection-less 
 *	      network protocol. 
 *
 * HISTORY
 * $Log:	us_net_clts.cc,v $
 * Revision 2.5  94/07/07  17:25:14  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/06  07:54:02  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.3  92/07/05  23:30:16  dpj
 * 	Specified that some methods are interruptible.
 * 	[92/05/10  01:20:40  dpj]
 * 
 * Revision 2.2  91/11/06  13:50:17  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:54:34  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:53  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 */

#include <us_net_clts_ifc.h>

#define BASE usNetConnector
DEFINE_ABSTRACT_CLASS(usNetCLTS);


DEFINE_METHOD_ARGS(net_clts_read1rec,"rpc intr K<100901>: IN int; OUT * char[*:32767]; IN OUT * int; OUT * word[4]; OUT * int[3]; OUT * int");

DEFINE_METHOD_ARGS(net_clts_write1rec,"rpc intr K<100902>: IN int; IN * char[*:32767]; IN int; OUT * word[4]; IN * int[3]; IN * int");

