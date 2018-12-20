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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxselect.cc,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxselect.cc,v $
 * Revision 2.4  94/07/08  16:02:18  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  94/05/17  14:08:45  jms
 * 	Fix state locking bug
 * 	[94/04/28  19:00:05  jms]
 * 
 * Revision 2.2  94/01/11  17:50:57  jms
 * 	Initial Version
 * 	[94/01/09  19:52:36  jms]
 * 
 */

#include <uxselect_ifc.h>
#include <ftab_ifc.h>
#include <io_error.h>
#include <mach/message.h>

extern "C" {
#include <base.h>
#include <debug.h>
#include <errno.h>
}

#include <ns_types.h>
#include <uxio.h>

#include <clone_ifc.h>

/*
 * States of a select obj
 */
#define SEL_STATE_NEW		(ux_select_state_t)0
#define SEL_STATE_WAITING	(ux_select_state_t)1
#define SEL_STATE_COLLECTING	(ux_select_state_t)2
#define SEL_STATE_COMPLETED	(ux_select_state_t)3


/*
 *	define the methods for the base file descriptor object
 */

#define BASE usTop
DEFINE_LOCAL_CLASS(uxselect)

uxselect::uxselect() : state(SEL_STATE_NEW)
{
	mutex_init(&(Local(lock)));
	condition_init(&(Local(cond)));
}

uxselect::~uxselect()
{
}

mach_error_t
uxselect::select(ftab *file_table, int nfds,
			fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
			mach_msg_timeout_t timeout, int *found_fds)
{
	int i,j,local_pass;
	boolean_t	found_locally = FALSE;
	fd_set		*fds_ref;
	kern_return_t	kr;
	mach_error_t	ret;
	uxio *io_obj;
	mach_msg_header_t	msg;
	mach_msg_return_t	msg_retval;
	int			seen_fds_cnt;

	if (SEL_STATE_NEW != state) return (US_OBJECT_DEAD);

	/* Are any set?? XXX */
	/* Ensure "no cthread fork" if only one selected XXX */

	*found_fds = 0;
	Local(count_found_fds) = 0;
	Local(n_fds) = nfds;

	/* Copy in the fds */
	signaled_fd_refs[SEL_TYPE_READ] = readfds;
	if (readfds) {
		bcopy(readfds, &(Local(selected_fds)[SEL_TYPE_READ]), sizeof(fd_set));
		FD_ZERO(readfds);
		Local(selected_any)[SEL_TYPE_READ] = TRUE;
	}
	else {
		Local(selected_any)[SEL_TYPE_READ] = FALSE;
	}
	
	signaled_fd_refs[SEL_TYPE_WRITE] = writefds;
	if (writefds) {
		bcopy(writefds, &(Local(selected_fds)[SEL_TYPE_WRITE]), sizeof(fd_set));
		FD_ZERO(writefds);
		Local(selected_any)[SEL_TYPE_WRITE] = TRUE;
	}
	else {
		Local(selected_any)[SEL_TYPE_WRITE] = FALSE;
	}
	
#if (! USE_EXCEPT_FDS)
	if (exceptfds) {
		FD_ZERO(exceptfds);
		exceptfds = NULL;
	}
#endif USE_EXCEPT_FDS
	signaled_fd_refs[SEL_TYPE_EXCEPT] = exceptfds;
	if (exceptfds) {
		bcopy(exceptfds, &(Local(selected_fds)[SEL_TYPE_EXCEPT]), sizeof(fd_set));
		FD_ZERO(exceptfds);
		Local(selected_any)[SEL_TYPE_EXCEPT] = TRUE;
	}
	else {
		Local(selected_any)[SEL_TYPE_EXCEPT] = FALSE;
	}

	/*
	 * Loop thru the descriptors, checking "locally" for previous
	 * select completes, then remote completes (cthread_forking as needed).
	 */
	for (local_pass=1;
	     ((!found_locally) && (local_pass >= 0));
	     local_pass--) {

		/* no fds given */
		if ((!local_pass) && (0 == seen_fds_cnt)) {
			/* XXX what should I return when there are no args specified? */
			return(unix_err(EBADF));
		}

		/* remote_pass: get the timeout_port if needed */
		if (timeout && (!local_pass)) {
			kr = mach_port_allocate(mach_task_self(), 
					MACH_PORT_RIGHT_RECEIVE, &Local(timeout_port));
			if (kr != KERN_SUCCESS) {
				mach_error("timer_init: port_allocate() failed ", kr);
				return(kr);
			}

			kr = mach_port_insert_right(mach_task_self(), 
				Local(timeout_port), Local(timeout_port),
				MACH_MSG_TYPE_MAKE_SEND);
			if (kr != KERN_SUCCESS) {
				mach_error("timer_init: mach_port_insert_right() failed ", kr);
				return(kr);
			}

		}

		/* Loop thru the different kinds of selects (read/write/...) */
		seen_fds_cnt = 0;
		for (i=SEL_TYPE_MIN; i<=SEL_TYPE_MAX; i++) {
			if (! selected_any[i])
				continue;

			/* Check the descriptors */
			for (j=0; ((j<FD_SETSIZE)&&(seen_fds_cnt<nfds)); j++) {
				if (! FD_ISSET(j, (&((Local(selected_fds))[i])))) {
					continue;
				}
				seen_fds_cnt++;
				ret = file_table->ftab_get_obj(j, &io_obj);
				if (ret != ERR_SUCCESS) {
					/* requested entry not in file table */
					return(ret);
				}

				ret = io_obj->ux_select_one(this, 
					(ux_select_fd_t)j, (ux_select_type_t)i, 
					local_pass);

				/* Did we find one */
				if (local_pass) {
					if (ERR_SUCCESS == ret) {
						found_locally = TRUE;
						FD_SET(j, ((Local(signaled_fd_refs))[i]));
						continue;
					}
					if (IO_WOULD_WAIT == ret) {
						continue;
					}
				}

				if (ERR_SUCCESS != ret) {
					/* did something really go wrong? */
					return(ret);
				}
			}
		}
        }

	/* Wait for something to happen if not found locally */
	if (! found_locally) {
		if (MACH_PORT_NULL == Local(timeout_port)) {
			/* No time out given */
			mutex_lock(&(Local(lock)));
			/* check that we are not already done */
			if (SEL_STATE_NEW != Local(state)) {
				ret = ERR_SUCCESS;
			}
			else {
				/* Wait on a condition */
				Local(state) = SEL_STATE_WAITING;
				while (SEL_STATE_WAITING == state) {
					ret = intr_cond_wait(&(Local(cond)), &Local(lock));
					if (ret != ERR_SUCCESS) {
						Local(state) = SEL_STATE_COMPLETED;
						mutex_unlock(&Local(lock));
						return(EXCEPT_SOFTWARE);
					}
				}
			}
		}
		else {
			/* timeout, wait on an incomming message */
			msg_retval = 
				intr_mach_msg(&msg,
					MACH_RCV_MSG|MACH_RCV_TIMEOUT,
					0, sizeof(msg),
					Local(timeout_port),
					timeout,
					MACH_PORT_NULL);

			mutex_lock(&(Local(lock)));

			if (msg_retval == MACH_RCV_INTERRUPTED) {
				Local(state) = SEL_STATE_COMPLETED;
				mutex_unlock(&(Local(lock)));
				return(EXCEPT_SOFTWARE);
			}

			if (msg_retval == MACH_RCV_TIMED_OUT) {
				if (count_found_fds) {
					msg_retval = MACH_MSG_SUCCESS;
				}
				else {
					Local(state) = SEL_STATE_COMPLETED;
					mutex_unlock(&(Local(lock)));
					return(US_TIMEOUT);
				}
			}

			if (msg_retval != MACH_MSG_SUCCESS) {
				mach_error("uxselect: timeout failed",
					msg_retval);
			}
		}
	}
	

	Local(state) = SEL_STATE_COMPLETED;
	*found_fds = count_found_fds;
	mutex_unlock(&(Local(lock)));
	return(ERR_SUCCESS);
}


