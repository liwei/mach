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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tmp_agency_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Base agency for objects in the volatile name space,
 *	that disappear from that name space when not in use.
 *
 * HISTORY
 * $Log:	tmp_agency_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:52  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:29:26  dpj
 * 	Remove the "real" temporary agency interface putting it into the new "tmp_prop"
 * 	mechinism.  Then use this new mechinism to declare tmp_agency.
 * 	[92/06/24  16:18:10  jms]
 * 
 * Revision 2.2  91/11/06  13:48:52  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:57:49  pjg]
 * 
 * Revision 2.2  91/05/05  19:27:55  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:13  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:21:47  dpj]
 * 
 */

#ifndef	_tmp_agency_ifc_h
#define	_tmp_agency_ifc_h

#include	<vol_agency_ifc.h>
#include	<tmp_prop_ifc.h>


class tmp_agency: public vol_agency {
      public:
	DECLARE_MEMBERS(tmp_agency);
	DECLARE_TMP_PROP(tmp_obj);

	tmp_agency(ns_mgr_id_t =null_mgr_id, access_table* =0);
	virtual ~tmp_agency();

	virtual mach_error_t ns_tmp_cleanup_for_shutdown(void);

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int *);

};


#endif	_tmp_agency_ifc_h
