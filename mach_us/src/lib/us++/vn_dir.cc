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
 *
 * Purpose: Directory agency for vnode-based servers.
 *
 * HISTORY:
 * $Log:	vn_dir.cc,v $
 * Revision 2.6  94/07/23  11:59:29  mrt
 * 	Changed ns_create to allow for "unix-style" file creation with
 * 	protections more restrictive than the returning access mode of
 * 	the agency (ie: creat(foobar, O_RDWR, O_RDONLY)).
 * 	[94/07/22  16:47:54  grm]
 * 
 * 	Added the mount option to the new fs_access.
 * 	[94/06/29  13:53:00  grm]
 * 
 * Revision 2.5  94/07/07  17:25:47  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/06/29  14:56:51  mrt
 * 	Added the mount option to the new fs_access.
 * 	[94/06/29  13:53:00  grm]
 * 
 * Revision 2.3  93/01/20  17:38:33  jms
 * 	Bzero a new fs_attr before filling it in ns_create.
 * 	[93/01/18  17:09:26  jms]
 * 
 * Revision 2.2  92/07/05  23:31:50  dpj
 * 	Use ns_get_item_ptr() instead of ns_get_agency_ptr().
 * 	[92/07/05  18:59:38  dpj]
 * 
 * 	First working version.
 * 	[92/06/24  17:26:11  dpj]
 * 
 * Revision 2.1  91/09/27  14:03:20  pjg
 * Created.
 * 
 * Revision 2.4  91/07/01  14:12:54  jms
 * 	Convert to using vn_reference.
 * 	[91/06/07  10:41:52  roy]
 * 
 * 	Changed aot_change_state call to aot_set_state.
 * 	[91/06/06  17:02:52  roy]
 * 
 * 	Modified ns_insert_entry and ns_remove_entry to mark 
 * 	file agencies as permanent and temporary, respectively.
 * 	[91/06/05  14:01:00  roy]
 * 
 * Revision 2.3  91/05/05  19:28:08  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:33  dpj]
 * 
 * 	Support transparent symlinks.
 * 	[91/04/28  10:24:27  dpj]
 * 
 * Revision 2.2  90/12/21  13:54:20  jms
 * 	Added call to fs_methods_setup() in setup_fs_dir_as_root().
 * 	[90/12/20  12:06:28  roy]
 * 
 * 	Initial revision.
 * 	(Equivalent functionality to old fs_dir object.)
 * 	[90/12/15  15:12:57  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:20:49  jms]
 * 
 * Revision 2.1  90/12/15  15:12:32  roy
 * Created.
 * 
 * 
 */

#include	<vn_dir_ifc.h>

#include	<vn_mgr_ifc.h>
#include	<vn_file_ifc.h>
#include	<vn_symlink_ifc.h>
#include	<vn_tsymlink_ifc.h>

extern "C" {
#include	<mach_init.h>	/* round_page() */
#include	<mach/machine/vm_param.h>
#include	<mach/time_value.h>     /* mach_get_time() */
#include	<us_error.h>
#include	<ns_error.h>
#include	<ns_types.h>
#include	<fs_types.h>
#include	<sys/file.h>
}


DEFINE_CLASS_MI(vn_dir);
DEFINE_CASTDOWN2(vn_dir,usName,vn_agency);

void vn_dir::init_class(usClass* class_obj)
{
	usName::init_class(class_obj);
	vn_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(vn_dir);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_resolve);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_create);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_create_transparent_symlink);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_insert_entry);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_insert_forwarding_entry);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_remove_entry);
//	SETUP_METHOD_WITH_ARGS(vn_dir,ns_rename_entry);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_list_entries);
	SETUP_METHOD_WITH_ARGS(vn_dir,ns_list_types);
	END_SETUP_METHOD_WITH_ARGS;
}


char* vn_dir::remote_class_name() const
{
	return "usName_proxy";
}


/*
 * Set-up a reply area for list_entries.
 */
