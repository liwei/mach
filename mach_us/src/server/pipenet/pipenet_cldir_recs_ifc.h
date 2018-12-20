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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_cldir_recs_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: directory for connection-less, record-oriented communications
 *
 * HISTORY
 * $Log:	pipenet_cldir_recs_ifc.h,v $
 * Revision 2.5  94/07/13  17:20:47  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  92/07/05  23:34:34  dpj
 * 	Added explicit definition of remote_class_name()
 * 	under GXXBUG_VIRTUAL1.
 * 	[92/06/29  17:26:40  dpj]
 * 
 * Revision 2.3  91/11/06  14:20:10  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:51:19  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:18  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:06:57  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:54:23  dpj]
 * 
 */

#ifndef	_pipenet_cldir_recs_ifc_h
#define	_pipenet_cldir_recs_ifc_h

#include	<pipenet_dir_base_ifc.h>

class pipenet_cldir_recs: public pipenet_dir_base {
      public:
	DECLARE_MEMBERS(pipenet_cldir_recs);
	pipenet_cldir_recs(ns_mgr_id_t =null_mgr_id, access_table * =0);

#ifdef	GXXBUG_VIRTUAL1
	virtual char* remote_class_name() const;
#endif	GXXBUG_VIRTUAL1

REMOTE	virtual mach_error_t ns_list_types(ns_type_t **, int *);
REMOTE	virtual mach_error_t net_create(net_addr_t *, int *, ns_prot_t, int, 
					ns_access_t, usItem **, ns_type_t *,
					net_info_t *);

	virtual mach_error_t pipenet_setup_oneside_connection(pipenet_conninfo_t);
};

#endif	_pipenet_cldir_recs_ifc_h
