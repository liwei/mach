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
 */
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_sys.cc,v $
 *
 * usSys: abstract class defining remotely callable sys/config interfacd
 *
 * HISTORY
 * $Log:	us_sys.cc,v $
 * Revision 2.5  94/07/07  17:25:27  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/06  07:53:49  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.3  92/07/05  23:31:19  dpj
 * 	Derive from usItem instead of usRemote.
 * 	[92/06/24  17:22:31  dpj]
 * 
 * Revision 2.2  91/11/06  14:08:45  jms
 * 	Interface support for new config server "sys" object.
 * 	[91/11/04  17:29:56  jms]
 * 
 * Revision 2.2  91/10/06  22:48:23  jjc
 * 	Fixed arguments to sys_get_prefix_table().
 * 	[91/10/01  19:04:49  jjc]
 * 
 * 	Created
 * 	[91/06/25            jjc]
 * 
 */

#include <us_sys_ifc.h>

#define BASE usItem
DEFINE_ABSTRACT_CLASS(usSys);

DEFINE_METHOD_ARGS(sys_get_prefix_table,
  "rpc K<100401>: OUT * object<usItem>[*:32]; OUT * int; OUT COPY ** char[*][256]; OUT * int;");
