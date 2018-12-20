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
 * HISTORY:
 * $Log:	ufs_export.c,v $
 * Revision 1.30  94/07/27  14:40:35  mrt
 * 	Changed the fss_setattr routine so that a timeval with
 * 	a value of zero will cause the related inode time to be
 * 	update with the current time.
 * 	[94/07/27  14:15:37  grm]
 * 
 * Revision 1.29  94/07/21  11:59:00  mrt
 * 	updated copyright
 * 
 * Revision 1.28  94/06/16  17:30:23  mrt
 * 	Fix chown1 so that root can do anything.
 * 	[94/06/02  13:26:34  jms]
 * 
 * Revision 1.27  93/01/20  17:40:29  jms
 * 	Fastlink stuff
 * 	[93/01/18  17:54:44  jms]
 * 
 * Revision 1.26  92/07/05  23:37:01  dpj
 * 	Added IO_APPEND.
 * 	[92/06/24  17:46:33  dpj]
 * 
 * Revision 1.25  91/12/20  17:45:28  jms
 * 	Comment Fix.
 * 	[91/12/20  16:48:49  jms]
 * 
 * 	Add compilation conditions asto whether to use credentials or access
 * 	[91/12/20  16:31:18  jms]
 * 
 *	lists. (DPJ)
 * 
 * Revision 1.24  91/07/01  14:16:28  jms
 * 	Removed max_count arg to fss_pagein().
 * 	[91/06/25  12:00:54  roy]
 * 
 * 	Added last_link out arg to fss_remove.
 * 	[91/06/05  12:43:20  roy]
 * 
 * 	Implemented notion of unix_master.
 * 	Changed fss_{pagein,pageout} to use ufs_{pagein,pageout}.
 * 	Fixed bug in fss_link not to leave parent dir locked.
 * 	[91/05/29  11:11:54  roy]
 * 
 * Revision 1.23  90/12/21  13:56:19  jms
 * 	Merge forward.
 * 	[90/12/15  14:21:02  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:28:05  jms]
 * 
 * Revision 1.22.4.1  90/12/14  11:24:08  roy
 * 	- check error return from namei_in_directory in fss_mkdir()
 * 	- fix error code return check from dirremove() in fss_rmdir()
 * 	- don't adjust uio_offset in fss_readdir() (fdwr() does it)
 * 	- implement fss_sync(), fss_pagein(), fss_pageout()
 * 
 * 
 * Revision 1.22  89/11/28  19:13:08  dpj
 * 	Improved handling of errors from namei_in_directory(), to try
 * 	and unlock the various inodes more often.
 * 	[89/11/20  21:08:45  dpj]
 * 
 * Revision 1.21  89/06/30  18:39:40  dpj
 * 	Update the uio_offset at the end of fss_readdir(), to follow
 * 	the protocol required for NFS.
 * 	[89/06/21  22:32:31  dpj]
 * 
 * Revision 1.20  89/06/05  17:18:36  dorr
 * 	fix typo in gid assignment
 * 	[89/06/05  14:32:40  dorr]
 * 
 * Revision 1.19  89/05/18  10:52:24  dorr
 * 	set mode field when extracting info from inode.
 * 	[89/05/15  12:33:55  dorr]
 * 
 * Revision 1.18  89/03/23  11:28:15  dorr
 * 	get rid of ufs.h ... no longer needed.
 * 
 * Revision 1.17  89/03/17  13:08:26  sanzi
 * 	Revision 1.16  89/01/20  09:21:23  sanzi
 * 	drop lock on parent in error case of fs_mkdir().
 * 	
 * 	conditionize debug output.
 * 	[89/03/12  20:23:54  dorr]
 * 	
 * 	Use "fss_" prefix for all exported functions.
 * 	Fixed includes to replace fs_export.h with fs_types.h.
 * 	[89/02/15  21:45:15  dpj]
 * 
 * Revision 1.14  88/12/08  10:40:06  sanzi
 * in fs_setattr(): only call itrunc() if the file changes size.  This is
 * a workaround for the current version of the file library and should
 * be deleted when and if the file library is replaced.
 * . 
 * 
 * Revision 1.13  88/12/02  13:25:02  sanzi
 * add some debugging code to insure that we are not getting
 * handed inode pointers with a zero i_count.
 * 
 * Revision 1.12  88/10/26  08:20:02  sanzi
 * fix duplicate iput() in fs_mkdir.
 * 
 * Revision 1.11  88/10/14  17:30:51  sanzi
 * Make link and symlink work.  Merge onto mainline.
 * 
 * Revision 1.10  88/10/11  09:06:56  sanzi
 * moved definition of chown1() into here from ufs_syscalls.c.
 * 
 * Revision 1.9  88/09/20  21:46:24  sanzi
 * fix parameter to itrunc()... soon I will fix all functions to return error codes 
 * rather than the current mess.
 * 
 * Revision 1.8  88/09/20  20:18:11  sanzi
 * add some deleted code to fs_remove() (but conditionalized off).
 * removed extraneous second call to dirremove() in fs_remove()
 * add some debugging code to test whether dan's package was calling
 * fs_release().
 * implement (trivally) fs_reference()
 * 
 * Revision 1.7  88/09/13  10:04:58  sanzi
 * fix reference counting. 
 * 
 * Revision 1.6  88/09/07  12:58:38  sanzi
 * fs_attributes --> fs_attr_t
 * 
 */
 
