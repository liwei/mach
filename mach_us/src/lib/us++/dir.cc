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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/dir.cc,v $
 *
 * Purpose: Generic directory object.
 *
 * HISTORY:
 * $Log:	dir.cc,v $
 * Revision 2.8  94/10/27  12:01:46  jms
 * 	Bugfix to directory iterator
 * 	[94/10/26  14:48:01  jms]
 * 
 * Revision 2.7  94/07/07  17:23:11  mrt
 * 	Updated copyright.
 * 
 * Revision 2.6  94/01/11  17:50:01  jms
 * 	Take locking out of dir_entry_count.  Locking now required at a higher
 * 	level (when needed?)
 * 	[94/01/09  19:39:23  jms]
 * 
 * Revision 2.5  92/07/05  23:27:11  dpj
 * 	hash_table -> entry_table (missed in jms merge).
 * 	[92/07/05  18:54:11  dpj]
 * 
 * 	Remove MI derivation from usName to avoid g++1.37 MI bug.
 * 	Remove ancient castdown decl
 * 	ns_lookup_entry_internal -> ns_lookup_entry
 * 	Add dir::dir_entry_count(void) to server internal use only.
 * 	Replace ns_get_agency_ptr with ns_get_item_ptr, take out null castdown hack.
 * 	Add a directory iterator for server internal use only.
 * 	[92/06/24  15:44:11  jms]
 * 	GXXBUG_VIRTUAL2: (temporarily) remove inheritance on usName.
 * 	Remote class = usName_proxy.
 * 	[92/06/24  16:13:04  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:53:40  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	Renamed "hash_table" to "entry_table".
 * 	[92/04/17  16:09:41  dpj]
 * 
 * Revision 2.4  92/03/05  15:05:30  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:30:35  jms]
 * 
 * Revision 2.3  91/11/13  17:17:51  dpj
 * 	condition_wait() -> intr_cond_wait()
 * 	[91/11/13  14:49:57  dpj]
 * 
 * Revision 2.2  91/11/06  13:45:55  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:29:46  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:24:39  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:36:06  pjg]
 * 
 * Revision 2.7  91/07/01  14:11:51  jms
 * 	Added setup for mgr_methods.
 * 	[91/06/21  18:08:56  dpj]
 * 
 * Revision 2.6  91/05/05  19:26:03  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:53:15  dpj]
 * 
 * 	Reworked to support explicit link count, temporary and strong links. 
 * 	Support transparent symlinks.
 * 	[91/04/28  10:05:33  dpj]
 * 
 * 	Fixed incorrect comparison of mgr_id in ns_insert_entry().
 * 	[91/02/25  10:26:55  dpj]
 * 
 * Revision 2.5  90/08/22  18:12:36  roy
 * 	Use mach_get_time() to get random mgr id.
 * 	[90/08/14  11:53:41  roy]
 * 
 * Revision 2.4  89/10/30  16:31:40  dpj
 * 	Reorganized to be more useful as a base class for specialized
 * 	directories. Derive from vol_agency.
 * 	[89/10/27  17:28:54  dpj]
 * 
 * Revision 2.3  89/06/30  18:33:37  dpj
 * 	Additional comment forgotten with the previous revision:
 * 	Modified setup routine to obtain the mgr ID as an argument.
 * 	[89/06/29  01:09:40  dpj]
 * 
 * 	Removed all uses of the active_table.
 * 	Made ns_insert_entry_internal() a real method, replacing add_dir_entry().
 * 	Reorganized ns_resolve() to deal with mount points and symbolic links.
 * 	First pass at keeping track of the link_count.
 * 	[89/06/29  00:19:25  dpj]
 * 
 * Revision 2.2  89/03/17  12:37:42  sanzi
 * 	fix reference counting.  use ns_duplicate to get new access for resolves
 * 	to namespace root.  enter forwarding objects into hash table.
 * 	[89/02/28  00:15:14  dorr]
 * 	
 * 	Do not use hash_free() to get rid of the malloc()ed hash table.
 * 	Added act_table and access_table to the local variables and relevant
 * 	methods.
 * 	Fixed ns_create() to return an agent and not the new object itself
 * 	Fixed ns_get_attributes() to return the correct length (in words).
 * 	[89/02/24  18:33:23  dpj]
 * 	
 * 	Start object ID's at 1 instead of 0.
 * 	Enter the agency in the access table as part of the setup
 * 	method.
 * 	Use ns_get_protection_ltd() to set the attributes structure.
 * 	Zero the times in the attributes structure.
 * 	[89/02/22  22:57:21  dpj]
 * 	
 * 	Added NS_ATTR_PROT to the valid_fields info in ns_get_attributes.
 * 	[89/02/21  17:54:52  dpj]
 * 	
 * 	Removed sys/dir.h. Use NS_NAMELEN instead of MAXNAMLEN.
 * 	Removed ns_create_anon() and ns_read_forwarding_entry().
 * 	Replaced all uses of act_id_t with ns_obj_id_t where appropriate.
 * 	[89/02/20  22:22:31  dpj]
 * 	
 * 	add forwarding objects.
 * 	[89/02/16  15:58:53  dorr]
 * 	
 * 	add ns_get_attributes.
 * 	[89/02/16  13:20:18  dorr]
 * 	
 * 	first working version.
 * 	[89/02/15  15:25:38  dorr]
 * 
 */

