/* 
 * Mach Operating System
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/vn_agency.cc,v $
 *
 * Purpose: Basic agency for FS-based file servers, common to files
 *	and directories.
 *
 * HISTORY:
 * $Log:	vn_agency.cc,v $
 * Revision 2.4  94/07/15  15:06:31  mrt
 * 	Fixed ns_set_times so that it won't reset the
 * 	protections to garbage.  (va_mode = -1).
 * 	[94/07/15  13:21:36  grm]
 * 
 * Revision 2.2  92/07/05  23:31:44  dpj
 * 	First working version.
 * 	[92/06/24  17:25:28  dpj]
 * 
 * Revision 2.1  91/09/27  11:33:49  pjg
 * Created.
 * 
 * Revision 2.8  90/12/19  11:04:54  jjc
 * 	Added test to fs_set_authid_map() to make sure that user is 
 * 	privileged before setting the authid map.
 * 	[90/10/30            jjc]
 * 
 * Revision 2.7  90/11/10  00:38:27  dpj
 * 	Replaced ns_set_attributes() with ns_set_times().
 * 	[90/11/08  22:18:34  dpj]
 * 
 * 	Added ns_set_attributes method.
 * 	[90/10/24  15:31:29  neves]
 * 
 * Revision 2.6  89/10/30  16:32:14  dpj
 * 	Use fs_access for most of the dirty work.
 * 	Added setup methods.
 * 	[89/10/27  17:32:51  dpj]
 * 
 * Revision 2.5  89/06/30  18:34:10  dpj
 * 	Use a global authenticator object.
 * 	Use a local copy of the mgr_id obtained from the active_table,
 * 	instead of a global variable.
 * 	[89/06/29  00:26:59  dpj]
 * 
 * Revision 2.4  89/06/05  17:18:09  dorr
 * 	uid's are shorts.  fix related bug.
 * 	[89/06/05  14:10:09  dorr]
 * 
 * Revision 2.3  89/03/30  12:05:43  dpj
 * 	Updated to new syntax for invoke_super().
 * 	[89/03/26  18:50:44  dpj]
 * 
 * Revision 2.2  89/03/17  12:40:06  sanzi
 * 	turn on admin rights for owner.
 * 	[89/03/07  19:49:30  dorr]
 * 	
 * 	use == for comparison.  works more reliably than =.
 * 	[89/03/01  17:28:15  dorr]
 * 	
 * 	Cleaned-up history after screw-up with dpj_moserver branch.
 * 	[89/02/24  19:14:24  dpj]
 * 	
 * 	Added access_table to the agency state.
 * 	[89/02/24  19:06:39  dpj]
 * 	
 * 	Eliminate brain damage with DECLARE_FS_CRED where
 * 	it *has* to be the last variable declaration.
 * 	[89/02/23  08:12:33  sanzi]
 * 	
 * 	Use invoke_super() instead of explicit procedure calls 
 * 	to access methods in the super_class
 * 	[89/02/16  17:23:04  dpj]
 * 	
 * 	Use "fss_" prefix for permanent storage primitives.
 * 	fs_export.h -> fs_types.h.
 * 	[89/02/15  22:03:20  dpj]
 * 	
 * 	fix size to be 64 bit quantity in get_attributes call
 * 	[89/02/16  13:22:50  dorr]
 * 	
 * 	Fixed prot structure length for ns_get_attributes().
 * 	[89/02/13  16:12:23  dpj]
 * 	
 * 	Reorganized check_access_from_data() to correctly handle privileged group ID.
 * 	[89/02/12  20:52:29  dpj]
 * 	
 * 	fix size of ns_prot_t in prot_convert_fs2ns
 * 	[89/02/10  13:54:30  dorr]
 * 	
 * 	dan...two &'s means logical and, one means bit test.
 * 	[89/02/09  18:01:33  dorr]
 * 	
 * 	Fixed the calls for conversion between authid's and uid/gid's to
 * 	operate on the fs_authenticator instead of Self.
 * 	[89/02/08  19:25:09  dpj]
 * 	
 * 	fix some conversions where someone (who shall remain nameless)
 * 	[89/02/08  16:56:16  dorr]
 * 	
 * 	first crack at compiling.
 * 	[89/02/08  14:24:21  dorr]
 * 	
 * 	Added NSR_GETATTR in access rights translations.
 * 	[89/02/07  18:50:24  dpj]
 * 	
 * 	First reasonable version.
 * 	[89/02/07  13:50:08  dpj]
 * 	[89/02/03  09:43:01  dpj]
 * 
 */

#ifndef lint
char * vn_agency_rcsid = "$Header: vn_agency.cc,v 2.4 94/07/15 15:06:31 mrt Exp $";
#endif	lint

