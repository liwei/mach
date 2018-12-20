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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxstat.cc,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxstat.cc,v $
 * Revision 2.5  94/07/08  16:02:26  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  94/05/17  14:08:57  jms
 * 	Need dummy  implementations of virtual methods in class uxstat 
 * 		for 2.3.3 g++ -modh
 * 	[94/04/28  19:01:57  jms]
 * 
 * Revision 2.2.2.1  94/02/18  11:34:56  modh
 * 	Need implementations of virtual methods in class uxstat for 2.3.3 g++
 * 
 * Revision 2.3  94/01/11  17:51:06  jms
 * 	Correct "owner" definition  so that the first entry in the ACL is the
 * 	owner, not the NSR_ADMIN one.  Corresponds to accepted practice, and
 * 	NSR_ADMIN need not be given to the owner.
 * 	[94/01/09  19:55:49  jms]
 * 
 * Revision 2.2  91/11/06  14:13:02  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:21:35  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:12:23  pjg]
 * 
 * Revision 2.8  91/05/05  19:29:10  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:47  dpj]
 * 
 * 	Support transparent symlinks, new pipes, network endpoints (sockets). 
 * 	[91/04/28  10:36:17  dpj]
 * 
 * Revision 2.7  90/11/10  00:39:05  dpj
 * 	Fixed a typo causing stat to report incorrect 
 * 	file modification times.
 * 	[90/10/24  14:42:30  neves]
 * 
 * Revision 2.6  90/09/05  09:45:14  mbj
 * 	Added TTY case.
 * 	[90/09/04  15:10:06  mbj]
 * 
 * Revision 2.5  90/01/02  22:18:06  dorr
 * 	add uxprot operations.
 * 
 * Revision 2.4.1.1  90/01/02  14:23:25  dorr
 * 	add protection conversion methods to facilitate
 * 	ditching uxprot object.  take access object
 * 	in setup call.
 * 
 * Revision 2.4  89/11/28  19:12:17  dpj
 * 	Added code to handle NST_UPIPE.
 * 	[89/11/20  20:59:13  dpj]
 * 
 * Revision 2.3  89/10/30  16:37:21  dpj
 * 	Temporarily removed type translation for pipes.
 * 	[89/10/27  19:30:43  dpj]
 * 
 * Revision 2.2  89/06/30  18:36:49  dpj
 * 	Initial revision.
 * 	[89/06/29  00:49:48  dpj]
 * 
 */

#include <uxstat_ifc.h>

extern "C" {
#include <base.h>
#include <debug.h>
#include <us_error.h>

#include <ns_types.h>

/* 
#include <uxprot_methods.h>
#include <fs_internal.h>
*/

#include <sys/types.h>
#include <sys/stat.h>
}


uxstat::uxstat(fs_access* acc_obj)
	: head(NULL), hint(NULL), next_dev(1)
{
	mutex_init(&_Local(lock));
	access_obj = acc_obj;
	mach_object_reference(access_obj);
}


uxstat::~uxstat()
{
	map_entry_t		entry1, entry2;

	entry1 = _Local(head);
	while(entry1 != NULL) {
		entry2 = entry1->next;
		Free(entry1);
		entry1 = entry2;
	}
	mach_object_dereference(_Local(access_obj));
}


mach_error_t
uxstat::uxstat_std_to_unix_mgrid(ns_mgr_id_t mgrid, dev_t *dev)
{
	map_entry_t		entry;

	mutex_lock(&_Local(lock));
	/*
	 * First look at the hint.
	 */
	if (_Local(hint) != NULL) {
		if ((_Local(hint)->id.v1 == mgrid.v1) &&
			(_Local(hint)->id.v2 == mgrid.v2)) {
			*dev = _Local(hint)->dev;
			goto finish;
		}
	}
	/*
	 * Search the whole list sequentially.
	 */
	entry = _Local(head);
	while (entry != NULL) {
		if ((entry->id.v1 == mgrid.v1) &&
			(entry->id.v2 == mgrid.v2)) {
			*dev = entry->dev;
			_Local(hint) = entry;
			goto finish;
		}
		entry = entry->next;
	}

	/*
	 * Allocate a new mapping.
	 */
	entry = New(struct map_entry);
	entry->id = mgrid;
	entry->dev = _Local(next_dev)++;
	entry->next = _Local(head);
	*dev = entry->dev;
	_Local(head) = entry;
	_Local(hint) = entry;

    finish:
	mutex_unlock(&_Local(lock));
	return(ERR_SUCCESS);
}


