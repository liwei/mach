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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/net_endpt_base_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for network service endpoints.
 *
 * HISTORY
 * $Log:	net_endpt_base_ifc.h,v $
 * Revision 2.3  94/07/07  17:23:51  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:47:18  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:55:12  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:55  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:18  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:14:03  dpj]
 * 
 */

#ifndef	_net_endpt_base_ifc_h
#define	_net_endpt_base_ifc_h

#include <tmp_agency_ifc.h>


class net_endpt_base: public tmp_agency {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(net_endpt_base);
	net_endpt_base(ns_mgr_id_t =null_mgr_id, access_table * =0);
#ifdef	NOTDEF
	mach_error_t virtual ns_create_agent(ns_access_t, std_cred*, agent**);
#endif	NOTDEF
};


#endif	_net_endpt_base_ifc_h
