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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxsignal.cc,v $
 *
 * Purpose: Unix style exceptions
 *
 * HISTORY: 
 * $Log:	uxsignal.cc,v $
 * Revision 2.8  94/07/08  16:02:21  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.7  94/05/17  14:08:49  jms
 * 	Need dummy implementations of virtual methods in class 
 * 		uxsignal for 2.3.3 g++ -modh
 * 	[94/04/28  19:00:52  jms]
 * 
 * Revision 2.6.1.1  94/02/18  11:33:57  modh
 * 	Need implementations of virtual methods in class uxsignal for 2.3.3 g++
 * 
 * 
 * Revision 2.6  93/01/20  17:38:57  jms
 * 	Add a thread abort right after the thread suspend.  Used to shrink window
 * 	of pagein vulnerability.  Assumes that thread_abort will not need to pagein
 * 	something that is currently being paged in.  Hope thread abort gets better
 * 	in later kernels.
 * 	[93/01/18  17:22:58  jms]
 * 
 * Revision 2.5  92/07/05  23:32:57  dpj
 * 	Make ther remote class for a usEvent be usEvent_proxy
 * 	[92/06/24  17:05:52  jms]
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:32:10  dpj]
 * 
 * Revision 2.4  92/03/05  15:07:35  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:47:51  jms]
 * 
 * Revision 2.3  91/11/13  17:19:04  dpj
 * 	Added a C entry point for signal_reset().
 * 	[91/11/12  17:56:44  dpj]
 * 
 * Revision 2.2  91/11/06  14:12:49  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:20:49  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:11:42  pjg]
 * 
 * 	temp declarations of unix error sub and mach exception sub.
 * 	[90/01/02  14:21:50  dorr]
 * 
 * 	initial checkin
 * 	[89/12/19  17:14:00  dorr]
 * 
 * 	created.
 * 	[89/08/10  16:13:06  dorr]
 * 
 * Revision 2.8  91/07/01  14:13:16  jms
 * 	Do interruption of syscalls (post interrupts, modify sigpause,wait etc.)
 * 	Enable direct posting to self without going thru TM.
 * 	Add "event_post_with_timeout"
 * 	[91/06/25  11:34:01  jms]
 * 
 * 	Misc fixes.
 * 	[91/05/07  11:35:33  jms]
 * 
 * 	Modify to use the "interrupt" mechinism to interrupt threads executing in
 * 	the emulation lib.
 * 	[91/04/15  18:04:44  jms]
 * 
 * 	Cosmetic changes
 * 	[91/02/21  14:07:54  jms]
 * 
 * Revision 2.7  91/05/05  19:29:02  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:31  dpj]
 * 
 * 	Fixed broken cloning logic (from Rich Draves @ CMU).
 * 	[91/04/28  10:34:15  dpj]
 * 
 * Revision 2.6  90/12/21  13:55:16  jms
 * 	Fixed uxsignal_signal_set_mask: Too few arguments.
 * 	[90/12/17  16:17:31  neves]
 * 
 * 	Added code to prevent handlers for SIGKILL and SIGSTOP
 * 	to be set using the uxsignal_signal_set_vec routine.
 * 	[90/12/17  16:12:46  neves]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:16:38  jms]
 * 
 * Revision 2.5  90/11/27  18:21:04  jms
 * 	Misc PureKernel KP.
 * 	[90/11/20  14:42:43  jms]
 * 
 * Revision 2.4  90/07/11  17:28:42  dorr
 * 	Cosmetic changes.
 * 
 * Revision 2.3  90/07/09  17:11:24  dorr
 * 	move exception handlers into cthreads.
 * 	[90/03/22  13:58:36  dorr]
 * 
 * 	add event handlers.  add signal_register & signal_reset
 * 	for tracking primary thread.  change signal_*() to
 * 	be internal routines instead of real internal hanlders
 * 	fix a very stupid precidence bug.
 * 	[90/03/01  15:04:40  dorr]
 * 
 * 	add signal_return, signal_pause and signal_wait.
 * 	get rid of alerts. use alerts.  get working.
 * 	switch stacks.
 * 	[90/02/23  14:55:12  dorr]
 * 
 * 	get sun machdep to work.
 * 	[90/01/26  15:17:22  dorr]
 * 
 * 	first working version.
 * 	[90/01/11  11:45:03  dorr]
 * 	Enable emulated signals (vs htg signals)
 * 	Add logic to track events in the lives of child proceses as they arrive.
 * 	This is done via the code and subcode for the unix exception "sigchld".
 * 
 * 	Detect and handle as best as possible (die meaningfully) failures in the
 * 	emulation lib.  This does not include turning failures in syscalls into
 * 	the appropriate errno.
 * 
 * 	Many many bug fixes.
 * 
 * 	Handle sync. of self signaling.  Deal with kernel exceptions.
 * 
 * 	Set the object up for clone completion.
 * 	[90/07/09  10:41:30  jms]
 * 
 * Revision 2.2  90/01/02  22:16:55  dorr
 * 	first crack at signals.
 * 
 */

