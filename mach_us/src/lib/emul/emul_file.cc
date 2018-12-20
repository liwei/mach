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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/emul/emul_file.cc,v $
 *
 * Purpose:
 *	user space emulation of unix i/o primitives
 *
 * HISTORY: 
 * $Log:	emul_file.cc,v $
 * Revision 2.9  94/10/27  12:01:28  jms
 * 	Added support for CMU "@sys" (GRM)
 * 	[94/10/26  14:43:08  jms]
 * 
 * Revision 2.8  94/07/08  16:56:53  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.7  94/05/17  13:53:26  jms
 * 	Make change dir follow all links including tsymlinks. -grm
 * 
 * Revision 2.6  94/01/11  17:48:43  jms
 * 	Don't require NSR_EXECUTE for emul_chdir.
 * 	Fix double dereference on eror in emul_mkdir.
 * 	Add net objs to change protection.
 * 	[94/01/09  18:32:08  jms]
 * 
 * Revision 2.5  93/01/20  17:36:41  jms
 * 	Bugfix.  Cleanup a reference to a "link_obj"
 * 	[93/01/18  15:57:45  jms]
 * 
 * Revision 2.4  92/07/05  23:24:56  dpj
 * 	Eliminated diag_format().
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:29:27  dpj]
 * 
 * Revision 2.3  91/12/20  17:43:16  jms
 * 	Fix misc rename bugs.
 * 	Prevent moving directories (didn't work anyway)
 * 	[91/12/20  14:39:56  jms]
 * 
 * Revision 2.2  91/11/06  11:29:42  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:28:09  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:23:58  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:23:38  pjg]
 * 
 * Revision 1.29  91/05/05  19:24:18  dpj
 * 	Removed NSR_INSERT and NSR_DELETE from resolve operation
 * 	in emul_truncate().
 * 	Merged up to US39.
 * 	[91/04/30            dpj]
 * 	Follow transparent symlinks.
 * 	Added transparent symlink and new pipes to open() logic.
 * 	[91/04/28  09:42:38  dpj]
 * 
 * Revision 1.28  91/04/12  18:47:02  jjc
 * 	Made some improvements/modifications to Paul Neves' changes:
 * 		1) Replaced Paul Neves' EFAULT handling with macros for copying
 * 		   arguments in and out of system calls to figure out whether
 * 		   they're good or not.
 * 		2) Use emul_path() in place of path_lengths_ok() and path()
 * 		   to check path name length and split the path name into 
 * 		   directory and leaf names.
 * 		3) Need not check for long path names before calling
 * 		   ns_resolve_obj() because ns_resolve_obj() has been fixed 
 * 		   to check.
 * 	[91/04/01            jjc]
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:46:39  jjc]
 * 
 * Revision 1.27  90/12/10  09:49:16  jms
 * 	Initialized local variable for chmod problem.
 * 	[90/11/19  18:13:40  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:01  jms]
 * 
 * Revision 1.26  90/11/10  00:37:55  dpj
 * 	Replaced ns_set_attributes() with ns_set_times().
 * 	[90/11/08  22:16:01  dpj]
 * 
 * 	Added code for utimes system call emulation.
 * 	[90/10/24  14:25:04  neves]
 * 
 * 	Initialized local variable to prevent havoc.
 * 	[90/10/18  17:03:08  neves]
 * 
 * 	Added emul_fchmod and emul_fchown routines.
 * 	Collapsed [f]chmod, [f]chown, and chgrp routines.
 * 	Added code to detect invalid operations on uxio objects.
 * 	[90/10/17  12:42:53  neves]
 * 
 * Revision 1.25  90/01/02  21:39:44  dorr
 * 	do protection with uxstat, no uxprot.
 * 
 * Revision 1.24.1.2  90/01/02  14:07:57  dorr
 * 	get rid of access_obj.  do protection conversions
 * 	with uxstat object, not uxprot object.
 * 
 * Revision 1.24.1.1  89/12/18  15:42:31  dorr
 * 	initial checkin.
 * 
 * Revision 1.24  89/06/30  18:31:00  dpj
 * 	Added NSF_MOUNT wherever needed.
 * 	Removed conversion between ns_attr and stat structure,
 * 	now done in the uxstat object.
 * 	Fixed emul_readlink() to not follow symlinks, to return
 * 	the length of the link, and to correctly zero-terminate it.
 * 	[89/06/29  00:04:59  dpj]
 * 
 * Revision 1.23  89/06/05  17:17:46  dorr
 * 	add missing gid arg to chown
 * 	[89/06/05  14:01:14  dorr]
 * 
 * Revision 1.22  89/05/17  16:13:20  dorr
 * 	include file cataclysm
 * 
 * Revision 1.21.1.1  89/05/15  12:02:19  dorr
 * 	get rid of emul_initialize() calls, make the uxprot object
 * 	global, do some random cleanup.
 * 
 * Revision 1.21  89/03/17  12:23:37  sanzi
 * 	fix emul_link bugs.
 * 	[89/03/10  16:19:16  dorr]
 * 	
 * 	code around lack of ns_rename().
 * 	[89/03/10  14:43:04  dorr]
 * 	
 * 	fix access to ns_resolve for file permission.
 * 	fix args to uxprot calls.
 * 	[89/03/08  14:39:17  dorr]
 * 	
 * 	fix args to std_to_unix_prot.
 * 	[89/03/07  19:50:36  dorr]
 * 	
 * 	add uxprot stuff.
 * 	[89/03/02  21:06:54  dorr]
 * 	
 * 	Fix parameter declarations in emul_truncate().
 * 	[89/03/02  16:08:11  sanzi]
 * 	
 * 	fix new calling convention for DLONG's
 * 	[89/03/02  15:35:42  dorr]
 * 	
 * 	reinstate the wonderful world of uxprot's
 * 	[89/03/02  14:50:30  dorr]
 * 	
 * 	switch to user side mach objects.
 * 	[89/02/25  13:52:17  dorr]
 * 
 *
 */
