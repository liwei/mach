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
 * File: emul_io.c
 *
 * Purpose:
 *	user space emulation of unix i/o primitives
 *
 * HISTORY: 
 * $Log:	emul_io.cc,v $
 * Revision 2.8  94/07/08  16:57:00  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.7  94/01/11  17:48:50  jms
 * 	Implement "real" select emulation.
 * 	[94/01/09  18:37:32  jms]
 * 
 * Revision 2.6  92/07/05  23:25:07  dpj
 * 	No changes.
 * 	[92/06/29  22:44:34  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:31:31  dpj]
 * 
 * 	Removed dead code.
 * 	[92/03/12  13:46:19  dpj]
 * 
 * Revision 2.5  92/03/05  14:55:44  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:16:43  jms]
 * 
 * Revision 2.4  91/12/20  17:43:20  jms
 * 	Bug fix for close-on-exec. -DPJ
 * 	[91/12/20  14:46:11  jms]
 * 
 * Revision 2.3  91/11/13  16:39:57  dpj
 * 	Cleaned-up. Removed some compiler warnings. Use new IPC when appropriate.
 * 	[91/11/12  17:47:08  dpj]
 * 
 * Revision 2.2  91/11/06  11:29:55  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:31:01  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:25:08  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:24:12  pjg]
 * 
 * Revision 1.52  91/10/06  22:26:40  jjc
 * 	Changed uses of /pipenet to /server/pipenet.
 * 	[91/07/22            jjc]
 * 
 * Revision 1.51  91/07/01  14:06:34  jms
 * 	Waste ns_[de]reference.
 * 	[91/06/24  16:02:12  jms]
 * 
 * Revision 1.50  91/05/05  19:24:32  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:50:40  dpj]
 * 
 * 	Follow transparent symlinks.
 * 	Added new pipes and sockets in stat() logic.
 * 	Modified pipe() to use the pipenet server.
 * 	Added a very stupid select() copied from POE.
 * 	[91/04/28  09:45:20  dpj]
 * 
 * Revision 1.49  91/04/12  18:47:11  jjc
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
 * 		4) Changed Paul Neves' new version of copen() to set read
 * 		   access for files opened write only because writes will fail
 * 		   without read access when you try to get the mapped file 
 * 		   state.
 * 		5) Changed copen() to test to make sure that a file opened
 * 		   in truncate or append mode is writeable.
 * 	[91/04/01            jjc]
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:47:56  jjc]
 * 
 * Revision 1.48.1.6  91/02/05  12:11:32  neves
 * 	Initialized local variables
 * 
 * Revision 1.48.1.5  91/02/05  11:37:06  neves
 * 	Fixed bug in convert_access_to_rights which promoted
 * 	execute access into read and execute access.
 * 
 * Revision 1.48.1.4  91/02/04  18:48:30  neves
 * 	Emul_write[v] now generate SIGPIPE signals.
 * 
 * Revision 1.48.1.3  91/02/04  18:39:05  neves
 * 	Added SyscallTrace calls to emul_smmap and emul_munmap.
 * 
 * Revision 1.48.1.2  91/02/04  18:28:20  neves
 * 	Cleanup the copen routine and checked path lengths.
 * 
 * Revision 1.48.1.1  91/02/04  18:16:59  neves
 * 	Added VALD_ADDRESS macros to handle EFAULT.
 * 
 * Revision 1.48  90/12/10  09:49:28  jms
 * 	Added emul_smmap and emul_munmap syscall code.
 * 	[90/11/20  12:28:34  neves]
 * 
 * 	Fixed the implementation of emul_fcntl.
 * 	[90/11/19  18:23:29  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:07  jms]
 * 
 * Revision 1.47  90/11/27  18:18:02  jms
 * 	Remove reference to "environ".  Merge some stuff from trunk. Misc.
 * 	[90/11/20  11:26:42  jms]
 * 
 * 	Prepare to merge some changes from US31
 * 	[90/11/12  16:34:44  jms]
 * 
 * 	About to add tty changes from trunk
 * 	[90/11/08  13:39:04  jms]
 * 
 * 	Change for pure debug printf stuff
 * 	[90/08/20  17:16:59  jms]
 * 
 * Revision 1.46  90/11/10  00:38:01  dpj
 * 	Added code implementing writev and readv syscalls.
 * 	[90/10/24  14:29:13  neves]
 * 
 * 	Added code to create appropriate uxio-like object.
 * 	[90/10/17  12:46:23  neves]
 * 
 * Revision 1.45  90/09/05  09:44:55  mbj
 * 	Corrected longstanding typo resulting in loops on failing opens.
 * 	[90/09/04  14:54:53  mbj]
 * 
 * Revision 1.44  90/07/09  14:23:35  dorr
 * 	no change.
 * 	[90/03/01  14:43:42  dorr]
 * 
 * 	add clone_master support.  move fork support
 * 	into clone_master.  switch to DEBUG[012].
 * 	[90/01/11  11:30:45  dorr]
 * 	Add emul_getdirentries.
 * 	[90/07/06  14:41:04  jms]
 * 
 * Revision 1.43  90/03/21  17:20:07  jms
 * 	Get changes from dorr branch and objectified Task Master objects added to 
 * 	clone list.
 * 	[90/03/16  16:24:03  jms]
 * 
 * Revision 1.42  90/01/02  22:27:36  dorr
 * 	move initialization out.  update to pipe code.
 * 	redo identity establishment (umask/prefix table
 * 	re-initialization).
 * 
 * Revision 1.41.1.3  90/01/02  14:10:22  dorr
 * 	fix emul_io_change_identity to take identity as
 * 	a uxident instead of a token.  token is extracted
 * 	from the uxident, as are the authid's used to set
 * 	up our umask.  should we have a umask object or
 * 	use uxident to do our umasking?
 * 
 * Revision 1.41.1.2  89/12/19  17:04:58  dorr
 * 	checkin before christmas
 * 
 * Revision 1.41.1.1  89/12/18  15:48:15  dorr
 * 	Add dan's pipe stuff.  Conditionalize some stuff.
 * 
 * Revision 1.41.1.3  90/01/02  14:10:22  dorr
 * 	fix emul_io_change_identity to take identity as
 * 	a uxident instead of a token.  token is extracted
 * 	from the uxident, as are the authid's used to set
 * 	up our umask.  should we have a umask object or
 * 	use uxident to do our umasking?
 *
 * Revision 1.41.1.2  89/12/19  17:04:58  dorr
 * 	checkin before christmas
 * 
 * Revision 1.41.1.1  89/12/18  15:48:15  dorr
 * 	Add dan's pipe stuff.  Conditionalize some stuff.
 * 
 * Revision 1.40.2.1  89/12/19  16:07:45  jms
 * 	New taskmaster stuff
 * 
 * Revision 1.36.1.2  89/07/10  15:39:16  dorr
 * 	get rid of mf_initialize
 * 
 * Revision 1.36.1.1  89/06/21  15:53:22  dorr
 * 	make file table an object.
 * 
 * Revision 1.35.1.1  89/05/15  12:03:45  dorr
 * 	establish our initial identity using uxprot object.
 * 	get rid of some unneeded tty state.
 * 
 */
