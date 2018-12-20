/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/default_iobuf_mgr.cc,v $
 *
 * Purpose: Manager for a set of I/O buffer objects
 *		-- default version (private memory)
 *
 * This is a naive implementation, using malloc() and free() in private
 * memory for all blocks. More clever implementations could use a shared
 * memory pool for the exchange of blocks.
 *
 * HISTORY:
 * $Log:	default_iobuf_mgr.cc,v $
 * Revision 2.3  94/07/07  17:23:05  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:45:42  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:27:36  pjg]
 * 
 * Revision 2.2  91/05/05  19:25:53  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:52:58  dpj]
 * 
 * 	Removed support for user-defined blocks.
 * 	Removed explicit user info in records (now implicit).
 * 	[91/04/28  10:00:09  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:25:51  dpj]
 * 
 */

#ifndef lint
char * default_iobuf_mgr_rcsid = "$Header: default_iobuf_mgr.cc,v 2.3 94/07/07 17:23:05 mrt Exp $";
#endif	lint

#include <default_iobuf_mgr_ifc.h>



#define BASE usTop
DEFINE_LOCAL_CLASS(default_iobuf_mgr)


default_iobuf_mgr::default_iobuf_mgr() : infosize(0)
{}


mach_error_t 
default_iobuf_mgr::io_set_rec_infosize(unsigned int infosize)
{
	Local(infosize) = infosize;
	return(ERR_SUCCESS);
}


mach_error_t 
default_iobuf_mgr::io_alloc_block(unsigned int blksize, io_block_t *newblk)
{
	io_block_t		blk;

	blk = (io_block_t) malloc(sizeof(struct io_block) + blksize);
	if (blk == NULL) {
		*newblk = NULL;
		return(US_RESOURCE_EXHAUSTED);
	}

	blk->owner = (int) this;	/* XXX need reference ? */
	blk->next = NULL;
	blk->type = IOB_RAWDATA;
	blk->min_offset = sizeof(struct io_block);
	blk->max_offset = blk->min_offset + blksize;
	ioblk_clear(blk);
	*newblk = blk;

	return(ERR_SUCCESS);
}


mach_error_t default_iobuf_mgr::io_free_block(io_block_t blk)
{
	if (blk->owner == 0) {
		return(US_OBJECT_DEAD);
	}
	if (blk->owner != (int) this) {
		return(((default_iobuf_mgr*)(blk->owner))->io_free_block(blk));
	}

	/*
	 * XXX Should we check for non-null blk->next ?
	 */

	blk->owner = 0;
	free(blk);

	return(ERR_SUCCESS);
}


mach_error_t 
default_iobuf_mgr::io_alloc_record(unsigned int blksize, io_record_t *newrec)
{
	io_record_t		rec;
	mach_error_t		ret;

	rec = (io_record_t) malloc(sizeof(struct io_record) +
				Local(infosize) - sizeof(struct io_recinfo));
	if (rec == NULL) {
		*newrec = NULL;
		return(US_RESOURCE_EXHAUSTED);
	}

	rec->owner = (int) this;	/* XXX need reference ? */
	rec->next = NULL;
	rec->infosize = Local(infosize);
	io_recinfo_init_default(&rec->recinfo);

	if (blksize > 0) {
		/*
		 * XXX Should use a single malloc() to get both the record
		 * header and the block, with reference count to allow
		 * separate deallocation.
		 */
		ret = io_alloc_block(blksize,&(rec->first_block));
		if (ret != ERR_SUCCESS) {
			free(rec);
			return(ret);
		}
	} else {
		rec->first_block = 0;
	}

	rec->last_block = rec->first_block;
	*newrec = rec;

	return(ERR_SUCCESS);
}


