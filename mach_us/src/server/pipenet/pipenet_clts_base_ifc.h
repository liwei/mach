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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_clts_base_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose:	common base class for local "pipe-style" connection-less
 *		endpoints and pseudo-connectors.
 *		Takes care of the "connection" aspect of the endpoint.
 *
 * HISTORY
 * $Log:	pipenet_clts_base_ifc.h,v $
 * Revision 2.6  94/07/13  17:20:53  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/05/17  14:10:06  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/29  13:40:07  jms]
 * 
 * Revision 2.4  92/07/05  23:34:39  dpj
 * 	tmp_cleanup_for_shutdown -> ns_tmp_cleanup_for_shutdown
 * 	[92/06/24  17:19:41  jms]
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:30:08  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:39:33  dpj]
 * 
 * Revision 2.3  91/11/06  14:20:18  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:52:02  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:30  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:08  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:54:58  dpj]
 * 
 */

#ifndef	_pipenet_clts_base_ifc_h
#define	_pipenet_clts_base_ifc_h


#include	<us_net_clts_ifc.h>
#include	<pipenet_endpt_base_ifc.h>
#include	<pipenet_dir_base_ifc.h>
#include	<stream_base_ifc.h>

extern "C" {
#include	<macro_help.h>
#include	<dll.h>
#include	<net_types.h>
}

/************************************************************************\
 *									*
 *	Handling of the "peer" object in connection-less operation.	*
 *	The parent directory is used to find destinations, but		*
 *	the endpoint itself maintains a cache of frequently-used	*
 *	destinations.							*
 *									*
\************************************************************************/

/*
 * Record for current remote object.
 *
 * XXX Should cache more than one destination?
 */
typedef struct pipenet_curpeer {
	net_addr_t	addr;
	net_endpt_base	*obj;
} *pipenet_curpeer_t;

/************************************************************************\
 *									*
 *	Class declaration.						*
 *									*
\************************************************************************/

class pipenet_clts_base: public pipenet_endpt_base, public usNetCLTS {
      protected:
	struct mutex		lock;
	unsigned int		readers_count;
	pipenet_dir_base	*parent_dir;
	stream_base		*incoming_stream;
	net_addr_t		localaddr;
	struct pipenet_curpeer	curpeer;
	dll_head_t		connrecs;
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(pipenet_clts_base);

	pipenet_clts_base(ns_mgr_id_t =null_mgr_id, access_table * =0,
			  pipenet_dir_base * =0, net_addr_t * =0,
			  stream_base * =0);
	~pipenet_clts_base();

REMOTE	virtual mach_error_t ns_register_agent(ns_access_t);
REMOTE	virtual mach_error_t ns_unregister_agent(ns_access_t);
REMOTE	virtual mach_error_t net_get_localaddr(net_addr_t *);
REMOTE	virtual mach_error_t net_connect(net_addr_t *, net_options_t *,
			       char *, unsigned int, char *, unsigned int *,
			       ns_prot_t, unsigned int, ns_access_t, usItem **,
			       ns_type_t *);

	virtual mach_error_t pipenet_connect_upcall(pipenet_conninfo_t);
	virtual mach_error_t pipenet_snddis_upcall(net_addr_t *, char *,
						   unsigned int);

	virtual mach_error_t ns_tmp_cleanup_for_shutdown();
	virtual mach_error_t pipenet_terminate_connection(net_addr_t *,
							  net_endpt_base *);

	/*
	 * From usNetConnector but not implemented
	 */
	virtual mach_error_t net_listen(io_mode_t, net_addr_t*, 
					net_options_t*, char*,
					unsigned int*, int*);
	virtual mach_error_t net_accept(int, net_options_t*, char*,
					unsigned int, ns_prot_t,
					unsigned int, ns_access_t,
					usItem**, ns_type_t*);
	virtual mach_error_t net_reject(int, char*, unsigned int);
	virtual mach_error_t net_get_connect_qinfo(int*, int*);
	virtual mach_error_t net_set_connect_qmax(int);

};


/************************************************************************\
 *									*
 *	Handling of pseudo-connected endpoints attached to this		*
 *	endpoint. The CLTS endpoint is responsible for multiplexing	*
 *	incoming messages between itself and the various pseudo-COTS	*
 *	endpoint.							*
 *									*
\************************************************************************/

/*
 * List of pseudo-connected endpoints attached to this endpoint.
 *
 * XXX Should be turned into a hash table for better performance?
 */
