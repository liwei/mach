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
 * ObjectClass: vn_file
 *
 *	Agency for files in a vnode-based file server.
 *
 * SuperClass: fs_agency
 *
 * Delegated Objects: mfio_mgr if activated.
 *
 * ClassMethods:
 *
 * Notes:
 *
 *	All I/O operations for file objects are inherited from a delegate
 *	I/O object.
 */ 
/*
 * HISTORY
 * $Log:	vn_file_ifc.h,v $
 * Revision 2.4  94/07/07  17:25:53  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  94/05/17  14:08:21  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/28  18:55:51  jms]
 * 
 * Revision 2.2  92/07/05  23:32:00  dpj
 * 	First working version.
 * 	[92/06/24  17:26:49  dpj]
 * 
 * Revision 2.3  91/07/01  14:13:03  jms
 * 	Added act_obj_clean method; eliminated act_obj_clean_prepare
 * 	and act_obj_clean_commit methods.
 * 	[91/06/11  11:42:28  roy]
 * 
 * 	Convert to using ns_reference.
 * 	[91/06/07  10:46:07  roy]
 * 
 * 	Added 'writable' local var.  Got rid of unused local vars.
 * 	[91/06/06  17:05:27  roy]
 * 
 * 	Added permanent and temporary methods.
 * 	[91/06/05  12:46:58  roy]
 * 	ns_reference=>vn_reference
 * 	[91/06/24  17:35:57  jms]
 * 
 * Revision 2.2  90/12/21  14:14:56  jms
 * 	Initial revision.
 * 	[90/12/15  15:12:19  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:21:22  jms]
 * 
 * Revision 2.1  90/12/15  15:12:00  roy
 * Created.
 * 
 */

#ifndef	_vn_file_ifc_h
#define	_vn_file_ifc_h

#include	<usint_mf_ifc.h>
#include	<vn_agency_ifc.h>
#include	<vn_pager_ifc.h>
#include	<mf_mgr_ifc.h>


class vn_file: public vn_agency, public usint_mf {
	int			num_writers;
	boolean_t		io_active;
	boolean_t		temporary;
	boolean_t		writable;

	vn_pager		backing_obj;
	DECLARE_MF_MGR_PROP(mapping_obj);

      public:
	DECLARE_MEMBERS(vn_file);
				vn_file();
				vn_file(fs_id_t,fs_access*,ns_mgr_id_t,
					access_table*,vn_mgr*);
	virtual			~vn_file();
	virtual char*		remote_class_name() const;

	virtual void		vn_reference();
	virtual void		vn_dereference();
	virtual mach_error_t	vn_clean();
	virtual mach_error_t	vn_destroy();

	virtual void		vn_mark_permanent();
	virtual void		vn_mark_temporary();

	virtual mach_error_t	ns_register_agent(ns_access_t);
	virtual mach_error_t	ns_unregister_agent(ns_access_t);

	/*
	 * Methods exported remotely
	 */
REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
REMOTE	virtual mach_error_t io_read_seq(io_mode_t, char*, unsigned int*,
							 io_offset_t*)
					{ return US_UNSUPPORTED; }
REMOTE	virtual mach_error_t io_write_seq(io_mode_t, char*, unsigned int*,
					  io_offset_t*)
					{ return US_UNSUPPORTED; }
REMOTE	virtual mach_error_t io_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t)
					{ return US_UNSUPPORTED; }

};


#define	vn_file_lock()		\
	mutex_lock(&Local(lock))

#define	vn_file_unlock()	\
	mutex_unlock(&Local(lock))	


#endif	_vn_file_ifc_h


