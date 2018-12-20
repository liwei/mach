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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_cots_base.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose:	common base class for local "pipe-style" connection-oriented
 *		endpoints.
 *
 * HISTORY
 * $Log:	pipenet_cots_base.cc,v $
 * Revision 2.4  94/07/13  17:21:18  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:34:58  dpj
 * 	tmp_cleanup_for_shutdown -> ns_tmp_cleanup_for_shutdown
 * 	[92/06/24  17:23:14  jms]
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  01:30:57  dpj]
 * 
 * Revision 2.2  91/11/06  14:20:46  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:53:16  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:48  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:53  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:57:18  dpj]
 * 
 *
 */

#include	<pipenet_cots_base_ifc.h>
#include	<pipenet_connector_ifc.h>

DEFINE_ABSTRACT_CLASS_MI(pipenet_cots_base)
DEFINE_CASTDOWN2(pipenet_cots_base, pipenet_endpt_base, usNetCOTS)


void pipenet_cots_base::init_class(usClass* class_obj)
{
	usNetCOTS::init_class(class_obj);
	pipenet_endpt_base::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_cots_base);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_base,net_get_localaddr);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_base,net_get_peeraddr);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_base,net_snddis);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_base,net_rcvdis);
	END_SETUP_METHOD_WITH_ARGS;
}

pipenet_cots_base::pipenet_cots_base(ns_mgr_id_t mgr_id, 
				     access_table *access_tab,
				     pipenet_dir_base *_parent_dir,
				     net_addr_t *_localaddr,
				     net_addr_t *_peeraddr,
				     net_endpt_base *_connector,
				     net_endpt_base *_peerobj,
				     stream_base *_incoming_stream)
:
 pipenet_endpt_base(mgr_id,access_tab),
 costate(NET_COSTATE_CONNECTED), disconn_data(NULL), disconn_datalen(0),
 incoming_stream(_incoming_stream), parent_dir(_parent_dir), peerobj(_peerobj),
 connector(_connector)
{
	net_addr_null_init(&Local(localaddr));
	net_addr_copy(_localaddr,&Local(localaddr));
	net_addr_null_init(&Local(peeraddr));
	net_addr_copy(_peeraddr,&Local(peeraddr));

	mach_object_reference(incoming_stream);
	mach_object_reference(parent_dir);
	mach_object_reference(peerobj);

	if (connector) {
		mach_error_t ret = connector->ns_register_stronglink();
		if (ret != ERR_SUCCESS) {
			Local(connector) = 0;
		}
	}
}

pipenet_cots_base::~pipenet_cots_base()
{
	DESTRUCTOR_GUARD();
	net_addr_destroy(&Local(localaddr));
	net_addr_destroy(&Local(peeraddr));
	if (Local(connector) != 0) {
		(void) Local(connector)->ns_unregister_stronglink();
	}
	mach_object_dereference(Local(parent_dir));
	mach_object_dereference(Local(peerobj));
	mach_object_dereference(Local(incoming_stream));
	if (Local(disconn_data) != NULL) {
		Free(Local(disconn_data));
	}
}


mach_error_t pipenet_cots_base::ns_tmp_cleanup_for_shutdown(void)
{
	mach_error_t		ret;

	if (Local(costate) == NET_COSTATE_CONNECTED) {
		(void) this->net_snddis(NULL,0);
	}

	tmp_agency::ns_tmp_cleanup_for_shutdown();

	return(ret);
}


/*
 * Exported client interface.
 */

mach_error_t pipenet_cots_base::net_get_localaddr(net_addr_t *addr)
{
	net_addr_copy(&Local(localaddr),addr);

	return(ERR_SUCCESS);
}


mach_error_t pipenet_cots_base::net_get_peeraddr(net_addr_t *addr)
{
	net_addr_copy(&Local(peeraddr),addr);

	return(ERR_SUCCESS);
}


mach_error_t pipenet_cots_base::net_snddis(char *udata, unsigned int udatalen)
{
	mach_error_t		ret;

	if (Local(costate) != NET_COSTATE_CONNECTED) {
		return(NET_NOT_CONNECTED);
	}

	/*
	 * Notify the other end.
	 */
	pipenet_endpt_base *p = pipenet_endpt_base::castdown(peerobj);
	if (p) {
		ret = p->pipenet_snddis_upcall(&localaddr, udata, udatalen);
	} else {
		ret = _notdef();
	}
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	Local(costate) = NET_COSTATE_DISCONNECTED;

	/*
	 * Make sure no one can read/write data here; wakeup
	 * threads blocked in pending I/O operations.
	 */
	(void) Local(incoming_stream)->io_set_read_strategy(0);
	(void) Local(incoming_stream)->io_set_write_strategy(0);
	(void) Local(incoming_stream)->io_flush_stream();

	/*
	 * Notify the connector.
	 */
	p = pipenet_endpt_base::castdown(connector);
	if (p) {
		(void) p->pipenet_terminate_connection(&peeraddr, this);
	}
	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_cots_base::net_rcvdis(char *udata, unsigned int *udatalen)
{
	if (Local(costate) != NET_COSTATE_DISCONNECTED) {
		*udatalen = 0;
		return(NET_IS_CONNECTED);
	}

	if (*udatalen < Local(disconn_datalen)) {
		*udatalen = Local(disconn_datalen);
		return(US_INVALID_BUFFER_SIZE);
	}

	bcopy(Local(disconn_data),udata,Local(disconn_datalen));
	*udatalen = Local(disconn_datalen);

	return(ERR_SUCCESS);
}


/*
 * Upcalls from the other side of the connection.
 */

mach_error_t 
pipenet_cots_base::pipenet_snddis_upcall(net_addr_t *peeraddr, char *udata,
					 unsigned int udatalen)
{
	if (Local(costate) != NET_COSTATE_CONNECTED) {
		return(NET_NOT_CONNECTED);
	}

	/*
	 * Remember the disconnection.
	 */
	Local(costate) = NET_COSTATE_DISCONNECTED;
	if (udatalen > 0) {
		Local(disconn_data) = Malloc(udatalen);
		Local(disconn_datalen) = udatalen;
		bcopy(udata,Local(disconn_data));
	}

	/*
	 * Disable writing from the other side, just in case.
	 * This should not be necessary.
	 */
	(void) Local(incoming_stream)->io_set_write_strategy(0);

	/*
	 * Let the client drain the data in the buffer,
	 * but do not let him/her wait for more (~IOS_WAIT_ALLOWED).
	 *
	 * XXX This may be UNIX-specific.
	 *
	 * XXX Should notify the client with a signal/callback...
	 */
	(void) Local(incoming_stream)->io_set_read_strategy(IOS_ENABLED);

	/*
	 * Release the peer reference.
	 */
	mach_object_dereference(Local(peerobj));
	Local(peerobj) = NULL;

	return(ERR_SUCCESS);
}


net_endpt_base *
pipenet_cots_base::set_peerobj(net_endpt_base* _peerobj)
{
	peerobj = _peerobj;
	mach_object_reference(_peerobj);
}
