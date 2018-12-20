/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_recio.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Class defining the extended record-oriented protocol for pipenet
 *	    objects.
 *
 * HISTORY
 * $Log:	pipenet_recio.cc,v $
 * Revision 2.3  94/07/13  17:21:46  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  91/11/06  14:23:13  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:55:41  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:09  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:26  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:59:41  dpj]
 * 
 */

#ifndef lint
char * pipenet_recio_rcsid = "$Header: pipenet_recio.cc,v 2.3 94/07/13 17:21:46 mrt Exp $";
#endif	lint

#include	<pipenet_recio_ifc.h>

#define BASE usRecIO
DEFINE_LOCAL_CLASS(pipenet_recio)

