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
 * File: us/lib/emul/emul_memory.c
 *
 * Purpose: VM allocation routines for the emulation library and its
 * 	dependents. These routines make sure the memory gets allocated
 *	in the area that is not deallocated on exec().
 *
 * HISTORY:
 * $Log:	emul_memory.c,v $
 * Revision 2.5  94/07/08  16:57:06  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  92/03/05  14:55:54  jms
 * 	Switch mach_types.h => mach/mach_types.h
 * 	[92/02/26  17:17:13  jms]
 * 
 * Revision 2.3  91/07/01  14:06:44  jms
 * 	Noise
 * 	[91/06/24  16:07:27  jms]
 * 
 * 	Modify to call mig_vm_map and syscall_vm_map instead fo htg_vm_map.
 * 	[91/04/15  17:07:09  jms]
 * 
 * Revision 2.2  90/11/27  18:18:11  jms
 * 	Add pure kernel memory allocation
 * 	[90/11/20  17:10:33  jms]
 * 
 * Revision 2.1  90/08/20  17:17:12  jms
 * Created.
 * 
 * Revision 1.6  89/05/17  16:30:40  dorr
 * 	include file cataclysm
 * 
 * Revision 1.5.1.1  89/05/15  12:11:05  dorr
 * 	use generic mach_types include.  updated
 * 	mach_types... have to include std_types.h now, too.
 * 
 * Revision 1.5  88/10/19  14:40:37  sanzi
 * fix to poor vm_allocate behavior when anywhere is true.
 * 
 * 
 * Revision 1.4  88/09/23  15:30:03  sanzi
 * Re-implement vm_allocate in terms of vm_map().
 * 
 * Revision 1.3  88/09/21  16:26:22  mbj
 * Changed to define the base as an external symbol so it can be statically
 * defined to have different values.
 * 
 * Revision 1.2  88/09/21  14:48:53  dpj
 * Really made EMUL_MEMORY to work.
 * 
 * Revision 1.1  88/09/21  14:01:55  dpj
 * Initial revision
 * 
 *
 */

#include	"mach/mach_types.h"
#include	"mach/memory_object.h"
#include	"mach/vm_inherit.h"

/*
 * Base address for memory for the emulation system.
 */
extern vm_address_t	vm_allocate_base_address;


kern_return_t vm_allocate(target_task,address,size,anywhere)
	vm_task_t target_task;
	vm_address_t *address;
	vm_size_t size;
	boolean_t anywhere;
{
	if ((anywhere) && (*address < vm_allocate_base_address)) {
		*address = vm_allocate_base_address;
	}

    	return vm_map(target_task, address, size, 0, anywhere,
			MEMORY_OBJECT_NULL, 0, FALSE, VM_PROT_ALL,
			VM_PROT_ALL, VM_INHERIT_DEFAULT);
}


#if MACH3_UNIX
kern_return_t vm_allocate_with_pager(target_task,address,size,
					anywhere,memory_object,offset)
	vm_task_t target_task;
	vm_address_t *address;
	vm_size_t size;
	boolean_t anywhere;
	memory_object_t memory_object;
	vm_offset_t offset;
{
	if ((anywhere) && (*address < vm_allocate_base_address)) {
		*address = vm_allocate_base_address;
	}

	return emul_htg_vm_allocate_with_pager(target_task, address, size, 
					anywhere, memory_object, offset);
}
#endif MACH3_UNIX

kern_return_t vm_map(target_task,address,size,mask,anywhere,memory_object,
			offset,copy,cur_protection,max_protection,inheritance)
	vm_task_t target_task;
	vm_address_t *address;
	vm_size_t size;
	vm_address_t mask;
	boolean_t anywhere;
	memory_object_t memory_object;
	vm_offset_t offset;
	boolean_t copy;
	vm_prot_t cur_protection;
	vm_prot_t max_protection;
	vm_inherit_t inheritance;
{
	kern_return_t result;
	
	if ((anywhere) && (*address < vm_allocate_base_address)) {
		*address = vm_allocate_base_address;
	}

	result = syscall_vm_map(target_task, address, size, mask, anywhere,
				memory_object, offset, copy,
				cur_protection, max_protection, inheritance);
	if (result != KERN_SUCCESS)
		result = mig_vm_map(target_task, address, size, mask, anywhere,
				memory_object, offset, copy,
				cur_protection, max_protection, inheritance);
	return(result);
}


