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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxselect_ifc.h,v $
 * Purpose: Define select object to support u*x select operation.
 *
 * Author: J. Mark Stevenson
 *
 * HISTORY
 * $Log:	uxselect_ifc.h,v $
 * Revision 2.3  94/07/08  16:02:20  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  94/01/11  17:51:02  jms
 * 	Initial Version
 * 	[94/01/09  19:53:05  jms]
 * 
 */

#ifndef	_uxselect_ifc_h
#define	_uxselect_ifc_h

#include <clone_ifc.h>
#include <cthreads.h>
#include <sys/types.h>

class ftab;
/*
 * Select_Type: select for what purpose
 */
typedef int ux_select_type_t;
#define SEL_TYPE_NULL	(ux_select_type_t)0
#define SEL_TYPE_READ	(ux_select_type_t)1
#define SEL_TYPE_WRITE	(ux_select_type_t)2
#define SEL_TYPE_EXCEPT	(ux_select_type_t)3

#define SEL_TYPE_MIN		SEL_TYPE_READ
#define SEL_TYPE_MAX		SEL_TYPE_EXCEPT

/*
 * Id for a given item being selected upon
 */
typedef int ux_select_fd_t;

/* Current state of a select obj */
typedef int ux_select_state_t;


class uxselect: public usTop {
	struct mutex		lock;
	struct condition	cond;
	ux_select_state_t	state;

	int		n_fds;
	boolean_t	selected_any[SEL_TYPE_MAX];
	fd_set		selected_fds[SEL_TYPE_MAX];

	fd_set *	signaled_fd_refs[SEL_TYPE_MAX];
	int		count_found_fds;

	mach_port_t	timeout_port;
      public:
	DECLARE_LOCAL_MEMBERS(uxselect);
	uxselect();
	virtual ~uxselect();

	virtual mach_error_t select(ftab *file_table, int nfds,
			fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
			mach_msg_timeout_t timeout, int *found_fds);

	virtual boolean_t select_completed(void);

	virtual mach_error_t signal(ux_select_fd_t id, 
					ux_select_type_t select_type,
					mach_error_t probe_ret, 
					boolean_t *signaled);

        virtual mach_error_t clone_init(mach_port_t);

        virtual mach_error_t clone_abort(mach_port_t);

        virtual mach_error_t clone_complete();
};

#endif	_uxselect_ifc_h
