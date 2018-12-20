/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/iobuf_user.cc,v $
 *
 * Purpose: automatic forwarding of iobuf methods to the manager.
 *
 * HISTORY:
 * $Log:	iobuf_user.cc,v $
 * Revision 2.4  94/07/07  17:23:27  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:27:41  dpj
 * 	Derive from usItem instead of usTop.
 * 	[92/06/24  16:24:47  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:35  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:47:29  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:36  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:53:59  dpj]
 * 
 * 	Removed support for user-defined blocks.
 * 	Removed explicit user info in records (now implicit).
 * 	[91/04/28  10:11:32  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:27:51  dpj]
 * 
 */

#include	<iobuf_user_ifc.h>

#define BASE usItem
//#define BASE usTop
DEFINE_LOCAL_CLASS(iobuf_user)


iobuf_user::iobuf_user(default_iobuf_mgr *mgr_obj)
: mgr(mgr_obj)
{
	mach_object_reference(mgr);
}


iobuf_user::~iobuf_user()
{
	mach_object_dereference(mgr);
}


mach_error_t iobuf_user::io_alloc_block(unsigned int size, io_block_t *newblk)
{
	return mgr->io_alloc_block(size,newblk);
}


mach_error_t iobuf_user::io_free_block(io_block_t blk)
{
	return mgr->io_free_block(blk);
}


mach_error_t 
iobuf_user::io_alloc_record(unsigned int blksize, io_record_t *newrec)
{
	return mgr->io_alloc_record(blksize,newrec);
}


mach_error_t iobuf_user::io_free_record(io_record_t rec)
{
	return mgr->io_free_record(rec);
}


#ifdef	USERBLOCKS

mach_error_t iobuf_user::io_copy_blocks(io_block_t fromblk, io_block_t toblk)
{
	return mgr->io_copy_blocks(fromblk,toblk);
}


mach_error_t 
iobuf_user::io_copy_records(io_record_t fromrec, io_record_t torec)
{
	return mgr->io_copy_records(fromrec,torec);
}

#endif	USERBLOCKS


mach_error_t iobuf_user::io_pullup_whole_record(io_record_t rec)
{
	return mgr->io_pullup_whole_record(rec);
}