#include <emul_io_ifc.h>

#if	(ComplexIO || FileOps)

#include <kio_ifc.h>
#include <select_ifc.h>
#include <ftab_ifc.h>
#endif	Complexio || FileOps

#include <us_name_ifc.h>
#include <us_byteio_ifc.h>
#include <uxio_ifc.h>
#include <uxio_dir_ifc.h>
#include <uxio_pipe_ifc.h>
#include <uxio_tty_ifc.h>
#include <diag_ifc.h>
#include "emul_base.h"

	extern "C" {
#if	(ComplexIO || FileOps)
		
#include <mach/machine/vm_param.h>
		
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <auth_defs.h>
#include <us_ports.h>
		
#endif	Complexio || FileOps
		
#include <cthreads.h>
#include <mach_error.h>
}

extern mach_error_t do_self_signal(int);


#if	ComplexIO

int		umask_prot_data[DEFAULT_NS_PROT_LEN];
ns_prot_t	umask_prot = (ns_prot_t)umask_prot_data;
int		umask_protlen = sizeof(umask_prot_data) / sizeof(int);

static ns_access_t convert_access_to_rights(int);

ftab*		ftab_obj;	/* file table */
std_name*	prefix_obj;	/* name space */
uxident*	uxident_obj;	/* authentication */
fs_access*	fs_access_obj;	/* uid mapping */
uxstat*		uxstat_obj;	/* stat ?! */
diag*		diag_obj;	/* XXX this is an unstable, temp hack*/

#define INIT_UMASK	022

void emul_io_init(void)
{
	/*
	 * get a file table
	 */
	int emul_io_set_umask(int);

	DEBUG2(emul_debug,(Diag,"emul_io_init\n"));

	ftab_obj = new ftab;
	clone_master_obj->list_add(ftab_obj);

	emul_io_set_umask(INIT_UMASK);
}

/*
 *	umask manipulation
 */

/*
 *	emul_io_umask():  take a unix creation mode and turn it
 *	into a protection, applying the current umask
 */
mach_error_t emul_io_umask(int mode, ns_prot_t prot, int* protlen)
{
	bcopy(umask_prot,prot,NS_PROT_SIZE(umask_prot));
	prot->acl[0].rights &= convert_access_to_rights( (mode & 0700) >> 6 ) | NSR_ADMIN;
	prot->acl[1].rights &= convert_access_to_rights( (mode & 070) >> 3 );
	prot->acl[2].rights &= convert_access_to_rights( mode & 07 );

	*protlen = NS_PROT_LEN(3);

	return(ERR_SUCCESS);
}


static ns_access_t
convert_access_to_rights(int mode)
{
	ns_access_t		access = NSR_GETATTR;

	if (mode & 1) access |= NSR_LOOKUP | NSR_EXECUTE;
	if (mode & 2) access |= NSR_INSERT | NSR_DELETE | NSR_WRITE;
	if (mode & 4) access |= NSR_READ;

	return access;
}


/*
 *	emul_io_set_umask():  modify the current umask value
 */
int
emul_io_set_umask(int mask)
{
	static int	umask = INIT_UMASK;
	int		old_mask;

	old_mask = umask;
	umask = mask;

	/* owner */
	umask_prot->acl[0].rights = 
		convert_access_to_rights( (~umask & 0700) >> 6 ) | NSR_ADMIN;

	/* group */
	umask_prot->acl[1].rights = convert_access_to_rights( (~umask & 070) >> 3 );

	/* others */
	umask_prot->acl[2].rights = convert_access_to_rights( ~umask & 07 );

	return(old_mask);
}

/*
 * Define a way to print (for debugging) that will print something somewhere
 * no matter how poorly initialized we are.
 */
#if MACH3_UNIX || MACH3_VUS
#define dbg_print(str) \
    if (fork_debug) \
	{ \
	syscall_val_t p_rv; \
	htg_write(1, (str), strlen((str)), &p_rv); \
	}
#endif MACH3_UNIX || MACH3_VUS

#if MACH3_US
#define dbg_print(str) \
    console_print(str)
#endif MACH3_US

#ifndef dbg_print
#define dbg_print(str) \
    (void) write(1, (str), strlen((str)))
#endif

