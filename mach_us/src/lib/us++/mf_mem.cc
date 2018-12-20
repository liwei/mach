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
 * $Log:	mf_mem.cc,v $
 * Revision 2.8  94/07/28  13:40:38  mrt
 * 		Don't do previous time fix on user side
 * 	[94/07/27            mrt]
 * 
 * Revision 2.7  94/07/27  14:40:21  mrt
 * 	Made changes to allow io_write and io_append to update the
 * 	last modified time on the mapping's agency.
 * 	[94/07/27  14:08:32  grm]
 * 
 * Revision 2.6  94/07/07  17:23:36  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  93/01/20  17:37:54  jms
 * 	Add MF_CLIENT_EOF_WRITE_AS_APPEND logic.  This logic is used to recognize
 * 	when we believe that we are writing at the end of a file and turn it
 * 	into an "append" operation instead of a write.  The primary purpose of which
 * 	is to make log files work without true shared seek keys.
 * 
 * 	do_write_through => is_mgr.  More correct description.
 * 
 * 	Show errors when doing "cleaning" instead of just returning.
 * 	[93/01/18  16:44:43  jms]
 * 
 * Revision 2.4  92/07/05  23:27:49  dpj
 * 	Reorganized for use with usint_mf class.
 * 	Full lazy initialization.
 * 	Converted for new C++ RPC package.
 * 	[92/06/24  16:27:54  dpj]
 * 
 * 	Eliminated ambiguity with local member "lock".
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:55:03  dpj]
 * 
 * Revision 2.3  92/03/05  15:05:35  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:31:27  jms]
 * 
 * Revision 2.2  91/11/06  13:46:41  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:50:03  pjg]
 * 
 * 	Created.
 * 	[91/04/14  18:28:13  pjg]
 * 
 * Revision 1.14  90/11/27  18:20:22  jms
 * 	mach/machine/vm_param.h NOT mach/vm_param.h
 * 	[90/11/20  14:26:05  jms]
 * 
 * Revision 1.13  90/07/09  17:09:20  dorr
 * 	raise a bunch of debug output levels.
 * 	[90/03/01  14:59:32  dorr]
 * 	No Further Changes
 * 	[90/07/06  17:39:21  jms]
 * 
 * Revision 1.12  89/11/28  19:11:32  dpj
 * 	Replaced "is_mgr" with "do_write_through", which is
 * 	hopefully more explicit.
 * 	[89/11/20  20:51:02  dpj]
 * 
 * Revision 1.11  89/10/30  16:33:53  dpj
 * 	Complete reorganization of client-server relationship.
 * 	[89/10/27  18:57:47  dpj]
 * 
 * Revision 1.10  89/07/09  14:19:20  dpj
 * 	Fixed debugging statements for new DEBUG macros.
 * 	[89/07/08  12:57:23  dpj]
 * 
 * Revision 1.9  89/05/24  10:42:09  dorr
 * 	add some debugging output.
 * 
 * Revision 1.8  89/05/17  16:42:19  dorr
 * 	include file cataclysm
 * 
 * Revision 1.7  89/03/30  12:06:20  dpj
 * 	Updated to new syntax for invoke_super().
 * 	[89/03/26  18:51:34  dpj]
 * 
 * Revision 1.6  89/03/17  12:46:05  sanzi
 * 	Fix access check in io_read and io_write (the precedence
 * 	was wrong wrong wrong).
 * 	[89/03/16  17:36:41  sanzi]
 * 	
 * 	Remove check for changing file size in io_read().  It wasn't
 * 	permanent.
 * 	[89/03/16  17:21:12  sanzi]
 * 	
 * 	Latest round of setting file size changes.
 * 	[89/03/15  16:57:14  sanzi]
 * 	
 * 	turn off mf_mem debug by default.
 * 	[89/03/12  20:29:28  dorr]
 * 	
 * 	Changes to support mf_cat.
 * 	[89/03/11  16:20:38  sanzi]
 * 	
 * 	add modified flag.
 * 	[89/03/09  18:02:54  dorr]
 * 	
 * 	In io_read(), check for changing file size (Dan is going to kill me 
 * 	for this).
 * 	[89/03/09  12:25:13  sanzi]
 * 	
 * 	Add clone_complete.
 * 	No longer set size of Delegate in terminate.
 * 	[89/03/08  14:40:48  sanzi]
 * 	
 * 	Really break out of vm_map() loops on unhandable errors.
 * 	[89/03/06  20:49:11  sanzi]
 * 	
 * 	Break out of vm_map loops if error encountered.
 * 	Fixed the computation of win_size in io_read and io_write.
 * 	Quiet the debugging output from this module.
 * 	[89/03/06  14:14:38  sanzi]
 * 	
 * 	Fixed the computation of win_size in io_read and io_write.
 * 	[89/03/04  09:10:33  sanzi]
 * 	
 * 	Quiet the debugging output from this module.
 * 	[89/03/03  12:14:26  sanzi]
 * 	
 * 	don't get wanked by a bad delegate.
 * 	[89/03/02  20:46:07  dorr]
 * 	
 * 	In terminate, deallocate the window if it is valid.
 * 	[89/03/02  13:56:18  sanzi]
 * 	
 * 	Fix setting of object size in io_write().
 * 	[89/03/01  16:30:07  sanzi]
 * 	
 * 	construct --> mf_construct.
 * 	io_sync --> io_clean.
 * 	[89/03/01  10:44:49  sanzi]
 * 	
 * 	Whoops, don't iop_get_port() on Base, do it on Self.  Everyone
 * 	is happier.
 * 	[89/02/24  21:33:19  sanzi]
 * 	
 * 	Cleanup.
 * 	[89/02/24  17:25:44  sanzi]
 * 
 */

