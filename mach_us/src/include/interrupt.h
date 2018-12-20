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
 * interrupt.h
 *
 * Mechanism for interrupting code execution at points/regions defined by
 *  the code being interrupted.
 *
 * HISTORY
 * $Log:	interrupt.h,v $
 * Revision 2.5  94/07/08  15:51:15  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/05  23:23:09  dpj
 * 	Added extern declarations for C++.
 * 	[92/05/10  00:16:47  dpj]
 * 
 * Revision 2.3  91/07/03  19:00:39  jms
 * 	Comment fixes.
 * 
 * Revision 2.2  91/07/01  14:05:53  jms
 * 	Remove "RE_REGIONS" just needles complication.
 * 
 * 	Add intr_set_sync_default() to define a routine to be called for sychroniously
 * 	generated interrupts when no handler is set.
 * 
 * 	Add intr_reset to reset the intrrupt stuff after forking.
 * 	[91/06/24  15:17:44  jms]
 * 
 * 	Initial Version
 * 	[91/04/15  16:58:55  jms]
 * 
 */

#ifndef	_INTERRUPT_
#define	_INTERRUPT_ 1


#include	<setjmp.h>
#include	<mach_error.h>
#include	<cthreads.h>
#include	<exception_error.h>

/*
 * Declare the types of interrupts (interupt generation)
 */
typedef int intr_type_t;
#define	INTR_SYNC		((intr_type_t)0xf)
#define	INTR_ASYNC		((intr_type_t)0xf0)


/*
 * Declare the different ways interrupts can be delivered
 */
typedef int intr_dtype_t;
#define INTR_IMMEDIATE		((intr_dtype_t)0x1)
    /* Deliver before executing further instructions in the target thread */

#define INTR_BLOCKED		((intr_dtype_t)0x2)
    /* Don't deliver.  Hold as pending (overrides others) */

#define INTR_BOUNDS		((intr_dtype_t)0x4)
    /* Deliver at beginning and end of region */

#define INTR_WITH_TEST		((intr_dtype_t)0x8)
    /* Arbitrary test points enabled */


/*
 * Declare the types if interrupt handlers/regions 
 * These may be OR'd together for "combined" handling.
 */
typedef int intr_rtype_t;
#define INTR_SYNC_NULL		INTR_RNULL
#define INTR_SYNC_IMMEDIATE	((intr_rtype_t)(INTR_IMMEDIATE))

#define INTR_ASYNC_NULL		INTR_RNULL
#define	INTR_ASYNC_IMMEDIATE	((intr_rtype_t)(INTR_IMMEDIATE<<4))
#define	INTR_ASYNC_BOUNDS	((intr_rtype_t)(INTR_BOUNDS<<4))
#define	INTR_ASYNC_TEST		((intr_rtype_t)((INTR_BOUNDS|INTR_WITH_TEST)<<4))
#define	INTR_ASYNC_BLOCKED	((intr_rtype_t)(INTR_BLOCKED<<4))

/* All or nothing */
#define INTR_ALL		((intr_type_t)(-1))
#define INTR_DALL		((intr_dtype_t)(-1))
#define INTR_RALL		((intr_rtype_t)(-1))
#define INTR_NULL		((intr_type_t)0)
#define INTR_DNULL		((intr_dtype_t)0)
#define INTR_RNULL		((intr_rtype_t)0)

/*
 * INTR_APPLICABLE_REGION: Return the sub_region of the region_type 
 *	applicable to  the given interrupt(s).
 */
#define INTR_APPLICABLE_REGION(intr_types, region_type) \
	(((intr_rtype_t)(intr_types)) & (region_type))

/* Misc */
#define INTR_IS_IMMEDIATE (INTR_SYNC_IMMEDIATE | INTR_ASYNC_IMMEDIATE)

typedef struct intr_handler {
	jmp_buf		jmpbuf;
	char		*thread_info;	/* Opaque pointer to thread specific interupt info */
	int		index;
	intr_rtype_t	region_type;
	int		do_handler;
	int		got_jmpbuf;
	int		return_index;

	/* Available for use by the handler */
	intr_type_t	intr_type;
	mach_error_t	error;
	int		code;
	int		subcode;
	mach_error_t	abort_return;
} * intr_handler_t;
#define INTR_HANDLER_NULL	((intr_handler_t)0)

extern boolean_t intr_routine_top_region;