mach_error_t vn_dir::fsint_setup_list(
	int			old_count,
	vm_address_t		old_names,
	vm_address_t		old_entries,
	int			new_count,
	vm_address_t		*new_names,	/* out */
	vm_address_t		*new_entries)	/* out */
{
	mach_error_t		ret;
	vm_address_t		addr;
	vm_size_t		old_names_size;
	vm_size_t		old_entries_size;
	vm_size_t		new_names_size;
	vm_size_t		new_entries_size;

	*new_names = NULL;
	*new_entries = NULL;

	old_names_size = round_page(old_count * sizeof(ns_name_t));
	old_entries_size = round_page(old_count * sizeof(struct ns_entry));
	new_names_size = round_page(new_count * sizeof(ns_name_t));
	new_entries_size = round_page(new_count * sizeof(struct ns_entry));

	/*
	 * Get the new space.
	 */
	addr = NULL;
	ret = vm_allocate(mach_task_self(),&addr,new_names_size,TRUE);
	if (ret == KERN_SUCCESS) {
		*new_names = addr;
	} else {
		mach_error("fsint_setup_list.vm_allocate(names)",ret);
		goto finish;
	}

	addr = NULL;
	ret = vm_allocate(mach_task_self(),&addr,new_entries_size,TRUE);
	if (ret == KERN_SUCCESS) {
		*new_entries = addr;
	} else {
		mach_error("fsint_setup_list.vm_allocate(entries)",ret);
		(void)vm_deallocate(mach_task_self(),
						*new_names,new_names_size);
		goto finish;
	}

	if (old_count) {
		/*
		 * Copy the old area onto the new.
		 */
		bcopy((char *)old_names,(char *)(*new_names),old_names_size);
		bcopy((char *)old_entries,(char *)(*new_entries),old_entries_size);
	}

finish:
	if (old_count) {
		/*
		 * Get rid of the old area.
		 */
		(void)vm_deallocate(mach_task_self(),old_names,old_names_size);
		(void)vm_deallocate(mach_task_self(),old_entries,old_entries_size);
	}

	return(ret);
}


/*
 * Clean-up the reply area for list entries, generated by fsint_setup_list().
 */
void vn_dir::fsint_cleanup_list(
	int			used_count,
	int			total_count,
	vm_address_t		names,
	vm_address_t		entries)
{
	vm_size_t		names_size;
	vm_size_t		names_used;
	vm_size_t		entries_size;
	vm_size_t		entries_used;

	names_size = round_page(total_count * sizeof(ns_name_t));
	entries_size = round_page(total_count * sizeof(struct ns_entry));
	names_used = round_page(used_count * sizeof(ns_name_t));
	entries_used = round_page(used_count * sizeof(struct ns_entry));

	(void)vm_deallocate(mach_task_self(),names + names_used,
					names_size - names_used);
	(void)vm_deallocate(mach_task_self(),entries + entries_used,
					entries_size - entries_used);

	return;
}


/*
 * fs_fsid_export(): find the agency for the specified fsid, and return
 * a new agent with the required access and credentials. 
 */
mach_error_t vn_dir::fs_fsid_export(
   	fs_id_t			fsid,
	ns_type_t		type,
 	ns_access_t		access,
	fs_cred*		cred_obj,
	usItem**		newobj)	/* out */ 
{
	mach_error_t		ret;
 	int			tag;
	agent*			agent_obj;
	vn_agency*		agency_obj;

	/* 
	 * Lookup the relevant agency.  The aot makes the following 
	 * guarantees.
	 * if found (agency != NULL): 
	 * 	- a vn_reference is obtained for the agency
	 * if not found (agency == NULL):
	 *	- an entry is reserved in the aot and returned locked
	 *	- all other threads looking up the same fsid will block
	 *	  on the locked entry.
	 * Requirements of this module:
	 *   If the entry was not found, then this module must subsequently
	 *   call either aot_install() or aot_cancel() to unlock the entry
	 *   (which will wakeup any waiters).
	 */   


 	ret = vn_mgr_obj->aot_lookup_or_reserve((aot_key_t)fsid,
						&agency_obj, &tag);
 	if (ret != ERR_SUCCESS) {
 		*newobj = NULL;
 		return(ret);
 	}

	/*
 	 * Create a new agency if needed.
  	 */
 	if (agency_obj == NULL) {
 		switch(type) {
 		case NST_DIRECTORY:
 			agency_obj = new vn_dir(fsid,fs_access_obj,
						mgr_id,access_tab,vn_mgr_obj);
			break;

		case NST_FILE:
 			agency_obj = new vn_file(fsid,fs_access_obj,
						mgr_id,access_tab,vn_mgr_obj);
			break;

		case NST_SYMLINK:
 			agency_obj = new vn_symlink(fsid,fs_access_obj,
						mgr_id,access_tab,vn_mgr_obj);
			break;

		case NST_TRANSPARENT_SYMLINK:
 			agency_obj = new vn_tsymlink(fsid,fs_access_obj,
						mgr_id,access_tab,vn_mgr_obj);
			break;

		default:
			ret = US_INVALID_ARGS;
			break;
		}
		if (ret != ERR_SUCCESS) {
			*newobj = NULL;
			mach_object_dereference(agency_obj);
			vn_mgr_obj->aot_cancel(tag);
			return(ret);
		}

		/*
		 * Obtain a vn_reference for the agency to ensure that
		 * it doesn't go away before we create the first agent.
		 */
		agency_obj->vn_reference();

		/*
		 * Install the new agency in the entry previously reserved
		 * in the aot.  After this call the aot will "own" the (sole) 
		 * mach_object_reference to the object.  
		 */
		if (type == NST_FILE)
			vn_mgr_obj->aot_install(tag, agency_obj,
						AOT_STATE_FILE_ACTIVE);
		else
			/* directories and symlinks */
			vn_mgr_obj->aot_install(tag, agency_obj,
						AOT_STATE_DIR_ACTIVE);
	}

	/*
	 * Create the desired agent.  This will result in a new vn_reference
	 * being obtained for the agency.  We are guaranteed that the
	 * agency will not be deactivated before then because we still
	 * hold a vn_reference.
	 */
	ret = agency_obj->fs_create_agent(access,cred_obj,&agent_obj);
	if (ret != ERR_SUCCESS) {
		*newobj = NULL;
		agency_obj->vn_dereference();
		return(ret);
	}

	agency_obj->vn_dereference();  /* Release the vn_reference obtained 
					* earlier.
					*/
	*newobj = (usItem*)agent_obj;
	return(ERR_SUCCESS);
}


