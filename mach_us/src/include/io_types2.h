/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/io_types2.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Standard types for I/O interface -- extended interface.
 *
 */
/*
 * HISTORY:
 * $Log:	io_types2.h,v $
 * Revision 2.3  94/07/08  15:51:20  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/05/05  19:23:35  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:43:27  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  09:22:26  dpj]
 * 
 */

#ifndef	_IO_TYPES2_H_
#define	_IO_TYPES2_H_

#include	<mach_error.h>
#include	<cthreads.h>

#include	<base.h>
#include	<io_types.h>


/************************************************************************\
 *									*
 *		Abstract data types for data manipulation		*
 *									*
\************************************************************************/

/*
 * io_block_t: abstract data type for data blocks.
 *
 * XXX This data type is currently not very abstract... In particular,
 * the user is allowed to see that it is a pointer and not an inline
 * structure, and he/she is also allowed to use the "next" field
 * explicitly.
 */
typedef	unsigned int		io_blktype_t;
#define	IOB_RAWDATA		1
#define	IOB_RAWDATA_READONLY	1

typedef struct io_block {
	int			owner;
	struct io_block		*next;
	io_blktype_t		type;
	unsigned int		min_offset;
	unsigned int		max_offset;
	unsigned int		start_offset;
	unsigned int		end_offset;
} *io_block_t;

#define	ioblk_start(_blk)						\
	((char *)(((char *)(_blk)) + ((io_block_t)(_blk))->start_offset))

#define	ioblk_end(_blk)							\
	((char *)(((char *)(_blk)) + ((io_block_t)(_blk))->end_offset))

#define	ioblk_cursize(_blk)						\
	((int)(((io_block_t)(_blk))->end_offset				\
		- ((io_block_t)(_blk))->start_offset))

#define	ioblk_maxsize(_blk)						\
	((int)(((io_block_t)(_blk))->max_offset				\
		- ((io_block_t)(_blk))->min_offset))

#define	ioblk_freesize(_blk)						\
	((int)(((io_block_t)(_blk))->max_offset				\
		- ((io_block_t)(_blk))->end_offset))

#define	ioblk_empty(_blk)						\
	(((io_block_t)(_blk))->start_offset == 				\
			((io_block_t)(_blk))->end_offset)

#define	ioblk_clear(_blk)						\
MACRO_BEGIN								\
	((io_block_t)(_blk))->start_offset =				\
			((io_block_t)(_blk))->min_offset; 		\
	((io_block_t)(_blk))->end_offset =				\
			((io_block_t)(_blk))->start_offset;  		\
MACRO_END


/*
 * io_recinfo_t: abstract data type for user-specified info associated
 *		 with each data record.
 *
 * The structure defined here is intended to be used as a base, to be
 * extended for specific user applications. The actual size of a
 * "derived" recinfo structure is specified in the record allocation
 * calls.
 *
 * io_recinfo_ops_t is used to hold pointers to a few "standard" operations
 * that may be called on io_recinfo_t instances by the I/O system.
 *
 * Note: if this does not look like a C++ class with virtual functions,
 * I don't know what does...
 */
typedef struct io_recinfo_ops {
	void			(*destroyproc)();
	void			(*copyproc)();
	int			(*sizeproc)();
} io_recinfo_ops_t;

typedef struct io_recinfo {
	io_recinfo_ops_t	*ops;
} io_recinfo_t;

#define	io_recinfo_init_default(_ri)					\
MACRO_BEGIN								\
	((io_recinfo_t *)(_ri))->ops = NULL;				\
MACRO_END

#define	io_recinfo_init(_ri,_ops)					\
MACRO_BEGIN								\
	((io_recinfo_t *)(_ri))->ops = (_ops);				\
MACRO_END

#define	io_recinfo_destroy(_ri)						\
MACRO_BEGIN								\
	if ((_ri) && (((io_recinfo_t *)(_ri))->ops) &&			\
			(((io_recinfo_t *)(_ri))->ops->destroyproc))	\
		(*(((io_recinfo_t *)(_ri))->ops->destroyproc))(_ri);	\
MACRO_END

#define	io_recinfo_copy(_ri,_dest)					\
MACRO_BEGIN								\
	if ((((io_recinfo_t *)(_ri))->ops) &&				\
			(((io_recinfo_t *)(_ri))->ops->copyproc))	\
		(*(((io_recinfo_t *)(_ri))->ops->copyproc))((_ri),(_dest));\
MACRO_END

#define	io_recinfo_size(_ri)						\
	(((((io_recinfo_t *)(_ri))->ops) &&				\
			(((io_recinfo_t *)(_ri))->ops->destroyproc)) ?	\
		(*(((io_recinfo_t *)(_ri))->ops->sizeproc))(_ri)	\
		: sizeof(io_recinfo_t))


/*
 * io_record_t: abtract data type for data records.
 *
 * Each record is composed of a sequence of blocks.
 *
 * XXX There is currently nothing to prevent a user from
 * modifying blocks after they have been inserted inside a record.
 */
typedef struct io_record {
	int			owner;
	struct io_record	*next;
	struct io_block		*first_block;
	struct io_block		*last_block;
	unsigned int		infosize;
	struct io_recinfo	recinfo;
		/* more recinfo data may follow here */
} *io_record_t;

#define	iorec_contiguous_p(_rec)					\
	(((io_record_t)(_rec))->first_block				\
		&& (((io_record_t)(_rec))->first_block->next == NULL))

#define	iorec_append_blk(_rec,_blk)					\
MACRO_BEGIN								\
	if (((io_record_t)(_rec))->first_block == NULL) {		\
		((io_record_t)(_rec))->first_block = ((io_block_t)(_blk));\
	} else {							\
		((io_record_t)(_rec))->last_block->next =		\
					((io_block_t)(_blk));		\
	}								\
	((io_record_t)(_rec))->last_block = ((io_block_t)(_blk));	\
MACRO_END

extern unsigned int		iorec_cursize();
extern mach_error_t		iorec_extract();
extern mach_error_t		iorec_read();


#endif	_IO_TYPES2_H_