#include <mach_init.h>
#include <mach_error.h>


#include "base.h"
#include <debug.h>
#include <mach/port.h>

#include "hash.h"

#include "types.h"
#include "param.h"
#include "user.h"
#include "inode.h"
#include "file.h"
#include "fs.h"

#include <fs_types.h>
#include <ns_error.h>
#include <io_error.h>

#define READ_MACH_FASTLINK
#define IO_APPEND	0x02		/* append write for VOP_RDWR */

struct mutex	master_mutex = MUTEX_INITIALIZER;
#define	unix_master()				\
	    mutex_lock(&master_mutex)		

#define	unix_release()				\
	    mutex_unlock(&master_mutex)	


#define	ICOUNT_CHECK(ip)	if (ip && (ip->i_count < 1)) panic("icount");

extern int unix_error_table[], unix_error_table_size;
#define	unix_error_to_fs_error(err)					\
	( ((err >= 0) && (err < unix_error_table_size)) ?		\
		unix_error_table[err] : 				\
		unix_err(err))


#define	FS_SUCCESS	ERR_SUCCESS


fss_open(ipp,flags, cred)
struct inode **ipp;
int flags;
fs_cred_t cred;
{
    struct inode *ip = *ipp;

    unix_master();
    ILOCK(ip);
    ICOUNT_CHECK(ip);  /* debug */
    iincr_chk(ip);
    IUNLOCK(ip);
    unix_release();

    return FS_SUCCESS;
}


fss_close(ip,flags, cred)
struct inode *ip;
int flags;
fs_cred_t cred;
{
    unix_master();
    ICOUNT_CHECK(ip);  /* debug */
    irele(ip);
    unix_release();

    return FS_SUCCESS;    
}

fss_pagein(ip, offset, data, count, cred)
struct inode  *ip;
vm_offset_t   offset;
vm_address_t  *data;
vm_size_t     *count;
fs_cred_t     cred;
{
    int error = 0;

    unix_master();
    ILOCK(ip);
    ICOUNT_CHECK(ip);  /* DEBUG */

    error = ufs_pagein(ip, offset, data, count);

    IUNLOCK(ip);
    unix_release();

    return (unix_error_to_fs_error(error));

}

fss_pageout(ip, offset, data, count, cred)
struct inode  *ip;
vm_offset_t   offset;
vm_address_t  data;
vm_size_t     *count;
fs_cred_t     cred;
{
    int error = 0;

    unix_master();
    ILOCK(ip);
    ICOUNT_CHECK(ip);  /* DEBUG */

    error = ufs_pageout(ip, offset, data, count);

    IUNLOCK(ip);
    unix_release();

    return (unix_error_to_fs_error(error));

}

fss_rdwr(ip, uiop, readp, flags, cred)
struct inode *ip;
struct uio *uiop;
int readp;
int flags;
fs_cred_t cred;
{
    int error = 0;
    int rw;

    if (readp) rw = UIO_READ;
    else rw = UIO_WRITE;

    unix_master();
    ILOCK(ip);
    ICOUNT_CHECK(ip);  /* debug */
    if ((ip->i_mode&IFMT) == IFREG) {
	if ((flags & IO_APPEND) && (rw == UIO_WRITE)) {
		/*
		 * in append mode start at end of file.
		 */
		uiop->uio_offset = ip->i_size;
	}
	error = rwip(ip, uiop, rw, flags);
	/* ILOCK_WRITE_DONE(ip); */
    } else {
        /* ILOCK_WRITE_TO_READ(ip); */	
	error = rwip(ip, uiop, rw);
	/* ILOCK_READ_DONE(ip); */	
    }
    IUNLOCK(ip);    
    unix_release();

    return (unix_error_to_fs_error(error));
    
}

enum vtype inode_type_to_fs_type(imode)
int imode;
{
    switch(imode&IFMT) {
	case IFCHR:
		return(VCHR);
	case IFDIR:
		return(VDIR);	
	case IFBLK:
		return(VBLK);
	case IFREG:
		return(VREG);
	case IFLNK:
		return(VLNK);
	case IFSOCK:
		return(VSOCK);
	default:
		return(VNON);
    }
}