#include <dir_ifc.h>
#include <tsymlink_ifc.h>
#include <mountpt_ifc.h>

#include <access_table_ifc.h>
#include <agent_ifc.h>

extern "C" {
#include <mach/time_value.h>     /* mach_get_time() */
extern mach_error_t mach_get_time();
#include <ns_types.h>
extern void _mach_static_class_load_cat();
#include	<interrupt.h>
}

#ifdef	GXXBUG_VIRTUAL2
#define	BASE	vol_agency
DEFINE_CLASS(dir);
#else	GXXBUG_VIRTUAL2
DEFINE_CLASS_MI(dir);
DEFINE_CASTDOWN2(dir,usName,vol_agency);
#endif	GXXBUG_VIRTUAL2

/*
 * Manipulation of object ID's.
 */
static struct mutex	dir_id_lock = MUTEX_INITIALIZER;
static ns_obj_id_t	dir_id = 0;

#define	NEW_ID(_id)	\
	{ 	mutex_lock(&dir_id_lock); \
		*(_id) = ++dir_id; \
		mutex_unlock(&dir_id_lock); \
	}	

/*
 * Manipulation of the hash table containing directory entries.
 */
#define	HASHSIZE	32
#define	HASHMASK	((HASHSIZE)-1)
#define NAME_HASH(_name, _len, _ret)				\
{								\
	register char * name_0 = _name;				\
	register int	sum_0 = 0;				\
	register int	_i;					\
	if (name_0 != NULL) {					\
		for (_i = 0; _i < _len; _i++) {			\
			sum_0 = sum_0^(unsigned char)*name_0++;	\
		}						\
	}							\
	_ret = (sum_0 & HASHMASK);				\
}

#define	SCAN_FOR_NAME(_hash_offset,_name,_namelen,_entry,_prev)	\
{								\
	register dir_entry_t		hash_bucket;		\
								\
	hash_bucket = Local(entry_table)[(_hash_offset)];	\
	(_prev) = &Local(entry_table)[(_hash_offset)];		\
	(_entry) = NULL;					\
	while (hash_bucket != NULL) {				\
		if (((_namelen) == hash_bucket->namelen) &&	\
			!bcmp(hash_bucket->name, (_name), (_namelen))) { \
			(_entry) = hash_bucket;			\
			break;					\
		}						\
		(_prev) = &hash_bucket->next;			\
		hash_bucket = hash_bucket->next;		\
	}							\
}

#define	SCAN_FOR_ENTRY(_hash_offset,_entry,_prev,_found)	\
{								\
	register dir_entry_t		hash_bucket;		\
								\
	hash_bucket = Local(entry_table)[(_hash_offset)];	\
	(_prev) = &Local(entry_table)[(_hash_offset)];		\
	(_found) = FALSE;					\
	while (hash_bucket != NULL) {				\
		if (hash_bucket == (_entry)) {			\
			(_found) = TRUE;			\
			break;					\
		}						\
		(_prev) = &hash_bucket->next;			\
		hash_bucket = hash_bucket->next;		\
	}							\
}


/*
 * Extract the next component out of a path name.
 */
#define FIND_COMPONENT(_cp,_len) {				\
	char	*_np;						\
								\
	while ((*(_cp) != '\0') && (*(_cp) == '/')) (_cp)++;	\
	(_len) = 0;						\
	_np = (_cp);						\
	while ((*_np != '\0') && (*_np != '/')) {		\
		_np++;						\
		(_len)++;					\
	}							\
}

