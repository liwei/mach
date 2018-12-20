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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_dir_base_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for directories in the pipenet server.
 *
 * HISTORY
 * $Log:	pipenet_dir_base_ifc.h,v $
 * Revision 2.5  94/07/13  17:21:35  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  94/01/11  18:11:16  jms
 * 	Add "remote_class_name" method
 * 	[94/01/10  13:29:17  jms]
 * 
 * Revision 2.3  91/11/06  14:21:38  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:54:24  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:11  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:30  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:00:01  dpj]
 * 
 */

#ifndef	_pipenet_dir_base_ifc_h
#define	_pipenet_dir_base_ifc_h

#include	<net_dir_base_ifc.h>
#include	<default_iobuf_mgr_ifc.h>

#include	<pipenet_internal.h>


class pipenet_dir_base: public net_dir_base {
	struct mutex		lock;
	unsigned int		name_seed;
      protected:
	default_iobuf_mgr	*iobuf_mgr;
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(pipenet_dir_base);
	pipenet_dir_base(ns_mgr_id_t =null_mgr_id, access_table * =0, 
			 net_info_t * =0);
	virtual ~pipenet_dir_base();

#ifdef	GXXBUG_VIRTUAL1
	virtual char* remote_class_name() const;
#endif	GXXBUG_VIRTUAL1

	virtual mach_error_t pipenet_check_localaddr(net_addr_t *localaddr);
	virtual mach_error_t pipenet_request_connection(pipenet_conninfo_t);
	virtual mach_error_t pipenet_find_peer(net_addr_t *, net_addr_t *,
					       net_endpt_base **);

	virtual mach_error_t pipenet_setup_oneside_connection(pipenet_conninfo_t);
};

#endif	_pipenet_dir_base_ifc_h
