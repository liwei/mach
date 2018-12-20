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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tty/tty_agency.cc,v $
 *
 * Purpose: Base class for the manager side of all I/O objects.
 *
 * HISTORY:
 * $Log:	tty_agency.cc,v $
 * Revision 2.5  94/07/21  16:14:47  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  94/01/11  18:11:49  jms
 * 	Use "usTTY_proxy" remote object name, not "tty_bsd_proxy".
 * 	Add tty "probe" and "select" support
 * 	[94/01/10  13:47:12  jms]
 * 
 * Revision 2.3  92/07/05  23:36:16  dpj
 * 	Remote class name = tty_bsd_proxy.
 * 	[92/06/24  17:41:32  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:33:40  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:56:50  dpj]
 * 
 * Revision 2.2  91/11/06  14:24:07  jms
 * 	Update to use C++ language and US object structure.
 * 	[91/09/17  14:32:02  jms]
 * 
 * Revision 2.4  91/05/05  19:33:38  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:09:24  dpj]
 * 
 * 	Derive from vol_agency instead of agency. Do not do a full open if
 * 	just stat()'ing the item.
 * 	[91/04/28  11:04:51  dpj]
 * 
 * Revision 2.3  91/03/25  14:15:07  jjc
 * 	Changed tty_agency_ns_register_agent() to convert the access to 
 * 	flags when passing it to tty_open_proc().
 * 	[91/02/18            jjc]
 * 
 * Revision 2.2  90/09/05  09:45:42  mbj
 * 	Moved here from lib/us.
 * 	[90/09/04  15:20:19  mbj]
 * 
 * Revision 1.5  89/10/30  16:36:53  dpj
 * 	Fixed for new act_enter() arguments and new
 * 	setup method for the agency class.
 * 	[89/10/27  19:25:41  dpj]
 * 
 * Revision 1.4  89/05/17  16:44:37  dorr
 * 	include file cataclysm
 * 
 * Revision 1.3  89/03/30  12:06:47  dpj
 * 	Updated to new syntax for invoke_super().
 * 	[89/03/26  18:52:05  dpj]
 * 
 * Revision 1.2  89/03/17  12:54:06  sanzi
 * 	Removed ns_create_agent(), which can now be inherited from the
 * 	agency class,as long as we intercept ns_register_agent().
 * 	Fixed the length returned with ns_get_attributes().
 * 	[89/02/25  00:31:09  dpj]
 * 	
 * 	Added the access_table to the local variables and the setup
 * 	method, and removed it from ns_create_agent().
 * 	[89/02/24  18:44:32  dpj]
 * 	
 * 	Enter the agency in the active table as part of the
 * 	setup method.
 * 	Use ns_get_protection_ltd() for the attributes structure.
 * 	[89/02/22  23:04:50  dpj]
 * 	
 * 	Fixed the "count" argument in I/O operations to be io_count_t
 * 	instead of io_size_t.
 * 	Clear the time_values in the attributes structure.
 * 	[89/02/22  14:45:36  dpj]
 * 	
 * 	First cut at reorganized TTY server.
 * 	[89/02/21  17:59:02  dpj]
 * 
 * Revision 1.1  88/11/01  14:33:34  mbj
 * Initial revision
 * 
 *
 */

#ifndef lint
char * tty_agency_rcsid = "$Header: tty_agency.cc,v 2.5 94/07/21 16:14:47 mrt Exp $";
#endif	lint

#include	"tty_agency_ifc.h"
#include	<agent_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<ns_error.h>
#include 	<sys/types.h>
#include	<sys/file.h>
}

/*
 * From tty_io_procs.c.
 */
extern "C" int io_mode_to_flags();
extern "C" int ns_access_to_flags();



/*
 * Convert a TTY device number into an object ID.
 */
#define	DEVNUM_TO_ID(_devnum)	((ns_obj_id_t)(_devnum))