vn_dir::vn_dir()
{
}


vn_dir::vn_dir(
	fs_id_t			fsid,
	fs_access*		_fs_access,
	ns_mgr_id_t		mgr_id,
	access_table*		_access_table,
	vn_mgr*			_vn_mgr)
:
	vn_agency(fsid,_fs_access,mgr_id,_access_table,_vn_mgr)
{
}


/* replaces setup_vn_dir_as_root */
vn_dir::vn_dir(
	fs_id_t			fsid,
	ns_mgr_id_t		mgr_id,
        int			dev_mode,
	mach_error_t*		ret)
:
	vn_agency(fsid, new fs_access(dev_mode) , mgr_id, 
		  new access_table(ret), new vn_mgr)
{
	/*
	 * Enter this object into the active object table, but first obtain
	 * a vn_reference for it (to make sure it doesn't go away).
	 */
	this->vn_reference();
	(void) vn_mgr_obj->aot_enter((aot_key_t)fsid,
						this, AOT_STATE_DIR_ACTIVE);
}


/*
 * ns_resolve(): One step in the resolving loop.
 *
 * If the desired object is found, it is returned in newobj, with the
 * access specified.
 *
 * If a forwarding entry is encountered, its contents
 * are returned in newobj and/or newpath, and usedlen specifies how many
 * characters were used in path before finding this forwarding entry.
 * Note that contrary to the old interface, newpath is simply the contents
 * of the symlink, not the concatenation of the symlink with the remaining
 * path name.
 *
 * action specifies what to do next.
 *
 * XXX XXX Should specify the appropriate user-side class for newobj.
 */

