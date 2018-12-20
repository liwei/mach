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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tm/tm_task_timer.cc,v $
 *
 * Purpose:
 *	Interval timer routines
 *
 * HISTORY
 * $Log:	tm_task_timer.cc,v $
 * Revision 2.5  94/07/13  17:33:45  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  94/05/17  14:10:49  jms
 * 	Static variables bug in 2.3.3 g++ requires declaration of 
 * 	static object name -modh
 * 
 * 	Cast args to cthread_fork
 * 	[94/04/29  13:46:09  jms]
 * 
 * Revision 2.3.1.1  94/02/18  11:38:54  modh
 * 	static variables bug in 2.3.3 g++ requires declaration of static object name
 * 
 * Revision 2.3  93/01/20  17:39:40  jms
 * 	Timer bug fixes
 * 	[93/01/18  17:39:35  jms]
 * 
 * Revision 2.2  92/07/05  23:35:58  dpj
 * 	Use new us_tm_task_ifc.h interface for the C++ taskmaster.
 * 	Translate to G++
 * 	[92/06/24  18:18:19  jms]
 * 
 * Revision 2.5  92/03/05  15:12:52  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:30:04  jms]
 * 
 * Revision 2.4  90/12/21  13:55:53  jms
 * 	Undo last change and wait for the real fix.
 * 	[90/12/19  14:34:14  neves]
 * 
 * 	Do some interval timer cleanup.
 * 	[90/12/17  14:46:46  neves]
 * 
 * 	Fixed timer_abs_to_rel and timer_rel_to_abs under MACH3_US.
 * 	[90/12/17  14:37:09  neves]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:16:53  jms]
 * 
 * Revision 2.3  90/11/27  18:21:36  jms
 * 	Use pure kernel time mechinisms (from jjc)
 * 	[90/11/20  17:30:32  jms]
 * 
 * Revision 2.2  90/08/13  15:44:40  jjc
 * 	Created.
 * 	[90/07/19            jjc]
 * 
 *
 */

/*
 *	TODO
 *		1) Optimize
 */
extern "C" {
#include <cthreads.h>
#include "dll.h"
#include <mach.h>
#include <mach/message.h>
#include <mach/time_value.h>
}
#include "tm_task_ifc.h"
#include "us_error.h"
#include "timer.h"

#define	TICK	1000		/* accuracy of timers in microseconds */

struct timer {
	dll_chain_t		chain;		/* queue pointers */
	tm_task			*task;		/* TM task object */
	timer_id_t		id;		/* ID for remove */
	struct timer_value	value;		/* interval and time left */
	int			event;		/* exception to raise */
	int			code;		/* exception code */
	int			subcode;	/* exception subcode */
};

static timer_fix(time_value_t *);
static timer_rel_to_abs(time_value_t *);
static timer_abs_to_rel(time_value_t *);
extern "C" void timer_start();


#ifdef GXXBUG_STATIC_MEM
static timer_id_t       _7tm_task$timer_id;
static struct mutex     _7tm_task$timer_lock;
static mach_port_t      _7tm_task$timer_port;
static dll_head_t       _7tm_task$timer_queue;
static int              _7tm_task$timer_up;
#endif


/*
 *	Initialize this module
 */
mach_error_t tm_task::timer_init()
{
	register cthread_t	new_thread;
	kern_return_t		kr;

	if (! timer_up) {
		dll_init(&timer_queue);
		mutex_init(&timer_lock);
		timer_id = TIMER_INVALID_ID;

		kr = mach_port_allocate(mach_task_self(), 
				MACH_PORT_RIGHT_RECEIVE, &timer_port);
		if (kr != KERN_SUCCESS) {
			mach_error("timer_init: port_allocate() failed ", kr);
			return(kr);
		}

		kr = mach_port_insert_right(mach_task_self(), 
				timer_port, timer_port,
				MACH_MSG_TYPE_MAKE_SEND);
		if (kr != KERN_SUCCESS) {
			mach_error("timer_init: mach_port_insert_right() failed ", kr);
			return(kr);
		}
		timer_up = TRUE;

		new_thread = cthread_fork((cthread_fn_t)timer_start, this);
		cthread_set_name(new_thread, "timer_server");
		cthread_detach(new_thread);
	}
	return(ERR_SUCCESS);
}

