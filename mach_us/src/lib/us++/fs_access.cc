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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/fs_access.cc,v $
 *
 * Purpose: Handle all protection operations for FS-based systems.
 *
 * HISTORY:
 * $Log:	fs_access.cc,v $
 * Revision 2.6  94/07/07  17:23:15  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  94/06/29  14:56:44  mrt
 * 	Fixed code to support access to read only mounted devices.
 * 	[94/06/29  13:50:05  grm]
 * 
 * Revision 2.4  94/05/17  14:07:15  jms
 * 	fs_check_access_in_data: don't permit special ids to succeed on NSR_EXECUTE
 * 	unless some one is permitted to execute.
 * 	[94/04/28  18:46:47  jms]
 * 
 * 	add fs_is_root().
 * 	[90/01/02  14:14:29  dorr]
 * 
 * Revision 2.3  92/07/05  23:27:20  dpj
 * 	Added a DESTRUCTOR_GUARD.
 * 	Defined as a LOCAL_CLASS.
 * 	Changed some signed ints to unsigned ints and vice-versa.
 * 	[92/06/24  16:17:14  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:23  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:32:29  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:32:42  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:27:18  pjg]
 * 
 * Revision 2.7  91/10/06  22:30:01  jjc
 * 	Backed-out Paul Neves' fix for NSR_EXECUTE, which does not work
 * 	for execute-only files.
 * 	[91/10/06            dpj]
 * 
 * Revision 2.6  91/04/12  18:48:15  jjc
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:49:11  jjc]
 * 
 * Revision 2.5.1.1  91/02/05  16:01:29  neves
 * 	Fixed a conversion bug which always promoted
 * 	execute access to execute and read access.
 * 
 * Revision 2.5  90/12/19  11:04:40  jjc
 * 	Fixed bug in fs_access_initialize() that tried to pass 
 * 	a short pointer to fs_authid_to_{uid,gid}() which expect
 * 	an integer pointer.
 * 	[90/12/03            jjc]
 * 	Defined USER_AUTHID and GROUP_AUTHID as starting user and group
 * 	authids, and use them to convert uids and gids to authids and
 * 	vice-a-versa by default.
 * 	Added fs_is_priv() which checks to see if the user is root or in a
 * 	privileged group.
 * 	Changed fs_set_authid_map() to re-evaluate priv_uid and priv_gid
 * 	after setting the authid map.
 * 	Include ns_internal.h for ns_get_cred_obj.
 * 	[90/10/29            jjc]
 * 
 * Revision 2.4  90/01/02  22:12:09  dorr
 * 	add fs_is_root.
 * 
 * Revision 2.3  89/11/28  19:11:09  dpj
 * 	Make the parsing of the authid data more robust:
 * 		- force a zero at the end of the data string,
 * 		- generate an error if the data string does not
 * 		end with a newline,
 * 		- strengthen the sscanf() format string.
 * 	[89/11/20  20:45:56  dpj]
 * 
 * Revision 2.2  89/10/30  16:32:02  dpj
 * 	First version.
 * 	[89/10/27  17:30:29  dpj]
 * 
 */

#ifndef lint
char * fs_access_rcsid = "$Header: fs_access.cc,v 2.6 94/07/07 17:23:15 mrt Exp $";
#endif	lint

#include <fs_access_ifc.h>

extern "C" {
#include	<sys/file.h>
int atoi(char*);
}

#define	BASE	usTop
DEFINE_LOCAL_CLASS(fs_access)


/*
 * Debugging switch.
 */
int		fs_access_debug = TRUE;


/*
 * Default values for special authentication IDs,
 * and for the size of mapping hash tables.
 *
 * These values can be patched with a debugger.
 */
ns_authid_t	fs_access_default_priv_authid = 1;
ns_authid_t	fs_access_default_root_authid = 10;
int		fs_access_default_table_size = 64;


/*
 * Special uid/gid used to represent root internally, since
 * 0 cannot be stored in the hash table.
 */
#define		ZERO_ID			-1	


/*
 * Starting user and group authids
 */
#define	USER_AUTHID	1000
#define	GROUP_AUTHID	2000

/*
 * Methods.
 */
