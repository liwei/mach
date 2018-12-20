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
 * ObjectClass: default_iobuf_mgr
 * Manager for a set of I/O buffer objects
 *		-- default version (private memory)
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/default_iobuf_mgr_ifc.h,v $
 *
 * HISTORY:
 * $Log:	default_iobuf_mgr_ifc.h,v $
 * Revision 2.4  94/07/07  17:23:06  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:27:03  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  16:08:47  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:08:44  dpj]
 * 
 * Revision 2.2  91/11/06  13:45:45  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:49:47  pjg]
 * 
 * Revision 2.2  91/05/05  19:25:55  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:53:01  dpj]
 * 
 * 	Removed support for user-defined blocks.
 * 	Removed explicit user info in records (now implicit).
 * 	[91/04/28  10:00:59  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:26:07  dpj]
 * 
 */

#ifndef	_default_iobuf_mgr_ifc_h
#define	_default_iobuf_mgr_ifc_h

#include	<top_ifc.h>

extern "C" {
#include <io_types.h>
#include <io_types2.h>
}

class default_iobuf_mgr: public VIRTUAL3 usTop {
	unsigned int		infosize;
      public:
	DECLARE_LOCAL_MEMBERS(default_iobuf_mgr);
	default_iobuf_mgr();

	virtual mach_error_t io_alloc_block(unsigned int, io_block_t *);
	virtual mach_error_t io_free_block(io_block_t);
	virtual mach_error_t io_alloc_record(unsigned int, io_record_t *);
	virtual mach_error_t io_free_record(io_record_t);
#ifdef	USERBLOCKS
	virtual mach_error_t io_copy_blocks(io_block_t, io_block_t);
	virtual mach_error_t io_copy_records(io_record_t, io_record_t);
#endif	USERBLOCKS
	virtual mach_error_t io_pullup_whole_record(io_record_t);

	virtual mach_error_t io_set_rec_infosize(unsigned int);
};

#endif	_default_iobuf_mgr_ifc_h
