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
 * tty_io_procs.c
 *
 * Purpose: tty_io_mgr i/o routine implementations.
 *
 * Michael B. Jones  --  31-Oct-1988
 *
 * HISTORY:
 * $Log:	tty_io_procs.c,v $
 * Revision 1.12  94/07/21  16:14:55  mrt
 * 	Updated copyright
 * 
 * Revision 1.11  94/01/11  18:12:05  jms
 * 	Add probe/select support.
 * 	Cope with interupts durring "open" and "close"
 * 	Misc bug fixes.
 * 	Much debugging stuff.
 * 	[94/01/10  13:51:29  jms]
 * 
 * Revision 1.10  92/07/05  23:36:25  dpj
 * 	Removed include of mach_object.h.
 * 	[92/05/10  01:33:58  dpj]
 * 
 * 	Added include of mach_object.h.
 * 	[92/04/17  16:57:15  dpj]
 * 
 * Revision 1.9  92/03/05  15:15:30  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:47:54  jms]
 * 
 * Revision 1.8  91/07/01  14:15:42  jms
 * 	Do per thread "uareas" instead of just one.
 * 	[91/06/25  13:30:02  jms]
 * 
 * 	pickup mainline changes
 * 	[91/04/15  18:07:10  jms]
 * 
 * Revision 1.7  91/03/25  14:15:20  jjc
 * 	Added ns_access_to_flags() to convert NSR_READ | NSR_WRITE
 * 	to FREAD | FWRITE for tty_open_proc().
 * 	[91/02/18            jjc]
 * 
 * Revision 1.6  89/07/09  14:22:13  dpj
 * 	Updated debugging statements for new DEBUG macros.
 * 	[89/07/08  13:19:54  dpj]
 * 
 * Revision 1.5  89/03/17  13:05:31  sanzi
 * 	IO_INVALID_RECNUM -> IO_INVALID_OFFSET.
 * 	[89/02/22  14:58:35  dpj]
 * 
 * Revision 1.4  88/12/05  01:12:06  mbj
 * Return IO_INVALID_RECNUM for end-of-file as per i/o protocol.
 * 
 * Revision 1.3  88/11/16  14:44:12  mbj
 * Leave thread & task defitions alone.
 * 
 * Revision 1.2  88/11/16  11:32:38  mbj
 * Build without -DKERNEL or multitudes of feature .h files.
 * 
 * Revision 1.1  88/11/01  14:13:35  mbj
 * Initial revision
 * 
 * 31-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

#include <base.h>
#include <mach/mach_types.h>
#include <io_types.h>
#include <exception_error.h>

#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/uio.h>
#include <sys/conf.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/thread.h>

#include <tty_name.h>

#undef MIN
#undef MAX


/*
 * Debugging flag.
 */
int tty_procs_debug = 1;


int io_access_to_mode(access) int access;
{
    return access & 3;	/* XXX */
}

int io_mode_to_flags(mode) int mode;
{
    return 3;	/* XXX */
}

int ns_access_to_flags(access)
    ns_access_t access;
{
    int flags;

    flags = 0;
    if (access & NSR_READ) flags |= FREAD;
    if (access & NSR_WRITE) flags |= FWRITE;
    return (flags);
}

mach_error_t tty_open_proc(dev, mode, userid)
    dev_t dev; int mode; int userid;
{
    register mach_error_t	ret;
    register tty_name *namep;

    DEBUG1(tty_procs_debug,(Diag,"tty_open_proc(dev = 0x%x, mode = 0x%x, userid = %d)\n",
	dev, mode, userid));

    if (! (namep = tty_lookup(dev))) {
	return EBADF;
    }

    DEBUG0(1,(Diag,"tty_io_proc _setjmp: u_qsave 0x%x th:0x%x\n", &u.u_qsave,mach_thread_self()));

    if (_setjmp(&u.u_qsave)) {
#if US
        DEBUG0(1,(Diag,"tty_io_proc: open_proc INTR th:0x%x\n", mach_thread_self()));
	tty_release_thread_info();
	return EXCEPT_SOFTWARE;
#else US
	/* Never run this code */
	if (auio.uio_resid == count) {
	    if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0)
		tty_release_thread_info();
		return EINTR;
	    else
		goto restartsys;
	}
#endif US
    } 
    else {
	DEBUG0(1,(Diag,"tty_io_proc: open_proc before call th:0x%x\n", mach_thread_self()));
	ret = (*cdevsw[major(dev)].d_open)(dev, mode);
	DEBUG0(1,(Diag,"tty_io_proc: open_proc after call th:0x%x\n", mach_thread_self()));
    }

    tty_release_thread_info();
    return (ret);
}

