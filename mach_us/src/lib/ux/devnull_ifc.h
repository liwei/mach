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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/devnull_ifc.h,v $
 *
 * devnull: implementation for UNIX /dev/null
 *
 * HISTORY:
 * $Log:	devnull_ifc.h,v $
 * Revision 2.4  94/07/08  16:01:40  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  92/07/05  23:32:31  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:29:56  dpj]
 * 
 * Revision 2.2  91/12/20  17:45:00  jms
 * 	First version (DPJ)
 * 	[91/12/20  16:13:46  jms]
 * 
 */

#ifndef	_devnull_h
#define	_devnull_h

#include <clone_ifc.h>
#include <us_byteio_ifc.h>

extern "C" {
#include <io_types.h>
}

class devnull: public virtual usByteIO, public virtual usClone {
      public:
	DECLARE_LOCAL_MEMBERS(devnull);

	virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
	virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
	virtual mach_error_t ns_set_times(time_value_t, time_value_t);
	virtual mach_error_t ns_get_protection(ns_prot_t, int*);
	virtual mach_error_t ns_set_protection(ns_prot_t, int);
	virtual mach_error_t ns_get_privileged_id(int*);
	virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);
	virtual mach_error_t ns_get_manager(ns_access_t, usItem **);

	virtual mach_error_t io_read(io_mode_t, io_offset_t, pointer_t, 
				     unsigned int*);
	virtual mach_error_t io_write(io_mode_t, io_offset_t, pointer_t, 
				      unsigned int*);
	virtual mach_error_t io_append(io_mode_t, pointer_t, unsigned int*);
	virtual mach_error_t io_read_seq(io_mode_t, char*, unsigned int*,
					 io_offset_t*);
	virtual mach_error_t io_write_seq(io_mode_t, char*, unsigned int*,
					  io_offset_t*);
	virtual mach_error_t io_set_size(io_size_t);
	virtual mach_error_t io_get_size(io_size_t *);
	virtual mach_error_t io_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t);

	virtual mach_error_t clone_init(mach_port_t) { return ERR_SUCCESS; }
	virtual mach_error_t clone_abort(mach_port_t) { return ERR_SUCCESS; }
	virtual mach_error_t clone_complete() { return ERR_SUCCESS; }
};

#endif	_devnull_h