#include	<mf_mem_ifc.h>
#include	<top_ifc.h>
#include	<usint_mf_ifc.h>

extern "C" {
#include	<us_error.h>
#include	<mach/std_types.h>
#include	<mach_init.h>	/* round_page() */
#include	<mach/machine/vm_param.h>
}

/*
 * Debugging control.
 */
static int	mf_mem_debug = 1;


/*
 * Parameters
 */	
static vm_size_t 	mf_mem_default_win_size = 4096 * 64; 


/*
 * Make sure that the object is ready.
 *
 * This macro assumes that the object is locked on entry.
 */
#define	CHECK_READY() {					\
	if (! Local(ready)) {				\
		mach_error_t	ret;			\
							\
		mf_mem_unlock();			\
		ret = init_upcall();			\
		if (ret != ERR_SUCCESS) {		\
			return(ret);			\
		}					\
		mf_mem_lock();				\
	}						\
}
/* Use the "write at end == append" hack XXX */
#if MF_CLIENT_EOF_WRITE_AS_APPEND
boolean_t use_eof_write_as_append = TRUE;
#endif MF_CLIENT_EOF_WRITE_AS_APPEND
	
mf_mem::mf_mem()
	: 
	ready(FALSE),
	clone(FALSE),
	my_agency(NULL)
{
	mutex_init(&Local(lock));
}


mf_mem::~mf_mem()
{
	if (! ready) return;

	(void) io_unmap_window();

	if (Local(obj_port) != MACH_PORT_NULL) {
		(void) mach_port_deallocate(mach_task_self(), Local(obj_port));
		Local(obj_port) = MACH_PORT_NULL;
	}
}

mach_error_t mf_mem::mf_mem_start_user(
	ns_access_t		_access,
	io_size_t		_obj_size,
	mach_port_t		_obj_port)
{
	mf_mem_lock();

	if (ready) {
		mf_mem_unlock();
		return(ERR_SUCCESS);
	}

	is_mgr = FALSE;
	cleaning = FALSE;
	winvalid = FALSE;
	modified = FALSE;
	dirty = FALSE;

	Local(obj_port) = _obj_port;
	Local(obj_size) = _obj_size;
	Local(access) = _access;

	Local(ready) = TRUE;
#if MF_CLIENT_EOF_WRITE_AS_APPEND
	if (! Local(clone)) {
	    Local(size_guess) = _obj_size;
	    eof_write_as_append = TRUE;
	}
#endif MF_CLIENT_EOF_WRITE_AS_APPEND

	mf_mem_unlock();

	DEBUG2(mf_mem_debug,(0,
"mf_mem_start_user() succeeded: access=0x%x, size=%d, pager_port=0x%x\n",
		Local(access),DLONG_TO_LONG(Local(obj_size)),Local(obj_port)));

	return(ERR_SUCCESS);
}

