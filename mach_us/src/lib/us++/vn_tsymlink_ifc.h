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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/vn_tsymlink_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: FS-based transparent symlink
 *
 * HISTORY
 * $Log:	vn_tsymlink_ifc.h,v $
 * Revision 2.3  94/07/07  17:26:07  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:32:20  dpj
 * 	First working version.
 * 	[92/06/24  17:28:35  dpj]
 * 
 * Revision 2.2  91/05/05  19:28:19  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:52  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:25:43  dpj]
 * 
 */

#ifndef	_vn_tsymlink_ifc_h
#define	_vn_tsymlink_ifc_h

#include <vn_symlink_ifc.h>

class vn_tsymlink: public vn_symlink {
      public:
	DECLARE_MEMBERS(vn_tsymlink);
				vn_tsymlink();
				vn_tsymlink(
					fs_id_t,
					fs_access*,
					ns_mgr_id_t,
					access_table*,
					vn_mgr*);

REMOTE	virtual mach_error_t	ns_get_attributes(ns_attr_t, int*);
REMOTE	virtual mach_error_t	ns_read_forwarding_entry(usItem**, char*);

};

#endif	_vn_tsymlink_ifc_h

