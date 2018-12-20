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
 * $Log:	ufs_xxx.c,v $
 * Revision 1.37  94/07/21  11:59:06  mrt
 * 	updated copyright
 * 
 * Revision 1.36  93/01/20  17:40:41  jms
 * 	ninode 300 => 5000
 * 	[93/01/18  17:58:16  jms]
 * 
 * Revision 1.35  92/07/05  23:37:08  dpj
 * 	Moved main() into separate file ufs_main.cc.
 * 	[92/06/24  17:47:17  dpj]
 * 
 * Revision 1.34  92/03/05  15:16:36  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:52:52  jms]
 * 
 * Revision 1.33  91/12/20  17:45:35  jms
 * 	Cause different ufs servers to have different server ids (dpj)
 * 	[91/12/20  16:33:17  jms]
 * 
 * Revision 1.32  91/07/01  14:16:38  jms
 * 	Merge in stuff from roy@osf
 * 	[91/06/27  13:23:59  jms]
 * 
 * 	   Update args to "mach_object_start_handler".
 * 	[91/05/07  11:37:51  jms]
 * 
 *
 * Revision 1.30.3.2  91/05/29  18:35:05  roy
 * 	Inode count at 300.  Should be drastically reduced
 * 	when garbage collection from dead clients works.
 * 
 * 
 * Revision 1.30.3.1  91/05/29  11:49:48  roy
 * 	Moved various routines to their proper files,
 * 	including disk_*, binit, ihinit, uiomove.
 * 
 * 
 * Revision 1.30  90/12/21  13:56:34  jms
 * 	Bump inode count again for now.
 * 	[90/12/18  11:56:59  roy]
 * 
 * 	Merge forward.
 * 	[90/12/15  14:20:37  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:28:20  jms]
 * 
 * Revision 1.29.1.1  90/12/14  11:12:11  roy
 * 	Bumped ninode value from 100 to 200.  Cleaned up time_of_day 
 * 	thread.
 * 
 * 
 * Revision 1.29  90/11/27  18:22:33  jms
 * 	Add -stop switch (dpj)
 * 	[90/11/20  15:09:59  jms]
 * 
 * Revision 1.28  90/09/05  09:49:29  mbj
 * 	Fix bugs in disk_write and disk_open which clearly show that the new
 * 	code was never tested or even run.
 * 
 * Revision 1.27  90/08/22  18:16:36  roy
 * 	No change.
 * 	[90/08/17  13:13:32  roy]
 * 
 * 	Device name can be with or without prepended "/dev/".
 * 	[90/08/17  11:53:54  roy]
 * 
 * 	No change.
 * 	[90/08/15  16:01:36  roy]
 * 
 * 	Implemented disk_open(), disk_read(), disk_write().
 * 	Added -console option.
 * 	[90/08/14  12:42:17  roy]
 * 
 * Revision 1.26  90/03/14  17:30:56  orr
 * 	move the netname lookup after finishing the debug initialization
 * 	(so that error output doesn't hose up because it hash't been
 * 	initialized).
 * 	[90/03/14  17:09:03  orr]
 * 
 * Revision 1.25  89/11/28  19:13:15  dpj
 * 	Fixed allocbuf() to copy the old data into the new buffer
 * 	instead of just losing it.
 * 	Added error checking on vm_allocate() and vm_deallocate().
 * 	[89/11/27  13:48:27  dpj]
 * 
 * Revision 1.24  89/10/30  16:40:34  dpj
 * 	Cleaned-up initialization. Most of the work is now done
 * 	in the agency at the root of the tree.
 * 	Added some dummy code to get around a bad compiler bug on
 * 	the RT.
 * 	[89/10/27  19:54:19  dpj]
 * 
 * Revision 1.23  89/07/09  14:22:46  dpj
 * 	Added "-d<debug-level>" switch.
 * 	[89/07/08  15:47:11  dpj]
 * 
 * 	Updated diag initialization for new Diag object.
 * 	[89/07/08  13:23:58  dpj]
 * 
 * Revision 1.22  89/06/30  18:39:47  dpj
 * 	Modified to use US utility routines, and to explicitly allocate
 * 	the mgr ID.
 * 	[89/06/29  01:01:06  dpj]
 * 
 * Revision 1.21  89/05/18  13:15:52  dorr
 * 	include file cataclysm
 * 
 * Revision 1.20  89/03/17  13:09:18  sanzi
 * 	Added include of debug.h.
 * 	[89/03/14  11:39:05  dpj]
 * 	
 * 	conditionalize debug output.
 * 	[89/03/12  20:23:18  dorr]
 * 	
 * 	Dont call test_us_driver() at all.
 * 	[89/03/09  09:26:20  sanzi]
 * 	
 * 	Only call test_us_driver() if -test specified on 
 * 	command line.
 * 	[89/03/02  16:55:45  sanzi]
 * 	
 * 	Fixed includes to replace fs_export.h with fs_types.h.
 * 	[89/02/15  21:45:46  dpj]
 * 	
 * 	Check return code for front-end initialization.
 * 	Added comments for debugging loop.
 * 	[89/02/10  18:13:35  dpj]
 * 	
 * 	fix calling convention for test driver
 * 	[89/02/10  13:53:18  dorr]
 * 
 */