#include <uxsignal_ifc.h>

extern "C" {
#include <base.h>
#include <mach/std_types.h>
#include <mach/exception.h>
#include "ux_exception.h"	/* XXX should be somewhere else? */
#include <us_error.h>
#include <sig_error.h>
#include <interrupt.h>

#include <errno.h>

#include <syscall_val.h>
}

extern "C" {
boolean_t    event_is_sensitive_md(thread_t);
boolean_t    uxsignal_xlate_exception_md(int, int, int, int*, int*);
mach_error_t uxsignal_call_md(thread_t		thread,
			      vm_offset_t	stack,
			      mach_error_fn_t	hand,
			      int		sig,
			      int		code,
			      int		mask,
			      boolean_t		on_stack,
			      boolean_t		on_exit,
			      boolean_t		is_default_handler);
}
/*
 * Debugging control.
 */
int	signal_debug = 1;


#define cantmask (sigmask(SIGKILL) | sigmask(SIGCONT) | sigmask(SIGSTOP))


extern "C" void signal_terminate(int, int);
extern "C" void signal_core(int, int);
extern "C" void signal_stop(int, int);
extern "C" void signal_ignore(int);

typedef enum { SIGDEF_IGNORE, SIGDEF_TERMINATE, SIGDEF_CORE, SIGDEF_STOP } sigdef_t;

static sigdef_t		default_handlers[NSIG] = {
	SIGDEF_IGNORE,		/* ... */
	SIGDEF_TERMINATE,	/* hup */
	SIGDEF_TERMINATE,	/* int */
	SIGDEF_CORE,		/* quit */
	SIGDEF_CORE,		/* illegal instruction */
	SIGDEF_CORE,		/* trap */
	SIGDEF_CORE,		/* iot */
	SIGDEF_CORE,		/* emt */
	SIGDEF_CORE,		/* fpe */
	SIGDEF_TERMINATE,    	/* kill */
	SIGDEF_CORE,		/* bus error */
	SIGDEF_CORE,		/* seg fault */
	SIGDEF_CORE,		/* bad argument to syscall */
	SIGDEF_TERMINATE,    	/* pipe */
	SIGDEF_TERMINATE,    	/* alarm */
	SIGDEF_TERMINATE,    	/* term */
	SIGDEF_IGNORE,		/* urgent condition */
	SIGDEF_STOP,		/* stop */
	SIGDEF_STOP,		/* terminal stop */
	SIGDEF_IGNORE,		/* continue */
	SIGDEF_IGNORE,		/* child status change */
	SIGDEF_STOP,		/* background read */
	SIGDEF_STOP,		/* background write */
	SIGDEF_IGNORE,		/* i/o */
	SIGDEF_TERMINATE,	/* cpu time limit */
	SIGDEF_TERMINATE,	/* file size limit */
	SIGDEF_TERMINATE,	/* virtual timer alarm */
	SIGDEF_TERMINATE,	/* profile timer */
	SIGDEF_IGNORE,		/* window size */
	SIGDEF_TERMINATE,	/* user defined signal 1 */
	SIGDEF_TERMINATE,	/* user defined signal 2 */
};

#define	LOCK	mutex_lock(&Local(lock));
#define	ENDLOCK	mutex_unlock(&Local(lock));
#define	UNLOCK	ENDLOCK

#define	UXSIGNAL_THREAD_HASH_SIZE	10



#ifdef	GXXBUG_CLONING2
#define BASE usEvent
DEFINE_CLASS(uxsignal);
#else	GXXBUG_CLONING2
DEFINE_CLASS_MI(uxsignal);
DEFINE_CASTDOWN2(uxsignal,usEvent,usClone);
#endif	GXXBUG_CLONING2

