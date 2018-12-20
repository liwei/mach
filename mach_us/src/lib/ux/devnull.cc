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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/devnull.cc,v $
 *
 * HISTORY
 * $Log:	devnull.cc,v $
 * Revision 2.4  94/07/08  16:01:38  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  92/07/05  23:32:29  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:29:39  dpj]
 * 
 * Revision 2.2  91/12/20  17:44:58  jms
 * 	First Revision (DPJ)
 * 	[91/12/20  16:11:46  jms]
 * 
 */

#ifndef lint
char * devnull_rcsid = "$Header: devnull.cc,v 2.4 94/07/08 16:01:38 mrt Exp $";
#endif	lint

#include <devnull_ifc.h>

//#define	BASE	usByteIO
//DEFINE_LOCAL_CLASS(devnull)
DEFINE_LOCAL_CLASS_MI(devnull);
DEFINE_CASTDOWN2(devnull,usByteIO,usClone);


mach_error_t 
devnull::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
	mach_object_reference(this);
	*obj = this;
	return(ERR_SUCCESS);
}


mach_error_t 
devnull::ns_duplicate(ns_access_t access, usItem** newobj)
{
	mach_object_reference(this);
	*newobj = this;
	return(ERR_SUCCESS);
}


mach_error_t devnull::ns_get_attributes(ns_attr_t attr, int* attrlen)
{
	int			protlen;

	*attrlen = sizeof(*attr) / sizeof(int);
	attr->version = NS_ATTR_VERSION;
	attr->valid_fields =
			(NS_ATTR_TYPE|NS_ATTR_ID|NS_ATTR_NLINKS|NS_ATTR_PROT);
	attr->type = NST_FILE;
	attr->mgr_id.v1 = 0;
	attr->mgr_id.v2 = 0;

	/*
	 * Try to get an object ID that will not conflict with
	 * anything else likely to be used in the same server...
	 */
	attr->obj_id = 0x10000000 | (ns_obj_id_t) this;

	CLEAR_TV(attr->access_time);	/* XXX */
	CLEAR_TV(attr->modif_time);	/* XXX */
	CLEAR_TV(attr->creation_time);	/* XXX */
	INT_TO_IO_SIZE(0,&attr->size);

	(void) ns_get_protection((ns_prot_t)&attr->prot_ltd,&protlen);

	attr->nlinks = 1;

	return(ERR_SUCCESS);
}

mach_error_t devnull::ns_set_times(time_value_t atime, time_value_t mtime)
{
	return(US_NOT_IMPLEMENTED);
}

mach_error_t devnull::ns_get_protection(ns_prot_t prot, int* protlen)
{
	prot->head.version = NS_PROT_VERSION;
	prot->head.generation = 0;
	prot->head.acl_len = 1;
	prot->acl[0].authid = NS_AUTHID_WILDCARD; 
	prot->acl[0].rights = NSR_ALL;

	*protlen = NS_PROT_SIZE(prot);

	return(ERR_SUCCESS);
}

mach_error_t devnull::ns_set_protection(ns_prot_t prot, int protlen)
{
	return(US_NOT_IMPLEMENTED);
}

mach_error_t devnull::ns_get_privileged_id(int* id)
{
	return(US_NOT_IMPLEMENTED);
}

mach_error_t 
devnull::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
	*access = NSR_ALL;
	cred->head.version = NS_CRED_VERSION;
	cred->head.cred_len = 1;
	cred->authid[0] = NS_AUTHID_WILDCARD;

	*credlen = NS_CRED_SIZE(cred);

	return(ERR_SUCCESS);
}

mach_error_t 
devnull::ns_get_manager(ns_access_t access, usItem **newobj)
{
	return(US_NOT_IMPLEMENTED);
}


mach_error_t devnull::io_read(io_mode_t mode, io_offset_t start, pointer_t addr, unsigned int *num)
{
	*num = 0;
	return(IO_INVALID_OFFSET);
}


mach_error_t devnull::io_write(io_mode_t mode, io_offset_t start, pointer_t addr, unsigned int *num)
{
	return(ERR_SUCCESS);
}

mach_error_t devnull::io_append(io_mode_t mode, pointer_t addr, unsigned int *num)
{
	return(ERR_SUCCESS);
}

mach_error_t 
devnull::io_read_seq(io_mode_t mode, char *addr, unsigned int *num, 
			    io_offset_t *offset)
{
	*num = 0;
	return(IO_INVALID_OFFSET);
}

mach_error_t 
devnull::io_write_seq(io_mode_t mode, char *addr, unsigned int *num,
			     io_offset_t *offset)
{
	return(ERR_SUCCESS);
}

mach_error_t devnull::io_set_size(io_size_t size)
{
	return(ERR_SUCCESS);
}


mach_error_t devnull::io_get_size(io_size_t *size)
{
	UINT_TO_IO_SIZE(0,size);
	return(ERR_SUCCESS);
}


mach_error_t devnull::io_map(task_t task, vm_address_t *addr, vm_size_t size, vm_offset_t mask, boolean_t anywhere, vm_offset_t paging_offset, boolean_t copy, vm_prot_t cprot, vm_prot_t mprot, vm_inherit_t inherit)
{
	return(MACH_OBJECT_NO_SUCH_OPERATION);
}