fss_getattr(ip, vap, cred)
struct inode *ip;
fs_attr_t vap;
fs_cred_t cred;
{
	unix_master();
	ILOCK_READ(ip);
	ICOUNT_CHECK(ip);  /* debug */

#define	IFTOVT(x)	(inode_type_to_fs_type(x))
	/*
	 * Copy from inode table.
	 */
	vap->va_type = IFTOVT(ip->i_mode);
	vap->va_mode = ip->i_mode;
	vap->va_uid = ip->i_uid;
	vap->va_gid = ip->i_gid;
	vap->va_fsid = ip->i_dev;
	vap->va_nodeid = ip->i_number;
	vap->va_nlink = ip->i_nlink;
	vap->va_size = ip->i_size;
	vap->va_atime.tv_sec = ip->i_atime;
	vap->va_atime.tv_usec = 0;
	vap->va_mtime.tv_sec = ip->i_mtime;
	vap->va_mtime.tv_usec = 0;
	vap->va_ctime.tv_sec = ip->i_ctime;
	vap->va_ctime.tv_usec = 0;
	vap->va_rdev = ip->i_rdev;
	vap->va_blocks = ip->i_blocks;
	switch(ip->i_mode & IFMT) {

	case IFBLK:
		vap->va_blocksize = BLKDEV_IOSIZE;
		break;

	case IFCHR:
		vap->va_blocksize = MAXBSIZE;
		break;

	default:
		vap->va_blocksize = ip->i_fs->fs_bsize;
		break;
	}
	ILOCK_READ_DONE(ip);
	unix_release();

	return (FS_SUCCESS);
    
}

/*
 * Perform chown operation on inode ip;
 * inode must be locked prior to call.
 */
chown1(ip, uid, gid, is_root)
	register struct inode *ip;
	int uid, gid;
	boolean_t is_root;
{
#if	QUOTA
	register long change;
#endif	QUOTA

	if (ip->i_fs->fs_ronly)
		return (EROFS);

	if (uid == -1)
		uid = ip->i_uid;
	if (gid == -1)
		gid = ip->i_gid;

	if (uid != ip->i_uid && (! is_root))
		return (EPERM);

	if (gid != ip->i_gid && !groupmember((gid_t)gid) && (! is_root))
		return (EPERM);

#if	QUOTA
	if (ip->i_uid == uid)		/* this just speeds things a little */
		change = 0;
	else
		change = ip->i_blocks;
	(void) chkdq(ip, -change, 1);
	(void) chkiq(ip->i_dev, ip, ip->i_uid, 1);
	dqrele(ip->i_dquot);
#endif	QUOTA
	ip->i_uid = uid;
	ip->i_gid = gid;
	ip->i_flag |= ICHG;
#if	TODO	
	if (u.u_ruid != 0)
#endif	TODO	
		ip->i_mode &= ~(ISUID|ISGID);
#if	QUOTA
	ip->i_dquot = inoquota(ip);
	(void) chkdq(ip, change, 1);
	(void) chkiq(ip->i_dev, (struct inode *)NULL, (uid_t)uid, 1);
	return (u.u_error);		/* should == 0 ALWAYS !! */
#else	QUOTA
	return (0);
#endif	QUOTA
}

fss_setattr(ip,vap, cred)
struct inode *ip;
fs_attr_t vap;
fs_cred_t cred;
{
	int chtime = 0;
	int error = 0;
	struct timeval atime, mtime;
	
	/*
	 * cannot set these attributes
	 */
	if ((vap->va_nlink != -1) || (vap->va_blocksize != -1) ||
	    (vap->va_rdev != -1) || (vap->va_blocks != -1) ||
	    (vap->va_fsid != -1) || (vap->va_nodeid != -1) ||
	    ((int)vap->va_type != -1)) {
		return unix_error_to_fs_error(EINVAL);
	}

	unix_master();
	ILOCK(ip);
	ICOUNT_CHECK(ip);  /* debug */

	/*
	 * Change file access modes. Must be owner or su.
	 */
	if (vap->va_mode != (u_short)-1) {
#if USE_CRED
		error = OWNER(cred, ip);
		if (error)
			goto out;
#endif USE_CRED
		ip->i_mode &= IFMT;
		ip->i_mode |= vap->va_mode & ~IFMT;
		if (cred->cr_uid != 0) {
			ip->i_mode &= ~ISVTX;
			if (!groupmember(ip->i_gid))
				ip->i_mode &= ~ISGID;
		}
		imark(ip, ICHG);
#if	TODO		
		if ((vp->v_flag & VTEXT) && ((ip->i_mode & ISVTX) == 0)) {
			inode_uncache(VTOI(vp));
		}
#endif	TODO		
	}
	/*
	 * To change file ownership, must be su.
	 * To change group ownership, must be su or owner and in target group.
	 * This is now enforced in chown1() below.
	 */
	if ((vap->va_uid != (uid_t)-1) || (vap->va_gid != (gid_t)-1)) {
		error = chown1(ip, vap->va_uid, vap->va_gid, (cred->cr_uid == 0));
		if (error)
			goto out;
	}
	/*
	 * Truncate file. Must have write permission and not be a directory.
	 */
	if (vap->va_size != (u_long)-1) {
		if ((ip->i_mode & IFMT) == IFDIR) {
			error = EISDIR;
			goto out;
		}
#if USE_ACCESS
/* XXX used to call iaccess() here.  Check this. */		
		if (error = access(ip, IWRITE)) {
			goto out;
		}
#endif USE_ACCESS
/*
 * XXX fix the file/io library not to call this for read-only files,
 * and not to call this if the size hasn't changed!
 */
 		if (ip->i_size != vap->va_size)
			error = itrunc(ip, vap->va_size);
	}
	/*
	 * Change file access or modified times.
	 */
	atime.tv_sec = ip->i_atime;
	mtime.tv_sec = ip->i_mtime;
	atime.tv_usec = 0;
	mtime.tv_usec = 0;
	if (vap->va_atime.tv_sec != -1) {
#if USE_CRED
		error = OWNER(cred, ip);
		if (error)
			goto out;
#endif USE_CRED
		if (vap->va_atime.tv_sec == 0)
			atime.tv_sec = time.tv_sec;
		else
			atime.tv_sec = vap->va_atime.tv_sec;
		chtime++;
	}
	if (vap->va_mtime.tv_sec != -1) {
#if USE_CRED
		error = OWNER(cred, ip);
		if (error)
			goto out;
#endif USE_CRED
		if (vap->va_mtime.tv_sec == 0)
			mtime.tv_sec = time.tv_sec;
		else
			mtime.tv_sec = vap->va_mtime.tv_sec;
		chtime++;
	}
	if (chtime) {
		ip->i_flag |= IACC|IUPD|ICHG;
		ip->i_ctime = time.tv_sec;
	}
out:
	iupdat(ip, &atime, &mtime, 1);	/* XXX should be asyn for perf */
	IUNLOCK(ip);
	unix_release();

	return unix_error_to_fs_error(error);
    
}

