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
 *
 * $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/interrupt.c,v $
 *
 * C Thread exception package
 *
 * HISTORY:
 * $Log:	interrupt.c,v $
 * Revision 2.4  94/07/07  17:23:24  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  94/06/29  13:41:25  mrt
 * 	change intr_mach_msg to use mach_msg_trap and clearing the "interrupt" option
 * 	bits ourselves, instead of using mach_msg.
 * 	[94/06/29  13:05:45  jms]
 * 
 * 	Last checkin before name change to interrupt.c
 * 	Abort a thread before setting its state, not after.
 * 	[91/02/21  11:21:42  jms]
 * 
 * Revision 2.2  92/07/05  23:27:36  dpj
 * 	First version. Moved here from lib/machobj++.
 * 	[92/05/10  00:54:14  dpj]
 * 
 * Revision 2.2  91/11/06  11:36:06  jms
 * 	Changed to work with C++ objects.
 * 
 * 	This version of lib/machobj++ can only be linked to C++ programs.
 * 	[91/09/27  15:25:30  pjg]
 * 
 * Revision 2.2  91/07/01  14:10:42  jms
 * 	Misc cleanup/bug fixes.
 * 
 * 	Add "intr_reset_self" to reset self after the "self" thread has completed
 * 	a task (eg finished emulating a given syscall, servicing a given request,...)
 * 
 * 	Add "intr_destroy_self" for removal of the interrupt information associated
 * 	with the current thread before destroying the thread.
 * 
 * 	Add "intr_reset" to reset after forking
 * 
 * 	add "intr_set_sync_default" to supply a routine to call when there is no
 * 	handler for synchronious interrupts and one arrives.
 * 	[91/06/24  17:00:50  jms]
 * 
 * 	Initial version
 * 	[91/04/15  17:46:49  jms]
 * 
 * Revision 2.2  90/07/09  17:05:46  dorr
 * 	created.
 * 	[90/03/22  13:57:51  dorr]
 * 
 */

#define	TIMING_GEN


#include <cthreads.h>
#include <base.h>
#include <hash.h>
#include <interrupt.h>

#include <mach/message.h>

#ifdef	TIMING_GEN
	extern mach_msg_header_t*	timing_msg;
#endif	TIMING_GEN

/*
 *	intr handlers:  internally, events within a sensitive
 *	region may be handled by special purpose event handlers
 *
 *	these handlers consist of setjmp buffers that describe the
 *	context that a thread is to be put within if a given class
 *	of exceptions occur.  they are hashed by stack base, since
 *	it assumed that all handlers/exceptions within handlers will
 *	have a fixed stack frame context.   (among other things, setjmp buffers
 *	pretty much insist on working within a given stack frame context).
 *
 *	these are currently set up to be a stack, but it's not really
 *	expected that there will much stacking.  more important is the
 *	differentiation between different types of errors (which won't
 *	typically need to be handled simultaneously).  this can, of course,
 *	all be extended if more complicated needs arise.
 *
 *	(one thing that we might want to consider is use of a general
 *	exception facility for returning errors.  this seems to be a
 *	much nicer model than the return-error-code-through-99-layers
 *	model.)
 */

/*
 *  stackable event handlers.  they consist of a stack
 *  of event handlers and their corresponding types (used
 *  for filtering events)
 */
#define	MAX_INTR_HANDLERS	10
#define	INTR_HASH_SIZE		16

typedef	struct intr_thread_info {
	intr_handler_t		handlers[MAX_INTR_HANDLERS];
	int			next_index;
	int			return_index;
	intr_type_t		pending;
	intr_type_t		masked;
	mach_error_t		sync_error, async_error;
	int			sync_code, async_code;
	int			sync_subcode, async_subcode;
	mach_error_t		(*shake_on_post)();
	int			*shake_info;
} * intr_thread_info_t;

#define INTR_THREAD_INFO_NULL ((intr_thread_info_t)0)
boolean_t intr_routine_top_region = TRUE;

/*
 * Lock for controlling access to the following taskwide data.
 */
