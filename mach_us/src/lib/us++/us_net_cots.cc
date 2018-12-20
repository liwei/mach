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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_net_cots.cc,v $
 *
 * usNetCOTS: abstract class defining the basic connection-oriented
 *	      network protocol
 *
 * HISTORY
 * $Log:	us_net_cots.cc,v $
 * Revision 2.5  94/07/07  17:25:20  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/06  07:54:04  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.3  92/07/05  23:30:53  dpj
 * 	Specified that some methods are interruptible.
 * 	[92/05/10  01:21:48  dpj]
 * 
 * Revision 2.2  91/11/06  14:06:09  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:56:12  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:53  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 */

#include <us_net_cots_ifc.h>

#define BASE usNetBase
DEFINE_ABSTRACT_CLASS(usNetCOTS);

DEFINE_METHOD_ARGS(net_get_peeraddr, "rpc K<101001>: OUT * int[3]");
DEFINE_METHOD_ARGS(net_snddis, "rpc intr K<101002>: IN * char[*:2048]; IN int");
DEFINE_METHOD_ARGS(net_rcvdis, "rpc intr K<101003>: OUT * char[*:2048]; IN OUT * int");

