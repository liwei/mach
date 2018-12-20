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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/rpcmgr.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Manager for RPC operations.
 *
 * HISTORY
 * $Log:	rpcmgr.cc,v $
 * Revision 2.8  94/07/15  15:17:00  mrt
 * 	Added a MACH_RCV_TMEOUT to _outgoing_invoke so that
 * 	abort messages don't hang.
 * 	[94/07/08  21:58:31  grm]
 * 
 * Revision 2.7  94/07/07  17:24:13  mrt
 * 	Updated copyright.
 * 
 * Revision 2.6  94/06/16  17:19:02  mrt
 * 	Add USSTATS stuff from DPJ.
 * 	[94/05/25  13:15:02  jms]
 * 
 * Revision 2.5  94/05/17  14:07:30  jms
 * 	Much more debugging stuff
 * 	Some misc stuff for gcc2.3.3 code purity
 * 	cast args to cthread_fork
 * 	[94/04/28  18:51:37  jms]
 * 
 * Revision 2.4  93/01/20  17:38:17  jms
 * 	Cleanup the mig reply port when something goes wrong.
 * 	Deactivated debugging code.
 * 	[93/01/18  17:04:26  jms]
 * 
 * Revision 2.3  92/07/06  16:36:09  dpj
 * 	Moved debugging msg for outgoing invokes to level 1 (was level 2).
 * 
 * Revision 2.2  92/07/05  23:28:33  dpj
 * 	Call msg_free() for incoming invokes based on complex bit in
 * 	header, not on predicted type in arg info.
 * 	[92/07/05            dpj]
 * 
 * 	Improved debug message for outgoing invokes.
 * 	[92/07/05  18:54:43  dpj]
 * 
 * 	Removed "static" keyword in global definitions.
 * 	Use pre-computed sized for input and output arguments.
 * 	[92/06/24  17:05:31  dpj]
 * 
 * 	First version.
 * 	[92/05/10  00:57:16  dpj]
 * 
 *
 */

/*#define	TIMING_GEN*/
/*#define	TIMING_0*/

#include	<rpcmgr_ifc.h>

#include	<method_info_ifc.h>
#include	<class_info_ifc.h>
#include	<debug.h>

extern	"C" {
#include	<mach/message.h>
#include	<mach/notify.h>
#include	<mach/msg_type.h>
#include	<exception_error.h>
#include	<interrupt.h>
#include	<logging.h>

extern mach_port_t	mig_get_reply_port();
extern mach_port_t	mig_dealloc_reply_port();
}

#define	MSG_SIZE_MAX	8192

#ifdef	TIMING_GEN
	static	char			timing_msgbuffer[MSG_SIZE_MAX];
	mach_msg_header_t*		timing_msg = 0;

class testclass_ifc: public virtual usRemote {
	public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(testclass_ifc);
REMOTE	virtual mach_error_t	testmethod(int*) =0;
REMOTE	virtual mach_error_t	testmethod2(testclass_ifc**) =0;
};

EXPORT_METHOD(testmethod2);
#endif	TIMING_GEN


typedef struct reply_msg_header {
	mach_msg_header_t	head;
	mach_msg_type_t		retval_type;
	mach_error_t		retval;
} * reply_mach_msg_header_t;

static mach_msg_type_t retval_type = {
	/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
	/* msgt_size = */		32,
	/* msgt_number = */		1,
	/* msgt_inline = */		TRUE,
	/* msgt_longform = */		FALSE,
	/* msgt_deallocate = */		FALSE,
	/* msgt_unused = */		FALSE,
};

static mach_msg_type_t method_string_info = {
	/* msgt_name = */		MACH_MSG_TYPE_STRING,
	/* msgt_size = */		8,
	/* msgt_number = */		0,
	/* msgt_inline = */		TRUE,
	/* msgt_longform = */		FALSE,
	/* msgt_deallocate = */		FALSE,
	/* msgt_unused = */		FALSE,
};


/*
 * Global instance.
 */
/*static*/ class rpcmgr		rpcmgr::GLOBAL_data;
/*static*/ class rpcmgr*	rpcmgr::GLOBAL = &rpcmgr::GLOBAL_data;


/*static*/ const int	rpcmgr::DATABUF_MAX_XXX = (40 * 1024);

rpcmgr::rpcmgr()
{
	mach_error_t	ret;

	ret = mach_port_allocate(mach_task_self(), 
			MACH_PORT_RIGHT_PORT_SET,
			&mach_object_port_set);
	ASSERT_RETCODE("rpcmgr::rpcmgr.mach_port_allocate",ret);
	mach_port_to_object_table = hash_init(0, 0, 64);

#if WATCH_THE_DEAD
	dead_port_to_object_table = hash_init(0, 0, 64);
#endif WATCH_THE_DEAD

	mutex_init(&port_table_lock);
	min_waiting = 0;
	max_waiting = 0;
	max_threads = 0;
	threads_waiting = 0;
	thread_count = 0;
	mutex_init(&thread_waiting_lock);

	send_timeout = 0; 
	rcv_timeout = 0;
	interrupt_timeout = 200;

	if (class_info::GLOBAL == 0)
		class_info::GLOBAL = new class_info;
	if (method_info::GLOBAL == 0)
		method_info::GLOBAL = new method_info;
}

rpcmgr::~rpcmgr()
{
	mach_error_t	ret;

	hash_free(mach_port_to_object_table);
#if WATCH_THE_DEAD
	hash_free(dead_port_to_object_table);
#endif WATCH_THE_DEAD
	ret = mach_port_deallocate(mach_task_self(),mach_object_port_set);
//	ASSERT_RETCODE("rpcmgr::~rpcmgr.mach_port_deallocate",ret);
}


/*
 * Debugging and statistics.
 */
int		mach_object_debug_level = Dbg_Level_Max;
boolean_t	mach_object_verbose = TRUE;
extern "C" void printf();
int		ustrace_rpc = 1;


boolean_t watch_external_ports = FALSE;

/*
 * External port management.
 */

mach_port_t rpcmgr::allocate_external_port()
{
	mach_error_t	ret;
	mach_port_t	t = mach_task_self();
	mach_port_t	p;
	mach_port_t	previous_port_dummy = MACH_PORT_NULL;

	ret = mach_port_allocate(t,MACH_PORT_RIGHT_RECEIVE,&p);
	DEBUG2(watch_external_ports,(0, "rpcmgr::_allocate_external_port: port=0x%x\n", p));

	ASSERT_RETCODE(
		"rpcmgr::allocate_external_port.mach_port_allocate",ret);

	LOG1(TRUE, 2003, p);

	if (p != MACH_PORT_NULL) {
		ret = mach_port_move_member(t, p, mach_object_port_set);
		ASSERT_RETCODE(
		"rpcmgr::allocate_external_port.mach_port_move_member",ret);

		ret = mach_port_request_notification(mach_task_self(),
			p, MACH_NOTIFY_NO_SENDERS, 1,
			p, MACH_MSG_TYPE_MAKE_SEND_ONCE,
			&previous_port_dummy);
		ASSERT_RETCODE(
	"rpcmgr::allocate_external_port.mach_port_request_notification",ret);

		if (previous_port_dummy != MACH_PORT_NULL) {
			ret = mach_port_deallocate(mach_task_self(),
				previous_port_dummy);
			ASSERT_RETCODE(
		"rpcmgr::allocate_external_port.mach_port_deallocate",ret);
		}
	}
	return(p);
}