void dir::init_class(usClass* class_obj)
{
#ifdef	GXXBUG_VIRTUAL2
#else	GXXBUG_VIRTUAL2
	usName::init_class(class_obj);
#endif	GXXBUG_VIRTUAL2
	vol_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(dir);
	SETUP_METHOD_WITH_ARGS(dir,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(dir,ns_resolve);
	SETUP_METHOD_WITH_ARGS(dir,ns_create);
	SETUP_METHOD_WITH_ARGS(dir,ns_create_anon);
	SETUP_METHOD_WITH_ARGS(dir,ns_create_transparent_symlink);
	SETUP_METHOD_WITH_ARGS(dir,ns_insert_entry);
	SETUP_METHOD_WITH_ARGS(dir,ns_insert_forwarding_entry);
	SETUP_METHOD_WITH_ARGS(dir,ns_remove_entry);
	SETUP_METHOD_WITH_ARGS(dir,ns_rename_entry);
	SETUP_METHOD_WITH_ARGS(dir,ns_list_entries);
	SETUP_METHOD_WITH_ARGS(dir,ns_list_types);
	SETUP_METHOD_WITH_ARGS(dir,ns_allocate_unique_name);
	END_SETUP_METHOD_WITH_ARGS;
}

dir::~dir()
{
	DESTRUCTOR_GUARD();
	if (entry_table)
		Free(_Local(entry_table));
}

dir::dir() : link_count(0), entry_count(0), anon_name_seed(0)
{
	mutex_init(&this->lock);
	condition_init(&reserved_sleep_cond);
	entry_table = 0;
}


/* replaces setup_dir */
dir::dir(ns_mgr_id_t mgr_id, access_table *acctab)
:
 vol_agency(mgr_id, acctab),
 link_count(0), entry_count(0), anon_name_seed(0)
{
	_setup_dir();
}

/* replaces setup_dir_as_root */
dir::dir(ns_mgr_id_t mgr_id, mach_error_t* ret)
:
 vol_agency(dir::new_mgr_id(mgr_id), new access_table(ret)),
 link_count(0), entry_count(0), anon_name_seed(0)
{
	/*
	 * Normal setup for the root directory.
	 */
	_setup_dir();
	/*
	 * Take one dummy link since there is no real parent.
	 */
	Local(link_count) = 1;
	mach_object_dereference(access_tab);
}


inline void dir::_setup_dir(void)
{
	entry_table = NewArray(dir_entry_t,HASHSIZE);
	bzero(entry_table, HASHSIZE * sizeof(dir_entry_t));

	mutex_init(&this->lock);
	condition_init(&reserved_sleep_cond);
}

/*
 * XXX Try to allocate a random mgr_id if none was specified.
 */
ns_mgr_id_t dir::new_mgr_id(ns_mgr_id_t mgr_id)
{

	if (mgr_id.v1 == 0  &&  mgr_id.v2 == 0) {
		time_value_t time_value;
		mach_error_t ret;
                if ((ret = mach_get_time(&time_value)) != ERR_SUCCESS) {
                        mach_error("mach_get_time", ret);
                }
                mgr_id.v1 = time_value.seconds;
                mgr_id.v2 = time_value.microseconds;
	}
	return mgr_id;
}

char* dir::remote_class_name() const
{
//	return "++dir_proxy";
	return "usName_proxy";
}

/*
 * Internal methods, used only by subclasses.
 */

mach_error_t dir::ns_reserve_entry(ns_name_t name, int *tag)
{
	int			hash_offset;
	int			namelen = strlen(name);
	dir_entry_t		entry;
	dir_entry_t		*prev_ptr;

	if (namelen >= NS_NAMELEN) {
		return(NS_NAME_TOO_LONG);
	}
	if (namelen == 0) {
		return(NS_INVALID_NAME);
	}

	mutex_lock(&_Local(lock));

	/*
	 * Check that the name is not already in use.
	 */
	NAME_HASH(name,namelen,hash_offset);
	SCAN_FOR_NAME(hash_offset,name,namelen,entry,prev_ptr);
	if (entry != NULL) {
		mutex_unlock(&Local(lock));
		return(NS_ENTRY_EXISTS);
	}

	/*
	 * Create the new entry.
	 */
	entry = New(struct dir_entry);
	strcpy(entry->name,name);
	entry->namelen = namelen;
	entry->type = NST_RESERVED;
	entry->obj_id = 0;
	entry->obj = NULL;
	entry->next = Local(entry_table)[hash_offset];
	Local(entry_table)[hash_offset] = entry;
	Local(entry_count)++;

	mutex_unlock(&Local(lock));
	*tag = (int) entry;

	return(ERR_SUCCESS);
}


mach_error_t dir::ns_install_entry(int tag, agency* obj, ns_type_t type)
{
	dir_entry_t		entry = (dir_entry_t) tag;
	struct ns_attr		attr;
	int			attrlen;
	mach_error_t		ret;

	mutex_lock(&Local(lock));

	if ((entry == NULL) || (entry->type != NST_RESERVED)) {
		mutex_unlock(&Local(lock));
		return(NS_ENTRY_NOT_RESERVED);
	}

	/*
	 * Warning: we do not actually take an explicit MachObjects
	 * reference for the object. However, the call to
	 * ns_register_tmplink() guarantees that the object will not go
	 * away before first calling ns_remove_tmplink() to take itself
	 * out of the directory, under protection of the lock.
	 */

	vol_agency* vobj;
	if ((vobj = vol_agency::castdown(obj)) == 0) {
		ret = _notdef();
	} else {
		ret = vobj->ns_register_tmplink(this,tag);
	}
	if (ret != ERR_SUCCESS) {
		mutex_unlock(&Local(lock));
		return(ret);
	}

	attrlen = sizeof(attr) / sizeof(int);
	ret = obj->ns_get_attributes(&attr,&attrlen);
	if (ret != ERR_SUCCESS) {
		mutex_unlock(&Local(lock));
		return(ret);
	}
	entry->type = type;
	entry->obj_id = attr.obj_id;
	entry->obj = obj;

	mutex_unlock(&Local(lock));

	condition_broadcast(&Local(reserved_sleep_cond));

	return(ERR_SUCCESS);
}

mach_error_t dir::ns_cancel_entry(int tag)
{
	dir_entry_t		entry = (dir_entry_t) tag;
	int			hash_offset;
	dir_entry_t		*prev_ptr;
	boolean_t		found;

	mutex_lock(&Local(lock));

	if ((entry == NULL) || (entry->type != NST_RESERVED)) {
		mutex_unlock(&Local(lock));
		return(NS_ENTRY_NOT_RESERVED);
	}

	if (entry->obj != NULL) {
		us_internal_error("ns_cancel_entry: entry->obj not NULL",
							US_OBJECT_BUSY);
	}

	NAME_HASH(entry->name,entry->namelen,hash_offset);
	SCAN_FOR_ENTRY(hash_offset,entry,prev_ptr,found);
	if (found) {
		*prev_ptr = entry->next;
		Local(entry_count)--;
	} else {
		us_internal_error("ns_cancel_entry: entry not found",
							US_OBJECT_NOT_FOUND);
	}

	mutex_unlock(&Local(lock));

	Free(entry);

	condition_broadcast(&Local(reserved_sleep_cond));

	return(ERR_SUCCESS);
}

mach_error_t
dir::ns_create_common(int tag, agency *new_agency, ns_type_t type, 
		      ns_prot_t prot, int protlen, ns_access_t access, 
		      agent** new_agent)
{
	mach_error_t	ret;
	std_cred*	credobj;

	*new_agent = NULL;

	ret = new_agency->ns_set_protection(prot,protlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	if (access != 0) {
		ret = agent::base_object()->ns_get_cred_obj(&credobj);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
		ret = new_agency->ns_create_agent(access,credobj,new_agent);
		mach_object_dereference(credobj);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
	}

	ret = this->ns_install_entry(tag,new_agency,type);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(*new_agent);
		*new_agent = NULL;
		return(ret);
	}

	return(ERR_SUCCESS);
}



/*
 * Internal method for lookups between instances of the directory class.
 * Not to be used by external clients.
 */

mach_error_t 
dir::ns_lookup_entry(ns_name_t name, int namelen, agency **newobj,
			      ns_type_t *newtype)
{
	int			hash_offset;
	dir_entry_t		entry;
	dir_entry_t		*prev_ptr;
	mach_error_t		ret;

#define	ABORT(_ret) {				\
	mutex_unlock(&Local(lock));		\
	*newobj = NULL;		\
	*newtype = NST_INVALID;			\
	return(_ret);				\
}

	if (namelen >= NS_NAMELEN) {
		*newobj = NULL;
		*newtype = NST_INVALID;
		return(NS_NAME_TOO_LONG);
	}

	NAME_HASH(name,namelen,hash_offset);

	mutex_lock(&Local(lock));

retry:

	SCAN_FOR_NAME(hash_offset,name,namelen,entry,prev_ptr);
	if (entry == NULL) {
		ABORT(NS_NOT_FOUND);
	}

	if (entry->type == NST_RESERVED) {
		ret = intr_cond_wait(&Local(reserved_sleep_cond),&Local(lock));
		if (ret != ERR_SUCCESS) {
			ABORT(EXCEPT_SOFTWARE);
		}
		goto retry;
	}

	if (entry->obj == NULL) {
		us_internal_error("ns_lookup_entry: entry->obj is NULL",
							US_OBJECT_NOT_FOUND);
		ABORT(US_INTERNAL_ERROR);
	}

	vol_agency* vobj;
	if ((vobj = vol_agency::castdown(entry->obj)) == 0) {
		ret = _notdef();
	} else {
		ret = vobj->ns_reference_tmplink();
	}
	if (ret == ERR_SUCCESS) {
		*newtype = entry->type;
		*newobj = entry->obj;
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}
	if (ret == US_OBJECT_DEAD) {
		/*
		 * Remove the entry from the directory, but
		 * do not deallocate the entry, because there
		 * is still a call to ns_remove_tmplink() pending.
		 */
		*prev_ptr = entry->next;
		Local(entry_count)--;
		entry->type = NST_INVALID;
		entry->obj = NULL;
		ABORT(NS_NOT_FOUND);
	}

	us_internal_error("ns_lookup_entry.ns_reference_tmplink",ret);
	ABORT(US_INTERNAL_ERROR);

#undef	ABORT
}