static struct mutex intr_lock = MUTEX_INITIALIZER;
#define	LOCK	mutex_lock(&intr_lock);
#define	UNLOCK	mutex_unlock(&intr_lock);
#define	ENDLOCK	mutex_unlock(&intr_lock);

/* Table of thread info and associated cached thread info. XXX hide in cproc?*/
static hash_table_t		intr_thread_info_table = (hash_table_t)0;
static intr_cthread_id_t	intr_last_id;
static intr_thread_info_t	intr_last_thread_info;

/*
 * "intr_sync_default" is called when a synchronious interrupt is posted
 * and there is no handler for sync intrs in the current scope.
 */
static mach_error_t		(*intr_sync_default)() = (mach_error_t (*)())0;
/*	intr_cthread_id_t	intr_cthread_id;
 *	mach_error_t		err;
 *	int			code, subcode;
 */

intr_init()
{
	if (intr_thread_info_table == (hash_table_t)0) {
		intr_thread_info_table = hash_init(0, 0, INTR_HASH_SIZE);
	}
	intr_last_id = INTR_CTHREAD_ID_NULL;
	intr_last_thread_info = NULL;
}

/*
 * intr_set_sync_default(): Set the routine to be called when a synchronious
 * interrupt is posted and there is no current sync handler.
 */
intr_set_sync_default(sync_default_routine)
	mach_error_t	(*sync_default_routine)();
{
	LOCK {
		intr_sync_default = sync_default_routine;
	} ENDLOCK;
}

/*
 * intr_reset:  Remove all of the thread_info structures associated
 *		with the current process.  Useful for reseting
 *		interrupts after forking.
 */
intr_reset()
{
	LOCK {
		hash_apply(intr_thread_info_table, free);
		hash_free(intr_thread_info_table);
		intr_thread_info_table = (hash_table_t)0;
		intr_init();
	} ENDLOCK ;
}

static intr_thread_info_init(thread_info)
	intr_thread_info_t	thread_info;
{
	thread_info->next_index = 0;
	thread_info->return_index = 0;
	thread_info->pending = 0;
	thread_info->sync_error = (mach_error_t)0;
	thread_info->async_error = (mach_error_t)0;
	thread_info->sync_code = 0;
	thread_info->async_code = 0;
	thread_info->sync_subcode = 0;
	thread_info->async_subcode = 0;
	thread_info->shake_on_post = (mach_error_t (*)())0;
	thread_info->shake_info = (int *)0;
}


/*
 * intr_get_thread_info: Get the thread_info for the current thread
 *			cache it in the handler and globally.
 *			lock is assumed to be held at call time
 */
static intr_thread_info_t intr_get_thread_info(id, handler, allocate_new)
	intr_cthread_id_t	id;
	intr_handler_t	handler;
	boolean_t	allocate_new;
{
	/* return from handler cache iff there */
	if ((INTR_HANDLER_NULL != handler) &&
	    (INTR_THREAD_INFO_NULL != (intr_thread_info_t)handler->thread_info))
		return((intr_thread_info_t)(handler->thread_info));

	/* get our id */
	if (INTR_CTHREAD_ID_NULL == id) {
		id = intr_cthread_id_self();
	}

	/* Return from global cache iff there */
	if (intr_last_id == id) {
		if (INTR_HANDLER_NULL != handler)
			(intr_thread_info_t)(handler->thread_info) =
				intr_last_thread_info;
		return(intr_last_thread_info);
	}

	intr_last_id = id;
	intr_last_thread_info = (intr_thread_info_t)
		hash_lookup(intr_thread_info_table, (hash_key_t)intr_last_id);
	
	/* New thread,  allocate new thread info if desired */
	if ((! intr_last_thread_info) && allocate_new) {
		intr_last_thread_info = New(struct intr_thread_info);

		intr_thread_info_init(intr_last_thread_info);
		(void) hash_enter(intr_thread_info_table,
			  (hash_key_t)intr_last_id,
			  (hash_value_t) intr_last_thread_info);
	}

	/* If we have nothing, cleanup and go home */
	if (INTR_THREAD_INFO_NULL == intr_last_thread_info) {
		intr_last_id = INTR_CTHREAD_ID_NULL;
		return(INTR_THREAD_INFO_NULL);
	}

	/* cache it in the handler and return it */
	if (INTR_HANDLER_NULL != handler)
		(intr_thread_info_t)(handler->thread_info) =
			intr_last_thread_info;

	return(intr_last_thread_info);
}