mach_error_t mf_mem::mf_mem_start_mgr(
	io_size_t		_obj_size,
	mach_port_t		_obj_port)
{
	is_mgr = TRUE;
	cleaning = FALSE;
	access = NSR_ALL;
	obj_port = _obj_port;
	obj_size = _obj_size;
	winvalid = FALSE;
	modified = FALSE;
	dirty = FALSE;

	ready = TRUE;
	return(ERR_SUCCESS);
}


mach_error_t mf_mem::io_get_mf_state_internal(
	io_size_t*		size,			/* OUT */
	mach_port_t*		pager_port)		/* OUT */
{
	mf_mem_lock();

	CHECK_READY();

	*size = Local(obj_size);
	*pager_port = Local(obj_port);

	mf_mem_unlock();

	return(ERR_SUCCESS);
}



/*
 * Exported methods.
 */

mach_error_t mf_mem::io_read(
	io_mode_t	mode,
	io_offset_t	start,
	pointer_t	addr,
	unsigned int*	num)
{
	mach_error_t		err = ERR_SUCCESS;
	vm_size_t       	win_size;
	vm_size_t       	n;
	vm_offset_t		win_start;
	vm_offset_t		s;
	vm_address_t		win_addr = 0;
	io_size_t       	end_size;
	io_size_t       	tmp;
	boolean_t       	dealloc = FALSE;
	vm_address_t		old_winaddr = 0;
	vm_size_t		old_winsize = 0;

	mf_mem_lock();

	CHECK_READY();

	/*
	 * Check access.
	 */
	if (!(Local(access) & NSR_READ)) {
		err = US_INVALID_ACCESS;
		*num = 0;
		goto out;
	}

	/*
	 * Make sure of the size on the client.
	 */
	end_size = start;
	ADD_U_INT_TO_DLONG(&end_size, *num);
	if ((! Local(is_mgr)) &&
	    dlong_cmpGT(end_size, Local(obj_size))) {
		io_size_t		size;

		mf_mem_unlock();
		err = remote_object()->outgoing_invoke(
					mach_method_id(io_get_size),&size);
		mf_mem_lock();
		DEBUG1(mf_mem_debug,(0,
			"io_read() calling remote io_get_size(); size=%d\n",
			DLONG_TO_LONG(size)));
		if (err != ERR_SUCCESS) {
			*num = 0;
			goto out;
		}
		Local(obj_size) = size;
#if (0 && MF_CLIENT_EOF_WRITE_AS_APPEND)
		Local(size_guess) = *size;
#endif MF_CLIENT_EOF_WRITE_AS_APPEND
	}

	/*
	 * Prepare copy parameters.
	 */
	IO_OFF_TO_UINT(start, &s);
	n = *num;

	/*
	 * Verify arguments.
	 */
	if (dlong_cmpGTEQ(start, Local(obj_size))) {
		err = IO_INVALID_OFFSET;
		*num = 0;
		goto out;
	}
	if (dlong_cmpGT(end_size, Local(obj_size))) {
		if (mode & IOM_TRUNCATE) {
			DLONG_SUB(&tmp, Local(obj_size), start);
			IO_SIZE_TO_UINT(tmp, &n);
		} else {
			err = IO_INVALID_SIZE;
			*num = 0;
			goto out;
		}
	}
	if (mode & IOM_PROBE) {
		*num = n;
		goto out;
	}

	/*
	 * Check the window.
	 */
	IO_OFF_TO_UINT(Local(winstart), &win_start);
	if (Local(winvalid) &&
	    !within_window(Local(winstart), Local(winsize), s, n)) {
	    	old_winaddr = Local(winaddr);
		old_winsize = Local(winsize);
		dealloc = TRUE;
		Local(winvalid) = FALSE;
	}
	if (!Local(winvalid)) {
		win_start = trunc_page(s);
		win_size = round_page(s + n) - win_start;

		if (win_size < mf_mem_default_win_size)
			win_size = mf_mem_default_win_size;

		err = this->get_window(win_start, win_size, &win_addr);

		if (err != ERR_SUCCESS) {
			*num = 0;
			goto out;
		}

		Local(winvalid) = TRUE;
		Local(winaddr) = win_addr;
		Local(winsize) = win_size;
		UINT_TO_IO_OFF(win_start, &Local(winstart));
	}

	/*
	 * Get the data.
	 */
	bcopy(Local(winaddr) + (s - win_start), addr, n);
	*num = n;

out:
	mf_mem_unlock();
	if (dealloc)
		(void) vm_deallocate(mach_task_self(),old_winaddr, old_winsize);
		
	return (err);
}