mach_error_t
uxstat::uxstat_std_to_unix_attr(ns_attr_t attr, struct stat *statbuf)
{
	mach_error_t		ret;

#define	is_not_valid(at,mask)	(!((at)->valid_fields & (mask)))
#define	tv_to_time(tv)		((tv).seconds)

	if (attr->version != NS_ATTR_VERSION) {
		bzero(statbuf,sizeof(struct stat));
		return(US_UNKNOWN_ERROR);
	}

	/*
	 * Sizes.
	 */
	statbuf->st_blksize = vm_page_size;	/* XXX */
	if ( is_not_valid(attr,NS_ATTR_SIZE) ) {
		statbuf->st_size = 0;
		statbuf->st_blocks = 0;
	} else {
		statbuf->st_size = DLONG_TO_U_INT(attr->size);
		/* XXX give an approximate # of blocks */
		statbuf->st_blocks = (statbuf->st_size / statbuf->st_blksize);
		if ( statbuf->st_size % statbuf->st_blksize )
			statbuf->st_blocks++;
	}

	/*
	 * Times.
	 */
	if ( is_not_valid(attr,NS_ATTR_ATIME) ) {
		statbuf->st_atime = 0;
	} else {
		statbuf->st_atime = tv_to_time(attr->access_time);
	}
	if ( is_not_valid(attr,NS_ATTR_MTIME) ) {
		statbuf->st_mtime = 0;
	} else {
		statbuf->st_mtime = tv_to_time(attr->modif_time);
	}
	if ( is_not_valid(attr,NS_ATTR_CTIME) ) {
		statbuf->st_ctime = 0;
	} else {
		statbuf->st_ctime = tv_to_time(attr->creation_time);
	}

	/*
	 * Protection.
	 */
	if ( is_not_valid(attr,NS_ATTR_PROT) ) {
		statbuf->st_mode = 0;
	} else {
		int		mode, uid, gid;

		ret = uxprot_std_to_unix_prot(attr->prot_ltd.acl, 3, &mode,
					       &uid,&gid);
		if (ret != ERR_SUCCESS) {
			/*
			 * The conversion will have provided the best
			 * possible translation, so we can ignore the
			 * error.
			 *
			 * XXX How to report the problem to a higher level?
			 */
		}
		statbuf->st_mode = mode;
		statbuf->st_uid = uid;
		statbuf->st_gid = gid;
	}

	/*
	 * Links.
	 */
	if ( is_not_valid(attr,NS_ATTR_NLINKS) ) {
		statbuf->st_nlink = 1;
	} else {
		statbuf->st_nlink = attr->nlinks;
	}
	
	/*
	 * Object + manager ID.
	 */
	if ( is_not_valid(attr,NS_ATTR_ID) ) {
		statbuf->st_dev = 0;
		statbuf->st_ino = 0;
	} else {
		statbuf->st_ino = attr->obj_id;
		ret = uxstat_std_to_unix_mgrid(attr->mgr_id,
						&statbuf->st_dev);
		if (ret != ERR_SUCCESS) {	/* XXX */
			statbuf->st_dev = 0;
		}
	}

	/*
	 * Type.
	 */
	switch(attr->type) {
	    case NST_DIRECTORY:
		statbuf->st_mode |= S_IFDIR;
		break;
	    case NST_SYMLINK:
		statbuf->st_mode |= S_IFLNK;
		break;
	    case NST_UPIPE_BYTES:
		statbuf->st_mode = 0;	/* Hey, that's the way it is ... */
		break;
	    case NST_CLTS_BYTES:
	    case NST_CLTS_RECS:
	    case NST_COTS_BYTES:
	    case NST_COTS_RECS:
	    case NST_CONNECTOR:
		statbuf->st_mode |= S_IFSOCK;
		break;
	    case NST_TTY:
		statbuf->st_mode |= S_IFCHR;
		statbuf->st_rdev = attr->obj_id;
		break;
	    case NST_TRANSPARENT_SYMLINK:
		/*
		 * This should only happen when we failed to resolve the
		 * target. Make a wild guess...
		 */
		statbuf->st_mode |= S_IFSOCK;
		break;
	    default:
		/* not really, but... */
	    case NST_FILE:
		statbuf->st_mode |= S_IFREG;
		break;
	}

	return(ERR_SUCCESS);
}