/*
 *	Shutdown this module
 */
mach_error_t tm_task::timer_shutdown()
{
	register dll_t		cur;
	register timer_t	it;
	mach_msg_header_t	msg;
	mach_msg_return_t	msg_retval;

	if (timer_up) {
		/*
		 *	Remove and free every element in the queue
		 */
		mutex_lock(&timer_lock);
		cur = dll_first(&timer_queue);
		while (cur != (dll_t)(&timer_queue)) {
			it = (timer_t)cur;
			dll_remove(&timer_queue, it, timer_t, chain);
			Free((char *)it);
			cur = dll_next(cur);
		}
		mutex_unlock(&timer_lock);
		timer_up = FALSE;
		/*
		 *	Send a message to the server to wake up and die
		 */
		bzero(&msg, sizeof(msg));
		msg.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
		msg.msgh_size = sizeof(msg);
		msg.msgh_remote_port = timer_port;
		msg_retval = mach_msg_send(&msg);
		if (msg_retval != MACH_MSG_SUCCESS) {
			mach_error("timer_shutdown: msg_send failed ",
					msg_retval);
		}
	}
	return(ERR_SUCCESS);
}

/*
 *	Get value (time left and interval) of interval timer record
 *	with given ID for the given task.
 */
mach_error_t tm_task::tm_timer_get(
	timer_id_t			id,
	timer_type_t			which,
	register timer_value_t		value)
{
	register dll_t		cur;
	register timer_t	it;
	int			error_code;

	if (value == 0)
		return(US_INVALID_ARGS);	/* bad value pointer */

	TIMER_CLEAR(&value->time);
	TIMER_CLEAR(&value->interval);

	it = (timer_t)(this->timer_find(id));
	error_code = US_OBJECT_NOT_FOUND;	/* can't find it */
	if (it) {
		*value = it->value;
		timer_abs_to_rel(&value->time);
		error_code = ERR_SUCCESS;	/* found it */
	}

	return(error_code);
}

/*
 *	Set (or reset) timer and insert it into queue
 */
mach_error_t tm_task::tm_timer_set(
	timer_type_t			which,
	int				event,
	int				code,
	int				subcode,
	timer_id_t			*id,
	timer_value_t			value)
{
	register timer_t	it;
	mach_msg_header_t	msg;
	mach_msg_return_t	msg_retval;

	if (value == 0 || id == 0 || timer_fix(&value->time) 
	    || timer_fix(&value->interval))
		return(US_INVALID_ARGS);	/* bad ptr or timer value */

	/*
	 *	Try to lookup timer record in queue, corresponding to 
	 *	the given task and ID.
	 */
	it = (timer_t)(this->timer_find(*id));
	if (it == 0) {
		it = (timer_t)Malloc(sizeof(struct timer));
		if (it == 0)
			return(US_RESOURCE_EXHAUSTED);	/* no memory for record */
		it->task = this;
		mach_object_reference(this);
		mutex_lock(&timer_lock);
		if (timer_id == TIMER_INVALID_ID)
			timer_id++;
		it->id = *id = timer_id++;
		mutex_unlock(&timer_lock);
	}
	else {
		mutex_lock(&timer_lock);
		dll_remove(&timer_queue, it, timer_t, chain);
		mutex_unlock(&timer_lock);
	}

	/* change value and signal */

	it->value = *value;
	timer_rel_to_abs(&it->value.time);
	it->event = event;
	it->code = code;
	it->subcode = subcode;

	/*
	 *	Insert timer record into queue
	 */
	this->timer_insert(it);

	if ((dll_t)it == dll_first(&timer_queue)) {
		/*
		 *	Send a message to the server to wake up
		 *	because we have added a timer to the queue
		 *	that is going to expire before the one he
		 *	is waiting for.
		 */
		bzero(&msg,sizeof(msg));
		msg.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
		msg.msgh_size = sizeof(msg);
		msg.msgh_remote_port = timer_port;
		msg_retval = mach_msg_send(&msg);
		if (msg_retval != MACH_MSG_SUCCESS) {
			mach_error("timer_set: msg_send failed ", msg_retval);
		}
	}
	return(ERR_SUCCESS);
}

