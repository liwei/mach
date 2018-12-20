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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_codir_bytes.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: directory for connection-oriented, byte-oriented communications
 *
 * HISTORY
 * $Log:	pipenet_codir_bytes.cc,v $
 * Revision 2.6  94/07/13  17:21:05  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/01/11  18:11:11  jms
 * 	Call pipenet_dir_base::remote_class_name not dir::remote_class_name in
 * 	pipenet_codir_recs::remote_class_name
 * 	[94/01/10  13:27:13  jms]
 * 
 * Revision 2.4  92/07/05  23:34:44  dpj
 * 	Added explicit definition of remote_class_name()
 * 	under GXXBUG_VIRTUAL1.
 * 	[92/06/29  17:27:13  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:30:29  dpj]
 * 
 * Revision 2.3  92/03/05  15:12:38  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:28:09  jms]
 * 
 * Revision 2.2  91/11/06  14:20:31  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:52:40  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:38  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:38  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:55:50  dpj]
 * 
 */

#ifndef lint
char * pipenet_codir_bytes_rcsid = "$Header: pipenet_codir_bytes.cc,v 2.6 94/07/13 17:21:05 mrt Exp $";
#endif	lint

#include	<pipenet_codir_bytes_ifc.h>
#include	<pipenet_cots_bytes_ifc.h>
#include	<pipenet_connector_ifc.h>
#include	<agent_ifc.h>


#define BASE pipenet_dir_base
DEFINE_CLASS(pipenet_codir_bytes)


void pipenet_codir_bytes::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_codir_bytes);
	SETUP_METHOD_WITH_ARGS(pipenet_codir_bytes,ns_list_types);
	SETUP_METHOD_WITH_ARGS(pipenet_codir_bytes,net_create);
	END_SETUP_METHOD_WITH_ARGS;
}

pipenet_codir_bytes::pipenet_codir_bytes(ns_mgr_id_t mgr_id,
					 access_table *access_tab)
	:
	pipenet_dir_base(mgr_id, access_tab, 0)
{}