mach_error_t intr_panic(err, code, subcode)
	mach_error_t	err;
	int	code, subcode;
{
	/* what can we do? XXX */
	return(err);
}


/*
 * intr_reset_self: Reset the interrupt info associated with the current
 *	thread.  Used when the processing associated with a given
 *	task is complete (eg. rpc/syscall complete)
 */
mach_error_t intr_reset_self()
{
	LOCK {
		intr_thread_info_t		thread_info;
		
		thread_info = intr_get_thread_info(INTR_CTHREAD_ID_NULL, INTR_HANDLER_NULL, FALSE);
		if (thread_info == INTR_THREAD_INFO_NULL) {
			UNLOCK;
			return(KERN_SUCCESS);
		}
			
		intr_thread_info_init(thread_info);

	} ENDLOCK ;
	return(KERN_SUCCESS);
}

/*
 * intr_destroy_self: Remove the interrupt info associated with the current
 *	thread.
 */
mach_error_t intr_destroy_self()
{
	LOCK {
		intr_thread_info_t		thread_info;
		
		thread_info = intr_get_thread_info(INTR_CTHREAD_ID_NULL, INTR_HANDLER_NULL, FALSE);
		if (thread_info == INTR_THREAD_INFO_NULL) {
			UNLOCK;
			return(KERN_SUCCESS);
		}
		(void) hash_remove(intr_thread_info_table,
					 (hash_key_t)(intr_cthread_id_self()));
		Free(thread_info);
	} ENDLOCK ;
	return(KERN_SUCCESS);
}

mach_error_t intr_enable_handler(handler)
	intr_handler_t		handler;
{
	LOCK {
		intr_thread_info_t		thread_info;
		thread_info = intr_get_thread_info(INTR_CTHREAD_ID_NULL, handler, TRUE);

		handler->index = thread_info->next_index;
		thread_info->handlers[thread_info->next_index++] = handler;

	} ENDLOCK;

	return(KERN_SUCCESS);
}


static mach_error_t intr_disable_handler_internal(handler)
	intr_handler_t		handler;
{
	intr_thread_info_t		thread_info;

	if (0 > handler->index) return KERN_SUCCESS;

	thread_info = intr_get_thread_info(INTR_CTHREAD_ID_NULL, handler, FALSE);
	thread_info->next_index = handler->index;
	if (0 == thread_info->next_index) {
		/* handler free */
		thread_info->return_index = 0;
	}
	else {
		/* Adjust the return index to be that of the "top" handler */
		thread_info->return_index = 
			((thread_info->handlers[thread_info->next_index -1])->
				return_index);
	}

	return(KERN_SUCCESS);
}

mach_error_t intr_disable_handler(handler)
	intr_handler_t	handler;
{
	mach_error_t	ret;

	LOCK {
		ret = intr_disable_handler_internal(handler);
	} ENDLOCK;

	return(ret);
}

/*
 * intr_enter_handler:  Enter a handler,  it is assumed that the interrupt
 * table is locked when this routine is called and will be unlocked by the 
 * caller. Return false iff we are currently handling an interrupt in question.
 */