void uxsignal::init_class(usClass* class_obj)
{
        usEvent::init_class(class_obj);

#ifdef	GXXBUG_CLONING2
#else	GXXBUG_CLONING2
        usClone::init_class(class_obj);
#endif	GXXBUG_CLONING2

	DEBUG2(TRUE, (0,"uxsignal::initClass\n"));

	BEGIN_SETUP_METHOD_WITH_ARGS(uxsignal);
	SETUP_METHOD_WITH_ARGS(uxsignal,event_post);
	SETUP_METHOD_WITH_ARGS(uxsignal,event_post_with_timeout);
	END_SETUP_METHOD_WITH_ARGS;

}

uxsignal::uxsignal()
{
	static boolean_t	first_time = TRUE;

	DEBUG2(signal_debug,(0,"%s::create\n", class_name()));

	Local(blocked) = (signal_set_t)0;
	Local(pending) = (signal_set_t)0;
	Local(state) = 0;

	/*  all handlers <- SIG_DFL, flags <- 0, masks <- 0 */
	bzero( Local(vec), sizeof(Local(vec)) );

	Local(sigstack) = (vm_address_t)0;

	mutex_init(&Local(lock));
	Local(pause) = (condition_t)0;

	intr_init();

	child_info_init();
}

uxsignal::~uxsignal()
{
	if (_Local(pause))
		condition_free(_Local(pause));
}

char* uxsignal::remote_class_name() const
{
	return "usEvent_proxy";
}

mach_error_t uxsignal::clone_init(mach_port_t child)
{
	return ERR_SUCCESS;
}

mach_error_t uxsignal::clone_abort(mach_port_t child)
{
	return ERR_SUCCESS;
}

mach_error_t uxsignal::clone_complete()
{
	mutex_init(&Local(lock));
	if (Local(pause) != (condition_t) 0)
		condition_init(Local(pause));
	child_info_post_fork();

/*	(void) invoke_super(Super,mach_method_id(clone_complete)); */
	return(ERR_SUCCESS);
}

/*
 *  signal_register:  keep track of the "primary" thread for delivery
 *  of signals not directed to a particular thread.
 */
mach_error_t uxsignal::signal_register(thread_t thread)
{
	DEBUG2(signal_debug,(0,"register primary thread=%d\n", thread));
	LOCK {
		_Local(primary_thread) = thread;
	} ENDLOCK;

	return ERR_SUCCESS;
}



/*
 *  signal_reset:  reset the state of all signal handlers
 *  to their default values
 */
mach_error_t uxsignal::signal_reset_locked(thread_t thread)
{
	DEBUG2(signal_debug,(0,"reset_locked; primary thread=%d\n", thread));
	Local(primary_thread) = thread;

	Local(blocked) = (signal_set_t)0;
	Local(pending) = (signal_set_t)0;
	Local(state) = 0;

	/*  all handlers <- SIG_DFL, flags <- 0, masks <- 0 */
	bzero( Local(vec), sizeof(Local(vec)) );

	Local(sigstack) = (vm_address_t)0;
	return ERR_SUCCESS;
}

mach_error_t uxsignal::signal_reset(thread_t thread)
{
	mach_error_t		ret;

	DEBUG2(signal_debug,(0,"reset; primary thread=%d\n", thread));
	LOCK {
		ret = signal_reset_locked(thread);
	} ENDLOCK;

	return(ret);
}

extern "C" mach_error_t signal_reset(uxsignal*, thread_t);

mach_error_t signal_reset(uxsignal *obj, thread_t thread)
{
	return obj->signal_reset(thread);
}


#define	MACH_EXCEPTION_SUB	13	/* XXX put this somewhere */
/*
 *	signal_post: make a given signal pending for a given thread.
 *	ASSUME internal routine called with the object locked
 */

mach_error_t
uxsignal::signal_post(thread_t thread, int sig, int code, int tm_post_id)
{

	/* XXX hash on thread; split thread/task signals */
	if (sig <= 0 || sig > NSIG)
		return US_INVALID_ARGS;

	Local(pending) |= sigmask(sig);
	Local(ux_code)[sig] = code;
	if (tm_post_id) {
		Local(tm_post_ids)[sig] = tm_post_id;
	}

	return ERR_SUCCESS;
	  
}
	

/*
 *	uxsignal::xlate_exception translates a mach exception, code 
 *	and subcode to a signal and u.u_code.  
 *	Calls uxsignal::xlate_exception_md
 *	(machine dependent) to attempt translation first.
 */
