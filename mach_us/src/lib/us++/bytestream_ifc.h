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
 * ObjectClass: bytestream
 * 	General-purpose queue for undifferentiated bytes (byte-stream).
 * 
 */
 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/bytestream_ifc.h,v $
 *
 * HISTORY:
 * $Log:	bytestream_ifc.h,v $
 * Revision 2.5  94/07/07  17:22:51  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/05/17  14:07:12  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/28  18:43:48  jms]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:22:35  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:02:34  pjg]
 * 
 * Revision 2.3  92/07/05  23:26:48  dpj
 * 	Define as an abstract class instead of a concrete class.
 * 	[92/06/24  16:06:12  dpj]
 * 
 * Revision 2.2  91/11/06  13:45:29  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:49:21  pjg]
 * 
 * Revision 2.4  91/05/05  19:25:48  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:52:55  dpj]
 * 
 * 	Reorganized for split between basic interface (io_methods) and
 * 	extended interface (io_methods2).
 * 	[91/04/28  09:58:54  dpj]
 * 
 * Revision 2.3  89/11/28  19:11:01  dpj
 * 	Modified to support writing with arbitrary sizes,
 * 	with high-water-mark for flow control and
 * 	variable-length list of blocks.
 * 	[89/11/22            dpj]
 * 
 * 	Added I/O strategies and related methods.
 * 	[89/11/20  20:42:42  dpj]
 * 
 * Revision 2.2  89/10/30  16:31:31  dpj
 * 	First version.
 * 	[89/10/27  17:26:22  dpj]
 * 
 *
 */

#ifndef	_bytestream_ifc_h
#define	_bytestream_ifc_h

#include <us_byteio_ifc.h>
#include <stream_base_ifc.h>
#include <default_iobuf_mgr_ifc.h>

class bytestream:  public stream_base, public usByteIO {
	struct mutex		lock;
	struct condition	put_cond;	/* condition for put reqs */
	struct condition	get_cond;	/* condition for get reqs */
	unsigned int		qused;		/* used bytes in queue */
	unsigned int		qmax;		/* maximum bytes in queue */
	unsigned int		qfree;		/* free bytes in queue */
	unsigned int		q_hwm;		/* blk allocation hwm */
	io_offset_t		read_offset;	/* offset for read */
	io_offset_t		write_offset;	/* offset for write */
	io_block_t		readblk;	/* next blk for read */
	io_block_t		writeblk;	/* next blk for write */
	unsigned int		blksize;	/* standard block size */
	unsigned int		maximum_xfer;	/* max read/write size */
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(bytestream);
	bytestream(default_iobuf_mgr* =0, int =0, 
		   io_strategy_t =0, io_strategy_t =0);
	virtual ~bytestream();

REMOTE	virtual mach_error_t io_read_seq(io_mode_t, char*, unsigned int*, 
					 io_offset_t*);
REMOTE	virtual mach_error_t io_write_seq(io_mode_t, char*, unsigned int*,
					  io_offset_t*);
REMOTE	virtual mach_error_t io_get_size(io_size_t *);
	virtual mach_error_t io_getbytes_seq(io_mode_t, io_block_t*, 
					     io_count_t*, io_offset_t*);
	virtual mach_error_t io_putbytes_seq(io_mode_t, io_block_t*, 
					     io_count_t*, io_offset_t*);

	virtual mach_error_t io_set_read_strategy(io_strategy_t);
	virtual mach_error_t io_set_write_strategy(io_strategy_t);
	virtual mach_error_t io_flush_stream(void);

	/*
	 * From usByteIO
	 */
	virtual mach_error_t io_read(io_mode_t, io_offset_t, pointer_t,
				     unsigned int*);
	virtual mach_error_t io_write(io_mode_t, io_offset_t, pointer_t,
				      unsigned int*);
	virtual mach_error_t io_append(io_mode_t, pointer_t, unsigned int*);
	virtual mach_error_t io_set_size(io_size_t);
	virtual mach_error_t io_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t);
      private:
	virtual mach_error_t get_more_blocks_internal(unsigned int);
};


#endif	_bytestream_ifc_h
