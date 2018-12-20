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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/rpcmgr_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Manager for RPC operations.
 *
 * HISTORY
 * $Log:	rpcmgr_ifc.h,v $
 * Revision 2.4  94/07/07  17:24:17  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  93/01/20  17:38:22  jms
 * 	Deactivated debugging code.
 * 	[93/01/18  17:05:02  jms]
 * 
 * Revision 2.2  92/07/05  23:28:37  dpj
 * 	First version.
 * 	[92/05/10  00:57:28  dpj]
 * 
 */

#ifndef	_rpcmgr_ifc_h
#define	_rpcmgr_ifc_h

#include	<top_ifc.h>

extern "C" {
#include	<hash.h>
#include	<mach/message.h>
}



typedef	enum { MSG_DIR_REQUEST, MSG_DIR_REPLY } msg_direction_t;

static const int	rpcmgr_PORT_CACHE_SIZE = 10;

class rpcmgr {
	/*
	 * Global instance.
	 */
	static class rpcmgr		GLOBAL_data;
public:
	static class rpcmgr*		GLOBAL;

/* private:	XXX TIMING */

	/*
	 * Data structures for handling incoming RPC's.
	 *
	 * XXX DPJ Use only one lock for port_to_object_table
	 * and threads_waiting -- get it once and for all...
	 */
	static const int	DATABUF_MAX_XXX;
	mach_port_t		mach_object_port_set;
	hash_table_t		mach_port_to_object_table;
#if WATCH_THE_DEAD
	hash_table_t		dead_port_to_object_table;
#endif WATCH_THE_DEAD

	struct mutex		port_table_lock;
	int			thread_count;
	int			threads_waiting;
	int			max_threads;
	int     		min_waiting;
	int     		max_waiting;
	struct mutex		thread_waiting_lock;

				/*
				 * Default timeout values for the server loop
				 */
	int			send_timeout; 
	int			rcv_timeout;
	int			interrupt_timeout;

	boolean_t		_adjust_threads(int, boolean_t);

	mach_error_t		_incoming_invoke(
					usRemote*,
					mach_msg_header_t*,
					mach_msg_header_t*,
					char*,
					boolean_t*);

	void			_outgoing_invoke_error(
					char*,
					mach_error_t,
					char*,
					mach_port_t);

	mach_error_t		_msg_pack(
					mach_msg_header_t*,
					int*,
					mach_method_id_t,
					msg_direction_t,
					mach_error_t);
	mach_error_t		_msg_unpack(
					mach_msg_header_t*,
					char*,
					int*,
					mach_method_id_t*,
					msg_direction_t);
	mach_error_t		_msg_free(
					mach_msg_header_t*,
					mach_method_id_t*);

public:
				rpcmgr();
	virtual			~rpcmgr();

	mach_port_t		allocate_external_port();
	void			deallocate_external_port(mach_port_t);
	void			_register_external_port(
						mach_port_t,
						usRemote*);
	void			_deregister_external_port(mach_port_t);
	usRemote*		_lookup_external_port(mach_port_t);
	usRemote*		_lookup_external_port(
						mach_port_t,
						mach_msg_seqno_t);
#if WATCH_THE_DEAD
	usRemote*		_lookup_dead_external_port(mach_port_t);
#endif WATCH_THE_DEAD
	void			_object_handler();
	void			start_object_handler(int, int, int);

	mach_error_t		clone_complete();

	mach_error_t		_outgoing_invoke(
						usRemote*,
						mach_method_id_t,
						arg_list_ptr);
};


#endif	_rpcmgr_ifc_h