/*
 *	Delete interval timer record with given ID for the given task
 *	and return its old value
 */
mach_error_t tm_task::tm_timer_delete(
	timer_id_t		id,
	timer_value_t		ovalue)
{
	register dll_t		cur;
	register timer_t	it;
	int			error_code;
	mach_msg_header_t	msg;
	mach_msg_return_t	msg_retval;
	int			need_msg;

	if (ovalue) {
		TIMER_CLEAR(&ovalue->time);
		TIMER_CLEAR(&ovalue->interval);
	}

	it = (timer_t)(this->timer_find(id));
	error_code = US_OBJECT_NOT_FOUND;
	need_msg = FALSE;
	if (it) {
		mutex_lock(&timer_lock);
		if ((dll_t)it == dll_first(&timer_queue))
			need_msg = TRUE;
		dll_remove(&timer_queue, it, timer_t, chain);
		mutex_unlock(&timer_lock);
		if (ovalue) {
			*ovalue = it->value;
			timer_abs_to_rel(&ovalue->time);
		}
		mach_object_dereference(it->task);
		Free((char *)it);
		error_code = ERR_SUCCESS;
	}
	if (need_msg) {
		/*
		 *	Send message to server to wake up because we just
		 *	deleted the timer he is waiting on.
		 */
		bzero(&msg, sizeof(msg));
		msg.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
		msg.msgh_size = sizeof(msg);
		msg.msgh_remote_port = timer_port;
		msg_retval = mach_msg_send(&msg);
		if (msg_retval != MACH_MSG_SUCCESS) {
			mach_error("timer_delete: msg_send failed ", 
					msg_retval);
		}
	}
	return(error_code);
}

/*
 *	Remove and free all the interval timer records for the given task
 */
mach_error_t tm_task::timer_task_delete()
{
	register dll_t		cur;
	register timer_t	it;
	mach_msg_header_t	msg;
	mach_msg_return_t	msg_retval;
	int			need_msg;

	mutex_lock(&timer_lock);
	cur = dll_first(&timer_queue);
	need_msg = FALSE;
	while (cur != (dll_t)&timer_queue) {
		it = (timer_t)cur;
		if (it->task == this) {
			if (!need_msg && (dll_t)it == dll_first(&timer_queue))
				need_msg = TRUE;
			dll_remove(&timer_queue, it, timer_t, chain);
			mach_object_dereference(it->task);
			Free(it);
		}
		cur = dll_next(cur);
	}
	mutex_unlock(&timer_lock);
	if (need_msg) {
		/*
		 *	Send message to server to wake up because we just
		 *	deleted the timer he is waiting on.
		 */
		bzero(&msg, sizeof(msg));
		msg.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
		msg.msgh_size = sizeof(msg);
		msg.msgh_remote_port = timer_port;
		msg_retval = mach_msg_send(&msg);
		if (msg_retval != MACH_MSG_SUCCESS) {
			mach_error("timer_task_delete: msg_send failed ", 
				msg_retval);
		}
	}
	return(ERR_SUCCESS);
}


/*
 *	Make sure interval timer values are in acceptable range.
 *	Round up times that are too small for clock resolution.
 */
static timer_fix(time_value_t	*tv)
{
	if (tv->seconds < 0 || tv->seconds > TIMER_MAXSEC
	    || tv->microseconds < 0 || tv->microseconds >= TIME_MICROS_MAX)
		return(1);
	else if (tv->seconds == 0 && tv->microseconds > 0
		 && tv->microseconds < TICK)
		tv->microseconds = TICK;
	return(0);
}

