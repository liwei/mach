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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/access_table.cc,v $
 *
 * HISTORY:
 * $Log:	access_table.cc,v $
 * Revision 2.7  94/10/27  12:01:40  jms
 * 	Add tm_get_shared_info method.
 * 	[94/10/26  14:46:30  jms]
 * 
 * Revision 2.6  94/07/07  17:22:33  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  93/01/20  17:37:43  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  16:26:58  jms]
 * 
 * Revision 2.4  92/07/05  23:26:29  dpj
 * 	Fix access table to use new us_tm_{root,task,tgrp}_ifc.h interfaces
 * 	for the C++ taskmaster.
 * 	[92/06/24  15:21:24  jms]
 * 	Added entry for io_get_mf_state.
 * 	[92/06/24  15:58:01  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:46:26  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:06:15  dpj]
 * 
 * Revision 2.3  91/11/13  17:17:41  dpj
 * 	Removed entry for no_remote_senders, that is now handled by the agent
 * 	and not by the agent helper.
 * 	[91/11/07            dpj]
 * 
 * Revision 2.2  91/11/06  13:33:28  jms
 * 	Initial C++ revision.
 * 	[90/11/14  15:12:09  pjg]
 * 
 * 	Upgraded to US41.
 * 	[91/10/07  13:55:04  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:31:43  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:12:09  pjg]
 * 	Checking before c++ tty merge
 * 	[91/10/29  11:47:20  jms]
 * 
 * Revision 2.13  91/10/06  22:29:58  jjc
 * 	[91/10/01  18:54:16  jjc]
 * 
 *	Added sys_get_prefix_table.
 * 	[91/06/25            jjc]
 * 
 * Revision 2.12  91/07/01  14:11:12  jms
 * 	Added ns_get_manager().
 * 	[91/06/21  17:13:25  dpj]
 * 	Remove access_entries for ns_[de]reference
 * 	Add tm_hurtme, no_remote_senders
 * 	[91/06/24  17:13:25  jms]
 * 
 * Revision 2.11  91/05/05  19:25:31  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:52:27  dpj]
 * 
 * 	Added all network-related methods, new I/O interface and
 * 	ns_create_transparent symlink. 
 * 	[91/04/28  09:57:37  dpj]
 * 
 * Revision 2.10  90/12/19  11:04:34  jjc
 * 	Merged up to US32.
 * 	[90/12/05            jjc]
 * 	Include fs_methods.h.
 * 	[90/11/12            jjc]
 * 	Forgot a comma.
 * 	[90/11/12            jjc]
 * 	Added fs_set_authid_map.
 * 	[90/10/29            jjc]
 * 
 * Revision 2.9  90/11/27  18:20:09  jms
 * 	ns_set_times access added.
 * 	[90/11/20  14:23:01  jms]
 * 
 * 	Prepare to merge some changes from US31
 * 	[90/11/12  16:34:52  jms]
 * 
 * Revision 2.8  90/11/10  00:38:24  dpj
 * 	Replaced ns_set_attributes() with ns_set_times().
 * 	[90/11/08  22:18:11  dpj]
 * 
 * 	Added a table entry for ns_set_attributes.
 * 	[90/10/24  15:31:13  neves]
 * 
 * Revision 2.7  90/08/13  15:44:29  jjc
 * 	Added methods for interval timer.
 * 	[90/07/19            jjc]
 * 
 * Revision 2.6  90/07/09  17:08:52  dorr
 * 	Add task_master methods for modifying emulation process status
 * 	[90/07/06  17:33:36  jms]
 * 
 * Revision 2.5  90/03/21  17:23:03  jms
 * 	Further mods for useing the objectified Task Master
 * 	[90/03/16  17:00:24  jms]
 * 
 * 	First objectified Task Master checkin
 * 	[89/12/19  16:49:04  jms]
 * 
 * Revision 2.4  89/11/28  19:10:50  dpj
 * 	Added a default access entry for ns_get_agency_ptr.
 * 	This is needed for most implementations of ns_insert_entry(),
 * 	and does not introduce a security hole, since there is
 * 	no method argument string defined for ns_get_agency_ptr().
 * 	[89/11/28  11:58:35  dpj]
 * 
 * Revision 2.3  89/10/30  16:30:00  dpj
 * 	Re-organized to contain all the data necessary for a
 * 	default access table.
 * 	[89/10/27  16:50:22  dpj]
 * 
 * Revision 2.2  89/03/17  12:35:04  sanzi
 * 	initialize method id's
 * 	[89/02/09  18:00:42  dorr]
 * 	
 * 	include us_error.h
 * 	[89/02/08  14:44:06  dorr]
 * 	
 * 	First cut.
 * 	[89/02/07  16:24:20  dpj]
 * 
 */

