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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_cots_recs_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: endpoint for local "pipe-style" connection-oriented endpoints
 *		-- record-level I/O
 *
 * HISTORY
 * $Log:	pipenet_cots_recs_ifc.h,v $
 * Revision 2.4  94/07/13  17:21:31  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  91/11/06  14:21:27  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:54:08  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:04  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:21  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:59:24  dpj]
 * 
 */

#ifndef	_pipenet_cots_recs_ifc_h
#define	_pipenet_cots_recs_ifc_h

#include	<pipenet_cots_base_ifc.h>
#include	<pipenet_recio_ifc.h>


class pipenet_cots_recs: public pipenet_cots_base, public pipenet_recio {
	net_options_t null_options;
      public:
	DECLARE_MEMBERS(pipenet_cots_recs);
	pipenet_cots_recs(ns_mgr_id_t =null_mgr_id, access_table * =0,
			  pipenet_dir_base * =0, net_addr_t * =0,
			  net_addr_t * =0, net_endpt_base * =0,
			  net_endpt_base * =0, default_iobuf_mgr * =0);
	virtual ~pipenet_cots_recs();
	virtual char* remote_class_name() const;

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int *);
REMOTE	virtual mach_error_t io_read1rec_seq(io_mode_t, char *, unsigned int *,
					     io_recnum_t *);
REMOTE	virtual mach_error_t io_write1rec_seq(io_mode_t, char *, unsigned int,
					      io_recnum_t *);
	virtual mach_error_t io_getrec_seq(io_mode_t, io_record_t *, 
					   io_count_t *, io_recnum_t *);
	virtual mach_error_t io_putrec_seq(io_mode_t, io_record_t *,
					   io_count_t *, io_recnum_t *);

	virtual mach_error_t pipenet_clts_write1rec_upcall(io_mode_t, char *,
					   unsigned int, io_recnum_t *,
					   net_addr_t *, net_options_t *);
	virtual mach_error_t pipenet_clts_put1rec_upcall(io_mode_t,io_record_t,
					   io_recnum_t *, net_addr_t *,
					   net_options_t *);

	/*
	 * From usRecIO but not implemented
	 */
	virtual mach_error_t io_read1rec(io_mode_t, io_recnum_t, char*,
					 unsigned int*);
	virtual mach_error_t io_write1rec(io_mode_t, io_recnum_t, char*,
					  unsigned int);
	virtual mach_error_t io_get_record_count(io_recnum_t*);

};

#endif	_pipenet_cots_recs_ifc_h