#ifdef	GXXBUG_VIRTUAL1
char* pipenet_codir_bytes::remote_class_name() const
	{ return pipenet_dir_base::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1


/*
 * Standard name service interface.
 */

mach_error_t 
pipenet_codir_bytes::ns_list_types(ns_type_t **types, int *count)
{
	mach_error_t		ret;
	vm_address_t		data;

	*count = 2;

	/*
	 * Get space for the reply.
	 */
	data = NULL;
	ret = vm_allocate(mach_task_self(),&data,*count * sizeof(ns_type_t),TRUE);
	if (ret != KERN_SUCCESS) {
		*count = 0;
		*types = NULL;
		return(ret);
	}

	/*
	 * Prepare the reply.
	 */
	((ns_type_t *)data)[0] = NST_CONNECTOR;
	((ns_type_t *)data)[1] = NST_COTS_BYTES;

	*types = (ns_type_t *)data;

	return(NS_SUCCESS);
}


/*
 * Standard network service interface.
 */

mach_error_t 
pipenet_codir_bytes::net_create(net_addr_t *localaddr, int *qmax, 
				ns_prot_t prot, int protlen, 
				ns_access_t access, usItem **newobj,
				ns_type_t *newtype, net_info_t *info)
{
	ns_name_t		newname;
	pipenet_connector	*endpt = 0;
	int			tag = 0;
	mach_error_t		ret;

#define	ABORT(_ret) {							\
	if (tag) (void) this->ns_cancel_entry(tag);			\
	mach_object_dereference(endpt);					\
	mach_object_dereference(*newobj);				\
	*newobj = NULL;					\
	*newtype = NST_INVALID;						\
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
	 * Prepare the full endpoint name.
	 */
	ret = this->pipenet_check_localaddr(localaddr);
	if (ret != ERR_SUCCESS) ABORT(ret);
	ret = net_addr_pipe_get_stringname(localaddr,newname,
							sizeof(ns_name_t));
	if (ret != ERR_SUCCESS) {
		us_internal_error(
			"net_create: cannot get stringname for local address",
			ret);
		ABORT(US_INTERNAL_ERROR);
	}

	/*
	 * Create the endpoint.
	 */
	ret = this->ns_reserve_entry(newname,&tag);
	if (ret != ERR_SUCCESS) ABORT(ret);
//	new_object(endpt,pipenet_connector);
	endpt = new pipenet_connector(mgr_id, access_tab,this,localaddr,*qmax);
	/*
	 * Make the endpoint available for service.
	 */
//	ret = invoke(endpt,mach_method_id(setup_pipenet_connector),
//			PublicLocal(mgr_id),
//			PublicLocal(access_table),ThisAgency,localaddr,*qmax);
	if (ret != ERR_SUCCESS) ABORT(ret);
	agent *agent_obj;
	ret = this->ns_create_common(tag,endpt,NST_CONNECTOR,
				     prot,protlen,access,&agent_obj);
	*newobj = agent_obj;
	if (ret != ERR_SUCCESS) ABORT(ret);
	tag = 0;

	mach_object_dereference(endpt);

	*newtype = NST_CONNECTOR;
	net_info_copy(&netinfo,info);

	return(ERR_SUCCESS);

#undef	ABORT
}


mach_error_t 
pipenet_codir_bytes::pipenet_setup_twoside_connection(pipenet_conninfo_t conninfo)
{
	ns_name_t		newname;
	unsigned int		namelen;
	int			tag = 0;
	ns_type_t		type;
	mach_error_t		ret;

#define	ABORT(_ret) {							\
	if (tag) (void) this->ns_cancel_entry(tag);			\
	return(_ret);							\
}

	/*
	 * Prepare the full passive endpoint name.
	 */
	ret = net_addr_pipe_get_stringname(conninfo->passive_addr,
						newname,sizeof(ns_name_t));
	if (ret != ERR_SUCCESS) {
		us_internal_error(
"net_setup_twoside_connection: cannot get stringname for passive address",
			ret);
		ABORT(US_INTERNAL_ERROR);
	}
	namelen = strlen(newname);
	newname[namelen++] = '-';
	ret = net_addr_pipe_get_stringname(conninfo->active_addr,
				&newname[namelen],sizeof(ns_name_t) - namelen);
	if (ret != ERR_SUCCESS) {
		us_internal_error(
"net_setup_twoside_connection: cannot get stringname for active address",
			ret);
		ABORT(US_INTERNAL_ERROR);
	}

	/*
	 * Reserve the passive_name in the directory.
	 */
	ret = this->ns_reserve_entry(newname,&tag);
	if (ret != ERR_SUCCESS) ABORT(ret);

	/*
	 * Create both endpoints.
	 */
	conninfo->active_type = NST_COTS_BYTES;
	pipenet_cots_bytes *p = new pipenet_cots_bytes(mgr_id,access_tab,this,
			conninfo->active_addr,conninfo->passive_addr,
			conninfo->active_connector, 0 /* XXX C++ */,
			iobuf_mgr);
	if (ret != ERR_SUCCESS) ABORT(ret);
	conninfo->active_endpt = p;
	conninfo->passive_type = NST_COTS_BYTES;
	conninfo->passive_endpt =new pipenet_cots_bytes(mgr_id,access_tab,this,
			conninfo->passive_addr,conninfo->active_addr,
			conninfo->passive_connector,conninfo->active_endpt,
			iobuf_mgr);
	if (ret != ERR_SUCCESS) ABORT(ret);
	(void) p->set_peerobj(conninfo->passive_endpt);
	/*
	 * Install the passive endpoint in the directory.
	 *
	 * Let the active side do the same for the active endpoint and get
	 * rid of all the MachObjects references acquired along the way.
	 */
	ret = conninfo->passive_endpt->ns_set_protection(
						    conninfo->passive_prot,
						    conninfo->passive_protlen);
	if (ret != ERR_SUCCESS) ABORT(ret);

	ret = this->ns_install_entry(tag, conninfo->passive_endpt,
				     conninfo->passive_type);
	if (ret != ERR_SUCCESS) ABORT(ret);
	tag = 0;

	return(ERR_SUCCESS);

#undef	ABORT
}


