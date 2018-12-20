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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tsymlink_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Transparent symlink for the "volatile name space".
 *
 * HISTORY
 * $Log:	tsymlink_ifc.h,v $
 * Revision 2.3  94/07/07  17:25:02  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:49:04  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:58:08  pjg]
 * 
 * Revision 2.2  91/05/05  19:28:00  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:20  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:22:26  dpj]
 * 
 */

#ifndef	_tsymlink_ifc_h
#define	_tsymlink_ifc_h

#include	<symlink_ifc.h>


class tsymlink: public symlink {
      public:
	DECLARE_MEMBERS(tsymlink);
	tsymlink(ns_mgr_id_t =null_mgr_id, access_table* =0, 
		 ns_path_t =0, mach_error_t* =0);

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int *);
};

#endif	_tsymlink_ifc_h
