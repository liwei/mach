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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/mountpt.cc,v $
 *
 * Purpose: Generic mount point object.
 *
 * HISTORY:
 * $Log:	mountpt.cc,v $
 * Revision 2.3  94/07/07  17:23:44  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:47:02  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:54:56  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:29:30  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:39:03  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  14:59:39  pjg]
 * 
 * Revision 2.5  91/07/01  14:12:17  jms
 * 	Remove some ns_[de]reference stuff.
 * 	[91/06/24  17:23:54  jms]
 * 
 * Revision 2.4  91/05/05  19:26:44  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:54:13  dpj]
 * 
 * 	Replace invoke_super() with invoke_super_with_base().
 * 	[91/04/28  10:12:48  dpj]
 * 
 * Revision 2.3  89/10/30  16:34:27  dpj
 * 	Added setup method.
 * 	Use PublicLocal().
 * 	Derove from vol_agency().
 * 	[89/10/27  19:05:21  dpj]
 * 
 * Revision 2.2  89/06/30  18:35:11  dpj
 * 	Initial revision.
 * 	[89/06/29  00:34:53  dpj]
 * 
 */

#ifndef lint
char * mountpt_rcsid = "$Header: mountpt.cc,v 2.3 94/07/07 17:23:44 mrt Exp $";
#endif	lint

#include <mountpt_ifc.h>
#include <agent_ifc.h>

/*
 * Class methods
 */
DEFINE_CLASS_MI(mountpt);

void mountpt::init_class(usClass* class_obj)
{
	usName::init_class(class_obj);
	vol_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(mountpt);
	SETUP_METHOD_WITH_ARGS(mountpt,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(mountpt,ns_read_forwarding_entry);
	END_SETUP_METHOD_WITH_ARGS;
}

void* mountpt::_castdown(const usClass& c) const
{
	if (&c == desc()) return (void*) this;
	void* p = usName::_castdown(c);
	void* q = p;
	if (p = vol_agency::_castdown(c)) ambig_check(p, q, c);
	return q;
}

mountpt::mountpt(ns_mgr_id_t mgr_id, access_table *acctab,
		 usItem* fwdobj, mach_error_t* ret)
	:
	vol_agency(mgr_id, acctab),
	fwd_obj(fwdobj)
{
	if (fwdobj) {
#if OLD_NS_REF
		fwdobj->ns_reference();
#endif OLD_NS_REF
	}
	mach_object_reference(fwdobj);
}


mountpt::~mountpt()
{
	mach_error_t		ret;

//	ret =  ns_dereference(Local(fwd_obj));		/* XXX */
//	if (fwd_obj) ret = invoke(fwd_obj,mach_method_id(ns_dereference));
#if OLD_NS_REF
	if (fwd_obj) ret = fwd_obj->ns_dereference();
#endif OLD_NS_REF
	mach_object_dereference(_Local(fwd_obj));
}

/*
 * Instance methods
 */
mach_error_t 
mountpt::ns_read_forwarding_entry(usItem** newobj, char *newpath)
{
	mach_error_t		ret;

	if (newpath != NULL) {
		newpath[0] = '\0';
	}
	*newobj = fwd_obj;
	mach_object_reference(*newobj);
#if OLD_NS_REF
	if (*newobj) ret = (*newobj)->ns_reference();
#endif OLD_NS_REF
	return(ERR_SUCCESS);
}


mach_error_t mountpt::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

	ret = this->vol_agency::ns_get_attributes(attr, attrlen);
	attr->type = NST_MOUNTPT;

	return(ret);
}

mach_error_t 
mountpt::ns_resolve(char* path, ns_mode_t mode, ns_access_t access, 
		    usItem** newobj, ns_type_t* newtype, char* newpath, 
		    int* usedlen, ns_action_t* action)
{
	return _notdef();
}

mach_error_t 
mountpt::ns_create(char *name, ns_type_t type, ns_prot_t prot, int protlen, 
		   ns_access_t access, usItem** newobj)
{
	return _notdef();
}

mach_error_t 
mountpt::ns_create_anon(ns_type_t type, ns_prot_t prot, int protlen, 
			ns_access_t access, char *name, usItem** newobj)
{
	return _notdef();
}

mach_error_t 
mountpt::ns_create_transparent_symlink(ns_name_t, ns_prot_t, int, char *)
{
	return _notdef();
}

mach_error_t mountpt::ns_insert_entry(char *name, usItem *target)
{
	return _notdef();
}

mach_error_t 
mountpt::ns_insert_forwarding_entry(char *name, ns_prot_t prot, int protlen, 
				    usItem* obj, char *path)
{
	return _notdef();
}

mach_error_t mountpt::ns_remove_entry(char *name)
{
	return _notdef();
}

mach_error_t
mountpt::ns_rename_entry(char *name, usItem* newdir, char *newname)
{
	return _notdef();
}


mach_error_t
mountpt::ns_list_entries(ns_mode_t mode, ns_name_t (**names), 
			 unsigned int *names_count, ns_entry_t *entries, 
			 unsigned int *entries_count)
{
	return _notdef();
}

mach_error_t mountpt::ns_allocate_unique_name(char *name)
{
	return _notdef();
}


mach_error_t mountpt::ns_list_types(ns_type_t **types, int *count)
{
	return _notdef();
}

