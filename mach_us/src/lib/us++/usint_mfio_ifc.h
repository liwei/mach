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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/usint_mfio_ifc.h,v $
 *
 * usint_mfio: abstract class defining the interactions between a mapped
 *	file and its proxy.
 *
 * HISTORY:
 * $Log:	usint_mfio_ifc.h,v $
 * Revision 2.3  94/07/07  17:25:42  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:31:42  dpj
 * 	First working version.
 * 	[92/06/24  17:25:10  dpj]
 * 
 */

#ifndef	_usint_mfio_h
#define	_usint_mfio_h

#include <us_item_ifc.h>

extern "C" {
#include <io_types.h>
}

class usint_mfio: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usint_mfio);

REMOTE	virtual mach_error_t io_read(io_mode_t, io_offset_t, pointer_t, 
				     unsigned int*) =0;
REMOTE	virtual mach_error_t io_write(io_mode_t, io_offset_t, pointer_t, 
				      unsigned int*) =0;
REMOTE	virtual mach_error_t io_append(io_mode_t, pointer_t, unsigned int*) =0;
REMOTE	virtual mach_error_t io_read_seq(io_mode_t, char*, unsigned int*,
					 io_offset_t*) =0;
REMOTE	virtual mach_error_t io_write_seq(io_mode_t, char*, unsigned int*,
					  io_offset_t*) =0;
REMOTE	virtual mach_error_t io_set_size(io_size_t) =0;
REMOTE	virtual mach_error_t io_get_size(io_size_t *) =0;
REMOTE	virtual mach_error_t io_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t) =0;
};

EXPORT_METHOD(io_read);
EXPORT_METHOD(io_write);
EXPORT_METHOD(io_read_seq);
EXPORT_METHOD(io_write_seq);
EXPORT_METHOD(io_append);
EXPORT_METHOD(io_set_size);
EXPORT_METHOD(io_get_size);

#endif	_usint_mfio_h