/*
 * Macros for use when defining where one wishes to interrupt code.
 */


/*
 * Declare the start of a region where the associate interrupt handler will
 * execute as soon as an interrupt of the correct type occurs or immediately
 * if one is already marked/posted. used as follows:
 *
 *	INTR_REGION(INTR_SYNC_IMMEDIATE | INTR_ASYNC_TEST)
 *		/% Note: INTR_TEST for INTR_ASYNC_TEST. This region only %/
 *		.
 *		.
 *		.
 *		/%
 *		 % Invoke handler "here" for innermost INTR_ASYNC_TEST 
 *		 % region iff INTR_ASYNC is marked/pending.
 *		 %/
 *		INTR_TEST;
 *		.
 *		.
 *		.
 *	INTR_HANDLER	
 *		/% Handle interrupts.  Optional when "blocked"  %/
 *
 *		intr_typr_t 	type;
 *		mach_error_t	error;
 *		int 		code, subcode;
 *		INTR_INFO(&type, &error, &code, &subcode); /% Optional %/
 *		.
 *		.
 *		.
 *		/% 
 *		 % Optional return from the routine,
 *		 % NOTE: Normal "return" not valid from inside of INTR_HANDLER
 *		 % or INTR_REGION !!!
 *		 %/
 *		INTR_RETURN(EXCPT_SOFTWARE);
 *	INTR_END
 */

/*
 * INTR_REGION(region_type): Start an interupt region.
 */
#define INTR_REGION(rtype)  { \
    if (intr_routine_top_region) intr_new_routine(); \
    { \
	struct intr_handler	intr_hand_rec; \
	boolean_t		intr_routine_top_region = FALSE; \
	intr_hand_rec.got_jmpbuf = FALSE; \
	intr_hand_rec.thread_info = (char *)0; \
	intr_hand_rec.region_type = (rtype); \
	intr_hand_rec.index = -1; \
	intr_hand_rec.return_index = -1; \
	intr_hand_rec.do_handler = FALSE; \
	if (intr_pending(((intr_handler_t)(&intr_hand_rec)), TRUE)) { \
	    intr_hand_rec.do_handler = TRUE; \
	} else { \
	    if ((! intr_hand_rec.got_jmpbuf) && \
		(((rtype) | INTR_IS_IMMEDIATE) || \
		 ((rtype) & INTR_ASYNC_TEST))) { \
		intr_hand_rec.do_handler = \
		    _setjmp(intr_hand_rec.jmpbuf); \
		intr_hand_rec.got_jmpbuf = TRUE; \
	    } \
	} \
	if (! intr_hand_rec.do_handler) { \
	    intr_enable_handler(((intr_handler_t)(&intr_hand_rec)));\
	    { \

/*
 * INTR_TEST: Put a place for invoking a INTR_ASYNC_TEST handler "here".
 */
#define INTR_TEST \
		intr_deliver_interrupt(thread_self(), \
		    INTR_WITH_TEST, INTR_ASYNC)

/*
 * INTR_HANDLER: The beginning of the handler for a region.  Also supplies
 * and interrupt point but only for this region.
 */
#define INTR_HANDLER \
	    } \
	} \
	if (intr_hand_rec.do_handler || \
	    intr_pending(((intr_handler_t)(&intr_hand_rec)), TRUE)) { \
	    { \

/*
 * INTR_RETURN(return_value): The only valid way to return from the inside of
 * a handler or region.
 */
#define INTR_RETURN(val) \
		intr_return(((intr_handler_t)(&intr_hand_rec))); \
		return((val))

/*
 * INTR_END: How to end a INTR_REGION
 */
#define INTR_END \
		intr_exit_handler(((intr_handler_t)(&intr_hand_rec))); \
	    } \
	} \
	intr_disable_handler(((intr_handler_t)(&intr_hand_rec))); \
    }}

/*
 * INTR_INFO(intr_type_t *intr_type, mach_error_t *error, int *code, int *subcode)
 *  Get information about the interrupt being delivered.  Only to be used 
 *  from inside a INTR_HANDLER
 */
#define INTR_INFO(_intr_type, _error, _code, _subcode) \
	(*(_intr_type)) = intr_hand_rec.intr_type; \
	(*(_error)) = intr_hand_rec.error; \
	(*(_code)) = intr_hand_rec.code; \
	(*(_subcode)) = intr_hand_rec.subcode
    
