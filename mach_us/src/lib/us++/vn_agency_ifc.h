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
 * ObjectClass: vn_agency
 *
 *	Basic agency for all files and directories in a vnode-based file
 *	server.
 *
 * SuperClass: agency
 *
 * ClassMethods:
 *	Exported: 
 *
 *	ns_get_attributes: return a standard attributes structure.
 *	ns_set_times: changes the access and modification times.
 *	ns_get_protection: return a standard prot structure.
 *	ns_set_protection: change the protection, using standard format.
 *	ns_get_privileged_id: return the authentication ID that has full
 *		privileges for the object.
 *	fs_set_authid_map: set up mapping between IDs and authids
 *
 *	Internal:
 *
 *	setup_vn_agency: initialize the agency.
 *	fs_get_fsid: return the FS ID for the underlying object.
 *	fs_convert_type_fs2ns: convert FS type designation into standard
 *		designation.
 *	ns_check_access: check access to the object, from a given
 *		credentials object.
 *	ns_create_initial_agent: create a first agent to export a
 *		file system.
 *
 * Notes:
 *
 *	This class implements all the operations common to files,
 *	directories and symlinks.
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/vn_agency_ifc.h,v $
 *
 * HISTORY:
 * $Log:	vn_agency_ifc.h,v $
 * Revision 2.3  94/07/07  17:25:45  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:31:47  dpj
 * 	First working version.
 * 	[92/06/24  17:25:42  dpj]
 * 
 * Revision 2.1  91/09/27  14:51:16  pjg
 * Created.
 * 
 * Revision 2.6  90/12/19  11:05:00  jjc
 * 	Added fs_set_authid_map to list of exported methods above.
 * 	[90/10/30            jjc]
 * 
 * Revision 2.5  90/11/10  00:38:32  dpj
 * 	Replaced ns_set_attributes() with ns_set_times().
 * 	[90/11/08  22:18:49  dpj]
 * 
 * 	Declared the ns_set_attributes method.
 * 	[90/10/24  15:31:40  neves]
 * 
 * Revision 2.4  89/10/30  16:32:21  dpj
 * 	Use fs_access for most of the dirty work.
 * 	Added setup methods.
 * 	Inherit more info through PublicLocal().
 * 	[89/10/27  17:33:46  dpj]
 * 
 * Revision 2.3  89/06/30  18:34:15  dpj
 * 	Added mgr_id.
 * 	[89/06/29  00:27:15  dpj]
 * 
 * Revision 2.2  89/03/17  12:40:22  sanzi
 * 	fs_export.h -> fs_types.h.
 * 	[89/02/15  21:49:43  dpj]
 * 	
 * 	*** empty log message ***
 * 	[89/02/07  13:47:35  dpj]
 * 
 */

#ifndef	_vn_agency_ifc_h
#define	_vn_agency_ifc_h

#include	<agency_ifc.h>
#include	<fs_access_ifc.h>
#include	<fs_cred_ifc.h>
#include	<agent_ifc.h>

class access_table;
class vn_mgr;

extern "C" {
#include	<ns_types.h>
#include	<fs_types.h>
#include	<sys/uio.h>
}


class vn_agency: public agency {
      protected:
	fs_id_t		fsid;		/* fs_id of underlying obj */
	vn_mgr*		vn_mgr_obj;
	fs_access*	fs_access_obj;

	struct mutex	vn_lock;
	int		vn_refcount;

      public:

	DECLARE_MEMBERS(vn_agency);
			vn_agency();
			vn_agency(fs_id_t, fs_access*, ns_mgr_id_t,
				  access_table*, vn_mgr*);
	virtual		~vn_agency();

	virtual void		vn_reference();
	virtual void		vn_dereference();
	virtual mach_error_t	vn_destroy();
	virtual mach_error_t	vn_clean();

	virtual void		vn_mark_permanent()
					{ (void) _notdef() ; }
	virtual void		vn_mark_temporary()
					{ (void) _notdef() ; }

//REMOTE virtual mach_error_t	fs_set_authid_map(char *, unsigned int);
	virtual mach_error_t	fs_set_authid_map(char *, unsigned int);
	virtual mach_error_t	fs_get_fsid(fs_id_t*);
	virtual ns_type_t	fs_convert_type_fs2ns(enum vtype);
	virtual mach_error_t	fs_check_access(ns_access_t, fs_cred*);
	virtual mach_error_t	ns_check_access(ns_access_t, std_cred*);
	virtual mach_error_t	fs_create_agent(ns_access_t,fs_cred*,agent**);

REMOTE	virtual mach_error_t	ns_get_attributes(ns_attr_t, int*);
REMOTE	virtual mach_error_t	ns_set_times(time_value_t, time_value_t);
REMOTE	virtual mach_error_t	ns_set_protection(ns_prot_t, int);
REMOTE	virtual mach_error_t	ns_get_protection(ns_prot_t, int*);
REMOTE	virtual mach_error_t	ns_get_privileged_id(int*);
	virtual mach_error_t	ns_create_initial_agent(agent**);
	virtual mach_error_t	ns_register_agent(ns_access_t);
	virtual mach_error_t	ns_unregister_agent(ns_access_t);

};


/************************************************\
 *						*
 *		Utility Macros. 		*
 *						*
\************************************************/

/*
 * Declaration for one UIO record describing a single buffer.
 */
#define	DECLARE_UIO(_name)					\
	struct iovec	_name##_iov;				\
	struct uio	_name

#define	SETUP_UIO(_name,_bufp,_len,_off) {			\
	_name##_iov.iov_base = (caddr_t)(_bufp);		\
	_name##_iov.iov_len = (_len);				\
	_name.uio_iov = &_name##_iov;				\
	_name.uio_iovcnt = 1;					\
	_name.uio_offset = (_off);				\
	_name.uio_segflg = 0;					\
	_name.uio_resid = (_len);				\
}


/*
 * Get a pointer to a FS-format credentials structure for the caller.
 */
 
#define	DECLARE_FS_CRED(_p)					\
	struct fs_cred_data		*_p


#define	SETUP_FS_CRED(_p)					\
	{							\
		std_cred*		ns_cred_obj;		\
		fs_cred*		fs_cred_obj;		\
		agent::base_object()->ns_get_cred_obj(&ns_cred_obj); \
		fs_cred_obj = fs_cred::castdown(ns_cred_obj);	\
		(void) fs_cred_obj->fs_get_cred_ptr(&_p);	\
		mach_object_dereference(ns_cred_obj);		\
	}



#endif	_vn_agency_ifc_h