void rpcmgr::deallocate_external_port(mach_port_t p)
{
	mach_error_t		ret;

#if DELAY_PORT_DEALLOCATE
#define PORTS_TO_DIE_SIZE 0x1000
	static mach_port_t		ports_to_die[PORTS_TO_DIE_SIZE];
	static int			next_port_to_die = -1;
	mach_port_t			tmp_port;

	LOG1(TRUE, 2004, p);

	if (next_port_to_die < 0) {
		bzero(&(ports_to_die[0]), PORTS_TO_DIE_SIZE*sizeof(mach_port_t));
		next_port_to_die++;
	}

	tmp_port = ports_to_die[next_port_to_die];
	
	ports_to_die[next_port_to_die] = p;
	p = tmp_port;

	next_port_to_die++;
	if (next_port_to_die >= PORTS_TO_DIE_SIZE) {
		next_port_to_die = 0;
	}
#endif DELAY_PORT_DEALLOCATE

	DEBUG2(watch_external_ports,(0, "rpcmgr::_deallocate_external_port: port=0x%x\n", p));

	if (p != MACH_PORT_NULL) {
		ret = mach_port_destroy(mach_task_self(), p);
			ASSERT_RETCODE(
		"rpcmgr::deallocate_external_port.mach_port_destroy",ret);
	}
}

int register_external_port_cnt = 0;

void rpcmgr::_register_external_port(mach_port_t key, usRemote* obj)
{
	boolean_t	ret = FALSE;

	DEBUG2(watch_external_ports,(0, "rpcmgr::_register_external_port 0: port=0x%x, obj=0x%x\n",
		key, obj));
	mutex_lock(&port_table_lock);
	LOG2(TRUE, 2005, key, obj);

	register_external_port_cnt++;
	ret = hash_enter(mach_port_to_object_table,
				(hash_key_t) key, (hash_value_t) obj);
	register_external_port_cnt++;
	DEBUG2(watch_external_ports,(0, "rpcmgr::_register_external_port 2: port=0x%x, obj=0x%x, ret=0x%x, reg_cnt=%d\n",
		key, obj, ret, register_external_port_cnt));
#if WATCH_THE_DEAD
	if (! ret) {
	    printf("reregister_external_port:obj=0x%x, size=0x%x, is_where=0x%x, class=%s\n",
			obj, sizeof(*obj), obj->is_where(), obj->is_a()->class_name());
	    printf("Suspending...\n");
	    task_suspend(mach_task_self());
	}
#endif WATCH_THE_DEAD

	mach_object_reference(obj);
	mutex_unlock(&port_table_lock);
}

void rpcmgr::_deregister_external_port(mach_port_t key)
{
	usRemote*		obj;

	DEBUG2(watch_external_ports,(0, "rpcmgr::_deregister_external_port: port=0x%x\n", key));

	mutex_lock(&port_table_lock);

	LOG2(TRUE, 2006, key, obj);

	obj = (usRemote*) hash_lookup(mach_port_to_object_table,
							(hash_key_t) key);
	if (obj != NULL) {
		(void) hash_remove(mach_port_to_object_table,
							(hash_key_t) key);

#if WATCH_THE_DEAD
	(void)hash_remove(dead_port_to_object_table, (hash_key_t)key);
	(void)hash_enter(dead_port_to_object_table,
				(hash_key_t) key, (hash_value_t) obj);
#endif WATCH_THE_DEAD

		mach_object_dereference(obj);
	}

	mutex_unlock(&port_table_lock);
}

usRemote* rpcmgr::_lookup_external_port(
	mach_port_t		key)
{
	usRemote*		obj;

	DEBUG2(watch_external_ports,(0, "rpcmgr::_lookup_external_port: port=0x%x\n", key));

	mutex_lock(&port_table_lock);

	obj = (usRemote*) hash_lookup(mach_port_to_object_table,
							(hash_key_t) key);
	if (obj != NULL) {
		mach_object_reference(obj);
	}

	mutex_unlock(&port_table_lock);
	return(obj);
}


usRemote* rpcmgr::_lookup_external_port(
	mach_port_t		key,
	mach_msg_seqno_t	seqno)
{
	usRemote*		obj;
	int			cnt;

	DEBUG2(watch_external_ports,(0, "rpcmgr::_lookup_external_port: port=0x%x, seq=%d\n", key, seqno));

	mutex_lock(&port_table_lock);
	cnt = register_external_port_cnt;

	obj = (usRemote*) hash_lookup(mach_port_to_object_table,
							(hash_key_t) key);
	if (obj == NULL) {
		mutex_unlock(&port_table_lock);
		mutex_lock(&port_table_lock);
		obj = (usRemote*) hash_lookup(mach_port_to_object_table,
							(hash_key_t) key);
		DEBUG2(watch_external_ports,(0, "rpcmgr::_lookup_external_port_seq 2: port=0x%x, obj=0x%x, reg_cnt=%d\n", key, obj, cnt));
	}

	if (obj != NULL) {
		DEBUG2(watch_external_ports,(0, "rpcmgr::_lookup_external_port_seq 3: port=0x%x, obj=0x%x, reg_cnt=%d\n", key, obj, cnt));
		obj->_reference_with_seqno(seqno);
	}

	DEBUG2(watch_external_ports,(0, "rpcmgr::_lookup_external_port_seq 99: port=0x%x, obj=0x%x\n", key, obj));
	mutex_unlock(&port_table_lock);
	return(obj);
}

#if WATCH_THE_DEAD
usRemote* rpcmgr::_lookup_dead_external_port(
	mach_port_t		key)
{
	usRemote*		obj;

	mutex_lock(&port_table_lock);

	obj = (usRemote*) hash_lookup(dead_port_to_object_table,
							(hash_key_t) key);
	if (obj != NULL) {
		mach_object_reference(obj);
	}

	mutex_unlock(&port_table_lock);
	return(obj);
}

#endif WATCH_THE_DEAD

/*
 * Interface routine to start a handler thread from C-threads.
 */
void _c_object_handler(rpcmgr* obj)
{
	obj->_object_handler();
}


/*
 * adjust_threads: 
 *	Adjust (create/destroy) threads in the server loop.
 *	Maintains the correct numbers of waiting/total threads servicing
 *	user requests.
 *
 *	Generally called when a thread will soon wait or has just waited.
 *
 *	Note: A newly created thread is already marked as "waiting" and
 *	should not be "adjusted" again before it actually does wait.
 *
 *	Args:
 *		"waiting_adjust" values:
 *			 1: about to wait (receive) (if needed)
 *			-1: just waited
 *			 0: already counted as waiting (new thread).
 *
 *		"exit_ok" :	If thread un-needed, suicide
 *		return :	Thread counted as "waiting" and should do so.
 *
 */
boolean_t rpcmgr::_adjust_threads(int waiting_adjust, boolean_t exit_ok)
{

	/*
	 * Do we need more or fewer threads?
	 */
	mutex_lock(&thread_waiting_lock);
	threads_waiting += waiting_adjust;

	if ((threads_waiting > max_waiting) &&
	    (waiting_adjust >= 0)) {
		/* Too many waiters and we are one of them. */

		threads_waiting--;

		if (exit_ok) {
			/* Nothing to do, go home */
			thread_count--;
			mutex_unlock(&thread_waiting_lock);

			intr_destroy_self();
			cthread_exit(0);
		}
		mutex_unlock(&thread_waiting_lock);
		return(FALSE);
	}

	if ((threads_waiting < min_waiting) &&
	    ((thread_count < max_threads) ||
	     (0 == max_threads))) {
		/* Too few waiters and not too many threads */

		thread_count++;
		threads_waiting++;
		mutex_unlock(&thread_waiting_lock);
		cthread_detach(cthread_fork((cthread_fn_t)_c_object_handler,this));
	} 
	else {
		mutex_unlock(&thread_waiting_lock);
	}
	return(waiting_adjust >= 0);
}

