/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	machid_notify_procs.c,v $
 * Revision 2.2  94/07/13  16:37:30  mrt
 * 	copied here from the user area
 * 
 * Revision 2.5  92/11/13  17:37:23  mrt
 * 	Added code to do_mach_notify_dead_name to handle the death of
 * 	the NetMsgServer.
 * 	[92/11/06            pds]
 * 
 * Revision 2.4  91/03/27  17:26:29  mrt
 * 	Changed mach.h include
 * 
 * Revision 2.3  91/03/19  12:30:44  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.2  90/09/12  16:31:50  rpd
 * 	Created.
 * 	[90/06/18            rpd]
 * 
 */

#include <stdio.h>
#include <mach.h>
#include "machid_internal.h"

kern_return_t
do_mach_notify_port_deleted(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    quit(1, "machid: do_mach_notify_port_deleted\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_msg_accepted(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    quit(1, "machid: do_mach_notify_msg_accepted\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_port_destroyed(notify, port)
    mach_port_t notify;
    mach_port_t port;
{
    quit(1, "machid: do_mach_notify_port_destroyed\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_no_senders(notify, mscount)
    mach_port_t notify;
    mach_port_mscount_t mscount;
{
    quit(1, "machid: do_mach_notify_no_senders\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_send_once(port)
    mach_port_t port;
{
    quit(1, "machid: do_mach_notify_send_once\n");
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_dead_name(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    if (name != monitor_port)
	port_destroy(name);
    else {
#if	defined(not_yet)
#include <cthreads.h>
	/*
	 * XXX - Can't do this yet because 3.0 C-threads requires isn't
	 * backwards compatable.  When we have a libmach.a with
	 * MACH_IPC_COMPAT turned on, it should be possible to do this
	 * using the 2.5 version of libthreads.
	 */
	/*
	 * If name is the NetMsgServer's monitor port than the program needs
	 * to check in its service port again.  A separate thread is used for
	 * this so that programs that already have the service port will
	 * continue to be serviced.
	 */
	cthread_detach(cthread_fork((cthread_fn_t) do_check_in, (any_t) TRUE));
#else	/* not defined(not_yet) */
	do_check_in(TRUE);
#endif	/* defined(not_yet) */
    }

    return KERN_SUCCESS;
}