#include <access_table_ifc.h>
#include <us_item_ifc.h>
#include <us_name_ifc.h>
#include <us_tm_root_ifc.h>
#include <us_tm_task_ifc.h>
#include <us_tm_tgrp_ifc.h>
#include <us_byteio_ifc.h>
#include <us_recio_ifc.h>
#include <us_net_name_ifc.h>
#include <us_net_connector_ifc.h>
#include <us_net_clts_recs_ifc.h>
#include <us_net_cots_ifc.h>
#include <us_sys_ifc.h>
#include <usint_mf_ifc.h>

extern "C" {
#include	<ns_types.h>
}

#define BASE usTop
DEFINE_LOCAL_CLASS(access_table);	

/* agent method invoked from MachExternalObject. Not remotely invoked */
EXPORT_METHOD(tty_bsd_ioctl);

begin_access_table(default_access_table_data)
//	access_entry(ns_reference,NSR_REFERENCE),
//	access_entry(ns_dereference,NSR_REFERENCE),
	access_entry(ns_authenticate,NSR_REFERENCE),
	access_entry(ns_duplicate,NSR_REFERENCE),
	access_entry(ns_get_access,NSR_REFERENCE),
	access_entry(ns_get_manager,NSR_REFERENCE),
	access_entry(ns_get_attributes,NSR_GETATTR),
	access_entry(ns_set_times,NSR_ADMIN),
	access_entry(ns_set_protection,NSR_ADMIN),
	access_entry(ns_get_protection,NSR_GETATTR),
	access_entry(ns_get_privileged_id,NSR_REFERENCE),
	access_entry(ns_resolve,NSR_LOOKUP),
	access_entry(ns_create,NSR_INSERT),
	access_entry(ns_create_anon,NSR_INSERT),
	access_entry(ns_create_transparent_symlink,NSR_INSERT),
	access_entry(ns_insert_forwarding_entry,NSR_INSERT),
	access_entry(ns_insert_entry,NSR_INSERT),
	access_entry(ns_remove_entry,NSR_DELETE),
	access_entry(ns_list_entries,NSR_READ),
	access_entry(ns_list_types,NSR_INSERT),
	access_entry(ns_read_forwarding_entry,NSR_READ),
	access_entry(ns_allocate_unique_name,NSR_INSERT),

//	access_entry(ns_get_agency_ptr,NSR_REFERENCE),

	access_entry(io_read,NSR_READ),
	access_entry(io_read_seq,NSR_READ),
	access_entry(io_read1rec,NSR_READ),
	access_entry(io_read1rec_seq,NSR_READ),
	access_entry(io_write,NSR_WRITE),
	access_entry(io_write_seq,NSR_WRITE),
	access_entry(io_write1rec,NSR_WRITE),
	access_entry(io_write1rec_seq,NSR_WRITE),
	access_entry(io_append,NSR_WRITE),
	access_entry(io_set_size,NSR_WRITE),
	access_entry(io_get_size,NSR_READ),	
	access_entry(io_get_record_count,NSR_READ),
//	access_entry(io_clean,NSR_WRITE),
	access_entry(io_get_mf_state,NSR_READ),

	access_entry(tty_bsd_ioctl,NSR_READ),