mach_error_t mf_mem::io_write(
	io_mode_t	mode,
	io_offset_t	start,
	pointer_t	addr,
	unsigned int*	num)
{
	mach_error_t    	err = ERR_SUCCESS;
	vm_size_t       	win_size;
	vm_size_t       	n;
	vm_offset_t     	win_start;
	vm_offset_t     	s;
	vm_address_t    	win_addr = 0;
	boolean_t       	dealloc = FALSE;
	vm_address_t		old_winaddr = 0;
	vm_size_t		old_winsize = 0;
	io_size_t       	end_size;
	usItem*		     	agency;
	struct time_value	zero_time;

	mf_mem_lock();

	CHECK_READY();

	/*
	 * Check access.
	 */
	if (!(Local(access) & NSR_WRITE)) {
		err = US_INVALID_ACCESS;
		*num = 0;
		goto out;
	}

	zero_time.seconds = 0;
	zero_time.microseconds = 0;

	/*
	 * Always go remote from the client.
	 *
	 * Note: we cannot be sure of the new size, so don't update it.
	 */
	if (! Local(is_mgr)) {
		mf_mem_unlock();
#if MF_CLIENT_EOF_WRITE_AS_APPEND
		DEBUG0(mf_mem_debug,(0,"io_write() obj_size=%d, guess=%d, start=%d, num= %d\n",
				dlong_to_int(Local(obj_size)), 
				dlong_to_int(Local(size_guess)), 
				dlong_to_int(start), *num));
		if (use_eof_write_as_append &&
		    Local(eof_write_as_append) &&
		    (dlong_cmpEQ(start, Local(size_guess)))) {
			DEBUG0(mf_mem_debug,(0,"io_write(): do io_append\n"));
			return(io_append(mode, addr, num));
		}
		else {
			DEBUG0(mf_mem_debug,(0,"io_write(): dont io_append\n"));
			Local(eof_write_as_append) = FALSE;
		}	
#endif MF_CLIENT_EOF_WRITE_AS_APPEND

		err = remote_object()->outgoing_invoke(mach_method_id(io_write),
						mode, start, addr, num);
		mf_mem_lock();
		DEBUG1(mf_mem_debug,(0,"io_write() going remote\n"));
		if (err != ERR_SUCCESS) {
			*num = 0;
			goto out;
		}
		Local(modified) = TRUE;
		Local(dirty) = TRUE;

#if MF_CLIENT_EOF_WRITE_AS_APPEND
		/* 
		 * XXX Guess the new object size.  
		 */

		if (use_eof_write_as_append) {
			end_size = start;
			ADD_U_INT_TO_DLONG(&end_size, *num);
			if (dlong_cmpGT(end_size, Local(size_guess))) {
				Local(size_guess) = end_size;
				DEBUG0(mf_mem_debug,(0,"io_write() new guess=%d\n",
					dlong_to_int(Local(size_guess))));
			}
		}
#endif MF_CLIENT_EOF_WRITE_AS_APPEND
		goto out;
	}

	/*
	 * Prepare copy parameters.
	 */
	IO_OFF_TO_UINT(start, &s);
	n = *num;

	/*
	 * Verify arguments.
	 */
	if (mode & IOM_PROBE) {
		*num = n;
		goto out;
	}

	/*
	 * Check the window.
	 */
	IO_OFF_TO_UINT(Local(winstart), &win_start);
	if (Local(winvalid) &&
	    !within_window(Local(winstart), Local(winsize), s, n)) {
	    	old_winaddr = Local(winaddr);
		old_winsize = Local(winsize);
		dealloc = TRUE;
		Local(winvalid) = FALSE;
	}
	if (!Local(winvalid)) {
		win_start = trunc_page(s);
		win_size = round_page(s + n) - win_start;

		if (win_size < mf_mem_default_win_size)
			win_size = mf_mem_default_win_size;

		err = this->get_window(win_start, win_size, &win_addr);

		if (err != ERR_SUCCESS) {
			*num = 0;
			goto out;
		}

		Local(winvalid) = TRUE;
		Local(winaddr) = win_addr;
		Local(winsize) = win_size;
		UINT_TO_IO_OFF(win_start, &Local(winstart));
	}

	/*
	 * Store the data.
	 */
	bcopy(addr, Local(winaddr) + (s - win_start), n);
	Local(modified) = TRUE;
	Local(dirty) = TRUE;
	*num = n;

	/* Update the file's modified time */
	agency = Local(my_agency);
	/* zero, zero means set to current time */
	if (agency)
		agency->ns_set_times(zero_time, zero_time);
	
	/*
	 * Update the size if appropriate.
	 */
	end_size = start;
	ADD_U_INT_TO_DLONG(&end_size, n);
	if (dlong_cmpGT(end_size, Local(obj_size))) {
		Local(obj_size) = end_size;
	}

out:
	mf_mem_unlock();
	if (dealloc)
		(void) vm_deallocate(mach_task_self(),old_winaddr, old_winsize);

	return (err);

}

