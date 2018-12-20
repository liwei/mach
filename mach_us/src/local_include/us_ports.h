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
 * File:  us_ports.h
 *
 * Purpose:  Well-known names for ports to be passed down to children.
 *
 * HISTORY:
 * $Log:	us_ports.h,v $
 * Revision 1.12  94/07/08  18:43:26  mrt
 * 	Updated copyright
 * 
 * Revision 1.11  92/03/05  15:07:52  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:55:01  jms]
 * 
 * Revision 1.10  91/10/06  22:48:30  jjc
 * 	Changed NS_ROOT_PORT to CONFIG_PORT.
 * 	[91/04/25            jjc]
 * 
 * Revision 1.9  90/03/21  17:23:40  jms
 * 	Mods for useing the objectified Task Master and TM->emul signal support.
 * 	[90/03/16  17:07:03  jms]
 * 
 * Revision 1.8  90/01/02  22:19:11  dorr
 * 	get rid of AS_TOKEN_PORT.
 * 
 * Revision 1.7.2.1  90/01/02  14:24:44  dorr
 * 	get rid of AS_TOKEN_PORT
 * 
 * Revision 1.7  89/05/04  17:50:48  mbj
 * 	Merge up to U3.
 * 	[89/04/17  15:24:51  mbj]
 * 
 * Revision 1.6  89/03/21  14:31:29  mbj
 * 	Merge mbj_pgrp branch onto mainline.
 * 
 * Revision 1.5  88/10/17  08:04:39  sanzi
 * NS_ROOT_PORT should not be passed from parent to child here. 
 * 
 * Revision 1.4  88/09/22  13:25:03  dorr
 * add as_token_port.
 * 
 * Revision 1.3  88/09/01  14:05:51  dorr
 * get rid of rcsid
 * 
 * Revision 1.2  88/08/30  15:54:49  mbj
 * Add public Task Master port and names for first & last inherited ports.
 * 
 * Revision 1.1  88/08/26  17:30:06  dorr
 * Initial revision
 * 
 * Revision 1.7.2.1  90/01/02  14:24:44  dorr
 * 	get rid of AS_TOKEN_PORT
 *
 * Revision 1.7.1.1  89/12/19  16:19:52  jms
 *	New task master mods
 * 
 * Revision 1.7  89/05/04  17:50:48  mbj
 * 	Merge up to U3.
 * 	[89/04/17  15:24:51  mbj]
 * 
 * Revision 1.6.1.1  89/03/31  16:08:35  mbj
 * 	Create mbj_signal branch
 * 
 * Revision 1.5.1.1  89/03/30  17:07:29  mbj
 * 	Add TM_NOTIFICATION_PORT.
 * 
 */

#ifndef	_US_PORTS_H
#define	_US_PORTS_H

/*
 * Ports in the range FIRST_US_PORT .. LAST_US_PORT are automatically inserted
 * by parents into all children.
 */

#define FIRST_US_PORT		(mach_port_t) 101

#define	PUBLIC_AS_PORT		(mach_port_t) 101

#define LAST_US_PORT		(mach_port_t) 101

/*
 * The following ports are not inserted into children, but are needed across
 * tm_replace_task calls in order to be able to find the existing port.
 */

#define	CONFIG_PORT		(mach_port_t) 120

#endif	_US_PORTS_H
