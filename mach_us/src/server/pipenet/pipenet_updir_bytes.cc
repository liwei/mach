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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_updir_bytes.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: directory holding all uni-directional byte-level pipes
 *
 * HISTORY
 * $Log:	pipenet_updir_bytes.cc,v $
 * Revision 2.5  94/07/13  17:21:58  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  92/07/05  23:35:26  dpj
 * 	Added explicit definition of remote_class_name()
 * 	under GXXBUG_VIRTUAL1.
 * 	[92/06/29  17:28:17  dpj]
 * 
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  01:31:42  dpj]
 * 
 * Revision 2.3  92/03/05  15:12:43  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:28:37  jms]
 * 
 * Revision 2.2  91/11/06  14:23:39  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:56:48  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:27  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:09:00  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:02:02  dpj]
 * 
 */

#ifndef lint
char * pipenet_updir_bytes_rcsid = "$Header: pipenet_updir_bytes.cc,v 2.5 94/07/13 17:21:58 mrt Exp $";
#endif	lint

#include	<pipenet_updir_bytes_ifc.h>
#include	<pipenet_upipe_bytes_ifc.h>
#include	<agent_ifc.h>

#define BASE dir
DEFINE_CLASS(pipenet_updir_bytes)

void pipenet_updir_bytes::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_updir_bytes);
	SETUP_METHOD_WITH_ARGS(pipenet_updir_bytes,ns_create);
	SETUP_METHOD_WITH_ARGS(pipenet_updir_bytes,ns_insert_entry);
	SETUP_METHOD_WITH_ARGS(pipenet_updir_bytes,ns_insert_forwarding_entry);
	SETUP_METHOD_WITH_ARGS(pipenet_updir_bytes,ns_list_types);
	END_SETUP_METHOD_WITH_ARGS;
}


pipenet_updir_bytes::pipenet_updir_bytes(ns_mgr_id_t mgr_id,
					 access_table *access_tab)
:
 dir(mgr_id,access_tab)
{
	iobuf_mgr = new default_iobuf_mgr;
}


pipenet_updir_bytes::~pipenet_updir_bytes()
{
	DESTRUCTOR_GUARD();
	mach_object_dereference(iobuf_mgr);
}


#ifdef	GXXBUG_VIRTUAL1
char* pipenet_updir_bytes::remote_class_name() const
	{ return dir::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1


mach_error_t 
pipenet_updir_bytes::ns_create(ns_name_t name, ns_type_t type, ns_prot_t prot,
			       int protlen,ns_access_t access, usItem **newobj)
{
	int			tag;
	mach_error_t		ret;

	/*
	 * We do not support anything other than pipes.
	 */
	if (type != NST_UPIPE_BYTES) {
		return(US_UNSUPPORTED);
	}

	ret = this->ns_reserve_entry(name,&tag);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

//	new_object(newagency,pipenet_upipe_bytes);
//	ret = setup_pipenet_upipe_bytes(newagency,PublicLocal(mgr_id),
//				PublicLocal(access_table),Local(iobuf_mgr));
	pipenet_upipe_bytes* newagency = new pipenet_upipe_bytes(
						    mgr_id, access_tab, 
						    iobuf_mgr);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
		mach_object_dereference(newagency);
		return(ret);
	}
	agent *agent_obj;
	ret = this->ns_create_common(tag, newagency, type, prot, protlen, 
				     access, &agent_obj);
	*newobj = agent_obj;

	mach_object_dereference(newagency);

	return(ret);

}


mach_error_t pipenet_updir_bytes::ns_insert_entry(char *name, usItem* target)
{
	mach_error_t		ret;
	struct ns_attr		attr;
	int			attrlen;

	attrlen = sizeof(attr) / sizeof(int);
	ret = target->ns_get_attributes(&attr,&attrlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	if ((attr.version != NS_ATTR_VERSION) ||
		((attr.valid_fields & NS_ATTR_TYPE) != NS_ATTR_TYPE)) {
		return(US_UNKNOWN_ERROR);
	}

	if (attr.type != NST_UPIPE_BYTES) {
		return(US_UNSUPPORTED);
	}

//	ret = invoke_super(Super,mach_method_id(ns_insert_entry),name,target);
	ret = dir::ns_insert_entry(name, target);

	return(ret);
}


mach_error_t 
pipenet_updir_bytes::ns_insert_forwarding_entry(ns_name_t name, ns_prot_t prot,
						int protlen, usItem *obj, 
						char* path)
{
	return(US_UNSUPPORTED);
}


mach_error_t pipenet_updir_bytes::ns_list_types(ns_type_t **types, int *count)
{
	mach_error_t		ret;
	vm_address_t		data;

	*count = 1;

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
	((ns_type_t *)data)[0] = NST_UPIPE_BYTES;

	*types = (ns_type_t *)data;

	return(NS_SUCCESS);
}
