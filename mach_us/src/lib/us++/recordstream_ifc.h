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
 * ObjectClass: recordstream
 * 	General-purpose queue for undifferentiated records.
 * 
 * SuperClass: iobuf_user
 * 	
 * Delegated Objects:
 * 
 * ClassMethods:
 *
 * 	Internal:
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/recordstream_ifc.h,v $
 *
 * HISTORY:
 * $Log:	recordstream_ifc.h,v $
 * Revision 2.5  94/07/07  17:24:08  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/05/17  14:07:26  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/28  18:49:10  jms]
 * 
 * Revision 2.3  92/07/05  23:28:27  dpj
 * 	Define as an abstract class instead of a concrete class.
 * 	[92/06/24  17:02:02  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:31  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:55:23  pjg]
 * 
 * Revision 2.2  91/05/05  19:27:18  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:57  dpj]
 * 
 * 	Reorganized for split between basic interface (io_methods) and
 * 	extended interface (io_methods2).
 * 	[91/04/28  10:18:02  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:30:24  dpj]
 * 
 */

#ifndef	_recordstream_ifc_h
#define	_recordstream_ifc_h

#include <us_recio_ifc.h>
#include <stream_base_ifc.h>
#include <default_iobuf_mgr_ifc.h>

//class recordstream: public virtual usRecIO, public virtual stream_base {

class recordstream: public stream_base, public usRecIO {
	struct mutex		lock;
	struct condition	put_cond;	/* condition for put reqs */
	struct condition	get_cond;	/* condition for get reqs */
	unsigned int		qused;		/* used records in queue */
	unsigned int		qmax;		/* max records in queue */
	io_recnum_t		first_recnum;	/* record number for get */
	io_recnum_t		next_recnum;	/* record number for put */
	io_record_t		first_record;
	io_record_t		last_record;
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(recordstream);
	recordstream(default_iobuf_mgr* =0, int =0, 
		     io_strategy_t =0, io_strategy_t =0);
	virtual ~recordstream();
REMOTE	virtual mach_error_t io_read1rec_seq(io_mode_t, char*, unsigned int*,
					     io_recnum_t*);
REMOTE	virtual mach_error_t io_write1rec_seq(io_mode_t, char*, unsigned int,
					      io_recnum_t*);
REMOTE	virtual mach_error_t io_get_record_count(io_recnum_t *);

	virtual mach_error_t io_read1rec_seq_with_info(io_mode_t, char *, 
					     unsigned int *, io_recnum_t *,
					     io_recinfo_t*);
	virtual mach_error_t io_write1rec_seq_with_info(io_mode_t, char *, 
					     unsigned int, io_recnum_t *,
					     io_recinfo_t*);
	virtual mach_error_t io_getrec_seq(io_mode_t, io_record_t *, 
					   unsigned int *,
					   io_recnum_t *);
	virtual mach_error_t io_putrec_seq(io_mode_t, io_record_t *, 
					   unsigned int *,
					   io_recnum_t *);

	virtual mach_error_t io_set_read_strategy(io_strategy_t);
	virtual mach_error_t io_set_write_strategy(io_strategy_t);
	virtual mach_error_t io_flush_stream(void);

	/*
	 * From usRecIO but not implemented
	 */
	virtual mach_error_t io_read1rec(io_mode_t, io_recnum_t, char*,
					 unsigned int*);
	virtual mach_error_t io_write1rec(io_mode_t, io_recnum_t, char*,
					  unsigned int);

};

#endif	_recordstream_ifc_h