fss_access(ip,mode, cred)
struct inode *ip;
int mode;
fs_cred_t cred;
{
    int error;
    
    unix_master();
    ILOCK(ip);
    ICOUNT_CHECK(ip);  /* debug */
    error = access(ip,mode);
    IUNLOCK(ip);
    unix_release();

    return unix_error_to_fs_error(error);
}

/*
 * Assumptions:
 *
 *	1:  Name in fss_lookup() does not start with a slash.
 */


fss_lookup(dirp, name, ipp, vap, cred)
struct inode *dirp;
char *name;
struct inode **ipp;
fs_attr_t  vap;
fs_cred_t cred;
{
    struct inode *ip;
    struct nameidata nd, *ndp = &nd;
    int error;

    ndp->ni_segflg = 0;	/* XXX holdover from the kernel and is ignored*/
    ndp->ni_dirp = name;
    ndp->ni_nameiop = LOOKUP;

    unix_master();
    error = namei_in_directory(dirp, ndp, &ip); 

    if ( ! error ) {
	vap->va_type = inode_type_to_fs_type(ip->i_mode);
	vap->va_mode = ip->i_mode;
	vap->va_uid = ip->i_uid;
	vap->va_gid = ip->i_gid;
	vap->va_size = ip->i_size;

	ICOUNT_CHECK(ip);  /* debug */

	IUNLOCK(ip);
	unix_release();
	*ipp = ip;
	return FS_SUCCESS;
    }
    if (ip != NULL) {
	    iput(ip);
    }
    unix_release();

    return unix_error_to_fs_error(error);
}


fss_create(dirp, name, vap, ipp, cred)
struct inode *dirp;
char *name;
fs_attr_t vap;
struct inode **ipp;
fs_cred_t cred;
{
	struct inode *ip;
	struct nameidata nd, *ndp = &nd;
	int error;
	int dmode = (vap->va_mode & 0777) | IFREG;
	
	ip = (struct inode *) 0;	
	/*
	 * can not create directories. use ufs_mkdir
	 */
	if (vap->va_type == VDIR) {
		return unix_error_to_fs_error(EISDIR);
	}

	ndp->ni_nameiop = CREATE | LOCKPARENT;
	ndp->ni_segflg = 0; /* ignored */
	ndp->ni_dirp = name;

	unix_master();
	error = namei_in_directory(dirp, ndp, &ip);

	if (error) {
		if (ip != NULL) {
		    	*ipp = (struct inode *) NULL;
			iput(ip);
			iput(dirp);
		}
		unix_release();
		return unix_error_to_fs_error(error) ;
	}

	if (ip != NULL) {
	    	*ipp = (struct inode *) NULL;
		iput(ip);
		iput(dirp);
		unix_release();
		return unix_error_to_fs_error(EEXIST);
	}

	error = maknode(dmode, ndp, &ip, vap);

out:
	if (error)
		*ipp = (struct inode *) NULL;
	else
		*ipp = ip;

	if (ip != NULL)
		IUNLOCK(ip);   
	unix_release();
	return(unix_error_to_fs_error(error));
	 
}
/*
 * Make a new file.
 */