mach_error_t vn_dir::ns_resolve(
	ns_path_t		path,
	ns_mode_t		mode,
	ns_access_t		access,
	usItem**		newobj,		/* out */
	ns_type_t*		newtype,	/* out */
	ns_path_t		newpath,	/* out */
	int*			usedlen,	/* out */
	ns_action_t*		action)		/* out */
{
	mach_error_t		ret;
	char			*curpos;
	int			curlen;
	fs_id_t			cur_id;
	fs_id_t			new_id;
	struct fs_attr		fs_attr;
	char			curname[NS_NAMELEN];
	std_cred*		ns_cred_obj;
	fs_cred*		cred_obj;
	struct fs_cred_data*	fs_cred_ptr;

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

#define	ABORT(_ret) {						\
	*newobj = NULL;						\
	*newtype = NST_INVALID;					\
	newpath[0] = '\0';					\
	*usedlen = curpos - path;				\
	*action = 0;						\
	(void) fss_release(cur_id);				\
	mach_object_dereference(cred_obj);			\
	return(_ret);						\
}

	agent::base_object()->ns_get_cred_obj(&ns_cred_obj);
	cred_obj = fs_cred::castdown(ns_cred_obj);
	(void) cred_obj->fs_get_cred_ptr(&fs_cred_ptr);

	curpos = path;
	FIND_COMPONENT(curpos,curlen);
	cur_id = fsid;
	(void) fss_reference(cur_id);

	if (curlen == 0) {
		ret = agent::base_object()->ns_duplicate(access,newobj);
		*newtype = NST_DIRECTORY;
		newpath[0] = '\0';
		*usedlen = 0;
		*action = 0;
		(void) fss_release(cur_id);
		mach_object_dereference(cred_obj);
		return(ret);
	}

	for (;;) {
		if (curlen >= NS_NAMELEN) {
			ABORT(NS_NAME_TOO_LONG);
		}

		bcopy(curpos,curname,curlen);
		curname[curlen] = '\0';

		ret = fss_lookup(cur_id,curname,&new_id,&fs_attr,fs_cred_ptr);
		if (ret != NS_SUCCESS) {
			ABORT(ret);
		}

		curpos += curlen;
		FIND_COMPONENT(curpos,curlen);

		if ((fs_attr.va_type != VDIR) || (curlen == 0)) {
			/*
			 * We have found something. End of the loop.
			 */
			break;
		}

		(void) fss_release(cur_id);
		cur_id = new_id;

		ret = fs_access_obj->fs_check_access_from_data(
					&fs_attr,NSR_LOOKUP,fs_cred_ptr);
		if (ret != NS_SUCCESS) {
			ABORT(ret);
		}
	}

	/*
	 * At this point, new_id designates the last object found,
	 * and cur_id designates its parent directory.
	 * We are holding one fs_reference for each.
	 */

	*newtype = fs_convert_type_fs2ns(fs_attr.va_type);

	/*
	 * Special treatment for transparent symlinks:
	 * we must read the link and look at its contents to know
	 * if it is a regular or a transparent symlink.
	 */
	if (*newtype == NST_SYMLINK) {
		DECLARE_UIO(uio);

		SETUP_UIO(uio,newpath,sizeof(ns_path_t),0);
		ret = fss_readlink(new_id,&uio,fs_cred_ptr);
		if (ret != NS_SUCCESS) {
			(void) fss_release(new_id);
			ABORT(ret);
		}
		newpath[sizeof(ns_path_t) - uio.uio_resid] = '\0';

		if (! bcmp(newpath," TRANSPARENT ",13)) {
			int			len = strlen(newpath);

			bcopy(&newpath[13],newpath,len - 13 + 1);
			*newtype = NST_TRANSPARENT_SYMLINK;
		}
	}

	if (((*newtype == NST_SYMLINK) &&
			((curlen > 0) || (mode & NSF_FOLLOW))) ||
		((*newtype == NST_TRANSPARENT_SYMLINK) &&
			((curlen > 0) || (mode & NSF_TFOLLOW)))) {
		/*
		 * Follow the symlink.
		 */

		if (*newtype == NST_TRANSPARENT_SYMLINK) {
			*action = NSA_TFORWARD;
		} else {
			*action = NSA_FORWARD;
		}

		ret = fs_access_obj->fs_check_access_from_data(
					&fs_attr,NSR_READ,fs_cred_ptr);
		if (ret != NS_SUCCESS) {
			(void) fss_release(new_id);
			ABORT(ret);
		}

		/*
		 * The contents of the symlink are already in newpath.
		 */

		(void) fss_release(new_id);
	
		/*
		 * If the new path is relative, return a handle
		 * for the current directory.
		 */
		if (newpath[0] != '/') {
			*newtype = NST_DIRECTORY;

			ret = fs_fsid_export(cur_id,*newtype,
						NSR_LOOKUP,cred_obj,newobj);
			if (ret != NS_SUCCESS) {
				ABORT(ret);
			}

			*usedlen = curpos - path;
			(void) fss_release(cur_id);
		} else {
			*newtype = NST_INVALID;
			*newobj = NULL;
			*usedlen = curpos - path;
			(void) fss_release(cur_id);
		}
	} else {
		/*
		 * Return the desired object.
		 */

		(void) fss_release(cur_id);
		cur_id = new_id;

		if (curlen > 0) {
			ABORT(NS_NOT_DIRECTORY);
		}

		if (mode & NSF_ACCESS) {
			ret = fs_access_obj->fs_check_access_from_data(
						&fs_attr,access,fs_cred_ptr);
			ABORT(ret);
		}

		ret = fs_fsid_export(cur_id,*newtype,
					access,cred_obj,newobj);
		if (ret != NS_SUCCESS) {
			ABORT(ret);
		}

		newpath[0] = '\0';
		*usedlen = curpos - path;
		*action = 0;
		(void) fss_release(cur_id);
	}

	mach_object_dereference(cred_obj);
	return(NS_SUCCESS);

