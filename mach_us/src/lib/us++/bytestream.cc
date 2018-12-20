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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/bytestream.cc,v $
 *
 * Purpose: General-purpose queue for undifferentiated bytes (byte-stream).
 *
 * HISTORY:
 * $Log:	bytestream.cc,v $
 * Revision 2.7  94/10/27  12:01:44  jms
 * 	25-Oct-94  J. Mark Stevenson (jms)
 * 	Fixed queue size check.  This fixes the problem of
 * 	having a pipe gobble up all of the paging space with
 * 	zero filled pages.
 * 	[94/10/26  14:46:45  jms]
 * 
 * Revision 2.6  94/07/07  17:22:50  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  94/01/11  17:49:56  jms
 * 	Misc bug fixes in empty block manipulation.
 * 	[94/01/09  19:37:49  jms]
 * 
 * Revision 2.4  92/07/05  23:26:44  dpj
 * 	Define as an abstract class instead of a concrete class.
 * 	[92/06/24  16:05:28  dpj]
 * 
 * 	Fixed ambiguity with local member "lock".
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:51:00  dpj]
 * 
 * Revision 2.3  91/11/13  17:17:47  dpj
 * 	condition_wait() -> intr_cond_wait()
 * 	[91/11/13  14:49:41  dpj]
 * 
 * Revision 2.2  91/11/06  13:45:25  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:26:07  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:22:21  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:33:00  pjg]
 * 
 * Revision 2.5  91/07/01  14:11:37  jms
 * 	Code from dan to make bytestreams interruptable
 * 	[91/06/24  17:21:03  jms]
 * 
 * Revision 2.4  91/05/05  19:25:44  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:52:50  dpj]
 * 
 * 	Reorganized for split between basic interface (io_methods) and
 * 	extended interface (io_methods2).
 * 	[91/04/28  09:58:27  dpj]
 * 
 * 	Fixed include files.
 * 	[91/02/25  10:25:29  dpj]
 * 
 * Revision 2.3  89/11/28  19:10:57  dpj
 * 	Modified to support writing with arbitrary sizes,
 * 	with high-water-mark for flow control and
 * 	variable-length list of blocks.
 * 	[89/11/22            dpj]
 * 
 * 	Added I/O strategies (IOS_ENABLED, IOS_WAIT_ALLOWED) in
 * 	io_read() and io_write().
 * 	Implemented io_set_{read,write}_strategy().
 * 	[89/11/20  20:40:26  dpj]
 * 
 * Revision 2.2  89/10/30  16:31:26  dpj
 * 	First version.
 * 	[89/10/27  17:25:46  dpj]
 * 
 *
 */

#ifndef lint
char * bytestream_rcsid = "$Header: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/bytestream.cc,v 2.7 94/10/27 12:01:44 jms Exp $";
#endif	lint

#include <bytestream_ifc.h>

extern "C" {
#include	<interrupt.h>
}

/*
 * Remove a list of blocks from the read queue.
 *
 * Note: if the last block is at the write pointer,
 * the free space at its tail is counted in the free
 * count -- take care of it.
 */
#define	DEQ_RBLKS(startblk,endblk,len)					\
MACRO_BEGIN								\
	if ((startblk) != Local(readblk)) {				\
		us_internal_error("DEQ_RBLKS: startblk != readblk",	\
					US_INTERNAL_ERROR);		\
	}								\
									\
	if ((endblk) == Local(writeblk)) {				\
		Local(writeblk) = (endblk)->next;			\
		Local(qfree) -= ioblk_freesize(endblk);			\
	}								\
	Local(readblk) = (endblk)->next;				\
	(endblk)->next = NULL;						\
	Local(qused) -= len;						\
MACRO_END

/*
 * Append a list of blocks containing data at the end of the read queue.
 *
 * Note: the end of the read queue is marked by the write pointer.
 */
