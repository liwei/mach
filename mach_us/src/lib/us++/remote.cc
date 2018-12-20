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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/remote.cc,v $
 *
 * Purpose: usRemote: base class for all objects available remotely.
 *
 * HISTORY
 * $Log:	remote.cc,v $
 * Revision 2.6  94/07/07  17:24:10  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  93/01/20  17:38:12  jms
 * 	Much debugging code. All deactivated.
 * 	[93/01/18  17:02:56  jms]
 * 
 * Revision 2.4  92/07/05  23:28:29  dpj
 * 	Fixed history.
 * 	[92/06/29  22:57:49  dpj]
 * 
 * 	Define as an abstract class instead of a concrete class.
 * 	Fixed termination sequence for external ports.
 * 	[92/06/24  17:04:35  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:56:59  dpj]
 * 
 * 	Commented-out suspicious task_suspend().
 * 	Fixed arg types for _make_external_port().
 * 	[92/03/12  13:51:11  dpj]
 * 
 * Revision 2.3  92/03/05  15:05:38  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:32:10  jms]
 *
 * Revision 2.2  91/11/06  13:47:34  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:04:09  pjg]
 * 
 */

#define	FAST_CASTDOWN

#include	<top_ifc.h>

#include	<rpcmgr_ifc.h>

#include	<debug.h>

extern "C" {
#include	<stdarg.h>
#include	<exception_error.h>
#include	<interrupt.h>
#include	<logging.h>
}


#define BASE usTop
DEFINE_ABSTRACT_CLASS(usRemote);
//_DEFINE_CLASS(usRemote)
//_DEFINE_INSTANTIATION(usRemote)
//_DEFINE_CONVERTER(usRemote)
//_DEFINE_CONSTRUCTOR(usRemote)
//_DEFINE_CASTDOWN(usRemote)

#define LOG_OBJ(id) \
	LOG5(TRUE, (id), this, refcount(), no_senders, mscount, external_port_data);

#if WATCH_SENDERS
#define AIX1 10
#define AI_X_FIRST_INDEX AIX1
#define AI_X_FIRST ((int)(active_invokes[AIX1]))
#define NO_SENDERS_COUNT ((int)(active_invokes[AIX1+1]))
#define NO_SENDERS_RESET_COUNT ((int)(active_invokes[AIX1+2]))
#define CONSTRUCTOR_COUNT ((int)(active_invokes[AIX1+3]))
#define DESTRUCTOR_COUNT ((int)(active_invokes[AIX1+4]))
#define INVOKE_COUNT ((int)(active_invokes[AIX1+5]))
#define GET_TRANSFER_PORT_COUNT ((int)(active_invokes[AIX1+6]))
#define REG_INVOKE_COUNT ((int)(active_invokes[AIX1+7]))
#define DEREG_INVOKE_COUNT ((int)(active_invokes[AIX1+8]))
#define INTERRUPT_INVOKE_COUNT ((int)(active_invokes[AIX1+9]))
#define NO_SENDERS_CALLS_COUNT ((int)(active_invokes[AIX1+10]))
#define MSCOUNT ((int)(active_invokes[AIX1+11]))
#define MSCOUNT_INCR_COUNT ((int)(active_invokes[AIX1+12]))
#define MSCOUNT_ONE_COUNT ((int)(active_invokes[AIX1+13]))
#define AI_X_LAST_INDEX AIX1+14
#define AI_X_LAST ((int)(active_invokes[AI_X_LAST_INDEX]))
#endif WATCH_SENDERS

usRemote::usRemote()
{
	int i;
	mutex_init(&lock);
	object_port_data = MACH_PORT_NULL;
	external_port_data = MACH_PORT_NULL;
	next_active_invoke_index = 0;

	mscount = 0;
	no_senders = FALSE;
	next_seqno = 0;

	LOG1(TRUE, 1001, this);
#if WATCH_SENDERS
	bzero(&(active_invokes[0]), sizeof(active_invokes[0])*usRemote_MAX_ACTIVE_INVOKES);
	CONSTRUCTOR_COUNT=1;
	AI_X_FIRST = 0xcafe;
	AI_X_LAST = 0xbabe;
#endif WATCH_SENDERS
}

