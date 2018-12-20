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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_clts_base.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: common base class for local "pipe-style" connection-less
 *		endpoints and pseudo-connectors
 *
 * HISTORY
 * $Log:	pipenet_clts_base.cc,v $
 * Revision 2.4  94/07/13  17:20:50  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:34:36  dpj
 * 	tmp_cleanup_for_shutdown -> ns_tmp_cleanup_for_shutdown
 * 	[92/06/24  17:19:06  jms]
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:29:57  dpj]
 * 
 * Revision 2.2  91/11/06  14:20:13  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:51:43  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:22  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:06:59  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:54:41  dpj]
 * 
 *
 */

#include	<pipenet_clts_base_ifc.h>
#include	<pipenet_connector_ifc.h>
#include	<agent_ifc.h>

DEFINE_ABSTRACT_CLASS_MI(pipenet_clts_base)
DEFINE_CASTDOWN2(pipenet_clts_base, usNetCLTS, pipenet_endpt_base)


void pipenet_clts_base::init_class(usClass* class_obj)
{
	usNetCLTS::init_class(class_obj);
	pipenet_endpt_base::init_class(class_obj);
}


pipenet_clts_base::pipenet_clts_base(ns_mgr_id_t mgr_id, 
				     access_table *access_tab,
				     pipenet_dir_base *_parent_dir,
				     net_addr_t *_localaddr,
				     stream_base *_incoming_stream)
:
 pipenet_endpt_base(mgr_id, access_tab),
 readers_count(0), parent_dir(_parent_dir), incoming_stream(_incoming_stream)
{

	mutex_init(&Local(lock));
	mach_object_reference(parent_dir);
	mach_object_reference(incoming_stream);

	net_addr_null_init(&Local(localaddr));
	net_addr_copy(_localaddr,&Local(localaddr));

	curpeer.obj = 0;
	net_addr_pipe_init_default(&Local(curpeer).addr);
	dll_init(&Local(connrecs));
}


pipenet_clts_base::~pipenet_clts_base()
{
	pipenet_connrec_t	connrec;

	DESTRUCTOR_GUARD();

	mach_object_dereference(Local(parent_dir));
	mach_object_dereference(Local(incoming_stream));

	net_addr_destroy(&Local(localaddr));
	if (Local(curpeer).obj != 0) {
		CLEAR_PEER(Local(curpeer).obj);
		net_addr_destroy(&Local(curpeer).addr);
	}

	connrec = (pipenet_connrec_t) dll_first(&Local(connrecs));
	while (! dll_end(&Local(connrecs),(dll_entry_t)connrec)) {
		dll_remove(&Local(connrecs),connrec,pipenet_connrec_t,chain);
		net_addr_destroy(&connrec->addr);
		mach_object_dereference(connrec->obj);
		Free(connrec);
		connrec = (pipenet_connrec_t) dll_first(&Local(connrecs));
	}
}


mach_error_t pipenet_clts_base::ns_tmp_cleanup_for_shutdown()
{
	mach_error_t		ret;

	/*
	 * Everything should already have been taken care of
	 * when the last agent was destroyed.
	 */

	tmp_agency::ns_tmp_cleanup_for_shutdown();

	return(ret);
}


mach_error_t pipenet_clts_base::ns_register_agent(ns_access_t access)
{
	mach_error_t		ret;

	if (access & NSR_READ) {
		mutex_lock(&Local(lock));

		Local(readers_count)++;

		if (Local(readers_count) == 1) {
			(void) incoming_stream->io_set_write_strategy(
					IOS_ENABLED | IOS_WAIT_ALLOWED);
		}

		mutex_unlock(&Local(lock));
	}

//	ret = invoke_super_with_base(Super,Base,
//				mach_method_id(ns_register_agent),access);
	ret = tmp_agency::ns_register_agent(access);

	return(ret);
}


