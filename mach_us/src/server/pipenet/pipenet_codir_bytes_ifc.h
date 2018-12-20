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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_codir_bytes_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: directory for connection-oriented, byte-oriented communications
 *
 * HISTORY
 * $Log:	pipenet_codir_bytes_ifc.h,v $
 * Revision 2.5  94/07/13  17:21:08  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  92/07/05  23:34:47  dpj
 * 	Added explicit definition of remote_class_name()
 * 	under GXXBUG_VIRTUAL1.
 * 	[92/06/29  17:27:31  dpj]
 * 
 * 	Added dummy snddis_upcall() to avoid undefined virtual.
 * 	[92/04/17  16:40:53  dpj]
 * 
 * Revision 2.3  91/11/06  14:20:35  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:52:45  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:40  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:40  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:56:06  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:48:37  dpj]
 * 
 */

#ifndef	_pipenet_codir_bytes_ifc_h
#define	_pipenet_codir_bytes_ifc_h


#include	<pipenet_dir_base_ifc.h>


class pipenet_codir_bytes: public pipenet_dir_base {
      public:
	DECLARE_MEMBERS(pipenet_codir_bytes);
	pipenet_codir_bytes(ns_mgr_id_t =null_mgr_id, access_table * =0);

#ifdef	GXXBUG_VIRTUAL1
	virtual char* remote_class_name() const;
#endif	GXXBUG_VIRTUAL1

REMOTE	virtual mach_error_t ns_list_types(ns_type_t **, int *);
REMOTE	virtual mach_error_t net_create(net_addr_t *, int *, ns_prot_t, int, 
					ns_access_t, usItem **, ns_type_t *,
					net_info_t *);
	virtual mach_error_t pipenet_setup_twoside_connection(pipenet_conninfo_t);

	virtual mach_error_t pipenet_snddis_upcall(
					net_addr_t * a1,
					char * a2,
					unsigned int a3) 
				{ return _notdef(); };

};

#endif	_pipenet_codir_bytes_ifc.h_h
