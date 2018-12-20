/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 * $Log:	device_utils.c,v $
 * Revision 2.3  94/07/21  16:14:35  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  90/10/02  11:37:03  mbj
 * 	Paul Neves' pure kernel tty_server changes
 * 	[90/09/10  17:50:22  mbj]
 * 
 */
/*
 * Support routines for device interface in out-of-kernel kernel.
 */

#include <uxkern/device_utils.h>
#include <sys/errno.h>

#include <sys/queue.h>
#include <sys/zalloc.h>
#include <cthreads.h>

/*
 * device_number to pointer hash table.
 */
#define	NDEVHASH	8
#define	DEV_NUMBER_HASH(dev)	\
		(major(dev) & minor(dev) & (NDEVHASH-1))

struct mutex	dev_number_hash_lock = MUTEX_INITIALIZER;
queue_head_t	dev_number_hash_table[NDEVHASH];

struct dev_entry {
	queue_chain_t	chain;
	char *		object;	/* anything */
	dev_t		dev;
};

zone_t	dev_entry_zone;

void dev_utils_init()
{
	register int	i;

	for (i = 0; i < NDEVHASH; i++)
	    queue_init(&dev_number_hash_table[i]);

	dev_entry_zone =
		zinit((vm_size_t)sizeof(struct dev_entry),
		      (vm_size_t)sizeof(struct dev_entry)
			* 4096,
		      vm_page_size,
		      FALSE,	/* must be wired because inode_pager
				   uses these structures */
		      "device to device_request port");
}

/*
 * Enter a device in the devnumber hash table.
 */
void dev_number_hash_enter(dev, object)
	dev_t	dev;
	char *	object;
{
	register struct dev_entry *de;
	register queue_t	q;

	de = (struct dev_entry *)zalloc(dev_entry_zone);
	de->dev = dev;
	de->object = object;

	mutex_lock(&dev_number_hash_lock);

	q = &dev_number_hash_table[DEV_NUMBER_HASH(dev)];
	enqueue_tail(q, (queue_entry_t)de);

	mutex_unlock(&dev_number_hash_lock);
}

/*
 * Remove a device from the devnumber hash table.
 */
void dev_number_hash_remove(dev)
	dev_t	dev;
{
	register struct dev_entry *de;
	register queue_t	q;

	q = &dev_number_hash_table[DEV_NUMBER_HASH(dev)];

	mutex_lock(&dev_number_hash_lock);

	for (de = (struct dev_entry *)queue_first(q);
	     !queue_end(q, (queue_entry_t)de);
	     de = (struct dev_entry *)queue_next(&de->chain)) {
	    if (de->dev == dev) {
		queue_remove(q, de, struct dev_entry *, chain);
		zfree(dev_entry_zone, (vm_offset_t)de);
		break;
	    }
	}

	mutex_unlock(&dev_number_hash_lock);
}

/*
 * Map a device to an object.
 */
char *
dev_number_hash_lookup(dev)
	dev_t	dev;
{
	register struct dev_entry *de;
	register queue_t	q;
	char *	object = 0;

	q = &dev_number_hash_table[DEV_NUMBER_HASH(dev)];
	mutex_lock(&dev_number_hash_lock);

	for (de = (struct dev_entry *)queue_first(q);
	     !queue_end(q, (queue_entry_t)de);
	     de = (struct dev_entry *)queue_next(&de->chain)) {
	    if (de->dev == dev) {
		object = de->object;
		break;
	    }
	}

	mutex_unlock(&dev_number_hash_lock);
	return (object);
}

/*
 * Map kernel device error codes to BSD error numbers.
 */
int
dev_error_to_errno(err)
	int	err;
{
	switch (err) {
	    case D_SUCCESS:
		return (0);

	    case D_IO_ERROR:
		return (EIO);

	    case D_WOULD_BLOCK:
		return (EWOULDBLOCK);

	    case D_NO_SUCH_DEVICE:
		return (ENXIO);

	    case D_ALREADY_OPEN:
		return (EBUSY);

	    case D_DEVICE_DOWN:
		return (ENETDOWN);

	    case D_INVALID_OPERATION:
		return (ENOTTY);    /* weird, but ioctl() callers expect it */

	    case D_INVALID_RECNUM:
	    case D_INVALID_SIZE:
		return (EINVAL);

	    default:
		return (EIO);
	}
	/*NOTREACHED*/
}