int 
maknode(mode, ndp, ipp, vap)
	int mode;
	struct nameidata *ndp;
	struct inode **ipp;
	fs_attr_t vap;
{
	struct inode *ip;
	register struct inode *pdir = ndp->ni_pdir;
	ino_t ipref;
	int error;

	if ((mode & IFMT) == IFDIR)
		ipref = dirpref(pdir->i_fs);
	else
		ipref = pdir->i_number;
	error = ialloc(pdir, ipref, mode, &ip);
	if (ip == NULL) {
		iput(pdir);
		*ipp = (struct inode *) NULL;
		return unix_error_to_fs_error(error);
	}
#if	QUOTA
	if (ip->i_dquot != NODQUOT)
		panic("maknode: dquot");
#endif	QUOTA
	ip->i_flag |= IACC|IUPD|ICHG;
	if ((mode & IFMT) == 0)
		mode |= IFREG;
/*
 * XXX The following is something the user should be passing in.
 * u.u_cmask set in the enviroment.
 */
	ip->i_mode = mode;
	ip->i_nlink = 1;
	ip->i_uid = vap->va_uid;
#if	TODO	
#if	CS_GENERIC
	if ((u.u_modes&UMODE_P_GID) == 0)
	    ip->i_gid = u.u_gid;
p	else
#endif	CS_GENERIC
#endif	TODO
	
	ip->i_gid = vap->va_gid;
	if (ip->i_mode & ISGID && !groupmember(ip->i_gid))
		ip->i_mode &= ~ISGID;
#if	QUOTA
	ip->i_dquot = inoquota(ip);
#endif	QUOTA

	/*
	 * Make sure inode goes to disk before directory entry.
	 */
	iupdat(ip, &time, &time, 1);
	error = direnter(ip, ndp);
	if (error) {
		/*
		 * Write error occurred trying to update directory
		 * so must deallocate the inode.
		 */
		ip->i_nlink = 0;
		ip->i_flag |= ICHG;
		iput(ip);
		*ipp = (struct inode *) NULL;
		return unix_error_to_fs_error(error);
	}
	*ipp = ip;
	return unix_error_to_fs_error(error);
}

/* DEBUG */ struct inode *delete;

fss_remove(dirp, name, cred, last_link)
struct inode *dirp;
char *name;
fs_cred_t cred;
boolean_t *last_link;  /* out arg:  was the last link removed? */
{
	struct inode *ip;
	struct nameidata nd, *ndp = &nd;
	int error;
	
	*last_link = FALSE;  /* assume last link is not removed */
	ip = (struct inode *) 0;	

	ndp->ni_nameiop = DELETE | LOCKPARENT;
	ndp->ni_segflg = 0; /* ignored */
	ndp->ni_dirp = name;

	unix_master();
	error = namei_in_directory(dirp, ndp, &ip);

	ICOUNT_CHECK(ip);  /* debug */

	if (error) {
		if (ip != NULL) {
			iput(ip);
			iput(dirp);
		}
		unix_release();
		return unix_error_to_fs_error(error) ;
	}

	if (ip == NULL) 
		panic("remove confused.");	/* XXX check this namei() should return error code */

	if (ndp->ni_pdir != dirp)
		panic("remove confused about parent"); /* XXX should be equal */

#if	TODO	
	if ((ip->i_mode&IFMT) == IFDIR && !suser())
		goto out;
	/*
	 * Don't unlink a mounted file.
	 */
	if (ip->i_dev != dp->i_dev) {
		u.u_error = EBUSY;
		goto out;
	}
#endif	TODO
	error = dirremove(ndp);
	if (! error) {
		ip->i_nlink--;
		ip->i_flag |= ICHG;
	} else {
		printf("file %s parent 0x%x inode 0x%x nlink %d icount %d\n",
		       name, dirp, ip, ip->i_nlink, ip->i_count);
		panic("fss_remove: dirremove failed");
	}

	delete = ip;  /* debug */

	if (ip->i_nlink == 0)
		*last_link = TRUE;  /* file has no links */
	
	if (dirp == ip)
		irele(ip);
	else
		iput(ip);
	iput(dirp);
	unix_release();

	return FS_SUCCESS;	
}