/*
 * Internal method for the management of links between directories
 * and their entries. Not to be used by external clients.
 */

mach_error_t dir::ns_register_tmplink(dir *parent, int tag)
{
	mutex_lock(&Local(lock));
	Local(link_count)++;
	mutex_unlock(&Local(lock));

	mach_object_reference(this);

	return(ERR_SUCCESS);
}


mach_error_t dir::ns_unregister_tmplink(int tag)
{
	mutex_lock(&Local(lock));

	if ((Local(link_count) == 1) && (Local(entry_count) > 0)) {
		mutex_unlock(&Local(lock));
		return(NS_DIR_NOT_EMPTY);
	}

	Local(link_count)--;
	mutex_unlock(&Local(lock));
	mach_object_dereference(this);
	return(ERR_SUCCESS);
}


mach_error_t dir::ns_reference_tmplink(void)
{
	mach_object_reference(this);
	return(ERR_SUCCESS);
}


mach_error_t dir::ns_remove_tmplink(int tag)
{
	dir_entry_t		entry = (dir_entry_t) tag;
	int			hash_offset;
	dir_entry_t		*prev_ptr;
	boolean_t		found;

	mutex_lock(&Local(lock));

	if (entry == NULL) {
		mutex_unlock(&Local(lock));
		us_internal_error("ns_remove_tmplink: entry is NULL",
							US_OBJECT_NOT_FOUND);
		return(US_INTERNAL_ERROR);
	}

	if (entry != NST_INVALID) {
		NAME_HASH(entry->name,entry->namelen,hash_offset);
		SCAN_FOR_ENTRY(hash_offset,entry,prev_ptr,found);
		if (found) {
			*prev_ptr = entry->next;
			Local(entry_count)--;
		} else {
			us_internal_error("ns_remove_tmplink: entry not found",
							US_OBJECT_NOT_FOUND);
		}
	}

	mutex_unlock(&Local(lock));
	Free(entry);
	return(ERR_SUCCESS);
}