static boolean_t intr_enter_handler(handler, interrupt)
	intr_handler_t		handler;
	intr_type_t		interrupt;
{
	intr_type_t		intr;
	intr_thread_info_t	thread_info = 
					intr_get_thread_info(INTR_CTHREAD_ID_NULL, handler,TRUE);

	/* 
	 * Panic if we are already handling a INTR_SYNC interrupt
	 * and we get another one
	 */
	if (INTR_SYNC & thread_info->pending & thread_info->masked) {
		/* Panic: Gotta a sync_error durring a sync handler */
		printf("PANIC: got a sync interrupt while handling one.  Suicide.\n");
		task_terminate(mach_task_self());
	}
	
	/* if sync interrupt, only do sync */
	intr = (interrupt & INTR_SYNC) ? INTR_SYNC : INTR_ASYNC;

	/* 
	 * if we are already in a async handler and have a async intr, 
	 * then we cannot do it now.
	 */
	if (INTR_ASYNC & intr & thread_info->masked) {
		return(FALSE);
	}

	/* setup handler user info */
	handler->abort_return = 0;
	handler->intr_type = intr;
	if (INTR_SYNC & intr) {
		handler->error = thread_info->sync_error;
		handler->code = thread_info->sync_code;
		handler->subcode = thread_info->sync_subcode;
	}
	else {
		handler->error = thread_info->async_error;
		handler->code = thread_info->async_code;
		handler->subcode = thread_info->async_subcode;
	}

	thread_info->masked |= intr;	/* Mark it as masked */

	/* Clear it from pending */
	thread_info->pending &= (~ intr);
	if (INTR_SYNC & intr) {
		thread_info->sync_error = (mach_error_t)0;
		thread_info->sync_code = 0;
		thread_info->sync_subcode = 0;
	}
	else {
		thread_info->async_error = (mach_error_t)0;
		thread_info->async_code = 0;
		thread_info->async_subcode = 0;
	}
	return(TRUE);
}

/*
 * intr_exit_handler: Lets exit a handler normally, Not a INTR_RETURN.
 */
static intr_exit_handler(handler)
	intr_handler_t		handler;
{
	LOCK {
		intr_thread_info_t	thread_info =
			intr_get_thread_info(INTR_CTHREAD_ID_NULL, handler, FALSE);

		thread_info->masked &= (~ handler->intr_type);

		/* If we got more while handling, then do it as IMMEDIATE */
		if (thread_info->pending) {
			intr_deliver_interrupt_internal(mach_thread_self(), 
				INTR_IMMEDIATE, thread_info->pending,
				thread_info) ;
		}
	} ENDLOCK;
}

/*
 * intr_new_routine(): retister that the thread has entered a new routine
 *	where intr_regions are expected.
 */
intr_new_routine()
{
	LOCK {
		intr_thread_info_t	thread_info =
			intr_get_thread_info(INTR_CTHREAD_ID_NULL, INTR_HANDLER_NULL, FALSE);

		if (thread_info) 
			thread_info->return_index = thread_info->next_index;
	} ENDLOCK;
}

/*
 * intr_return(): Pop the handlers associated with the current routine.
 *		Note: May also be returning from within a hander.
 */
intr_return(handler)
{

	LOCK {
		intr_thread_info_t	thread_info = 
			intr_get_thread_info(INTR_CTHREAD_ID_NULL, handler, FALSE);

		if (! thread_info) return;
		
		/* Unpend and clear the masked intrs for leaving handler(s) */
		thread_info->pending &= (~ thread_info->masked);
		thread_info->masked = INTR_NULL;

		if (thread_info->next_index == thread_info->return_index) {
			/* No handlers added in this routine */
			thread_info->return_index = 
			 ((thread_info->handlers[thread_info->next_index -1])->
				return_index);
		}
		else {
			/* Handlers added in this routine, disable them */
			intr_disable_handler_internal(
			    thread_info->handlers[thread_info->return_index],
			    thread_info);
		}
	} ENDLOCK;
}


/*
 * intr_deliverable_region: Return the subtype of the given region
 * that could be delivered to by any of the given intr_types using the
 * given delivery_types.
 */