emul_io_change_identity(uxident* ux_id)
{
	mach_error_t			err;
	ns_authid_t			u_auth, g_auth;
	ns_token_t			token;

	(void)ux_id->uxident_get_token(&token);

	if ( err = prefix_obj->ns_set_token(token) ) {
		INFO((Diag,"ns_set_token failed... %x, %s\n",
				  err,
				  mach_error_string(err)));
		return err;
	}

	/*
	 * reinitialize the umask
	 */

	err = ux_id->uxident_get_eids(&u_auth,&g_auth);
	if (err) return (err);

#if 0
"prot 0x%x, head.version 0x%x, head.generation 0x%x\n",
		    umask_prot, &umask_prot->head.version, &umask_prot->head.generation));
#endif

	umask_prot->head.version = NS_PROT_VERSION;
	umask_prot->head.generation = 0;
	umask_prot->head.acl_len = 3;

	/* owner */
	umask_prot->acl[0].authid = u_auth;
	/* group */
	umask_prot->acl[1].authid = g_auth;
	/* others */
	umask_prot->acl[2].authid = 0;
	umask_protlen = NS_PROT_LEN(3);

    finish:
	return (ERR_SUCCESS);

}

/*
 *	Open File Table pointer Manipulation
 */

mach_error_t
emul_io_exec(void)
{
	return ftab_obj->ftab_exec();
}


/* syscall entries (for traps into user space from kernel svc's)*/


static mach_error_t
copen(boolean_t do_open, char* path_name, unsigned int ux_access, 
      unsigned int permission, syscall_val_t* rv)
{
	boolean_t		append_option;
	boolean_t		create_option;
	boolean_t		exclusive_option;
	boolean_t		truncate_option;
	int			fd = -1;
	mach_error_t		err = ERR_SUCCESS;
	usItem*		cat_obj = 0;
	uxio*		obj = 0;
//	mach_object_t		cat = NULL;
//	mach_object_t		obj = NULL;
	ns_access_t		access = NSR_GETATTR;
	ns_type_t		type = NST_INVALID;
	copyinstr_t		pathname;

	SyscallTrace("open");

	COPYINSTR(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);

	DEBUG0(emul_debug,(Diag,"open: '%s' o%o/o%o\n",
		    pathname, ux_access, permission));

#define	O_ACCESS	(O_RDONLY | O_WRONLY | O_RDWR)

	append_option    = ux_access & O_APPEND;
	create_option    = ux_access & O_CREAT;
	exclusive_option = ux_access & O_EXCL;
	truncate_option  = ux_access & O_TRUNC;
	

	/*  check for bad options  */
	if ((truncate_option || append_option) &&
	    !( (ux_access & O_ACCESS) == O_WRONLY
	      || (ux_access & O_ACCESS) == O_RDWR) ) {
		err = unix_err(EINVAL);
		goto finish;
	}
	
	switch(ux_access & O_ACCESS) {
	     case O_RDONLY:
  	        access |= NSR_READ;
		break;
	      case O_WRONLY:
		/*
		 * XXX Write-only does not work XXX
		 *
		 * Writes will fail because io_get_mf_state() needs
		 * NSR_READ access.
		 */
		access |= NSR_READ | NSR_INSERT | NSR_DELETE | NSR_WRITE;
		break;
	      case O_RDWR:
		access |= NSR_READ | NSR_INSERT | NSR_DELETE | NSR_WRITE;
		break;
	      default:
		err = unix_err(EINVAL);
		goto finish;
	}

	/*
	 * XXX truncate operation wants NSR_READ rights
	 * in addition to NSR_WRITE rights. This stems
	 * from io_set_size needing NSR_READ rights.
	 */
	if (truncate_option) access |= NSR_READ;

#undef	O_ACCESS

restart:

	/* find an existing object */
	err = prefix_obj->ns_resolve_fully(pathname, NSF_FOLLOW_ALL,
					    access, &cat_obj, &type, NULL);

	/* try the open or create */
	if (err != ERR_SUCCESS) {
		int			prot_data[DEFAULT_NS_PROT_LEN];
		int			protlen;
		usItem*		b_obj = 0;
		usName*		dir_obj = 0;
//		mach_object_t		dir = NULL;
		ns_mode_t		d_mode;
		ns_access_t		d_access;
		ns_name_t		leafname;
		ns_path_t		dirpath;
		ns_prot_t		prot = (ns_prot_t)prot_data;
		ns_type_t		dir_type = NST_INVALID;


		DEBUG2(emul_debug,(Diag,"open/create: got back %s\n",
			    mach_error_string(err)));
	        /* 
		 * proceed only if create option is specified and
		 * we got here because the file does not exist.
		 */

		if (!create_option || err != NS_NOT_FOUND) {
			if (!create_option && err == NS_NOT_FOUND)
				err = unix_err(ENOENT);
			goto finish;
		}

		err = emul_path(pathname,dirpath,sizeof(ns_path_t),
				leafname,sizeof(ns_name_t));
		if (err != ERR_SUCCESS) goto finish;

		/* resolve the target directory */
		d_mode = NSF_FOLLOW_ALL;
		d_access = NSR_INSERT;
		err = prefix_obj->ns_resolve_fully(dirpath, d_mode, d_access,
						    &b_obj, &dir_type, NULL);
		if (err != ERR_SUCCESS) goto finish;


		if (dir_type != NST_DIRECTORY) {
			err = NS_NOT_DIRECTORY;
			mach_object_dereference(b_obj);
			goto finish;
		}

		if ((dir_obj = usName::castdown(b_obj)) == 0) {
			ERROR((Diag,"copen: castdown error: %s->%s\n", b_obj->is_a()->class_name(), "usName"));
			err = MACH_OBJECT_NO_SUCH_OPERATION;
			goto finish;
		}

		/* get some umask'd protection */
		(void)emul_io_umask(permission,prot,&protlen);

		type = NST_FILE;
		err = dir_obj->ns_create(leafname, type, prot, protlen,
					  access, &cat_obj);

		if (err != ERR_SUCCESS) {
			DEBUG2(emul_debug,(Diag,"ns_add_entry: %s\n",
						  mach_error_string(err)));
			/*
			 * Maybe somebody else just created the file.
			 * In that case, maybe we should just use it.
			 */
			if (err != NS_ENTRY_EXISTS) {
				mach_object_dereference(dir_obj);
				goto finish;
			}

			if (exclusive_option) {
				err = unix_err(EEXIST);
				mach_object_dereference(dir_obj);
				goto finish;
			}
		}

		/* deallocate the old dir */
		mach_object_dereference(dir_obj);

	} else if (create_option && exclusive_option) {
		err = unix_err(EEXIST);
		goto finish;
	}

	/* creat() specific behavior */
	if (!do_open) {
		if (!(access & NSR_WRITE)) {
			err = unix_err(EACCES);
			goto finish;
		}
	}

	/* 
	 * you get back from ns_resolve some sort of an i/o object.
	 * make a unix i/o object and give the underlying i/o object to it.
	 */

	/* make a new object that understands unix ops */

	switch(type) {
	case NST_DIRECTORY:
	  {
	    /* directories get some special operations */
//	    mach_object_t		dir;
//	    new_object(obj, uxio_dir);
//	    new_object(dir, uxdir);
//	    delegate_object(dir,cat);
//	    cat = dir;
	    /*
	     * XXX C++
	     * uxio_dir now has the functionality previously provided
	     * by uxdir.
	     */
	    obj = new uxio_dir;
	    break;
	  }
	case NST_TTY:
//	    new_object(obj, uxio_tty);
	    obj = new uxio_tty;
	    break;

	case NST_UPIPE_BYTES:
//	    new_object(obj, uxio_pipe);
	    obj = new uxio_pipe;
	    break;

	/*
	 * XXX Need to deal with sockets.
	 */

	default:
//	    new_object(obj, uxio);
	    obj = new uxio;
	    break;
	}

	err = obj->ux_open(cat_obj, ux_access, access);
	if (err) {
		if ((err == US_OBJECT_BUSY) || (err == NS_INVALID_HANDLE)) {
			mach_object_dereference(cat_obj);
			mach_object_dereference(obj);
			goto restart;
		} else 
			goto finish;
	}

	err = ftab_obj->ftab_add_obj(obj, &fd);

    finish:
	COPYINSTR_DONE(path_name, pathname, sizeof(ns_path_t), rv, EFAULT);
	mach_object_dereference(cat_obj);
	mach_object_dereference(obj);

	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = fd;
	
	return( err );


}

