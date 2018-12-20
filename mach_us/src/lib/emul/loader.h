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
 * File:        loader.h
 *
 * Purpose:
 *	
 *	Interface for program loader.
 *
 * HISTORY: 
 * $Log:	loader.h,v $
 * Revision 2.3  94/07/08  16:57:38  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/13  17:17:00  dpj
 * 	First working version.
 * 	[91/11/12  17:50:40  dpj]
 * 
 */

#ifndef	_loader_h
#define	_loader_h

#include	<us_byteio_ifc.h>
#include	<loader_info.h>

extern mach_error_t loader_load_program_file(usByteIO*,struct loader_info*,
							vm_address_t*);

extern mach_error_t loader_ex_get_header(usByteIO*,struct loader_info*);
extern mach_error_t loader_set_entry_address(struct loader_info*,
							int*,unsigned int*);

#endif	_loader_h