fs_access::fs_access()
{
	int		gid, uid;
	mach_error_t		ret;

	_Local(authid_to_uid) = NULL;
	_Local(authid_to_gid) = NULL;
	_Local(uid_to_authid) = NULL;
	_Local(gid_to_authid) = NULL;

	_Local(priv_authid) = fs_access_default_priv_authid;
	_Local(root_authid) = fs_access_default_root_authid;

	ret = fs_authid_to_uid(_Local(priv_authid),&uid);
	if (ret != ERR_SUCCESS) {
		_Local(priv_uid) = 0;
		mach_error("Cannot setup the priv uid",ret);
	}
	_Local(priv_uid) = uid;
	ret = fs_authid_to_gid(_Local(priv_authid),&gid);
	if (ret != ERR_SUCCESS) {
		_Local(priv_gid) = 0;
		mach_error("Cannot setup the priv gid",ret);
	}
	_Local(priv_gid) = gid;
	_Local(dev_mode) = O_RDWR;
}

fs_access::fs_access(
	int			dev_mode)
{
	int		gid, uid;
	mach_error_t		ret;

	_Local(authid_to_uid) = NULL;
	_Local(authid_to_gid) = NULL;
	_Local(uid_to_authid) = NULL;
	_Local(gid_to_authid) = NULL;

	_Local(priv_authid) = fs_access_default_priv_authid;
	_Local(root_authid) = fs_access_default_root_authid;

	ret = fs_authid_to_uid(_Local(priv_authid),&uid);
	if (ret != ERR_SUCCESS) {
		_Local(priv_uid) = 0;
		mach_error("Cannot setup the priv uid",ret);
	}
	_Local(priv_uid) = uid;
	ret = fs_authid_to_gid(_Local(priv_authid),&gid);
	if (ret != ERR_SUCCESS) {
		_Local(priv_gid) = 0;
		mach_error("Cannot setup the priv gid",ret);
	}
	_Local(priv_gid) = gid;
	_Local(dev_mode) = dev_mode;
}


fs_access::~fs_access()
{
	DESTRUCTOR_GUARD();

	if (_Local(authid_to_uid) != NULL) hash_free(_Local(authid_to_uid));
	if (_Local(authid_to_gid) != NULL) hash_free(_Local(authid_to_gid));
	if (_Local(uid_to_authid) != NULL) hash_free(_Local(uid_to_authid));
	if (_Local(gid_to_authid) != NULL) hash_free(_Local(gid_to_authid));
}


/*
 * Initialize the tables for authid - uid/gid mapping.
 */