boolean_t
uxsignal::xlate_exception(int exception, int code, int subcode, 
			   int *ux_signal, int *ux_code)
{
	/*
	 *	Try machine-dependent translation first.
	 */
	if (uxsignal_xlate_exception_md(exception, code, subcode, 
				ux_signal, ux_code))
		return(TRUE);
	
	switch(exception) {

	    case EXC_BAD_ACCESS:
		if (code == KERN_INVALID_ADDRESS)
		    *ux_signal = SIGSEGV;
		else
		    *ux_signal = SIGBUS;
		break;

	    case EXC_BAD_INSTRUCTION:
	        *ux_signal = SIGILL;
		break;

	    case EXC_ARITHMETIC:
	        *ux_signal = SIGFPE;
		break;

	    case EXC_EMULATION:
		*ux_signal = SIGEMT;
		break;

	    case EXC_SOFTWARE:
		switch (code) {
		    case EXC_UNIX_BAD_SYSCALL:
			*ux_signal = SIGSYS;
			break;
		    case EXC_UNIX_BAD_PIPE:
		    	*ux_signal = SIGPIPE;
			break;
		    case EXC_UNIX_ABORT:
			*ux_signal = SIGABRT;
			break;
		}
		break;

	    case EXC_BREAKPOINT:
		*ux_signal = SIGTRAP;
		break;
	}
	return(TRUE);
}

/*
 *	event_post, event_post_with_timeout:
 *	Under normal conditions, make the given signal
 *	one of the pending signals and call signal_deliver to try to
 *	deliver a signal to user code.  if you are in sensitive code,
 *	either process a machine fault or interrupt what's in process
 *	so that the signal can be processed.
 *
 *	ASSUME: you are operating on some thread other than yourself.
 *	machine dependent code knows how to find and manipulate that
 *	thread's state in ways that may be different if the thread
 *	is in sensitive code.
 */


#define IsUnixSig(err)	(err_get_sub(err) == UNIX_SIG_ERR_SUB)
#define IsMachException(err)	(err_get_sub(err) == MACH_EXCEPTION_SUB)