/*
 * Access specific info about the interrupt.  Again, only valid from inside of
 * a INTR_HANDLER
 */
#define INTR_TYPE		(intr_hand_rec.intr_type)
#define INTR_ERROR		(intr_hand_rec.error)
#define INTR_EVENT		INTR_ERROR
#define INTR_CODE		(intr_hand_rec.code)
#define INTR_SUBCODE		(intr_hand_rec.subcode)
#define INTR_ABORT_RETURN	(intr_hand_rec.abort_return)

/*
 * Interrupt thread identification stuff (Id of who to interrupt)
 * XXX These definitions know entirely too much about cthreads
 */
typedef	vm_offset_t	intr_cthread_id_t;
#define INTR_CTHREAD_ID_NULL	((intr_cthread_id_t)0)

extern vm_offset_t	cthread_thread_stack();
extern int		cthread_bind(/* ur_cthread_t ur */);

#define intr_cthread_id_self() \
		((intr_cthread_id_t)(cthread_sp()&cthread_stack_mask))
#define intr_thread_to_cthread_id(thread) \
		((intr_cthread_id_t)(cthread_thread_stack((thread))))
#define intr_cthread_id_to_thread(id) \
		((thread_t)(cthread_bind(* ((ur_cthread_t *)(id)))))

/*
 * Initialize the interrupt mechinism.
 */
extern intr_init();

/*
 * intr_set_sync_default(): Set the routine to be called when a synchronous
 * interrupt is posted and there is no current sync handler.
 */
extern intr_set_sync_default(/* sync_default_routine */);
/* mach_error_t sync_default_routine(intr_cthread_id, err, code, subcode)
 *	intr_cthread_id_t	intr_cthread_id;
 *	mach_error_t		err;
 *	int			code, subcode;
 */

/*
 * intr_reset:  Remove all of the thread_info structures associated
 *		with the current process.  Useful for reseting
 *		interrupts after forking.
 */
extern intr_reset();

/*
 * intr_reset_self(): reset the current thread clearing it of any pending 
 *	      interrupts. Useful when the code to be interrupted (the syscall) 
 *            had completed.
 */
extern intr_reset_self();

/*
 * intr_destroy_self: Remove the interrupt info associated with the current
 *	thread.
 */
extern mach_error_t intr_destroy_self();

/*
 * intr_post_interrupt: Mark an interrupt as having occured and
 *	attempt to deliver it.
 */
extern mach_error_t intr_post_interrupt(/* intr_cthread_id, delivery_type, 
			interrupt_type, error, code, subcode */);
/*
 *	intr_cthread_id_t	intr_cthread_id;
 *	intr_dtype_t		delivery_type;	/%Attempt immediate delivery?%/
 *	intr_type_t		type;
 *	mach_error_t		error;
 *	int             code, subcode;
 */
/*
 * intr_mach_msg:  An asynchroniously interruptable form of mach_msg.
 *			this routine is used just like mach_msg except that
 *			it is interrupted when an INTR_ASYNC occurs.
 *			it returns the same interrupt values that mach_msg
 *			normally returns.
 */
extern mach_error_t
intr_mach_msg(/* msg, option, send_size, rcv_size, rcv_name, timeout, notify */);
/*
 *	mach_msg_header_t	msg;
 *	mach_msg_option_t	option;
 *	mach_msg_size_t		send_size;
 *	mach_msg_size_t		rcv_size;
 *	mach_port_t		rcv_name;
 *	mach_msg_timeout_t	timeout;
 *	mach_port_t		notify;
 */

/*
 * intr_cond_wait:  a form of condition wait that can be interrupted
 *		by INTR_ASYNC interrupts.  It used the same way as
 *		a normal condition wait except that the "condition
 *		loop" cannot be in a region where INTR_ASYNC interrupts
 *		are delivered immediately.  The return value is the
 *		INTR_EVENT/INTR_ERROR value for the interrupt else,
 *		ERR_SUCCESS.
 */
extern mach_error_t
intr_cond_wait(/*cond, lock*/);
/*
 *	condition_t	cond;
 *	mutex_t		lock;
 */

/* Routines for use by the supplied macros ONLY */
extern mach_error_t intr_enable_handler();
extern mach_error_t intr_disable_handler();
extern intr_exit_handler();
extern boolean_t intr_pending() ;
extern mach_error_t intr_deliver_interrupt();
#endif	_INTERRUPT_
