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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_cldir_recs.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: directory for connection-less, record-oriented communications
 *
 * HISTORY
 * $Log:	pipenet_cldir_recs.cc,v $
 * Revision 2.6  94/07/13  17:20:45  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/01/11  18:11:03  jms
 * 	Call pipenet_dir_base::remote_class_name not dir::remote_class_name in
 * 	pipenet_cldir_recs::remote_class_name
 * 	[94/01/10  13:25:46  jms]
 * 
 * Revision 2.4  92/07/05  23:34:31  dpj
 * 	Added explicit definition of remote_class_name()
 * 	under GXXBUG_VIRTUAL1.
 * 	[92/06/29  17:26:27  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:29:47  dpj]
 * 
 * Revision 2.3  92/03/05  15:12:35  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:27:34  jms]
 * 
 * Revision 2.2  91/11/06  14:20:06  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:51:11  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:16  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:06:54  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:54:04  dpj]
 * 
 */

#ifndef lint
char * pipenet_cldir_recs_rcsid = "$Header: pipenet_cldir_recs.cc,v 2.6 94/07/13 17:20:45 mrt Exp $";
#endif	lint

#include	<pipenet_cldir_recs_ifc.h>
#include	<pipenet_clts_recs_ifc.h>
#include	<pipenet_cots_recs_ifc.h>
#include	<agent_ifc.h>

#define BASE pipenet_dir_base
DEFINE_CLASS(pipenet_cldir_recs)


void pipenet_cldir_recs::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_cldir_recs);
	SETUP_METHOD_WITH_ARGS(pipenet_cldir_recs,ns_list_types);
	SETUP_METHOD_WITH_ARGS(pipenet_cldir_recs,net_create);
	END_SETUP_METHOD_WITH_ARGS;
}

pipenet_cldir_recs::pipenet_cldir_recs(ns_mgr_id_t mgr_id,
				       access_table *access_tab)
:
 pipenet_dir_base(mgr_id, access_tab, 0)
{}


#ifdef	GXXBUG_VIRTUAL1
char* pipenet_cldir_recs::remote_class_name() const
	{ return pipenet_dir_base::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1

mach_error_t 
pipenet_cldir_recs::ns_list_types(ns_type_t **types, int *count)
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
	((ns_type_t *)data)[0] = NST_CLTS_RECS;
	((ns_type_t *)data)[1] = NST_COTS_RECS;

	*types = (ns_type_t *)data;

	return(NS_SUCCESS);
}


/*
 * Standard network service interface.
 */

mach_error_t 
pipenet_cldir_recs::net_create(net_addr_t *localaddr, int *qmax, 
			       ns_prot_t prot, int protlen, ns_access_t access,
			       usItem **newobj, ns_type_t *newtype,
			       net_info_t *info)
{
	ns_name_t		newname;
	pipenet_clts_recs	*endpt = 0;
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
	*qmax = 0;

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
//	new_object(endpt,pipenet_clts_recs);
	endpt = new pipenet_clts_recs(mgr_id, access_tab, this, localaddr, 
				      iobuf_mgr);
	/*
	 * Make the endpoint available for service.
	 */
//	ret = invoke(endpt,mach_method_id(setup_pipenet_clts_recs),
//				PublicLocal(mgr_id),
//				PublicLocal(access_table),ThisAgency,
//				localaddr,PublicLocal(iobuf_mgr));
	if (ret != ERR_SUCCESS) ABORT(ret);
	agent *agent_obj;
	ret = this->ns_create_common(tag,endpt,NST_CONNECTOR,
				     prot,protlen,access,&agent_obj);
	*newobj = agent_obj;
	if (ret != ERR_SUCCESS) ABORT(ret);
	tag = 0;

	mach_object_dereference(endpt);

	*newtype = NST_CLTS_RECS;
	net_info_copy(&netinfo,info);

	return(ERR_SUCCESS);

#undef	ABORT
}


mach_error_t 
pipenet_cldir_recs::pipenet_setup_oneside_connection(pipenet_conninfo_t	conninfo)
{
	mach_error_t		ret = ERR_SUCCESS;

	/*
	 * Create the (single) new connected endpoint for the active side.
	 */
	conninfo->active_type = NST_COTS_RECS;
//	new_object(conninfo->active_endpt,pipenet_cots_recs);
	conninfo->active_endpt = new pipenet_cots_recs(mgr_id, access_tab,this,
			conninfo->active_addr, conninfo->passive_addr,
			conninfo->active_connector, conninfo->passive_endpt,
			iobuf_mgr);

//	ret = invoke(conninfo->active_endpt,
//			mach_method_id(setup_pipenet_cots_recs),
//			PublicLocal(mgr_id),PublicLocal(access_table),
//			ThisAgency,
//			conninfo->active_addr,conninfo->passive_addr,
//			conninfo->active_connector,conninfo->passive_endpt,
//
//			PublicLocal(iobuf_mgr));

	return (ret);
}