void rpcmgr::_object_handler()
{
	usRemote*		obj;
	mach_error_t		ret;
	int			base_msg_options = MACH_MSG_OPTION_NONE;
	int			msg_options;
	char			msgbuffer1[MSG_SIZE_MAX];
	char			msgbuffer2[MSG_SIZE_MAX];
	mach_msg_header_t 	* req_msg = (mach_msg_header_t *) msgbuffer1;
	mach_msg_header_t 	* rep_msg = (mach_msg_header_t *) msgbuffer2;
	mach_msg_header_t	* tmp_msg;
	char			data[DATABUF_MAX_XXX];
	char			*data_out = data;		
	boolean_t		send_reply, rcv_request;
	boolean_t		sending_failure_notification;
	int			wait_cnt;
	int			timeout;

	base_msg_options |= send_timeout ? 
				MACH_SEND_TIMEOUT : MACH_MSG_OPTION_NONE;
	base_msg_options |= rcv_timeout ? 
				MACH_RCV_TIMEOUT : MACH_MSG_OPTION_NONE;
	send_reply = FALSE;
	wait_cnt = 0;	/* New thread is already in the "threads_waiting" */

	for (;;) {
#ifdef	TIMING_6
		if (timing_msg != 0) {
			bcopy(timing_msg,rep_msg,timing_msg->msgh_size);

			rep_msg->msgh_bits |= MACH_MSGH_BITS(
				    MACH_MSGH_BITS_REMOTE(req_msg->msgh_bits),
				    0);
			rep_msg->msgh_remote_port = req_msg->msgh_remote_port;
			rep_msg->msgh_local_port = MACH_PORT_NULL;

			ret = mach_msg(rep_msg,
				(MACH_RCV_MSG | MACH_SEND_MSG),
				rep_msg->msgh_size,
				MSG_SIZE_MAX,
				mach_object_port_set,
				0,
				MACH_PORT_NULL);
			if (ret != ERR_SUCCESS) mach_error("mach_msg",ret);

			/* Since we received into the reply message, swappem */
			tmp_msg = req_msg;
			req_msg = rep_msg;
			rep_msg = tmp_msg;

			continue;
		}
#endif	TIMING_6
		/* 
		 * Adjust threads: Soon to wait/receive and may kill current
		 * thread iff superfluous and not sending.
		 */
		rcv_request = _adjust_threads(wait_cnt,(! send_reply));
		wait_cnt = 1;	/* No longer a new thread */

		/*
		 * Send reply and Wait for request needed.
		 */
		timeout = 0;
		msg_options = base_msg_options;

		if (rcv_request) {
			if (send_reply) {
				/* We need to reply and then receive */
				msg_options |= (MACH_RCV_MSG | MACH_SEND_MSG);
				timeout = (send_timeout > rcv_timeout) ?
					send_timeout : rcv_timeout;
				rep_msg->msgh_bits |= MACH_MSGH_BITS(
				    MACH_MSGH_BITS_REMOTE(req_msg->msgh_bits),
				    0);
				rep_msg->msgh_remote_port = 
					req_msg->msgh_remote_port;
				rep_msg->msgh_local_port = MACH_PORT_NULL;
			}
			else {
				/* Just receive the next one */
				msg_options |= MACH_RCV_MSG;
				timeout = rcv_timeout;
				rep_msg->msgh_size = 0;
				rep_msg->msgh_local_port = MACH_PORT_NULL;
			}
		}
		else {
			/* Just send the reply */
			msg_options |= MACH_SEND_MSG;
			timeout = send_timeout;
			rep_msg->msgh_bits |= MACH_MSGH_BITS(
				MACH_MSGH_BITS_REMOTE(req_msg->msgh_bits),0);
			rep_msg->msgh_remote_port = req_msg->msgh_remote_port;
			rep_msg->msgh_local_port = MACH_PORT_NULL;
		}

		ret = mach_msg(rep_msg, msg_options,
				rep_msg->msgh_size,
				MSG_SIZE_MAX,
				mach_object_port_set,
				timeout,
				MACH_PORT_NULL);

		/* This thread no longer waiting, so adjust the threads */
		if (rcv_request) {
			_adjust_threads(-1, FALSE);
		}

		/* reset the send_reply for msg cycle (no reply till we get one) */
		send_reply = FALSE;

		/*
		 * Cope with failure
		 */
		if (ret != MACH_MSG_SUCCESS) {
			/* If it's a invalid_dest to a dead port then ignore */
			if (MACH_SEND_INVALID_DEST == ret) {
				/*
				 * XXX Should clean-up the outgoing msg:
				 *	- send-once right for the reply port
				 *	- all msg elems. with dealloc bit set
				 *	- references to objects in msg
				 *	- make_send port rights to get
				 *	  the mscount right.
				 */
				mach_port_type_t	port_type;
				mach_error_t		mpt_ret;
				mpt_ret = mach_port_type(mach_task_self(), 
						rep_msg->msgh_remote_port,
						&port_type);
				if ((MACH_PORT_TYPE_DEAD_NAME == port_type) &&
					(! mpt_ret)) {
					mach_port_deallocate(mach_task_self(),
						rep_msg->msgh_remote_port);
					continue;
				}
			}

			if (mach_object_verbose)
				printf("rpcmgr: mach_msg error: 0x%x\n", ret);
			
			/* Do we have a send error */
			if ((err_get_system(ret) == err_mach_ipc) &&
			    (err_get_sub(ret) == 0)) {
				if (sending_failure_notification) {
					/* We tried anyhow, lets give up */
					sending_failure_notification = FALSE;
					continue;
				}
				/* we have a send error, notify the user */
				sending_failure_notification = TRUE;
			} 
			else if ((err_get_system(ret) == err_mach_ipc) &&
				   (err_get_sub(ret) == 1)) {
				/*
				 * Failed to receive, oh well.
				 *
				 * Must bump the seqnos if appropriate.
				 */
				obj = (usRemote*) _lookup_external_port(
						rep_msg->msgh_local_port,
						rep_msg->msgh_seqno);
				mach_object_dereference(obj);

				continue;
			}
			else {
				/* Some other error, assume the send worked XXX */
				continue;
			}
		}
		else {
			/* mach_msg was successful */
			sending_failure_notification = FALSE;

			if (! rcv_request) {
				/* Give this thread another chance */
				continue;
			}

			/* Since we received into the reply message, swappem */
			tmp_msg = req_msg;
			req_msg = rep_msg;
			rep_msg = tmp_msg;
		}

#ifdef	TIMING_5
		if (timing_msg != 0) goto end_timing_5;
#endif	TIMING_5

		/* find the agent */
		obj = (usRemote*) _lookup_external_port(
						req_msg->msgh_local_port,
						req_msg->msgh_seqno);

		DEBUG2(TRUE,(0, "rpcmgr::_object_handler: obj=0x%0x, local_port=0x%x\n",
						obj,req_msg->msgh_local_port));

		/* Do what has been requested */
		if (obj != NULL) {
			
			if (sending_failure_notification) {
				/*
				 * Failed to send a reply, build a failure msg
				 */
				reply_mach_msg_header_t reply =
					(reply_mach_msg_header_t)rep_msg;

				/*
				 * Notify invoker, assuming an rpc.
				 * If it is an async message, remote_port
				 * is port_null.
				 */
				reply->head.msgh_size = sizeof *reply;
/*				reply->head.msgh_seqno = 0; */
				reply->head.msgh_id += 100;
				reply->retval_type = retval_type;
				reply->retval = ret;

				/*
				 * XXX Should GC the invalid message.
				 */

				send_reply = TRUE;
			}
			else if (req_msg->msgh_id == MACH_NOTIFY_NO_SENDERS) {
				/*
				 * Not_now...
				 */
				USSTATS(USSTATS_NOTIF_NMS);
				DEBUG0(ustrace_rpc,
	       (0,"USTRACE: rpcmgr:: no remote senders: obj=0x%x\n",obj));
				mach_no_senders_notification_t* not_msg =
				(mach_no_senders_notification_t*)req_msg;
				(void) obj->_no_remote_senders(
							not_msg->not_count);
			}
			else if (req_msg->msgh_id == 0xAB/*ort*/) {
				/*
				 * Interrupt all syscalls active on the obj
				 */
				USSTATS(USSTATS_ABORT_RPC);
				DEBUG0(ustrace_rpc,
	       (0,"USTRACE: rpcmgr:: abort RPC: obj=0x%x\n",obj));
				(void) obj->_interrupt_incoming_invokes();
			}
			else {
				/*
				 * Cause a server method to be invoked
				 *
				 * XXX DPJ Put this inline ?
				 */
				(void) _incoming_invoke(obj,
					req_msg, rep_msg, 
					data_out, &send_reply);
#ifdef	TIMING_GEN
				if (timing_msg == 0) {
					timing_msg = (mach_msg_header_t*)
							timing_msgbuffer;
					bcopy(rep_msg,timing_msg,
							rep_msg->msgh_size);
				}
#endif	TIMING_GEN
#if	defined(TIMING_3) || defined(TIMING_4)
				bcopy(timing_msg,rep_msg,
						timing_msg->msgh_size);
#endif	TIMING_3 || TIMING_4
			}
			mach_object_dereference(obj);
		} 
		else {
			if (mach_object_verbose) {
				printf("rpcmgr: Received a msg on an unknown port\n");

#if WATCH_THE_DEAD
				obj = (usRemote*) _lookup_dead_external_port(
						req_msg->msgh_local_port);

				if (NULL != obj) {
				    printf("WATCH_DEAD:obj=0x%x, size=0x%x, is_where=0x%x, class=%s\n",
					obj, sizeof(*obj), obj->is_where(), obj->is_a()->class_name());
				}
				printf("Suspending...\n");
				
				task_suspend(mach_task_self());
#endif WATCH_THE_DEAD
				
			}
		}
#ifdef	TIMING_5
end_timing_5:
		bcopy(timing_msg,rep_msg,timing_msg->msgh_size);
		send_reply = TRUE;
#endif	TIMING_5
	}
}

