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
 * File:        inherit_ports.cc
 *
 * Purpose:
 *	Inherit ports across an emulated fork.
 *
 * HISTORY: 
 * $Log:	inherit_ports.cc,v $
 * Revision 2.5  94/07/08  16:57:36  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  92/07/05  23:25:49  dpj
 * 	Eliminated diag_format().
 * 	[92/05/10  00:40:17  dpj]
 * 
 * Revision 2.3  92/03/05  14:56:26  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:32:04  jms]
 * 
 * Revision 2.2  91/11/06  11:33:37  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:40:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:33:16  pjg]
 * 
 * Revision 1.4  90/03/21  17:21:50  jms
 * 	Comment changes
 * 	[90/03/16  16:50:41  jms]
 * 
 * 	Mods for new task master
 * 	[89/12/19  16:44:00  jms]
 * 
 * Revision 1.3  90/01/02  21:57:21  dorr
 * 	get rid of complexio.h
 * 
 * Revision 1.2.1.1  89/12/18  15:53:34  dorr
 * 	get rid of complexio.h
 *
 * Revision 1.2  88/11/30  23:16:46  sanzi
 * Delint. Remove dependence on dbg_XXX
 * 
 * Revision 1.1  88/09/02  02:49:37  mbj
 * Initial revision
 * 
 */

#include "emul_base.h"
extern "C" {
#include "mach/mach_types.h"
#include "us_ports.h"
}

mach_port_t	public_tm_port;		/* Public connection to Task Master */
mach_port_t	tm_task;		/* Task Master connection for this task */

mach_error_t insert_ports(task_t task)
/*
 * Insert inherited ports into a task.
 */
{
    mach_error_t err;
    mach_error_t ret = ERR_SUCCESS;
    mach_port_t port;

    for (port = FIRST_US_PORT; port <= LAST_US_PORT; port++) {
	err = mach_port_insert_right(task, port, port, 
				 MACH_MSG_TYPE_COPY_SEND);
	DEBUG0(emul_debug, (Diag, "insert_ports: insert port %d\n", port));
	if (err) {
	    ERROR((Diag,
		"insert_ports: mach_port_insert_right(%d) error %x %s\n",
		port, err, mach_error_string(err)));
	    ret = err;
	}
    }
    return ret;
}