mach_error_t
emul_creat( char* pathname, unsigned int permission, syscall_val_t* rv )
{
	return copen(FALSE, pathname, O_RDWR|O_CREAT|O_TRUNC, permission, rv);
}

mach_error_t
emul_open( char* pathname, unsigned int access, unsigned int permission, 
	  syscall_val_t* rv )
{
	return copen(TRUE, pathname, access, permission, rv);
}



mach_error_t
emul_close( int fd, syscall_val_t* rv )
{
	mach_error_t		err = ERR_SUCCESS;

	SyscallTrace("close");

	err = ftab_obj->ftab_close(fd);

	if ( err )
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = ESUCCESS;

	return( err );
}


mach_error_t
emul_read(int fd, char* buffer, unsigned int len, syscall_val_t* rv)
{
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;
	unsigned int		rlen = len;
	char			*buf;

	SyscallTrace("read");

	COPYOUT_INIT(buf, buffer, len, char, rv, EFAULT);

	if (err = ftab_obj->ftab_get_obj(fd,&obj)) goto finish;

	DEBUG2(emul_debug,(Diag,"read(%x %x %x)",fd,buf,len));

	err = obj->ux_read(buf, &rlen);
 	if ((err == IO_INVALID_OFFSET) || (err == IO_REJECTED)) {
		rlen = 0;
		err = ERR_SUCCESS;
	}

    finish:
	COPYOUT(buf, buffer, len, char, rv, EFAULT);
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = rlen;

	return( err );
}

mach_error_t
emul_readv(int fd, struct iovec* _iov, int iovcnt, syscall_val_t* rv)
{
	int			i;
	int			iov_len_sum = 0;
	unsigned int		nbytes = 0;
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;
        struct iovec            * iov;

	SyscallTrace("readv");

	/* 
	 * An invalid iovec address produces an EINVAL error
         * rather than the EFAULT error suggested by the 4.3BSD manual.
	 */
	COPYOUT_INIT(iov, _iov, iovcnt, struct iovec, rv, EINVAL);
	/* XXX check data buffer itself */

	DEBUG2(emul_debug,(Diag,"readv(%x %x %x)",fd,iov,iovcnt));

	if (iovcnt <= 0 || iovcnt > 16) {
		err = unix_err(EINVAL);
		goto finish;
	}

	for (i = 0; i < iovcnt; i++) {
		if (iov[i].iov_len < 0 || /* or overflow? */
		    (iov_len_sum + iov[i].iov_len < iov_len_sum)) {
				err = unix_err(EINVAL);
				goto finish;
	        }
		/*
		 * XXX ux_readv() or whatever io_read methods it invokes
		 *     should check to see if iov_base is valid. XXX
		 */
		VALID_ADDRESS(iov[i].iov_base, rv, EFAULT);
	}

	/* XXX Check for iov pointing outside process addr */

	if (err = ftab_obj->ftab_get_obj(fd, &obj)) goto finish;

	err = obj->ux_readv(iov, iovcnt, &nbytes);
	
 	if ((err == IO_INVALID_OFFSET) || (err == IO_REJECTED)) {
		nbytes = 0;
		err = ERR_SUCCESS;
	}

    finish:
	COPYOUT(iov, _iov, iovcnt, struct iovec, rv, EINVAL);
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = nbytes;

	return( err );
}