usRemote::~usRemote()
{
	DESTRUCTOR_GUARD();
	LOG_OBJ(1002);
#if WATCH_SENDERS
	DESTRUCTOR_COUNT++;
	if ((mscount > 0) && (! no_senders)) {
	    printf("~usRemote without no_senders\n");
	    printf("this=0x%x, size=0x%x, is_where=0x%x, class=%s\n",
		this, sizeof(*this), is_where(), is_a()->class_name());
	    printf("Suspending...\n");
	    task_suspend(mach_task_self());
	}
#endif WATCH_SENDERS
	if (object_port_data != MACH_PORT_NULL) {
		(void) mach_port_deallocate(mach_task_self(),object_port_data);
		object_port_data = MACH_PORT_NULL;
	}

	if (external_port_data != MACH_PORT_NULL) {
		if (no_senders == FALSE) {
			/*
			 * In a perfect world, since the external object holds
			 * a reference to this object, this terminate routine
			 * could only be called when the external port has
			 * already been taken care of. This is certainly what
			 * happens if the external object receives a
			 * no-more-senders notification.
			 *
			 * Something is seriously wrong; let's make the best
			 * of a bizarre situation...
			 */
			DEBUG0(TRUE,(0,
		"usRemote: terminating with non-null external port\n"));
			_rpcmgr()->_deregister_external_port(
							external_port_data);
		}
		_rpcmgr()->deallocate_external_port(external_port_data);
		external_port_data = MACH_PORT_NULL;
		LOG_OBJ(1002);
	}
}

//void usRemote::init_class(usClass* class_obj)
//{
//}

inline class rpcmgr* usRemote::_rpcmgr()
{
	return rpcmgr::GLOBAL;
}

mach_error_t usRemote::invoke(mach_method_id_t mid, ...)
{
	mach_error_t 		ret;
	va_list			ap;

	va_start(ap, mid);

	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset,
				*(arg_list_ptr)ap);
#else	FAST_CASTDOWN
		ret = (*entryp->pfunc)(
				_castdown(*entryp->cl),
				*(arg_list_ptr)ap);
#endif	FAST_CASTDOWN
	}
	else {
		ret = outgoing_invoke(mid,*(arg_list_ptr)ap);
	}

	va_end(ap);

	return ret;
}

mach_error_t usRemote::invoke(mach_method_id_t mid)
{
#if WATCH_SENDERS
	INVOKE_COUNT++;
#endif WATCH_SENDERS

	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		return (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset);
#else	FAST_CASTDOWN
		return (*entryp->pfunc)(
				_castdown(*entryp->cl));
#endif	FAST_CASTDOWN
	}
	else {
		return outgoing_invoke(mid);
	}
}

mach_error_t usRemote::invoke(mach_method_id_t mid, void* arg0)
{
#if WATCH_SENDERS
	INVOKE_COUNT++;
#endif WATCH_SENDERS

	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		return (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset,
				arg0);
#else	FAST_CASTDOWN
		return (*entryp->pfunc)(
				_castdown(*entryp->cl),
				arg0);
#endif	FAST_CASTDOWN
	}
	else {
		return outgoing_invoke(mid,arg0);
	}
}

mach_error_t usRemote::invoke(mach_method_id_t mid, void* arg0, void* arg1)
{
#if WATCH_SENDERS
	INVOKE_COUNT++;
#endif WATCH_SENDERS

	obj_method_entry_t entryp = is_a()->lookup_method(mid);
	if (entryp != 0) {
#ifdef	FAST_CASTDOWN
		return (*entryp->pfunc)(
				((void*)((usTop*)this)) + entryp->offset,
				arg0,arg1);
#else	FAST_CASTDOWN
		return (*entryp->pfunc)(
				_castdown(*entryp->cl),
				arg0,arg1);
#endif	FAST_CASTDOWN
	}
	else {
		return outgoing_invoke(mid,arg0,arg1);
	}
}


mach_error_t usRemote::outgoing_invoke(mach_method_id_t mid, ...)
{
	arg_list_ptr		arglist;
	mach_error_t		ret;
	va_list			ap;

	va_start(ap, mid);
	arglist = (arg_list_ptr) ap;

#ifdef	TIMING_13
	*(int*) arglist->args[0] = 42;
	ret = ERR_SUCCESS;
#else	TIMING_13
	ret = _rpcmgr()->_outgoing_invoke(this,mid,arglist);
#endif	TIMING_13

	va_end(ap);

	return(ret);
}


char* usRemote::remote_class_name() const
{
	return is_a()->remote_class_name();
}

void usRemote::set_remote_class_name(char* name)
{
	is_a()->set_remote_class_name(name);
}

/*
 * XXX DPJ Add locking for external port and object port allocation / access
 */

void usRemote::set_object_port(mach_port_t p)
{
	if (object_port_data != MACH_PORT_NULL) {
		mach_port_destroy(mach_task_self(),object_port_data);
	}
	object_port_data = p;
}


