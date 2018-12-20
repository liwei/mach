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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/io_types.cc,v $
 *
 * Purpose: Support functions for standard I/O data types.
 *
 * HISTORY
 * $Log:	io_types.cc,v $
 * Revision 2.4  94/07/07  17:23:26  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:27:39  dpj
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:10:56  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:32  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:46:40  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:33  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:53:54  dpj]
 * 
 * 	Reorganized for split between basic interface (io_methods) and
 * 	extended interface (io_methods2).
 * 	Added iorec_read().
 * 	[91/04/28  10:10:22  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:27:16  dpj]
 * 
 *
 */

#ifndef lint
char * io_types_rcsid = "$Header: io_types.cc,v 2.4 94/07/07 17:23:26 mrt Exp $";
#endif	lint


extern "C" {
#include	<base.h>
#include	<io_types.h>
#include	<io_types2.h>

unsigned int iorec_cursize(io_record_t);
mach_error_t iorec_extract(io_record_t, unsigned int, char *, unsigned int);
mach_error_t iorec_read(io_record_t, io_mode_t, char *, unsigned int *);
}


/*
 * Find the current data size of a io_record_t.
 */
unsigned int iorec_cursize(io_record_t rec)
{
	io_block_t		curblk = rec->first_block;
	unsigned int		size = 0;

	while (curblk != NULL) {
		size += ioblk_cursize(curblk);
		curblk = curblk->next;
	}

	return(size);
}


/*
 * Extract the contents of a io_record_t into a contiguous user buffer.
 */
mach_error_t 
iorec_extract(io_record_t rec, unsigned int offset, char *addr, 
	      unsigned int len)
{
	io_block_t		curblk;
	unsigned int		xlen;

	/*
	 * Find the start of the desired area.
	 */
	curblk = rec->first_block;
	while (curblk != NULL) {
		if (ioblk_cursize(curblk) > offset) {
			break;
		}
		offset -= ioblk_cursize(curblk);
		curblk = curblk->next;
	}
	if (curblk == NULL) {
		return(IO_INVALID_OFFSET);
	}

	/*
	 * Fill the buffer.
	 */
	while ((len > 0) && (curblk != NULL)) {
		xlen = MIN(ioblk_cursize(curblk) - offset,len);
		bcopy(ioblk_start(curblk) + offset,addr,xlen);
		addr += xlen;
		len -= xlen;
		offset = 0;
		curblk = curblk->next;
	}
	if (len != 0) {
		return(US_INVALID_BUFFER_SIZE);
	}

	return(ERR_SUCCESS);
}


/*
 * Copy the whole contents of a io_record_t into a user buffer, in
 * accordance with the specifications for the io_read1rec() operation.
 */
mach_error_t iorec_read(io_record_t rec, io_mode_t mode, char *addr,
			unsigned int *len)
{
	io_block_t		curblk;
	unsigned int		resid;
	unsigned int		xlen;

	if (mode & IOM_PROBE) {
		*len = iorec_cursize(rec);
		return(ERR_SUCCESS);
 	}

	curblk = rec->first_block;
	resid = *len;

	while (curblk != NULL) {
		xlen = ioblk_cursize(curblk);
		if (xlen > resid) {
			*len = iorec_cursize(rec);
			return(US_INVALID_BUFFER_SIZE);
		}
		bcopy(ioblk_start(curblk),addr,xlen);
		addr += xlen;
		resid -= xlen;
		curblk = curblk->next;
	}

	*len -= resid;

	return(ERR_SUCCESS);
}

