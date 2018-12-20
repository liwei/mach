
/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * tty_dir.c
 *
 * Mach tty server name methods module.
 *
 * Michael B. Jones  --  22-Aug-90
 */
/*
 * 
 * Purpose:  Support for the TTY directory of the tty server.
 * 
 * HISTORY
 * $Log:	tty_dir.cc,v $
 * Revision 2.6  94/07/21  16:14:51  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/01/11  18:12:02  jms
 * 	Add probe/select support
 * 	[94/01/10  13:48:51  jms]
 * 
 * Revision 2.4  92/07/05  23:36:18  dpj
 * 	Added dummy forwarding for remote_class_name (GXXBUG_VIRTUAL1).
 * 	[92/06/24  17:42:37  dpj]
 * 
 * Revision 2.3  92/03/05  15:15:24  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:47:03  jms]
 * 
 * Revision 2.2  91/11/06  14:24:13  jms
 * 	Update to use C++.
 * 	[91/09/17  14:21:49  jms]
 * 
 * Revision 2.3  91/05/05  19:33:47  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:09:40  dpj]
 * 
 * 	Fixed bug in call to create_tty_names().
 * 	Cleaned-up ns_list_types().
 * 	[91/04/28  11:07:16  dpj]
 * 
 * Revision 2.2  90/09/05  09:45:52  mbj
 * 	Wrote it.
 * 	[90/09/04  15:21:16  mbj]
 * 
 */

#include	<tty_dir_ifc.h>
#include	<tty_agency_ifc.h>
#include	<agent_ifc.h>
extern "C" {
#include	<tty_name.h>
}

extern "C" mach_error_t
    tty_read_proc(),
    tty_write_proc(),
    tty_end_proc(),
    tty_open_proc(),
    tty_bsd_ioctl_proc(),
    tty_select_proc();

extern "C" char *rindex();
void create_tty_names(tty_dir *);


#define BASE dir
DEFINE_CLASS(tty_dir);


/*
 * Class methods
 */
void tty_dir::init_class(usClass* class_obj)
{
	BASE::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(tty_dir);
	SETUP_METHOD_WITH_ARGS(tty_dir,ns_create);
	SETUP_METHOD_WITH_ARGS(tty_dir,ns_insert_entry);
	SETUP_METHOD_WITH_ARGS(tty_dir,ns_insert_forwarding_entry);
	SETUP_METHOD_WITH_ARGS(tty_dir,ns_list_types);
	END_SETUP_METHOD_WITH_ARGS;
}

tty_dir::~tty_dir()
{}

tty_dir::tty_dir()
{}

tty_dir::tty_dir(ns_mgr_id_t mgr_id, access_table* acctab)
	:
	dir(mgr_id, acctab)
{
}


tty_dir::tty_dir(ns_mgr_id_t mgr_id, mach_error_t* ret)
	:
	dir(mgr_id, ret)
{
	create_tty_names(this);
}

#ifdef	GXXBUG_VIRTUAL1
char* tty_dir::remote_class_name() const
	{ return dir::remote_class_name(); }
#endif	GXXBUG_VIRTUAL1

/*
 * Instance methods
 */
mach_error_t 
tty_dir::ns_create(char *name, ns_type_t type, ns_prot_t prot, int protlen,
		    ns_access_t access, usItem** newobj)
{
    int			tag;
    mach_error_t	ret;
    int		    	device_number;

    /*
     * We do not support anything other than ttys.
     */
    if (type != NST_TTY) {
	return(US_UNSUPPORTED);
    }

    {
	/*
	 * Get device_number for a tty name
	 */
	register tty_name *namep;
	char *lookup_name;

	lookup_name = rindex(name, '/')
	    ? (rindex(name, '/') + 1) /* XXX Trailing component only */
	    : name;

	if (! (namep = tty_name_lookup(lookup_name))) {
	printf("%s not found\n", lookup_name);
	return NS_NAME_NOT_FOUND;
	}

	device_number = namep->cdev_num;
/*
	printf("%s device_number = 0x%x\n", name, device_number);
*/
    }    

    ret = ns_reserve_entry(name,&tag);
    if (ret != ERR_SUCCESS) {
	return(ret);
    }

    tty_agency* newagency = new tty_agency(mgr_id, access_tab, device_number,
					   tty_read_proc, tty_write_proc,
					   tty_end_proc, tty_open_proc,
					   tty_bsd_ioctl_proc,
					   tty_select_proc);

    agent* agent_obj;
    ret = ns_create_common(tag, newagency, type, prot, protlen,
			   access, &agent_obj);
    *newobj = agent_obj;
    mach_object_dereference(newagency);

    return(ret);
}


mach_error_t tty_dir::ns_insert_entry(char *name, usItem* target)
{
    mach_error_t	ret;
    struct ns_attr	attr;
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

    if (attr.type != NST_TTY) {
	return(US_UNSUPPORTED);
    }

    ret = dir::ns_insert_entry(name, target);
    return(ret);
}


mach_error_t
tty_dir::ns_insert_forwarding_entry(char *name, ns_prot_t prot, int protlen,
				      usItem* obj, char *path)
{
	return(US_UNSUPPORTED);
}

mach_error_t tty_dir::ns_list_types(ns_type_t **types, int *count)
{
    mach_error_t	ret;
    vm_address_t	data;

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
    ((ns_type_t *)data)[0] = NST_TTY;

    *types = (ns_type_t *)data;

    return(NS_SUCCESS);
}