mach_error_t default_iobuf_mgr::io_free_record(io_record_t rec)
{
	io_block_t		curblk, nxtblk;
	mach_error_t		ret;

	if (rec->owner == 0) {
		return(US_OBJECT_DEAD);
	}
	if (rec->owner != (int) this) {
		return(((default_iobuf_mgr*)(rec->owner))->io_free_record(rec));
	}

	curblk = rec->first_block;
	while (curblk != NULL) {
		nxtblk = curblk->next;
		curblk->next = NULL;
		ret = io_free_block(curblk);
		if (ret != ERR_SUCCESS) {
			ERROR((0,
				"io_free_record.io_free_block failed: %s\n",
				mach_error_string(ret)));
		}
		curblk = nxtblk;
	}

	io_recinfo_destroy(&rec->recinfo);

	rec->owner = 0;
	free(rec);

	return(ERR_SUCCESS);
}


mach_error_t default_iobuf_mgr::io_pullup_whole_record(io_record_t rec)
{
	mach_error_t		ret;
	unsigned int		len;
	io_block_t		curblk;
	io_block_t		nextblk;
	io_block_t		newblk;
	unsigned int		xlen;

	/*
	 * Fast exit if nothing to do.
	 */
	if (rec->last_block == rec->first_block) {
		return(ERR_SUCCESS);
	}

	/*
	 * Figure-out the total length.
	 */
	len = 0;
	curblk = rec->first_block;
	while (curblk != NULL) {
		len += ioblk_cursize(curblk);
		curblk = curblk->next;
	}

	/*
	 * Decide if we can pull everything into the existing first
	 * block, or if we need a new bigger block.
	 *
	 * XXX Should take advantage of a big block further down the list.
	 */
	if (len > ioblk_maxsize(rec->first_block)) {
		ret = this->io_alloc_block(len,&newblk);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
		curblk = rec->first_block;
	} else {
		newblk = rec->first_block;
		curblk = newblk->next;
	}

	/*
	 * Copy the data.
	 */
	while (curblk != NULL) {
		nextblk = curblk->next;
		curblk->next = NULL;
		xlen = ioblk_cursize(curblk);
		if (xlen != 0) {
			bcopy(ioblk_start(curblk),ioblk_end(newblk),xlen);
			newblk->end_offset += xlen;
		}
		ret = io_free_block(curblk);
		if (ret != ERR_SUCCESS) {
			ERROR((0,
			"io_pullup_whole_record.io_free_block failed: %s\n",
				mach_error_string(ret)));
		}
		curblk = nextblk;
	}

	/*
	 * Update the record head.
	 */
	rec->first_block = newblk;
	rec->last_block = newblk;

	return(ERR_SUCCESS);
}


#ifdef	USERBLOCKS

mach_error_t 
default_iobuf_mgr::io_copy_blocks(io_block_t fromblk, io_block_t toblk)
{
	char			*fromptr;
	unsigned int		fromlen;
	unsigned int		xferlen;

	while (fromblk != NULL) {
		fromptr = ioblk_start(fromblk);
		fromlen = ioblk_cursize(fromblk);
		while (fromlen > 0) {
			if (toblk == NULL) {
				return(US_INVALID_BUFFER_SIZE);
			}
			xferlen = MIN(ioblk_cursize(toblk),fromlen);
			if (xferlen == 0) {
				toblk = toblk->next;
				if (toblk == NULL) {
					return(US_INVALID_BUFFER_SIZE);
				}
				xferlen = MIN(ioblk_cursize(toblk),fromlen);
			}
			bcopy(fromptr,ioblk_start(toblk),xferlen);
			toblk->end_offset += xferlen;
			fromlen -= xferlen;
		}
		fromblk = fromblk->next;
	}
}


mach_error_t 
default_iobuf_mgr::io_copy_records(io_record_t fromrec, io_record_t torec)
{
	mach_error_t		ret;

	while (fromrec != NULL) {
		if (torec == NULL) {
			return(US_INVALID_BUFFER_SIZE);
		}
		ret = io_copy_blocks(fromrec->first_block,
				     torec->first_block);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
		fromrec = fromrec->next;
		torec = torec->next;
	}
}

#endif	USERBLOCKS
