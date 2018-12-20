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
 * ObjectClass: stream_base
 * 	General-purpose queue for undifferentiated bytes (byte-stream).
 * 
 */
 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/stream_base_ifc.h,v $
 *
 * HISTORY:
 * $Log:	stream_base_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:42  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  94/05/17  14:08:14  jms
 * 	Needed to declare virtual functions in class stream_base for 2.3.3 g++ -modh
 * 	[94/04/28  18:55:10  jms]
 * 
 * Revision 2.2.1.1  94/02/18  11:30:02  modh
 * 	Needed to declare virtual functions in class stream_base for 2.3.3 g++
 * 
 * Revision 2.2  91/11/06  13:48:20  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:56:50  pjg]
 * 
 * Revision 2.3.1.2  91/04/14  18:22:35  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.3.1.1  90/11/14  17:02:34  pjg
 * 	Initial C++ revision.
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

#ifndef	_stream_base_ifc_h
#define	_stream_base_ifc_h

#include <iobuf_user_ifc.h>
#include <default_iobuf_mgr_ifc.h>

class stream_base: public iobuf_user {
      protected:
	io_strategy_t		read_strategy;
	io_strategy_t		write_strategy;
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(stream_base);
	stream_base(default_iobuf_mgr* =0, io_strategy_t =0, io_strategy_t =0);

	virtual mach_error_t io_set_read_strategy(io_strategy_t) =0;
	virtual mach_error_t io_set_write_strategy(io_strategy_t) =0;
	virtual mach_error_t io_flush_stream(void) =0;

	virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
	virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
	virtual mach_error_t ns_set_times(time_value_t, time_value_t);
	virtual mach_error_t ns_get_protection(ns_prot_t, int*);
	virtual mach_error_t ns_set_protection(ns_prot_t, int);
	virtual mach_error_t ns_get_privileged_id(int*);
	virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);
	virtual mach_error_t ns_get_manager(ns_access_t, usItem **);
};


#endif	_stream_base_ifc_h
