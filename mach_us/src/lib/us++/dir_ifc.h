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
 * ObjectClass: dir
 *	Server-side object that is used to construct a hierarchical
 *	volatile name space.
 *
 * Notes:
 *	The dir object maintains a mapping from names to
 *	agency objects.
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/dir_ifc.h,v $
 *
 * Purpose: Generic dir object.
 *
 * HISTORY:
 * $Log:	dir_ifc.h,v $
 * Revision 2.4  94/07/07  17:23:13  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:27:15  dpj
 * 	Remove MI derivation from usName to avoid g++1.37 MI bug.
 * 	Add dir::dir_entry_count(void) to server internal use only.
 * 	Add a directory iterator for server internal use only.
 * 	ns_lookup_entry_internal -> ns_lookup_entry
 * 	Make ns_xxx_entry "public" as they always were all other classes
 * 	[92/06/24  15:57:04  jms]
 * 	GXXBUG_VIRTUAL2: (temporarily) remove inheritance on usName.
 * 	Made ns_forwarding_entry() return _notdef() inline.
 * 	[92/06/24  16:14:20  dpj]
 * 
 * 	Renamed "hash_table" to "entry_table".
 * 	[92/04/17  16:10:12  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:02  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:50:22  pjg]
 * 
 * 	Checkin for us tree reorganization
 * 	[88/11/18  11:21:36  dpj]
 * 
 * 	Dummy checkin for file creation.
 * 	[88/11/12  18:41:07  dpj]
 * 
 * Revision 2.5.1.2  91/04/14  18:24:51  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.5.1.1  90/11/14  17:05:36  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.6  91/05/05  19:26:09  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:53:23  dpj]
 * 
 * 	Reworked to support explicit link count, temporary and strong links.  
 * 	[91/04/28  10:06:17  dpj]
 * 
 * Revision 2.5  89/10/30  16:31:56  dpj
 * 	Reorganized to be more useful as a base class for specialized
 * 	directories. Derive from vol_agency.
 * 	[89/10/27  17:29:46  dpj]
 * 
 * Revision 2.4  89/06/30  18:33:45  dpj
 * 	Added link_count.
 * 	act_table -> active_table.
 * 	Added ns_insert_entry_internal() and ns_{increment,decrement}_link_count().
 * 	[89/06/29  00:21:08  dpj]
 * 
 * Revision 2.3  89/03/17  12:37:58  sanzi
 * 	Added the access_table to the local variables and setup call.
 * 	[89/02/24  18:34:12  dpj]
 * 	
 * 	Removed gen_ops.h.
 * 	Moved DIR_MGR_ID from here to dir_init.c.
 * 	Replaced all uses of act_id_t with ns_obj_id_t where appropriate.
 * 	Taken out ns_create_anon() and ns_read_forwarding_entry().
 * 	[89/02/20  22:20:25  dpj]
 * 	
 * 	add DIR_MGR_ID and ns_get_attributes.
 * 	[89/02/16  13:19:40  dorr]
 * 	
 * 	directory -> dir.
 * 	[89/02/15  15:22:35  dorr]
 * 	
 * 	modernize.
 * 	[89/02/14  14:41:04  dorr]
 * 
 */

#ifndef	_dir_ifc_h
#define	_dir_ifc_h

#include <us_name_ifc.h>
#include <vol_agency_ifc.h>

typedef agency *ns_tmplink_t;
/*
 * Generic dir entry.
 */
typedef struct dir_entry {
	struct dir_entry	*next;		/* link in hash bucket */
	ns_type_t		type;
	ns_name_t		name;
	int			namelen;
	ns_obj_id_t		obj_id;
	ns_tmplink_t		obj;
} *dir_entry_t;

class dir_iterator;

#ifdef	GXXBUG_VIRTUAL2
class dir: public vol_agency {
#else	GXXBUG_VIRTUAL2
class dir: public vol_agency, public usName {
#endif	GXXBUG_VIRTUAL2
	struct mutex		lock;
	struct condition	reserved_sleep_cond;
	unsigned int		link_count;
	unsigned int		entry_count;
	dir_entry_t		*entry_table;
	unsigned int		anon_name_seed;

      private:
	static ns_mgr_id_t new_mgr_id(ns_mgr_id_t =null_mgr_id);
	inline void _setup_dir(void);
      protected:
	friend class dir_iterator;