static intr_rtype_t intr_deliverable_region(intr_types, delivery_types, region_type)
	intr_type_t	intr_types;
	intr_dtype_t	delivery_types;
	intr_rtype_t	region_type;
{
	intr_rtype_t app_region = 
			INTR_APPLICABLE_REGION(intr_types, region_type);

	if (INTR_ASYNC_BLOCKED & app_region) {
		/* Remove async since blocked */
		app_region &= (intr_rtype_t)(~ INTR_ASYNC);
	}
	else {
		if (! (INTR_BOUNDS & delivery_types)) {
			/* Remove INTR_ASYNC_BOUNDS, not a bounds delivery */
			app_region &= (~ INTR_ASYNC_BOUNDS);
		}
		if (! (INTR_WITH_TEST & delivery_types)) {
			/* Remove INTR_WITH_TEST, not a test delivery */
			app_region &= (intr_rtype_t)(~ (INTR_WITH_TEST<<4));
		}
	}

	return (app_region);
}

/*
 * intr_pending:  Would the handler supplied be called for the currently
 *			pending interrupt(s)?
 */

boolean_t intr_pending(hand, enter_handler) 
	intr_handler_t		hand;
	boolean_t		enter_handler;
{
	boolean_t		ret = FALSE;

	LOCK {
		intr_thread_info_t	thread_info = 
			intr_get_thread_info(INTR_CTHREAD_ID_NULL, hand, FALSE);

		if (thread_info == INTR_THREAD_INFO_NULL) {
			UNLOCK;
			return(FALSE);
		}

		/* Return if we are not pending on this region */
		if (! intr_deliverable_region(thread_info->pending,
				INTR_DALL, hand->region_type)) {
			UNLOCK;
			return(FALSE);
		}

		if (enter_handler) {
			intr_enter_handler(hand, thread_info->pending);
		}

	} ENDLOCK;
	return(TRUE);
}

/*
 *	intr_deliver_interrupt:  
 *	Find the appropriate handler for the given interrupt/delivery type.
 *	Call it unless: 
 *		it is a INTR_BLOCKED handler
 *		it's a INTR_POINT handler with an attempt of immediate delivery
 *	
 */
mach_error_t intr_deliver_interrupt_internal(intr_cthread_id, delivery_type, interrupt_type, thread_info) 
	intr_cthread_id_t	intr_cthread_id;
	intr_dtype_t		delivery_type;
	intr_type_t		interrupt_type;
	intr_thread_info_t	thread_info;	/* info for a specific thread */
{
	intr_handler_t		hand = INTR_HANDLER_NULL;
	intr_rtype_t		app_region;
	int			i;
	thread_t		thread;
	mach_error_t		ret;

	if (thread_info->next_index == 0) {
		/* No handlers die */
		return KERN_FAILURE;
	}
	
	/* Try to find one which fits the interrupt_type */
	for (i = thread_info->next_index - 1; i < 0; i--) {
		if (INTR_APPLICABLE_REGION(interrupt_type,
				thread_info->handlers[i]->region_type)) {
			hand = thread_info->handlers[i];
		}
	}
	/* Did we find one?  If not fail */
	if (INTR_HANDLER_NULL == hand) {
		if ((INTR_SYNC & interrupt_type) && intr_sync_default) {
			ret = (*intr_sync_default)(
				intr_cthread_id,
				thread_info->sync_error,
				thread_info->sync_code,
				thread_info->sync_subcode);
			return(ret);
		}
		else {
			return(KERN_FAILURE);
		}
	}
	
	/* Can we not deliver to this handler?.  Not an error, just stop */
	if (! (app_region = intr_deliverable_region(interrupt_type,
				delivery_type, hand->region_type))) {
		return 0;
	}

	/*
	 * We have a handler for delivery.  Prepare for it and deliver.
	 */
	if (! intr_enter_handler(hand, interrupt_type)) {
		/* Currently handling  interrupt, can't deliver */
		return(0);
	}

	/* Handler can handle itself. */
	intr_disable_handler_internal(hand, thread_info);

	/* Call the handler */
	thread = intr_cthread_id_to_thread(intr_cthread_id);
	if (mach_thread_self() == thread) {
		/* We are doing this to ourself */
		longjmp(hand->jmpbuf, interrupt_type);
	}
	else {
		/* We are delivering to another thread */
		mach_error_t		ret;

		thread_suspend(thread);
		ret = cthread_do_handler(thread, hand);
		thread_resume(thread);

		return(ret);
	}
}