fss_link(dirp, target_ip, target_name, cred)
struct inode *dirp;
struct inode *target_ip;
char *target_name;
fs_cred_t cred;
{
	struct inode *ip = target_ip, *xp;
	struct nameidata nd, *ndp = &nd;
	int error = 0;

	unix_master();
	ILOCK(ip);
	ICOUNT_CHECK(ip);  /* debug */

	iincr_chk(ip);		/* bump reference count for duration of link */
	ip->i_nlink++;
	ip->i_flag |= ICHG;
	iupdat(ip, &time, &time, 1);
	IUNLOCK(ip);
	
	/*
	 * Assert that the target does not exist.
	 */
	ndp->ni_nameiop = CREATE | LOCKPARENT;
	ndp->ni_segflg = 0;
	ndp->ni_dirp = target_name;

	error = namei_in_directory(dirp, ndp, &xp);
	if (xp != NULL) {
	    iput(xp);
	    iput(dirp);
	    error = EEXIST;
	    goto out;
	}

	if (dirp->i_dev != ip->i_dev) {
		iput(dirp);
		error = EXDEV;
		goto out;
	}

	if (ndp->ni_pdir != dirp) panic("link confused");

	/* direnter loses the dirp reference, regardless of error */
	error = direnter(ip, ndp);
out:
	if (error) {
		ILOCK(ip);
		ip->i_nlink--;
		ip->i_flag |= ICHG;
		IUNLOCK(ip);
	}

	irele(ip);		/* lose the reference gained above */
	unix_release();

	return unix_error_to_fs_error(error);	
}


/*
 * A virgin directory (no blushing please).
 */
struct dirtemplate mastertemplate = {
	0, 12, 1, ".",
	0, DIRBLKSIZ - 12, 2, ".."
};

fss_mkdir(dirp, entry_name, vap, new_entry, cred)
struct inode *dirp;
char *entry_name;
fs_attr_t  vap;
struct inode **new_entry;
fs_cred_t cred;
{
    	struct inode *ip = NULL;
	struct nameidata nd, *ndp = &nd;
	struct dirtemplate dirtemplate;
	int error;
	int dmode = vap->va_mode;

	ndp->ni_nameiop = CREATE | LOCKPARENT;
	ndp->ni_segflg = 0; 
	ndp->ni_dirp = entry_name;

	unix_master();
 	error = namei_in_directory(dirp, ndp, &ip);

	ICOUNT_CHECK(ip);  /* debug */

	if (ip != NULL) {
	    	iput(ip);
	    	iput(dirp);
		unix_release();
		return unix_error_to_fs_error(EEXIST);
	}

	if (error) {
		unix_release();
		return unix_error_to_fs_error(error);
	}

	if (ndp->ni_pdir != dirp) {
	    printf("parent 0x%x dirp 0x%x\n",
	    	ndp->ni_pdir, dirp);
	    panic("mkdir confused.");
	}
	
	dmode &= 0777;
	dmode |= IFDIR;
	/*
	 * Must simulate part of maknode here
	 * in order to acquire the inode, but
	 * not have it entered in the parent
	 * directory.  The entry is made later
	 * after writing "." and ".." entries out.
	 */
	error = ialloc(dirp, dirpref(dirp->i_fs), dmode, &ip);
	if (ip == NULL) {
		iput(dirp);
		unix_release();
		return unix_error_to_fs_error(error); 	/* XXX */
	}
#if	QUOTA
	if (ip->i_dquot != NODQUOT)
		panic("mkdir: dquot");
#endif	QUOTA
	ip->i_flag |= IACC|IUPD|ICHG;
/*
 * XXX the following should be coming from the user (u.u_cmask)
 *  & ~u.u_cmask; 
 */
	ip->i_mode = dmode;
	ip->i_nlink = 2;
	ip->i_uid = vap->va_uid;
	ip->i_gid = vap->va_gid;
#if	QUOTA
	ip->i_dquot = inoquota(ip);
#endif	QUOTA
	iupdat(ip, &time, &time, 1);

	/*
	 * Bump link count in parent directory
	 * to reflect work done below.  Should
	 * be done before reference is created
	 * so reparation is possible if we crash.
	 */
	dirp->i_nlink++;
	dirp->i_flag |= ICHG;
	iupdat(dirp, &time, &time, 1);

	/*
	 * Initialize directory with "."
	 * and ".." from static template.
	 */
	dirtemplate = mastertemplate;
	dirtemplate.dot_ino = ip->i_number;
	dirtemplate.dotdot_ino = dirp->i_number;
	error = rdwri(UIO_WRITE, ip, (caddr_t)&dirtemplate,
		sizeof (dirtemplate), (off_t)0, 1, (int *)0);
	if (error) {
		dirp->i_nlink--;
		dirp->i_flag |= ICHG;
		iput(dirp);
		goto bad;
	}
	if (DIRBLKSIZ > ip->i_fs->fs_fsize)
		panic("mkdir: blksize");    /* XXX - should grow with bmap() */
	else
		ip->i_size = DIRBLKSIZ;
	/*
	 * Directory all set up, now
	 * install the entry for it in
	 * the parent directory.
	 */
	/* direnter loses the reference to dirp, regardless of error */
	error = direnter(ip, ndp);
	if (error) {
		ndp->ni_nameiop = LOOKUP | NOCACHE;
		ndp->ni_segflg = 0;
		ndp->ni_dirp = entry_name;
		error = namei_in_directory(dirp, ndp, &dirp);		
		if (dirp) {
			dirp->i_nlink--;
			dirp->i_flag |= ICHG;		    
			iput(dirp);
		}
	}
bad:
	/*
	 * No need to do an explicit itrunc here,
	 * irele will do this for us because we set
	 * the link count to 0.
	 */
		
	if (error) {
	    	*new_entry = (struct inode *) 0;
		ip->i_nlink = 0;
		ip->i_flag |= ICHG;
		iput(ip);
	}

	if (!error)  {
	    *new_entry = ip;
	    IUNLOCK(ip);
	}
	unix_release();

	return unix_error_to_fs_error(error);
}

