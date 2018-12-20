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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_endpt_base_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for network service endpoints.
 *
 * HISTORY
 * $Log:	pipenet_endpt_base_ifc.h,v $
 * Revision 2.4  94/07/13  17:21:39  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:35:08  dpj
 * 	Fixed argument types for pipenet_terminate_connection().
 * 	[92/04/17  16:55:23  dpj]
 * 
 * Revision 2.2  91/11/06  14:21:44  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:55:03  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:55  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:18  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:14:03  dpj]
 * 
 */

#ifndef	_pipenet_endpt_base_ifc_h
#define	_pipenet_endpt_base_ifc_h

#include <net_endpt_base_ifc.h>
#include <us_net_base_ifc.h>
#include <pipenet_internal.h>


class pipenet_endpt_base: public net_endpt_base {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(pipenet_endpt_base);
	pipenet_endpt_base(ns_mgr_id_t =null_mgr_id, access_table * =0);

	virtual mach_error_t pipenet_connect_upcall(pipenet_conninfo_t) =0;
	virtual mach_error_t pipenet_snddis_upcall(net_addr_t *, char *,
						   unsigned int) =0;
	virtual mach_error_t pipenet_terminate_connection(net_addr_t *,
						  net_endpt_base *) =0;
};


#endif	_pipenet_endpt_base_ifc_h