#include	<vn_agency_ifc.h>
#include	<fs_auth_ifc.h>
#include	<vn_mgr_ifc.h>

extern "C" {
#include	<base.h>
#include	<ns_types.h>
#include	<fs_types.h>
}


#define BASE agency
DEFINE_CLASS(vn_agency)

void vn_agency::init_class(usClass* class_obj)
{
        BASE::init_class(class_obj);

        BEGIN_SETUP_METHOD_WITH_ARGS(vn_agency);
//        SETUP_METHOD_WITH_ARGS(vn_agency,fs_set_authid_map);
        SETUP_METHOD_WITH_ARGS(vn_agency,ns_get_attributes);
        SETUP_METHOD_WITH_ARGS(vn_agency,ns_set_times);
        SETUP_METHOD_WITH_ARGS(vn_agency,ns_get_protection);
        SETUP_METHOD_WITH_ARGS(vn_agency,ns_set_protection);
        SETUP_METHOD_WITH_ARGS(vn_agency,ns_get_privileged_id);
        END_SETUP_METHOD_WITH_ARGS;
}


vn_agency::vn_agency()
:
	fsid(0), fs_access_obj(0), vn_mgr_obj(0)
{
}

vn_agency::vn_agency(fs_id_t _fsid, fs_access* _fs_access_obj, 
		     ns_mgr_id_t mgr_id,
		     access_table* acctab, vn_mgr* _mgr)
:
 agency(mgr_id, acctab),
 fsid(_fsid), fs_access_obj(_fs_access_obj), vn_mgr_obj(_mgr)
{
	(void) fss_reference(fsid);
	mach_object_reference(fs_access_obj);
	mach_object_reference(vn_mgr_obj);

	mutex_init(&vn_lock);
	vn_refcount = 0;
}


vn_agency::~vn_agency()
{
	DESTRUCTOR_GUARD();
	if (Local(fsid != 0)) fss_release(Local(fsid));
	mach_object_dereference(Local(fs_access_obj));
	mach_object_dereference(Local(vn_mgr_obj));
}


/*
 * Establish a mapping table for authids - uid/gid.
 */
mach_error_t vn_agency::fs_set_authid_map(char *data, unsigned int len)
{
	std_cred*		cred_obj;
	ns_cred_t               cred_ptr;
	mach_error_t		ret;

	ret = agent::base_object()->ns_get_cred_obj(&cred_obj);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	ret = cred_obj->ns_get_cred_ptr(&cred_ptr);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	ret = fs_access_obj->fs_is_priv(cred_ptr);
	if (ret == ERR_SUCCESS) {
		return(ret);
	}

	return(fs_access_obj->fs_set_authid_map(data,len));
}


/*
 * Get the FS ID associated with this object.
 */
mach_error_t vn_agency::fs_get_fsid(fs_id_t *fsid)
	      			     
	       			      		/* out */
{
	*fsid = Local(fsid);
	(void) fss_reference(Local(fsid));

	return(ERR_SUCCESS);
}


/*
 * Convert FS object type into NS type.
 */
ns_type_t vn_agency::fs_convert_type_fs2ns(enum vtype fstype)
{
	switch(fstype) {
		case VREG:
			return(NST_FILE);
		case VDIR:
			return(NST_DIRECTORY);
		case VLNK:
			return(NST_SYMLINK);
		default:
			return(NST_INVALID);
	}
}


/*
 * Get a standard attributes structure for an object.
 */
mach_error_t vn_agency::ns_get_attributes(ns_attr_t attr, int *attrlen)
	      			     
	         		     		/* out */
	   			         	/* out */
{
	mach_error_t		ret;
	struct fs_attr		fs_attr;
	int			ns_prot_data[NS_PROT_LEN(16)];
	ns_prot_t		ns_prot = (ns_prot_t)ns_prot_data;
	int			ns_protlen = NS_PROT_LEN(16);
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);	

	ret = fss_getattr(Local(fsid),&fs_attr,fs_cred_ptr);
	if (ret != ERR_SUCCESS) {
		*attrlen = 0;
		return(ret);
	}

	attr->version = 1;
	attr->valid_fields = NS_ATTR_ALL;
	attr->type = fs_convert_type_fs2ns(fs_attr.va_type);
	attr->nlinks = (unsigned long) fs_attr.va_nlink;
	attr->mgr_id = mgr_id;
	attr->obj_id = fs_attr.va_nodeid;
	attr->access_time.seconds = fs_attr.va_atime.tv_sec;
	attr->access_time.microseconds = fs_attr.va_atime.tv_usec;
	attr->modif_time.seconds = fs_attr.va_mtime.tv_sec;
	attr->modif_time.microseconds = fs_attr.va_mtime.tv_usec;
	attr->creation_time.seconds = fs_attr.va_ctime.tv_sec;
	attr->creation_time.microseconds = fs_attr.va_ctime.tv_usec;
	INT_TO_DLONG(&attr->size,fs_attr.va_size);

	ret = fs_access_obj->fs_convert_prot_fs2ns(&fs_attr,
					ns_prot,(unsigned int*)&ns_protlen);
	if (ret != ERR_SUCCESS) {
		*attrlen = 0;
		return(ret);
	}
	bzero(&(attr->prot_ltd),sizeof(struct ns_prot_ltd));
	bcopy(ns_prot->acl,&(attr->prot_ltd),
			ns_prot->head.acl_len * sizeof(struct ns_acl_entry));

	*attrlen = sizeof(struct ns_attr) / sizeof(int);

	return(ERR_SUCCESS);
}