void rpcmgr::start_object_handler(
	int	min_waiting,
	int	max_waiting,
	int	max_threads)
{
#ifdef	TIMING_6
	this->min_waiting = 1;
	this->max_waiting = 1;
	this->max_threads = 1;
#else	TIMING_6
	this->min_waiting = min_waiting;
	this->max_waiting = max_waiting;
	this->max_threads = max_threads;
#endif	TIMING_6
	threads_waiting = 1;
	thread_count = 1;
	mutex_init(&thread_waiting_lock);
	cthread_detach(cthread_fork((cthread_fn_t)_c_object_handler, this));
}


/*
 * Internal routine for cloning. NOT A METHOD
 */
void _c_clone_internal(hash_value_t val)
{
	usRemote*		obj = (usRemote*) val;
	obj->_reset_after_clone();
	mach_object_dereference(obj);
	return;
}


mach_error_t rpcmgr::clone_complete()
{
	mach_error_t		ret;

	/*
	 * This code should essentially reset the RPC manager
	 * to a clean state. It is unlikely to work if other parties
	 * are invoking various operations on this object before the
	 * cloning is finished.
	 */

	mutex_lock(&port_table_lock);

	/*
	 * Remove all the entries from the port_to_object table.
	 *
	 * Note that we cannot let the target objects remove themselves
	 * from the table, because this would break the table scanning
	 * logic.
	 *
	 * No need to deallocate the ports, because we never inherited
	 * the rights anyway.
	 */
	(void) hash_apply(mach_port_to_object_table,_c_clone_internal);
	hash_free(mach_port_to_object_table);
#if WATCH_THE_DEAD
	(void) hash_apply(dead_port_to_object_table,_c_clone_internal);
	hash_free(dead_port_to_object_table);
#endif WATCH_THE_DEAD
	/*
	 * Re-initialize the object as if it was new.
	 * Note that the current port set is bogus; no need to deallocate it.
	 */
	ret = mach_port_allocate(mach_task_self(), 
			MACH_PORT_RIGHT_PORT_SET,
			&mach_object_port_set);
	ASSERT_RETCODE(
		"rpcmgr::clone_complete.mach_port_allocate",ret);
	mach_port_to_object_table = hash_init(0, 0, 64);
#if WATCH_THE_DEAD
	dead_port_to_object_table = hash_init(0, 0, 64);
#endif WATCH_THE_DEAD
	mutex_unlock(&port_table_lock);

	/*
	 * Restart the handler threads if appropriate.
	 * We did not normally inherit any such threads from the parent.
	 */
	if (thread_count > 0) {
		(void) start_object_handler(
					min_waiting,max_waiting,max_threads);
	}

	return(ERR_SUCCESS);
}



mach_error_t rpcmgr::_outgoing_invoke(
	usRemote*		obj,
	mach_method_id_t	mid,
	arg_list_ptr		arglist)
{
	mach_error_t		result;
	char			msgbuffer[MSG_SIZE_MAX];
	mach_msg_header_t * 	msg = (mach_msg_header_t *) msgbuffer;
	mach_error_t		retval;
	boolean_t		ok;
	mach_msg_option_t  	msg_options = MACH_MSG_OPTION_NONE;

	USSTATS(USSTATS_OUTGOING_RPC);
	DEBUG0(ustrace_rpc,
	       (0, "USTRACE: rpcmgr:: outgoing RPC: obj=0x%0x, mid[%d]=%s\n",
		obj,mid,mid->method_name));

	if (obj->object_port() == 0) {
		DEBUG0(TRUE,(0, "rpcmgr::_outgoing_invoke: no object port"));
		return MACH_OBJECT_NO_SUCH_OPERATION;
	}

	msg->msgh_remote_port = obj->object_port();

#ifdef	TIMING_12
	*(int*)(arglist->args[0]) = 42;
	return(ERR_SUCCESS);
#endif	TIMING_12

	_msg_pack(msg,(int*)arglist,mid,MSG_DIR_REQUEST,ERR_SUCCESS);

	if (mid->arg_type.rpc) {
#ifdef	TIMING_9
		if (timing_msg != 0) {
			bcopy(timing_msg,msg,timing_msg->msgh_size);
			goto end_timing_9;
		}
#endif	TIMING_9
#ifdef	TIMING_10
		if (timing_msg != 0) goto end_timing_10;
#endif	TIMING_10
#ifdef	TIMING_11
		if (timing_msg != 0) goto end_timing_11;
#endif	TIMING_11
		msg->msgh_local_port = mig_get_reply_port();

		msg->msgh_bits |= MACH_MSGH_BITS(
					MACH_MSG_TYPE_COPY_SEND,
					MACH_MSG_TYPE_MAKE_SEND_ONCE);

		msg_options |= MACH_SEND_MSG|MACH_RCV_MSG;
		if (mid->arg_type.timeout) {
			msg_options |= MACH_SEND_TIMEOUT|MACH_RCV_TIMEOUT;
		}

#ifdef	TIMING_8
		if (timing_msg != 0)
			result = KERN_SUCCESS;
		else
#endif	TIMING_8
		if (mid->arg_type.interruptible) {
			result = intr_mach_msg(msg, msg_options,
					msg->msgh_size, sizeof(msgbuffer),
					msg->msgh_local_port,
					mid->arg_type.timeout,
					MACH_PORT_NULL);
		} else {
			result = mach_msg(msg, msg_options,
					msg->msgh_size, sizeof(msgbuffer),
					msg->msgh_local_port,
					mid->arg_type.timeout,
					MACH_PORT_NULL);
		}

#if	defined(TIMING_7) || defined(TIMING_8)
		if (timing_msg != 0)
			bcopy(timing_msg,msg,timing_msg->msgh_size);
#endif	TIMING_7 || TIMING_8

#ifdef	TIMING_GEN
		if (timing_msg == 0) {
			timing_msg = (mach_msg_header_t*) timing_msgbuffer;
			bcopy(msg,timing_msg,msg->msgh_size);
		}
#endif	TIMING_GEN

		/* Waste the reply port upon timeout */
		if (result == MACH_RCV_TIMED_OUT) {
			mig_dealloc_reply_port(); /* insure no late reply */
		}

		/*  
		 * If we were interrupted after the send, we must
		 * try to interrupt the server and be prepared for
		 * the server to interrupt or complete.
		 */
		if (MACH_RCV_INTERRUPTED == result) {
			mach_port_t old_reply_port = msg->msgh_local_port;

			/* build the abort message */
			msg->msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
			msg->msgh_local_port = MACH_PORT_NULL;
			msg->msgh_id = 0xAB/*ort*/;
			msg->msgh_size = sizeof(mach_msg_header_t);
	
			/* Send the abort and wait for the reply on original reply port */
			result = mach_msg(msg, 
					  (MACH_SEND_MSG|MACH_RCV_MSG|
					   MACH_RCV_TIMEOUT),
					msg->msgh_size, sizeof(msgbuffer),
					old_reply_port,
					interrupt_timeout,
					MACH_PORT_NULL);

			if (result == MACH_RCV_TIMED_OUT) {
				mig_dealloc_reply_port();  /* insure no late reply */
				result = MACH_RCV_INTERRUPTED;
			}
		}

		if (result != MACH_MSG_SUCCESS) {
			if ((MACH_RCV_INTERRUPTED == result) ||
			    (MACH_SEND_INTERRUPTED == result)) {
				return(EXCEPT_SOFTWARE);
			}
			_outgoing_invoke_error("Error in RPC",
					result, mid->method_name,
					msg->msgh_remote_port);
			return(result);
		}
#ifdef	TIMING_9
end_timing_9:
#endif	TIMING_9
#ifdef	TIMING_10
end_timing_10:
		retval = _msg_unpack(timing_msg,0,(int*)arglist,
							&mid,MSG_DIR_REPLY);
#else	TIMING_10
		retval = _msg_unpack(msg,0,(int*)arglist,
							&mid,MSG_DIR_REPLY);
#endif	TIMING_10
#ifdef	TIMING_11
end_timing_11:
		*(int*)(arglist->args[0]) = 42;
#endif	TIMING_11
		return(retval);
	} else {
		msg->msgh_local_port = MACH_PORT_NULL;

		msg->msgh_bits |= MACH_MSGH_BITS(
					MACH_MSG_TYPE_COPY_SEND, 0);
		msg_options |= MACH_SEND_MSG;
		if (mid->arg_type.timeout) {
			msg_options |= MACH_SEND_TIMEOUT;
		}

		/*
		 * Non-RPC things never need to be interruptible.
		 */
		result = mach_msg(msg, msg_options,
				msg->msgh_size, 0,
				MACH_PORT_NULL,
				mid->arg_type.timeout,
				MACH_PORT_NULL);

		if (result != ERR_SUCCESS) {
			if ((MACH_RCV_INTERRUPTED == result) ||
			    (MACH_SEND_INTERRUPTED == result)) {
				return(EXCEPT_SOFTWARE);
			}
			_outgoing_invoke_error("Error in send", 
					result, mid->method_name,
					msg->msgh_remote_port);

 			return result;
		}
		return (ERR_SUCCESS);
	}
}


