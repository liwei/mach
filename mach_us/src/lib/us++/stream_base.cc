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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/stream_base.cc,v $
 *
 * Purpose: General-purpose queue for undifferentiated bytes (byte-stream).
 *
 * HISTORY:
 * $Log:	stream_base.cc,v $
 * Revision 2.4  94/07/07  17:24:41  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  94/05/17  14:08:08  jms
 * 	Need implementations for virtual methods in class stream_base for 2.3.3 g++-modh
 * 	[94/04/28  18:54:42  jms]
 * 
 * Revision 2.2.1.1  94/02/18  11:29:37  modh
 * 	Need implementations for virtual methods in class stream_base for 2.3.3 g++
 * 
 * Revision 2.2  91/11/06  13:48:15  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:15:20  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:22:21  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:33:00  pjg]
 * 
 * Revision 2.3  89/11/28  19:10:57  dpj
 * 	Modified to support writing with arbitrary sizes,
 * 	with high-water-mark for flow control and
 * 	variable-length list of blocks.
 * 	[89/11/22            dpj]
 * 
 * 	Added I/O strategies (IOS_ENABLED, IOS_WAIT_ALLOWED) in
 * 	io_read() and io_write().
 * 	Implemented io_set_{read,write}_strategy().
 * 	[89/11/20  20:40:26  dpj]
 * 
 * Revision 2.2  89/10/30  16:31:26  dpj
 * 	First version.
 * 	[89/10/27  17:25:46  dpj]
 * 
 *
 */

#ifndef lint
char * stream_base_rcsid = "$Header: stream_base.cc,v 2.4 94/07/07 17:24:41 mrt Exp $";
#endif	lint

#include <stream_base_ifc.h>


#define BASE iobuf_user
DEFINE_LOCAL_CLASS(stream_base)


stream_base::stream_base(default_iobuf_mgr* mgr,
	       io_strategy_t readstrat, io_strategy_t writestrat)
	: iobuf_user(mgr),
	  read_strategy(readstrat), write_strategy(writestrat)
{}

mach_error_t
stream_base::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
  return _notdef();
}

mach_error_t
stream_base::ns_duplicate(ns_access_t access, usItem** newobj)
{
  return _notdef();
}

mach_error_t
stream_base::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
  return _notdef();
}

mach_error_t
stream_base::ns_set_times(time_value_t atime, time_value_t mtime)
{
  return _notdef();
}

mach_error_t
stream_base::ns_get_protection(ns_prot_t prot, int* protlen)
{
  return _notdef();
}

mach_error_t
stream_base::ns_set_protection(ns_prot_t prot, int protlen)
{
  return _notdef();
}

mach_error_t
stream_base::ns_get_privileged_id(int* id)
{
  return _notdef();
}

mach_error_t
stream_base::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
  return _notdef();
}

mach_error_t
stream_base::ns_get_manager(ns_access_t access, usItem **newobj)
{
  return _notdef();
}