mach_error_t pipenet_clts_base::ns_unregister_agent(ns_access_t access)
{
	mach_error_t		ret;

//	ret = invoke_super_with_base(Super,Base,
//				mach_method_id(ns_unregister_agent),access);
	ret = tmp_agency::ns_unregister_agent(access);

	if (access & NSR_READ) {
		mutex_lock(&Local(lock));

		Local(readers_count)--;

		if (Local(readers_count) == 0) {
			(void) incoming_stream->io_set_read_strategy(0);
			(void) incoming_stream->io_set_write_strategy(0);
			(void) incoming_stream->io_flush_stream();
		}

		mutex_unlock(&Local(lock));
	}

	return(ret);
}


/*
 * Exported client interface.
 */

mach_error_t pipenet_clts_base::net_get_localaddr(net_addr_t *addr)
{
	net_addr_copy(&Local(localaddr),addr);

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_clts_base::net_connect(net_addr_t *peeraddr, net_options_t *options,
			       char *in_udata, unsigned int in_udatalen,
			       char *out_udata, unsigned int *out_udatalen,
			       ns_prot_t prot, unsigned int protlen,
			       ns_access_t access, usItem **newobj,
			       ns_type_t *newtype)
{
	struct pipenet_conninfo		conninfo;
	pipenet_connrec_t		connrec;
	std_cred			*cred = 0;
	mach_error_t			ret;

#define	ABORT(_ret) {							\
	pipenet_conninfo_destroy(&conninfo);				\
	mach_object_dereference(*newobj);				\
	*newobj = 0;							\
	*newtype = NST_INVALID;						\
	mach_object_dereference(cred);					\
	return(_ret);							\
}

	*newobj = NULL;

	/*
	 * No use doing anything if not returning an agent, since
	 * the endpoint would just disappear at once.
	 */
	if (access == 0) {
		*newtype = NST_INVALID;
		return(US_UNSUPPORTED);
	}

	/*
	 * Prepare a connection request record to be exchanged
	 * and filled between the active and passive sides.
	 */
	conninfo.active_addr = &Local(localaddr);
	conninfo.passive_addr = peeraddr;
	conninfo.options = options;
	conninfo.in_udata = in_udata;
	conninfo.in_udatalen = in_udatalen;
	conninfo.out_udata = out_udata;
	conninfo.out_udatalen = *out_udatalen;
	conninfo.active_connector = this;
	mach_object_reference(conninfo.active_connector);
	conninfo.active_prot = prot;
	conninfo.active_protlen = protlen;
	condition_init(&conninfo.cond);
	conninfo.state = PIPENET_CONNINFO_INITIAL;

	conninfo.passive_connector = NULL;
	conninfo.passive_prot = NULL;
	conninfo.passive_protlen = 0;
	conninfo.active_endpt = NULL;
	conninfo.active_type = 0;
	conninfo.passive_endpt = NULL;
	conninfo.passive_type = 0;
	dll_init(&conninfo.chain);
	conninfo.seqno = 0;
	conninfo.error = 0;

	/*
	 * Reserve a record for the new connection.
	 * Just leave the "obj" field null to prevent forwarding to the
	 * new endpoint.
	 */
	connrec = New(struct pipenet_connrec);
	connrec->obj = NULL;
	net_addr_null_init(&connrec->addr);
	net_addr_copy(conninfo.passive_addr,&connrec->addr);
	mutex_lock(&Local(lock));
	dll_enter_first(&Local(connrecs),connrec,pipenet_connrec_t,chain);
	mutex_unlock(&Local(lock));

	/*
	 * Call on the "protocol" directory to initiate the connection.
	 */
	ret = parent_dir->pipenet_request_connection(&conninfo);
	if (ret != ERR_SUCCESS) {
		if (ret == NET_CONNECTION_REFUSED) {
			*out_udatalen = conninfo.out_udatalen;
		} else {
			*out_udatalen = 0;
		}
		ABORT(ret);
	}

	/*
	 * Setup the OUT arguments.
	 *
	 * Most of them have already been copied in-place from the
	 * connection request record.
	 */
	*out_udatalen = conninfo.out_udatalen;
	ret = agent::base_object()->ns_get_cred_obj(&cred);
	if (ret != ERR_SUCCESS) {
		us_internal_error(
				"net_connect: cannot get current credentials",
				ret);
		ABORT(US_INTERNAL_ERROR);
	}
	agent *agent_obj;
	ret = conninfo.active_endpt->ns_create_agent(access,cred,&agent_obj);
	*newobj = agent_obj;
	if (ret != ERR_SUCCESS) ABORT(ret);
	mach_object_dereference(cred);
	cred = NULL;

	*newtype = conninfo.active_type;

	/*
	 * Finish setting-up the new connection record.
	 */
	mutex_lock(&Local(lock));
	connrec->obj = conninfo.active_endpt;
	mach_object_reference(connrec->obj);
	mutex_unlock(&Local(lock));

	/*
	 * Clean-up stuff left-over in the connection request record.
	 */
	pipenet_conninfo_destroy(&conninfo);

	return(ERR_SUCCESS);

#undef	ABORT
}


