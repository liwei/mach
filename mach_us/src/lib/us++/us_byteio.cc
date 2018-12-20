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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_byteio.cc,v $
 *
 * usByteIO: abstract class defining the byte-oriented IO protocol. 
 *
 *
 * HISTORY
 * $Log:	us_byteio.cc,v $
 * Revision 2.5  94/07/07  17:25:04  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/06  07:53:45  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.3  92/07/05  23:29:48  dpj
 * 	Added dummy definition for io_map() as a null RPC.
 * 	[92/06/24  17:17:40  dpj]
 * 
 * 	Specified that some methods are interruptible.
 * 	[92/05/10  01:18:05  dpj]
 * 
 * Revision 2.2  91/11/06  13:49:20  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:49:38  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:41:32  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:03:58  pjg]
 * 
 */

#include <us_byteio_ifc.h>

#define BASE usItem
DEFINE_ABSTRACT_CLASS(usByteIO);


DEFINE_METHOD_ARGS(io_read,"rpc intr K<100201>: IN int; IN word[4]; OUT * char[*:32768]; IN OUT * int;");
DEFINE_METHOD_ARGS(io_write,"rpc intr K<100202>: IN int; IN word[4]; IN  * char[*:32768]; IN OUT * int;");
DEFINE_METHOD_ARGS(io_append,"rpc intr K<100203>: IN int;  IN  * char[*:32768]; IN OUT * int;");
DEFINE_METHOD_ARGS(io_read_seq,"rpc intr K<100204>: IN int; OUT * char[*:32768]; IN OUT * int; OUT * word[4];");
DEFINE_METHOD_ARGS(io_write_seq,"rpc intr K<100205>: IN int; IN * char[*:32768]; IN OUT * int; OUT * word[4];");
DEFINE_METHOD_ARGS(io_set_size,"rpc K<100206>: IN word[4];");
DEFINE_METHOD_ARGS(io_get_size,"rpc K<100207>: OUT * word[4];");

DEFINE_METHOD_ARGS(io_map,"rpc:");