mach_error_t mf_mem::io_append(
	io_mode_t	mode,
	pointer_t	addr,
	unsigned int*	num)
{
	mach_error_t    	err = ERR_SUCCESS;
	vm_size_t       	win_size;
	vm_size_t       	n;
	vm_offset_t     	win_start;
	vm_offset_t     	s;
	vm_address_t    	win_addr = 0;
	boolean_t       	dealloc = FALSE;
	vm_address_t		old_winaddr = 0;
	vm_size_t		old_winsize = 0;
	usItem*			agency;
	struct time_value	zero_time;

	mf_mem_lock();

	CHECK_READY();

	/*
	 * Check access.
	 */
	if (!(Local(access) & NSR_WRITE)) {
		err = US_INVALID_ACCESS;
		*num = 0;
		goto out;
	}

	zero_time.seconds = 0;
	zero_time.microseconds = 0;

	/*
	 * Always go remote from the client.
	 *
	 * Note: we cannot be sure of the new size, so don't update it.
	 */
	if (! Local(is_mgr)) {
		mf_mem_unlock();
		err = remote_object()->outgoing_invoke(mach_method_id(io_append),
							mode, addr, num);
		mf_mem_lock();
		DEBUG1(mf_mem_debug,(0,"io_append() going remote\n"));
		if (err != ERR_SUCCESS) {
			*num = 0;
			goto out;
		}
		Local(modified) = TRUE;
		Local(dirty) = TRUE;

#if MF_CLIENT_EOF_WRITE_AS_APPEND
		/* 
		 * XXX Guess the new object size.  
		 */
		if (use_eof_write_as_append) {
			ADD_U_INT_TO_DLONG(&(Local(size_guess)), *num);
			DEBUG0(mf_mem_debug,(0,"io_append() new size_guess=%d\n",
				dlong_to_int(Local(size_guess))));
		}
#endif MF_CLIENT_EOF_WRITE_AS_APPEND
		goto out;
	}

	/*
	 * Prepare copy parameters.
	 */
	IO_OFF_TO_UINT(Local(obj_size), &s);
	n = *num;

	/*
	 * Verify arguments.
	 */
	if (mode & IOM_PROBE) {
		*num = n;
		goto out;
	}

	/*
	 * Check the window.
	 */
	IO_OFF_TO_UINT(Local(winstart), &win_start);
	if (Local(winvalid) &&
	    !within_window(Local(winstart), Local(winsize), s, n)) {
	    	old_winaddr = Local(winaddr);
		old_winsize = Local(winsize);
		dealloc = TRUE;
		Local(winvalid) = FALSE;
	}
	if (!Local(winvalid)) {
		win_start = trunc_page(s);
		win_size = round_page(s + n) - win_start;

		if (win_size < mf_mem_default_win_size)
			win_size = mf_mem_default_win_size;

		err = this->get_window(win_start, win_size, &win_addr);

		if (err != ERR_SUCCESS) {
			*num = 0;
			goto out;
		}

		Local(winvalid) = TRUE;
		Local(winaddr) = win_addr;
		Local(winsize) = win_size;
		UINT_TO_IO_OFF(win_start, &Local(winstart));
	}

	/*
	 * Store the data.
	 */
	bcopy(addr, Local(winaddr) + (s - win_start), n);
	Local(modified) = TRUE;
	Local(dirty) = TRUE;
	*num = n;

	/* Update the file's modified time */
	agency = Local(my_agency);
	/* zero, zero means set to current time */
	if (agency)
		agency->ns_set_times(zero_time, zero_time);

	/*
	 * Update the size.
	 */
	ADD_U_INT_TO_DLONG(&Local(obj_size), n);

out:
	mf_mem_unlock();
	if (dealloc)
		(void) vm_deallocate(mach_task_self(),old_winaddr, old_winsize);

	return (err);

}


