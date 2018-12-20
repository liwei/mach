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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/net_dir_base.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for network service directories.
 *
 * HISTORY
 * $Log:	net_dir_base.cc,v $
 * Revision 2.4  94/07/07  17:23:47  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:08  dpj
 * 	ns_lookup_entry_internal -> ns_lookup_entry
 * 	[92/06/24  15:57:53  jms]
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:56:04  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:08  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:57:00  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:46  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:08  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:13:07  dpj]
 * 
 */

#include	<net_dir_base_ifc.h>
#include	<net_endpt_base_ifc.h>
#include	<agent_ifc.h>

#define BASE dir
DEFINE_CLASS(net_dir_base)
//DEFINE_CASTDOWN2(net_dir_base, usNetName, dir)

net_dir_base::net_dir_base(ns_mgr_id_t mgr_id, access_table *access_table_obj,
			   net_info_t *info)
	: dir(mgr_id,access_table_obj)
{
	net_info_null_init(&netinfo);
	net_info_copy(info,&netinfo);
}


net_dir_base::~net_dir_base()
{
	net_info_destroy(&Local(netinfo));
}

void net_dir_base::init_class(usClass* class_obj)
{
//	usNetName:init_class(class_obj);
	dir::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(net_dir_base);
	SETUP_METHOD_WITH_ARGS(net_dir_base,ns_create);
	SETUP_METHOD_WITH_ARGS(net_dir_base,ns_create_transparent_symlink);
	SETUP_METHOD_WITH_ARGS(net_dir_base,ns_insert_entry);
	SETUP_METHOD_WITH_ARGS(net_dir_base,ns_insert_forwarding_entry);
	SETUP_METHOD_WITH_ARGS(net_dir_base,ns_rename_entry);
	SETUP_METHOD_WITH_ARGS(net_dir_base,ns_list_types);
	SETUP_METHOD_WITH_ARGS(net_dir_base,net_lookup);
	SETUP_METHOD_WITH_ARGS(net_dir_base,net_cots_lookup);
	END_SETUP_METHOD_WITH_ARGS;
}

char* net_dir_base::remote_class_name() const
{
	return "usNetName_proxy";
}

#ifdef	NOTDEF
mach_error_t 
net_dir_base::ns_create_agent(ns_access_t access, std_cred *cred_obj,
			      agent **newobj)
{
	mach_error_t		ret;

	/*
	 * Access check.
	 */
       	ret = ns_check_access(access,cred_obj);
	if (ret != NS_SUCCESS) {
		*newobj = NULL;
		return(ret);
	}

	/*
	 * Create the agent.
	 */
	*newobj = new agent(this, cred_obj, access_tab, access, ret);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(*newobj);
		*newobj = NULL;
		return(ret);
	}

	return(ERR_SUCCESS);
}
#endif	NOTDEF

mach_error_t 
net_dir_base::ns_create(ns_name_t name, ns_type_t type, ns_prot_t prot,
			int protlen, ns_access_t access, usItem **newobj)
{
	return(US_UNSUPPORTED);
}


mach_error_t 
net_dir_base::ns_create_transparent_symlink(ns_name_t name, ns_prot_t prot,
					    int protlen, char *path)
{
	return(US_UNSUPPORTED);
}


mach_error_t 
net_dir_base::ns_insert_entry(char *name, usItem* target)
{
	return(US_UNSUPPORTED);
}


mach_error_t 
net_dir_base::ns_insert_forwarding_entry(ns_name_t name, ns_prot_t prot, 
					 int protlen, usItem *obj, char* path)
{
	return(US_UNSUPPORTED);
}


mach_error_t 
net_dir_base::ns_rename_entry(ns_name_t name, usItem *newdir,ns_name_t newname)
{
	return(US_UNSUPPORTED);
}


mach_error_t net_dir_base::ns_list_types(ns_type_t **types, int *count)
{
	return(US_UNSUPPORTED);
}


mach_error_t 
net_dir_base::net_lookup(net_addr_t *localaddr, ns_access_t access,
			 usItem **newobj, ns_type_t *newtype, net_info_t *info)
{
	std_cred		*cred;
	ns_name_t		newname;
	agency		*endpt;
	mach_error_t		ret;

	*newobj = NULL;

	/*
	 * Prepare the full endpoint name.
	 */
	ret = net_addr_get_stringname(localaddr,newname,sizeof(ns_name_t));
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Find the endpoint.
	 */
	ret = ns_lookup_entry(newname,strlen(newname),&endpt,newtype);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Return results to caller.
	 */
	ret = agent::base_object()->ns_get_cred_obj(&cred);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(endpt);
		return(ret);
	}
	agent* agent_obj;
	ret = endpt->ns_create_agent(access,cred,&agent_obj);
	*newobj = agent_obj;
	mach_object_dereference(endpt);
	mach_object_dereference(cred);

	net_info_copy(&Local(netinfo),info);

	return(ret);
}


mach_error_t 
net_dir_base::net_cots_lookup(net_addr_t *localaddr, net_addr_t *peeraddr,
			      ns_access_t access, usItem **newobj,
			      ns_type_t *newtype, net_info_t *info)
{
	std_cred		*cred;
	ns_name_t		newname;
	unsigned int		namelen;
	agency		*endpt;
	mach_error_t		ret;

	*newobj = NULL;

	/*
	 * Prepare the full endpoint name.
	 */
	ret = net_addr_get_stringname(localaddr,newname,sizeof(ns_name_t));
	if (ret != ERR_SUCCESS) {
		return(ret);
	}
	namelen = sizeof(newname);
	newname[namelen++] = '-';
	ret = net_addr_pipe_get_stringname(peeraddr,
				&newname[namelen],sizeof(ns_name_t) - namelen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Find the endpoint.
	 */
	ret = ns_lookup_entry(newname,strlen(newname),&endpt,newtype);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Return results to caller.
	 */
	ret = agent::base_object()->ns_get_cred_obj(&cred);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(endpt);
		return(ret);
	}
	agent *agent_obj;
	ret = endpt->ns_create_agent(access,cred,&agent_obj);
	*newobj = agent_obj;
	mach_object_dereference(endpt);
	mach_object_dereference(cred);

	net_info_copy(&Local(netinfo),info);

	return(ret);
}

mach_error_t net_dir_base::net_create(net_addr_t *, int *, ns_prot_t, int,
				      ns_access_t, usItem **, ns_type_t *,
				      net_info_t *)
{
	return _notdef();
}

