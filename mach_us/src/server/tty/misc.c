/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 * $Log:	misc.c,v $
 * Revision 2.4  94/07/21  16:14:43  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/03/05  15:15:16  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:46:15  jms]
 * 
 * Revision 2.2  90/10/02  11:37:25  mbj
 * 	Paul Neves' pure kernel tty_server changes
 * 	[90/09/10  17:51:48  mbj]
 * 
 * Revision 2.6  90/03/14  21:30:58  rwd
 * 	Add share_lock_solid and share_unlock_solid.
 * 	[90/02/16            rwd]
 * 	Include default_pager_object_user here.
 * 	[90/01/22            rwd]
 * 
 * Revision 2.5  89/11/29  15:31:32  af
 * 	No execute permission for mips, if one can avoid it.
 * 	[89/11/26  11:36:05  af]
 * 
 * Revision 2.4  89/09/26  10:30:27  rwd
 * 	Added Debugger
 * 	[89/09/20            rwd]
 * 
 * Revision 2.3  89/09/15  15:29:39  rwd
 * 	Make mapped time dependant on MAP_TIME
 * 	[89/09/13            rwd]
 * 	Change includes
 * 	[89/09/11            rwd]
 * 
 * Revision 2.2  89/08/09  14:46:09  rwd
 * 	Use sizeof(time_value_t) instead of NBPG since device_map and
 * 	vm_map are rounding up for us.
 * 	[89/08/09            rwd]
 * 	Fixed microtime to get time from mtime.
 * 	[89/08/08            rwd]
 * 	Added copyright to file by dbg.  Changed get_time to use mapped
 * 	time and be macro in sys/kernel.h.
 * 	[89/08/03            rwd]
 * 
 */
/*
 * Miscellaneous routines.
 */

#include <cthreads.h>
#include <uxkern/device.h>
#include <uxkern/device_utils.h>

extern mach_port_t	privileged_host_port;

void
set_thread_priority(thread, pri)
	thread_t	thread;
	int		pri;
{
#if 0
	(void) thread_set_priority(thread, privileged_host_port, pri);
#endif
}