fss_rmdir(dirp,entry_name, cred)
struct inode *dirp;
char *entry_name;
fs_cred_t cred;
{
 	struct inode *ip, *dp;
 	struct nameidata nd, *ndp = &nd;
	int error =0;
	
	ndp->ni_nameiop = DELETE | LOCKPARENT;
	ndp->ni_segflg = 0;
	ndp->ni_dirp = entry_name;

	unix_master();
	error = namei_in_directory(dirp, ndp, &ip);
	
	if (ip == NULL) {
		unix_release();
		return unix_error_to_fs_error(ENOENT);
	}
		
	ICOUNT_CHECK(ip);  /* debug */

	dp = ndp->ni_pdir;
	/*
	 * No rmdir "." please.
	 */
	if (dp == ip) {
		irele(dp); 
		iput(ip);
		unix_release();
		return NS_BAD_DIRECTORY;
	}
	if ((ip->i_mode&IFMT) != IFDIR) {
		error = ENOTDIR;
		goto out;
	}
	/*
	 * Don't remove a mounted on directory.
	 */
	if (ip->i_dev != dp->i_dev) {
		error = EBUSY;
		goto out;
	}
	/*
	 * Verify the directory is empty (and valid).
	 * (Rmdir ".." won't be valid since
	 *  ".." will contain a reference to
	 *  the current directory and thus be
	 *  non-empty.)
	 */
	if (ip->i_nlink != 2 || !dirempty(ip, dp->i_number)) {
		error = ENOTEMPTY;
		goto out;
	}
	/*
	 * Delete reference to directory before purging
	 * inode.  If we crash in between, the directory
	 * will be reattached to lost+found,
	 */
	if (error = dirremove(ndp))
		goto out;
	dp->i_nlink--;
	dp->i_flag |= ICHG;
	cacheinval(dp);
	iput(dp);
	dp = NULL;
	/*
	 * Truncate inode.  The only stuff left
	 * in the directory is "." and "..".  The
	 * "." reference is inconsequential since
	 * we're quashing it.  The ".." reference
	 * has already been adjusted above.  We've
	 * removed the "." reference and the reference
	 * in the parent directory, but there may be
	 * other hard links so decrement by 2 and
	 * worry about them later.
	 */
	ip->i_nlink -= 2;
	error = itrunc(ip, (u_long)0);
	cacheinval(ip);
out:
	if (dp)
		iput(dp);
	iput(ip);
	unix_release();
	
	return unix_error_to_fs_error(error);

}

fss_readdir(dirp, uiop, cred)
struct inode *dirp;
struct uio *uiop;
fs_cred_t cred;
{
	register struct iovec *iovp;
	register unsigned count;
	mach_error_t ret;

	iovp = uiop->uio_iov;
	count = iovp->iov_len;
	if ((uiop->uio_iovcnt != 1) || (count < DIRBLKSIZ) ||
	    (uiop->uio_offset & (DIRBLKSIZ -1)))
		return IO_BAD_ARGS;
	count &= ~(DIRBLKSIZ - 1);
	uiop->uio_resid -= iovp->iov_len - count;
	iovp->iov_len = count;

	unix_master();
	ret = unix_error_to_fs_error(rwip(dirp, uiop, UIO_READ));
	unix_release();

	return(ret);
}

/* local_name: value of the link */
fss_symlink(dirp, local_name, vap, target_name, cred)
struct inode *dirp;
char *local_name;
fs_attr_t  vap;
char *target_name;
fs_cred_t cred;
{
	struct nameidata nd, *ndp = &nd;
	struct inode *ip;
	int error;
	int dmode = (vap->va_mode & 0777) | IFLNK;
	int local_name_len;
	
        /* check that target does not exist */
	
	ndp->ni_nameiop = CREATE;
	ndp->ni_segflg = 0; /* ignored */
	ndp->ni_dirp = target_name;

	unix_master();
	error = namei_in_directory(dirp, ndp, &ip);
	if (error) {
		if (ip != NULL) {
			iput(ip);
		}
		unix_release();
		return unix_error_to_fs_error(error) ;
	}
	if (ip != NULL) {
		iput(ip);
		error = EEXIST;
		unix_release();
		return unix_error_to_fs_error(error);
	}
	
	error = maknode(dmode, ndp, &ip, vap);

 	if (error) {
		unix_release();
		return unix_error_to_fs_error(error);
	}

	local_name_len = strlen(local_name);	
#if	MACH_FASTLINK
	if (local_name_len < MAX_FASTLINK_SIZE && create_fastlinks) {
		strcpy(ip->i_symlink, local_name);
		ip->i_size = local_name_len;
		ip->i_icflags |= IC_FASTLINK;
		ip->i_flag |= IACC|IUPD|ICHG;
	} else
#endif	MACH_FASTLINK
	error = rdwri(UIO_WRITE, ip, local_name, local_name_len,
			0, UIO_SYSSPACE, (int *) 0);
	iput(ip);
	unix_release();

	return unix_error_to_fs_error(error);
}