mach_error_t mf_mem::io_set_size(io_size_t size)
{
	mach_error_t		err = ERR_SUCCESS;

 	DEBUG1(mf_mem_debug,(0,"io_set_size: size=%d\n",
			     DLONG_TO_LONG(size)));

	mf_mem_lock();

	CHECK_READY();

	/*
	 * Check access.
	 */
	if (!(Local(access) & NSR_WRITE)) {
		err = US_INVALID_ACCESS;
		goto out;
	}

	/*
	 * Update the size.
	 */
	if (! Local(is_mgr)) {
		mf_mem_unlock();
		err = remote_object()->outgoing_invoke(
					mach_method_id(io_set_size),size);
		mf_mem_lock();
		if (err != ERR_SUCCESS) {
			goto out;
		}

#if MF_CLIENT_EOF_WRITE_AS_APPEND
		Local(size_guess) = size;
#endif MF_CLIENT_EOF_WRITE_AS_APPEND
	}
	Local(obj_size) = size;
	Local(modified) = TRUE;
	Local(dirty) = TRUE;

out:
	mf_mem_unlock();

	return(err);
}


mach_error_t mf_mem::io_get_size(io_size_t* size)
{
	mach_error_t		err = ERR_SUCCESS;

	mf_mem_lock();

	CHECK_READY();

	/*
	 * Check access.
	 */
	if (!(Local(access) & (NSR_READ | NSR_GETATTR))) {
		err = US_INVALID_ACCESS;
		goto out;
	}

	/*
	 * Get the size.
	 */
	if (! Local(is_mgr)) {
		mf_mem_unlock();
		err = remote_object()->outgoing_invoke(
					mach_method_id(io_get_size),size);
		mf_mem_lock();
		if (err != ERR_SUCCESS) {
			goto out;
		}
		Local(obj_size) = *size;
	} else {
		*size = Local(obj_size);
	}

out:
	mf_mem_unlock();

 	DEBUG1(mf_mem_debug,(0,"io_get_size: size=%d\n",
						DLONG_TO_LONG(*size)));

	return(err);
}


mach_error_t mf_mem::io_map(
	task_t		task,
	vm_address_t*	addr,
	vm_size_t	size, 
	vm_offset_t	mask,
	boolean_t	anywhere,
	vm_offset_t	paging_offset,
	boolean_t	copy,
	vm_prot_t	cprot,
	vm_prot_t	mprot, 
	vm_inherit_t	inherit)
{
	mach_error_t 		err;

	mf_mem_lock();

	CHECK_READY();

	DEBUG1(mf_mem_debug,(0,
		"io_map.vm_map(%#x,%#x,%#x,%#x,%s,%#x,%#x,%s,%#x,%#x,%s)\n",
		task,*addr,size,mask,
		anywhere?"anywhere":"here",
		Local(obj_port),paging_offset,
		copy?"copy":"share",
		cprot,mprot,
		inherit?"inherit":"no-inherit"));

	err = vm_map(task,addr,size,mask,anywhere,Local(obj_port),
				paging_offset,copy,cprot,mprot,inherit);

out:
	mf_mem_unlock();

	return (err);
}