mach_error_t
emul_getdirentries(int fd, char* buffer, unsigned int nbytes, long* baseptr,
		   syscall_val_t* rv)
{
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;
	char			* buf;
	long			* basep;

	SyscallTrace("getdirentries");

	COPYOUT_INIT(buf, buffer, nbytes, char, rv, EFAULT);
	COPYOUT_INIT(basep, baseptr, 1, long, rv, EFAULT);

	if (err = ftab_obj->ftab_get_obj(fd, &obj)) goto finish;

	DEBUG2(emul_debug,(Diag,"getdirentries(%x %x %x %x)",fd,buf,nbytes,basep));

	/* get the current location into "basep" */
	*basep = 0;
	err = obj->ux_lseek(basep, L_INCR);
	if (ERR_SUCCESS != err) goto finish;
	
	/* get the directory info */
	err = obj->ux_read(buf,&nbytes);

    finish:
	COPYOUT(buf, buffer, nbytes, char, rv, EFAULT);
	COPYOUT(basep, baseptr, 1, long, rv, EFAULT);
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = nbytes;

	return( err );
}

mach_error_t
emul_write( int fd, char* buffer, int len, syscall_val_t* rv )
{
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;
	unsigned int		rlen = len;
	char			* buf;

	SyscallTrace("write");

	COPYIN(buffer, buf, len, char, rv, EFAULT);

	DEBUG2(emul_debug,(Diag,"emul_write(%x %x %x)",fd,buf,len));
	DEBUG2(emul_debug,(Diag,"emul_write: '%*.*s'\n", len > 30 ? 30 : len, 10, buf));

	if (err = ftab_obj->ftab_get_obj(fd,&obj)) goto finish;

	err = obj->ux_write(buf,&rlen);

	if ((err == IO_INVALID_OFFSET) || (err == IO_REJECTED)) {
	        (void) do_self_signal(SIGPIPE);
		rlen = 0;
		err = unix_err(EPIPE);
	}

    finish:
	COPYIN_DONE(buffer, buf, len, char, rv, EFAULT);
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = rlen;

	DEBUG2(emul_debug,(Diag,"emul_write: val=%d, rc=%s\n",rv->rv_val1,
		    mach_error_string(err)));


	return( err );
}

mach_error_t
emul_writev(int fd, struct iovec* _iov, int iovcnt, syscall_val_t* rv)
{
	int			i;
	int			iov_len_sum = 0;
	unsigned int		nbytes = 0;
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;
        struct iovec            * iov;

	SyscallTrace("writev");

	/* 
	 * An invalid iovec address produces an EINVAL error
         * rather than the EFAULT error suggested by the 4.3BSD manual.
	 */
	COPYIN(_iov, iov, iovcnt, struct iovec, rv, EINVAL);
	/* XXX check data buffer itself */

	DEBUG2(emul_debug,(Diag,"writev(%x %x %x)",fd,iov,iovcnt));

	if (iovcnt <= 0 || iovcnt > 16) {
		err = unix_err(EINVAL);
		goto finish;
	}

	for (i = 0; i < iovcnt; i++) {
		if (iov[i].iov_len < 0 || /* or overflow? */
		    (iov_len_sum + iov[i].iov_len < iov_len_sum)) {
			err = unix_err(EINVAL);
			goto finish;
	        }
		/*
		 * XXX ux_writev() or whatever io_write methods it invokes
		 *     should check to see if iov_base is valid. XXX
		 */
		VALID_ADDRESS(iov[i].iov_base, rv, EFAULT);
	}

	/* XXX Check for iov pointing outside process addr */

	if (err = ftab_obj->ftab_get_obj(fd,&obj)) goto finish;

	err = obj->ux_writev(iov, iovcnt, &nbytes);
	
 	if ((err == IO_INVALID_OFFSET) || (err == IO_REJECTED)) {
	        (void) do_self_signal(SIGPIPE);
		nbytes = 0;
		err = unix_err(EPIPE);
	}

    finish:
	COPYIN_DONE(_iov, iov, iovcnt, struct iovec, rv, EINVAL);
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = nbytes;

	return( err );
}

mach_error_t
emul_ftruncate( int fd, unsigned int len, syscall_val_t* rv )
{
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;

	SyscallTrace("ftruncate");

	/* get the fp */
	if ( err = ftab_obj->ftab_get_obj(fd, &obj) ) goto finish;

	err = obj->ux_ftruncate(len);

	if (err == MACH_OBJECT_NO_SUCH_OPERATION)
	  err = unix_err(EINVAL);

    finish:
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix( err );
	else
		rv->rv_val1 = ESUCCESS;
		
	return( err );
}


mach_error_t
emul_dup( int fd, int opt_fd, syscall_val_t* rv )
{
	mach_error_t		err = ERR_SUCCESS;
	int			targ = -1;

	SyscallTrace("dup");

	if ( fd & ~077 ) {
		targ = opt_fd;
		err = ftab_obj->ftab_cdup(fd & 077, &targ, FALSE);
	} else {
		err = ftab_obj->ftab_cdup(fd & 077, &targ, TRUE);
	}

	if (err)
		rv->rv_val1 = emul_error_to_unix( err );
	else
		rv->rv_val1 = targ;

	DEBUG1(1,(Diag,"dup: %d->%d", fd, rv->rv_val1));

	return err;
}

mach_error_t
emul_dup2( int fd_from, int fd_to, syscall_val_t* rv )
{
	mach_error_t		err = ERR_SUCCESS;

	SyscallTrace("dup2");
	
	err = ftab_obj->ftab_cdup(fd_from, &fd_to, FALSE);
	if (err)
		rv->rv_val1 = emul_error_to_unix( err );
	else
		rv->rv_val1 = fd_to;

	return err;
}


