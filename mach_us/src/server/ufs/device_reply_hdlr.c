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
 * $Log:	device_reply_hdlr.c,v $
 * Revision 2.4  94/07/21  11:57:41  mrt
 * 	updated copyright
 * 
 * Revision 2.3  91/12/20  17:45:17  jms
 * 	Bump size of device reply hash table.
 * 	[91/12/20  16:26:00  jms]
 * 
 * Revision 2.2  91/07/01  14:16:00  jms
 * 	Merge forward to new branch.
 * 	[91/05/29  10:58:03  roy]
 * 
 * Revision 2.1.2.1  90/11/05  18:44:18  roy
 * 	No change.
 * 
 * 
 * Revision 2.1.1.2  90/11/05  17:15:44  roy
 * 	Changed REPLY_HASH function.
 * 
 * 
 * Revision 2.1.1.1  90/10/29  15:58:47  roy
 * 	Initial revision created from Mach 3 single-server source.
 * 
 * 
 */

/*
 * Handler for device read and write replies.  
 */


#if !defined(MACH3_UNIX)

#include <mach.h>
#include <base.h>
#include <mach/mig_errors.h>
#include <device/device_types.h>
#include <device/device.h>
#include <queue.h>
#include "zalloc.h"
#include "device_reply_hdlr.h"

#define	NREPLY_HASH	128

#define	REPLY_HASH(port)	((port >> 8) & (NREPLY_HASH-1))

struct mutex		reply_lock = MUTEX_INITIALIZER;
queue_head_t		reply_hash[NREPLY_HASH];

struct reply_entry {
	queue_chain_t	chain;
	mach_port_t	port;
	char *		object;
	kr_fn_t 	read_reply;
	kr_fn_t		write_reply;
};
typedef struct reply_entry *reply_entry_t;

zone_t	reply_entry_zone;

mach_port_t	reply_port_set;

any_t	device_reply_loop();	/* forward */

/* extern void	ux_create_thread(); */

enum dev_reply_select {
	DR_READ,
	DR_WRITE
};
typedef enum dev_reply_select	dr_select_t;

void
device_callback_enter(port, object, read_reply, write_reply)
	mach_port_t	port;
	char		*object;
	kr_fn_t		read_reply, write_reply;
{
	register reply_entry_t	re;
	register queue_t	q;

	re = (reply_entry_t) zalloc(reply_entry_zone);
	re->port = port;
	re->object = object;
	re->read_reply = read_reply;
	re->write_reply = write_reply;

	mutex_lock(&reply_lock);
	q = &reply_hash[REPLY_HASH(port)];
	queue_enter(q, re, reply_entry_t, chain);

	mutex_unlock(&reply_lock);
	(void) mach_port_move_member(mach_task_self(), port, reply_port_set);
}

boolean_t
device_callback_lookup(port, which, object, func)
	mach_port_t	port;
	dr_select_t	which;
	char		**object;	/* OUT */
	kr_fn_t		*func;		/* OUT */
{
	register reply_entry_t	re;
	register queue_t	q;

	mutex_lock(&reply_lock);
	q = &reply_hash[REPLY_HASH(port)];
	for (re = (reply_entry_t)queue_first(q);
	     !queue_end(q, (queue_entry_t)re);
	     re = (reply_entry_t)queue_next(&re->chain)) {
	    if (re->port == port) {
		*object = re->object;
		*func   = (which == DR_WRITE) ? re->write_reply
					      : re->read_reply;
		mutex_unlock(&reply_lock);
		return (TRUE);
	    }
	}
	mutex_unlock(&reply_lock);
	return (FALSE);
}

void
device_callback_remove(port)
	mach_port_t	port;
{
	register reply_entry_t	re;
	register queue_t	q;

	(void) mach_port_move_member(mach_task_self(), port, MACH_PORT_NULL);

	mutex_lock(&reply_lock);
	q = &reply_hash[REPLY_HASH(port)];
	for (re = (reply_entry_t)queue_first(q);
	     !queue_end(q, (queue_entry_t)re);
	     re = (reply_entry_t)queue_next(&re->chain)) {
	    if (re->port == port) {
		queue_remove(q, re, reply_entry_t, chain);
		mutex_unlock(&reply_lock);
		zfree(reply_entry_zone, (vm_offset_t)re);
		return;
	    }
	}
	mutex_unlock(&reply_lock);
}

void device_reply_hdlr()
{
	register int	i;

	for (i = 0; i < NREPLY_HASH; i++)
	    queue_init(&reply_hash[i]);

	reply_entry_zone =
		zinit((vm_size_t)sizeof(struct reply_entry),
		      (vm_size_t)sizeof(struct reply_entry)
			* 4096,
		      vm_page_size,
		      FALSE,	/* must be wired because inode_pager
				   uses these structures */
		      "device_reply_port to device");
	(void) mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET,
				  &reply_port_set);

/* ux_create_thread(device_reply_loop); */
	cthread_detach(cthread_fork(device_reply_loop));
}

any_t
device_reply_loop(arg)
	any_t	arg;
{
	kern_return_t		rc;

	union reply_msg {
	    mach_msg_header_t	hdr;
	    char		space[8192];
	} reply_msg;

	/*
	 * We KNOW that none of these messages have replies...
	 */
	mig_reply_header_t	rep_rep_msg;

	Debug(printf("device_reply_hdlr started...\n"));

	cthread_set_name(cthread_self(), "device reply handler");

	/*
	 * Wire this cthread to a kernel thread so we can
	 * up its priority
	 */
/* cthread_wire(); */

	/*
	 * Make this thread high priority.
	 */
/* set_thread_priority(mach_thread_self(), 2); */

	for (;;) {
	    rc = mach_msg(&reply_msg.hdr, MACH_RCV_MSG,
			  0, sizeof reply_msg, reply_port_set,
			  MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	    if (rc == MACH_MSG_SUCCESS) {
		if (device_reply_server(&reply_msg.hdr,
					&rep_rep_msg.Head))
		{
		    /*
		     * None of these messages need replies
		     */
		}
	    }
	}
}

kern_return_t
device_open_reply(reply_port, return_code, device_port)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	mach_port_t	device_port;
{
	/* NOT USED */
}

kern_return_t
device_write_reply(reply_port, return_code, bytes_written)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	int		bytes_written;
{
	char		*object;
	kr_fn_t		func;

	if (!device_callback_lookup(reply_port, DR_WRITE, &object, &func))
	    return(0);

	return ((*func)(object, return_code, bytes_written));
}

kern_return_t
device_write_reply_inband(reply_port, return_code, bytes_written)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	int		bytes_written;
{
	char		*object;
	kr_fn_t		func;

	if (!device_callback_lookup(reply_port, DR_WRITE, &object, &func))
	    return(0);

	return ((*func)(object, return_code, bytes_written));
}

kern_return_t
device_read_reply(reply_port, return_code, data, data_count)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	io_buf_ptr_t	data;
	unsigned int	data_count;
{
	char		*object;
	kr_fn_t		func;

	if (!device_callback_lookup(reply_port, DR_READ, &object, &func))
	    return(0);

	return ((*func)(object, return_code, data, data_count));
}

kern_return_t
device_read_reply_inband(reply_port, return_code, data, data_count)
	mach_port_t	reply_port;
	kern_return_t	return_code;
	io_buf_ptr_t	data;
	unsigned int	data_count;
{
	char		*object;
	kr_fn_t		func;

	if (!device_callback_lookup(reply_port, DR_READ, &object, &func))
	    return(0);

	return ((*func)(object, return_code, data, data_count));
}

#endif !defined(MACH3_UNIX)
