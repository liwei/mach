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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_ifc.h,v $
 *
 * Purpose:  The interface for general UNIX IO objects.
 * 
 * HISTORY
 * $Log:	uxio_ifc.h,v $
 * Revision 1.9  94/07/08  16:02:03  mrt
 * 	Updated copyrights.
 * 
 * Revision 1.8  94/06/29  14:57:02  mrt
 * 	Added the recordio_obj as a local var to the uxio class.  This
 * 	was to fix the i/o probe of the datagram socket (a record dev).
 * 	[94/06/29  13:55:43  grm]
 * 
 * Revision 1.7  94/01/11  17:50:34  jms
 * 	Add "select" and "probe" logic for "real" select.
 * 	[94/01/09  19:47:53  jms]
 * 
 * Revision 1.6  91/11/06  14:12:04  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:16:04  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:10:10  pjg]
 * 
 * Revision 1.5  91/05/05  19:28:44  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:01  dpj]
 * 
 * 	Use sequential versions of I/O operations when appropriate.
 * 	Added code to close and replace underlying item.
 * 	[91/04/28  10:32:02  dpj]
 * 
 * Revision 1.4  90/12/10  09:50:03  jms
 * 	Added ux_map and ux_get_access methods.
 * 	[90/11/20  13:36:10  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:29  jms]
 * 
 * Revision 1.3  90/11/10  00:38:50  dpj
 * 	Added ux_readv and ux_writev methods.
 * 	[90/10/24  14:39:24  neves]
 * 
 * 	Added ux_modify_protection method.
 * 	Moved the ux_ioctl method to uxio_tty object.
 * 	Made instance variable obj publiclocal.
 * 	[90/10/17  12:53:54  neves]
 * 
 * Revision 1.2  89/11/28  19:12:13  dpj
 * 	Removed ux_pipe().
 * 	[89/11/20  20:58:21  dpj]
 * 
 * Revision 1.1  89/02/21  21:57:46  dorr
 * Initial revision
 * 
 * 
 */

#ifndef	_uxio_ifc_h
#define	_uxio_ifc_h

#include <us_byteio_ifc.h>
#include <us_recio_ifc.h>
#include <uxselect_ifc.h>
#include <cthreads.h>

extern "C" {
#include "uxio.h"
}

/*
 * Record of info dto describe a pending select
 */
/*
 * Info record for forking
 */
typedef struct uxio_select_info_rec {
	uxselect		*selector;
	ux_select_fd_t		select_fd;
	ux_select_type_t	select_type;
	io_mode_t		mode;
	mach_port_t		thread;
} *uxio_select_info_t;

/* max active selects at one time */
#define UXIO_MAX_SELECT_COUNT 16

class uxio: public usTop {

	io_offset_t	cur_offset;	/* current seek pos */
	file_flag_t	flags;		/* i/o flags */
	mach_port_t	dbg_port;


      protected:
	struct mutex	lock;		/* XXX used only for select for now */
	int		select_state[SEL_TYPE_MAX];

	struct uxio_select_info_rec
			active_selects[UXIO_MAX_SELECT_COUNT];

	usItem		*obj;		/* underlying object */
	usByteIO	*byteio_obj;	/* underlying I/O object (cache) */
	usRecIO		*recordio_obj;	/* One of these two will be valid. */

      public:
	DECLARE_LOCAL_MEMBERS(uxio);
	uxio();
	virtual ~uxio();
	virtual mach_error_t ux_open(usItem*, unsigned int, ns_access_t);
	virtual mach_error_t ux_read(char*, unsigned int*);
	virtual mach_error_t ux_readv(struct iovec*, int, unsigned int*);
	virtual mach_error_t ux_write(char*, unsigned int*);
	virtual mach_error_t ux_writev(struct iovec*, int, unsigned int*);
	virtual mach_error_t ux_fstat(struct stat*);
	virtual mach_error_t ux_lseek(long int*, unsigned int);
	virtual mach_error_t ux_fcntl(int, int*);
	virtual mach_error_t ux_ioctl(int, int*);
	virtual mach_error_t ux_ftruncate(unsigned int);
	virtual mach_error_t ux_modify_protection(int, int, int);
	virtual mach_error_t ux_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t);
	virtual mach_error_t ux_get_access(ns_access_t*);

	virtual mach_error_t ux_select_one(uxselect *selector,
					ux_select_fd_t select_fd,
					ux_select_type_t select_type,
					boolean_t local_only);

	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_complete();
	virtual mach_error_t clone_abort(mach_port_t);

	/* Method to do probe on uxio_obj, may wait */
	virtual mach_error_t uxio_probe_internal(int active_select_index);

      protected:
	virtual mach_error_t ux_close_internal();
	virtual mach_error_t ux_set_sequential_internal();
	virtual mach_error_t io_read_internal(int, io_offset_t, pointer_t, 
					       unsigned int*);
	virtual mach_error_t io_read_seq_internal(io_mode_t, char*, 
						  unsigned int*,io_offset_t*);
	virtual mach_error_t ns_get_attributes_internal(ns_attr_t, int*);

	/* Called by uxio_probe_internal to complete ux_signal_one */
	virtual mach_error_t ux_select_one_done(int active_select_index,
					mach_error_t probe_ret);

	/* Return routine for cthread fork to call inorder to call the probe */
	virtual cthread_fn_t uxio_fork_probe_routine_internal();

};

/*
 * Info record for select forking.
 * Arg type for "forked_probe" routine.
 * Needed when over-riding uxio_fork_probe_routine_internal.
 */
typedef struct uxio_fork_rec {
	uxio			*self_obj;
	int			active_select_index;		
} *uxio_fork_rec_t;


#endif	_uxio_ifc_h