mach_error_t emul_fcntl(int fd, int cmd, int arg, syscall_val_t* rv)
{
	int			flags = 0;
	int			res = ERR_SUCCESS;
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;
	
	SyscallTrace("fcntl");
	
	switch( cmd ) {
	    case F_DUPFD:
		res = arg;
		err = ftab_obj->ftab_cdup(fd, &res, TRUE);
		break;
		
	    case F_GETFD:
		err = ftab_obj->ftab_get_flags(fd, &flags);
		if (flags & FD_MASK(FD_CLOSE_ON_EXEC))
			res = 1;
		else
			res = 0;
		break;
		
	    case F_SETFD:
		err = ftab_obj->ftab_set_flag(fd, FD_CLOSE_ON_EXEC, arg);
		break;
		
	    default:
		err = ftab_obj->ftab_get_obj(fd,&obj);
		if (err == ERR_SUCCESS ) {
			/* everybody else operates on the object */
			err = obj->ux_fcntl(cmd, &arg);
			res = arg;
		}
		break;
	}
	
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = res;
	
	return err;
}

mach_error_t
emul_lseek(int fd, long pos, int mode, syscall_val_t* rv)
{
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;

	SyscallTrace("lseek");
	DEBUG2(emul_debug,(Diag,"lseek(fd %d, pos 0x%x, mode 0x%x)",fd,pos,mode));

	/* get the obj */
	if ( err = ftab_obj->ftab_get_obj(fd, &obj) ) goto finish;

	err = obj->ux_lseek(&pos, mode);
	if (err == MACH_OBJECT_NO_SUCH_OPERATION)
		err = unix_err(EINVAL);

    finish:
	DEBUG2(emul_debug,(Diag,"lseek(...) at exit offset=0x%x",pos));
	mach_object_dereference( obj );
	if (err)
		rv->rv_val1 = emul_error_to_unix( err );
	else
		rv->rv_val1 = pos;

	return( err );
}

mach_error_t
emul_getdtablesize( syscall_val_t* rv )
{
	SyscallTrace("getdtablesize");

	rv->rv_val1 = MAX_OPEN_FILES;
	return ERR_SUCCESS;	/* what could go wrong? */
}


#if MACH3_US_NO_MORE
#include <unix_include/sys/select_types.h>
#endif MACH3_US_NO_MORE

#ifndef	USE_HACK_SELECT
mach_error_t
emul_select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds,
	    struct timeval* timeout, syscall_val_t* rv )
{
	mach_error_t ret;
	mach_msg_timeout_t m_timeout = 0;
	uxselect *selobj;
	int found_fds;

	if (timeout) {
		m_timeout = (timeout->tv_sec * 1000) + (timeout->tv_usec / 100);
	}
	selobj = new uxselect;
	ret = selobj->select(ftab_obj, nfds, readfds, writefds, exceptfds,
				m_timeout, &found_fds);

	if (ret == ERR_SUCCESS) {
		rv->rv_val1 = found_fds;
	}
	else {
		rv->rv_val1 = emul_error_to_unix(ret);
	}

	return(ret);
}
#else	USE_HACK_SELECT
/*
 * XXX Incredibly sleazy hack to make select() "sort of work" for
 * naive applications.
 *
 * Copied from POE.
 */
extern int itimerfix(struct timeval *);

mach_error_t
emul_select(int			nfds,
	    fd_set		* readfds, 
	    fd_set		* writefds, 
	    fd_set		* exceptfds,
	    struct timeval	* timeout,
	    syscall_val_t	* rv)
{
	mach_error_t		err;

	SyscallTrace("select");

	DEBUG0(emul_debug,(Diag,"select: nfds=%d timeout=%x\n",nfds,timeout));

	if (timeout) {
		int			milliseconds;
#if	1
		mach_msg_header_t	sleep_msg;
		static mach_port_t	sleep_port = MACH_PORT_NULL;
#else	1
		mach_msg_header_t		sleep_msg;
		static port_t		sleep_port = MACH_PORT_NULL;
#endif	1
		if (itimerfix(timeout) != 0) {
			err = unix_err(EINVAL);
			goto finish;
		}

		if (sleep_port == MACH_PORT_NULL) {
			err = mach_port_allocate(mach_task_self(),
					MACH_PORT_RIGHT_RECEIVE,&sleep_port);
			if (err != ERR_SUCCESS) {
				us_internal_error(
					"emul_select.mach_port_allocate()",
					err);
				err = US_INTERNAL_ERROR;
				goto finish;
			}
		}

		milliseconds = timeout->tv_sec * 1000
						+ timeout->tv_usec / 1000;
		if (milliseconds > 0) {
			sleep_msg.msgh_size = sizeof(mach_msg_header_t);
			sleep_msg.msgh_local_port = sleep_port;
			err = mach_msg(&sleep_msg,
					MACH_RCV_MSG|MACH_RCV_TIMEOUT,
					0,sizeof(mach_msg_header_t),sleep_port,
					milliseconds,MACH_PORT_NULL);
			if (err != MACH_RCV_TIMED_OUT) {
				us_internal_error(
						"emul_select.mach_msg()",err);
				err = US_INTERNAL_ERROR;
				goto finish;
			}
		}
	}

	/*
	 * XXX We have no support for out of band yet.
	 */
	if (exceptfds) {
		FD_ZERO(exceptfds);
	}

	/*
	 * XXX Just pretend that one descriptor is ready, but
	 * don't mess with the bit masks...
	 */
	rv->rv_val1 = 1;
	err = ERR_SUCCESS;

finish:
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = emul_error_to_unix(err);
	}

	return(err);
}
#endif	USE_HACK_SELECT