#undef	FIND_COMPONENT
#undef	ABORT
}


/*
 * Object creation: the object is created at the same time that it is
 * inserted in the containing directory.
 *
 * The standard call creates an empty object of the specified type, and
 * returns a handle (new_agent) for this object, with the desired access.
 *
 * Additional calls are available to create specific objects in some
 * servers: ns_create_directory, ns_create_anon, etc.
 */
mach_error_t vn_dir::ns_create(
	ns_name_t		name,
	ns_type_t		type,
	ns_prot_t		prot,
	int			protlen,
	ns_access_t		access,
	usItem**		new_agent)		/* out */
{
	mach_error_t		ret;
	unsigned int		namelen = strlen(name);
	struct fs_attr		fs_attr;
	fs_id_t			newid;
	std_cred*		ns_cred_obj;
	fs_cred*		cred_obj;
	struct fs_cred_data*	fs_cred_ptr;
	usItem*			tmp_agent;

	*new_agent = NULL;

	if (namelen >= NS_NAMELEN) {
		return(NS_NAME_TOO_LONG);
	}
	if (namelen == 0) {
		return(NS_INVALID_NAME);
	}

	agent::base_object()->ns_get_cred_obj(&ns_cred_obj);
	cred_obj = fs_cred::castdown(ns_cred_obj);
	(void) cred_obj->fs_get_cred_ptr(&fs_cred_ptr);

	/*
	 * XXX Check that the current credentials match the owner in
	 * the new protection ?
	 */

	bzero(&fs_attr, (sizeof(fs_attr)));
	if (type == NST_DIRECTORY) {
		ret = fs_access_obj->fs_convert_prot_ns2fs(prot,protlen,
							   &fs_attr);
		if (ret != ERR_SUCCESS) {
			mach_object_dereference(cred_obj);
			return(ret);
		}

		ret = fss_mkdir(fsid,name,&fs_attr,&newid,fs_cred_ptr);

		if (ret != ERR_SUCCESS) {
			mach_object_dereference(cred_obj);
			return(ret);
		}

		ret = fs_fsid_export(newid,type,access,cred_obj,&tmp_agent);

		(void) fss_release(newid);
		mach_object_dereference(cred_obj);

		if (ret != ERR_SUCCESS) {
			mach_object_dereference(tmp_agent);
			return (ret);
		}

		*new_agent = tmp_agent;
		return(ret);

	}else if (type == NST_FILE) {
		int		tmp_prot_data[DEFAULT_NS_PROT_LEN];
		int		tmp_protlen;
		ns_prot_t	tmp_prot = (ns_prot_t)tmp_prot_data;
		usItem*		new_agency;

		/*
		 * Make sure that the object's creation mode is at least
		 * as permissive as the access rights.  We'll change it after
		 * we've got an object ready for the creator.  We did this so
		 * the unix creat (or open) with O_RDWR and mode=RDONLY works.
		 */

		tmp_prot->head = prot->head;
		tmp_prot->head.acl_len = (prot->head.acl_len > 3 ? 3 :
					  prot->head.acl_len);
		tmp_prot->acl[0].authid = prot->acl[0].authid;
		tmp_prot->acl[0].rights = access | prot->acl[0].rights;
		if (prot->head.acl_len > 1) {
			tmp_prot->acl[1].authid = prot->acl[1].authid;
			tmp_prot->acl[1].rights = access | prot->acl[1].rights;
		}
		if (prot->head.acl_len > 2) {
			tmp_prot->acl[2].authid = prot->acl[2].authid;
			tmp_prot->acl[2].rights = access | prot->acl[2].rights;
		}
		tmp_protlen = NS_PROT_LEN((prot->head.acl_len > 3 ? 3 :
					   prot->head.acl_len));

		ret = fs_access_obj->fs_convert_prot_ns2fs(tmp_prot,
							   tmp_protlen,
							   &fs_attr);
		if (ret != ERR_SUCCESS) {
			mach_object_dereference(cred_obj);
			return (ret);
		}

		ret = fss_create(fsid,name,&fs_attr,&newid,fs_cred_ptr);

		if (ret != ERR_SUCCESS) {
			mach_object_dereference(cred_obj);
			return(ret);
		}

		ret = fs_fsid_export(newid,type,access,cred_obj,&tmp_agent);

		(void) fss_release(newid);
		mach_object_dereference(cred_obj);

		if (ret != ERR_SUCCESS) {
			mach_object_dereference(tmp_agent);
			return (ret);
		}

		/*
		 * Reset the protections for the object.  See the comment
		 * above relating to the tmp_prot.
		 */
		ret = tmp_agent->ns_get_item_ptr(&new_agency);
		if (ret != ERR_SUCCESS) {
			mach_object_dereference(tmp_agent);
			return (ret);
		}

		ret = new_agency->ns_set_protection(prot, protlen);
		if (ret != ERR_SUCCESS) {
			mach_object_dereference(tmp_agent);
			mach_object_dereference(new_agency);
			return (ret);
		}

		mach_object_dereference(new_agency);

		*new_agent = tmp_agent;
		return(ret);

	}else if ((type == NST_SYMLINK) || (type == NST_TRANSPARENT_SYMLINK)) {
		/*
		 * XXX Should allow the creation of an empty symlink.
		 */
		mach_object_dereference(cred_obj);
		return(NS_UNKNOWN_ENTRY_TYPE);
	}else{
		mach_object_dereference(cred_obj);
		return(NS_UNKNOWN_ENTRY_TYPE);
	}

	/* NEVER REACHED */
}


