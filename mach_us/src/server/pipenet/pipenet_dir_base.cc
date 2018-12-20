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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_dir_base.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for directories in the pipenet server.
 *
 * HISTORY
 * $Log:	pipenet_dir_base.cc,v $
 * Revision 2.5  94/07/13  17:21:33  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  94/01/11  18:11:14  jms
 * 	Call net_dir_base::remote_class_name in pipenet_dir_base::remote_class_name
 * 	[94/01/10  13:28:44  jms]
 * 
 * Revision 2.3  92/07/05  23:35:05  dpj
 * 	tmp_cleanup_for_shutdown -> ns_tmp_cleanup_for_shutdown
 * 	[92/06/24  17:24:03  jms]
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  01:31:19  dpj]
 * 
 * Revision 2.2  91/11/06  14:21:31  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:54:17  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:09  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:26  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:59:41  dpj]
 * 
 */

#include	<pipenet_dir_base_ifc.h>
#include	<pipenet_endpt_base_ifc.h>
#include	<pipenet_connector_ifc.h>

#define BASE net_dir_base
DEFINE_ABSTRACT_CLASS(pipenet_dir_base)

pipenet_dir_base::pipenet_dir_base(ns_mgr_id_t mgr_id,
				   access_table *access_tab,
				   net_info_t *info)
:
 net_dir_base(mgr_id,access_tab,info),
 name_seed(1)
{
	mutex_init(&Local(lock));

//	new_object(Local(iobuf_mgr),default_iobuf_mgr);
	iobuf_mgr = new default_iobuf_mgr;
//	(void) io_set_rec_infosize(Local(iobuf_mgr),sizeof(pipenet_recinfo_t));
	(void) iobuf_mgr->io_set_rec_infosize(sizeof(pipenet_recinfo_t));
}

pipenet_dir_base::~pipenet_dir_base()
{
	DESTRUCTOR_GUARD();
	mach_object_dereference(Local(iobuf_mgr));
}


