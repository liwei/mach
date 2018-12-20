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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_dir_ifc.h,v $
 *
 *
 * HISTORY
 * $Log:	uxio_dir_ifc.h,v $
 * Revision 2.4  94/07/08  16:02:01  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  91/11/06  14:11:58  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:15:58  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:09:58  pjg]
 * 
 * Revision 2.2  90/11/10  00:38:48  dpj
 * 	Subclass of uxio representing directories.
 * 	[90/10/17  12:59:41  neves]
 * 
 */

#ifndef	_uxio_pipe_ifc_h
#define	_uxio_dir_ifc_h


#include <uxio_ifc.h>


class uxio_dir: public uxio {
	ns_name_list_t		dir_list;
	unsigned int		dir_cnt;
	ns_entry_t		dir_entries;
	unsigned int		dir_entries_cnt;
	mach_port_t		dbg_port;
      public:
	DECLARE_LOCAL_MEMBERS(uxio_dir);
	uxio_dir();
	virtual ~uxio_dir();
	virtual mach_error_t ux_ftruncate(unsigned int);

      protected:
	virtual mach_error_t io_read_internal(int, io_offset_t, pointer_t, 
					      unsigned int*);
	virtual mach_error_t ns_get_attributes_internal(ns_attr_t, int*);
};

#endif	_uxio_dir_ifc_h