/*
 * Create a special "transparent symlink" in the directory.
 *
 * A transparent symlink is one that is followed or not during pathname
 * resolution, according to the NSF_TFOLLOW flag instead of NSF_FOLLOW.
 *
 * Transparent symlinks are implemented as regular symlinks in the
 * underlying file system, but the string " TRANSPARENT " is prepended
 * to their user-visible content string.
 */
mach_error_t vn_dir::ns_create_transparent_symlink(
	ns_name_t		name,
	ns_prot_t		prot,
	int			protlen,
	ns_path_t		path)
{
	mach_error_t		ret;
	unsigned int		namelen = strlen(name);
	unsigned int		pathlen = strlen(path);
	ns_path_t		internal_path;
	struct fs_attr		fs_attr;
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
	if (namelen >= NS_NAMELEN) {
		return(NS_NAME_TOO_LONG);
	}
	if (namelen == 0) {
		return(NS_INVALID_NAME);
	}

	if (pathlen >= (NS_PATHLEN - 13)) {
		return(NS_INVALID_ENTRY_DATA);
	}
	if (pathlen == 0) {
		return(NS_INVALID_ENTRY_DATA);
	}

	ret = fs_access_obj->fs_convert_prot_ns2fs(
						prot,protlen,&fs_attr);
	if (ret != ERR_SUCCESS) return(ret);

	bcopy(" TRANSPARENT ",internal_path,13);
	bcopy(path,&internal_path[13],pathlen + 1);

	ret = fss_symlink(fsid,internal_path,&fs_attr,name,fs_cred_ptr);
	if (ret != ERR_SUCCESS) return(ret);

	return(NS_SUCCESS);
}


/*
 * Entry insertion: insert an entry for an already existing object
 * in a directory.
 *
 * ns_insert_forwarding entry creates mount points and symlinks.
 * ns_insert_entry inserts hard links.
 */
mach_error_t vn_dir::ns_insert_forwarding_entry(
	ns_name_t		name,
	ns_prot_t		prot,
	int			protlen,
	usItem*			obj,
	ns_path_t		path)
{
	mach_error_t		ret;
	unsigned int		namelen = strlen(name);
	struct fs_attr		fs_attr;
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
	if (namelen >= NS_NAMELEN) {
		return(NS_NAME_TOO_LONG);
	}
	if (namelen == 0) {
		return(NS_INVALID_NAME);
	}
	if ((obj != NULL) || (path == NULL) || (path[0] == '\0')) {
		return(NS_INVALID_ENTRY_DATA);
	}

	ret = fs_access_obj->fs_convert_prot_ns2fs(
						prot,protlen,&fs_attr);
	if (ret != ERR_SUCCESS) return(ret);

	ret = fss_symlink(fsid,path,&fs_attr,name,fs_cred_ptr);
	if (ret != ERR_SUCCESS) return(ret);

	return(NS_SUCCESS);
}


