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
 * $Log:	disk.c,v $
 * Revision 2.5  94/07/21  11:57:46  mrt
 * 	updated copyright
 * 
 * Revision 2.4  93/01/20  17:40:00  jms
 * 	Hooks to use bug fixes from roy@osf.org
 * 	[93/01/18  17:47:20  jms]
 * 
 * Revision 2.3  92/03/05  15:15:45  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:49:20  jms]
 * 
 * Revision 2.2  91/07/01  14:16:03  jms
 * 	Errors not tolerated on disk I/O.
 * 	[91/05/29  11:00:04  roy]
 * 
 * Revision 2.1.2.1  90/11/05  18:45:28  roy
 * 	No change.
 * 
 * 
 * Revision 2.1.1.2  90/11/05  17:18:00  roy
 * 	Add disk_init() for MACH3_UNIX case.
 * 
 * 
 * Revision 2.1.1.1  90/10/29  15:33:10  roy
 * 	Initial Revision.
 * 
 * 
 *
 */

/*
 * Routines implementing disk device abstraction.
 */

#include <mach.h>
#include <base.h>
#include <sys/file.h>	/* to get modes for open */
#include "disk.h"

#if !defined(MACH3_UNIX)
#include <device/device_types.h>
#include <device/device.h>
#include <mach_privileged_ports.h>
#endif !defined(MACH3_UNIX)



#if !defined(MACH3_UNIX)

/* 
 * port to the open device
 */
device_t device = MACH_PORT_NULL;
int disk_write_debug = 0;

void disk_init()
{
	/*
	 * Startup the thread responsible for handling device replies.
	 */
	device_reply_hdlr();
}


mach_port_t disk_device(dev)
	dev_t	dev;
{
	return(device);		/* XXX Should be from table of open devices */
}

int disk_blksize(dev)
	dev_t	dev;
{
	return(512);	/* XXX How to get the actual disk block size? */
}

mach_error_t disk_open(name, mode)
	char 		*name;
	int  		mode;
{
	mach_error_t 	err;
	dev_mode_t 	dev_mode;
	dev_name_t 	dev_name;
	mach_port_t 	device_server_port = MACH_PORT_NULL;

	if (mode == O_RDWR)
		dev_mode = D_READ|D_WRITE;
	else
		dev_mode = D_READ;

	/*
	 * Strip off "/dev/" from the name, if it exists.
	 */
	if (!strncmp("/dev/", name, 5)) 
		strcpy(dev_name, name+5);
	else
		strcpy(dev_name, name);

	if ((device_server_port = mach_device_server_port()) == MACH_PORT_NULL) {
		printf("Error: Can't get device_server_port\n");
		return(KERN_FAILURE);
	}

	if ((err = device_open(device_server_port, dev_mode, 
			       dev_name, &device)) != ERR_SUCCESS) {
		printf("device_open error:  err = 0x%x = %s\n", 
		       err, mach_error_string(err));
	}

	return(err);
}


mach_error_t disk_read(dev, blkno, size, addr, count)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	vm_address_t 	*addr;
	unsigned int 	*count;
{
	mach_error_t err;

	/* Debug(printf("disk_read() blkno=%d, size=%d\n",
			 blkno, size));  */

	if ((err = device_read(device, D_READ, (recnum_t) blkno, size,
			  (io_buf_ptr_t *) addr, count)) 
	    != ERR_SUCCESS || size != *count) {
		printf("device_read error:  block = %d, size = %d, count = %d, err = 0x%x = %s\n", blkno, size, *count, err, mach_error_string(err));
		panic("disk_read");
	}

	return(err);
}


mach_error_t disk_read_async(dev, blkno, size, reply_port)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	mach_port_t	reply_port;
{
	mach_error_t err;

	if ((err = device_read_request(device, reply_port, D_READ, 
				     (recnum_t) blkno, size)) != ERR_SUCCESS) {
		printf("device_read_request error:  block = %d, size = %d, err = 0x%x = %s\n", blkno, size, err, mach_error_string(err));
		panic("disk_read_async");
	}

	return(err);
}