/*
 * Internal methods.
 */

/*
 * Destroy the internal window.
 */
mach_error_t mf_mem::io_unmap_window()
{
	mach_error_t 		err = ERR_SUCCESS;

	DEBUG2(mf_mem_debug,(0,"io_unmap_window() called\n"));

	mf_mem_lock();

	if (Local(winvalid)) {
		err = vm_deallocate(mach_task_self(),Local(winaddr),Local(winsize));
		Local(winvalid) = FALSE;
		Local(winaddr) = 0;
	}

	mf_mem_unlock();

	return (err);
}


/*
 * Freeze the object in preparation for cleaning.
 */
mach_error_t mf_mem::io_clean_prepare(boolean_t *dirty, io_size_t *size)

{
	mf_mem_lock();
	if (Local(cleaning)) {
		ERROR((Diag,"mf_mem.io_clean_complete: %s\n",
					mach_error_string(US_OBJECT_BUSY)));
		mf_mem_unlock();
		return(US_OBJECT_BUSY);
	}

	Local(cleaning) = TRUE;
	*dirty = Local(dirty);
	*size = Local(obj_size);

	return(ERR_SUCCESS);
}


/*
 * Un-freeze the object after cleaning.
 */
mach_error_t mf_mem::io_clean_complete()
{
	if (! Local(cleaning)) {
		ERROR((Diag,"mf_mem.io_clean_complete: %s\n",
					mach_error_string(US_OBJECT_BUSY)));
		return(US_OBJECT_BUSY);
	}

	Local(cleaning) = FALSE;
	Local(dirty) = FALSE;
	mf_mem_unlock();
	return(ERR_SUCCESS);
}


/*
 * Cloning operations for use on a client-side.
 */

mach_error_t mf_mem::clone_complete()
{
	mach_error_t		err;

	/*
	 * Destroy any window inherited from the parent.
	 */
	(void) io_unmap_window();

	/*
	 * Reinitialize the object, to force it to
	 * get a new state record from the manager.
	 */
	mutex_init(&Local(lock));
	Local(ready) = FALSE;
	Local(cleaning) = FALSE;
	Local(obj_port) = MACH_PORT_NULL;
	Local(winvalid) = FALSE;
	Local(modified) = FALSE;
	Local(dirty) = FALSE;
	Local(clone) = TRUE;

	return(ERR_SUCCESS);
}


/*
 * Internal utility routines.
 */
 
boolean_t 
mf_mem::within_window(io_offset_t winstart, vm_size_t wsize, 
		      vm_offset_t rstart, vm_size_t rsize)
{
	unsigned int wstart;

	IO_OFF_TO_UINT(winstart, &wstart);

	return((rstart >= wstart) && (rstart + rsize <= wstart + wsize));
}

mach_error_t 
mf_mem::get_window(vm_offset_t win_start, vm_size_t win_size, 
		    vm_address_t *win_addr)
{
	mach_error_t err;

	DEBUG2(mf_mem_debug,(0,
	"get_window.vm_map(%#x,%#x,%#x,%#x,%s,%#x,%#x,%s,%#x,%#x,%s)\n",
		mach_task_self(),*win_addr,win_size,0,
		TRUE?"anywhere":"here",
		Local(obj_port),win_start,
		FALSE?"copy":"share",
		VM_PROT_DEFAULT,VM_PROT_DEFAULT,
		VM_INHERIT_DEFAULT?"inherit":"no-inherit"));

	err = vm_map(mach_task_self(),win_addr,win_size,0,TRUE,
			Local(obj_port),win_start,FALSE,
			VM_PROT_DEFAULT,VM_PROT_DEFAULT,VM_INHERIT_DEFAULT);

	return (err);
}