mach_error_t tty_read_proc(dev, flags, addr, size)
    dev_t dev; int flags; char *addr; int *size;
{
    struct uio auio;
    struct iovec aiov;
    int count;
    int error;
    register tty_name *namep;

    DEBUG1(tty_procs_debug,(Diag,"tty_read_proc(dev = 0x%x, flags = 0x%x, addr = 0x%x, *size = %d)\n",
	dev, flags, addr, *size));

    if (! (namep = tty_lookup(dev))) {
	tty_release_thread_info();
	return EBADF;
    }

restartsys:
    auio.uio_iov = &aiov;
    auio.uio_iovcnt = 1;
    aiov.iov_base = addr;
    aiov.iov_len = *size;

    auio.uio_segflg = UIO_USERSPACE;
    if (aiov.iov_len < 0) {
	tty_release_thread_info();
	return EINVAL;
    }
    auio.uio_resid = aiov.iov_len;

    count = auio.uio_resid;
    auio.uio_offset = 0;

    DEBUG0(1,(Diag,"tty_io_proc _setjmp: u_qsave 0x%x th:0x%x\n", &u.u_qsave,mach_thread_self()));

    if (_setjmp(&u.u_qsave)) {
#if US
        DEBUG0(1,(Diag,"tty_io_proc: read_proc INTR th:0x%x\n", mach_thread_self()));
	tty_release_thread_info();
	return EXCEPT_SOFTWARE;
#else US
	if (auio.uio_resid == count) {
	    if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0)
		tty_release_thread_info();
		return EINTR;
	    else
		goto restartsys;
	}
#endif US
    } 
    else {
	DEBUG0(1,(Diag,"tty_io_proc: read_proc before call th:0x%x\n", mach_thread_self()));
	error = (*cdevsw[major(dev)].d_read)(dev, &auio);
	DEBUG0(1,(Diag,"tty_io_proc: read_proc after call th:0x%x\n", mach_thread_self()));
    }
    if (error) {
	tty_release_thread_info();
	return error;
    }
    *size = (count - auio.uio_resid);

    if (*size == 0) return IO_INVALID_OFFSET;	/* EOF indication */

    tty_release_thread_info();
    return ERR_SUCCESS;
}

mach_error_t tty_write_proc(dev, flags, addr, size)
    dev_t dev; int flags; char *addr; int *size;
{
    struct uio auio;
    struct iovec aiov;
    int count;
    int error;
    register tty_name *namep;

    DEBUG1(tty_procs_debug,(Diag,"tty_write_proc(dev = 0x%x, flags = 0x%x, addr = 0x%x, *size = 0x%x)\n",
	dev, flags, addr, *size));

    if (! (namep = tty_lookup(dev))) {
	tty_release_thread_info();
	return EBADF;
    }

restartsys:
    auio.uio_iov = &aiov;
    auio.uio_iovcnt = 1;
    aiov.iov_base = addr;
    aiov.iov_len = *size;

    auio.uio_segflg = UIO_USERSPACE;
    if (aiov.iov_len < 0) {
	tty_release_thread_info();
	return EINVAL;
    }
    auio.uio_resid = aiov.iov_len;

    count = auio.uio_resid;
    auio.uio_offset = 0;
    if (_setjmp(&u.u_qsave)) {
#if US
        DEBUG1(1,(Diag,"tty_io_proc: write_proc INTR th:0x%x\n", mach_thread_self()));
	tty_release_thread_info();
	return EXCEPT_SOFTWARE;
#else US
	if (auio.uio_resid == count) {
	    if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0) {
		tty_release_thread_info();
		return EINTR;
	    }
	    else {
		goto restartsys;
	    }
	}
#endif US
    } 
    else {
	error = (*cdevsw[major(dev)].d_write)(dev, &auio);
    }
    if (error) {
	tty_release_thread_info();
	return error;
    }
    *size = (count - auio.uio_resid);

    tty_release_thread_info();
    return ERR_SUCCESS;
}