mach_error_t vn_dir::ns_insert_entry(
	ns_name_t		name,
	usItem*			obj)
{
	mach_error_t		ret;
	unsigned int		namelen = strlen(name);
	vn_agency*		newagency;
	fs_id_t			newid;
	struct fs_attr		fs_attr;
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
	if (namelen >= NS_NAMELEN) {
		return(NS_NAME_TOO_LONG);
	}
	if (namelen == 0) {
		return(NS_INVALID_NAME);
	}
	if (obj == NULL) {
		return(NS_INVALID_ENTRY_DATA);
	}

	/*
	 * Strictly speaking we don't want ns_get_agency_ptr to obtain a
	 * mach_object reference because the rule is that the aot owns
	 * the sole such reference.  It's harmless though because while
	 * we hold the reference we know that the agency will not be
	 * deactivated (because there is an active agent).
	 */

	/* 
	 * XXX C++ Not very beautiful. Should agent be a sub-type of agency ?
	 * For now let it be like this, as this is the only place
	 * where this test is necessary.
	 */
	newagency = vn_agency::castdown(obj);
	if (newagency == NULL) {
		agent* agent_obj = agent::castdown(obj);
		usItem* agency_obj;
		(void) agent_obj->ns_get_item_ptr(&agency_obj);
		newagency = vn_agency::castdown(agency_obj);
	}

	ret = newagency->fs_get_fsid(&newid);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(newagency);
		return(ret);
	}

	ret = fss_link(fsid,newid,name,fs_cred_ptr);
	if (ret == ERR_SUCCESS) {
		ret = fss_getattr(newid,&fs_attr,fs_cred_ptr);
		if (ret == ERR_SUCCESS && fs_attr.va_type == VREG) {
			/*
			 * Notify file agencies that a new link has been
			 * created in persistent storage.
			 */
			newagency->vn_mark_permanent();
			ret = NS_SUCCESS;
		}
	}

	mach_object_dereference(newagency);
	(void) fss_release(newid);
	return(ret);
}


/*
 * Remove an entry from a directory.
 */
mach_error_t vn_dir::ns_remove_entry(
	ns_name_t		name)
{
	mach_error_t		ret;
	unsigned int		namelen = strlen(name);
	fs_id_t			target_id;
	boolean_t		last_link;
	int			tag;
	vn_agency*		agency_obj;
	struct fs_attr		fs_attr;
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
	if (namelen >= NS_NAMELEN) {
		return(NS_NAME_TOO_LONG);
	}
	if (namelen == 0) {
		return(NS_INVALID_NAME);
	}

	/*
	 * Find out what we are trying to remove.
	 *
	 * XXX Is there a problem if another thread removes the entry
	 * after the fss_lookup(), but before the fss_remove()?
	 */
	ret = fss_lookup(fsid,name,&target_id,&fs_attr,fs_cred_ptr);
	if (ret != NS_SUCCESS) return(ret);

	(void) fss_release(target_id);

	if (fs_attr.va_type == VDIR) {
		/*
		 * Removing a directory.
		 */
		ret = fss_rmdir(fsid,name,fs_cred_ptr);
	} else {
		/*
		 * Removing a file or symlink.
		 *
		 * There is no problem with open files, because the
		 * agency keep a reference count.
		 */
		ret = fss_remove(fsid,name,fs_cred_ptr,&last_link);
		if (ret == ERR_SUCCESS && last_link == TRUE && 
		    fs_attr.va_type == VREG) {
			/*
			 * The last link to a file was removed.  Notify the
			 * file that it's now "temporary".
			 */
			ret = vn_mgr_obj->aot_lookup_or_reserve(
				(aot_key_t)target_id, &agency_obj, &tag); 
			if (ret != ERR_SUCCESS)
				return(ret);
			else if (agency_obj == NULL) {
				/* nothing to do if it's not active */
				vn_mgr_obj->aot_cancel(tag);
				return(ERR_SUCCESS);
			}
			/*
			 * After notifying the agency that its file is
			 * temporary, release the reference obtained by
			 * aot_lookup_or_reserver().
			 */
			agency_obj->vn_mark_temporary();
			agency_obj->vn_dereference();
			return(ERR_SUCCESS);
		}
	}

	return(ret); 
}


