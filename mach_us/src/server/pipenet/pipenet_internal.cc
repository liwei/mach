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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_internal.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Internal definitions for local "pipe-style"
 *		communication channels.
 *
 * HISTORY
 * $Log:	pipenet_internal.cc,v $
 * Revision 2.4  94/07/13  17:21:42  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:35:12  dpj
 * 	Added some include files.
 * 	[92/04/17  16:55:56  dpj]
 * 
 * Revision 2.2  91/11/06  14:22:18  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:55:25  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:15  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:43  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:00:42  dpj]
 * 
 *
 */

#ifndef lint
char * pipenet_internal_rcsid = "$Header: pipenet_internal.cc,v 2.4 94/07/13 17:21:42 mrt Exp $";
#endif	lint

#include	<pipenet_internal.h>

#include	<top_ifc.h>
#include	<pipenet_connector_ifc.h>
#include	<net_endpt_base_ifc.h>

/*
 * Management of connection information structures.
 */

void pipenet_conninfo_destroy(pipenet_conninfo_t conninfo)
{
	mach_object_dereference(conninfo->active_connector);
	mach_object_dereference(conninfo->passive_connector);
	mach_object_dereference(conninfo->active_endpt);
	mach_object_dereference(conninfo->passive_endpt);
}


/*
 * Management of record information structures.
 */

void pipenet_recinfo_destroy(pipenet_recinfo_t *recinfo)
{
	net_addr_destroy(&recinfo->addr);
	net_options_destroy(&recinfo->options);
}

void pipenet_recinfo_copy(pipenet_recinfo_t *from, pipenet_recinfo_t *dest)
{
	io_recinfo_destroy(dest);
	pipenet_recinfo_init(dest,&from->addr,&from->options);
}

int pipenet_recinfo_size(pipenet_recinfo_t *recinfo)
{
	return(sizeof(pipenet_recinfo_t));
}

io_recinfo_ops_t pipenet_recinfo_ops = {
	pipenet_recinfo_destroy,
	pipenet_recinfo_copy,
	pipenet_recinfo_size
};