boolean_t
uxselect::select_completed(void) {
	return(SEL_STATE_COMPLETED == Local(state));
}

mach_error_t
uxselect::signal(ux_select_fd_t id,ux_select_type_t select_type,
			mach_error_t probe_ret, boolean_t *signaled)
{
	mach_msg_header_t	msg;
	mach_msg_return_t	msg_retval;
	fd_set			*fds;

	*signaled = FALSE;
	mutex_lock(&(Local(lock)));

	if (SEL_STATE_COMPLETED == Local(state)) {
		mutex_unlock(&(Local(lock)));
		return(ERR_SUCCESS);
	}

	/* find the set to be marked */
	fds = (Local(signaled_fd_refs))[select_type];

	/* Die if bads select id/type */
	if ((NULL == fds) ||
	    (! FD_ISSET(id, &((Local(selected_fds))[select_type])))) {
		mutex_unlock(&(Local(lock)));
		return(US_INVALID_ARGS);
	}

	/* This one will be returned when we do return from the select */
	FD_SET(id, fds);
	Local(count_found_fds++);
	*signaled = TRUE;
	Local(state) = SEL_STATE_COLLECTING;

	mutex_unlock(&(Local(lock)));

	cthread_yield();	/* XXX Not if only one fd */
	/* signal the condition: message to timeout port if any */
	if (MACH_PORT_NULL == Local(timeout_port)) {
		condition_signal(&(Local(cond)));
	}
	else {
		/* send a dummy message to the port to wake up early */
		bzero(&msg,sizeof(msg));
		msg.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
		msg.msgh_size = sizeof(msg);
		msg.msgh_remote_port = timeout_port;
		msg.msgh_id = 0xAA;	/* lets watchit */
		msg_retval = mach_msg_send(&msg);
		if (msg_retval != MACH_MSG_SUCCESS) {
			mach_error("timer_set: msg_send failed ", msg_retval);
		}
	}
	return(ERR_SUCCESS);
}

mach_error_t
uxselect::clone_init(mach_port_t child)
{
	return(ERR_SUCCESS);
}

mach_error_t
uxselect::clone_complete()
{
	int i, refs;

	refs = refcount();

	for (i=1;i <= refs;i++) {
		mach_object_dereference(this);
	}
	return(ERR_SUCCESS);
}

mach_error_t
uxselect::clone_abort(mach_port_t child)
{
	return(ERR_SUCCESS);
}

