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
 * ObjectClass: iobuf_user
 * Automatic forwarding of iobuf methods to the manager.
 * 
 * SuperClass: base
 * 	
 * Delegated Objects:
 * 
 * ClassMethods:
 *
 * Notes:
 *
 * Bugs:
 * 
 * Features:
 * 
 * Transgressions:
 *
 */
 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/iobuf_user_ifc.h,v $
 *
 * HISTORY:
 * $Log:	iobuf_user_ifc.h,v $
 * Revision 2.4  94/07/07  17:23:29  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:27:43  dpj
 * 	Conditionalized virtual base class specifications.
 * 	Derive from usItem instead of usTop.
 * 	[92/06/24  16:25:29  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:38  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:53:14  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:38  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:54:02  dpj]
 * 
 * 	Removed support for user-defined blocks.
 * 	Removed explicit user info in records (now implicit).
 * 	[91/04/28  10:11:51  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:28:07  dpj]
 * 
 */

#ifndef	_iobuf_user_ifc_h
#define	_iobuf_user_ifc_h

#include	<us_item_ifc.h>
#include	<default_iobuf_mgr_ifc.h>


/*
 * Base class only really needs to be usTop, but we make it usItem
 * in order to avoid forcing usTop to be declared "virtual" everywhere".
 *	- dpj 06/04/92
 */
class iobuf_user: public VIRTUAL2 usItem {
//class iobuf_user: public VIRTUAL4 usTop {
	default_iobuf_mgr		*mgr;
      public:
	DECLARE_LOCAL_MEMBERS(iobuf_user);
	iobuf_user(default_iobuf_mgr * =0);
	virtual ~iobuf_user();

	virtual mach_error_t io_alloc_block(unsigned int, io_block_t *);
	virtual mach_error_t io_free_block(io_block_t);
	virtual mach_error_t io_alloc_record(unsigned int, io_record_t *);
	virtual mach_error_t io_free_record(io_record_t);
#ifdef	USERBLOCKS
	virtual mach_error_t io_copy_blocks(io_block_t, io_block_t);
	virtual mach_error_t io_copy_records(io_record_t, io_record_t);
#endif	USERBLOCKS
	virtual mach_error_t io_pullup_whole_record(io_record_t);
};

#endif	_iobuf_user_ifc_h