mach_error_t intr_deliver_interrupt(intr_cthread_id, delivery_type, interrupt_type) 
	intr_cthread_id_t	intr_cthread_id;
	intr_dtype_t		delivery_type;
	intr_type_t		interrupt_type;
{
	mach_error_t		ret = KERN_SUCCESS;
	LOCK {
		intr_thread_info_t	thread_info = 
			intr_get_thread_info(intr_cthread_id, INTR_HANDLER_NULL, FALSE);

		if (INTR_THREAD_INFO_NULL != thread_info)
			ret = intr_deliver_interrupt_internal(
				intr_cthread_id, delivery_type, interrupt_type, 
				thread_info);
	} ENDLOCK;
	return(ret);
}

/*
 * intr_post_interrupt: Mark an interrupt as having occured and
 *	attempt to deliver it.
 */
mach_error_t intr_post_interrupt(id, delivery_type, interrupt_type, 
			error, code, subcode)
	intr_cthread_id_t		id;
	intr_dtype_t		delivery_type;
	intr_type_t		interrupt_type;
	mach_error_t		error;
	int 			code, subcode;
{
	mach_error_t		ret = KERN_SUCCESS;

	LOCK {
		intr_thread_info_t	thread_info = 
			intr_get_thread_info(id, INTR_HANDLER_NULL, TRUE);

		/* Mark it as pending, if not already pending */
		if (! (thread_info->pending & interrupt_type)) {
			thread_info->pending |= interrupt_type;
			if (INTR_SYNC & interrupt_type) {
				thread_info->sync_error = error;
				thread_info->sync_code = code;
				thread_info->sync_subcode = subcode;
			}
			else {
				thread_info->async_error = error;
				thread_info->async_code = code;
				thread_info->async_subcode = subcode;
			}
		}

		/* Do the "shake_on_post" if needed */
		if ((thread_info->shake_on_post) &&
		    (interrupt_type & (~ thread_info->masked)))	{
			(*(thread_info->shake_on_post))(thread_info->shake_info);
		}

		if (!((INTR_BLOCKED & delivery_type) ||
		      (INTR_DNULL == delivery_type))) {
			ret = intr_deliver_interrupt_internal(id, INTR_IMMEDIATE, 
					interrupt_type, thread_info);
		}

	} ENDLOCK;
	return(ret);
}

/*
 * intr_shake_on_post: Cause the given call to be invoked with the info
 * 	value when an interrupt is posted to the current thread
 *	or if one was pending when this routine is called.
 */
static mach_error_t intr_shake_on_post(call, shake_info)
	mach_error_t	(*call)();
	int		*shake_info; 
{
	mach_error_t	ret = ERR_SUCCESS;
	LOCK {
		intr_thread_info_t	thread_info = 
			intr_get_thread_info(INTR_CTHREAD_ID_NULL, INTR_HANDLER_NULL, TRUE);
		thread_info->shake_on_post = call;
		thread_info->shake_info = shake_info;
		
		/* Call it if we are pending (and not masked) */
		if (thread_info->pending & (~thread_info->masked)) {
			ret = (*call)(shake_info);
		}
	} ENDLOCK;
	return(ret);
}
/*
 * intr_clear_shake_on_post: Call the given routine on the shake_info and
 *		prevent clear any previous shakes on post.
 */
static mach_error_t intr_clear_shake_on_post(call)
	mach_error_t	(*call)();
{
	mach_error_t	ret = ERR_SUCCESS;
	LOCK {
		intr_thread_info_t	thread_info = 
			intr_get_thread_info(INTR_CTHREAD_ID_NULL, INTR_HANDLER_NULL, TRUE);
		/* if not shak'n, do noth'n */
		if (! thread_info->shake_on_post) {
			UNLOCK;
			return(ERR_SUCCESS);
		}

		if (call) {
		    ret = (*call)(thread_info->shake_info);
		}

		thread_info->shake_on_post = (int (*)())0;
		thread_info->shake_info = (mach_error_t *)0;
	} ENDLOCK;
	return(ret);
}