typedef struct pipenet_connrec {
	dll_chain_t		chain;
	net_addr_t		addr;
	net_endpt_base		*obj;
} *pipenet_connrec_t;

/*
 * Find a pseudo-connection record attached to this endpoint.
 */
#define	FIND_CONN_REC(_addr,_rec)					\
MACRO_BEGIN								\
	/* XXX MUST BE CALLED WITH THE OBJECT LOCKED */			\
									\
	pipenet_connrec_t	_currec;				\
									\
	_currec = (pipenet_connrec_t) dll_first(&Local(connrecs));\
	if (dll_end(&Local(connrecs),(dll_entry_t)_currec)) {	\
		(_rec) = NULL;						\
	} else if (net_addr_equal((_addr),&_currec->addr)) {		\
		(_rec) = _currec;					\
	} else {							\
		(_rec) = NULL;						\
		while (! dll_end(&Local(connrecs),		\
					(dll_entry_t)_currec)) {	\
			if (net_addr_equal((_addr),&_currec->addr)) {	\
				dll_remove(&Local(connrecs),	\
					_currec,pipenet_connrec_t,chain);\
				dll_enter_first(&Local(connrecs),	\
					_currec,pipenet_connrec_t,chain);\
				(_rec) = _currec;			\
				break;					\
			}						\
			_currec = (pipenet_connrec_t)			\
				dll_next((dll_entry_t)&_currec->chain);	\
		}							\
	}								\
MACRO_END

/*
 * Find a pseudo-connection endpoint attached to this endpoint.
 */
#define	FIND_CONN_ENDPT(_addr,_obj)					\
MACRO_BEGIN								\
	pipenet_connrec_t	_rec;					\
									\
	mutex_lock(&Local(lock));					\
									\
	FIND_CONN_REC((_addr),_rec);					\
	if (_rec != NULL) {						\
		(_obj) = _rec->obj;					\
		mach_object_reference(_obj);				\
	} else {							\
		(_obj) = NULL;				\
	}								\
									\
	mutex_unlock(&Local(lock));				\
MACRO_END

/*
 * Release a pseudo-connection endpoint obtained from FIND_CONN_ENDPT().
 */
#define	RELEASE_CONN_ENDPT(_obj)					\
MACRO_BEGIN								\
	mach_object_dereference(_obj);					\
MACRO_END



/*
 * Find a peer endpoint with a given address.
 */
#define	FIND_PEER(_addr,_obj,_ret)					\
MACRO_BEGIN								\
	mutex_lock(&Local(lock));					\
									\
	if ((Local(curpeer).obj == NULL) ||			\
			(! net_addr_equal((_addr),			\
					&Local(curpeer).addr))) {	\
		mach_object_dereference(Local(curpeer).obj);		\
		_ret = Local(parent_dir)->pipenet_find_peer(&Local(localaddr),\
					 (_addr),			\
					 &Local(curpeer).obj);		\
		if ((_ret) == ERR_SUCCESS) {				\
			net_addr_destroy(&Local(curpeer).addr);		\
			net_addr_copy((_addr),&Local(curpeer).addr);	\
			(_obj) = Local(curpeer).obj;			\
			mach_object_reference(_obj);			\
		} else {						\
			Local(curpeer).obj = NULL;	\
			(_obj) = NULL;			\
		}							\
	} else {							\
		(_obj) = Local(curpeer).obj;			\
		mach_object_reference(_obj);				\
		(_ret) = ERR_SUCCESS;					\
	}								\
									\
	mutex_unlock(&Local(lock));				\
MACRO_END

/*
 * Release an endpoint obtained from FIND_PEER().
 */
#define	RELEASE_PEER(_obj)						\
MACRO_BEGIN								\
	mach_object_dereference(_obj);					\
MACRO_END

/*
 * Release an endpoint obtained from FIND_PEER(), and clear the cache.
 */
#define	CLEAR_PEER(_obj)						\
MACRO_BEGIN								\
	mutex_lock(&Local(lock));					\
	if (Local(curpeer).obj == (_obj)) {			\
		mach_object_dereference(_obj);				\
		Local(curpeer).obj = NULL;		\
		net_addr_destroy(&Local(curpeer).addr);		\
	}								\
	mutex_unlock(&Local(lock));				\
	mach_object_dereference(_obj);					\
MACRO_END




#endif	_pipenet_clts_base_ifc_h