/*
 * Check that a proposed timeout value is acceptable,
 * and fix it to have at least minimal value (i.e. if
 * it is less than the minimal resolution, round it up.)
 */

static tick = 10000;/* 10 msecs */

itimerfix(struct timeval* tv)
{

	if (tv->tv_sec < 0 || tv->tv_sec > 100000000 ||
	    tv->tv_usec < 0 || tv->tv_usec >= 1000000)
		return (EINVAL);
	if (tv->tv_sec == 0 && tv->tv_usec != 0 && tv->tv_usec < tick)
		tv->tv_usec = tick;
	return (0);
}


/*
 */
mach_error_t
emul_pipe(syscall_val_t* rv)
{
	int			fildes[2];
	int			prot_data[DEFAULT_NS_PROT_LEN];
	int			protlen = 0;
	mach_error_t		err = ERR_SUCCESS;
	usItem*			a_obj = 0;
	usName*			dir_proxy = 0;
	usByteIO*		read_proxy = 0;
	usByteIO*		write_proxy = 0;
	uxio*			read_obj = 0;
	uxio*			write_obj = 0;
	ns_name_t		name;
	ns_prot_t		prot = (ns_prot_t)prot_data;
	ns_type_t		type = NST_INVALID;

	SyscallTrace("pipe");

	/*
	 * XXX Check that fildes is a valid pointer.
	 */

	fildes[0] = -1;
	fildes[1] = -1;
	/*
	 * Create a new unnamed pipe in the special
	 * /pipenet/UPIPE_BYTES directory.
	 */
	err = prefix_obj->ns_resolve_fully("/server/pipenet/UPIPE_BYTES",
				NSF_FOLLOW_ALL,NSR_INSERT,&a_obj,&type,NULL);
	if (err != ERR_SUCCESS) {
		ERROR((Diag,"Cannot find /server/pipenet/UPIPE_BYTES directory: %s\n",
						mach_error_string(err)));
		goto finish;
	}
	if ((dir_proxy = usName::castdown(a_obj)) == 0) {
		DEBUG0 (emul_debug, (0, "emul_pipe\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		ERROR((Diag,"emul_pipe: castdown error: %s->%s\n", a_obj->is_a()->class_name(), "usName"));
		goto finish;
	}
	(void) emul_io_umask(0700,prot,&protlen);
	prot->acl[0].rights = NSR_READ | NSR_WRITE | NSR_GETATTR | NSR_ADMIN;
	err = dir_proxy->ns_create_anon(NST_UPIPE_BYTES,prot,protlen,
					NSR_WRITE | NSR_GETATTR,
					name,&a_obj);
	if (err != ERR_SUCCESS) {
		ERROR((Diag,"Cannot create a new unnamed pipe: %s\n",
						mach_error_string(err)));
		goto finish;
	}
	if ((write_proxy = usByteIO::castdown(a_obj)) == 0) {
		ERROR((Diag,"emul_pipe: castdown error: %s->%s\n", a_obj->is_a()->class_name(), "usByteIO"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}
	a_obj = 0;
	err = write_proxy->ns_duplicate(NSR_READ | NSR_GETATTR, &a_obj);
	if (err != ERR_SUCCESS) {
		ERROR((Diag,
		"Cannot setup the write side for new unnamed pipe: %s\n",
						mach_error_string(err)));
		goto finish;
	}
	if ((read_proxy = usByteIO::castdown(a_obj)) == 0) {
		ERROR((Diag,"emul_pipe: castdown error: %s->%s\n", a_obj->is_a()->class_name(), "usByteIO"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}
	a_obj = 0;

//	new_object(read_obj, uxio_pipe);
	read_obj = new uxio_pipe;
	err = read_obj->ux_open(read_proxy,O_RDONLY,NSR_READ | NSR_GETATTR);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

//	new_object(write_obj, uxio_pipe);
	write_obj = new uxio_pipe;
	err = write_obj->ux_open(write_proxy,O_WRONLY,NSR_WRITE|NSR_GETATTR);
	if (err != ERR_SUCCESS) {
		goto finish;
	}

	err = ftab_obj->ftab_add_obj(read_obj,&fildes[0]);
	if (err != ERR_SUCCESS) {
		goto finish;
	}
	err = ftab_obj->ftab_add_obj(write_obj,&fildes[1]);
	if (err != ERR_SUCCESS) {
		(void) ftab_obj->ftab_close(fildes[0]);
		goto finish;
	}

finish:
	mach_object_dereference(a_obj);
	mach_object_dereference(dir_proxy);
	mach_object_dereference(read_proxy);
	mach_object_dereference(write_proxy);
	mach_object_dereference(read_obj);
	mach_object_dereference(write_obj);

	if (err) {
		rv->rv_val1 = emul_error_to_unix(err);
	} else {
		rv->rv_val1 = fildes[0];
		rv->rv_val2 = fildes[1];
	}

	DEBUG1(emul_debug,(Diag,"pipe(): fildes=0x%x, rval1=%d, rval2=%d\n",
						fildes,fildes[0],fildes[1]));

	return(err);
}

mach_error_t
emul_fstat(int fd, struct stat* buffer, syscall_val_t* rv)
{
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;
	struct stat		* buf;

	SyscallTrace("fstat");

	COPYOUT_INIT(buf, buffer, 1, struct stat, rv, EFAULT);

	/* get the fp */
	if ( err = ftab_obj->ftab_get_obj(fd, &obj) ) goto finish;

	err = obj->ux_fstat(buf);

    finish:
	COPYOUT(buf, buffer, 1, struct stat, rv, EFAULT);
	mach_object_dereference( obj );
	if (err)
		rv->rv_val1 = emul_error_to_unix( err );
	else
		rv->rv_val1 = ESUCCESS;
		
	return( err );
}

mach_error_t
emul_umask(int mask, syscall_val_t* rv)
{
	DEBUG1(emul_debug,(Diag,"emul_umask: umask=o%o\n",mask));
	rv->rv_val1 = emul_io_set_umask(mask);
	return(ERR_SUCCESS);
}

mach_error_t
emul_smmap(caddr_t addr, int len, int prot, int flags, int fd, off_t pos,
	   syscall_val_t* rv)
{
	boolean_t		anywhere;
	mach_error_t		err = ERR_SUCCESS;
//	mach_object_t		obj = NULL;
	uxio*		obj = 0;
	ns_access_t		access = NSR_NONE;
	off_t			file_pos  = pos;
	vm_offset_t		pager_offset;
	vm_offset_t		user_addr = (vm_offset_t) addr;
	vm_size_t		user_size = (vm_size_t) len;
	vm_prot_t		user_prot =
	                                (prot & PROT_WRITE) ?
					  VM_PROT_ALL :
					  VM_PROT_READ|VM_PROT_EXECUTE;
	
	SyscallTrace("smmap");

	err = ftab_obj->ftab_get_obj(fd, &obj);
	if (err != ERR_SUCCESS) {
		err = unix_err(EINVAL);
		goto finish;
	}

	/* Round the start and end addresses to nearest page boundary */

	user_addr = trunc_page(user_addr);
	user_size = round_page(user_size);
	
	/* 
	 *	File can be COPIED at an arbitrary offset.
	 *	File can only be SHARED if the offset is at
	 *      a page boundary.
	 */

	if ((flags & MAP_SHARED) && (flags & MAP_PRIVATE)) {
		err = unix_err(EINVAL);
		goto finish;
	}

	if (flags == MAP_SHARED &&
	    (trunc_page(file_pos) != (vm_offset_t)(file_pos))) {
		err = unix_err(EINVAL);
		goto finish;
	}

	/*
	 *	File must be writable if mapping will be writable
	 *	and shared. Otherwise we get a copy.
	 */

	/* Get my access to the object */
	err = obj->ux_get_access(&access);
	if (err != ERR_SUCCESS) goto finish;
	
	if ((prot & PROT_WRITE) && (access & NSR_WRITE) == 0
	    && flags == MAP_SHARED) {
		err = unix_err(EINVAL);
		goto finish;
	}
	if ((prot & PROT_READ) && (access & NSR_READ) == 0 ) {
		err = unix_err(EINVAL);
		goto finish;
	}

	pager_offset = (vm_offset_t) file_pos;
	
	/*
	 * Deallocate the existing memory, then map the appropriate
	 * memory object into the space left.
	 */

	(void) vm_deallocate(mach_task_self(), user_addr, user_size);
	err = obj->ux_map(mach_task_self(), &user_addr, user_size, 0, FALSE,
			   pager_offset,
			   (flags != MAP_SHARED),
			   user_prot,
			   (flags == MAP_SHARED) ?
			   user_prot : VM_PROT_ALL,
			   ((flags == MAP_SHARED) ?
			    VM_INHERIT_SHARE :VM_INHERIT_COPY)
		      );

	if (err == MACH_OBJECT_NO_SUCH_OPERATION)
		err = unix_err(EINVAL);

      finish:
	mach_object_dereference(obj);
	if (err)
		rv->rv_val1 = emul_error_to_unix( err );
	else
		rv->rv_val1 = user_addr;
	return err;
}

mach_error_t
emul_munmap(caddr_t addr, int len, syscall_val_t* rv)
{
	mach_error_t		err = ERR_SUCCESS;
	vm_offset_t		user_addr;
	vm_size_t		user_size;

	SyscallTrace("munmap");

	user_addr = (vm_offset_t) addr;
	user_size = (vm_size_t) len;
	if ((user_addr & (vm_page_size-1)) ||
	    (user_size & (vm_page_size-1))) {
		err = unix_err(EINVAL);
		goto finish;
	}

	err = vm_deallocate(mach_task_self(), user_addr, user_size);
	  
      finish:
	if (err)
		rv->rv_val1 = emul_error_to_unix( err );
	else
		rv->rv_val1 = ERR_SUCCESS;
	return err;
}

#endif	ComplexIo

#if	(ComplexIo || FileOps)	

mach_error_t
emul_ioctl( int fd, int cmd, vm_address_t arg, syscall_val_t* rv )
{
	mach_error_t		err = ERR_SUCCESS;
	uxio*		obj = 0;
//	mach_object_t		obj = NULL;

	SyscallTrace("ioctl");

	DEBUG0(emul_debug,(Diag,"ioctl: %d, 0x%x, 0x%x\n",	
		    fd, cmd, arg));

	switch (cmd) {
		case FIOCLEX:
			err = ftab_obj->ftab_set_flag(fd,FD_CLOSE_ON_EXEC,1);
			break;

		case FIONCLEX:
			err = ftab_obj->ftab_set_flag(fd,FD_CLOSE_ON_EXEC,0);
			break;

		default:
			/* get the obj */
			if ( err = ftab_obj->ftab_get_obj(fd, &obj) )
				goto finish;

			err = obj->ux_ioctl(cmd, (int*) arg);
			if (err == MACH_OBJECT_NO_SUCH_OPERATION)
				err = unix_err(ENOTTY);
			break;
	}

    finish:
	mach_object_dereference(obj);
	if (err) {
		rv->rv_val1 = emul_error_to_unix(err);
		DEBUG0(emul_debug,(Diag, "ioctl: fails err=0x%x, unix err=0x%0x\n", err, rv->rv_val1));
	} else
		rv->rv_val1 = ESUCCESS;

	return( err );
}


#endif	