/*
 * Upcalls from other endpoints.
 */

mach_error_t 
pipenet_clts_base::pipenet_connect_upcall(pipenet_conninfo_t conninfo)
{
	mach_error_t		ret;

	/*
	 * We always accept connections.
	 */

	conninfo->out_udatalen = 0;
	conninfo->passive_endpt = this;
	mach_object_reference(this);
	conninfo->state = PIPENET_CONNINFO_ACCEPTING;

	ret = parent_dir->pipenet_setup_oneside_connection(conninfo);
	if (ret != ERR_SUCCESS) {
		conninfo->state = PIPENET_CONNINFO_REJECTED;
	}

	conninfo->state = PIPENET_CONNINFO_ACCEPTED;

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_clts_base::pipenet_snddis_upcall(net_addr_t *peeraddr, char *udata,
					 unsigned int udatalen)
{
	mutex_lock(&Local(lock));

	if ((Local(curpeer).obj != NULL) &&
		net_addr_equal(peeraddr,&Local(curpeer).addr)) {
		mach_object_dereference(Local(curpeer).obj);
		Local(curpeer).obj = NULL;
		net_addr_destroy(&Local(curpeer).addr);
	}

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_clts_base::pipenet_terminate_connection(net_addr_t *peeraddr,
						net_endpt_base *endpt)
{
	pipenet_connrec_t	connrec;

	mutex_lock(&Local(lock));

	FIND_CONN_REC(peeraddr,connrec);
	if (connrec == NULL) {
		us_internal_error(
	"pipenet_terminate_connection() could not find matching peer address",
						US_INTERNAL_ERROR);
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	if (connrec->obj != endpt) {
		us_internal_error(
		"pipenet_terminate_connection() mismatch with object pointer",
						US_INTERNAL_ERROR);
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	dll_remove(&Local(connrecs),connrec,pipenet_connrec_t,chain);
	mach_object_dereference(connrec->obj);
	net_addr_destroy(&connrec->addr);
	Free(connrec);

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}

mach_error_t 
pipenet_clts_base::net_listen(io_mode_t, net_addr_t*, 
					net_options_t*, char*,
					unsigned int*, int*)
{
	return _notdef();
}

mach_error_t 
pipenet_clts_base::net_accept(int, net_options_t*, char*,
					unsigned int, ns_prot_t,
					unsigned int, ns_access_t,
					usItem**, ns_type_t*)
{
	return _notdef();
}

mach_error_t 
pipenet_clts_base::net_reject(int, char*, unsigned int)
{
	return _notdef();
}

mach_error_t 
pipenet_clts_base::net_get_connect_qinfo(int*, int*)
{
	return _notdef();
}

mach_error_t 
pipenet_clts_base::net_set_connect_qmax(int)
{
	return _notdef();
}

