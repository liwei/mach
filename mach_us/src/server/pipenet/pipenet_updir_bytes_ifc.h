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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_updir_bytes_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: directory holding all uni-directional byte-level pipes
 *
 * HISTORY
 * $Log:	pipenet_updir_bytes_ifc.h,v $
 * Revision 2.5  94/07/13  17:22:00  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  92/07/05  23:35:29  dpj
 * 	Added explicit definition of remote_class_name()
 * 	under GXXBUG_VIRTUAL1.
 * 	[92/06/29  17:28:28  dpj]
 * 
 * Revision 2.3  91/11/06  14:23:42  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:56:57  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:31  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:09:08  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:02:25  dpj]
 * 
 */

#ifndef	_pipenet_updir_bytes_ifc_h
#define	_pipenet_updir_bytes_ifc_h

#include	<dir_ifc.h>
#include	<default_iobuf_mgr_ifc.h>


class pipenet_updir_bytes: public dir {
	default_iobuf_mgr *iobuf_mgr;
      public:
	DECLARE_MEMBERS(pipenet_updir_bytes);
	pipenet_updir_bytes(ns_mgr_id_t =null_mgr_id, access_table * =0);
	virtual ~pipenet_updir_bytes();

#ifdef	GXXBUG_VIRTUAL1
	virtual char* remote_class_name() const;
#endif	GXXBUG_VIRTUAL1

REMOTE	virtual mach_error_t ns_create(ns_name_t, ns_type_t, ns_prot_t, int,
				       ns_access_t, usItem**);
REMOTE	virtual mach_error_t ns_insert_entry(char*, usItem*);
REMOTE	virtual mach_error_t ns_insert_forwarding_entry(ns_name_t, ns_prot_t, 
							int, usItem*, char*);
REMOTE	virtual mach_error_t ns_list_types(ns_type_t**, int*);
};


#endif	_pipenet_updir_bytes_ifc_h