mach_port_t usRemote::get_transfer_port(int* name)
{
	mutex_lock(&lock);

	LOG_OBJ(1003);
#if WATCH_SENDERS
	GET_TRANSFER_PORT_COUNT++;
#endif WATCH_SENDERS

	if (external_port_data != MACH_PORT_NULL) {
		*name = MACH_MSG_TYPE_MAKE_SEND;
		mscount++;
		LOG_OBJ(1004);
#if WATCH_SENDERS
		MSCOUNT_INCR_COUNT++;
		MSCOUNT=mscount;
#endif WATCH_SENDERS
		if (no_senders) {
			/*
			 * Cancel the effect of a previous
			 * no-more-senders notification.
			 */
			LOG_OBJ(1005);
#if WATCH_SENDERS
			NO_SENDERS_RESET_COUNT++;
#endif WATCH_SENDERS
			no_senders = FALSE;
			_rpcmgr()->_register_external_port(
						external_port_data, this);
		}
		mutex_unlock(&lock);
		return(external_port_data);
	}

	if (object_port_data != MACH_PORT_NULL) {
		*name = MACH_MSG_TYPE_COPY_SEND;
		mutex_unlock(&lock);
		return(object_port_data);
	}

	external_port_data = _rpcmgr()->allocate_external_port();
	_rpcmgr()->_register_external_port(external_port_data, this);
	mscount = 1;
	LOG_OBJ(1006);
#if WATCH_SENDERS
	MSCOUNT_ONE_COUNT++;
	MSCOUNT=mscount;
#endif WATCH_SENDERS
	next_seqno = 0;
	no_senders = FALSE;
	*name = MACH_MSG_TYPE_MAKE_SEND;

	mutex_unlock(&lock);

	return(external_port_data);
}


void usRemote::_reset_after_clone()
{
	mutex_init(&lock);
	external_port_data = MACH_PORT_NULL;
	next_seqno = 0;
	mscount = 0;
}


mach_error_t usRemote::_register_incoming_invoke(intr_cthread_id_t id)
{
	mutex_lock(&lock);
	LOG_OBJ(1007);
#if WATCH_SENDERS
	REG_INVOKE_COUNT++;
#endif WATCH_SENDERS

	if (usRemote_MAX_ACTIVE_INVOKES == next_active_invoke_index) {
		DEBUG0(TRUE,
			(0,"usRemote: too many active invokes at once\n"));
		mutex_unlock(&lock);
		return(MACH_OBJECT_MAX_INVOKES);
	}
	active_invokes[next_active_invoke_index] = id;
	next_active_invoke_index++;
	mutex_unlock(&lock);

	return(ERR_SUCCESS);
}

mach_error_t usRemote::_deregister_incoming_invoke(intr_cthread_id_t id)
{
	int	i,j;

	mutex_lock(&lock);
	LOG_OBJ(1008);
#if WATCH_SENDERS
	DEREG_INVOKE_COUNT++;
#endif WATCH_SENDERS
	for (i = 0; i < next_active_invoke_index; i++) {
		if (id == active_invokes[i]) {
			break;
		}
	}
	if (next_active_invoke_index == i) {
		/* No such invoke found, probably done */
		DEBUG0(TRUE,
			(0,"usRemote: too many active invokes at once\n"));
		mutex_unlock(&lock);
		return(MACH_OBJECT_INVOKE_NOT_FOUND);
	}
	
	/* Found it, lets pop it */
	j = --next_active_invoke_index;
	if (0 != j) {
		active_invokes[i] = active_invokes[j];
	}
	mutex_unlock(&lock);
	return(ERR_SUCCESS);
}

mach_error_t usRemote::_interrupt_incoming_invokes()
{
	int	i;
	mutex_lock(&lock);
	LOG_OBJ(1009);
#if WATCH_SENDERS
	INTERRUPT_INVOKE_COUNT++;
#endif WATCH_SENDERS

	for (i = 0; i < next_active_invoke_index; i++) {
		intr_post_interrupt(active_invokes[i],
					INTR_IMMEDIATE, INTR_ASYNC,
					EXCEPT_SOFTWARE, 0, 0);
	}
	mutex_unlock(&lock);
	return(ERR_SUCCESS);
}


mach_error_t usRemote::_no_remote_senders(unsigned int not_mscount)
{
	mutex_lock(&lock);
	LOG_OBJ(1000);
#if WATCH_SENDERS
	NO_SENDERS_CALLS_COUNT++;
#endif WATCH_SENDERS

	ASSERT_EXPR("usRemote::_no_remote_senders: count too big!",
						not_mscount <= mscount);

	if (not_mscount == mscount) {
		/*
		 * This is for real.
		 */
		no_senders = TRUE;
		_rpcmgr()->_deregister_external_port(external_port_data);
		LOG_OBJ(1001);
#if WATCH_SENDERS
		NO_SENDERS_COUNT++;
#endif WATCH_SENDERS
	}

	mutex_unlock(&lock);
	return(ERR_SUCCESS);
}


void usRemote::_reference_with_seqno(mach_msg_seqno_t seqno)
{
	/*
	 * The next_seqno is protected by the lock on the
	 * external_port table that calls this function.
	 */
	for (; seqno >= next_seqno; next_seqno++) {
		this->reference();
	}
}
