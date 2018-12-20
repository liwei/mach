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
 * HISTORY:
 * $Log:	emul_addr.c,v $
 * Revision 1.5  94/07/08  17:01:36  mrt
 * 	Updtated copyright
 * 
 * Revision 1.4  92/03/05  14:55:29  jms
 * 	Switch mach_types.h => mach/mach_types.h
 * 	[92/02/26  17:06:17  jms]
 * 
 * Revision 1.3  90/03/14  17:27:59  orr
 * 	gcc isn't generating a "return 0" if you
 * 	fall out of a procedure.  make it explicit.
 * 	[90/03/14  16:50:56  orr]
 * 
 */
#include <mach/mach_types.h>
#include <machine/machine_address_space.h>

main()
{
	printf( "%x\n", EMULATION_BASE_TEXT_ADDRESS );
	return 0;
}