mach_error_t dir::ns_register_stronglink(void)
{
	mutex_lock(&Local(lock));
	Local(link_count)++;
	mutex_unlock(&Local(lock));

	mach_object_reference(this);

	return(ERR_SUCCESS);
}


mach_error_t dir::ns_unregister_stronglink(void)
{
	mutex_lock(&Local(lock));

	if ((Local(link_count) == 1) && (Local(entry_count) > 0)) {
		mutex_unlock(&Local(lock));
		return(NS_DIR_NOT_EMPTY);
	}

	Local(link_count)--;

	mutex_unlock(&Local(lock));

	mach_object_dereference(this);

	return(ERR_SUCCESS);
}

/*
 * Server internal use only.  Must insure at higher level that entry_count
 * will not change. Danger here.
 */
int dir::dir_entry_count(void) {
	int	ret;

//	mutex_lock(&Local(lock));
	ret = Local(entry_count);
//	mutex_unlock(&Local(lock));
	return(ret);
}


/*
 * Exported methods.
 */

mach_error_t 
dir::ns_resolve(ns_path_t path, ns_mode_t mode, ns_access_t access, 
		usItem **newobj, ns_type_t *newtype, ns_path_t newpath, 
		int *usedlen, ns_action_t *action)
{
	mach_error_t	ret;
	char		*curpos;
	int		curlen;
	dir 		*curdir =0;
	agency		*newentry =0;
	std_cred	*credobj =0;

#define	ABORT(_ret) {				\
	*newobj = NULL;		\
	*newtype = NST_INVALID;			\
	mach_object_dereference(curdir);	\
	mach_object_dereference(credobj);	\
	return(_ret);				\
}

	newpath[0] = '\0';
	*usedlen = 0;
	*action = 0;

	curpos = path;
	FIND_COMPONENT(curpos,curlen);

	if (curlen == 0) {
		ret = agent::base_object()->ns_duplicate(access, newobj);
//		ret = ns_duplicate(Base,access,newobj);
		*newtype = NST_DIRECTORY;
		return(ret);
	}

	curdir = this;
	(void) ns_reference_tmplink();

	ret = agent::base_object()->ns_get_cred_obj(&credobj);
	if (ret != ERR_SUCCESS) {
		ABORT(ret);
	}

	for (;;) {
		/*
		 * Lookup the next entry.
		 */
		ret = curdir->ns_lookup_entry(curpos, curlen, 
						       &newentry, newtype);
		if (ret != ERR_SUCCESS) {
			ABORT(ret);
		}

		curpos += curlen;
		FIND_COMPONENT(curpos,curlen);

		if ((curlen == 0) || (*newtype != NST_DIRECTORY)) {
			break;
		}

		mach_object_dereference(curdir);
		if ((curdir = dir::castdown(newentry)) == 0) {
			ABORT(MACH_OBJECT_NO_SUCH_OPERATION);
		}

		/*
		 * Check access for the next iteration.
		 *
		 * XXX Funny typecast needed to get around a compiler bug.
		 */
		ret = ((agency*)curdir)->ns_check_access(NSR_LOOKUP,credobj);
		if (ret != ERR_SUCCESS) {
			ABORT(ret);
		}
	}

	/*
	 * At this point, newentry designates the last object found,
	 * and curdir designates its parent directory.
	 * We are holding one reference for each.
	 */

	*usedlen = curpos - path;


	if (
		((*newtype == NST_SYMLINK) && 
			((curlen > 0) || (mode & NSF_FOLLOW))) ||
		((*newtype == NST_TRANSPARENT_SYMLINK) && 
			((curlen > 0) || (mode & NSF_TFOLLOW))) ||
		((*newtype == NST_MOUNTPT) && 
			((curlen > 0) || (mode & NSF_MOUNT))) ||
		((*newtype == NST_FORWARD) && 
			((curlen > 0) || (mode & NSF_FOLLOW)))
	) {
		/*
		 *
		 * Apply the forwarding entry;
		 *
		 * By default, the object returned with a forwarding entry
		 * can only be a directory.
		 */

		if (*newtype == NST_TRANSPARENT_SYMLINK) {
			*action |= NSA_TFORWARD;
		} else {
			*action |= NSA_FORWARD;
		}

		usName* temp = usName::castdown(newentry);/* XXX castdown */
		if (temp == 0) {
			DEBUG0(TRUE, (0, "%s::ns_resolve()\n", class_name()));
			ABORT(MACH_OBJECT_NO_SUCH_OPERATION);
		}
		ret = temp->ns_read_forwarding_entry(newobj, newpath);
		mach_object_dereference(newentry);

		if (ret != ERR_SUCCESS) {
			ABORT(ret);
		}
		if (*newobj == NULL) {
			if (newpath[0] != '/') {
				/*
				 * Relative symlink.
				 * Return the containing
				 * directory.
				 */
				agent* agent_obj;
				ret = curdir->ns_create_agent(NSR_LOOKUP,
							      credobj,
							      &agent_obj);
				*newobj = agent_obj;
				if (ret != ERR_SUCCESS) {
					ABORT(ret);
				}
				*newtype = NST_DIRECTORY;
			} else {
				*newtype = NST_INVALID;
			}
		} else {
			/*
			 * The forwarding entry contains
			 * an external object.
			 * Just return it unmodified.
			 */
			*newtype = NST_DIRECTORY;	/* XXX */
			*action |= NSA_AUTHENTICATE;
		}
	} else {
		/*
		 * Return the object just found.
		 */
		if (curlen > 0) {
			mach_object_dereference(newentry);
			ABORT(NS_NOT_DIRECTORY);
		}

		if (mode & NSF_ACCESS) {
			ret = newentry->ns_check_access(access,credobj);
			mach_object_dereference(newentry);
			ABORT(ret);
		}

		/*
		 * Create a new agent with our credentials
		 */
		agent* agent_obj;
		ret = newentry->ns_create_agent(access,credobj,&agent_obj);
		*newobj = agent_obj;
		mach_object_dereference(newentry);
		if (ret != ERR_SUCCESS) {
			ABORT(ret);
		}
	}

	mach_object_dereference(curdir);
	mach_object_dereference(credobj);

	return(ret);