mach_error_t fs_access::fs_set_authid_map(char *data, unsigned int len)
{
	char			*cp = data;
	char			*end = data + len;
	int			err;
	char			authid_str[100];
	char			uid_str[100];
	char			gid_str[100];
	ns_authid_t		authid;
	int			uid;
	int			gid;
	int			error_count = 0;
	mach_error_t		ret;

	_Local(authid_to_uid) = hash_init(NULL,NULL,
					fs_access_default_table_size);
	_Local(authid_to_gid) = hash_init(NULL,NULL,
					fs_access_default_table_size);
	_Local(uid_to_authid) = hash_init(NULL,NULL,
					fs_access_default_table_size);
	_Local(gid_to_authid) = hash_init(NULL,NULL,
					fs_access_default_table_size);

	if (data[len - 1] != '\n') {
				ERROR((0,
		"Authid map table data does not end with a newline\n"));
				return(US_INVALID_ARGS);
	}
	data[len - 1] = '\0';

	while ((cp != NULL) && (cp < end)) {
		if (*cp != '#') {
			authid_str[0] = '\0';
			uid_str[0] = '\0';
			gid_str[0] = '\0';
			err = sscanf(cp,"%[^:]:%[^:]:%[^:]: \n",
						authid_str,uid_str,gid_str);
			if ((authid_str[0] == '\0') && (err > 0)) {
				ERROR((0,
		"Invalid authid map table entry, near \"%20s\"",cp));
				return(US_INVALID_ARGS);
			}

			DEBUG1(fs_access_debug,(0,
					"Mapping entry: %5s %5s %5s",
					authid_str,uid_str,gid_str));

			authid = (ns_authid_t) atoi(authid_str);

			if (uid_str[0] != '\0') {
				uid = atoi(uid_str);
				if (uid == 0) uid = ZERO_ID;
				if (! hash_enter(_Local(authid_to_uid),
								authid,uid)) {
					ERROR((0,
				"Duplicate authid in map table: %d",
						authid));
					error_count++;
				}
				if (! hash_enter(_Local(uid_to_authid),
								uid,authid)) {
					ERROR((0,
				"Duplicate uid in map table: %d",
						uid));
					error_count++;
				}
			}

			if (gid_str[0] != '\0') {
				gid = atoi(gid_str);
				if (gid == 0) gid = ZERO_ID;
				if (! hash_enter(_Local(authid_to_gid),
								authid,gid)) {
					ERROR((0,
				"Duplicate authid in map table: %d",
						authid));
					error_count++;
				}
				if (! hash_enter(_Local(gid_to_authid),
								gid,authid)) {
					ERROR((0,
				"Duplicate gid in map table: %d",
						gid));
					error_count++;
				}
			}
		}

		cp = (char *) index(cp,'\n');
		if (cp != NULL) cp++;
	}

	if (error_count > 0) {
		return(US_INVALID_ARGS);
	} else {
		ret = fs_authid_to_uid(priv_authid, &priv_uid);
		if (ret != ERR_SUCCESS) {
			priv_uid = 0;
			mach_error("Cannot setup the priv uid",ret);
		}
		ret = fs_authid_to_gid(priv_authid, &priv_gid);
		if (ret != ERR_SUCCESS) {
			priv_gid = 0;
			mach_error("Cannot setup the priv gid",ret);
		}

		return(ERR_SUCCESS);
	}
}


/*
 *	Conversion functions, mapping uid & gid's <-> authid's.
 */
mach_error_t fs_access::fs_authid_to_uid(ns_authid_t id, int* uid)
{
	if ((_Local(authid_to_uid) != NULL) &&
	    (*uid = hash_lookup(_Local(authid_to_uid),id))) {
		if (*uid == ZERO_ID) {
			*uid = 0;
		}
	} else {
		*uid = ((id == _Local(root_authid)) ||
			(id == _Local(priv_authid))) ? 0 : id - USER_AUTHID;
	}

	return (ERR_SUCCESS);
}

mach_error_t fs_access::fs_authid_to_gid(ns_authid_t id, int* gid)
{
	if ((_Local(authid_to_gid) != NULL) &&
	    (*gid = hash_lookup(_Local(authid_to_gid),id))) {
		if (*gid == ZERO_ID) {
			*gid = 0;
		}
	} else {
		*gid = ((id == _Local(root_authid)) ||
			(id == _Local(priv_authid))) ? 1 : id - GROUP_AUTHID;
	}

	return (ERR_SUCCESS);
}

mach_error_t fs_access::fs_uid_to_authid(int uid, ns_authid_t* id)
{
	if ((_Local(uid_to_authid) != NULL) &&
	    (*id = hash_lookup(_Local(uid_to_authid),uid))) {
		if (*id == ZERO_ID) {
			*id = 0;
		}
	} else {
		*id = (uid == 0) ? _Local(root_authid) : uid + USER_AUTHID;
	}

	return (ERR_SUCCESS);
}

mach_error_t fs_access::fs_gid_to_authid(int gid, ns_authid_t* id)
{
	if ((_Local(gid_to_authid) != NULL) &&
	    (*id = hash_lookup(_Local(gid_to_authid),gid))) {
		if (*id == ZERO_ID) {
			*id = 0;
		}
	} else {
		*id = ((gid == 1) || (gid == 0)) ? _Local(priv_authid) : gid + GROUP_AUTHID;
	}

	return (ERR_SUCCESS);
}

/*
 * Convert FS mode into access rights.
 *
 * XXX Assume a one-to-one mapping between UNIX rights and NS rights.
 */
ns_access_t fs_access::fs_convert_access_fs2ns(int mode)
{
	ns_access_t		access = NSR_GETATTR;

	if (mode & 1) access |= NSR_LOOKUP | NSR_EXECUTE | NSR_READ;
	if (mode & 2) access |= NSR_INSERT | NSR_DELETE | NSR_WRITE;
	if (mode & 4) access |= NSR_READ;

	return(access);
}