mach_error_t 
uxsignal::event_post_internal(thread_t thread, mach_error_t event,
			      int code,	/* machine specific */
			      int subcode, int tm_post_id)
{
	int			ux_code;
	mach_error_t		err = ERR_SUCCESS;
	int			sig;
	intr_handler_t		h;
	boolean_t		selfsig = FALSE;	/* from own process? */
	intr_cthread_id_t	intr_cthread_id;

	DEBUG1(signal_debug,(0,"event_post: thread = %#x err=%#x code=%#x subcode=%#x\n", 
		      thread, event, code, subcode));

	LOCK {
		/*
		 *  translate from a generic mach error into a unix signal
		 */
		if ( IsUnixSig(event) ) {
			/* standard unix signals */
			sig = err_get_code(event);
			ux_code = 0;

			/* XXX store away sigchild stuff */
			if (UNIX_SIG_CHLD == event) {
			    DEBUG2(signal_debug,(0,"event_post: got child info code=%#x subcode=%#x\n", 
		      code, subcode));

			    set_child_info(code, subcode);
			}

		} else if ( IsMachException(event) ) {
			/*
			 * do a translation for things like
			 * SIGFPE, etc.
			 */
			if (! xlate_exception(err_get_code(event), 
					      code, subcode, &sig, &ux_code)) {
				/*
				 *  huh?
				 */
				DEBUG0(signal_debug,(0,"error from xlate event %#x?\n",
					     event));
				sig = SIGSYS;
			}
		} else {
			/* oddball, non-translatable exception */
			DEBUG0(signal_debug,(0,"couldn't translate event %#x\n",
					     event));
			sig = SIGSYS;
		}

		/*
		 *  non-machine signals go to the primary signal thread
		 *  If the thread was supplied, then it must have been stopped
		 *  by the caller.  XXX Must insure that the "thread" is
		 *  bound to the "thread" at this point.
		 */
		if (thread == MACH_PORT_NULL) {
			thread  = Local(primary_thread);
		}
		if (thread == mach_thread_self()) {
			/* 
			 * Currently emulating self signaling syscall
			 * in this thread, postit and finish.
			 */
			selfsig = TRUE;
		}
		else {
			/*
			 * Applying signal to another thread. 
			 */
			if (! IsMachException(event)) {
				thread_suspend(thread);
				thread_abort(thread);
			}
		}
	} ENDLOCK;

	/*
	 * Done with info gathering, lets do it.
	 */

	/* 
	 * Post and Deliver the signal in the appropriate fashion 
	 * based on whether we are sensitive, if we are a 
	 * Mach Exception, and if there are special handlers.
	 */
	if ( selfsig || event_is_sensitive_md(thread) ) {
		/*
		 *  in sensitive land.  special processing is needed
		 *
		 *  Try to interrupt the sensitive code (syscall).
		 *  Set up to take the signal on exit from 
		 *  sensitive mode.
		 */

		/* 
		 * Ensure that the cthreads knows the right cthread
		 * stuff represents this thread.
		 * Cannot bind self XXX
		 */
		
		if (! selfsig) {
			intr_cthread_id = 	 /* XXX Bad cthread magic */
			    (intr_cthread_id_t)(cthread_bind_phantom(thread));
		}

		/*
		 *  post the signal to be handled on exit 
		 *  from the sensitive area.
		 */
		err = signal_post(thread, sig, ux_code, tm_post_id);
		if (err) goto finish;

		/*
		 * Try to deliver it. In other words setup to call handler
		 * upon return from the current syscall;
		 */
		err = deliver(thread, TRUE);
		if (err) goto finish;

		
		/*
		 *  Setup to interrupt sensitive code and deliver if
		 *  possible.
		 */
		if (selfsig) goto finish;

		if (! IsMachException(event)) {
			/* We have an asynchronious event */
			intr_post_interrupt(intr_cthread_id,
				INTR_IMMEDIATE, INTR_ASYNC,
				event, code, subcode);
		}
		else if (! intr_post_interrupt(intr_cthread_id,
				INTR_IMMEDIATE, INTR_SYNC,
				event, code, subcode)) {

			/* Post failed to deliver a sync
			 * interrupt,  must be no handler.
			 * XXX get out with its shame for now.
			 */
#if SIGNAL_HOT_DEATH
			(void)signal_reset_locked(thread); /* XXX */
			(void)signal_post(thread, sig, 
					  ux_code, tm_post_id);
			(void)deliver(thread, FALSE);
#if MACH_UNIX
			exit(-1);
#else MACH_UNIX
			task_terminate(mach_task_self());
#endif MACH_UNIX
#else SIGNAL_HOT_DEATH
			/* Lets just keep going, with luck... XXX */
			goto finish;
#endif SIGNAL_HOT_DEATH
		}
	} 
	else {
		/*
		 *  not in sensitive code.  post and deliver the signal
		 */

		err = signal_post(thread, sig, ux_code, tm_post_id);
		if (err == ERR_SUCCESS) {
			(void)deliver(thread, FALSE);
		}

	}

    finish:
	/* something went wrong */;

	if (! selfsig) {
		(void)thread_resume(thread);
	}
	return ERR_SUCCESS;
}


mach_error_t 
uxsignal::event_post(thread_t thread, mach_error_t event,
		     int code,	/* machine specific */
		     int subcode)
{
	return(event_post_internal(thread, event, code, subcode, 0));
}

mach_error_t 
uxsignal::event_post_with_timeout(thread_t thread, mach_error_t event,
				  int code, /* machine specific */
				  int subcode, int tm_post_id)
{
	return(event_post_internal(thread, event, code, subcode, tm_post_id));
}

#define	max_bit(mask)	(sizeof(mask)*8)
int uxsignal::nxt_bit(int mask)
{
	register int		i;

	for (i=1; i<max_bit(mask); i++)
		if ( mask & sigmask(i) ) break;

	return i == max_bit(mask) ? 0 : i;
}


/*
 * signal_deliver:  try to deliver a signal.  run through the 
 * list of pending signals and deliver the next available one.
 *
 * ASSUME that the target thread is a different, suspended thread 
 * which may be inside of or outside of the emulation library (as
 * indicated by the "sensitive" flag) or the current thread which
 * is performing some very careful signal operation (in which case
 * case the "sensitive" flag is always on and the thread is not suspended).
 *
 * ASSUME you are called with the object locked.
 */