static timer_rel_to_abs(
	register time_value_t	*t)
{
	time_value_t		tv;

	if (mach_get_time(&tv) != -1) {
		t->seconds += tv.seconds;
		t->microseconds += tv.microseconds;
		if (t->microseconds >= TIME_MICROS_MAX) {
			t->microseconds -= TIME_MICROS_MAX;
			t->seconds++;
		}
	}
	else {
		ERROR((Diag, "timer_rel_to_abs: mach_get_time failed\n"));
	}
}

static timer_abs_to_rel(
	register time_value_t	*t)
{
	time_value_t		tv;

	if (mach_get_time(&tv) != -1) {
		t->seconds -= tv.seconds;
		t->microseconds -= tv.microseconds;
		if (t->microseconds < 0) {
			t->microseconds += TIME_MICROS_MAX;
			t->seconds--;
		}
	}
	else {
		ERROR((Diag, "timer_abs_to_rel: mach_get_time failed\n"));
	}
}

/*
 *	Find the interval timer record with the given task and ID
 */
timer_addr_t tm_task::timer_find(
	timer_id_t	id)
{
	register dll_t		cur;
	register timer_t	found_it, it;

	found_it = 0;
	mutex_lock(&timer_lock);
	cur = dll_first(&timer_queue);
	while (cur != (dll_t)&timer_queue && found_it == 0) {
		it = (timer_t)cur;
		if ((it->task == this) && (it->id == id))
			found_it = it;
		else {
			cur = dll_next(cur);
		}
	}
	mutex_unlock(&timer_lock);
	return((timer_addr_t)found_it);
}

/*
 *	Insert interval timer record into queue of records sorted in increasing
 *	order by time left until expiration
 */
mach_error_t tm_task::timer_insert(
	timer_t	it)
{
	register dll_t		cur, prev;
	register time_value_t	*cur_time, *new_time;

	/*
	 *	Start at head of queue and find timer to put the new one 
	 *	before
	 */

	mutex_lock(&timer_lock);

	cur = dll_first(&timer_queue);
	while (cur != (dll_t)&timer_queue) {
		cur_time = &((timer_t)cur)->value.time;
		new_time = &it->value.time;
		if (TIMER_CMP(cur_time, new_time, >)) {
			if (cur->prev == &timer_queue) {
				dll_enter_first(&timer_queue, it, timer_t, chain);
			}
			else {
				dll_enter(cur, it, timer_t, chain);
			}
			break;
		}
		else {
			cur = dll_next(cur);
		}
	}
	/*
	 *	If we ended up back at the head, then the new timer goes at
	 *	the end of the queue.
	 */
	if (cur == (dll_t)&timer_queue)
		dll_enter(&timer_queue, it, timer_t, chain);

	mutex_unlock(&timer_lock);

	return(ERR_SUCCESS);
}

/*
 *	Invoke server (called by cthread_fork())
 */
void timer_start(tm_task *atask){timer_start_friend(atask);}
void timer_start_friend(
	tm_task	*atask)
{
	atask->timer_server();
}

/*
 *	Server loop which dequeues the timer at the head of the queue, sleeps
 *	until it expires, resets the timer, requeues it,  and waits for next
 *	timer to expire.  If the queue is empty, we sleep in msg_receive(),
 *	waiting for a message to arrive and signal that the queue is no longer
 *	empty.
 */