/*
 * Find out the privileged authentication ID that has full access to
 * this object.
 */
mach_error_t fs_access::fs_is_root(ns_authid_t privid)
{
	if (privid == _Local(priv_authid) || privid == _Local(root_authid))
		return(ERR_SUCCESS);
	else
		return(US_INVALID_ACCESS);
}


/*
 * Convert access rights into FS mode.
 *
 * XXX Assume a one-to-one mapping between UNIX rights and NS rights.
 */
int fs_access::fs_convert_access_ns2fs(ns_access_t access)
{
	int		mode = 0;

	access &= ~(NSR_ADMIN | NSR_GETATTR); /* ignore these rights */

	if (access & NSR_EXECUTE) mode |= 1;
	if (access & NSR_LOOKUP) mode |= 1;
	if (access & NSR_INSERT) mode |= 2;
	if (access & NSR_DELETE) mode |= 2;
	if (access & NSR_WRITE) mode |= 2;
	if (access & NSR_READ) mode |= 4;
 
	return(mode);
}


/*
 * Convert standard protection info into FS attributes.
 */
mach_error_t 
fs_access::fs_convert_prot_ns2fs(ns_prot_t prot, unsigned int protlen, 
				  fs_attr_t fs_attr)
{
	int			i;
	int			owner_index = -1;
	int			group_index = -1;
	int			default_index = -1;
	int		uid, gid;

	/*
	 * Identify the entries for owner, group, and default in
	 * the prot structure.
	 */
	if (prot->head.acl_len > 3)
		return(NS_UNSUPPORTED_PROT);
	for (i = 0; i < prot->head.acl_len; i++) {
		if (prot->acl[i].authid == 0) {		/* wildcard */
			if (default_index != -1) return(NS_UNSUPPORTED_PROT);
			default_index = i;
			continue;
		}
		if (prot->acl[i].rights & NSR_ADMIN) {	/* owner */
			if (owner_index != -1) return(NS_UNSUPPORTED_PROT);
			owner_index = i;
			continue;
		}
							/* group */
		if (group_index != -1) return(NS_UNSUPPORTED_PROT);
		group_index = i;
	}

	if ((owner_index == -1) || (group_index == -1)) {
		return(NS_UNSUPPORTED_PROT);
	}

	/*
	 * Store the correct values in the FS attributes structure.
	 */
	(void)fs_authid_to_uid(prot->acl[owner_index].authid,&uid);
	fs_attr->va_uid = uid;

	(void)fs_authid_to_gid(prot->acl[group_index].authid,&gid);
	fs_attr->va_gid = gid;

	fs_attr->va_mode = 0;
	fs_attr->va_mode |= fs_convert_access_ns2fs(
				prot->acl[owner_index].rights) << 6;
	fs_attr->va_mode |= fs_convert_access_ns2fs(
				prot->acl[group_index].rights) << 3;
	if (default_index != -1) {
		fs_attr->va_mode |= fs_convert_access_ns2fs(
				prot->acl[default_index].rights);
	}

#ifdef	notdef

	/*
	 * Fix all the other attributes.
	 */
	fs_attr->va_type = (enum vtype)-1;
	fs_attr->va_fsid = -1;
	fs_attr->va_nodeid = -1;
	fs_attr->va_nlink = -1;
	fs_attr->va_size = -1;
	fs_attr->va_blocksize = -1;
	fs_attr->va_atime.tv_sec = -1;
	fs_attr->va_atime.tv_usec = -1;
	fs_attr->va_mtime.tv_sec = -1;
	fs_attr->va_mtime.tv_usec = -1;
	fs_attr->va_ctime.tv_sec = -1;
	fs_attr->va_ctime.tv_usec = -1;
	fs_attr->va_rdev = -1;
	fs_attr->va_blocks = -1;

#endif	notdef

	return(ERR_SUCCESS);
}


/*
 * Convert FS attributes into standard protection info.
 */