#undef	ABORT
}


mach_error_t 
dir::ns_create(ns_name_t name, ns_type_t type, ns_prot_t prot, int protlen, 
	       ns_access_t access, usItem **newobj)
{
	dir		*newdir;
	int		tag;
	mach_error_t	ret;

	switch(type) {

		case NST_DIRECTORY:
		{
			ret = this->ns_reserve_entry(name,&tag);
			if (ret != ERR_SUCCESS) {
				return(ret);
			}

			newdir = new dir(mgr_id, access_tab);
/*
			ret = newdir->_setup_dir(mgr_id, access_tab);
			if (ret != ERR_SUCCESS) {
				(void) this->ns_cancel_entry(tag);
				mach_object_dereference(newdir);
				return(ret);
			}
*/
			break;
		}

		default:	
			/*
			 * Figure out from the type server what type
			 * this is and if we support it...
			 */
			return(NS_UNKNOWN_ENTRY_TYPE);
	}
	agent* agent_obj;
	ret = this->ns_create_common(tag, newdir, type, prot, protlen, access,
				     &agent_obj);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
	}
	*newobj = agent_obj;
	mach_object_dereference(newdir);
	return(ret);
}


mach_error_t 
dir::ns_create_anon(ns_type_t type, ns_prot_t prot, int protlen, 
		    ns_access_t access, ns_name_t name, usItem **newobj)
{
	mach_error_t		ret;

	ret = this->ns_allocate_unique_name(name);
	if (ret != ERR_SUCCESS) {
		*newobj = NULL;
		return(ret);
	}
	ret = this->ns_create(name,type,prot,protlen,access,newobj);

	return(ret);
}


mach_error_t 
dir::ns_create_transparent_symlink(ns_name_t name, ns_prot_t prot,
				   int protlen, char *path)
{
	tsymlink		*newobj;
	mach_error_t		ret;
	int			tag;

	ret = this->ns_reserve_entry(name,&tag);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

//	new_object(newobj,tsymlink);
//	ret = setup_tsymlink(newobj,PublicLocal(mgr_id),
//					PublicLocal(access_table),path);
	newobj = new tsymlink(mgr_id, access_tab, path, &ret);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
		mach_object_dereference(newobj);
		return(ret);
	}

	ret = newobj->ns_set_protection(prot,protlen);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
		mach_object_dereference(newobj);
		return(ret);
	}

	ret = this->ns_install_entry(tag,newobj,NST_TRANSPARENT_SYMLINK);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
	}

	mach_object_dereference(newobj);
	return(ret);
}