#ifdef	notdef
/*
 * Rename an entry. This primitive is only guaranteed to be atomic
 * within a single directory. In other cases, the server is free to
 * perform the operation non-atomically, or to reject the request
 * by reporting that this primitive is not supported.
 */
mach_error_t vn_dir::ns_rename_entry(
	ns_name_t		name,
	usItem*			newdir,
	ns_name_t		newname)
{
	return(KERN_FAILURE);
}
#endif	notdef


/*
 * Return a standard attributes structure for all the entries in a directory.
 */
mach_error_t vn_dir::ns_list_entries(
	ns_mode_t		mode,
	ns_name_t		**names,	/* out */
	unsigned int		*names_count,	/* out */
	ns_entry_t		*entries,	/* out */
	unsigned int		*entries_count)	/* out */
{
	mach_error_t		ret;
	unsigned int		curoffset;
	unsigned int		curindex;
	unsigned int		maxindex;
	int			curcount;
	char			dirbuf[DIRBLKSIZ];
	unsigned int		dir_offset;
	unsigned int		max_dir_offset;
	struct direct		*curentry;
	DECLARE_UIO(uio);
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
#define	ABORT(_ret) {				\
	*names = NULL;				\
	*names_count = 0;			\
	*entries = NULL;			\
	*entries_count = 0;			\
	return(_ret);				\
}

	/*
	 * We preallocate a (very) large area for the reply, fill it
	 * one directory block at a time, and deallocate the excess space.
	 */

	ret = fsint_setup_list(0,NULL,NULL,1024,
			(vm_address_t *)names,(vm_address_t *)entries);
	if (ret != KERN_SUCCESS) {
		ABORT(ret);
	}

	curindex = 0;
	maxindex = 1023;
	curoffset = 0;
	curcount = 1024;

	for (;;) {
		/*
		 * Get one directory block.
		 */
		SETUP_UIO(uio,dirbuf,DIRBLKSIZ,curoffset);
		ret = fss_readdir(fsid,&uio,fs_cred_ptr);
		if (ret != NS_SUCCESS) {
			fsint_cleanup_list(0,curcount,
				(vm_address_t)*names,(vm_address_t)*entries);
			ABORT(ret);
		}

		/*
		 * Check that we have not reached the end of the directory.
		 */
		max_dir_offset = DIRBLKSIZ - uio.uio_resid;
		if (max_dir_offset == 0) {
			break;
		}

		/*
		 * Scan the directory block, fill-in names area.
		 */
		dir_offset = 0;
		while (dir_offset < max_dir_offset) {
			curentry =
				(struct direct *)(((char *)dirbuf) + dir_offset);

			if (curentry->d_ino != 0) {
				if (curindex > maxindex) {
					/*
					 * Double the size of the reply area.
					 */
					ret = fsint_setup_list(
							curcount,
							(vm_address_t)*names,
							(vm_address_t)*entries,
							curcount << 1,
							(vm_address_t *)names,
							(vm_address_t *)entries);
					if (ret != KERN_SUCCESS) {
						ABORT(ret);
					}
					curcount <<= 1;
					maxindex = (2 * maxindex) + 1;
				}
#ifdef	notdef
/* DEBUG */			Debug(printf("list_entries found:  0x%x %s\n",
					curentry->d_name,curentry->d_name));
#endif	notdef
					
				bcopy(curentry->d_name,
							((*names)[curindex]),
							curentry->d_namlen);
			((*names)[curindex])[curentry->d_namlen] = '\0';
/* XXX */			(*entries)[curindex].type = NST_UNSPECIFIED;
				(*entries)[curindex].obj_id = curentry->d_ino;
				curindex++;
			}
			dir_offset += curentry->d_reclen;
		}

		/*
		 * The offset for the next entry is normally left
		 * in the uio by fss_readdir().
		 */
		curoffset = uio.uio_offset;
	}

	*names_count = curindex;
	*entries_count = curindex;

	/*
	 * Deallocate excess space in reply areas.
	 */
	fsint_cleanup_list(curindex,curcount,
				(vm_address_t)*names,(vm_address_t)*entries);

	return(NS_SUCCESS);

#undef	ABORT
}


/*
 * List the types valid in this directory (i.e. for which a ns_create()
 * would succeed).
 */
mach_error_t vn_dir::ns_list_types(
	ns_type_t		**types,	/* out */
	int			*count)		/* out */
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