mach_error_t
uxstat::uxprot_unix_to_std_prot(int mode, int uid, int gid, 
				ns_acl_entry_t acl, int *acllen)
{
	*acllen = 3;

	access_obj->fs_uid_to_authid(uid,&acl[0].authid);
	acl[0].rights =
		access_obj->fs_convert_access_fs2ns((mode & 0700) >> 6) | NSR_ADMIN;

	access_obj->fs_uid_to_authid(gid,&acl[1].authid);
	acl[1].rights =
		access_obj->fs_convert_access_fs2ns((mode & 070) >> 3);

	acl[2].authid = 0;	/* wildcard */
	acl[2].rights =
		access_obj->fs_convert_access_fs2ns(mode & 07);

	return(ERR_SUCCESS);
}


mach_error_t 
uxstat::uxprot_std_to_unix_prot(ns_acl_entry_t acl, int acllen, int *mode, 
				int *uid, int *gid)
{
	int			i;
	int			owner = 0;
	int			group = 0;
	boolean_t		owner_found = FALSE;
	boolean_t		group_found = FALSE;
	int			owner_rights = 0;
	int			group_rights = 0;
	int			default_rights = 0;
	mach_error_t		ret = ERR_SUCCESS;

	/*
	 * Identify the entries for owner, group, and default in
	 * the prot structure. Be pessimistic if the translation
	 * cannot be perfect, and report greater privileges than
	 * are actually available, rather than smaller privileges.
	 */

	for (i = 0; i < acllen; i++) {
		if (acl[i].authid == 0) {		/* wildcard */
			default_rights |= acl[i].rights;
			continue;
		}

#if OWNER_IS_NSR_ADMIN_NOT_FIRST
		if (acl[i].rights & NSR_ADMIN) {	/* owner */
#else OWNER_IS_NSR_ADMIN_NOT_FIRST
		if (i == 0) {
#endif OWNER_IS_NSR_ADMIN_NOT_FIRST
			if (owner_found) {
				owner = 0;
				default_rights |= acl[i].rights;
			} else {
				owner = acl[i].authid;
				owner_found = TRUE;
			}
			owner_rights |= acl[i].rights;
		} else {				/* group */	
			if (group_found) {
				group = 0;
				default_rights |= acl[i].rights;
			} else {
				group = acl[i].authid;
				group_found = TRUE;
			}
			group_rights |= acl[i].rights;
		}
	}

	if (default_rights & NSR_ADMIN) {
		owner = 0;
		owner_rights |= default_rights;
	}

	if (owner == 0) {
		ret = NS_UNSUPPORTED_PROT;
		*uid = -1;
	} else {
		access_obj->fs_authid_to_uid(owner,uid);
	}

	if (owner == 0) {
		ret = NS_UNSUPPORTED_PROT;
		*gid = -1;
	} else {
		access_obj->fs_authid_to_gid(group,gid);
	}

	*mode = 0;
	*mode |= access_obj->fs_convert_access_ns2fs(owner_rights) << 6;
        *mode |= access_obj->fs_convert_access_ns2fs(group_rights) << 3;
	*mode |= access_obj->fs_convert_access_ns2fs(default_rights);

	return(ret);
}

mach_error_t
uxstat::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_duplicate(ns_access_t access, usItem** newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_set_times(time_value_t atime, time_value_t mtime)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_get_protection(ns_prot_t prot, int* protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_set_protection(ns_prot_t prot, int protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_get_privileged_id(int* id)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxstat::ns_get_manager(ns_access_t access, usItem **newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