mach_error_t disk_write(dev, blkno, size, addr, count)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	vm_address_t 	addr;
	unsigned int 	*count;
{
	mach_error_t 	err;

	if (disk_write_debug) {
		printf("device_write:  block = %d, size = %d\n", blkno, size);
	}

	*count = size;
	if ((err = device_write(device, D_WRITE, (recnum_t) blkno,
			    (io_buf_ptr_t) addr, size, count)) 
	    != ERR_SUCCESS || size != *count) {
		printf("device_write error:  block = %d, size = %d, count = %d, err = 0x%x = %s\n", blkno, size, *count, err, mach_error_string(err));
		panic("disk_write");
	}

	return(err);
}


mach_error_t disk_write_async(dev, blkno, size, addr, reply_port)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	vm_address_t 	addr;
	mach_port_t	reply_port;
{
	mach_error_t 	err;

	if (disk_write_debug) {
		printf("device_write_async:  block = %d, size = %d, port = 0x%x\n", blkno, size, reply_port);
	}

	if ((err = device_write_request(device, reply_port, D_WRITE, 
					(recnum_t) blkno, (io_buf_ptr_t) addr, 
					size)) != ERR_SUCCESS) {
		printf("device_write_request error:  block = %d, size = %d, err = 0x%x = %s\n", blkno, size, err, mach_error_string(err));
		panic("disk_write_async");
	}

	return(err);
}


#else !defined(MACH3_UNIX)

#include <param.h>

/* 
 * file descriptor for the open device
 */
int fd;

void disk_init()
{
	 /* Nothing to do. */
}


mach_error_t disk_open(name, mode)
	char *name;
	int  mode;
{
	mach_error_t err;
	extern int errno;

        if ((fd = open(name, mode, 0777)) < 0) {
		mach_error("disk_open.open", unix_err(errno));
		return(KERN_FAILURE);
	}

	return(ERR_SUCCESS);
}


mach_error_t disk_read(dev, blkno, size, addr, count)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	vm_address_t 	*addr;
	unsigned int 	*count;
{
	mach_error_t 	err;
	extern int 	errno;

	/* XXX for now, ignore device number... */
	errno = 0;

	if (lseek(fd, dbtob(blkno), 0) < 0) {
		mach_error("disk_read.lseek", unix_err(errno));
		return(unix_err(errno));
	}

	if ((err = vm_allocate(mach_task_self(), addr, size, TRUE)) 
	    != KERN_SUCCESS) {
		mach_error("disk_read.vm_allocate", err);
		return(err);
	}
	
	*count = read(fd, *addr, size);

	if (errno != 0) {
		mach_error("disk_read.read", unix_err(errno));
		err = vm_deallocate(mach_task_self(), *addr, size);
		if (err != ERR_SUCCESS) {
			printf("disk_read(): cannot deallocate buffer: %s\n",
				mach_error_string(err));
		}
		return(unix_err(errno));
	} else
		return(KERN_SUCCESS);
}


mach_error_t disk_write(dev, blkno, size, addr, count)
	dev_t 		dev;
	daddr_t 	blkno;
	int 		size;
	vm_address_t 	addr;
	unsigned int 	*count;
{
	mach_error_t 	err;
	extern int 	errno;

	/* XXX for now, ignore device number... */
	errno = 0;

	if (lseek(fd, dbtob(blkno), 0) < 0) {
		mach_error("disk_write.lseek", unix_err(errno));
		return(unix_err(errno));
	}

	if ((*count = write(fd, addr, size)) < 0) {
		mach_error("disk_write.write", unix_err(errno));
		return(unix_err(errno));
	} else
		return(ERR_SUCCESS);

}


#endif !defined(MACH3_UNIX)


#if 0

mach_error_t disk_read_hook(dev, blkno, size, addr, count)
	dev_t dev;
	daddr_t blkno;
	int size;
	vm_address_t *addr;
	unsigned int *count;
{
	mach_error_t err;
	extern boolean_t no_disk;

	/* no_disk is used for performance measurements */
	if (no_disk) {
		err = vm_allocate(mach_task_self(), addr, size, TRUE);
		if (err != KERN_SUCCESS) {
			printf("no_disk: vm_allocate failed, err=%s\n",mach_error_string(err));
			return(err);
		}
		*count = size;
	} else
		return(disk_read(dev, blkno, size, addr, count));

}
	
#endif 0