mach_error_t dir::ns_insert_entry(char *name, usItem* target)
{
	mach_error_t		ret;
	agency*			agency_ptr;
	struct ns_attr		attr;
	int			attrlen;
	int			tag;

	attrlen = sizeof(attr) / sizeof(int);
	ret = target->ns_get_attributes(&attr,&attrlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	if ((attr.version != NS_ATTR_VERSION) ||
		((attr.valid_fields & (NS_ATTR_TYPE | NS_ATTR_ID)) !=
		(NS_ATTR_TYPE | NS_ATTR_ID))) {
		return(US_UNKNOWN_ERROR);
	}

	if (bcmp(&attr.mgr_id,&mgr_id, sizeof(struct ns_mgr_id))) {
		return(NS_CANNOT_INSERT);
	}

	ret = this->ns_reserve_entry(name,&tag);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	usItem *agency_item;
	ret = target->ns_get_item_ptr(&agency_item);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
		return(ret);
	}

	agency_ptr = agency::castdown(agency_item);
	if (NULL == agency_ptr) {
		mach_object_dereference(agency_item);
		(void) this->ns_cancel_entry(tag);
		return(ret);
	}

	ret = this->ns_install_entry(tag, agency_ptr, attr.type);
	mach_object_dereference(agency_ptr);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
	}
	return(ret);
}


mach_error_t 
dir::ns_insert_forwarding_entry(ns_name_t name, ns_prot_t prot, int protlen, 
				usItem *obj, char* path)
{
	agency		*newobj;
	ns_type_t		newtype;
	mach_error_t		ret;
	int			tag;

	ret = this->ns_reserve_entry(name,&tag);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	if (obj == NULL) {
//		new_object(newobj,symlink);
		newobj = new symlink (mgr_id, access_tab, path, &ret);
		newtype = NST_SYMLINK;
	} else {
//		new_object(newobj,mountpt);
		newobj = new mountpt (mgr_id, access_tab, obj, &ret);
		newtype = NST_MOUNTPT;
	}
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
		mach_object_dereference(newobj);
		return(ret);
	}

	ret = newobj->ns_set_protection(prot,protlen);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
		mach_object_dereference(newobj);
		return(ret);
	}

	ret = this->ns_install_entry(tag, newobj, newtype);
	if (ret != ERR_SUCCESS) {
		(void) this->ns_cancel_entry(tag);
	}

	mach_object_dereference(newobj);
	return(ret);
}


mach_error_t dir::ns_remove_entry(ns_name_t name)
{
	int			hash_offset;
	int			namelen = strlen(name);
	dir_entry_t		entry;
	dir_entry_t		*prev_ptr;
	mach_error_t		ret;

	if (namelen >= NS_NAMELEN) {
		return(NS_NAME_TOO_LONG);
	}

	NAME_HASH(name,namelen,hash_offset);

	mutex_lock(&Local(lock));

retry:

	SCAN_FOR_NAME(hash_offset,name,namelen,entry,prev_ptr);
	if (entry == NULL) {
		mutex_unlock(&Local(lock));
		return(NS_NOT_FOUND);
	}

	if (entry->type == NST_RESERVED) {
		ret = intr_cond_wait(&Local(reserved_sleep_cond),&Local(lock));
		if (ret != ERR_SUCCESS) {
			mutex_unlock(&Local(lock));
			return(EXCEPT_SOFTWARE);
		}
		goto retry;
	}

	if (entry->obj != NULL) {
		vol_agency* vobj;
		if ((vobj = vol_agency::castdown(entry->obj)) == 0) {
			ret = _notdef();
		} else {
			ret = vobj->ns_unregister_tmplink((int)entry);
		}
	} else {
		us_internal_error("ns_remove_entry: entry->obj is NULL",
							US_OBJECT_NOT_FOUND);
		ret = ERR_SUCCESS;
	}

	if (ret == ERR_SUCCESS) {
		*prev_ptr = entry->next;
		Local(entry_count)--;
		Free(entry);
	} else if (ret == US_OBJECT_DEAD) {
		/*
		 * Remove the entry from the directory, but
		 * do not deallocate the entry, because there
		 * is still a call to ns_remove_tmplink() pending.
		 */
		*prev_ptr = entry->next;
		Local(entry_count)--;
		entry->type = NST_INVALID;
		entry->obj = NULL;
		ret = NS_NOT_FOUND;
	}

	mutex_unlock(&Local(lock));

	return(ret);
}


mach_error_t
dir::ns_rename_entry(ns_name_t name, usItem *newdir, ns_name_t newname)
{
	return(US_NOT_IMPLEMENTED);
}


