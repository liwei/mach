/* 
 * Mach Operating System
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dir.h	7.1 (Berkeley) 6/4/86
 */
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach3-rcs/us/local_include/fs_types.h,v $
 *
 * Purpose: Definitions for the interface between the file system storage
 *	interface (fss) and the standard library for file servers (fs).
 *
 * HISTORY:
 * $Log:	fs_types.h,v $
 * Revision 2.5  92/07/05  23:33:10  dpj
 * 	Added a few extern declarations to reduce compiler warnings.
 * 	Fixed various syntax for new C++ compiler.
 * 	[92/06/24  17:34:40  dpj]
 * 
 * Revision 2.4  91/11/06  14:13:31  jms
 * 	Ifdef'd the constructs related to C++ so that this file
 * 	could still be included by C files.
 * 	[91/10/03  15:04:03  pjg]
 * 
 * 	Changed struct fs_cred to struct fs_cred_s to avoid
 * 	name collision with the C++ fs_cred class.
 * 	[91/09/26  18:31:25  pjg]
 * 
 * Revision 2.3  89/05/17  16:47:52  dorr
 * 	include file cataclysm
 * 
 * Revision 2.2  89/03/17  12:16:44  sanzi
 * 	Created to replace fs_export.h and fs_dir.h.
 * 	[89/02/15  21:31:59  dpj]
 * 
 */

#ifndef	_FS_TYPES_H_
#define	_FS_TYPES_H_

#include	<sys/types.h>
#include	<sys/time.h>


/*
 * Identifier for one FS node (e.g. vnode).
 */
typedef int	fs_id_t;


/*
 * Credentials information.
 */
#ifndef	NGROUPS
#define	NGROUPS	16
#endif	NGROUPS
#if	__cplusplus
typedef struct fs_cred_data {
#else	__cplusplus
typedef struct fs_cred {
#endif	__cplusplus
	u_short	cr_ref;			/* reference count */
	short   cr_uid;			/* effective user id */
	short   cr_gid;			/* effective group id */
	short   cr_groups[NGROUPS];	/* groups, 0 terminated */
	short   cr_ruid;		/* real user id */
	short   cr_rgid;		/* real group id */
} *fs_cred_t;

    
/*
 * FS attributes.  A field value of -1
 * represents a field whose value is unavailable
 * (getattr) or which is not to be changed (setattr).
 */
/*
 * FS types. VNON means no type.
 */
enum vtype 	{ VNON, VREG, VDIR, VBLK, VCHR, VLNK, VSOCK, VBAD, VFIFO,
		  VSTR  };
 
typedef struct fs_attr {
	enum vtype	va_type;	/* vnode type (for create) */
	u_short		va_mode;	/* files access mode and type */
	short		va_uid;		/* owner user id */
	short		va_gid;		/* owner group id */
	long		va_fsid;	/* file system id (dev for now) */
	long		va_nodeid;	/* node id */
	short		va_nlink;	/* number of references to file */
	u_long		va_size;	/* file size in bytes (quad?) */
	long		va_blocksize;	/* blocksize preferred for i/o */
	struct timeval	va_atime;	/* time of last access */
	struct timeval	va_mtime;	/* time of last modification */
	struct timeval	va_ctime;	/* time file ``created */
	dev_t		va_rdev;	/* device the file represents */
	long		va_blocks;	/* kbytes of disk space held by file */
} *fs_attr_t;

#ifdef	notdef

/*
 * flags for above
 */
#define IO_UNIT		0x01		/* do io as atomic unit for VOP_RDWR */
#define IO_APPEND	0x02		/* append write for VOP_RDWR */
#define IO_SYNC		0x04		/* sync io for VOP_RDWR */
#define	IO_NDELAY	0x08		/* non-blocking i/o for fifos */

/*
 *  Modes. Some values same as Ixxx entries from inode.h for now
 */
#define	VSUID	04000		/* set user id on execution */
#define	VSGID	02000		/* set group id on execution */
#define VSVTX	01000		/* save swapped text even after use */
#define	VREAD	0400		/* read, write, execute permissions */
#define	VWRITE	0200
#define	VEXEC	0100

#endif	notdef


/*
 * A directory consists of some number of blocks of DIRBLKSIZ
 * bytes, where DIRBLKSIZ is chosen such that it can be transferred
 * to disk in a single atomic operation (e.g. 512 bytes on most machines).
 *
 * Each DIRBLKSIZ byte block contains some number of directory entry
 * structures, which are of variable length.  Each directory entry has
 * a struct direct at the front of it, containing its inode number,
 * the length of the entry, and the length of the name contained in
 * the entry.  These are followed by the name padded to a 4 byte boundary
 * with null bytes.  All names are guaranteed null terminated.
 * The maximum length of a name in a directory is MAXNAMLEN.
 *
 * The macro DIRSIZ(dp) gives the amount of space required to represent
 * a directory entry.  Free space in a directory is represented by
 * entries which have dp->d_reclen > DIRSIZ(dp).  All DIRBLKSIZ bytes
 * in a directory block are claimed by the directory entries.  This
 * usually results in the last entry in a directory having a large
 * dp->d_reclen.  When entries are deleted from a directory, the
 * space is returned to the previous entry in the same directory
 * block by increasing its dp->d_reclen.  If the first entry of
 * a directory block is free, then its dp->d_ino is set to 0.
 * Entries other than the first in a directory do not normally have
 * dp->d_ino set to 0.
 */
/* so user programs can just include dir.h */
#if !defined(UFS_USER) && !defined(DEV_BSIZE)
#define	DEV_BSIZE	512
#endif
#define DIRBLKSIZ	DEV_BSIZE
#define	MAXNAMLEN	255

struct	direct {
	u_long	d_ino;			/* inode number of entry */
	u_short	d_reclen;		/* length of this record */
	u_short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};


/*
 * Extern declarations for low-level operations.
 */
extern fss_open();
extern fss_close();
extern fss_pagein();
extern fss_pageout();
extern fss_rdwr();
extern fss_getattr();
extern fss_setattr();
extern fss_access();
extern fss_lookup();
extern fss_create();
extern fss_remove();
extern fss_link();
extern fss_mkdir();
extern fss_rmdir();
extern fss_readdir();
extern fss_symlink();
extern fss_readlink();
extern fss_release();
extern fss_reference();
extern fss_sync();


#endif	_FS_TYPES_H_
