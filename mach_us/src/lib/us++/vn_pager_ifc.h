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
 * HISTORY
 * $Log:	vn_pager_ifc.h,v $
 * Revision 2.4  94/07/07  17:26:00  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  93/01/20  17:38:48  jms
 * 	Add pager_may_cache and io_pagein deallocate logic.
 * 	[93/01/18  17:20:10  jms]
 * 
 * Revision 2.2  92/07/05  23:32:11  dpj
 * 	First working version.
 * 	[92/06/24  17:27:46  dpj]
 * 
 * Revision 2.5  90/12/21  13:53:45  jms
 * 	Added io_pagein(), io_pageout().
 * 	[90/12/15  15:10:13  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:19:51  jms]
 * 
 * Revision 2.4  89/10/30  16:33:04  dpj
 * 	Added a setup method.
 * 	Keep track of credentials.
 * 	[89/10/27  17:43:46  dpj]
 * 
 * Revision 2.3  89/06/30  18:34:48  dpj
 * 	Added internal storage for a credentials structure, to be
 * 	used for fss_* calls.
 * 	Added a credentials argument to fio_construct().
 * 	[89/06/21  22:23:42  dpj]
 * 
 * Revision 2.2  89/03/17  12:42:56  sanzi
 * 	construct --> fio_construct.
 * 	[89/03/01  10:53:35  sanzi]
 * 	
 * 	Check onto branch.
 * 	[89/02/24  17:53:53  sanzi]
 * 
 */
#ifndef	_vn_pager_ifc_h
#define	_vn_pager_ifc_h

#include	<pager_base_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<io_types.h>
#include	<fs_types.h>
}


class vn_pager: public pager_base {
	boolean_t		started;
	struct mutex		lock;
	fs_id_t			fsid;	/* !! NO REFERENCE !! */
	struct fs_cred_data	fs_cred_info;
	boolean_t		has_cred;

      public:
				vn_pager();
	virtual			~vn_pager();

	mach_error_t		vn_pager_start(fs_id_t);

	virtual mach_error_t	ns_register_agent(ns_access_t);

	virtual mach_error_t	io_pagein(
					vm_offset_t,
					vm_address_t*,
					vm_size_t*,
					boolean_t*);
	virtual mach_error_t	io_pageout(
					vm_offset_t,
					vm_address_t,
					vm_size_t*);
	virtual mach_error_t	io_get_size(io_size_t*);
	virtual mach_error_t	io_set_size(io_size_t);
};

#endif	_vn_pager_ifc_h