mach_error_t rpcmgr::_incoming_invoke(
	usRemote*		obj,
	mach_msg_header_t*	req_msg,
	mach_msg_header_t*	rep_msg,
	char*			data_out,
	boolean_t*		send_reply)
{
	mach_method_id_t	mid;
	int			result;
	obj_arg_list_t		arglist;
	intr_cthread_id_t	intr_cthread_id;

#ifdef	TIMING_4
	if (timing_msg != 0) goto end_timing_4;
#endif	TIMING_4

	result = _msg_unpack(req_msg,data_out,(int*)&arglist,&mid, 
							MSG_DIR_REQUEST);

	if (result != ERR_SUCCESS) {
		reply_mach_msg_header_t reply = 
					(reply_mach_msg_header_t) rep_msg;

		/*
		 * Notify invoker, assuming an rpc.  If it is an async
		 * message, remote_port is MACH_PORT_NULL.
		 */
		reply->head.msgh_size = sizeof *reply;
/*		reply->head.msgh_seqno = 0; */
		reply->head.msgh_id = -1;
		reply->retval_type = retval_type;
		reply->retval = result;
		reply->head.msgh_remote_port = req_msg->msgh_remote_port;
		reply->head.msgh_local_port = MACH_PORT_NULL;

		/*
		 * XXX Should GC the invalid message.
		 */

		return(MACH_OBJECT_NO_SUCH_OPERATION);
	}

	USSTATS(USSTATS_INCOMING_RPC);
	DEBUG0(ustrace_rpc,
	       (0, "USTRACE: rpcmgr:: incoming RPC: mid[%d]=%s\n",
		mid,mid->method_name));

	req_msg->msgh_local_port = MACH_PORT_NULL;

	if (mid->arg_type.rpc) {
		/* Have an RPC, invoke and reply */
		mach_error_t		retval;
#ifdef	TIMING_3
		if (timing_msg != 0) goto end_timing_3;
#endif	TIMING_3

#ifdef	TIMING_2
		retval = ERR_SUCCESS;
		if (mid == mach_method_id(testmethod2)) {
			mach_object_reference(obj);
			*(testclass_ifc**) arglist.args[0] =
				(testclass_ifc*)testclass_ifc::castdown(obj);
		} else {
			*(int*) arglist.args[0] = 42;
		}
#else	TIMING_2
		intr_cthread_id = intr_cthread_id_self();
		obj->_register_incoming_invoke(intr_cthread_id);

#ifdef	TIMING_0
		retval = ERR_SUCCESS;
		*(int*) arglist.args[0] = 42;
#else	TIMING_0
#ifdef	TIMING_1
		retval = ERR_SUCCESS;
		if (mid == mach_method_id(testmethod2)) {
			mach_object_reference(obj);
			*(testclass_ifc**) arglist.args[0] =
				(testclass_ifc*)testclass_ifc::castdown(obj);
		} else {
			*(int*) arglist.args[0] = 42;
		}
#else	TIMING_1
		LOGCHECK;
		LOG2(TRUE, 2001, obj, req_msg->msgh_id);

		switch (mid->arg_type.size_args) {
			case 0:
				retval = obj->invoke(mid);
				break;
			case 4:
				retval = obj->invoke(mid,
						(void*)arglist.args[0]);
				break;
			case 8:
				retval = obj->invoke(mid,
						(void*)arglist.args[0],
						(void*)arglist.args[1]);
				break;
			default:
				retval = obj->invoke(mid, arglist);
				break;
		}
#endif	TIMING_1
#endif	TIMING_0

		obj->_deregister_incoming_invoke(intr_cthread_id);
#endif	TIMING_2

		*rep_msg = *req_msg;
		rep_msg->msgh_id = -1;
		_msg_pack(rep_msg,(int*)&arglist,mid,MSG_DIR_REPLY,retval);
#ifdef	TIMING_3
end_timing_3:
#endif	TIMING_3
#ifdef	TIMING_4
end_timing_4:
#endif	TIMING_4
		*send_reply = TRUE;
	} else {
		/* No Reply Required */
		intr_cthread_id = intr_cthread_id_self();
		obj->_register_incoming_invoke(intr_cthread_id);

		LOG2(TRUE, 2002, obj, req_msg->msgh_id);
		switch (mid->arg_type.size_args) {
			case 0:
				(void) obj->invoke(mid);
				break;
			case 4:
				(void) obj->invoke(mid,
						(void*)arglist.args[0]);
				break;
			case 8:
				(void) obj->invoke(mid,
						(void*)arglist.args[0],
						(void*)arglist.args[1]);
				break;
			default:
				(void) obj->invoke(mid, arglist);
				break;
		}

		obj->_deregister_incoming_invoke(intr_cthread_id);

		result = ERR_SUCCESS;
		*send_reply = FALSE;
	}

	if (req_msg->msgh_bits & MACH_MSGH_BITS_COMPLEX) {
		(void) _msg_free(req_msg,&mid);
	}

	return(result);
}

