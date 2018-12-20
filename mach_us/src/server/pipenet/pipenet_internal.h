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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_internal.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Internal definitions for local "pipe-style"
 *		communication channels.
 *
 * HISTORY
 * $Log:	pipenet_internal.h,v $
 * Revision 2.5  94/07/13  17:21:44  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  92/07/05  23:35:15  dpj
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:56:12  dpj]
 * 
 * Revision 2.3  91/11/06  14:22:22  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:55:29  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:18  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:48  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:00:58  dpj]
 * 
 */

#ifndef	_pipenet_internal_h
#define	_pipenet_internal_h


extern "C" {
#include	<base.h>
#include	<cthreads.h>
#include	<dll.h>
#include	<io_types.h>
#include	<io_types2.h>
#include	<net_types.h>
}

class pipenet_connector;
class net_endpt_base;

/*
 * Pending connection requests.
 */

typedef	int	pipenet_conninfo_state_t;
#define	PIPENET_CONNINFO_INITIAL		1
#define	PIPENET_CONNINFO_LISTENING		2
#define	PIPENET_CONNINFO_ACCEPTING		3
#define	PIPENET_CONNINFO_REJECTING		4
#define	PIPENET_CONNINFO_ACCEPTED		5
#define	PIPENET_CONNINFO_REJECTED		6
#define	PIPENET_CONNINFO_DONE			7

typedef struct pipenet_conninfo {
	pipenet_conninfo_state_t	state;		/* status of request */

	/*
	 * Data used for listen/accept/reject interaction.
	 *
	 * Supplied by active side, modified by passive side,
	 * returned to active side.
	 */
	net_addr_t		*active_addr;	/* address of active end */
	net_addr_t		*passive_addr;	/* address of passive end */
	net_options_t		*options;	/* conn. service options */
	char			*in_udata;	/* udata from active end */
	int			in_udatalen;
	char			*out_udata;	/* udata from passive end */
	int			out_udatalen;

	/*
	 * Data supplied by active side, used for accept/reject.
	 */
	net_endpt_base		*active_connector;
	ns_prot_t		active_prot;
	unsigned int		active_protlen;

	/*
	 * Data supplied by passive side, used for accept/reject.
	 */
	net_endpt_base		*passive_connector;
	ns_prot_t		passive_prot;
	unsigned int		passive_protlen;

	/*
	 * Data supplied by passive side, returned to active side
	 * after accept/reject.
	 */
	net_endpt_base		*active_endpt;	/* newly-created active
						   endpoint and type */
	ns_type_t		active_type;
	net_endpt_base		*passive_endpt;	/* newly-created passive
						   endpoint and type */
	ns_type_t		passive_type;

	/*
	 * Control information in "listen" state.
	 */
	dll_chain_t		chain;		/* link in list */
	int			seqno;		/* ID to match accept/reject */
	struct condition	cond;		/* sleep point for
						   active side */
	mach_error_t		error;		/* retcode from
						   accept/reject */
} *pipenet_conninfo_t;


/*
 * Information accompanying each record.
 */

typedef struct pipenet_recinfo {
	io_recinfo_t		base;	
	net_addr_t		addr;
	net_options_t		options;
} pipenet_recinfo_t;

extern io_recinfo_ops_t		pipenet_recinfo_ops;

extern "C" {
void pipenet_conninfo_destroy(pipenet_conninfo_t);
void pipenet_recinfo_destroy(pipenet_recinfo_t *);
void pipenet_recinfo_copy(pipenet_recinfo_t *, pipenet_recinfo_t *);
int pipenet_recinfo_size(pipenet_recinfo_t *);
}

#define	pipenet_recinfo_init(_ri,_addr,_options)			\
MACRO_BEGIN								\
	io_recinfo_init(((pipenet_recinfo_t *)(_ri)),&pipenet_recinfo_ops);\
	net_addr_copy((_addr),&((pipenet_recinfo_t *)(_ri))->addr);	\
	net_options_copy((_options),&((pipenet_recinfo_t *)(_ri))->options);\
MACRO_END


#endif	_pipenet_internal_h