DEFINE_CLASS_MI(tty_agency)
DEFINE_CASTDOWN2(tty_agency, usByteIO, vol_agency)

EXPORT_METHOD(tty_bsd_ioctl);

/*
 * Class Methods
 */
void tty_agency::init_class(usClass* class_obj)
{
    usByteIO::init_class(class_obj);
    vol_agency::init_class(class_obj);

    BEGIN_SETUP_METHOD_WITH_ARGS(tty_agency);
    SETUP_METHOD_WITH_ARGS(tty_agency, ns_get_attributes);

    SETUP_METHOD_WITH_ARGS(tty_agency, io_read_seq);
    SETUP_METHOD_WITH_ARGS(tty_agency, io_write_seq);
    SETUP_METHOD_WITH_ARGS(tty_agency, io_set_size);

    SETUP_METHOD_WITH_ARGS(tty_agency, tty_bsd_ioctl);
    END_SETUP_METHOD_WITH_ARGS;
}

#ifdef OLD_CASTDOWN
void* tty_agency::_castdown(const usClass& c) const
{
	if (&c == desc()) return (void*) this;
	void* p = usByteIO::_castdown(c);
	void* q = p;
	if (p = vol_agency::_castdown(c)) ambig_check(p, q, c);
	return q;
}
#endif OLD_CASTDOWN

tty_agency::tty_agency() : 
       device_number(-1),
       ext_read(0),
       ext_write(0),
       ext_end(0),
       ext_open(0),
       ext_bsdctl(0)
{}

tty_agency::tty_agency(
    ns_mgr_id_t		mgr_id,
    access_table	*acctab,
    int			devnum,
    mach_error_fn_t	readproc,
    mach_error_fn_t	writeproc,
    mach_error_fn_t	endproc,
    mach_error_fn_t	openproc,
    mach_error_fn_t	bsd_ioctlproc,
    mach_error_fn_t	selectproc)
    :
    vol_agency(mgr_id,acctab),
    device_number(devnum),
    ext_read(readproc),
    ext_write(writeproc),
    ext_end(endproc),
    ext_open(openproc),
    ext_bsdctl(bsd_ioctlproc),
    ext_select(selectproc)
{
/*	attributes |= IOA_EXCLUSIVE | IOA_NODELAY;	XXX */
}


tty_agency::~tty_agency()
{}

char* tty_agency::remote_class_name() const
{
	return "usTTY_proxy";
}

/*
 * Register the presence of a new agent for this agency.
 */
mach_error_t tty_agency::ns_register_agent(
    ns_access_t		access)
{
    mach_error_t	ret = ERR_SUCCESS;
    std_cred		*cred;
    ns_authid_t		userid;
    int			flags;

    flags = ns_access_to_flags(access);

    /*
     * If opening for real, tell the underlying UNIX code.
     * If just stat()'ing, don't bother.
     */
    if (flags & (FREAD | FWRITE)) {

	ret = agent::base_object()->ns_get_cred_obj(&cred);
	if (ret != ERR_SUCCESS) {
	    return(ret);
	}

	ret = cred->ns_get_principal_id(&userid);
	if (ret != ERR_SUCCESS) {
	    mach_object_dereference(cred);
	    return(ret);
	}

	ret = (*ext_open)(device_number, flags, userid);
	if (ret != ERR_SUCCESS) {
	    mach_object_dereference(cred);
	    return(ret);
	}

	mach_object_dereference(cred);
    }

    ret = this->vol_agency::ns_register_agent(access);
    if (ret == MACH_OBJECT_NO_SUCH_OPERATION) ret = ERR_SUCCESS;

    return(ret);
}


/*
 * Register the disappearance of an agent for this agency.
 */