mach_error_t uxsignal::deliver(thread_t thread, boolean_t sensitive)
{
	int			sig;
	signal_set_t		pending;
	signal_set_t		old_mask;

	mach_error_fn_t		handler = (mach_error_fn_t)signal_ignore;
	mach_error_fn_t		signal_ignore_tmp = (mach_error_fn_t)signal_ignore;
	boolean_t		is_default_handler;
	vm_offset_t		stack;
	int			code;
	signal_set_t		old_blocked, old_pending;
	int			old_state;
	boolean_t		on_stack;
	
	
	mach_error_t		err = ERR_SUCCESS;
	
	while ( handler == signal_ignore_tmp ) {

		/*
		 *  process the next pending signal
		 */
		pending = Local(pending) &~ Local(blocked);
		if (pending == 0) break;
	
		sig = nxt_bit(pending);
		if (sig == 0) {
			/* nothing left pending (and not blocked) */
			break;
		}

		
		DEBUG0(signal_debug,(0,"signal_deliver: delivering %#d\n",
				     sig));

		old_blocked = Local(blocked);
		old_pending = Local(pending);
		old_state = Local(state);

		
		/*
		 * remove it from the list of pending signals 
		 */
		Local(pending) &= ~sigmask(sig);
		
		handler = Local(vec)[sig].sv_handler;
		if (SIG_IGN == handler) {
			continue;
		}
		code = Local(ux_code)[sig];
		
		if (handler == SIG_DFL) {
			switch ( default_handlers[sig] ) {
			    case SIGDEF_CORE:
				handler = (mach_error_fn_t)signal_core;
				code = Local(tm_post_ids)[sig];
				break;
			    case SIGDEF_TERMINATE:
				handler = (mach_error_fn_t)signal_terminate;
				code = Local(tm_post_ids)[sig];
				break;
			    case SIGDEF_STOP:
				handler = (mach_error_fn_t)signal_stop;
				code = Local(tm_post_ids)[sig];
				break;
			    case SIGDEF_IGNORE:
				handler = signal_ignore_tmp;
				code = Local(tm_post_ids)[sig];
				continue;
			}
			Local(tm_post_ids)[sig] = 0;

			if (! sensitive) {
				handler(sig,code);
				continue;
			}
			is_default_handler = TRUE;
		}
		else {
			is_default_handler = FALSE;
		}

		
		if (Local(state) & SIG_STATE_USE_OLD_MASK) {
			old_mask= Local(prev_blocked);
			Local(state) &= ~SIG_STATE_USE_OLD_MASK;
		} else
			old_mask = Local(blocked);
		
		
		/* 
		 * block the signal and any associated signals 
		 */
		Local(blocked) |= sigmask(sig)|Local(vec)[sig].sv_mask;
		
		DEBUG2(signal_debug,(0, "signal_deliver: handler=%#x  old mask=%#x new mask=%#x\n",
				     handler, old_mask, Local(blocked)));
		
		/*
		 *  get a stack and code value;
		 *  make the call to the handler.  the machine-dependent
		 *  code will set up the scp
		 */
		if (on_stack = (Local(vec)[sig].sv_flags & SV_ONSTACK)) {
			stack = Local(sigstack);
			Local(state) |= SIG_STATE_ON_STACK;
		} else {
			stack = (vm_offset_t)0;
			Local(state) &= ~SIG_STATE_ON_STACK;
		}
		
		err = uxsignal_call_md(thread, 
				       stack,
				       handler, 
				       sig, 
				       code, 
				       old_mask,
				       on_stack,
				       sensitive,
				       is_default_handler);
		if (err) {
			/*
			 *  something went wrong -- restore previous signal state
			 *  and vamoose.  this usually means we've already delivered
			 *  one signal.
			 */
			Local(blocked) = old_blocked;
			Local(state) = old_state;
			Local(pending) = old_pending;
			handler = signal_ignore_tmp;
			break;
		} 

	}
	
	if (handler == signal_ignore_tmp) {
		
		/*
		 *  indicate that no signal delivery took place
		 */
		err = UNIX_SIG_NO_SIGNAL;
	}
	else {
		/*
		 *  wake up anyone who's paused
		 */
		if ( Local(state) & SIG_STATE_PAUSED ) {
			Local(state) &= ~SIG_STATE_PAUSED;
				
			condition_broadcast(Local(pause));
		}
	}
	
	return err;
}

/*
 *	signal_return: reset the signal mask and do a non-local goto
 *	back into some former context
 */