//	access_entry(nfs_mount,NSR_INSERT),

	access_entry(tm_task_id_to_task,NSR_REFERENCE),
	access_entry(tm_kernel_port_to_task,NSR_REFERENCE),
	access_entry(tm_pre_register_forked_task,NSR_REFERENCE),
	access_entry(tm_post_register_forked_task,NSR_REFERENCE),
	access_entry(tm_register_initial_task,NSR_REFERENCE),
	access_entry(tm_find_tgrp,NSR_REFERENCE),
	access_entry(tm_get_task_id,NSR_REFERENCE),
	access_entry(tm_get_kernel_port,NSR_REFERENCE),
	access_entry(tm_change_task_auth,NSR_REFERENCE),
	access_entry(tm_debug_children_of,NSR_REFERENCE),
	access_entry(tm_get_parent,NSR_REFERENCE),
	access_entry(tm_get_tgrp,NSR_REFERENCE),
	access_entry(tm_set_tgrp,NSR_REFERENCE),
	access_entry(tm_get_task_emul_status,NSR_REFERENCE),
	access_entry(tm_set_task_emul_status,NSR_REFERENCE),
	access_entry(tm_hurtme,NSR_REFERENCE),
	access_entry(tm_event_to_task,NSR_REFERENCE),
	access_entry(tm_timer_get,NSR_REFERENCE),
	access_entry(tm_timer_set,NSR_REFERENCE),
	access_entry(tm_timer_delete,NSR_REFERENCE),
	access_entry(tm_get_tgrp_id,NSR_REFERENCE),
	access_entry(tm_event_to_tgrp,NSR_REFERENCE),
#if SHARED_DATA_TIMING_EQUIVALENCE
	access_entry(tm_touch_shared,NSR_REFERENCE),
	access_entry(tm_get_shared_info,NSR_REFERENCE),
#endif SHARED_DATA_TIMING_EQUIVALENCE

//	access_entry(fs_set_authid_map,NSR_REFERENCE),

	access_entry(net_create,NSR_INSERT),
	access_entry(net_lookup,NSR_LOOKUP),
	access_entry(net_cots_lookup,NSR_LOOKUP),

	access_entry(net_get_localaddr,NSR_GETATTR),
	access_entry(net_get_peeraddr,NSR_GETATTR),
	access_entry(net_connect,NSR_INSERT),
	access_entry(net_listen,NSR_INSERT),
	access_entry(net_accept,NSR_INSERT),
	access_entry(net_reject,NSR_INSERT),
	access_entry(net_get_connect_qinfo,NSR_GETATTR),
	access_entry(net_set_connect_qmax,NSR_ADMIN),
	access_entry(net_snddis,NSR_READ),
	access_entry(net_rcvdis,NSR_READ),

	access_entry(net_clts_read1rec,NSR_READ),
	access_entry(net_clts_write1rec,NSR_WRITE),

	access_entry(sys_get_prefix_table,NSR_REFERENCE)
end_access_table(default_access_table_data);

/*
 * Internal routines.
 */
#define	access_table_table_size		128

int access_table_hash(mach_method_id_t id)
{
	return (((int)id) ^ (access_table_table_size - 1));
}

boolean_t access_table_compare(mach_method_id_t id1, mach_method_id_t id2)
{
	return(id1 == id2);
}

access_table::access_table() : table(0) {}

access_table::access_table(mach_error_t* ret)
{

	*ret = setup_access_table(default_access_table_data);
}

mach_error_t 
access_table::setup_access_table(ns_access_table_t access_data)
{
	int			i;
	boolean_t		ok;

	_Local(table) = hash_init(access_table_hash,access_table_compare,
					access_table_table_size);

	for (i = 0; access_data[i].method_id != NULL; i++) {
		ok = hash_enter(_Local(table),
					access_data[i].method_id,
					access_data[i].access);
		if (!ok) {
			return(US_OBJECT_EXISTS);
		}
	}

	return(ERR_SUCCESS);
}


access_table::~access_table()
{
	DESTRUCTOR_GUARD();
	hash_free(_Local(table));
}

mach_error_t 
access_table::ns_find_required_access(mach_method_id_t method_id, 
				      ns_access_t *req_access)
{
	*req_access = (ns_access_t)hash_lookup(_Local(table),method_id);
	if (*req_access == 0) {
		return(US_OBJECT_NOT_FOUND);
	} else {
		return(ERR_SUCCESS);
	}
}


