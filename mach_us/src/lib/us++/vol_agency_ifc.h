/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/vol_agency_ifc.h,v $
 *
 * Purpose: Base agency for objects in the volatile name space.
 *
 * HISTORY
 * $Log:	vol_agency_ifc.h,v $
 * Revision 2.4  94/07/07  17:26:11  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:32:27  dpj
 * 	Add the ns_tmp_xxx methods to enable the tmp_prop.
 * 	ns_register_tmplink only acts on dirs now.
 * 	[92/06/24  17:01:57  jms]
 * 	No changes.
 * 	[92/05/10  01:24:17  dpj]
 * 
 * Revision 2.2  91/11/06  14:10:07  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  15:03:49  pjg]
 * 
 * Revision 2.2.3.2  91/04/14  18:45:00  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.2.3.1  90/11/14  16:13:49  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.4  91/05/05  19:28:27  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:59  dpj]
 * 
 * 	Reworked to support explicit link count, temporary and strong links. 
 * 	[91/04/28  10:26:34  dpj]
 * 
 * Revision 2.3  90/11/10  00:38:39  dpj
 * 	Replaced ns_set_attributes() with ns_set_times().
 * 	[90/11/08  22:19:30  dpj]
 * 
 * 	Declared ns_set_attributes method.
 * 	[90/10/24  15:32:30  neves]
 * 
 * Revision 2.2  89/10/30  16:37:14  dpj
 * 	First version.
 * 	[89/10/27  19:29:43  dpj]
 * 
 */

#ifndef	_vol_agency_ifc_h
#define	_vol_agency_ifc_h


#include <std_prot_ifc.h>
class dir;

class vol_agency: public std_prot {
	struct mutex lock;
	int link_count;
      public:
	DECLARE_MEMBERS(vol_agency);
	vol_agency();
	vol_agency(ns_mgr_id_t, access_table*);
//	static void init_class(usClass*);

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
REMOTE	virtual mach_error_t ns_set_times(time_value_t, time_value_t);

	virtual mach_error_t ns_register_tmplink(dir*, int);
	virtual mach_error_t ns_unregister_tmplink(int);
	virtual mach_error_t ns_reference_tmplink();
	virtual mach_error_t ns_register_stronglink();
	virtual mach_error_t ns_unregister_stronglink();

	virtual mach_error_t ns_tmp_last_link(void);	
	virtual mach_error_t ns_tmp_last_chance(void);
	virtual mach_error_t ns_tmp_cleanup_for_shutdown(void);

	virtual mach_error_t ns_check_access(ns_access_t, std_cred*); 	/* Work-around bug in g++  */
};

#endif	_vol_agency_ifc_h
