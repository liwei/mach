/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/local_include/net_internal.h,v $
 *
 * HISTORY:
 * $Log:	net_internal.h,v $
 * Revision 2.4  94/07/08  18:43:15  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:33:13  dpj
 * 	Removed include of mach_object.h.
 * 	[92/05/10  01:26:16  dpj]
 * 
 * Revision 2.2  91/05/05  19:29:22  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:03:03  dpj]
 * 
 * 	Added net_recinfo_clts_t and support functions.
 * 	[91/04/28  10:38:43  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:36:04  dpj]
 * 
 */

#ifndef	_net_internal_h
#define	_net_internal_h

#include <base.h>
#include <io_types.h>
#include <io_types2.h>
#include <net_types.h>


/************************************************************************\
 *									*
 *		Random constants and data structures			*
 *									*
\************************************************************************/

/*
 * Maximum size for a single record.
 *
 * XXX Needed for pre-allocation with RPC interface.
 * Should be eliminated by integrating block I/O with
 * the MachObjects RPC system.
 */
#define	NET_MAX_RECSIZE		32767


/*
 * net_recinfo_clts_t: abstract data type for information associated
 *	with data records on connection-less channels.
 */
typedef struct net_recinfo_clts {
	io_recinfo_t		base;
	net_addr_t		addr;
	net_options_t		options;
} net_recinfo_clts_t;

extern int	net_recinfo_clts_flavor;
extern void	net_recinfo_clts_destroy();

#define	net_recinfo_clts_init(ri,addr,options)				\
MACRO_BEGIN								\
	io_recinfo_init(&((net_recinfo_clts_t *)(ri))->base,		\
		&net_recinfo_clts_flavor,&net_recinfo_clts_destroy);	\
	net_addr_copy((addr),&((net_recinfo_clts_t *)(ri))->addr);	\
	net_options_copy((options),&((net_recinfo_clts_t *)(ri))->options); \
MACRO_END


#endif	_net_internal_h