/* #include <sys/signal.h>	 temporary for debugging purposes. */

#include <base.h>
#include <stdio.h>
#include <mach.h>
#include <mach/message.h>
#include <cthreads.h>
#include <ns_types.h>
#include <fs_types.h>

#include "param.h"
#include "user.h"
#include "conf.h"

#include "fs.h"
#include "inode.h"
#include "buf.h"
#include "systm.h"
#include "uio.h"

#include <mach.h>
#include <mach_error.h>
#include "mach_init.h"

#include <sys/file.h>
#include <mach/time_value.h>

extern struct inode * open_file();
extern struct user *ustruct_init();

struct linesw linesw[1];
struct bdevsw bdevsw[1];
struct cdevsw cdevsw[1];

struct user	u;

int	diagnostic_level;
int	nbuf = 10;
int	ninode = 5000;


#if !defined(MACH3_UNIX)
time_of_day_thread()
{
   	mach_port_t 		time_port;
	mach_msg_header_t 	msg_header;
	time_value_t 		time_value;
	mach_error_t 		err;
	mach_msg_timeout_t	timeout = 1000;  /* millisec */

	Debug(printf("time_of_day_thread started...\n"));

	if ((err = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				      &time_port)) != ERR_SUCCESS) 
		mach_error("time_of_day_thread.mach_port_allocate", err);

	while (1) {
		if ((err = mach_get_time(&time_value)) != ERR_SUCCESS)
			mach_error("time_of_day_thread.mach_get_time", err);

		/*
		 * Set the global 'time' variable.
		 */
		time.tv_sec = time_value.seconds;
		time.tv_usec = time_value.microseconds;

		/* 
		 * Delay for a while.
		 */
		msg_header.msgh_size = sizeof(msg_header);
		msg_header.msgh_local_port = time_port;
		err = mach_msg(&msg_header, MACH_RCV_MSG|MACH_RCV_TIMEOUT, 
			       0, msg_header.msgh_size, time_port, timeout,
			       MACH_PORT_NULL);

		if (err != MACH_RCV_TIMED_OUT) 
			mach_error("time_of_day_thread.mach_msg", err);

	}
}
#else !defined(MACH3_UNIX)
time_of_day_thread()
{
 	typedef struct {
	mach_msg_header_t	head;
	char		body[MACH_MSG_SIZE_MAX-sizeof(mach_msg_header_t)];
	} msg_t;

    	mach_port_t		time_port;
	msg_t 			in_msg;
	mach_error_t 		err;
	time_value_t 		time_value;
	mach_msg_timeout_t		timeout = 1000;  /* millisec */

	Debug(printf("time_of_day_thread started...\n"));

	if ((err = port_allocate( mach_task_self(), &time_port))
	    != ERR_SUCCESS) 
		mach_error("time_of_day_thread.port_allocate", err);

    	in_msg.head.msg_local_port = time_port;
	in_msg.head.msg_size = sizeof(in_msg);

	while (1) {
		if ((err = mach_get_time(&time_value)) != ERR_SUCCESS)
			mach_error("time_of_day_thread.mach_get_time", err);

		/*
		 * Set the global 'time' variable.
		 */
		time.tv_sec = time_value.seconds;
		time.tv_usec = time_value.microseconds;

		/* 
		 * Delay for a while.
		 */
		err = msg_receive( &in_msg, RCV_TIMEOUT, timeout );
		if (err != RCV_TIMED_OUT) 
			mach_error("time_of_day_thread.msg_receive", err);

	}
}
#endif !defined(MACH3_UNIX)