/* Error print routine to be used only by rpcmgr::default */
void rpcmgr::_outgoing_invoke_error(
	char			*msg_str,
	mach_error_t		err,
	char*			method_name,
	mach_port_t		remote_port)
{
	boolean_t	ok = FALSE; /* It's sorta "ok" to have some problems */

	if ((MACH_SEND_TIMEOUT == err) ||
	    (MACH_RCV_TIMEOUT == err)) {
	    ok = TRUE;
	}
	else if (MACH_SEND_INVALID_DEST == err) {
		mach_port_type_t	port_type;
		mach_error_t		mpt_ret;
		mpt_ret = mach_port_type(mach_task_self(), 
				remote_port,
				&port_type);
		if ((MACH_PORT_TYPE_DEAD_NAME == port_type) &&
			(! mpt_ret)) {
			/* object port dead, prevent doing this again?? XXX */
			ok = TRUE;
		}
	}
	if (mach_object_verbose && (! ok)) {
		printf("%s: 0x%x method=%s port=%d\n", 
		       msg_str, err, method_name,remote_port);
	}
}


/*
 * Marshalling and un-marshalling functions.
 *
 * XXX The code in _msg_pack(), _msg_unpack(), etc. relies on the fact that
 * sizeof(int) == sizeof(char *). This should be fixed.
 */

#define		msghdr(ptr)	((mach_msg_type_long_t *)ptr)->msgtl_header
#define		msghdrptr(ptr)	((mach_msg_type_long_t *)ptr)
#define		roundup_long(val) 	(((val)+3) & ~3)

mach_error_t rpcmgr::_msg_pack(
	mach_msg_header_t*	msg,
	int*			args,
	mach_method_id_t	mid,
	msg_direction_t		msg_direction,
	mach_error_t		retval)
{
	const arg_type_t	arg_types = &mid->arg_type;
	const arg_info_t*	arg_info = arg_types->args;
	boolean_t	is_simple;
	int 		number, cnt, i, j, copy_size, arg_size;
	int *		src;
	int * 		dst;
	void*		obj;
	usRemote*	robj;
	int		indirect_cnt;
	boolean_t	variable;
	boolean_t	is_object;
	int		obj_port_name;
	char *		method_name_str;

	if (msg_direction == MSG_DIR_REPLY) {
		reply_mach_msg_header_t reply = (reply_mach_msg_header_t)msg;
		/*
		 * by convention, each reply message starts with an integer return value
		 */
		reply->head.msgh_size = sizeof(*reply);
		reply->retval_type = retval_type;
		reply->retval = retval;

		dst = (int *)(((char *)reply) + sizeof(*reply));

		/*
		 * If the return code indicates a failure, we should
		 * not transfer any other arguments.
		 *
		 * XXX For now, any non-zero code indicates a failure.
		 * In the future, there should be a way to report
		 * some warnings without totally aborting the operation.
		 */
		if (retval != ERR_SUCCESS) {
			msg->msgh_bits = MACH_MSGH_BITS_ZERO;
/*			msg->msgh_seqno = 0; */
			return;
		}
		is_simple = arg_types->out_simple;
	} else {
		/*
		 * request messages ignore the return value
		 */

		msg->msgh_size = sizeof(*msg);
		dst = (int *)(((char *)msg) + sizeof(*msg));

		/*
		 * By convention, request messages contain the 
		 * string name of their method if the msg id is -1.
		 */
		msg->msgh_id = arg_types->msgid;
		if (arg_types->msgid == -1) {
			method_name_str = mid->method_name;
			/*
			 * Reserve enough space (on long word boundary) for 
			 * method string.
			 */
			number = roundup_long(strlen(method_name_str)+1);
			*(mach_msg_type_t *)dst = method_string_info;
			msghdr(dst).msgt_number = number;
			dst = (int *) ((char *)dst+sizeof(mach_msg_type_t));
			strcpy((char *)dst, method_name_str);
			msg->msgh_size += number + sizeof(mach_msg_type_t);
			dst = (int *)(((char *)dst) + number);
		}
		is_simple = arg_types->in_simple;
	}

	for (i = 0; i < arg_types->num_args; i++) {
		variable = arg_info[i].variable;
		indirect_cnt = arg_info[i].indirect_cnt;
		arg_size = arg_info[i].arg_size;
		if (msg_direction == MSG_DIR_REQUEST) {
			if (!arg_info[i].input) {
				args = (int *)(((char *)args) + arg_size);
				if (variable && !arg_info[i+1].input) {
					i++;
					args++;
				}
				continue;
			}
		} else {
			if (!arg_info[i].output) {
				args = (int *)(((char *)args) + arg_size);
				if (variable && !arg_info[i+1].output) {
					i++;
					args++;
				}
				continue;
			}
		}

		src = args;
		cnt = indirect_cnt;
		while (cnt > 0) { src = (int *)*src; cnt--; }

		if (variable) {
			int * cnt_ptr = args + 1;
			int cnt = arg_info[i+1].indirect_cnt;
			while (cnt > 0) { cnt_ptr = (int *)*cnt_ptr; cnt--; }
			number = *cnt_ptr * arg_info[i].unit_number;

		} else if (arg_info[i].name == MACH_MSG_TYPE_STRING) {
			number = strlen((char *)src) + 1;
		} else {
			number = arg_info[i].number;
		}

		copy_size = ((number * arg_info[i].size)+7)>>3;
		copy_size = roundup_long(copy_size);

		if (is_object = arg_info[i].object) {
			if (arg_info[i].class_desc == 0) {
				mid->arg_type.args[i].class_desc =
					class_info::GLOBAL->_lookup_class(
						arg_info[i].class_name);
				if (arg_info[i].class_desc == 0) {
					ERROR((Diag,
					"Unknown class in method args: %s",
					arg_info[i].class_name));
					return(MACH_OBJECT_NO_SUCH_OPERATION);
				}
			}

			for (j=0; j< number; j++) {
				obj = (void*) src[j];
				if (obj == NULL)  {
					obj_port_name = MACH_MSG_TYPE_COPY_SEND;
				}
				else {
					/*
					 * Convert the object to usRemote*
					 */
					robj = arg_info[i].class_desc->
						converter_to_remote(obj);

					/*
					 * Find a suitable port to represent
					 * the object
					 */
					src[j] = (int) robj->
						get_transfer_port(
							&obj_port_name);
					/* 
					 * Release ref if this is
					 * a return value.
					 */
					if (msg_direction == MSG_DIR_REPLY) {
						mach_object_dereference(robj);
					}
				}
			}
		}
		msghdr(dst).msgt_unused = FALSE;

		if ((copy_size > 4095) ||
		    (number > 4095) ||
		    (arg_info[i].dealloc) ||
		    ((msg_direction == MSG_DIR_REQUEST) && arg_info[i].copy) ||
		    ((msg_direction == MSG_DIR_REPLY) && (indirect_cnt > 1))) {
			is_simple = FALSE;
			msghdr(dst).msgt_name = 0;
			msghdr(dst).msgt_size = 0;
			msghdr(dst).msgt_number = 0;
			msghdr(dst).msgt_deallocate      =
					arg_info[i].dealloc;
			msghdr(dst).msgt_longform 	     = TRUE;
			msghdr(dst).msgt_inline 	     = FALSE;
			if (is_object) {
				msghdrptr(dst)->msgtl_name   = 
					obj_port_name;
			}
			else {
				msghdrptr(dst)->msgtl_name   = 
					arg_info[i].name;
			}
			msghdrptr(dst)->msgtl_size   = 
					arg_info[i].size;
			msghdrptr(dst)->msgtl_number = number;
			dst = (int *)((char *)dst+sizeof(mach_msg_type_long_t));
			*dst = (int)src;
			msg->msgh_size += sizeof(int *)+sizeof(mach_msg_type_long_t);
			dst = dst + 1;
		} else {
			*(mach_msg_type_t *)dst = arg_info[i].info;
			if (is_object) {
				msghdr(dst).msgt_name = obj_port_name;
			}
			msghdr(dst).msgt_number = number;
			dst = (int *) ((char *)dst+sizeof(mach_msg_type_t));
			bcopy((char *)src, (char *)dst, copy_size);
			msg->msgh_size += copy_size + sizeof(mach_msg_type_t);
			dst = (int *)(((char *)dst) + copy_size);
		}


		args = (int *)(((char *)args) + arg_size);
		if (variable) { 
			args++; i++; 
		}

		if (is_object) {
			if (obj != NULL) {
				/*
				 * Send the name of the proxy class
				 * to be instantiated at the client.
				 */
				char * name;
				name = (char*) robj->remote_class_name();
				
				number = strlen(name) + 1;
				number = roundup_long(number);
				msghdr(dst).msgt_unused    = FALSE;
				msghdr(dst).msgt_deallocate= FALSE;
				msghdr(dst).msgt_longform  = FALSE;
				msghdr(dst).msgt_inline    = TRUE;
				msghdr(dst).msgt_name      = MACH_MSG_TYPE_CHAR;
				msghdr(dst).msgt_number    = number;
				msghdr(dst).msgt_size      = 8;
				dst = (int *) ((char *)dst+sizeof(mach_msg_type_t));
				bcopy(name, (char *)dst, number);
				msg->msgh_size += number+sizeof(mach_msg_type_t);
				dst = (int *)(((char *)dst) + number);
			}
		}
	}

	if (! is_simple) {
		msg->msgh_bits = MACH_MSGH_BITS_COMPLEX;
	}
	else {
		msg->msgh_bits = MACH_MSGH_BITS_ZERO;
	}

/*	msg->msgh_seqno = 0; */

	return(ERR_SUCCESS);
}