mach_error_t
dir::ns_list_entries(ns_mode_t mode, ns_name_t** names,
		     unsigned int *names_count, ns_entry_t *entries, 
		     unsigned int *entries_count)
{
	mach_error_t		ret;
	int			hash_index;
	int			entry_index;
	dir_entry_t		entry;


#define	ABORT(_ret) {						\
	if (*names)						\
		(void)vm_deallocate(mach_task_self(),		\
			(vm_address_t)names,			\
			*names_count*sizeof(**names));		\
	if (*entries)						\
		(void)vm_deallocate(mach_task_self(),		\
			(vm_address_t)entries,			\
			*entries_count*sizeof(*entries[0]));	\
	*names = NULL;						\
	*entries = NULL;					\
	*names_count = 0;					\
	*entries_count = 0;					\
	return(_ret);						\
}

	mutex_lock(&Local(lock));

	*entries_count = Local(entry_count);
	*names_count = Local(entry_count);
	*names = NULL;
	*entries = NULL;

	ret = vm_allocate(mach_task_self(), (vm_address_t*)names,
				*names_count * sizeof(**names),TRUE);
	if (ret != KERN_SUCCESS) {
		mutex_unlock(&Local(lock));
		ABORT(ret);
	}

	ret = vm_allocate(mach_task_self(), (vm_address_t*)entries,
				*entries_count * sizeof((*entries)[0]),TRUE);
	if (ret != KERN_SUCCESS) {
		mutex_unlock(&Local(lock));
		ABORT(ret);
	}

	entry_index = 0;

	for (hash_index = 0; hash_index < HASHSIZE; hash_index++) {
		entry = Local(entry_table)[hash_index];
		while (entry != NULL) {
			if (entry_index >= *names_count) {
				mutex_unlock(&Local(lock));
				ABORT(NS_BAD_DIRECTORY)
			}
			bcopy(entry->name,((*names)[entry_index]),NS_NAMELEN);
			(*entries)[entry_index].type = entry->type;
			(*entries)[entry_index].obj_id = entry->obj_id;
			entry_index++;
			entry = entry->next;
		}
	}

	mutex_unlock(&Local(lock));
	return(ERR_SUCCESS);

#undef	ABORT
}


mach_error_t dir::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

//	ret = invoke_super(Super,mach_method_id(ns_get_attributes),
//			   attr,attrlen);
	ret = vol_agency::ns_get_attributes(attr, attrlen);
	attr->type = NST_DIRECTORY;

	mutex_lock(&Local(lock));
	attr->valid_fields |= NS_ATTR_NLINKS;
	attr->nlinks = Local(link_count);
	mutex_unlock(&Local(lock));

	return(ret);
}

mach_error_t dir::ns_allocate_unique_name(ns_name_t name)
{
	mutex_lock(&_Local(lock));
	sprintf(name,"+Anon-%d+", _Local(anon_name_seed));
	_Local(anon_name_seed)++;
	mutex_unlock(&_Local(lock));

	return(ERR_SUCCESS);
}


mach_error_t dir::ns_list_types(ns_type_t **types, int *count)
{
	mach_error_t		ret;
	vm_address_t		data;

	*count = 4;

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
	((ns_type_t *)data)[0] = NST_DIRECTORY;
	((ns_type_t *)data)[1] = NST_SYMLINK;
	((ns_type_t *)data)[2] = NST_FILE;
	((ns_type_t *)data)[3] = NST_TRANSPARENT_SYMLINK;

	*types = (ns_type_t *)data;

	return(NS_SUCCESS);
}


/*
 * Iterator for looping thru directories
 */
dir_iterator::dir_iterator(
	dir *iter_dir_a)
	:
	hash_index(0),
	entry(NULL),
	iter_dir(iter_dir_a)
{
	mach_object_reference(iter_dir);
}

dir_iterator::~dir_iterator(void) {
	if (NULL != Local(entry)) dir_cancel_iteration();
	mach_object_dereference(iter_dir);
}

/*
 * dir_iterate:
 *	Iterate thru the entries in a directory.
 *
 *	NOTE: THE FIRST ITERATION CAUSES THE DIRECTORY TO BE LOCKED.
 *		IT MUST BE UNLOCKED BY EITHER ITERATING THRU ALL
 *		THE ENTRIES IN THE DIR OR CANCELING THE ITERATION.
 */
mach_error_t dir_iterator::dir_iterate(dir_entry_t *rtn_entry) {
	mach_error_t	ret;

	/* First time ? */
	if (NULL == Local(entry)) {
		mutex_lock(&(iter_dir->lock));
		Local(hash_index) = 0;
		Local(entry) = NULL;
	}

	for (0; Local(hash_index) < HASHSIZE; Local(hash_index)++) {
		if (Local(entry) == NULL) {
			/* find non-empty hash chain */
			Local(entry) = (iter_dir->entry_table)[Local(hash_index)];
		}
		else {
			/* Follow down the current hash chain */
			Local(entry) = Local(entry)->next;
		}

		if (Local(entry) != NULL) {
			/* We found a new one, return it */
			*rtn_entry = Local(entry);
			return(ERR_SUCCESS);
		}
	}

	/* End of dir */
	Local(hash_index) = 0;
	Local(entry) = NULL;
	*rtn_entry = NULL;
	mutex_unlock(&(iter_dir->lock));
	return(ERR_SUCCESS);
}

/*
 * dir_cancel_iteration:
 *	Stop an iteration thru the entries in a directory prematurely
 *	Doing necessary cleanup an lock releasing.
 *
 *	NOTE: THIS ROUTINE MUST BE CALLED TO STOP ITTERATING BEFORE
 *		GOING THRU ALL THE ENTRIES OR THE DIRECTORY WILL BE
 *		LEFT LOCKED!
 */
mach_error_t dir_iterator::dir_cancel_iteration(void) {
	Local(hash_index) = 0;
	Local(entry) = NULL;
	mutex_unlock(&(iter_dir->lock));
	return(ERR_SUCCESS);
}
