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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_root.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: root directory of the pipenet server.
 *
 * HISTORY
 * $Log:	pipenet_root.cc,v $
 * Revision 2.6  94/07/13  17:21:52  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/01/11  18:11:20  jms
 * 	Fix server directory protections.
 * 	[94/01/10  13:30:11  jms]
 * 
 * Revision 2.4  92/07/05  23:35:19  dpj
 * 	Added explicit definition of remote_class_name()
 * 	under GXXBUG_VIRTUAL1.
 * 	[92/06/29  17:27:47  dpj]
 * 
 * Revision 2.3  92/03/05  15:12:41  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:28:25  jms]
 * 
 * Revision 2.2  91/11/06  14:23:33  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:56:13  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:22  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:52  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:01:22  dpj]
 * 
 */

#include	<pipenet_root_ifc.h>
#include	<pipenet_updir_bytes_ifc.h>
#include	<pipenet_cldir_recs_ifc.h>
#include	<pipenet_codir_bytes_ifc.h>

extern ns_authid_t fs_access_default_root_authid;

#define BASE dir
DEFINE_CLASS(pipenet_root)

void pipenet_root::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_root);
	SETUP_METHOD_WITH_ARGS(pipenet_root,ns_create);
	SETUP_METHOD_WITH_ARGS(pipenet_root,ns_list_types);
	END_SETUP_METHOD_WITH_ARGS;
}

pipenet_root::pipenet_root()
{}

pipenet_root::pipenet_root(ns_mgr_id_t mgr_id, mach_error_t* ret)
	:
	dir(mgr_id, ret)
{
	dir		*subdir;

	ns_prot_t		subdir_prot = NULL;
	int			acl_len = 2;

	/*
	 * Create the protocol and upipes subdirectories.
	 */
	subdir_prot = (ns_prot_t)malloc(sizeof(struct ns_prot_head) +
				(acl_len * sizeof(struct ns_acl_entry)));
	subdir_prot->head.version = NS_PROT_VERSION;
	subdir_prot->head.generation = 0;
	subdir_prot->head.acl_len = acl_len;

	subdir_prot->acl[0].authid = fs_access_default_root_authid;
	subdir_prot->acl[0].rights = NSR_ADMIN |
		NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;
	subdir_prot->acl[1].authid = NS_AUTHID_WILDCARD;
	subdir_prot->acl[1].rights =
		NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;


	*ret = ns_set_protection(subdir_prot, 
				     NS_PROT_SIZE(subdir_prot) / sizeof(int));

	if (*ret != ERR_SUCCESS) {
		us_internal_error("ns_set_protection(pipenet_root)",*ret);
	}

	subdir = new pipenet_cldir_recs(mgr_id, access_tab);

	subdir_prot->acl[1].rights |= NSR_INSERT | NSR_WRITE;
	*ret = subdir->ns_set_protection(subdir_prot, 
				     NS_PROT_SIZE(subdir_prot) / sizeof(int));
	if (*ret != ERR_SUCCESS) {
		us_internal_error("ns_set_protection(CLTS_RECS)",*ret);
	}
	*ret = ns_insert_entry("CLTS_RECS",subdir);
	if (*ret != ERR_SUCCESS) {
		us_internal_error("ns_insert_entry(CLTS_RECS)",*ret);
	}
	mach_object_dereference(subdir);

//	new_object(subdir,pipenet_codir_bytes);
//	(void) invoke(subdir,mach_method_id(setup_pipenet_codir_bytes),
//				PublicLocal(mgr_id),PublicLocal(access_table));
	subdir = new pipenet_codir_bytes (mgr_id, access_tab);
	*ret = subdir->ns_set_protection(subdir_prot,
				NS_PROT_SIZE(subdir_prot) / sizeof(int));
	if (*ret != ERR_SUCCESS) {
		us_internal_error("ns_set_protection(COTS_BYTES)",*ret);
	}
	*ret = ns_insert_entry("COTS_BYTES",subdir);
	if (*ret != ERR_SUCCESS) {
		us_internal_error("ns_insert_entry(COTS_BYTES)",*ret);
	}
	mach_object_dereference(subdir);

//	new_object(subdir,pipenet_updir_bytes);
//	(void) invoke(subdir,mach_method_id(setup_pipenet_updir_bytes),
//				PublicLocal(mgr_id),PublicLocal(access_table));
	subdir = new pipenet_updir_bytes (mgr_id, access_tab);
	*ret = subdir->ns_set_protection(subdir_prot,
				NS_PROT_SIZE(subdir_prot) / sizeof(int));
	if (*ret != ERR_SUCCESS) {
		us_internal_error("ns_set_protection(UPIPE_BYTES)",*ret);
	}
	*ret = ns_insert_entry("UPIPE_BYTES",subdir);
	if (*ret != ERR_SUCCESS) {
		us_internal_error("ns_insert_entry(UPIPE_BYTES)",*ret);
	}
	mach_object_dereference(subdir);
	free(subdir_prot);
}


#ifdef	GXXBUG_VIRTUAL1
char* pipenet_root::remote_class_name() const
	{ return dir::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1

mach_error_t
pipenet_root::ns_create(ns_name_t name, ns_type_t type, ns_prot_t prot, 
			int protlen, ns_access_t access, usItem **newobj)
{
	/*
	 * We do not support anything so far.
	 */
	return(US_UNSUPPORTED);
}


mach_error_t pipenet_root::ns_list_types(ns_type_t **types, int *count)
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
	((ns_type_t *)data)[0] = NST_DIRECTORY;

	*types = (ns_type_t *)data;

	return(NS_SUCCESS);
}