mach_error_t uxsignal::signal_return(int onstack, int mask)
{
	LOCK {
		Local(blocked) = mask;
		if (onstack & 1) {
			Local(state) |= SIG_STATE_ON_STACK;
		} else {
			Local(state) &= ~SIG_STATE_ON_STACK;
		}

		/*
		 *  eck.  you've just changed the mask.  we may have to deliver
		 *  another signal now.
		 */
		(void)deliver(MACH_PORT_NULL, TRUE);
	} ENDLOCK;

	return ERR_SUCCESS;
}

extern "C" mach_error_t signal_return(uxsignal*, int, int);

mach_error_t signal_return(uxsignal *obj, int onstack, int mask)
{
	return obj->signal_return(onstack, mask);
}


/*
 *	signal_set_vec:	set the handler routine for a given signal
 */
mach_error_t 
uxsignal::signal_set_vec(int sig, struct sigvec *vec, struct sigvec *ovec)
{
	if (sig < 0 || sig > NSIG || sig == SIGKILL || sig == SIGSTOP)
		return US_INVALID_ARGS;

	DEBUG0(signal_debug,(0,"signal_set_vec: sig=%#d vec=%#x \n",
			     sig, vec));
	if (vec)
		DEBUG0(signal_debug,(0,"signal_set_vec: hand=%#x mask=%#x\n",
			     vec->sv_handler, vec->sv_mask));

	LOCK {
		if (ovec) {
			*ovec = Local(vec)[sig];
		}

		if (! (sigmask(sig) & cantmask)) {
			if (vec) {
				Local(vec)[sig] = *vec;
			}
		}

	} ENDLOCK;


	return ERR_SUCCESS;
}


/*
 *	signal_set_handler:	set the handler routine for a given signal
 */
mach_error_t 
uxsignal::signal_set_handler(int sig, mach_error_fn_t handler, 
			      mach_error_fn_t *old_handler)
{
	if (sig < 0 || sig > NSIG)
		return US_INVALID_ARGS;

	LOCK {
		if (old_handler)
			*old_handler = Local(vec)[sig].sv_handler;
		Local(vec)[sig].sv_handler = handler;
		Local(vec)[sig].sv_mask &= ~cantmask;
	} ENDLOCK;

	return ERR_SUCCESS;
}

/*
 *	signal_block:  block delivery of one or more signals
 */
mach_error_t uxsignal::signal_block(signal_set_t mask, signal_set_t *oldmask)
{

	LOCK {
		DEBUG2(signal_debug,(0, "signal_block: old=%#x new=%#x\n",
			Local(blocked), Local(blocked)|mask));	     
		if (oldmask)
			*oldmask = Local(blocked);
		Local(blocked) |= mask & ~cantmask;
	} ENDLOCK;

	return ERR_SUCCESS;
}

/*
 *	signal_set_mask:  set the mask of signals whose delivery
 *	is to be blocked
 */
mach_error_t uxsignal::signal_set_mask(signal_set_t mask, signal_set_t* omask)
{
	LOCK {
		DEBUG2(signal_debug,(0, "set_mask: old=%#x new=%#x\n",
			Local(blocked), mask));	     
		if (omask)
			*omask = Local(blocked);
		Local(blocked) = mask & ~cantmask;

		/*
		 * we might have just unblocked some signal.
		 * run through and deliver all pending, unblocked signals
		 */
		(void)deliver(MACH_PORT_NULL, TRUE);

	} ENDLOCK;

	return ERR_SUCCESS;
}


/*
 *	signal_get_mask:  retreive the mask of signals whose
 *	delivery is blocked
 */
mach_error_t uxsignal::signal_get_mask(signal_set_t *mask)
{
	LOCK {
		*mask = _Local(blocked);
	} ENDLOCK;

	return ERR_SUCCESS;
}

/*
 *	signal_set_stack:  set the new stack value and return the old
 */
mach_error_t uxsignal::signal_set_stack(struct sigstack *newstack, 
					struct sigstack *oldstack)
{

	/*
	 * XXX take care of "onstack".  will need something like
	 * cthread_stack()??  may need for oldstack if it is
	 * supposed to return stack area for thread if sigstack
	 * not set.
	 */
	LOCK {
		if (oldstack) {
			oldstack->ss_sp = (char *)Local(sigstack);
		}

		if (newstack) {
			Local(sigstack) = (vm_address_t)newstack->ss_sp;
		}
	} ENDLOCK;

	return ERR_SUCCESS;
}


/*
 *	signal_pause:  atomically set the mask and wait for incoming
 *	signals
 */