mach_error_t 
fs_access::fs_convert_prot_fs2ns(fs_attr_t fs_attr, ns_prot_t prot, 
				  unsigned int* protlen)
{
	prot->head.version = 1;
	prot->head.generation = 0;
	prot->head.acl_len = 3;

	(void)fs_uid_to_authid((int)fs_attr->va_uid,
						&prot->acl[0].authid);
	prot->acl[0].rights = NSR_ADMIN | fs_convert_access_fs2ns(
					(fs_attr->va_mode & 0700) >> 6);

	(void)fs_gid_to_authid((int)fs_attr->va_gid,
						&prot->acl[1].authid);
	prot->acl[1].rights = fs_convert_access_fs2ns(
					(fs_attr->va_mode & 070) >> 3);

	prot->acl[2].authid = 0;	/* wildcard */
	prot->acl[2].rights = fs_convert_access_fs2ns(
					(fs_attr->va_mode & 07));

	*protlen = NS_PROT_LEN(3);

	return(ERR_SUCCESS);
}


/*
 * Check access by matching the specified FS attributes against
 * the specified FS credentials.
 */
mach_error_t 
fs_access::fs_check_access_from_data(fs_attr_t fs_attr, ns_access_t access, 
				      fs_cred_t fs_cred)
{
	ns_access_t		max_access;
	int			mode;
	int			i;

	/*
	 * Construct the allowed access mode.
	 */
	mode = fs_attr->va_mode & 0777;
	max_access = NSR_REFERENCE;

	/* Fail on write to read-only mounted device */
	if ((NSR_WRITE & access) && (this->dev_mode == O_RDONLY)){
		return(NS_INVALID_ACCESS);
	}

	/* Fail on exec when no exec on file (even root can't) */
	if ((NSR_EXECUTE & access) && (! (mode & 0111))) {
		return(NS_INVALID_ACCESS);
	}

	if (fs_cred->cr_uid == fs_attr->va_uid) {
		mode >>= 6;
		max_access |= NSR_ADMIN;
	} else if (fs_cred->cr_gid == fs_attr->va_gid) {
		mode >>= 3;
	} else {
		i = 0;
		while((fs_cred->cr_groups[i] != 0) && (i < NGROUPS)) {
			if ((fs_cred->cr_groups[i] == fs_attr->va_gid) ||
				(fs_cred->cr_groups[i] == _Local(priv_gid))) {
				mode >>= 3;
				break;
			}
			i++;
		}
	}

	max_access |= fs_convert_access_fs2ns(mode & 07);

	if ((max_access & access) == access) {
		return(ERR_SUCCESS);
	} else {
		 /* Check for UNIX root id. */
		if (fs_cred->cr_uid == _Local(priv_uid)) {
			return(ERR_SUCCESS);
		}

		/*
		 * Check for special privileged group.
		 */
		if (fs_cred->cr_gid == _Local(priv_gid)) {
			return(ERR_SUCCESS);
		}
		i = 0;
		while((fs_cred->cr_groups[i] != 0) && (i < NGROUPS)) {
			if (fs_cred->cr_groups[i] == _Local(priv_gid)) {
				return(ERR_SUCCESS);
			}
			i++;
		}

		return(NS_INVALID_ACCESS);
	}
}


/*
 * Find out the privileged authentication ID that has full access to
 * this object.
 */
mach_error_t fs_access::ns_get_privileged_id(ns_authid_t* privid)
{
	*privid = _Local(priv_authid);
	return(ERR_SUCCESS);
}

/*
 * Check to see if user is root or in the privileged SYSOP group.
 */
mach_error_t fs_access::fs_is_priv(ns_cred_t cred_ptr)
{
	ns_authid_t		cur_authid;
	register int		i;
	mach_error_t		ret;

	if (cred_ptr->authid[0] == root_authid)	{ /* check user */
		return(ERR_SUCCESS);
	}

	for (i = 1; i < cred_ptr->head.cred_len; i++) {	/* check groups */
		cur_authid = cred_ptr->authid[i];

		/*
		 * Check for privileged ID.
		 */
		if (cur_authid == priv_authid) {
			return(ERR_SUCCESS);
		}
	}
	return(US_INVALID_ACCESS);
}
