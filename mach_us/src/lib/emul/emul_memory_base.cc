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
 * File: emul_memory_base.c
 *
 * Purpose: Define vm_allocate_base_address for lib/mach/emul_memory.c
 *
 * Michael B. Jones  --  21-Sep-1988
 *
 * HISTORY:
 * $Log:	emul_memory_base.cc,v $
 * Revision 2.3  94/07/08  16:57:07  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/06  11:30:09  jms
 * 	Fix the include of <mach/mach_types.h>
 * 	[91/09/17  14:04:11  jms]
 * 
 * 	Upgraded to US41.
 * 	[91/09/26  19:31:39  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:24:53  pjg]
 * 	Checking before c++ tty merge
 * 	[91/10/29  11:46:43  jms]
 * 
 * Revision 1.6  90/03/14  17:28:05  orr
 * 	nc
 * 	[90/03/14  16:51:32  orr]
 * 
 * Revision 1.5  89/03/31  15:20:27  mbj
 * 	Added definition for cthread_stack_base.  Cthreads will allocate all
 * 	thread stacks above this.
 * 	[89/03/31  11:37:51  mbj]
 * 
 * Revision 1.4  88/10/06  12:53:41  dorr
 * decouple emulation text and allocated memory into
 * two pieces.
 * 
 * Revision 1.3  88/10/05  14:12:09  dorr
 * split text and data locations so that we can decouple the
 * location of the emulation library text and static data from
 * the location of its vm_allocated memory.  if they're in the
 * same location then we have to figure out something better to do about
 * regions (so we don't deallocate it across exec).
 * 
 * Revision 1.2  88/09/22  14:50:36  mbj
 * Change to import base address from address_space.h.
 * 
 * Revision 1.1  88/09/21  16:59:04  mbj
 * Initial revision
 * 
 */

extern "C" {
#include <mach/mach_types.h>
#include <machine/machine_address_space.h>
}

/*
 * Base address used by vm_allocate and friends when under emulation.
 */
vm_address_t vm_allocate_base_address = EMULATION_BASE_DATA_ADDRESS;

/*
 * Base for cthread stack allocation when under emulation.
 */
vm_address_t cthread_stack_base = EMULATION_BASE_DATA_ADDRESS;
