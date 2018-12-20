/* 
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log:	mach_privileged_ports.h,v $
 * Revision 2.3  90/10/29  18:12:11  dpj
 * 	Added set_mach_privileged_host_port(), set_mach_device_server_port().
 * 	[90/08/15  15:01:01  dpj]
 * 
 * Revision 2.2  90/07/26  12:44:43  dpj
 * 	First version.
 * 	[90/07/24  16:53:16  dpj]
 * 
 */
/*
 * mach_privileged_ports.h
 *
 *
 * $Header: /afs/cs.cmu.edu/project/mach-2/rcs/sa_include/mach_privileged_ports.h,v 2.3 90/10/29 18:12:11 dpj Exp $
 *
 */

/*
 * Privileged ports exported by the Mach kernel.
 */


#ifndef	_MACH_PRIVILEGED_PORTS_
#define	_MACH_PRIVILEGED_PORTS_

#if	MACH3 && (! defined(MACH3_UNIX))

#include	<mach.h>
#include	<mach/kern_return.h>

extern mach_port_t	mach_privileged_host_port();
extern mach_port_t	mach_device_server_port();

extern void		set_mach_privileged_host_port();
extern void		set_mach_device_server_port();

#endif	MACH3 && (! defined(MACH3_UNIX))

#endif	_MACH_PRIVILEGED_PORTS_
