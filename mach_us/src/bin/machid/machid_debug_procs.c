/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	machid_debug_procs.c,v $
 * Revision 2.2  94/07/13  16:37:08  mrt
 * 	copied here from the user area
 * 
 * Revision 2.10  93/04/14  11:43:06  mrt
 * 	64bit cleanup.
 * 	[92/12/02            af]
 * 
 * Revision 2.9  92/01/22  22:51:44  rpd
 * 	Added <servers/machid_lib.h>.
 * 	[92/01/22            rpd]
 * 
 * Revision 2.8  92/01/17  14:23:20  rpd
 * 	Replaced old vm_region_info call with new
 * 	vm_region_info, vm_object_info, vm_object_pages.
 * 	[92/01/02            rpd]
 * 
 * 	Added port_kernel_object, mach_kernel_object.
 * 	[91/12/16            rpd]
 * 
 * Revision 2.7  91/08/30  14:51:58  rpd
 * 	Moved machid include files into the standard directory.
 * 
 * Revision 2.6  91/03/27  17:26:06  mrt
 * 	Changed mach.h include
 * 
 * Revision 2.5  91/03/19  12:30:26  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.4  91/03/10  13:41:15  rpd
 * 	Added host_zone_info.
 * 	[91/01/14            rpd]
 * 
 * Revision 2.3  90/11/05  23:33:58  rpd
 * 	Added host_stack_usage, processor_set_stack_usage.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.2  90/09/12  16:31:39  rpd
 * 	Created.
 * 	[90/06/18            rpd]
 * 
 */

#include <stdio.h>
#include <mach.h>
#include <mach_debug/mach_debug.h>
#include <servers/machid_types.h>
#include <servers/machid_lib.h>
#include "machid_internal.h"

kern_return_t
do_port_get_srights(server, auth, task, name, srightsp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_t name;
    mach_port_rights_t *srightsp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_port_get_srights(port, name, srightsp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_space_info(server, auth, task, infop,
		   table_infop, table_infoCntp,
		   tree_infop, tree_infoCntp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    ipc_info_space_t *infop;
    ipc_info_name_array_t *table_infop;
    natural_t *table_infoCntp;
    ipc_info_tree_name_array_t *tree_infop;
    natural_t *tree_infoCntp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_port_space_info(port, infop,
			      table_infop, table_infoCntp,
			      tree_infop, tree_infoCntp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_dnrequest_info(server, auth, task, name, totalp, usedp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_t name;
    unsigned int *totalp, *usedp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_port_dnrequest_info(port, name, totalp, usedp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_stack_usage(server, auth, host,
		    reserved, total, space, resident, maxusage, maxstack)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    vm_size_t *reserved;
    unsigned int *total;
    vm_size_t *space;
    vm_size_t *resident;
    vm_size_t *maxusage;
    vm_offset_t *maxstack;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    kr = host_stack_usage(port, reserved, total, space, resident,
			  maxusage, maxstack);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_stack_usage(server, auth, pset,
			     total, space, resident, maxusage, maxstack)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_t pset;
    unsigned int *total;
    vm_size_t *space;
    vm_size_t *resident;
    vm_size_t *maxusage;
    vm_offset_t *maxstack;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET_NAME;
	return kr;
    }

    kr = processor_set_stack_usage(port, total, space, resident,
				   maxusage, maxstack);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_zone_info(server, auth, host,
		  namesp, namesCntp, infop, infoCntp)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    zone_name_array_t *namesp;
    natural_t *namesCntp;
    zone_info_array_t *infop;
    natural_t *infoCntp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    kr = host_zone_info(port, namesp, namesCntp, infop, infoCntp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_kernel_object(server, auth, task, name, object_typep, object_addrp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_t name;
    unsigned int *object_typep;
    vm_offset_t *object_addrp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_port_kernel_object(port, name, object_typep, object_addrp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

static mach_type_t
convert_to_mach_type(type)
    unsigned int type;
{
    switch (type) {
      case IPC_INFO_TYPE_NONE:
	return MACH_TYPE_NONE;
      case IPC_INFO_TYPE_THREAD:
	return MACH_TYPE_THREAD;
      case IPC_INFO_TYPE_TASK:
	return MACH_TYPE_TASK;
      case IPC_INFO_TYPE_HOST:
	return MACH_TYPE_HOST;
      case IPC_INFO_TYPE_HOST_PRIV:
	return MACH_TYPE_HOST_PRIV;
      case IPC_INFO_TYPE_PROCESSOR:
	return MACH_TYPE_PROCESSOR;
      case IPC_INFO_TYPE_PSET:
	return MACH_TYPE_PROCESSOR_SET;
      case IPC_INFO_TYPE_PSET_NAME:
	return MACH_TYPE_PROCESSOR_SET_NAME;
      case IPC_INFO_TYPE_PAGER:
	return MACH_TYPE_OBJECT;
      case IPC_INFO_TYPE_PAGING_REQUEST:
	return MACH_TYPE_OBJECT_CONTROL;
      case IPC_INFO_TYPE_PAGING_NAME:
	return MACH_TYPE_OBJECT_NAME;
      default:
	return MACH_TYPE_NONE;
    }
}

kern_return_t
do_mach_kernel_object(server, auth, id, object_typep, object_addrp)
    mach_port_t server;
    mach_port_t auth;
    mach_id_t id;
    mach_type_t *object_typep;
    vm_offset_t *object_addrp;
{
    unsigned int type;
    vm_offset_t addr;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(id, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS)
	return kr;

    kr = mach_port_kernel_object(mach_task_self(), port, &type, &addr);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    *object_typep = convert_to_mach_type(type);
    *object_addrp = addr;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_vm_region_info(server, auth, task, addr, infop)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    vm_offset_t addr;
    vm_region_info_t *infop;
{
    mach_port_t object;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_vm_region_info(port, addr, infop, &object);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* propogate host from task to memory object */
    assoc_create(infop->vri_object =
			name_lookup(object, MACH_TYPE_OBJECT_NAME),
		 MACH_TYPE_HOST_PRIV,
		 assoc_lookup(task, MACH_TYPE_HOST_PRIV));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_vm_object_info(server, auth, object, infop)
    mach_port_t server;
    mach_port_t auth;
    mobject_name_t object;
    vm_object_info_t *infop;
{
    mhost_priv_t host;
    mach_port_t shadow, copy;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(object, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_MEMORY_OBJECT;
	return kr;
    }

    kr = mach_vm_object_info(port, infop, &shadow, &copy);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* propogate host among memory objects */
    host = assoc_lookup(infop->voi_object = object, MACH_TYPE_HOST_PRIV);
    assoc_create(infop->voi_shadow =
			name_lookup(shadow, MACH_TYPE_OBJECT_NAME),
		 MACH_TYPE_HOST_PRIV, host);
    assoc_create(infop->voi_copy =
			name_lookup(copy, MACH_TYPE_OBJECT_NAME),
		 MACH_TYPE_HOST_PRIV, host);

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_vm_object_pages(server, auth, object, pagesp, countp)
    mach_port_t server;
    mach_port_t auth;
    mobject_name_t object;
    vm_page_info_array_t *pagesp;
    unsigned int *countp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(object, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_MEMORY_OBJECT;
	return kr;
    }

    kr = mach_vm_object_pages(port, pagesp, countp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