microtime(tp)
struct timeval *tp;
{
    tp->tv_sec = time.tv_sec;
    tp->tv_usec = time.tv_usec;
}


#define	SLOP		32
#define MAXTSIZ         ((6*2048-SLOP) * 1024)  /* max text size */
#define	DFLDSIZ		(6*1024*1024)		/* initial data size limit */
#define MAXDSIZ         ((12*1024-32-SLOP) * 1024)  /* max data size */
#define	DFLSSIZ		(512*1024)		/* initial stack size limit */
#define MAXSSIZ         ((12*1024-32-SLOP) * 1024) /* max stack size */


struct rlimit vm_initial_limit_stack = { DFLSSIZ, MAXSSIZ };
struct rlimit vm_initial_limit_data = { DFLDSIZ, MAXDSIZ };
struct rlimit vm_initial_limit_core = { RLIM_INFINITY, RLIM_INFINITY };

struct user *ustruct_init(user)
struct user *user;
{
	user->u_rlimit[RLIMIT_STACK] = vm_initial_limit_stack;
	user->u_rlimit[RLIMIT_DATA] = vm_initial_limit_data;
	user->u_rlimit[RLIMIT_CORE] = vm_initial_limit_core;
	user->u_rlimit[RLIMIT_FSIZE] = vm_initial_limit_core;
	return(user);
}

groupmember(x)
int x; /* FOR LINT */
{
	return(1);
}

tablefull()
{
	panic( "nyi: tablefull" );
	return( 0 );
}


cleanup()
{
    printf("Updating all modified files.\n");
    update();	/* XXX update all open files */
    printf("Done.\n");
}
    
/*
 * DEBUGGING and TEMPORARY
 */
struct inode * 
open_file(name,mode,prot)
char *name;
int mode, prot;
{
    struct nameidata nd, *ndp = &nd;
    int error;
    struct inode *ip;

    ndp->ni_segflg = 0;	/* XXX holdover from the kernel and is ignored*/
    ndp->ni_dirp = name;
    ndp->ni_nameiop = FOLLOW;

    error = namei_in_directory(rootdir, ndp, &ip);     

    Debug(printf("open_file %s inode %x refs %x error %d\n",name,ip,ip->i_count,error));
    if ( ! error )  return(ip);
    else return(0);
    
}

/*
 * These are too restrictive if we are mapping files.
 */
write_to_inode(inode, buf, len)
	struct inode * inode;
	char * buf;
	int len;
{
	int error;
	struct uio uio;
	struct iovec iov;

	uio.uio_iovcnt = 1;
	uio.uio_offset = 0;
	uio.uio_iov = &iov;
	uio.uio_resid = len;

	iov.iov_base = buf;
	iov.iov_len = len;

	error = rwip( inode, &uio, UIO_WRITE );
	if ( error ) return( -1 );

	return( len - uio.uio_resid );
    	
}

read_from_inode(inode, buf, len)
	struct inode * inode;
	char * buf;
	int len;
{
	int error;
	struct uio uio;
	struct iovec iov;

	uio.uio_iovcnt = 1;
	uio.uio_offset = 0;
	uio.uio_iov = &iov;
	uio.uio_resid = len;

	iov.iov_base = buf;
	iov.iov_len = len;

	error = rwip( inode, &uio, UIO_READ );
	if ( error ) return( 0 );

	return( len - uio.uio_resid );
}