fss_readlink(ip,uiop, cred)
struct inode *ip;
struct uio *uiop;
fs_cred_t cred;
{
    int size;
    int error;

    unix_master();
    ILOCK(ip);

#if	MACH_FASTLINK
    /*
     *	Fast symbolic link support
     */
    if ((ip->i_icflags & IC_FASTLINK) != 0 && use_fastlinks) {
	error = uiomove(ip->i_symlink, ip->i_size, UIO_READ, uiop);
    }
    else
#endif	MACH_FASTLINK
	error = rwip(ip, uiop, UIO_READ);

    IUNLOCK(ip);
    unix_release();

    return unix_error_to_fs_error(error);
    
}

fss_release(ip)
struct inode *ip;
{
    ICOUNT_CHECK(ip);  /* debug */
    /*
     * make sure the freed inode gets put back on the free list
     */
    if (ip == delete) {
	Debug(printf("inode 0x%x nlink %d icount %d\n",
		ip, ip->i_nlink, ip->i_count));
    }

    unix_master();
    irele(ip);
    unix_release();

    return FS_SUCCESS;    
}

fss_reference(ip)
struct inode *ip;
{
    unix_master();
    ILOCK(ip);
    ICOUNT_CHECK(ip);  /* debug */
    iincr_chk(ip);    
    IUNLOCK(ip);    
    unix_release();

    return FS_SUCCESS;    
}

fss_sync()
{
	unix_master();
	update();  /* update file system data structures to disk */
	unix_release();
}



int unix_error_table[] = {
/*ESUCCESS 			 Call Succeeded. */
ERR_SUCCESS,
/*EPERM		1		 Not owner */
NS_INVALID_ACCESS,
/*ENOENT	2		 No such file or directory */
NS_NOT_FOUND,
/*ESRCH		3		 No such process */
unix_err(ESRCH),
/*EINTR		4		 Interrupted system call */
unix_err(EINTR),
/*EIO		5		 I/O error */
unix_err(EIO),
/*ENXIO		6		 No such device or address */
unix_err(ENXIO),
/*E2BIG		7		 Arg list too long */
unix_err(E2BIG),
/*ENOEXEC	8		 Exec format error */
unix_err(ENOEXEC),
/*EBADF		9		 Bad file number */
unix_err(EBADF),
/*ECHILD	10		 No children */
unix_err(ECHILD),
/*EAGAIN	11		 No more processes */
unix_err(EAGAIN),
/*ENOMEM	12		 Not enough core */
unix_err(ENOMEM),
/*EACCES	13		 Permission denied */
NS_INVALID_ACCESS,
/*EFAULT	14		 Bad address */
unix_err(EFAULT),
/*ENOTBLK	15		 Block device required */
unix_err(ENOTBLK),
/*EBUSY		16		 Mount device busy */
unix_err(EBUSY),
/*EEXIST	17		 File exists */
NS_ENTRY_EXISTS,
/*EXDEV		18		 Cross-device link */
unix_err(EXDEV),
/*ENODEV	19		 No such device */
unix_err(ENODEV),
/*ENOTDIR	20		 Not a directory*/
NS_BAD_DIRECTORY,
/*EISDIR	21		 Is a directory */
NS_IS_DIRECTORY,
/*EINVAL	22		 Invalid argument */
unix_err(EINVAL),
/*ENFILE	23		 File table overflow */
unix_err(ENFILE),
/*EMFILE	24		 Too many open files */
unix_err(EMFILE),
/*ENOTTY	25		 Not a typewriter */
unix_err(ENOTTY),
/*ETXTBSY	26		 Text file busy */
unix_err(ETXTBSY),
/*EFBIG		27		 File too large */
unix_err(EFBIG),
/*ENOSPC	28		 No space left on device */
unix_err(ENOSPC),
/*ESPIPE	29		 Illegal seek */
unix_err(ESPIPE),
/*EROFS		30		 Read-only file system */
unix_err(EROFS),
/*EMLINK	31		 Too many links */
unix_err(EMLINK),
/*EPIPE		32		 Broken pipe */
unix_err(EPIPE),
0
};
int unix_error_table_size =
	sizeof(unix_error_table) / sizeof (unix_error_table[0]);