/*
 * Set the access and modification times for an object.
 */
mach_error_t vn_agency::ns_set_times(time_value_t atime, time_value_t mtime)
{
	mach_error_t		ret;
	struct fs_attr		fs_attr;
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);	

	fs_attr.va_type = (enum vtype)-1;
	fs_attr.va_mode = -1;
	fs_attr.va_uid = -1;
	fs_attr.va_gid = -1;
	fs_attr.va_fsid = -1;
	fs_attr.va_nodeid = -1;
	fs_attr.va_nlink = -1;
	fs_attr.va_size = (unsigned int)-1;
	fs_attr.va_blocksize = -1;
	fs_attr.va_atime.tv_sec = atime.seconds;
	fs_attr.va_atime.tv_usec = atime.microseconds;
	fs_attr.va_mtime.tv_sec = mtime.seconds;
	fs_attr.va_mtime.tv_usec = mtime.microseconds;
	fs_attr.va_ctime.tv_sec = -1;
	fs_attr.va_ctime.tv_usec = -1;
	fs_attr.va_rdev = -1;
	fs_attr.va_blocks = -1;

	ret = fss_setattr(Local(fsid),&fs_attr,fs_cred_ptr);
	return(ret);
}

/*
 * Exported protection operations.
 *
 * On a normal agency, these methods would be inherited from a "prot"
 * object that is a delegate of the agency object. For FS-based
 * servers, these methods are implemented directly in the vn_agency
 * itself, because they need to access the FS layer directly.
 */

mach_error_t vn_agency::fs_check_access(
	ns_access_t	access,
	fs_cred*	cred_obj)
{
	mach_error_t		ret;
	struct fs_cred_data	*fs_cred_ptr;
	struct fs_attr		fs_attr;

	/*
	 * Get the credentials.
	 */
	ret = cred_obj->fs_get_cred_ptr(&fs_cred_ptr);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Get the protection from the FS layer.
	 */
	ret = fss_getattr(Local(fsid),&fs_attr,fs_cred_ptr);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Match the credentials against the protection.
	 */
	ret = fs_access_obj->fs_check_access_from_data(&fs_attr,
						access,fs_cred_ptr);

	return(ret);
}

mach_error_t vn_agency::ns_check_access(
	ns_access_t	access,
	std_cred*	cred_obj)
{
	fs_cred*	cr = fs_cred::castdown(cred_obj);
	if (cr == NULL) {
		return(US_INVALID_ACCESS);
	}

	return(fs_check_access(access,cr));
}


/*
 * Get and set the protection on an object, using the standard ACL
 * format.
 *
 * XXX This method currently ignores the generation number in the prot
 * structure.
 */
mach_error_t vn_agency::ns_set_protection(ns_prot_t prot, int protlen)
{
	mach_error_t		ret;
	struct fs_attr		fs_attr;
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
	ret = fs_access_obj->fs_convert_prot_ns2fs(prot,protlen,&fs_attr);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	/*
	 * Fix all the other attributes.
	 */
	fs_attr.va_type = (enum vtype)-1;
	fs_attr.va_fsid = -1;
	fs_attr.va_nodeid = -1;
	fs_attr.va_nlink = -1;
	fs_attr.va_size = (unsigned int)-1;
	fs_attr.va_blocksize = -1;
	fs_attr.va_atime.tv_sec = -1;
	fs_attr.va_atime.tv_usec = -1;
	fs_attr.va_mtime.tv_sec = -1;
	fs_attr.va_mtime.tv_usec = -1;
	fs_attr.va_ctime.tv_sec = -1;
	fs_attr.va_ctime.tv_usec = -1;
	fs_attr.va_rdev = -1;
	fs_attr.va_blocks = -1;

	/*
	 * XXX Check that the owner (NSR_ADMIN) does not
	 * change the ownership of the object ?
	 */

	ret = fss_setattr(Local(fsid),&fs_attr,fs_cred_ptr);

	return(ret);
}