int uxsignal::signal_pause(signal_set_t mask)
{
	signal_set_t	old_mask;
	mach_error_t	err;

	mutex_lock(&Local(lock));

	/*
	 * temporarily set mask; wait for something to happen
	 */
	Local(prev_blocked) = Local(blocked);
	Local(blocked) = mask;
	/* XXX Who puts these back??? */

	Local(state) |= SIG_STATE_USE_OLD_MASK;

	/*
	 *  try to deliver the signal right off the bat
	 */
	err = deliver(mach_thread_self(), TRUE);
	if (err == ERR_SUCCESS) {
		mutex_unlock(&Local(lock));

		return unix_err(EINTR);
	}

	if ( Local(pause) == (condition_t)0 ) {
		Local(pause) = condition_alloc();
	}

	Local(state) |= SIG_STATE_PAUSED;
	while (Local(state) & SIG_STATE_PAUSED) {
		intr_cond_wait(Local(pause), &Local(lock));
	}
	Local(state) &= (~ SIG_STATE_PAUSED);

	mutex_unlock(&Local(lock));

	return unix_err(EINTR);
}

uxsignal::signal_wait()
{
	/* XXX */
	return ERR_SUCCESS;
}


/*
 * Private methods
 */

/*
 * The following "child_info" mechinism is used for keeping a list
 * task_ids/status (wait status) pairs pending their use by a "wait"
 * emulation. (may not belong here)
 * XXX This may be a hack.  Maybe should be done with zombies in the
 * taskmaster for real (also).  This is faster.
 */
typedef struct child_info_entry {
	dll_chain_t	info_list;
	int		task_id;
	int		uxstatus;
} child_info_entry, *child_info_t;

void uxsignal::child_info_init()
{
    dll_init(&Local(child_info_list));
    mutex_init(&Local(child_info_lock));
    condition_init(&Local(child_info_arrived));
}

void uxsignal::child_info_post_fork()
{
    child_info_t	child_info;

    /* deallocate the stuff on the list if any */
    while (! dll_empty(&Local(child_info_list))) {
	dll_remove_first(&Local(child_info_list), child_info, child_info_t, info_list);
	free(child_info);
    }
    child_info_init();
}

void uxsignal::set_child_info(int task_id, int uxstatus)
{
    child_info_t	child_info;

    child_info = (child_info_t)malloc(sizeof(*child_info));
    child_info->task_id = task_id;
    child_info->uxstatus = uxstatus;

    mutex_lock(&Local(child_info_lock));
    dll_enter(&Local(child_info_list), child_info, child_info_t, info_list);
    mutex_unlock(&Local(child_info_lock));

    condition_signal(&Local(child_info_arrived));
}

mach_error_t 
uxsignal::get_child_info(int *task_id, int *uxstatus, boolean_t pause)
{
    child_info_t	child_info;
    mach_error_t	ret;

    *task_id = 0;
    *uxstatus = 0;

    mutex_lock(&Local(child_info_lock));
    if (dll_empty(&Local(child_info_list))) {
	if (! pause) {
	    mutex_unlock(&Local(child_info_lock));
	    return(ERR_SUCCESS);
	}
	
	while (dll_empty(&Local(child_info_list))) {
	    ret = intr_cond_wait(&Local(child_info_arrived), 
					&Local(child_info_lock));
	    if (ERR_SUCCESS != ret) {
		mutex_unlock(&Local(child_info_lock));
		return(EXCEPT_SOFTWARE);
	    }
	}
    }

    dll_remove_first(&Local(child_info_list), child_info, 
			child_info_t, info_list);
    mutex_unlock(&Local(child_info_lock));
    *task_id = child_info->task_id;
    *uxstatus = child_info->uxstatus;
    free(child_info);
    return(ERR_SUCCESS);
}


mach_error_t uxsignal::_notdef()
{
	DEBUG0(TRUE, (0, "uxsignal::Operation not defined !!\n"));
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxsignal::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_duplicate(ns_access_t access, usItem** newobj)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_set_times(time_value_t atime, time_value_t mtime)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_get_protection(ns_prot_t prot, int* protlen)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_set_protection(ns_prot_t prot, int protlen)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_get_privileged_id(int* id)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
  return _notdef();
}

mach_error_t
uxsignal::ns_get_manager(ns_access_t access, usItem **newobj)
{
  return _notdef();
}