#define	ENQ_RBLKS(startblk,endblk,len)					\
MACRO_BEGIN								\
	if (Local(writeblk) == NULL) {					\
		if (Local(readblk) != NULL) {				\
			us_internal_error(				\
			"ENQ_RBLKS: writeblk == NULL, readblk != NULL",	\
					US_INTERNAL_ERROR);		\
		}							\
		Local(writeblk) = (endblk);				\
		(endblk)->next = NULL;					\
		Local(readblk) = (startblk);				\
	} else {							\
		Local(qfree) -= ioblk_freesize(Local(writeblk));	\
		(endblk)->next = Local(writeblk)->next;			\
		Local(writeblk)->next = (startblk);			\
		Local(writeblk) = (endblk);				\
	}								\
	Local(qfree) += ioblk_freesize(endblk);				\
	Local(qused) += (len);						\
MACRO_END

/*
 * Add a list of free blocks after the write pointer.
 */
#define	ENQ_WBLKS(startblk,endblk,len)					\
MACRO_BEGIN								\
	if (Local(writeblk) == NULL) {					\
		if (Local(readblk) != NULL) {				\
			us_internal_error(				\
			"ENQ_WBLKS: writeblk == NULL, readblk != NULL",	\
					US_INTERNAL_ERROR);		\
		}							\
		Local(writeblk) = (startblk);				\
		(endblk)->next = NULL;					\
		Local(readblk) = Local(writeblk);			\
	} else {							\
		(endblk)->next = Local(writeblk)->next;			\
		Local(writeblk)->next = (startblk);			\
	}								\
	Local(qfree) += len;						\
MACRO_END

/*
 * Copy bytes into a list of blocks at the write pointer.
 */
#define	COPYTOBLKS(buf,len)						\
MACRO_BEGIN								\
	char			*_addr = (buf);				\
	unsigned int		_resid = (len);				\
	io_block_t		_curblk;				\
	unsigned int		_xfer;					\
	unsigned int		_blkfree;				\
									\
	_curblk = Local(writeblk);					\
	while (_resid > 0) {						\
		while ((_blkfree = ioblk_freesize(_curblk)) == 0) {	\
			_curblk = _curblk->next;			\
		}							\
		_xfer = MIN(_resid,_blkfree);				\
		bcopy(_addr,ioblk_end(_curblk),_xfer);			\
		_curblk->end_offset += _xfer;				\
		_addr += _xfer;						\
		_resid -= _xfer;					\
	}								\
	Local(writeblk) = _curblk;					\
	Local(qfree) -= (len);						\
	Local(qused) += (len);						\
MACRO_END


DEFINE_ABSTRACT_CLASS_MI(bytestream)
DEFINE_CASTDOWN2(bytestream, stream_base, usByteIO)

void bytestream::init_class(usClass* class_obj)
{
	usByteIO::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(bytestream);
	SETUP_METHOD_WITH_ARGS(bytestream,io_read_seq);
	SETUP_METHOD_WITH_ARGS(bytestream,io_write_seq);
	SETUP_METHOD_WITH_ARGS(bytestream,io_get_size);
	END_SETUP_METHOD_WITH_ARGS;
}

bytestream::bytestream(default_iobuf_mgr* mgr, int qsize, 
		       io_strategy_t readstrat, io_strategy_t writestrat)
	: stream_base(mgr, readstrat, writestrat),
	  qused(0), qmax(qsize), qfree(0), q_hwm(2 * qsize),
	  readblk(0), writeblk(0),
	  blksize(qsize), maximum_xfer(32768)
{
	mutex_init(&Local(lock));
	condition_init(&put_cond);
	condition_init(&get_cond);
	INT_TO_IO_OFF(0,&read_offset);
	INT_TO_IO_OFF(0,&write_offset);
}

bytestream::~bytestream()
{
	io_block_t		curblk;
	io_block_t		newblk;

	DESTRUCTOR_GUARD();

	curblk = readblk;
	while (curblk != NULL) {
		newblk = curblk->next;
		(void) io_free_block(curblk);
		curblk = newblk;
	}
	condition_clear(&put_cond);
	condition_clear(&get_cond);
}


mach_error_t bytestream::get_more_blocks_internal(unsigned int 	size)
{
	mach_error_t		ret;
	int			numblks;
	io_block_t		newblk;

	/* MUST BE CALLED WITH THE OBJECT LOCKED */

	/*
	 * By default, the standard block size is the same as the
	 * maximum I/O size, so this should only ever allocate
	 * a single block under normal use. However, there may be
	 * advantages to using a smaller standard block size, so
	 * let's be prepared for it...
	 */

	numblks = (size + Local(blksize) - 1) / Local(blksize);
	for (; numblks > 0; numblks--) {
		ret = io_alloc_block(Local(blksize),&newblk);
		if (ret != ERR_SUCCESS) {
			us_internal_error("io_alloc_block()",ret);
			return(US_INTERNAL_ERROR);
		}
		ENQ_WBLKS(newblk,newblk,Local(blksize));
	}

	return(ERR_SUCCESS);
}