/*
 * The following code is used to support asyncroniously interruptable
 * mach_msg invocations.
 */
typedef struct intr_msg_shake_info {
	boolean_t		shake_occured;
	intr_cthread_id_t	intr_cthread_id;
	mach_msg_header_t	*msg;
	mach_msg_option_t	option;
	mach_port_t		remote_port;
	mach_error_t		ret;
} *intr_msg_shake_info_t;

/*
 * intr_msg_shake:  Called when an interrupt is delivered to the thread
 *	doing a intr_mach_msg call.
 *	If it's only a receive and we don't have a remote port then
 *		thread_abort it.  If we still don't have a remote port
 *		then we the receive did not complete and we should deliver
 *		the interrupt (skip to the handler)
 *	Otherwise it is a send/send-receive.  In this case thread_abort
 *		it and set the remote port such that the mach_msg will fail
 *		if it has not yet occured.
 */
static mach_error_t intr_msg_shake(shake_info) 
	intr_msg_shake_info_t	shake_info;
{
	if (shake_info->shake_occured) {
		return(ERR_SUCCESS);
	}
	shake_info->shake_occured = TRUE;

	if (shake_info->option & MACH_SEND_MSG) {
		thread_t	thread = 
			intr_cthread_id_to_thread(shake_info->intr_cthread_id);
		/*
		 * We are shaking one that would send
		 */

		/* Give it a shake, thead is assumed suspended if not current thread */
		if (mach_thread_self() != thread) {
			thread_abort(thread);
		}

		/* Bad remote port so that a following mach_msg would fail */
		shake_info->remote_port = shake_info->msg->msgh_remote_port;
		shake_info->msg->msgh_remote_port = MACH_PORT_NULL;

	}
	else if (MACH_PORT_NULL == shake_info->msg->msgh_remote_port) {
		thread_t	thread = intr_cthread_id_to_thread(shake_info->intr_cthread_id);
		/*
		 * Shake it and see if we get a remote port (finishes)
		 */
		if (mach_thread_self() != thread) {
			thread_abort(thread);
		}

		/* 
		 * If we still have no remote port, jump to the handler
		 * else just keep rolling
		 */
		if (MACH_PORT_NULL == shake_info->msg->msgh_remote_port) {
			intr_thread_info_t	thread_info = 
				intr_get_thread_info(shake_info->intr_cthread_id, INTR_HANDLER_NULL, TRUE);
			intr_deliver_interrupt_internal(shake_info->intr_cthread_id,
				    INTR_WITH_TEST, INTR_ASYNC, thread_info);
		}

	}

	return (ERR_SUCCESS);
}

/*
 * intr_clear_msg_shake: Stop shak'n for a mach_msg call.  We are past it.
 *			cleanup whatever damage shaking may have done.
 */
static mach_error_t intr_clear_msg_shake(shake_info)
	intr_msg_shake_info_t	shake_info;
{
	if (! shake_info->shake_occured) {
		return(ERR_SUCCESS);
	}

	if (shake_info->option & MACH_SEND_MSG) {
		/* cleanup after the bad remote_port hack */		

		if (MACH_SEND_INVALID_DEST == shake_info->ret) {
			shake_info->ret = MACH_SEND_INTERRUPTED;
		}
		shake_info->msg->msgh_remote_port = shake_info->remote_port;
	}
	return(ERR_SUCCESS);
}

/*
 * intr_mach_msg:  An asynchroniously interruptable form of mach_msg.
 *			this routine is used just like mach_msg except that
 *			it is interrupted when an INTR_ASYNC occurs.
 *			it returns the same interrupt values that mach_msg
 *			normally returns.
 */
