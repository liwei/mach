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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_connector.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Connector for local "pipe-style" connections.
 *
 * HISTORY
 * $Log:	pipenet_connector.cc,v $
 * Revision 2.4  94/07/13  17:21:10  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:34:53  dpj
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  01:30:44  dpj]
 * 
 * 	Fixed argument types for pipenet_terminate_connection.
 * 	[92/04/17  16:53:00  dpj]
 * 
 * Revision 2.2  91/11/06  14:20:39  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:52:59  pjg]
 * 
 * Revision 2.3  91/07/01  14:15:01  jms
 * 	Make pipes and such interruptable from DPJ
 * 	[91/06/25  11:42:36  jms]
 * 
 * Revision 2.2  91/05/05  19:32:42  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:47  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:56:29  dpj]
 * 
 */

#ifndef lint
char * pipenet_connector_rcsid = "$Header: pipenet_connector.cc,v 2.4 94/07/13 17:21:10 mrt Exp $";
#endif	lint

#include	<pipenet_connector_ifc.h>
#include	<pipenet_codir_bytes_ifc.h>
#include	<agent_ifc.h>

extern "C" {
#include	<interrupt.h>
}

DEFINE_CLASS_MI(pipenet_connector)
DEFINE_CASTDOWN2(pipenet_connector, pipenet_endpt_base, usNetConnector)

void pipenet_connector::init_class(usClass* class_obj)
{
	usNetConnector::init_class(class_obj);
	pipenet_endpt_base::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_connector);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,net_get_localaddr);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,net_connect);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,net_listen);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,net_accept);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,net_reject);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,net_get_connect_qinfo);
	SETUP_METHOD_WITH_ARGS(pipenet_connector,net_set_connect_qmax);
	END_SETUP_METHOD_WITH_ARGS;
}

pipenet_connector::pipenet_connector()
:
 pipenet_endpt_base(),
 qmax(0), qsize(0), seqno_seed(1), parent_dir(0)
{

	mutex_init(&Local(lock));
	dll_init(&Local(queue));
	condition_init(&Local(listen_cond));
	net_addr_null_init(&Local(localaddr));
}

pipenet_connector::pipenet_connector(ns_mgr_id_t mgr_id, 
				     access_table *access_tab,
				     pipenet_dir_base *_parent_dir,
				     net_addr_t *_localaddr,
				     int qlen)
:
	pipenet_endpt_base(mgr_id,access_tab),
	qmax(qlen), qsize(0), seqno_seed(1)
{

	mutex_init(&Local(lock));
	dll_init(&Local(queue));
	condition_init(&Local(listen_cond));
	net_addr_null_init(&Local(localaddr));
	net_addr_copy(_localaddr,&Local(localaddr));

	mach_object_reference(_parent_dir);
	Local(parent_dir) = _parent_dir;
}


pipenet_connector::~pipenet_connector()
{
	DESTRUCTOR_GUARD();
	(void) this->pipenet_connector::net_set_connect_qmax(0);
	net_addr_destroy(&Local(localaddr));
	mach_object_dereference(Local(parent_dir));
}


char* pipenet_connector::remote_class_name() const
{
	return "usNetConnector_proxy";
}