#ifdef	GXXBUG_VIRTUAL1
char* pipenet_dir_base::remote_class_name() const
	{ return net_dir_base::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1

/*
 * Check that a local address supplied by a client is valid,
 * and assign a new address if necessary.
 */
mach_error_t pipenet_dir_base::pipenet_check_localaddr(net_addr_t *localaddr)
{
	if (!net_addr_pipe_p(localaddr)) return(NET_INVALID_ADDR_FLAVOR);

	if (net_addr_pipe_default_p(localaddr)) {
		mutex_lock(&Local(lock));
		net_addr_pipe_init_random(localaddr,Local(name_seed));
		Local(name_seed)++;
		mutex_unlock(&Local(lock));
	}

	return(ERR_SUCCESS);
}


/*
 * Attempt to create a connection with associated COTS endpoint(s).
 *
 * Called by a connector object as part of processing a net_connect()
 * call from the client.
 */
mach_error_t 
pipenet_dir_base::pipenet_request_connection(pipenet_conninfo_t	conninfo)
{
	ns_name_t		newname;
	unsigned int		namelen;
	ns_name_t		peername;
	int			tag = 0;
	ns_type_t		type;
	mach_error_t		ret;

#define	ABORT(_ret) {							\
	if (tag) (void) this->ns_cancel_entry(tag);			\
	return(_ret);							\
}

	/*
	 * Prepare the full active endpoint name.
	 */
	ret = net_addr_pipe_get_stringname(conninfo->active_addr,
						newname,sizeof(ns_name_t));
	if (ret != ERR_SUCCESS) {
		us_internal_error(
	"net_request_connection: cannot get stringname for local address",
			ret);
		ABORT(US_INTERNAL_ERROR);
	}
	namelen = strlen(newname);
	newname[namelen++] = '-';
	ret = net_addr_pipe_get_stringname(conninfo->passive_addr,
				&newname[namelen],sizeof(ns_name_t) - namelen);
	if (ret != ERR_SUCCESS) ABORT(ret);

	/*
	 * Reserve the active_name in the directory.
	 *
	 * No need to reserve a passive_name right now; we can wait until
	 * the connection is accepted. Besides, we may not even create
	 * a new passive endpoint (for one-sided connections).
	 *
	 * Note: this will cause lookups for the active_name to
	 * block until the connection is established or rejected.
	 * XXX Should we export a "CONNECTING" endpoint instead?
	 */
	ret = this->ns_reserve_entry(newname,&tag);
	if (ret != ERR_SUCCESS) ABORT(ret);

	/*
	 * Find a "passive" connector willing to accept to this connection.
	 */
	ret = net_addr_pipe_get_stringname(conninfo->passive_addr,peername,
							sizeof(ns_name_t));
	if (ret != ERR_SUCCESS) ABORT(ret);
	agency *agency_obj;
	ret = this->ns_lookup_entry(peername,strlen(peername),
					     &agency_obj,&type);
	if (ret == ERR_SUCCESS) {
		if ((conninfo->passive_connector = pipenet_connector::castdown(agency_obj)) == 0) {
			ret = MACH_OBJECT_NO_SUCH_OPERATION;
		}
	}
	if (ret != ERR_SUCCESS) ABORT(ret);

	/*
	 * Offer the connection to the remote connector, and wait for
	 * a response.
	 */
	pipenet_endpt_base *p = pipenet_endpt_base::castdown(conninfo->passive_connector);
	if (p) {
		ret = p->pipenet_connect_upcall(conninfo);
	} else {
		ret = _notdef();
	}
	if (ret != ERR_SUCCESS) ABORT(ret);

	/*
	 * At this point, we know that the connection is accepted, and
	 * both endpoints have been created (or simply set-up) by the
	 * passive-side connector. All we have left to do is finish
	 * entering the active endpoint in the directory, and return
	 * it to the client.
	 */

	ret = conninfo->active_endpt->ns_set_protection(
			conninfo->active_prot,conninfo->active_protlen);
	if (ret != ERR_SUCCESS) ABORT(ret);

	ret = this->ns_install_entry(tag,
			conninfo->active_endpt,conninfo->active_type);
	if (ret != ERR_SUCCESS) ABORT(ret);
	tag = 0;

	/*
	 * Let the caller get rid of all the MachObjects references
	 * acquired along the way.
	 */

	return(ERR_SUCCESS);

#undef	ABORT
}


/*
 * Find an endpoint to be used as temporary peer (destination)
 * for a message sent from a CLTS endpoint.
 *
 * Currently, the peer endpoint returned here is always CLTS,
 * and it is responsible for forwarding any messages to the appropriate
 * COTS endpoints when necessary.
 *
 * Other implementations may choose to return a particular COTS endpoint
 * directly, but they then need to be careful when COTS endpoints are
 * created/destroyed.
 */
mach_error_t 
pipenet_dir_base::pipenet_find_peer(net_addr_t *localaddr, 
				    net_addr_t *peeraddr,
				    net_endpt_base **peerobj)
{
	ns_name_t		namestring;
	agency *agency_obj =0;
	ns_type_t		type;
	mach_error_t		ret;

#define	ABORT(_ret) {							\
	mach_object_dereference(agency_obj);				\
	*peerobj = NULL;					\
	us_internal_error("pipenet_find_peer()",(_ret));		\
	return(US_INTERNAL_ERROR);					\
}

	/*
	 * Establish the CLTS endpoint name.
	 */
	ret = net_addr_pipe_get_stringname(peeraddr,
					   namestring,sizeof(ns_name_t));
	if (ret != ERR_SUCCESS) ABORT(ret);

	/*
	 * Look for the endpoint in the directory.
	 */
	ret = this->ns_lookup_entry(namestring,strlen(namestring),
					     &agency_obj,&type);
	if (ret == NS_NOT_FOUND) {
		*peerobj = NULL;
		return(ret);
	}
	if (ret != ERR_SUCCESS) ABORT(ret);

	if ((*peerobj = net_endpt_base::castdown(agency_obj)) == 0) {
		ABORT(MACH_OBJECT_NO_SUCH_OPERATION);
	} else {	
		return(ERR_SUCCESS);
	}

#undef	ABORT
}

mach_error_t 
pipenet_dir_base::pipenet_setup_oneside_connection(pipenet_conninfo_t)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

