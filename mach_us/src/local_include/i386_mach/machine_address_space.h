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
 *
 * Purpose:  Machine specific emulation address space constants.
 *
 * HISTORY: 
 * $Log:	machine_address_space.h,v $
 * Revision 2.3  94/07/08  18:41:49  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  90/03/14  17:30:14  orr
 * 	get the numbers right for the emulation library location.
 * 	[90/03/14  17:00:57  orr]
 * 
 * 	 initial checkin.
 * 	[90/02/04  17:02:27  orr]
 * 
 * Revision 1.2  88/10/05  14:05:31  dorr
 * split machine address space into two pieces to decouple location of
 * the emulation library from the location of its vm_allocated data.
 * 
 * Revision 1.1  88/09/22  14:42:04  mbj
 * Initial revision
 * 
 */

#ifndef	_MACHINE_ADDRESS_SPACE_H
#define	_MACHINE_ADDRESS_SPACE_H

/*
 * Base address of the emulation library and all memory it allocates
 */

#define EMULATION_BASE_TEXT_ADDRESS (vm_address_t) 0x09000000

#define EMULATION_BASE_DATA_ADDRESS (vm_address_t) 0x0a000000

#endif	_MACHINE_ADDRESS_SPACE_H