mach_error_t tty_bsd_ioctl_proc(dev, com, arg, userid)
    dev_t dev; unsigned long com; char *arg; int userid;
{
    register u_int size;
    char data[IOCPARM_MASK+1];
    int error;
    register tty_name *namep;

    DEBUG1(tty_procs_debug,(Diag,"tty_bsd_ioctl_proc(dev = 0x%x, com = 0x%x, arg = 0x%x, userid = 0x%x)\n",
	dev, com, arg, userid));

    if (! (namep = tty_lookup(dev))) {
	tty_release_thread_info();
	return EBADF;
    }

restartsys:
    /*
     * Interpret high order word to find
     * amount of data to be copied to/from the
     * user's address space.
     */
    size = (com &~ (IOC_INOUT|IOC_VOID)) >> 16;
    if (size > sizeof (data)) {
	tty_release_thread_info();
	return EFAULT;
    }
    if (com&IOC_IN) {
	if (size) {
	    error =
		copyin(arg, (caddr_t)data, (u_int)size);
	    if (error) {
		tty_release_thread_info();
		return error;
	    }
	} else
	    *(caddr_t *)data = arg;
    } else if ((com&IOC_OUT) && size)
	/*
	 * Zero the buffer on the stack so the user
	 * always gets back something deterministic.
	 */
	bzero((caddr_t)data, size);
    else if (com&IOC_VOID)
	*(caddr_t *)data = arg;

    if (_setjmp(&u.u_qsave)) {
#if US
        DEBUG1(1,(Diag,"tty_io_proc: ioctl_proc INTR th:0x%x\n", mach_thread_self()));
	tty_release_thread_info();
	return EXCEPT_SOFTWARE;
#else US
	if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0) {
	    tty_release_thread_info();
	    return EINTR;
	}
	goto restartsys;
#endif US
    }
    error = (*cdevsw[major(dev)].d_ioctl)(dev, com, data, 0);
    /*
     * Copy any data to user, size was
     * already set and checked above.
     */
    if (error == 0 && (com&IOC_OUT) && size)
	error = copyout(data, arg, (u_int)size);

    tty_release_thread_info();
    return error;
}

mach_error_t tty_end_proc(dev)
    dev_t dev;
{
    register mach_error_t	ret;
    register tty_name *namep;

    DEBUG1(tty_procs_debug,(Diag,"tty_end_proc(dev = 0x%x)\n", dev));

    if (! (namep = tty_lookup(dev))) {
	return EBADF;
    }

    DEBUG0(1,(Diag,"tty_io_proc _setjmp: u_qsave 0x%x th:0x%x\n", &u.u_qsave,mach_thread_self()));

    if (_setjmp(&u.u_qsave)) {
#if US
        DEBUG0(1,(Diag,"tty_io_proc: end_proc INTR th:0x%x\n", mach_thread_self()));
	tty_release_thread_info();
	return EXCEPT_SOFTWARE;
#else US
	/* Never run this code */
	if (auio.uio_resid == count) {
	    if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0)
		tty_release_thread_info();
		return EINTR;
	    else
		goto restartsys;
	}
#endif US
    } 
    else {
	DEBUG0(1,(Diag,"tty_io_proc: end_proc before call th:0x%x\n", mach_thread_self()));
	ret = (*cdevsw[major(dev)].d_close)(dev, 0);
	DEBUG0(1,(Diag,"tty_io_proc: end_proc after call th:0x%x\n", mach_thread_self()));
    }

    tty_release_thread_info();
    return (ret);
}