mach_error_t pipenet_connector::net_get_localaddr(net_addr_t *addr)
{
	net_addr_copy(&Local(localaddr),addr);
	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_connector::net_connect(net_addr_t *peeraddr, net_options_t *options,
			       char *in_udata, unsigned int in_udatalen,
			       char *out_udata, unsigned int *out_udatalen,
			       ns_prot_t prot, unsigned int protlen,
			       ns_access_t access, usItem **newobj,
			       ns_type_t *newtype)
{
	struct pipenet_conninfo		conninfo;
	std_cred			*cred = 0;
	mach_error_t			ret;

#define	ABORT(_ret) {							\
	pipenet_conninfo_destroy(&conninfo);				\
	mach_object_dereference(*newobj);				\
	*newobj = NULL;					\
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
	if (ret != ERR_SUCCESS) ABORT(ret);
	*newobj = agent_obj;
	mach_object_dereference(cred);
	cred = NULL;

	*newtype = conninfo.active_type;

	/*
	 * Clean-up stuff left-over in the connection request record.
	 */
	pipenet_conninfo_destroy(&conninfo);

	return(ERR_SUCCESS);

#undef	ABORT
}


mach_error_t 
pipenet_connector::net_listen(io_mode_t mode, net_addr_t *peeraddr,
			      net_options_t *options, char *udata,
			      unsigned int *udatalen, int *seqno)
{
	pipenet_conninfo_t		conninfo;
	mach_error_t			ret;

	/*
	 * Check mode. Only IOM_WAIT allowed for now.
	 */
	if (mode & ~IOM_WAIT) {
		return(IO_INVALID_MODE);
	}

	mutex_lock(&Local(lock));

	/*
	 * Quick exit for non-blocking option.
	 */
	if (((mode & IOM_WAIT) == 0) && (Local(qsize) == 0)) {
		mutex_unlock(&Local(lock));
		return(US_OBJECT_BUSY);
	}

	/*
	 * Wait until there is something on the queue.
	 */
	while (Local(qsize) == 0) {
		ret = intr_cond_wait(&Local(listen_cond),&Local(lock));
		if (ret != ERR_SUCCESS) {
			mutex_unlock(&Local(lock));
			return(EXCEPT_SOFTWARE);
		}
	}

	/*
	 * Return the connection information to the user.
	 */
	conninfo = (pipenet_conninfo_t) dll_first(&Local(queue));
	if (conninfo->in_udatalen < *udatalen) {
		*udatalen = conninfo->in_udatalen;
		mutex_unlock(&Local(lock));
		return(US_INVALID_BUFFER_SIZE);
	}
	net_addr_copy(conninfo->active_addr,peeraddr);
	net_options_copy(conninfo->options,options);
	bcopy(conninfo->in_udata,udata,conninfo->in_udatalen);
	*udatalen = conninfo->in_udatalen;
	*seqno = conninfo->seqno;

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_connector::net_accept(int seqno, net_options_t *options,
			      char *udata, unsigned int udatalen,
			      ns_prot_t prot, unsigned int protlen,
			      ns_access_t access, usItem **newobj,
			      ns_type_t *newtype)
{
	pipenet_conninfo_t		conninfo;
	std_cred			*cred = 0;
	mach_error_t			ret;

#define	ABORT(_ret) {							\
	mutex_unlock(&Local(lock));					\
	mach_object_dereference(*newobj);				\
	*newobj = NULL;					\
	*newtype = NST_INVALID;						\
	mach_object_dereference(cred);					\
	return(_ret);							\
}

#define	ABORT_WAKEUP(_ret) {						\
	mutex_lock(&Local(lock));					\
	conninfo->state = PIPENET_CONNINFO_REJECTED;			\
	conninfo->error = (_ret);					\
	conninfo->out_udatalen = 0;					\
	mutex_unlock(&Local(lock));					\
	condition_signal(&conninfo->cond);				\
	mach_object_dereference(*newobj);				\
	*newobj = NULL;					\
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

	mutex_lock(&Local(lock));

	/*
	 * Find the desired connection request record.
	 * The sequence number must match.
	 */
	if (Local(qsize) == 0) {
		ABORT(US_OBJECT_NOT_FOUND);
	}
	conninfo = (pipenet_conninfo_t) dll_first(&Local(queue));
	if (conninfo->seqno != seqno) {
		ABORT(US_OBJECT_NOT_FOUND);
	}

	/*
	 * Check that the buffer for user-specified data is big enough.
	 * If not, return without completing the connection.
	 *
	 * XXX Should we complete the connection instead?
	 */
	if (conninfo->out_udatalen < udatalen) {
		ABORT(US_INVALID_BUFFER_SIZE);
	}

	/*
	 * We are committed to handling this connection.
	 * Remove the record from the queue.
	 */
	dll_remove(&Local(queue),conninfo,pipenet_conninfo_t,chain);
	Local(qsize)--;

	mutex_unlock(&Local(lock));

	/*
	 * Update the connection information to be passed back to
	 * the active side.
	 */
	net_options_copy(options,conninfo->options);
	bcopy(udata,conninfo->out_udata,udatalen);
	conninfo->out_udatalen = udatalen;
	conninfo->passive_prot = prot;
	conninfo->passive_protlen = protlen;
	conninfo->state = PIPENET_CONNINFO_ACCEPTING;

	/*
	 * Call on the "protocol" directory to complete the connection.
	 */
	pipenet_codir_bytes *d = pipenet_codir_bytes::castdown(parent_dir);
	if (d) {
		ret = d->pipenet_setup_twoside_connection(conninfo);
	} else {
		ret = _notdef();
	}
	if (ret != ERR_SUCCESS) {
		ABORT_WAKEUP(ret);
	}

	/*
	 * Setup the OUT arguments.
	 */
	net_options_copy(conninfo->options,options);
	ret = agent::base_object()->ns_get_cred_obj(&cred);
	if (ret != ERR_SUCCESS) {
		us_internal_error(
				"net_accept: cannot get current credentials",
				ret);
		ABORT(US_INTERNAL_ERROR);
	}
	agent *agent_obj;
	ret = conninfo->passive_endpt->ns_create_agent(access,cred,&agent_obj);
	if (ret != ERR_SUCCESS) ABORT_WAKEUP(ret);
	*newobj = agent_obj;
	mach_object_dereference(cred);
	cred = NULL;

	*newtype = conninfo->passive_type;

	/*
	 * Wake-up the active side.
	 */
	mutex_lock(&Local(lock));
	conninfo->error = ERR_SUCCESS;
	conninfo->state = PIPENET_CONNINFO_ACCEPTED;
	mutex_unlock(&Local(lock));
	condition_signal(&conninfo->cond);

	return(ERR_SUCCESS);

#undef	ABORT
#undef	ABORT_WAKEUP
}


mach_error_t 
pipenet_connector::net_reject(int seqno, char *udata, unsigned int udatalen)
{
	pipenet_conninfo_t		conninfo;
	mach_error_t			ret;

	mutex_lock(&Local(lock));

	/*
	 * Find the desired connection request record.
	 * The sequence number must match.
	 */
	if (Local(qsize) == 0) {
		mutex_unlock(&Local(lock));
		return(US_OBJECT_NOT_FOUND);
	}
	conninfo = (pipenet_conninfo_t) dll_first(&Local(queue));
	if (conninfo->seqno != seqno) {
		mutex_unlock(&Local(lock));
		return(US_OBJECT_NOT_FOUND);
	}

	/*
	 * Check that the buffer for user-specified data is big enough.
	 * If not, return without breaking the connection.
	 *
	 * XXX Should we still break the connection anyway?
	 */
	if (conninfo->out_udatalen < udatalen) {
		mutex_unlock(&Local(lock));
		return(US_INVALID_BUFFER_SIZE);
	}

	/*
	 * We are committed to breaking this connection.
	 * Remove the record from the queue.
	 */
	dll_remove(&Local(queue),conninfo,pipenet_conninfo_t,chain);
	Local(qsize)--;

	/*
	 * Update the connection information to be passed back to
	 * the active side.
	 */
	bcopy(udata,conninfo->out_udata,udatalen);
	conninfo->out_udatalen = udatalen;
	conninfo->error = NET_CONNECTION_REFUSED;
	conninfo->state = PIPENET_CONNINFO_REJECTED;

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up the active side.
	 */
	condition_signal(&conninfo->cond);

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_connector::net_get_connect_qinfo(int *qsize, int *qmax)
{
	mutex_lock(&Local(lock));

	*qsize = Local(qsize);
	*qmax = Local(qmax);

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_connector::net_set_connect_qmax(int qmax)
{
	pipenet_conninfo_t		conninfo;

	if (Local(qmax) < 0) {
		return(US_INVALID_ARGS);
	}

	mutex_lock(&Local(lock));

	Local(qmax) = qmax;

	while (Local(qsize) > Local(qmax)) {
		conninfo = (pipenet_conninfo_t) dll_first(&Local(queue));
		dll_remove(&Local(queue),conninfo,pipenet_conninfo_t,chain);
		Local(qsize)--;

		mutex_unlock(&Local(lock));

		conninfo->out_udatalen = 0;
		conninfo->error = NET_CONNECTION_REFUSED;
		conninfo->state = PIPENET_CONNINFO_REJECTED;
		condition_signal(&conninfo->cond);

		mutex_lock(&Local(lock));
	}

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_connector::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

//	ret = invoke_super(Super,mach_method_id(ns_get_attributes),
//							attr,attrlen);
	ret = net_endpt_base::ns_get_attributes(attr, attrlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	attr->type = NST_CONNECTOR;

	/*
	 * XXX Should return the number of pending connections.
	 */

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_connector::pipenet_connect_upcall(pipenet_conninfo_t conninfo)
{
	mach_error_t		ret;

	mutex_lock(&Local(lock));

	/*
	 * Enforce limit on number of pending connections.
	 */
	if (Local(qsize) >= Local(qmax)) {
		mutex_unlock(&Local(lock));
		conninfo->out_udatalen = 0;
		return(NET_CONNECTION_REFUSED);
	}

	/*
	 * Place a new request at the tail of the queue.
	 */
	conninfo->state = PIPENET_CONNINFO_LISTENING;
	conninfo->seqno = Local(seqno_seed)++;
	conninfo->error = US_OBJECT_NOT_STARTED;
	dll_enter(&Local(queue),conninfo,pipenet_conninfo_t,chain);
	Local(qsize)++;

	/*
	 * Wake-up any potential users waiting in net_listen().
	 */
	mutex_unlock(&Local(lock));
	condition_signal(&Local(listen_cond));
	mutex_lock(&Local(lock));

	/*
	 * Wait until the client accepts or rejects the request.
	 */
	while (conninfo->error == US_OBJECT_NOT_STARTED) {
		ret = intr_cond_wait(&conninfo->cond,&Local(lock));
		if ((ret != ERR_SUCCESS) &&
				(conninfo->error == US_OBJECT_NOT_STARTED)) {
			dll_remove(&Local(queue),conninfo,
						pipenet_conninfo_t,chain);
			Local(qsize)--;
			conninfo->error = EXCEPT_SOFTWARE;
			mutex_unlock(&Local(lock));
			return(EXCEPT_SOFTWARE);
		}
	}

	mutex_unlock(&Local(lock));

	return(conninfo->error);
}


mach_error_t 
pipenet_connector::pipenet_terminate_connection(net_addr_t *peeraddr,
						net_endpt_base *endpt)
{
	/*
	 * We don't care...
	 */

	return(ERR_SUCCESS);
}

