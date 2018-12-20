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
 * HISTORY
 * $Log:	mf_mem_ifc.h,v $
 * Revision 2.6  94/07/27  14:40:25  mrt
 * 	Added a field in the mf_mem object to hold a pointer
 * 	to the mapping's agency.
 * 	[94/07/27  14:09:49  grm]
 * 
 * Revision 2.5  94/07/07  17:23:37  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  93/01/20  17:37:57  jms
 * 	Add MF_CLIENT_EOF_WRITE_AS_APPEND logic.  This logic is used to recognize
 * 	when we believe that we are writing at the end of a file and turn it
 * 	into an "append" operation instead of a write.  The primary purpose of which
 * 	is to make log files work without true shared seek keys.
 * 	[93/01/18  16:45:07  jms]
 * 
 * Revision 2.3  92/07/05  23:27:54  dpj
 * 	Reorganized for use with new usint_mf class.
 * 	Converted for new C++ RPC package.
 * 	[92/06/24  16:28:51  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:45  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:53:28  pjg]
 * 
 * Revision 1.7.1.1  91/04/14  18:28:27  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 1.7  89/11/28  19:11:39  dpj
 * 	Replaced "is_mgr" with "do_write_through", which is
 * 	hopefully more explicit.
 * 	[89/11/20  20:53:05  dpj]
 * 
 * Revision 1.6  89/10/30  16:33:59  dpj
 * 	Complete reorganization of client-server relationship.
 * 	[89/10/27  18:58:57  dpj]
 * 
 * Revision 1.5  89/03/17  12:46:23  sanzi
 * 	Latest round of setting file size changes.
 * 	[89/03/15  17:03:03  sanzi]
 * 	
 * 	add modified flag.
 * 	[89/03/09  18:03:20  dorr]
 * 	
 * 	Remove local declaration of io_unmap().
 * 	[89/03/08  17:13:56  sanzi]
 * 	
 * 	Add clone_complete.  Add mf_get_access (one place
 * 	where sub-class access to super-class variables is 
 * 	needed).
 * 	[89/03/08  14:43:40  sanzi]
 * 	
 * 	construct --> mf_construct.
 * 	io_sync --> io_clean.
 * 	[89/03/01  10:55:23  sanzi]
 * 	
 * 	Check onto branch.
 * 	[89/02/24  17:27:38  sanzi]
 * 
 */

#ifndef	_mf_mem_ifc_h
#define	_mf_mem_ifc_h

#include	<us_item_ifc.h>

class usRemote;

extern "C" {
#include	<mach.h>
#include	<cthreads.h>
#include	<ns_types.h>
#include	<io_types.h>
}

/* Use the "write at end == append" hack XXX */
#define MF_CLIENT_EOF_WRITE_AS_APPEND 1

class mf_mem {
	struct mutex	lock;
	boolean_t	ready;
	boolean_t	is_mgr;
	boolean_t	cleaning;
	ns_access_t	access;		/* access mode of object */
	mach_port_t	obj_port;	/* backing obj port (for vm_map())*/
	io_size_t	obj_size;	/* size of object */
	boolean_t	winvalid;	/* is the window valid? */
	vm_address_t  	winaddr;	/* starting address of window */
	vm_size_t     	winsize;	/* size of window */
	io_offset_t	winstart;	/* offset in obj of start of window */
	boolean_t	modified;	/* has the file been modified */
	boolean_t	dirty;		/* is the window dirty */
	boolean_t	clone;		/* are we a clone */

#if MF_CLIENT_EOF_WRITE_AS_APPEND
	boolean_t	eof_write_as_append;
				/* should we use the eof append logic */
	io_size_t	size_guess;	/* a local guess of the file size */
#endif MF_CLIENT_EOF_WRITE_AS_APPEND

      protected:
	usItem*		my_agency;	/* The mapping's agency */

	/*
	 * Methods to be supplied by a derived class.
	 */
	virtual mach_error_t	init_upcall() =0;
	virtual usRemote*	remote_object() =0;

	mach_error_t		mf_mem_start_user(ns_access_t,
						io_size_t,mach_port_t);
	mach_error_t		mf_mem_start_mgr(io_size_t,mach_port_t);

	virtual mach_error_t io_unmap_window();
	virtual mach_error_t io_clean_prepare(boolean_t*, io_size_t*);
	virtual mach_error_t io_clean_complete();

      public:
				mf_mem();
	virtual			~mf_mem();

	virtual mach_error_t io_read(io_mode_t, io_offset_t, pointer_t,
				      unsigned int*);
	virtual mach_error_t io_write(io_mode_t, io_offset_t, pointer_t, 
				       unsigned int*);
	virtual mach_error_t io_append(io_mode_t, pointer_t, unsigned int*);
	virtual mach_error_t io_set_size(io_size_t);
	virtual mach_error_t io_get_size(io_size_t*);
	virtual mach_error_t io_map(task_t, vm_address_t*, vm_size_t,
				     vm_offset_t, boolean_t, vm_offset_t,
				     boolean_t, vm_prot_t, vm_prot_t,
				     vm_inherit_t);

	virtual mach_error_t clone_complete();

	mach_error_t	     io_get_mf_state_internal(
					io_size_t*,mach_port_t*);

      private:
	boolean_t within_window(io_offset_t, vm_size_t,vm_offset_t,vm_size_t);
	mach_error_t get_window(vm_offset_t, vm_size_t, vm_address_t *);
};


#define	mf_mem_lock()		mutex_lock(&_Local(lock));
#define	mf_mem_unlock()		mutex_unlock(&_Local(lock));

#endif	_mf_mem_ifc_h

