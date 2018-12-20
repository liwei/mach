/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * tty_dir_ifc.h
 *
 * Mach tty server name interface module.
 *
 * Michael B. Jones  --  22-Aug-90
 */
/*
 * HISTORY
 * $Log:	tty_dir_ifc.h,v $
 * Revision 2.5  94/07/21  16:14:53  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  92/07/05  23:36:21  dpj
 * 	Added dummy forwarding for remote_class_name() (GXXBUG_VIRTUAL1).
 * 	[92/06/24  17:43:02  dpj]
 * 
 * Revision 2.3  91/11/06  14:24:17  jms
 * 	Update to use C++.
 * 	[91/09/17  14:22:18  jms]
 * 
 * Revision 2.2  90/09/05  09:45:56  mbj
 * 	Wrote it.
 * 	[90/09/04  15:21:46  mbj]
 * 
 */

#ifndef	_tty_dir_ifc_h
#define	_tty_dir_ifc_h


#include <dir_ifc.h>
#include <tty_agency_ifc.h>

class tty_dir: public dir {
      public:
	DECLARE_MEMBERS(tty_dir);
	~tty_dir();
	tty_dir();
	tty_dir(ns_mgr_id_t, access_table*);	/* replaces setup_tty_dir */
	tty_dir(ns_mgr_id_t, mach_error_t*);	/* replaces setup_..._as_root*/

#ifdef	GXXBUG_VIRTUAL1
	virtual char* remote_class_name() const
;//				{ return dir::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1

REMOTE	virtual mach_error_t ns_create(char*, ns_type_t, ns_prot_t, int, 
				       ns_access_t, usItem**);
REMOTE	virtual mach_error_t ns_insert_entry(char *, usItem*);
REMOTE	virtual mach_error_t ns_insert_forwarding_entry(char*, ns_prot_t, int,
							usItem*, char*);
REMOTE	virtual mach_error_t ns_list_types(ns_type_t**, int *);
};

#endif	_tty_dir_ifc_h