mach_error_t tty_agency::ns_unregister_agent(ns_access_t access)
{
    mach_error_t	ret = ERR_SUCCESS;
    int			flags;

    flags = ns_access_to_flags(access);

    /*
     * If we were opened for real, tell the underlying UNIX code.
     * If we were just stat()'ing, don't bother.
     */
    if (flags & (FREAD | FWRITE)) {
	ret = (*ext_end)(device_number);
	if (ret != ERR_SUCCESS) {
	    return(ret);
	}
    }

    ret = this->vol_agency::ns_unregister_agent(access);
    if (ret == MACH_OBJECT_NO_SUCH_OPERATION) ret = ERR_SUCCESS;

    return(ret);
}

mach_error_t 
tty_agency::io_read_seq(io_mode_t mode, char *buf, io_count_t *count,
				io_offset_t *offset)
{
    mach_error_t	ret;
    if (mode & IOM_PROBE) {
	/* we have a prove, doit with a select */
	ret = ((*ext_select)(device_number, FREAD));
	return(ret);
    }

    INT_TO_IO_OFF(0,offset);	/* Is it worth the overhead to make this real? */
    ret = ((*ext_read)(device_number, io_mode_to_flags(mode), buf, count));
}


mach_error_t 
tty_agency::io_write_seq(io_mode_t mode, char *buf, io_count_t *count,
				 io_offset_t *offset)
{
    mach_error_t	ret;
    if (mode & IOM_PROBE) {
	/* we have a prove, doit with a select */
	ret = ((*ext_select)(device_number, FWRITE));
	return(ret);
    }

    INT_TO_IO_OFF(0,offset);	/* Is it worth the overhead to make this real? */
    ret = ((*ext_write)(device_number, io_mode_to_flags(mode), buf, count));
}


mach_error_t tty_agency::tty_bsd_ioctl(
    unsigned long	cmd,
    pointer_t		arg)		/* inout */
{
    mach_error_t	ret = ERR_SUCCESS;
    std_cred		*cred;
    ns_authid_t		userid;

    ret = agent::base_object()->ns_get_cred_obj(&cred);
    if (ret != ERR_SUCCESS) {
	return(ret);
    }

    ret = cred->ns_get_principal_id(&userid);
    if (ret != ERR_SUCCESS) {
	mach_object_dereference(cred);
	return(ret);
    }

    mach_object_dereference(cred);

    ret = (*ext_bsdctl)(device_number, cmd, arg, userid);

    return(ret);
}

mach_error_t tty_agency::ns_get_attributes(
    ns_attr_t		attr,
    int			*attrlen)
{
    mach_error_t		ret;

    ret = vol_agency::ns_get_attributes(attr,attrlen);
    if (ret != ERR_SUCCESS) {
    	return(ret);
    }

    attr->type = NST_TTY;
    attr->obj_id = DEVNUM_TO_ID(device_number);

    return(ERR_SUCCESS);

}


/*
 * Unsupported methods
 */

mach_error_t 
tty_agency::io_read(io_mode_t, io_offset_t, pointer_t, 
				     unsigned int*)
{
	return(MACH_OBJECT_NO_SUCH_OPERATION);
}
mach_error_t 
tty_agency::io_write(io_mode_t, io_offset_t, pointer_t, unsigned int*)
{
	return(MACH_OBJECT_NO_SUCH_OPERATION);
}
mach_error_t 
tty_agency::io_append(io_mode_t, pointer_t, unsigned int*)
{
	return(MACH_OBJECT_NO_SUCH_OPERATION);
}
mach_error_t 
tty_agency::io_set_size(io_size_t)
{
	return(ERR_SUCCESS);
}
mach_error_t 
tty_agency::io_get_size(io_size_t *)
{
	return(MACH_OBJECT_NO_SUCH_OPERATION);
}
mach_error_t 
tty_agency::io_map(task_t, vm_address_t*, vm_size_t,
			   vm_offset_t, boolean_t, vm_offset_t,
			   boolean_t, vm_prot_t, vm_prot_t,
			   vm_inherit_t)
{
	return(MACH_OBJECT_NO_SUCH_OPERATION);
}