	virtual mach_error_t ns_create_common(int, agency*, ns_type_t,
					      ns_prot_t, int, ns_access_t,
					      agent**);

	virtual int dir_entry_count(void);

      public:
	/* These routines are for intra-server use only */
	virtual mach_error_t ns_register_tmplink(dir*, int);
	virtual mach_error_t ns_unregister_tmplink(int);
	virtual mach_error_t ns_reference_tmplink(void);
	virtual mach_error_t ns_register_stronglink(void);
	virtual mach_error_t ns_unregister_stronglink(void);

	virtual mach_error_t ns_reserve_entry(ns_name_t, int*);
	virtual mach_error_t ns_install_entry(int, agency*, ns_type_t);
	virtual mach_error_t ns_cancel_entry(int);
	virtual mach_error_t ns_lookup_entry(ns_name_t, int, 
						      agency**, ns_type_t*);

	virtual mach_error_t ns_remove_tmplink(int tag);

      public:
	DECLARE_MEMBERS(dir);
	dir();
	dir(ns_mgr_id_t, access_table*);	/* replaces setup_dir*/
	dir(ns_mgr_id_t, mach_error_t*);

	virtual ~dir();
	virtual char* remote_class_name() const;

	/*
	 * Methods exported remotely
	 */
REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
REMOTE	virtual mach_error_t ns_resolve(ns_path_t, ns_mode_t, ns_access_t, 
					usItem** , ns_type_t*, ns_path_t, 
					int*, ns_action_t*);
REMOTE	virtual mach_error_t ns_create(ns_name_t, ns_type_t, ns_prot_t, int,
				       ns_access_t, usItem**);
REMOTE	virtual mach_error_t ns_create_anon(ns_type_t, ns_prot_t, int,
					    ns_access_t, ns_name_t, usItem**);
REMOTE	virtual mach_error_t ns_create_transparent_symlink(ns_name_t, ns_prot_t,
							  int, char *);
REMOTE	virtual mach_error_t ns_insert_entry(char*, usItem*);
REMOTE	virtual mach_error_t ns_insert_forwarding_entry(ns_name_t, ns_prot_t, 
							int, usItem*, char*);
REMOTE	virtual mach_error_t ns_remove_entry(ns_name_t);
REMOTE	virtual mach_error_t ns_rename_entry(ns_name_t, usItem*, ns_name_t);
REMOTE	virtual mach_error_t ns_list_entries(ns_mode_t, ns_name_t**,
					     unsigned int*, ns_entry_t*,
					     unsigned int*);
REMOTE	virtual mach_error_t ns_list_types(ns_type_t**, int*);
REMOTE	virtual mach_error_t ns_allocate_unique_name(ns_name_t);

	/*
	 * From usName but not implemented.
	 */
REMOTE	virtual mach_error_t ns_read_forwarding_entry(usItem** a, char* b)
					{ return _notdef(); }

};


/*
 * Iterator for looping thru directories.  For server internal use only!
 */
class dir_iterator {
    private:
	int		hash_index;
	dir_entry_t	entry;
	dir		*iter_dir;

    public:
	dir_iterator(dir *);
	virtual ~dir_iterator(void);
	
	/*
	 * dir_iterate:
	 *	Iterate thru the entries in a directory.
	 *
	 *	NOTE: THE FIRST ITERATION CAUSES THE DIRECTORY TO BE LOCKED.
	 *		IT MUST BE UNLOCKED BY EITHER ITERATING THRU ALL
	 *		THE ENTRIES IN THE DIR OR CANCELING THE ITERATION.
	 */
	mach_error_t dir_iterator::dir_iterate(dir_entry_t *);

	/*
	 * dir_cancel_iter:
	 *	Stop an iteration thru the entries in a directory prematurely
	 *	Doing necessary cleanup an lock releasing.
	 *
	 *	NOTE: THIS ROUTINE MUST BE CALLED TO STOP ITTERATING BEFORE
	 *		GOING THRU ALL THE ENTRIES OR THE DIRECTORY WILL BE
	 *		LEFT LOCKED!
	 */
	mach_error_t dir_iterator::dir_cancel_iteration(void);
};
#endif	_dir_ifc_h

