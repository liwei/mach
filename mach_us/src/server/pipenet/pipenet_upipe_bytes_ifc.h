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
 * Author: Daniel P. Julin
 *
 * Purpose: Standard uni-directional, byte-level pipe object.
 *
 * HISTORY:
 * $Log:	pipenet_upipe_bytes_ifc.h,v $
 * Revision 2.5  94/07/13  17:22:05  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  94/05/17  14:10:14  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/29  13:41:44  jms]
 * 
 * Revision 2.3  91/11/06  14:23:50  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:57:18  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:35  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:09:18  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:02:59  dpj]
 * 
 */

#ifndef	_pipenet_upipe_bytes_ifc_h
#define	_pipenet_upipe_bytes_ifc_h

#include <bytestream_ifc.h>
#include <tmp_agency_ifc.h>


class pipenet_upipe_bytes: public tmp_agency, public bytestream {
	struct mutex		lock;
	int			num_readers;
	int			num_writers;
      public:
	DECLARE_MEMBERS(pipenet_upipe_bytes);

	pipenet_upipe_bytes(ns_mgr_id_t =null_mgr_id, access_table * =0,
			    default_iobuf_mgr * =0);
	virtual char* remote_class_name() const;

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int *);
REMOTE	virtual mach_error_t ns_register_agent(ns_access_t);
REMOTE	virtual mach_error_t ns_unregister_agent(ns_access_t);
};
#endif	_pipenet_upipe_bytes_ifc_h

