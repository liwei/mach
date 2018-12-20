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
 * File: emul_user_misc.c
 *
 * Purpose:
 *	Random data definitions needed for direct linking of user programs
 *	with the emulation library.
 *
 * HISTORY:
 * $Log:	emul_user_misc.c,v $
 * Revision 1.6  94/07/08  16:57:33  mrt
 * 	Updated copyrights.
 * 
 * Revision 1.5  92/03/05  14:56:10  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:26:30  jms]
 * 
 * Revision 1.4  91/10/06  22:27:02  jjc
 * 	Changed public_ns_port to public_config_port.
 * 	[91/06/28            jjc]
 * 
 * Revision 1.3  90/03/21  17:21:36  jms
 * 	Comment correction
 * 	[90/03/16  16:40:41  jms]
 * 
 * 	first objectified Task Master checkin
 * 	[89/12/19  16:08:17  jms]
 * 
 * Revision 1.2  89/05/04  17:25:45  mbj
 * 	Merge up to U3.
 * 	[89/04/17  15:17:05  mbj]
 * 
 * 	Add my_notification_port.
 * 	[89/03/30  16:46:15  mbj]
 * 
 * Revision 1.1.1.1  89/03/31  16:04:38  mbj
 * 	Create mbj_signal branch
 * 
 * Revision 1.1  88/09/21  10:45:37  dorr
 * Initial revision
 *
 */

#include "mach/mach_types.h"

mach_port_t	public_as_port;
mach_port_t	public_config_port;

mach_port_t  private_as_port;