#include <us_name_ifc.h>
#include <us_byteio_ifc.h>
#include <uxio_ifc.h>
#include <uxio_dir_ifc.h>
#include <uxio_pipe_ifc.h>
#include <uxio_tty_ifc.h>
#include <uxio_socket_ifc.h>
#include <ftab_ifc.h>

#include <uxstat_ifc.h>

#include "emul_base.h"

extern "C" {
#include <mach.h>

#include <sys/errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <machine/us_param.h> /* Definition of "@sys" */
}

extern mach_error_t emul_path();

fs_access* access_obj;

#if	FileOps


mach_error_t 
emul_chdir(char *path_name, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem*		obj = 0;
        ns_access_t		access = NSR_GETATTR;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname;

	SyscallTrace("chdir");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);

	access = NSR_LOOKUP;
//	access = NSR_LOOKUP | NSR_EXECUTE;
	err = prefix_obj->ns_resolve_fully(pathname, NSF_FOLLOW_ALL,
					    access, &obj, &type, NULL);

	if (err != ERR_SUCCESS) goto finish;

	if (type != NST_DIRECTORY) {
		err = unix_err(ENOENT);
		goto finish;
	}

	/* change the cwd prefix */
	if ( err = prefix_obj->ns_set_user_prefix("",pathname) ) {
		DEBUG0(1,(Diag,"ns_define_user_prefix (cwd) failed... %x, %s\n",
				  err,
				  mach_error_string(err)));
	}

    finish:
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return err;

}