mach_error_t rpcmgr::_msg_unpack(
	mach_msg_header_t*	msg,
	char*			data_out,
	int*			args,
	mach_method_id_t*	mid,
	msg_direction_t		msg_direction)
{
	int  		*dst, *dptr, *new_dst;
	arg_type_t 	arg_types;
	boolean_t	is_inline;
	int		msg_num, msg_size, msg_name, i, j;
	int		out_size, data_size, arg_size, msg_elem_size;
	int		indirect_cnt, number;
	boolean_t	variable;
	mach_error_t	retval;
	char *		method_name_str;

	/*
	 * Start parsing the message, and deal with the first element
	 * present in all messages (fixed-format):
	 *	- a method name (string) for requests, unless the
	 *		msg_id is not -1.
	 *	- a return code for replies
	 */
	if (msg_direction == MSG_DIR_REQUEST) {
		dst = (int *)(((char *)msg) + sizeof(*msg));
		retval = ERR_SUCCESS;

		if (msg->msgh_id == -1) {
			number = msghdr(dst).msgt_number;
			dst = (int *) ((char *)dst+sizeof(mach_msg_type_t));

			method_name_str = (char *)dst;
			*mid = method_info::GLOBAL->_lookup_name(
							method_name_str);
			if (*mid == 0) {
				ERROR(
		(0,"rpcmgr: incoming remote invoke for unknown method: %s",
							method_name_str));
				return (MACH_OBJECT_NO_SUCH_OPERATION);
			}
			dst = (int *)(((char *)dst) + number);
		} else {
			*mid = method_info::GLOBAL->_lookup_msgid(
							msg->msgh_id);
			if (*mid == 0) {
				ERROR(
		(0,"rpcmgr: incoming remote invoke for unknown method: 0x%x",
							msg->msgh_id));
				return (MACH_OBJECT_NO_SUCH_OPERATION);
			}
		}
		arg_types = &(*mid)->arg_type;
	} else {
		reply_mach_msg_header_t reply = (reply_mach_msg_header_t)msg;

		dst = (int *)(((char *)reply) + sizeof(*reply));
		retval = reply->retval;

		/*
		 * If the return code indicates a failure, we should
		 * not transfer any other arguments.
		 *
		 * XXX For now, any non-zero code indicates a failure.
		 * In the future, there should be a way to report
		 * some warnings without totally aborting the operation.
		 */
		if (retval != ERR_SUCCESS) {
			return(retval);
		}

		arg_types = &(*mid)->arg_type;
	}

	const arg_info_t*	arg_info = arg_types->args;

	/*
	 * Iterate over all arguments in the arg list.
	 */
	for (i = 0; i < arg_types->num_args; i++) {
		/*
		 * Set-up some utility variables concerning
		 * the current argument.
		 */
		variable = arg_info[i].variable;
		indirect_cnt = arg_info[i].indirect_cnt;
		arg_size = arg_info[i].arg_size;
		out_size = arg_info[i].out_size;

		/*
		 * Skip over arguments that are not represented
		 * in the message (i.e. wrong direction).
		 * Reserve space for OUT arguments.
		 */
		if (msg_direction == MSG_DIR_REQUEST) {
			if (!arg_info[i].input) {
				if (arg_info[i].output) {
					*args = (int) data_out;
					data_out += out_size;
				}
				args = (int *)(((char *)args) + arg_size);
				if (variable && !arg_info[i+1].input) {
					if (arg_info[i].output) {
						*args = (int) data_out;
						data_out += sizeof(int);
					}
					args++;
					i++;
				}
				continue;
			}
		} else {
			if (!arg_info[i].output) {
				args = (int *)(((char *)args) + arg_size);
				if (variable && !arg_info[i+1].output) {
					args++;
					i++;
				}
				continue;
			}
		}

		/*
		 * Examine the type descriptor.
		 */
		is_inline = msghdr(dst).msgt_inline;
		if (msghdr(dst).msgt_longform) {
			msg_num = msghdrptr(dst)->msgtl_number;
			msg_size= msghdrptr(dst)->msgtl_size;
			msg_name= msghdrptr(dst)->msgtl_name;
			dst = (int *)((char *)dst+sizeof(mach_msg_type_long_t));
		} else {
			msg_num = msghdr(dst).msgt_number;
			msg_size= msghdr(dst).msgt_size;
			msg_name= msghdr(dst).msgt_name;
			dst = (int *) ((char *)dst+sizeof(mach_msg_type_t));
		}

		/*
		 * Chek the type of the incoming data.
		 */
		if (((arg_info[i].name != msg_name) &&
		     (! (MACH_MSG_TYPE_PORT_ANY(arg_info[i].name) &&
		         MACH_MSG_TYPE_PORT_ANY(msg_name)))) ||
		    (arg_info[i].size != msg_size)) {
			if (mach_object_verbose) {
				printf(
				"ERROR IN MSG: Arg #%d, ID %d, name wanted: %d - got %d, size wanted: %d - got %d\n",
						i,msg->msgh_id,
						arg_info[i].name,
						msg_name,
						arg_info[i].size,
						msg_size);
			}
			return(MACH_OBJECT_BAD_MESSAGE);
	        }

		/*
		 * Figure-out the size and location of the incoming data.
		 */
		data_size = roundup_long(((msg_num*msg_size)+7)>>3);
		if (is_inline) {
			dptr = dst;
			msg_elem_size = data_size;
		} else {
			dptr = (int *)*dst;
			msg_elem_size = sizeof(int *);
		}

		/*
		 * If the argument is a variable-size array,
		 * set-up the "count" variable.
		 */
		if (variable) {
 			if (msg_direction == MSG_DIR_REQUEST) { /* preference to int* */
 				int * num_ptr = (int*)data_out;
 				int cnt = arg_info[i+1].indirect_cnt;
 				*num_ptr = msg_num /
					arg_info[i].unit_number;
 				while (cnt > 0) {
 					num_ptr[1] = (int)num_ptr;
 					num_ptr = num_ptr + 1;
 					cnt--;
 				}
 				args[1] = *num_ptr;
 				data_out = (char *) num_ptr;
 			} else {
 				int * num_ptr = args + 1;
 				int cnt = arg_info[i+1].indirect_cnt;
 				while (cnt > 0) { 
					num_ptr = (int *)*num_ptr;
					cnt--;
				}
 				*num_ptr = msg_num / arg_info[i].unit_number;
			}
		}

		/*
		 * If the argument contains object references,
		 * translate them in place by replacing port names with
		 * object pointers.
		 */
		new_dst = (int *) 0;
		if (arg_info[i].object) {
			if (arg_info[i].class_desc == 0) {
				(*mid)->arg_type.args[i].class_desc =
					class_info::GLOBAL->_lookup_class(
						arg_info[i].class_name);
				if (arg_info[i].class_desc == 0) {
					ERROR((Diag,
					"Unknown class in method args: %s",
					arg_info[i].class_name));
					return(MACH_OBJECT_NO_SUCH_OPERATION);
				}
			}

			char * class_name;
			char* proxy_type;
			if (dptr[0] != 0) {
				int len;
				new_dst = (int *)(((char *)dst) +
								msg_elem_size);
				len = msghdr(new_dst).msgt_number;
				len = roundup_long(len);
				new_dst = (int *)(((char *)new_dst) + 
							sizeof(mach_msg_type_t));
				class_name = (char *)new_dst;
				new_dst = (int *)(((char *)new_dst) + len);
			} else {
				class_name = 0;
			}
			proxy_type = arg_info[i].class_name;
			DEBUG1(TRUE,(0,"rpcmgr: proxy name=%s, proxy type=%s\n", class_name, proxy_type));

			for (j=0; j< msg_num; j++) {
				mach_error_t	ret;
				usRemote*	robj;
				void*		obj;

				mach_port_t p = dptr[j];
				if (p == MACH_PORT_NULL) continue;
				robj = _lookup_external_port(p);

				if (robj != NULL) {
					/*
					 * Convert the object to the
					 * appropriate proxy type
					 *
					 * XXX DPJ Cache the proxy class?
					 */
					usClass* class_desc =
					class_info::GLOBAL->_lookup_class(
								proxy_type);
					if (class_desc == 0) {
						ERROR((Diag,
						"RPC: unknown class: %s",
						proxy_type));
						return(
						MACH_OBJECT_NO_SUCH_OPERATION);
					}
					obj = (void*) robj->_castdown(*class_desc);
					dptr[j] = (int) obj;
					/*
					 * We already had the object --
					 * get rid of the new user reference
					 * (send right).
					 */
					mach_port_deallocate(
							mach_task_self(),p);
					continue;
				}

				/*
				 * Construct a new object
				 *
				 * XXX DPJ Cache the proxy class?
				 */
				usClass* class_desc = 
					class_info::GLOBAL->_lookup_class(
								class_name);
				if (class_desc == 0) {
					ERROR((Diag,
						"RPC: unknown class: %s",
						class_name));
					return(MACH_OBJECT_NO_SUCH_OPERATION);
				}
				usClass* type_desc  = arg_info[i].class_desc;
				obj = class_desc->virtual_constructor(
								p, *type_desc);
			    	dptr[j] = (int) obj;
			}
		}

		/*
		 * Place the data or data pointer in the argument list.
		 */
		switch(indirect_cnt) {
			case 0:
				/*
				 * Copy the data directly onto the stack.
				 */
				bcopy((char *)dptr,(char *)args,data_size);
				break;

			case 1:
				if (msg_direction == MSG_DIR_REQUEST) {
					if (arg_info[i].output) {
						/*
						 * IN OUT argument.
						 * Need to preserve the
						 * original data for later GC.
						 */
						*args = (int) data_out;
						bcopy((char *)dptr,
							(char *)data_out,
							data_size);
						data_out += out_size;
					} else {
						/*
						 * Use a pointer to the data
						 * inside the message.
						 */
						*args = (int) dptr;
					}
				} else {
					/*
					 * REPLY. Copy the data into the
					 * buffer specified by the caller.
					 */
					bcopy((char *)dptr,(char *)*args,
								data_size);
				}
				break;

			case 2:
				if (msg_direction == MSG_DIR_REQUEST) {
					/*
					 * Allocate a memory cell to hold
					 * a pointer to the data.
					 */
					*args = (int) data_out;
					data_out += out_size;
				}
				**((int **)args) = (int) dptr;
				break;
		}

		/*
		 * Advance the current pointer in the message,
		 * accounting for the extra data possibly transmitted
		 * with object references (i.e. class name).
		 */
		if (new_dst) {
			dst = new_dst;
		} else {
			dst = (int *)(((char *)dst) + msg_elem_size);
		}

		/*
		 * Advance the current pointer in the argument list,
		 * accounting for the "count" variable needed with
		 * variable-size arrays.
		 */
		args = (int *)(((char *)args) + arg_size);
		if (variable) { 
			args++; i++; 
		}
	}

	return(retval);
}