mach_error_t 
bytestream::io_read_seq(io_mode_t mode, char *addr, unsigned int *num, 
			io_offset_t *offset)
{
	unsigned int		minsize;
	io_block_t		curblk;
	unsigned int		resid;
	unsigned int		xfer;
	boolean_t		wakeup_readers;
	mach_error_t		ret;

	if (mode & ~(IOM_WAIT | IOM_PROBE | IOM_TRUNCATE)) {
		return(IO_INVALID_MODE);
	}

	mutex_lock(&Local(lock));

	if (! (Local(read_strategy) & IOS_ENABLED)) {
		mutex_unlock(&Local(lock));
		return(IO_REJECTED);
	}

	if (*num > Local(maximum_xfer)) {
		mutex_unlock(&Local(lock));
		return(IO_INVALID_SIZE);
	}

	if (mode & IOM_TRUNCATE) {
		minsize = 1;
	} else {
		minsize = *num;
	}

	/*
	 * Wait until there is a enough data on the queue.
	 */
	while (Local(qused) < minsize) {
		if (! (Local(read_strategy) & IOS_WAIT_ALLOWED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
		if (mode & IOM_WAIT) {
			ret = intr_cond_wait(&Local(get_cond),&Local(lock));
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&Local(lock));
				return(EXCEPT_SOFTWARE);
			}
		} else {
			mutex_unlock(&Local(lock));
			return(US_OBJECT_BUSY);
		}
		if (! (Local(read_strategy) & IOS_ENABLED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
	}
	*num = MIN(*num,Local(qused));
	*offset = Local(read_offset);

	if (mode & IOM_PROBE) {
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	/*
	 * Copy the data from the list of blocks.
	 */
	resid = *num;
	while (resid > 0) {
		curblk = Local(readblk);
		if (curblk == NULL) {
			mutex_unlock(&Local(lock));
			us_internal_error("io_read_seq() ran out of blocks",
							US_INTERNAL_ERROR);
			return(US_INTERNAL_ERROR);
		}
		xfer = MIN(resid,ioblk_cursize(curblk));
		if (xfer != 0) {
			bcopy(ioblk_start(curblk),addr,xfer);
			addr += xfer;
			resid -= xfer;
			curblk->start_offset += xfer;
		}
		if (ioblk_empty(curblk)) {
			DEQ_RBLKS(curblk,curblk,xfer);
			ioblk_clear(curblk);
			if (
				((Local(qfree) + ioblk_freesize(curblk)) >
							Local(q_hwm)) ||
				((ioblk_maxsize(curblk) <
							Local(blksize)))) {
				(void) io_free_block(curblk);
			} else {
				ENQ_WBLKS(curblk,curblk,
						ioblk_freesize(curblk));
			}
		} else {
			Local(qused) -= xfer;
		}
	}

	ADD_LONG_TO_DLONG(&Local(read_offset),*num);

	/*
	 * Decide if we should enable other readers.
	 */
	if (Local(qused) > 0) {
		wakeup_readers = TRUE;
	} else {
		wakeup_readers = FALSE;
	}

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up possible writers.
	 */
	condition_signal(&Local(put_cond));

	/*
	 * Wake-up possible readers.
	 */
	if (wakeup_readers) {
		condition_signal(&Local(get_cond));
	}

	return(ERR_SUCCESS);
}

mach_error_t 
bytestream::io_write_seq(io_mode_t mode, char *addr, unsigned int *num,
			 io_offset_t *offset)
{
	mach_error_t		ret;
	unsigned int		mincount;
	boolean_t		wakeup_writers;

	if (mode & ~(IOM_WAIT | IOM_PROBE | IOM_TRUNCATE)) {
		return(IO_INVALID_MODE);
	}

	mutex_lock(&Local(lock));

	if (! (Local(write_strategy) & IOS_ENABLED)) {
		mutex_unlock(&Local(lock));
		return(IO_REJECTED);
	}

	if (*num > Local(maximum_xfer)) {
		mutex_unlock(&Local(lock));
		return(IO_INVALID_SIZE);
	}

	if (mode & IOM_TRUNCATE) {
		mincount = 1;
	} else {
		mincount = *num;
	}

	/*
	 * Wait until there is enough space on the queue.
	 */
	while ((Local(qmax) - Local(qused)) < mincount) {
		if (! (Local(write_strategy) & IOS_WAIT_ALLOWED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
		if (mode & IOM_WAIT) {
			ret = intr_cond_wait(&Local(put_cond),&Local(lock));
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&Local(lock));
				return(EXCEPT_SOFTWARE);
			}
		} else {
			mutex_unlock(&Local(lock));
			return(US_OBJECT_BUSY);
		}
		if (! (Local(write_strategy) & IOS_ENABLED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
	}
	*num = MIN(*num,Local(qmax) - Local(qused));
	*offset = Local(write_offset);

	if (mode & IOM_PROBE) {
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	/*
	 * Allocate enough blocks to hold all the data.
	 */
	if (((signed)(((signed)(*num)) - (signed)Local(qfree))) > 0) {
		ret = get_more_blocks_internal(*num - Local(qfree));
		if (ret != ERR_SUCCESS) {
			*num = 0;
			mutex_unlock(&Local(lock));
			return(ret);
		}
	}

	/*
	 * Copy the data into the free blocks.
	 */
	COPYTOBLKS(addr,*num);

	ADD_LONG_TO_DLONG(&Local(write_offset),*num);

	/*
	 * Decide if we should enable other writers.
	 */
	if (Local(qused) < Local(qmax)) {
		wakeup_writers = TRUE;
	} else {
		wakeup_writers = FALSE;
	}

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up possible readers.
	 */
	condition_signal(&Local(get_cond));

	/*
	 * Wake-up possible writers.
	 */
	if (wakeup_writers) {
		condition_signal(&Local(put_cond));
	}

	return(ERR_SUCCESS);
}


mach_error_t 
bytestream::io_getbytes_seq(io_mode_t mode, io_block_t *blk,
			    io_count_t *count, io_offset_t *offset)
{
	mach_error_t		ret = ERR_SUCCESS;
	unsigned int		minsize;
	io_block_t		firstblk;
	io_block_t		endblk;
	io_block_t		lastblk;
	io_block_t		prevblk;
	unsigned int		resid;
	unsigned int		blksize;
	boolean_t		wakeup_readers;

	if (mode & ~(IOM_WAIT | IOM_PROBE | IOM_TRUNCATE)) {
		return(IO_INVALID_MODE);
	}

	mutex_lock(&Local(lock));

	if (! (Local(read_strategy) & IOS_ENABLED)) {
		mutex_unlock(&Local(lock));
		return(IO_REJECTED);
	}

	if (mode & IOM_TRUNCATE) {
		minsize = 1;
	} else {
		minsize = *count;
	}

	/*
	 * Wait until there is a enough data on the queue.
	 */
	while (Local(qused) < minsize) {
		if (! (Local(read_strategy) & IOS_WAIT_ALLOWED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
		if (mode & IOM_WAIT) {
			ret = intr_cond_wait(&Local(get_cond),&Local(lock));
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&Local(lock));
				return(EXCEPT_SOFTWARE);
			}
		} else {
			mutex_unlock(&Local(lock));
			return(US_OBJECT_BUSY);
		}
		if (! (Local(read_strategy) & IOS_ENABLED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
	}
	*count = MIN(*count,Local(qused));
	*offset = Local(read_offset);

	if (mode & IOM_PROBE) {
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	/*
	 * Dequeue as many "complete" blocks as possible.
	 */
	resid = *count;
	lastblk = Local(readblk);
	prevblk = NULL;
	while ((resid > 0) && (lastblk != NULL) &&
			((blksize = ioblk_cursize(lastblk)) <= resid)) {
		/*
		 * No need to check for empty blocks -- there should
		 * be none.
		 */
		resid -= blksize;
		prevblk = lastblk;
		lastblk = lastblk->next;
	}

	/*
	 * At this point, lastblk points to the first block after
	 * the last "complete" block. prevblk points to the last
	 * "complete" block (unless it is null).
	 */

	if (resid > 0) {
		if (lastblk == NULL) {
			mutex_unlock(&Local(lock));
			us_internal_error(
				"io_getbytes_seq() ran out of blocks",
				US_INTERNAL_ERROR);
			return(US_INTERNAL_ERROR);
		}

		if (prevblk != NULL) {
			DEQ_RBLKS(firstblk,prevblk,*count - resid);

			firstblk = Local(readblk);
			if (ioblk_freesize(prevblk) <= resid) {
				ret = io_alloc_block(MAX(resid,
						Local(blksize)),&endblk);
				if (ret != ERR_SUCCESS) {
					*count -= resid;
					goto finish;
				}
				prevblk->next = endblk;
			} else {
				endblk = prevblk;
			}
		} else {
			ret = io_alloc_block(MAX(resid, Local(blksize)),
					     &endblk);
			if (ret != ERR_SUCCESS) {
				firstblk = NULL;
				*count -= resid;
				goto finish;
			}
			firstblk = endblk;
		}

		bcopy(ioblk_start(lastblk),ioblk_start(endblk),resid);
		endblk->end_offset += resid;
		lastblk->start_offset += resid;
		Local(qused) -= resid;
	} else {
		if (prevblk != NULL) {
			DEQ_RBLKS(firstblk,prevblk,*count - resid);
		}
	}

finish:
	*blk = firstblk;
	ADD_LONG_TO_DLONG(&Local(read_offset),*count);

	/*
	 * Decide if we should enable other readers.
	 */
	if (Local(qused) > 0) {
		wakeup_readers = TRUE;
	} else {
		wakeup_readers = FALSE;
	}

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up possible writers.
	 */
	condition_signal(&Local(put_cond));

	/*
	 * Wake-up possible readers.
	 */
	if (wakeup_readers) {
		condition_signal(&Local(get_cond));
	}

	return(ret);
}


mach_error_t 
bytestream::io_putbytes_seq(io_mode_t mode, io_block_t *blk,
			    io_count_t *count, io_offset_t *offset)
{
	mach_error_t		ret = ERR_SUCCESS;
	unsigned int		mincount;
	io_block_t		prevblk;
	io_block_t		lastblk;
	unsigned int		resid;
	unsigned int		blksize;
	boolean_t		wakeup_writers;

	if (mode & ~(IOM_WAIT | IOM_PROBE | IOM_TRUNCATE)) {
		return(IO_INVALID_MODE);
	}

	if (! (Local(write_strategy) & IOS_ENABLED)) {
		mutex_unlock(&Local(lock));
		return(IO_REJECTED);
	}

	mutex_lock(&Local(lock));

	if (mode & IOM_TRUNCATE) {
		mincount = 1;
	} else {
		mincount = *count;
	}

	/*
	 * Wait until there is enough space on the queue.
	 */
	while ((Local(qmax) - Local(qused)) < mincount) {
		if (! (Local(write_strategy) & IOS_WAIT_ALLOWED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
		if (mode & IOM_WAIT) {
			ret = intr_cond_wait(&Local(put_cond),&Local(lock));
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&Local(lock));
				return(EXCEPT_SOFTWARE);
			}
		} else {
			mutex_unlock(&Local(lock));
			return(US_OBJECT_BUSY);
		}
		if (! (Local(write_strategy) & IOS_ENABLED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
	}
	*count = MIN(*count,Local(qmax) - Local(qused));
	*offset = Local(write_offset);

	if (mode & IOM_PROBE) {
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	/*
	 * Scan through as many complete input blocks as possible,
	 * and eliminate empty blocks.
	 */
	resid = *count;
	lastblk = *blk;
	prevblk = NULL;
	while ((resid > 0) && (lastblk != NULL) &&
			((blksize = ioblk_cursize(lastblk)) <= resid)) {
		if (blksize == 0) {
			io_block_t	oblk = lastblk;
			lastblk = lastblk->next;
			(void) io_free_block(oblk);
		} else {
			resid -= blksize;
			prevblk = lastblk;
			lastblk = lastblk->next;
		}
	}

	/*
	 * At this point, lastblk is the last block from which to transfer
	 * data (possibly none), and prevblk is the last block before lastblk
	 * (or NULL if lastblk is the first block).
	 */

	if (resid > 0) {
		if (lastblk == NULL) {
			mutex_unlock(&Local(lock));
			return(IO_NOT_ENOUGH_DATA);
		}

		/*
		 * Put all the "complete" blocks on the queue.
		 */
		if (prevblk != NULL) {
			ENQ_RBLKS(*blk,prevblk,*count - resid);
		}

		/*
		 * Take care of the last "incomplete" block, designated by
		 * lastblk.
		 *
		 * We do this by copying the excess data at the tail of
		 * the previous block and allocating new blocks as needed.
		 */
		if (resid > Local(qfree)) {
			ret = get_more_blocks_internal(resid - Local(qfree));
			if (ret != ERR_SUCCESS) {
				*count -= resid;
				goto finish;
			}
		}
		COPYTOBLKS(ioblk_start(lastblk),resid);
		lastblk->start_offset += resid;
	} else {
		if (prevblk != NULL) {
			ENQ_RBLKS(*blk,prevblk,*count);
		}
	}

finish:
	*blk = lastblk;
	ADD_LONG_TO_DLONG(&Local(write_offset),*count);

	/*
	 * Decide if we should enable other writers.
	 */
	if (Local(qused) < Local(qmax)) {
		wakeup_writers = TRUE;
	} else {
		wakeup_writers = FALSE;
	}

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up possible readers.
	 */
	condition_signal(&Local(get_cond));

	/*
	 * Wake-up possible writers.
	 */
	if (wakeup_writers) {
		condition_signal(&Local(put_cond));
	}

	return(ret);
}


mach_error_t bytestream::io_get_size(io_size_t *size)
{
	mutex_lock(&Local(lock));
	UINT_TO_IO_SIZE(Local(qused),size);
	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


mach_error_t bytestream::io_set_read_strategy(io_strategy_t strategy)
{
	if (strategy & ~(IOS_ENABLED | IOS_WAIT_ALLOWED)) {
		return(IO_INVALID_STRATEGY);
	}

	mutex_lock(&Local(lock));
	Local(read_strategy) = strategy;
	mutex_unlock(&Local(lock));

	condition_broadcast(&Local(get_cond));

	return(ERR_SUCCESS);
}


mach_error_t bytestream::io_set_write_strategy(io_strategy_t strategy)
{
	if (strategy & ~(IOS_ENABLED | IOS_WAIT_ALLOWED)) {
		return(IO_INVALID_STRATEGY);
	}

	mutex_lock(&Local(lock));
	Local(write_strategy) = strategy;
	mutex_unlock(&Local(lock));

	condition_broadcast(&Local(put_cond));

	return(ERR_SUCCESS);
}


mach_error_t bytestream::io_flush_stream(void)
{
	io_block_t		curblk;
	io_block_t		newblk;

	mutex_lock(&Local(lock));

	curblk = Local(readblk);
	while (curblk != NULL) {
		newblk = curblk->next;
		(void) io_free_block(curblk);
		curblk = newblk;
	}

	Local(qused) = 0;
	Local(qfree) = 0;
	Local(readblk) = NULL;
	Local(writeblk) = NULL;
	Local(read_offset) = Local(write_offset);

	condition_broadcast(&Local(put_cond));
	condition_broadcast(&Local(get_cond));

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}

mach_error_t bytestream::io_read(io_mode_t, io_offset_t, pointer_t,
				 unsigned int*)
{
	return _notdef();
}
mach_error_t bytestream::io_write(io_mode_t, io_offset_t,pointer_t,
				  unsigned int*)
{
	return _notdef();
}
mach_error_t 
bytestream::io_append(io_mode_t mode, pointer_t addr, unsigned int* num)
{
	return _notdef();
}

mach_error_t 
bytestream::io_set_size(io_size_t size)
{
	return _notdef();
}

mach_error_t 
bytestream::io_map(task_t task, vm_address_t *addr, vm_size_t size,
		   vm_offset_t mask, boolean_t anywhere, 
		   vm_offset_t paging_offset, boolean_t copy, 
		   vm_prot_t cprot, vm_prot_t mprot, vm_inherit_t inherit)
{
	return _notdef();
}