mach_error_t vn_agency::ns_get_protection(ns_prot_t prot,int *protlen)
{
	mach_error_t		ret;
	struct fs_attr		fs_attr;
	DECLARE_FS_CRED(fs_cred_ptr);
	SETUP_FS_CRED(fs_cred_ptr);
	
	ret = fss_getattr(Local(fsid),&fs_attr,fs_cred_ptr);
	if (ret != ERR_SUCCESS) {
		*protlen = 0;
		return(ret);
	}

	ret = fs_access_obj->fs_convert_prot_fs2ns(&fs_attr,
						prot,(unsigned int*)protlen);

	return(ret);
}


/*
 * Find out the privileged authentication ID that has full access to
 * this object.
 */
mach_error_t vn_agency::ns_get_privileged_id(int *privid)
	      			     
	   			        	/* out */
{
	return(fs_access_obj->ns_get_privileged_id(privid));
}


mach_error_t vn_agency::fs_create_agent(
	ns_access_t	access,
	fs_cred*	cred_obj,
	agent**		newobj)
{
	mach_error_t		ret;

	DEBUG1((1), (0, "vn_agency: fs_create_agent\n"));

	if (access == 0) {
		*newobj = 0;
		return(ERR_SUCCESS);
	}

	/*
	 * Access check.
	 */
       	ret = fs_check_access(access,cred_obj);
	if (ret != ERR_SUCCESS) {
		*newobj = NULL;
		return(ret);
	}

	/*
	 * Create the agent.
	 */
	mach_object_reference(this);
	*newobj = new agent(this, (std_cred*)cred_obj,
					access_tab, access, &ret);
	mach_object_dereference(this);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(*newobj);
		*newobj = 0;
		return(ret);
	}
	return(ERR_SUCCESS);
}


/*
 * Create a new initial agent for this agency, with anonymous credentials.
 */
mach_error_t vn_agency::ns_create_initial_agent(agent **newobj)
	      			     
	             		        		/* out */
{
	mach_error_t		ret;
	fs_cred		*anon_cred = 0;

	*newobj = 0;

	fs_auth	*authenticator = new fs_auth(fs_access_obj);

	ret = authenticator->fs_translate_token(MACH_PORT_NULL,&anon_cred);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(authenticator);
		return(ret);
	}
	ret = fs_create_agent(NSR_REFERENCE,anon_cred,newobj);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(anon_cred);
		mach_object_dereference(authenticator);
		return(ret);
	}
	mach_object_dereference(anon_cred);
	mach_object_dereference(authenticator);

	return(ERR_SUCCESS);
}


mach_error_t vn_agency::ns_register_agent(ns_access_t access)
{
	vn_reference();
	return(ERR_SUCCESS);
}


mach_error_t vn_agency::ns_unregister_agent(ns_access_t access)
{
	vn_dereference();
	return(ERR_SUCCESS);
}


void vn_agency::vn_reference()
{
	mutex_lock(&vn_lock);
	vn_refcount += 1;
	mutex_unlock(&vn_lock);
}


void vn_agency::vn_dereference()
{
	mach_error_t		ret;
	void			*tag;

	/*
	 * Must synchronize with the aot who initiates destroying and
	 * handles lookup requests from direcory objects.
	 * => get the aot_lock first before the local lock, and then
	 * diddle the refcount.
	 */

	if ((ret = vn_mgr_obj->aot_lock((aot_key_t)fsid, &tag))
	    != ERR_SUCCESS)
		CRITICAL((0,"vn_agency::vn_dereference"));

	mutex_lock(&vn_lock);

	vn_refcount -= 1;
	if (vn_refcount == 0)
		vn_mgr_obj->aot_set_state(tag, AOT_STATE_INACTIVE);

	mutex_unlock(&vn_lock);
	vn_mgr_obj->aot_unlock(tag);
}


mach_error_t vn_agency::vn_destroy()
{
	mach_error_t		ret;

	mutex_lock(&vn_lock);

	if (vn_refcount == 0) 
		ret = ERR_SUCCESS;
	else
		ret = US_OBJECT_BUSY;

	mutex_unlock(&vn_lock);
		
	/*
	 * If we're returning ERR_SUCCESS then this object
	 * is going to be freed.  We know noone else has a reference.
	 */
	return(ret);
}


mach_error_t vn_agency::vn_clean()
{
	/*
	 * vn_clean() only makes sense for files
	 * (but it must be defined here for uniformity).
	 */
	return _notdef();
}