mach_error_t 
emul_mkdir(char *path_name, int mode, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem		*a_obj   = 0;
	usName		*dir_obj = 0;
//	mach_object_t		obj = NULL;
//	mach_object_t		dir = NULL;
	ns_name_t		leafname;
	ns_path_t		dirpath;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname;

	SyscallTrace("mkdir");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);

	err = emul_path(pathname, dirpath, sizeof(ns_path_t),
			leafname, sizeof(ns_name_t));
	if (err != ERR_SUCCESS) goto finish;
	
	err = prefix_obj->ns_resolve_fully(dirpath, NSF_FOLLOW_ALL,
					   NSR_INSERT, &a_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	if (type != NST_DIRECTORY) {
		err = NS_NOT_DIRECTORY;
		goto finish;
	}
	if ((dir_obj = usName::castdown(a_obj)) == 0) {
		DEBUG0 (emul_debug, (0, "emul_mkdir\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}
	/*
	 * Support for CMU "@sys".
	 */
	if (strcmp("@sys", leafname) == 0) {
		bcopy(ATSYS_STRING, leafname, ATSYS_STRING_LEN);
		leafname[ATSYS_STRING_LEN] = '\000';
	}
	a_obj = NULL;
	err = dir_obj->ns_create(leafname, NST_DIRECTORY, umask_prot,
				  umask_protlen, NSR_NONE, &a_obj);

	if (err != ERR_SUCCESS) {
		goto finish;
	}

    finish:
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(dir_obj);
	mach_object_dereference(a_obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return err;
}

mach_error_t
emul_symlink(char *name1, char *name2, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem		*a_obj   = 0;
	usName		*dir_obj = 0;
//	mach_object_t		dir = NULL;
	ns_name_t		leafname;
	ns_path_t		dirpath;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		from, to;

	SyscallTrace("symlink");

	COPYINSTR(name2, from, sizeof(ns_path_t), rv, EFAULT);
	COPYINSTR(name1, to, sizeof(ns_path_t), rv, EFAULT);
	
	err = emul_path(from, dirpath, sizeof(ns_path_t),
			leafname, sizeof(ns_name_t));
	if (err != ERR_SUCCESS) goto finish;
	
	err = prefix_obj->ns_resolve_fully(dirpath, NSF_FOLLOW_ALL,
					   NSR_INSERT, &a_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	if (type != NST_DIRECTORY) {
		err = NS_NOT_DIRECTORY;
		goto finish;
	}
	if ((dir_obj = usName::castdown(a_obj)) == 0) {
		DEBUG0 (emul_debug, (0, "emul_symlink\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}

	/* XXX should ns_expand name1 for safety ? */
	err = dir_obj->ns_insert_forwarding_entry(leafname, umask_prot,
						  umask_protlen,
						  NULL, to);

	if (err == NS_ENTRY_EXISTS)
		err = unix_err(EEXIST);
	else
	if (err != ERR_SUCCESS) {
		goto finish;
	}


    finish:
	COPYINSTR_DONE(name2, from, sizeof(ns_path_t), rv, EFAULT);
	COPYINSTR_DONE(name1, to, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(dir_obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;
	return err;
}

static mach_error_t
unlink_internal(boolean_t rmdir, char *path_name, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem		*a_obj   = 0;
	usItem		*b_obj   = 0;
	usName		*dir_obj = 0;
//	mach_object_t		obj = NULL;
//	mach_object_t		dir = NULL;
	ns_name_t		leafname;
	ns_path_t		dirpath;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname;

	SyscallTrace("unlink internal");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);

	if (rmdir) {
		/* make sure the leaf is a directory */
		err = prefix_obj->ns_resolve_fully(pathname, 
						   NSF_FOLLOW_ALL,
						   NSR_NONE,&a_obj,&type,NULL);

		if (err != ERR_SUCCESS) {
			goto finish;
		}
		
		if (type != NST_DIRECTORY) {
			err = NS_NOT_DIRECTORY;
			goto finish;
		}
	}

	err = emul_path(pathname, dirpath, sizeof(ns_path_t),
			leafname, sizeof(ns_name_t));
	if (err != ERR_SUCCESS) goto finish;
	
	/* degenerate case */
	if (leafname[0] == '.' && leafname[1] == '\0')
		goto finish;

	err = prefix_obj->ns_resolve_fully(dirpath, NSF_FOLLOW_ALL,
					   NSR_DELETE, &b_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	if (type != NST_DIRECTORY) {
		err = NS_NOT_DIRECTORY;
		goto finish;
	}
	if ((dir_obj = usName::castdown(b_obj)) == 0) {
	        DEBUG0 (emul_debug, (0, "unlink internal\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}
	err = dir_obj->ns_remove_entry(leafname);

	if (err != ERR_SUCCESS) {
		goto finish;
	}

    finish:
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(dir_obj);
	mach_object_dereference(a_obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;
	return err;

}

mach_error_t
emul_unlink(char *pathname, syscall_val_t *rv)
{
	return unlink_internal(FALSE, pathname, rv);
}

mach_error_t
emul_rmdir(char *pathname, syscall_val_t *rv)
{
	return unlink_internal(TRUE, pathname, rv);
}


mach_error_t
stat_internal(boolean_t follow, char *path_name, struct stat *buf, 
	      syscall_val_t *rv)
{
	int			attrlen = 0;
	mach_error_t		err = ERR_SUCCESS;
	usItem		*a_obj   = 0;
//	mach_object_t		obj = NULL;
	ns_type_t		type = NST_INVALID;
	struct ns_attr		ns_attrs;
	copyinstr_t		pathname;
	struct stat		*statbuf;

	SyscallTrace("stat internal");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	COPYOUT_INIT(statbuf, buf, 1, struct stat, rv, EFAULT);

	/* find an existing object */
	err = prefix_obj->ns_resolve_fully(pathname,
			(NSF_MOUNT | NSF_TFOLLOW) | (follow ? NSF_FOLLOW : 0),
			NSR_GETATTR, &a_obj, &type, NULL);
	DEBUG2(emul_debug,(0, "stat_internal: ns_resolve_fully(path=\"%s\") -> err =0x%x\n", pathname, err));

	if (err) goto finish;


	attrlen = sizeof(ns_attrs) / sizeof(int);
	err = a_obj->ns_get_attributes(&ns_attrs, &attrlen);
	DEBUG2(emul_debug,(0, "stat_internal: ns_get_attributes -> err =0x%x\n", err));

	if (err) goto finish;

	err = uxstat_obj->uxstat_std_to_unix_attr(&ns_attrs,statbuf);

    finish:
	COPYOUT(statbuf, buf, 1, struct stat, rv, EFAULT);
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(a_obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return( err );
}

mach_error_t
emul_stat(char *pathname, struct stat *buf, syscall_val_t *rv)
{
	return stat_internal(TRUE, pathname, buf, rv);
}

mach_error_t
emul_lstat(char *pathname, struct stat *buf, syscall_val_t *rv)
{
	return stat_internal(FALSE, pathname, buf, rv);
}

mach_error_t
emul_ostat(char *pathname, struct stat *buf, syscall_val_t *rv)
{
	/* XXX what's this? */
	rv->rv_val1 = EIO;
	return -1;
}

mach_error_t
emul_link(char *path_name1, char *path_name2, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem		*a_obj   = 0;
	usItem		*b_obj   = 0;
	usName		*dir_obj = 0;
//	mach_object_t		dir = NULL;
//	mach_object_t		new_obj = NULL;
	ns_name_t		leafname;
	ns_path_t		dirname;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname1, pathname2;

	SyscallTrace("link");

	COPYINSTR(path_name1, pathname1, sizeof(ns_path_t), rv, EFAULT);
	COPYINSTR(path_name2, pathname2, sizeof(ns_path_t), rv, EFAULT);

	err = emul_path(pathname2, dirname, sizeof(ns_path_t),
			leafname, sizeof(ns_name_t));
	if (err != ERR_SUCCESS) goto finish;
	
	/*
	 * Find the file to link to.
	 */
	err = prefix_obj->ns_resolve_fully(pathname1, NSF_FOLLOW_ALL,
					  NSR_ADMIN, &a_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	/*
	 * Find the directory in which to put the link.
	 */
	path(pathname2,dirname,leafname);
	err = prefix_obj->ns_resolve_fully(dirname, NSF_FOLLOW_ALL,
					    NSR_INSERT, &b_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	if (type != NST_DIRECTORY) {
		err = NS_NOT_DIRECTORY;
		goto finish;
	}
	if ((dir_obj = usName::castdown(b_obj)) == 0) {
		DEBUG0 (emul_debug, (0, "emul_link\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}

	err = dir_obj->ns_insert_entry(leafname,a_obj);

	if (err == NS_ENTRY_EXISTS)
		err = unix_err(EEXIST);
	else
	if (err != ERR_SUCCESS) {
		goto finish;
	}


    finish:
	COPYINSTR_DONE(path_name1, pathname1, sizeof(ns_path_t), rv, EFAULT);
	COPYINSTR_DONE(path_name2, pathname2, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(dir_obj);
	mach_object_dereference(a_obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return err;
}

static mach_error_t
change_protection_internal(boolean_t use_fd, char *pathname, int fd, int uid, int gid, int mode, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem*		obj = 0;
	uxio*		temp_obj = 0;
//	mach_object_t		temp_obj = NULL;
//	mach_object_t		obj = NULL;
	ns_access_t		access = NSR_GETATTR|NSR_ADMIN;
	ns_action_t		action = NSF_FOLLOW_ALL;
	ns_type_t		type = NST_INVALID;

	if (use_fd) {		/* Already have file handle? */
	  
		err = ftab_obj->ftab_get_obj(fd,&temp_obj);
		if (err != ERR_SUCCESS) goto finish;

	} else {			/* No, get a file handle. */

		err = prefix_obj->ns_resolve_fully(pathname, action, access,
						    &obj, &type, NULL);
		if (err != ERR_SUCCESS) {
			goto finish;
		}

		switch(type) {
		  case NST_DIRECTORY:
//			new_object(temp_obj, uxio_dir);
			temp_obj = new uxio_dir;
			break;
		  case NST_UPIPE_BYTES:
//			new_object(temp_obj, uxio_pipe);
			temp_obj = new uxio_pipe;
			break;

		  case NST_TTY:
//			new_object(temp_obj, uxio_tty);
			temp_obj = new uxio_tty;
			break;

		  case NST_FILE:
		  case NST_SYMLINK:
		  case NST_TRANSPARENT_SYMLINK:
		  case NST_MOUNTPT:
		  case NST_CLTS_BYTES: /* XXX Whynot uxio_socket, to complex */
		  case NST_CLTS_RECS:
		  case NST_COTS_BYTES:
		  case NST_COTS_RECS:
		  case NST_CONNECTOR:
//			new_object(temp_obj, uxio);
			temp_obj = new uxio;
			break;
		  case NST_INVALID:
		  default:
			err = unix_err(EINVAL);
			goto finish;
		}
		/*
		 * XXX C++ "obj" should be of type usItem or usByteIO ?
		 */
		err = temp_obj->ux_open(obj, 0, 0);
		if (err != ERR_SUCCESS) goto finish;
	}

	err = temp_obj->ux_modify_protection(uid, gid, mode);

	/* Some Unix "objects" don't allow this operation (i.e. sockets) */
	if (err == MACH_OBJECT_NO_SUCH_OPERATION)
	  err = unix_err(EINVAL);

    finish:
	mach_object_dereference(obj);
	mach_object_dereference(temp_obj);
	if (err != ERR_SUCCESS)
		rv->rv_val1 = emul_error_to_unix(err);
        else
		rv->rv_val1 = ESUCCESS;

	return err;
}

mach_error_t
emul_fchmod(int fd, int mode, syscall_val_t *rv)
{
        char                  * pathname = NULL; /* use file descriptor */
	int			gid = -1;	/* use current gid */
  	int			uid = -1;	/* use current uid */

	SyscallTrace("fchmod");
	return change_protection_internal(TRUE,pathname,fd,uid,gid,mode,rv);
}

mach_error_t
emul_chmod(char *path_name, int mode, syscall_val_t *rv)
{
	int			fd  = -1;       /* use pathname */
  	int			gid = -1;	/* use current gid */
  	int			uid = -1;	/* use current uid */
	copyinstr_t		pathname;
	mach_error_t		err;

	SyscallTrace("chmod");
	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	err = change_protection_internal(FALSE,pathname,fd,uid,gid,mode,rv);
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	return(err);
}


mach_error_t
emul_fchown(int fd, int uid, int gid, syscall_val_t *rv)
{
	char		      * pathname = NULL;
	int			mode = -1;  /* use current mode */

	SyscallTrace("fchown");
	return change_protection_internal(TRUE,pathname,fd,uid,gid,mode,rv);
}

mach_error_t
emul_chown(char *path_name, int uid, int gid, syscall_val_t *rv)
{
	int			fd = -1;
	int			mode = -1;  /* use current mode */
	copyinstr_t		pathname;
	mach_error_t		err;

	SyscallTrace("chown");
	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	err = change_protection_internal(FALSE,pathname,fd,uid,gid,mode,rv);
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	return(err);

}

mach_error_t
emul_chgrp(char *path_name, int gid, syscall_val_t *rv)
{
	int			fd = -1;
	int			mode = -1;  /* use current mode */
	int			uid = -1;   /* use current uid */
	copyinstr_t		pathname;
	mach_error_t		err;

	SyscallTrace("chgrp");
	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	err = change_protection_internal(FALSE,pathname,fd,uid,gid,mode,rv);
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	return(err);
}

mach_error_t
emul_access(char *path_name, int mode, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem*		a_obj = 0;
//	mach_object_t		obj = NULL;
	ns_access_t		access = NSR_GETATTR;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname;

	SyscallTrace("access");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);

	if (mode & X_OK)
		access |= NSR_EXECUTE | NSR_LOOKUP;
	if (mode & W_OK)
		access |= NSR_INSERT | NSR_DELETE | NSR_WRITE;
	if (mode & R_OK)
		access |= NSR_READ;

	err = prefix_obj->ns_resolve_fully(pathname, 
					   NSF_FOLLOW_ALL | NSF_ACCESS,
					   access, &a_obj, &type, NULL);
    finish:
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(a_obj);
	if (err != ERR_SUCCESS)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return err;
}

mach_error_t
emul_truncate(char *path_name, unsigned int len, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem*		a_obj = 0;
	usByteIO*		f_obj = 0;
//	mach_object_t		obj = NULL;
	ns_access_t		access = NSR_GETATTR;
	ns_size_t		size;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname;

	SyscallTrace("truncate");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);

	/*
	 * XXX io_set_size should only need UXR_WRITE access
	 * but because of the way it's implemented, it also
	 * requires NSR_READ access. -- Argh!
	 */

	access |= NSR_WRITE | /*XXX*/ NSR_READ;
	err = prefix_obj->ns_resolve_fully(pathname, NSF_FOLLOW_ALL,
					   access, &a_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	if (type == NST_DIRECTORY) {
	    err = unix_err(EISDIR);
	    goto finish;
	}

	if ((f_obj = usByteIO::castdown(a_obj)) == 0) {
		DEBUG0 (emul_debug, (0, "emul_truncate\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}
	U_INT_TO_DLONG(&size,len);
	err = f_obj->io_set_size(size);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

    finish:
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(a_obj);
	if (err != ERR_SUCCESS)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return err;
}

mach_error_t
emul_rename(char *path_name1, char *path_name2, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem*		a_obj   = 0;
	usItem*		b_obj   = 0;
	usName*		newdir_obj   = 0;
	usName*		olddir_obj   = 0;
//	mach_object_t		newdir = NULL;
//	mach_object_t		olddir = NULL;
	ns_name_t		newleafname;
	ns_name_t		oldleafname;
	ns_path_t		newdirpath;
	ns_path_t		olddirpath;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname1, pathname2;

	SyscallTrace("rename");

	COPYINSTR(path_name1, pathname1, sizeof(ns_path_t), rv, EFAULT);
	COPYINSTR(path_name2, pathname2, sizeof(ns_path_t), rv, EFAULT);

	err = emul_path(pathname1, olddirpath, sizeof(ns_path_t),
			oldleafname, sizeof(ns_name_t));
	if (err != ERR_SUCCESS) goto finish;

	err = emul_path(pathname2, newdirpath, sizeof(ns_path_t),
			newleafname, sizeof(ns_name_t));
	if (err != ERR_SUCCESS) goto finish;

	/*
	 * Find the file we're renaming
	 */
	err = prefix_obj->ns_resolve_fully(olddirpath, NSF_FOLLOW_ALL,
					    NSR_DELETE, &a_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}
	if (type != NST_DIRECTORY) {
		err = NS_NOT_DIRECTORY;
		goto finish;
	}
	if ((olddir_obj = usName::castdown(a_obj)) == 0) {
		DEBUG0 (emul_debug, (0, "emul_rename\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}

	/*
	 * Find the directory in which to put the link.
	 */
	path(pathname2,newdirpath,newleafname);
	err = prefix_obj->ns_resolve_fully(newdirpath, NSF_FOLLOW_ALL,
			    NSR_INSERT|NSR_DELETE, &b_obj, &type, NULL);
			    /* XXX Only need NSR_DELETE when target exists */
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	if (type != NST_DIRECTORY) {
		err = NS_NOT_DIRECTORY;
		goto finish;
	}

#if	ns_rename
	/* link the new name to the existing object */
	err = olddir_obj->ns_rename_entry(oldleafname, b_obj, newleafname);

	if (err == NS_ENTRY_EXISTS)
		err = unix_err(EEXIST);
	else
	if (err != ERR_SUCCESS) {
		goto finish;
	}
#else
	{
		usItem		*c_obj = 0;
//		mach_object_t		obj;
		/* link the new name to the existing object */
		err = prefix_obj->ns_resolve_fully(pathname1,
					0,NSR_REFERENCE,
					&c_obj, &type, NULL);
		if (err != ERR_SUCCESS) goto finish;
		if (NST_DIRECTORY == type) {
			us_internal_error("rename(directory)",
					  US_NOT_IMPLEMENTED);
			err = US_NOT_IMPLEMENTED;
			goto finish;
		}

		if ((newdir_obj = usName::castdown(b_obj)) == 0) {
			DEBUG0 (emul_debug, (0, "emul_rename 2\n"));
			err = MACH_OBJECT_NO_SUCH_OPERATION;
			goto finish;
		}

		(void)newdir_obj->ns_remove_entry(newleafname);

		err = newdir_obj->ns_insert_entry(newleafname, c_obj);
		mach_object_dereference(c_obj);

		if (err == NS_ENTRY_EXISTS) {
			err = unix_err(EEXIST);
		}
		if (err != ERR_SUCCESS) {
			goto finish;
		}
	
		err = olddir_obj->ns_remove_entry(oldleafname);
		if (err != ERR_SUCCESS) {
			(void)newdir_obj->ns_remove_entry(newleafname);
			goto finish;
		}

	}
#endif	ns_rename
	

    finish:
	COPYINSTR_DONE(path_name1, pathname1, sizeof(ns_path_t), rv, EFAULT);
	COPYINSTR_DONE(path_name2, pathname2, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(a_obj);
	mach_object_dereference(b_obj);
	if (err != ERR_SUCCESS)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return err;
}

mach_error_t
emul_readlink(char *path_name, char *buffer,
	      unsigned int bufsize, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem		*a_obj = 0;
	usItem		*b_obj = 0;
	usName		*link_obj = 0;
//	mach_object_t		mp = NULL;	/* misc mount point */
//	mach_object_t		obj = NULL;	/* link */
	ns_path_t		path;
	ns_type_t		type = NST_INVALID;
	unsigned int		len = 0;
	char			*buf;
	copyinstr_t		pathname;

	SyscallTrace("readlink");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	COPYOUT_INIT(buf, buffer, bufsize, char, rv, EFAULT);

	/*
	 * Find the link
	 */
	err = prefix_obj->ns_resolve_fully(pathname, NSF_MOUNT,
					    NSR_READ, &a_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	if (type != NST_SYMLINK) {
	        err = unix_err(EINVAL);
		goto finish;
	}
	if ((link_obj = usName::castdown(a_obj)) == 0) {
		DEBUG0 (emul_debug, (0, "emul_readlink\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}

	path[0] = '\0';
	err = link_obj->ns_read_forwarding_entry(&b_obj, path);
	if (err != ERR_SUCCESS) goto finish;

	len = strlen(path);
	bcopy(path, buf, len + 1);

	mach_object_dereference(b_obj);

    finish:
	COPYOUT(buf, buffer, bufsize, char, rv, EFAULT);
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(a_obj);
	if (err != ERR_SUCCESS)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = len;

	return err;
}

mach_error_t
emul_utimes(char *path_name, struct timeval *tptr, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	usItem		*a_obj = 0;
//	mach_object_t		obj = NULL;
	ns_type_t		type = NST_INVALID;
	time_value_t		atime;
	time_value_t		mtime;
	copyinstr_t		pathname;
	struct timeval		*tvp;

	SyscallTrace("utimes");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	COPYIN(tptr, tvp, 2,  struct timeval, rv, EFAULT);

	/*
	 * Find the File.
	 */
	err = prefix_obj->ns_resolve_fully(pathname, NSF_FOLLOW_ALL,
					    NSR_ADMIN, &a_obj, &type, NULL);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	atime.seconds = tvp[0].tv_sec;
	atime.microseconds = tvp[0].tv_usec;
	mtime.seconds = tvp[1].tv_sec;
	mtime.microseconds = tvp[1].tv_usec;

	/*
	 * XXX C++ ns_set_times does not exist ??
	 */
	err = a_obj->ns_set_times(atime, mtime);

    finish:
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	COPYIN_DONE(tptr, tvp, 2, struct timeval, rv, EFAULT);
	mach_object_dereference(a_obj);
	if (err != ERR_SUCCESS)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return err;
}

#endif	FileOps