mach_error_t tm_task::timer_server()
{
	register dll_t		cur;
	register timer_t	it;
	register int		milliseconds;
	mach_msg_header_t	msg;
	mach_msg_return_t		msg_retval;
	mach_error_t		retval;
	time_value_t		tv;
	register time_value_t	*tvp = &tv;

	bzero(&msg, sizeof(msg));
	while (timer_up) {
		/*
		 *	Get timer off head of queue
		 */
		mutex_lock(&timer_lock);
		cur = dll_first(&timer_queue);
		mutex_unlock(&timer_lock);

		/* set up message header for msg_receive() */
		msg.msgh_bits = MACH_MSGH_BITS_ZERO;
		msg.msgh_local_port = timer_port;
		msg.msgh_size = sizeof(msg);

		if (cur == (dll_t)&timer_queue) {    /* queue empty? */
			/* sleep until we get a message */
			msg_retval = mach_msg_receive(&msg);
			ASSERT_RETCODE("Timer failed to wait on empty queue", msg_retval);
		}
		else {
			mutex_lock(&timer_lock);
			it = (timer_t)cur;
			*tvp = it->value.time;
			timer_abs_to_rel(tvp);
			milliseconds = tvp->seconds * 1000 
					+ tvp->microseconds / 1000;
			mutex_unlock(&timer_lock);
			/*
			 *	Wait for timer to expire
			 */
			msg_retval = MACH_MSG_SUCCESS;	/* init. msg_retval */
			if (milliseconds > 0) {
				msg_retval = 
					mach_msg(&msg,
						MACH_RCV_MSG|MACH_RCV_TIMEOUT,
						0, sizeof(msg),
						timer_port,
						(mach_msg_timeout_t)milliseconds,
						MACH_PORT_NULL);
				if ((msg_retval != MACH_MSG_SUCCESS) &&
				    (msg_retval != MACH_RCV_TIMED_OUT)) {
					mach_error("tm_timer_server: wait a bit",
						msg_retval);
				}
			}

			mutex_lock(&timer_lock);
			cur = dll_first(&timer_queue);

			/*
			 *	Go back to sleep if queue is now empty.
			 */
			if (cur == (dll_t)&timer_queue) {
				mutex_unlock(&timer_lock);
				continue;
			}
			it = (timer_t)cur;
#if MACH3_UNIX
			if (gettimeofday(tvp, &tz) == -1)
				ERROR((msg, "timer_server: gettimeofday failed\n"));
#else
			if (mach_get_time(tvp) == -1)
				ERROR((msg, "timer_server: mach_get_time failed\n"));
#endif MACH3_UNIX
			/*
			 *	If it isn't time to wakeup yet, go back to
			 *	sleep.  Otherwise, signal the task that its
			 *	timer has expired.
			 */
			if (TIMER_CMP(tvp, &it->value.time, <)) {
				mutex_unlock(&timer_lock);
			}
			else {
				retval = it->task->tm_event_to_task_thread(
							MACH_PORT_NULL,
							it->event, it->code,
							it->subcode);
				if (retval  != ERR_SUCCESS) {
					mach_error("tm_event_to_task_thread", retval);
				}
				dll_remove(&timer_queue, it, timer_t, chain);
				mutex_unlock(&timer_lock);
				if (TIMER_ISSET(&it->value.interval)) {
					it->value.time = it->value.interval;
					timer_rel_to_abs(&it->value.time);
					this->timer_insert(it);
				}
				else {
					mach_object_dereference(it->task);
					Free((char *)it);
				}
			}
		}
	}
	return(ERR_SUCCESS);
}


#ifdef	DEBUG
timer_printq()
{
	register dll_t	cur;

	mutex_lock(&timer_lock);

	if (dll_empty(&timer_queue))
		printf("timer_queue empty: prev %x, next %x\n", 
			timer_queue.prev, timer_queue.next);
	else {
		cur = dll_first(&timer_queue);
		printf("timer_queue at %x\n", &timer_queue);
		while (cur != (dll_t)&timer_queue) {
			timer_printrec((timer_t)cur);
			cur = dll_next(cur);
		}
	}

	mutex_unlock(&timer_lock);
}

timer_printrec(it)
	timer_t	it;
{
	printf("\trecord at %x\n", it);
	printf("\t\tprev\t%x\n", it->chain.prev);
	printf("\t\ttm_task\t%x\n", it->task);
	printf("\t\tid\t%x\n", it->id);
	printf("\t\ttime\t%d sec.\t%d usec.\n", it->value.time.seconds,
		it->value.time.microseconds);
	printf("\t\tinterval\t%d sec.\t%d usec.\n", it->value.interval.seconds,
		it->value.interval.microseconds);
	printf("\t\tsignal\t%x\n", it->signal);
	printf("\t\tnext\t%x\n", it->chain.next);
}
#endif	DEBUG
