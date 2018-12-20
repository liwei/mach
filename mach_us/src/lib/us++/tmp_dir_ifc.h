/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tmp_dir_ifc.h,v $
 *
 * Author: J. Mark Stevenson
 *
 * Purpose: Agency representing a temporary directory that will dissapear
 *		from a containing "dir" when all other references for it
 *		dissappear.  In other words, a "dir" with the "TMP_PROP"
 *
 * HISTORY
 * $Log:	tmp_dir_ifc.h,v $
 * Revision 2.3  94/07/07  17:24:55  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:29:32  dpj
 * 	Initial tmp_dir.  This is a directory which automatically/atomically
 * 	disappears when it nolonger has agents/stronglinks/entries just as a
 * 	tmp_agency goes away when it has not agents/stronglinks.
 * 
 * 	Implemented using the "tmp_prop" and overiding some additional "dir" methods
 * 	where needed.
 * 	[92/06/24  16:25:23  jms]
 * 
 */

#ifndef	_tmp_dir_ifc_h
#define	_tmp_dir_ifc_h

#include	<dir_ifc.h>
#include	<tmp_prop_ifc.h>

class tmp_dir: public dir {
	struct mutex		lock;

    public:
	DECLARE_MEMBERS(tmp_dir);
	DECLARE_TMP_PROP(tmp_prop_obj);
	
	virtual mach_error_t ns_tmp_last_link(void);	

	virtual mach_error_t ns_tmp_last_chance(void);

	tmp_dir(ns_mgr_id_t =null_mgr_id, access_table* =0);
	virtual ~tmp_dir();

REMOTE	virtual mach_error_t ns_remove_entry(ns_name_t);
REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int *);

};


#endif	_tmp_dir_ifc_h
