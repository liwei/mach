/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/symlink.cc,v $
 *
 * Purpose: Generic symbolic link object.
 *
 * HISTORY:
 * $Log:	symlink.cc,v $
 * Revision 2.4  94/07/07  17:24:46  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:29:07  dpj
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:15:05  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:23  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:15:55  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:36:26  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:42:52  pjg]
 * 
 * Revision 2.4  91/05/05  19:27:35  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:58:32  dpj]
 * 
 * 	Replace invoke_super() with invoke_super_with_base().
 * 	[91/04/28  10:20:18  dpj]
 * 
 * Revision 2.3  89/10/30  16:35:48  dpj
 * 	Added setup method.
 * 	Use PublicLocal().
 * 	Derive from vol_agency.
 * 	[89/10/27  19:17:03  dpj]
 * 
 * Revision 2.2  89/06/30  18:35:46  dpj
 * 	Initial revision.
 * 	[89/06/29  00:42:40  dpj]
 * 
 */

#ifndef lint
char * symlink_rcsid = "$Header: symlink.cc,v 2.4 94/07/07 17:24:46 mrt Exp $";
#endif	lint

#include	<symlink_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<io_types.h>
}

DEFINE_CLASS_MI(symlink);

/*
 * Class methods
 */

void symlink::init_class(usClass* class_obj)
{
	usName::init_class(class_obj);
	vol_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(symlink);
	SETUP_METHOD_WITH_ARGS(symlink,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(symlink,ns_read_forwarding_entry);
	END_SETUP_METHOD_WITH_ARGS;

}

void* symlink::_castdown(const usClass& c) const
{
	if (&c == desc()) return (void*) this;
	void* p = usName::_castdown(c);
	void* q = p;
	if (p = vol_agency::_castdown(c)) ambig_check(p, q, c);
	return q;
}

symlink::symlink(ns_mgr_id_t mgr_id, access_table *acctab, 
		 ns_path_t fwdpath, mach_error_t* ret)
	:
	vol_agency(mgr_id, acctab)
{
//	(void) this->_setup_vol_agency(mgr_id, acctab);

	if (fwdpath) {
		fwd_pathlen = strlen(fwdpath);
		if (fwd_pathlen >= NS_PATHLEN) {
			fwd_pathlen = NS_PATHLEN - 1;
		}

		bcopy(fwdpath,fwd_path,fwd_pathlen);
		fwd_path[fwd_pathlen] = '\0';
	}
}

/*
 * Instance methods
 */
mach_error_t 
symlink::ns_read_forwarding_entry(usItem** newobj, char *newpath)
{
	bcopy(_Local(fwd_path),newpath,_Local(fwd_pathlen) + 1);
	*newobj = 0;

	return(ERR_SUCCESS);
}


mach_error_t 
symlink::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

//	ret = invoke_super(Super,mach_method_id(ns_get_attributes),
//							attr,attrlen);
	ret = this->vol_agency::ns_get_attributes(attr, attrlen);
	attr->type = NST_SYMLINK;

	attr->valid_fields |= NS_ATTR_SIZE;
	UINT_TO_IO_SIZE(_Local(fwd_pathlen),&attr->size);

	return(ret);
}

mach_error_t 
symlink::ns_resolve(char* path, ns_mode_t mode, ns_access_t access, 
		    usItem** newobj, ns_type_t* newtype, char* newpath, 
		    int* usedlen, ns_action_t* action)
{
	return _notdef();
}


mach_error_t 
symlink::ns_create(char *name, ns_type_t type, ns_prot_t prot, int protlen, 
		   ns_access_t access, usItem** newobj)
{
	return _notdef();
}


mach_error_t 
symlink::ns_create_anon(ns_type_t type, ns_prot_t prot, int protlen, 
			ns_access_t access, char *name, usItem** newobj)
{
	return _notdef();
}

mach_error_t 
symlink::ns_create_transparent_symlink(ns_name_t, ns_prot_t, int, char *)
{
	return _notdef();
}

mach_error_t symlink::ns_insert_entry(char *name, usItem *target)
{
	return _notdef();
}


mach_error_t 
symlink::ns_insert_forwarding_entry(char *name, ns_prot_t prot, int protlen, 
				    usItem* obj, char *path)
{
	return _notdef();
}


mach_error_t symlink::ns_remove_entry(char *name)
{
	return _notdef();
}


mach_error_t
symlink::ns_rename_entry(char *name, usItem* newdir, char *newname)
{
	return _notdef();
}


mach_error_t
symlink::ns_list_entries(ns_mode_t mode, ns_name_t (**names), 
			 unsigned int *names_count, ns_entry_t *entries, 
			 unsigned int *entries_count)
{
	return _notdef();
}


mach_error_t symlink::ns_allocate_unique_name(char *name)
{
	return _notdef();
}


mach_error_t symlink::ns_list_types(ns_type_t **types, int *count)
{
	return _notdef();
}