mach_error_t
intr_mach_msg(msg, option, send_size, rcv_size, rcv_name, timeout, notify)
	mach_msg_header_t	*msg;
	mach_msg_option_t	option;
	mach_msg_size_t		send_size;
	mach_msg_size_t		rcv_size;
	mach_port_t		rcv_name;
	mach_msg_timeout_t	timeout;
	mach_port_t		notify;
{
	mach_error_t			ret = ERR_SUCCESS;
	struct intr_msg_shake_info	shake_info;
	
	shake_info.shake_occured = FALSE;
	shake_info.msg = msg;
	shake_info.remote_port = MACH_PORT_NULL;
	shake_info.intr_cthread_id = intr_cthread_id_self();
	shake_info.option = option;

	if (option & MACH_SEND_MSG) {
		/* We have a send and receive */
		INTR_REGION(INTR_ASYNC_BOUNDS)
			intr_shake_on_post(intr_msg_shake, &shake_info);
#ifdef	TIMING_7
			if (timing_msg != 0)
				ret = KERN_SUCCESS;
			else
#endif	TIMING_7
			/* mach_msg_trap does not need/use/want the "interrupt options" */
			option &= ~(MACH_RCV_INTERRUPT | MACH_SEND_INTERRUPT);
			ret = mach_msg_trap(msg, 
				option,
				send_size, rcv_size, rcv_name,
				timeout, notify);
			intr_clear_shake_on_post(intr_clear_msg_shake);
		INTR_HANDLER
			intr_clear_shake_on_post(intr_clear_msg_shake);
		INTR_END
	}
	else {
		/* we are just doing a receive */
		msg->msgh_remote_port = MACH_PORT_NULL;
		INTR_REGION(INTR_ASYNC_TEST)
			intr_shake_on_post(intr_msg_shake, &shake_info);
			ret = mach_msg(msg, 
				option|MACH_SEND_INTERRUPT|MACH_RCV_INTERRUPT,
				send_size, rcv_size, rcv_name, 
				timeout, notify);
			intr_clear_shake_on_post(intr_clear_msg_shake);
		INTR_HANDLER
			intr_clear_shake_on_post(intr_clear_msg_shake);
			ret = MACH_RCV_INTERRUPTED;
		INTR_END
	}
	return(ret);
}


/*
 * The following code is used to support asyncroniously interruptable
 * cond_wait invocations.
 */
typedef struct intr_cond_wait_shake_info {
	boolean_t	shake_occured;
	condition_t	cond;
	mutex_t		lock;
} *intr_cond_wait_shake_info_t;

/*
 * intr_cond_wait_shake: shake a condition wait at post time
 *		by signaling the condition.
 */
static mach_error_t intr_cond_wait_shake(shake_info) 
	intr_cond_wait_shake_info_t	shake_info;
{
	if (shake_info->shake_occured) {
		return(ERR_SUCCESS);
	}
	shake_info->shake_occured = TRUE;

	mutex_lock(shake_info->lock);
	mutex_unlock(shake_info->lock);
	cond_signal(shake_info->cond);

	return (ERR_SUCCESS);
}

/*
 * intr_clear_cond_wait_shake: time to stop shak'n
 */
static mach_error_t intr_clear_cond_wait_shake(shake_info)
	intr_cond_wait_shake_info_t	shake_info;
{
	return(ERR_SUCCESS);
}

/*
 * intr_cond_wait:  a form of condition wait that can be interrupted
 *		by INTR_ASYNC interrupts.  It used the same way as
 *		a normal condition wait except that the "condition
 *		loop" cannot be in a region where INTR_ASYNC interrupts
 *		are delivered immediately.
 */
mach_error_t
intr_cond_wait(cond, lock)
	condition_t	cond;
	mutex_t		lock;
{
	mach_error_t	ret = ERR_SUCCESS;
	struct intr_cond_wait_shake_info	shake_info;
	
	shake_info.shake_occured = FALSE;
	shake_info.cond = cond;
	shake_info.lock = lock;

	INTR_REGION(INTR_ASYNC_BOUNDS)
		intr_shake_on_post(intr_cond_wait_shake, &shake_info);
		condition_wait(cond, lock);
		intr_clear_shake_on_post(intr_clear_cond_wait_shake);
	INTR_HANDLER
		intr_clear_shake_on_post(intr_clear_cond_wait_shake);
		ret = INTR_EVENT;
	INTR_END
	return(ret);
}
