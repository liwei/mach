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
 * $Log:	mf_user.cc,v $
 * Revision 2.4  94/07/07  17:23:41  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:02  dpj
 * 	Converted for use as a "property" instead of a full base class.
 * 	Converted for new C++ RPC package.
 * 	[92/06/24  16:30:52  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:55:22  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:48  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:52:42  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:28:38  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:37:15  pjg]
 * 
 */

#include <mf_user_ifc.h>

#include <top_ifc.h>
#include <usint_mf_ifc.h>

mf_user::mf_user()
	:
	rem_object(0)
{}


mf_user::mf_user(usRemote* r)
	:
	rem_object(r)
{}


mach_error_t mf_user::init_upcall()
{
    	mach_error_t 		err;
	ns_access_t		_access;
	io_size_t		_obj_size;
	mach_port_t		_obj_port;

	if (rem_object == NULL) return US_NO_REMOTE_MGR;

	/*
	 * First get the info from the manager.
	 *
	 * We may not hold the lock now, because this call might be
	 * interrupted. If there is more than one instance of this
	 * call executing concurrently, the last one to return will win.
	 *
	 */

	err = rem_object->outgoing_invoke(
				mach_method_id(io_get_mf_state),
				&_access,&_obj_size,&_obj_port);
	if (err != ERR_SUCCESS) {
		mach_error("mf_user::init_upcall.io_get_mf_state()",err);
		return(US_OBJECT_NOT_STARTED);
	}

	return mf_mem_start_user(_access,_obj_size,_obj_port);
}
