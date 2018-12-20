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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tty/tty_agency_ifc.h,v $
 *
 * Purpose: Base class for the manager side of all I/O objects.
 *
 * HISTORY:
 * $Log:	tty_agency_ifc.h,v $
 * Revision 2.8  94/07/21  16:14:49  mrt
 * 	Updated copyright
 * 
 * Revision 2.7  94/05/17  14:10:58  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/29  13:47:29  jms]
 * 
 * Revision 2.6  94/01/11  18:11:51  jms
 * 	Add probe/select support
 * 	[94/01/10  13:47:41  jms]
 * 
 * Revision 2.5  91/11/06  14:24:10  jms
 * 	Update to us C++.
 * 	[91/09/17  14:18:59  jms]
 * 
 * Revision 2.4  91/05/05  19:33:41  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:09:26  dpj]
 * 
 * 	Derive from vol_agency instead of agency.
 * 	[91/04/28  11:05:38  dpj]
 * 
 * Revision 2.3  90/10/02  11:34:18  mbj
 * 	Change #include <vol_agency_ifc.h> -> #include <agency_ifc.h>.
 * 	[90/10/01  15:28:49  mbj]
 * 
 * Revision 2.2  90/09/05  09:45:48  mbj
 * 	Moved here from lib/us.
 * 	[90/09/04  15:20:50  mbj]
 * 
 * Revision 1.3  89/03/17  12:54:22  sanzi
 * 	Removed ns_create_agent(), which is now inherited from the
 * 	agency class.
 * 	[89/02/25  00:31:59  dpj]
 * 	
 * 	Added access_table to the local variables.
 * 	[89/02/24  18:45:02  dpj]
 * 	
 * 	First cut at reorganized TTY server.
 * 	[89/02/21  18:00:54  dpj]
 * 
 */

#ifndef	_tty_agency_ifc_h
#define	_tty_agency_ifc_h

#include <us_byteio_ifc.h>
#include <vol_agency_ifc.h>

extern "C" {
typedef mach_error_t (*mach_error_fn_t)();
}

/*
 * Class declaration.
 */
class tty_agency: public vol_agency, public usByteIO {
	int		device_number;
	mach_error_fn_t	ext_read;
	mach_error_fn_t	ext_write;
	mach_error_fn_t	ext_end;
	mach_error_fn_t	ext_open;
	mach_error_fn_t	ext_bsdctl;
	mach_error_fn_t	ext_select;

      public:
	DECLARE_MEMBERS(tty_agency);
	tty_agency();
	tty_agency(ns_mgr_id_t, access_table*, int, 
		mach_error_fn_t,	/* ext_read */
		mach_error_fn_t,	/* ext_write */
		mach_error_fn_t,	/* ext_end */
		mach_error_fn_t,	/* ext_open */
		mach_error_fn_t,	/* ext_bsd_ioctl */
		mach_error_fn_t);	/* ext_select */
	~tty_agency();
	virtual char* remote_class_name() const;

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
REMOTE	virtual mach_error_t ns_register_agent(ns_access_t);
	virtual mach_error_t ns_unregister_agent(ns_access_t);

REMOTE	virtual mach_error_t io_read(io_mode_t, io_offset_t, pointer_t, 
				     unsigned int*);
REMOTE	virtual mach_error_t io_write(io_mode_t, io_offset_t, pointer_t, 
				      unsigned int*);
REMOTE	virtual mach_error_t io_append(io_mode_t, pointer_t, unsigned int*);
REMOTE	virtual mach_error_t io_read_seq(io_mode_t, char*, unsigned int*,
					 io_offset_t*);
REMOTE	virtual mach_error_t io_write_seq(io_mode_t, char*, unsigned int*,
					  io_offset_t*);
REMOTE	virtual mach_error_t io_set_size(io_size_t);
REMOTE	virtual mach_error_t io_get_size(io_size_t *);
REMOTE	virtual mach_error_t io_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t);

REMOTE	virtual	mach_error_t tty_bsd_ioctl(unsigned long, pointer_t);
};

EXPORT_METHOD(ns_register_agent);
EXPORT_METHOD(tty_bsd_ioctl);

#endif	_tty_agency_ifc_h
