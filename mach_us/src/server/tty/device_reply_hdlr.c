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
 * Revision 2.4  94/07/21  16:14:32  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/03/05  15:14:33  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:37:48  jms]
 * 
 * Revision 2.2  90/10/02  11:36:58  mbj
 * 	Added #include <device/device_types.h>.
 * 	[90/10/01  15:05:28  mbj]
 * 
 * Revision 2.1.1.1  90/09/10  17:49:59  mbj
 * 	Paul Neves' pure kernel tty_server changes
 * 
 * Revision 2.5  89/12/08  20:16:38  rwd
 * 	Call cthread_wire before setting thread priority.
 * 	[89/11/01            rwd]
 * 
 * Revision 2.4  89/09/15  15:29:26  rwd
 * 	Change includes
 * 	[89/09/11            rwd]
 * 
 * Revision 2.3  89/08/31  16:29:18  rwd
 * 	Added device_read_reply_inband
 * 	[89/08/15            rwd]
 * 
 * Revision 2.2  89/08/09  14:46:05  rwd
 * 	Added copyright to file by dbg.  Added device_write_reply_inband.
 * 	[89/07/17            rwd]
 * 
 */
/*
 * Handler for device read and write replies.  Simulates an
 * interrupt handler.
 */
#include <sys/queue.h>
#include <sys/zalloc.h>

#include <mach.h>
#include <cthreads.h>

#include <uxkern/import_mach.h>
#include <device/device_types.h>
#include <uxkern/device.h>

#include <uxkern/device_reply_hdlr.h>

#ifdef MACH3_UNIX
#define io_buf_ptr_t char *
#endif MACH3_UNIX

#define MSG_SIZE_MAX	8192
#define	NREPLY_HASH	8

#define	REPLY_HASH(port)	((port) & (NREPLY_HASH-1))

struct mutex		reply_lock = MUTEX_INITIALIZER;
queue_head_t		reply_hash[NREPLY_HASH];

struct reply_entry {
	queue_chain_t	chain;
	mach_port_t		port;
	char *		object;
	kr_fn_t		read_reply;
	kr_fn_t		write_reply;
};
typedef struct reply_entry *reply_entry_t;

typedef mig_reply_header_t death_pill_t;

zone_t	reply_entry_zone;

mach_port_t	reply_port_set;

any_t	device_reply_loop();	/* forward */

enum dev_reply_select {
	DR_READ,
	DR_WRITE
};
typedef enum dev_reply_select	dr_select_t;

void
reply_hash_enter(port, object, read_reply, write_reply)
	mach_port_t		port;
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
reply_hash_lookup(port, which, object, func)
	mach_port_t		port;
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
reply_hash_remove(port)
	mach_port_t		port;
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

}

any_t
device_reply_loop(arg)
	any_t	arg;
{
	kern_return_t		rc;

	union reply_msg {
	    mach_msg_header_t	hdr;
	    char		space[MSG_SIZE_MAX];
	} reply_msg;

	/*
	 * We KNOW that none of these messages have replies...
	 */
	death_pill_t		rep_rep_msg;

	cthread_set_name(cthread_self(), "device reply handler");

	/*
	 * Wire this cthread to a kernel thread so we can
	 * up its priority
	 */
	/* cthread_wire(); */

	/*
	 * Make this thread high priority.
	 */
	set_thread_priority(mach_thread_self(), 10);

	bzero(&reply_msg.hdr, sizeof(mach_msg_header_t));
	for (;;) {
	    reply_msg.hdr.msgh_size = sizeof(reply_msg);
	    reply_msg.hdr.msgh_local_port = reply_port_set;
	    rc = mach_msg_receive(&reply_msg.hdr);
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
	mach_port_t		reply_port;
	kern_return_t	return_code;
	mach_port_t		device_port;
{
	/* NOT USED */
}

kern_return_t
device_write_reply(reply_port, return_code, bytes_written)
	mach_port_t		reply_port;
	kern_return_t	return_code;
	int		bytes_written;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_WRITE, &object, &func))
	    return(0);

	return ((*func)(object, return_code, bytes_written));
}

kern_return_t
device_write_reply_inband(reply_port, return_code, bytes_written)
	mach_port_t		reply_port;
	kern_return_t	return_code;
	int		bytes_written;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_WRITE, &object, &func))
	    return(0);

	return ((*func)(object, return_code, bytes_written));
}

kern_return_t
device_read_reply(reply_port, return_code, data, data_count)
	mach_port_t		reply_port;
	kern_return_t	return_code;
	io_buf_ptr_t	data;
	unsigned int	data_count;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_READ, &object, &func))
	    return(0);

	return ((*func)(object, return_code, data, data_count));
}

kern_return_t
device_read_reply_inband(reply_port, return_code, data, data_count)
	mach_port_t		reply_port;
	kern_return_t	return_code;
	io_buf_ptr_t	data;
	unsigned int	data_count;
{
	char		*object;
	kr_fn_t		func;

	if (!reply_hash_lookup(reply_port, DR_READ, &object, &func))
	    return(0);

	return ((*func)(object, return_code, data, data_count));
}