mach_error_t rpcmgr::_msg_free(
	mach_msg_header_t*	msg,
	mach_method_id_t*	mid)
{
	int  		*dst, *dptr, *new_dst;
	const arg_type_t 	arg_types = &(*mid)->arg_type;
	const arg_info_t*	arg_info = arg_types->args;
	boolean_t	is_inline;
	int		msg_num, msg_size, i, j, copy_size;
	int		number;
	boolean_t	variable;
	mach_error_t	retval;

	/* 
	 * XXX This code will correctly deal with object references only
	 * if those pointers have not been obliterated in the message
	 * by the method being invoked. This is guaranteed for
	 * simple-indirection IN OUT arguments, but not for the other
	 * cases (i.e. IN arguments, and double-indirection args).
	 */ 	

 	dst = (int *)(((char *)msg) + sizeof(*msg));
	retval = ERR_SUCCESS;

	/*
	 * By convention, request messages contain the 
	 * string name of their method if the id field is -1:
	 */
	if (arg_types->msgid == -1) {
		number = msghdr(dst).msgt_number;
		dst = (int *) ((char *)dst+sizeof(mach_msg_type_t));
		dst = (int *)(((char *)dst) + number);
	}


	for (i = 0; i < arg_types->num_args; i++) {
		variable = arg_info[i].variable;
		if (!arg_info[i].input) {
			if (variable && !arg_info[i+1].input) {
				i++;
			}
			continue;
		}

		is_inline = msghdr(dst).msgt_inline;
		if (msghdr(dst).msgt_longform) {
			msg_num = msghdrptr(dst)->msgtl_number;
			msg_size= msghdrptr(dst)->msgtl_size;
			dst = (int *)((char *)dst+sizeof(mach_msg_type_long_t));
		} else {
			msg_num = msghdr(dst).msgt_number;
			msg_size= msghdr(dst).msgt_size;
			dst = (int *) ((char *)dst+sizeof(mach_msg_type_t));
		}

		if (is_inline) {
			dptr = dst;
			copy_size = ((msg_num*msg_size)+7)>>3;
			copy_size = roundup_long(copy_size);
		} else {
			dptr = (int *)*dst;
			copy_size = sizeof(int *);
		}

		new_dst = (int *) 0;
		if (arg_info[i].object) {
			if (arg_info[i].class_desc == 0) {
				(*mid)->arg_type.args[i].class_desc =
					class_info::GLOBAL->_lookup_class(
						arg_info[i].class_name);
				if (arg_info[i].class_desc == 0) {
					ERROR((Diag,
					"Unknown class in method args: %s",
					arg_info[i].class_name));
					return(MACH_OBJECT_NO_SUCH_OPERATION);
				}
			}

			if (dptr[0] != 0) {
				int len;
				new_dst = (int *)(((char *)dst) + copy_size);
				len = msghdr(new_dst).msgt_number;
				len = roundup_long(len);
				new_dst = (int *)(((char *)new_dst) + 
							sizeof(mach_msg_type_t));
				new_dst = (int *)(((char *)new_dst) + len);
			}

			for (j=0; j< msg_num; j++) {
				void*		obj;
				usRemote*	robj;

				obj = (void *) dptr[j];
				if (obj != NULL)  {
					/*
					 * Convert the object to usRemote*
					 */
					robj = arg_info[i].class_desc->
						converter_to_remote(obj);
					mach_object_dereference(robj);
				}
			}
		}

		if ((!is_inline) && (!arg_info[i].copy)) {
			vm_size_t	size = ((msg_num*msg_size)+7)>>3;

			size = roundup_long(size);
			retval = vm_deallocate(mach_task_self(),
						(vm_offset_t)dptr,size);
			if (retval != KERN_SUCCESS) return (retval);
		}

		if (new_dst) {
			dst = new_dst;
		} else {
			dst = (int *)(((char *)dst) + copy_size);
		}

		if (variable) { 
			i++; 
		}
	}

	return(retval);
}


