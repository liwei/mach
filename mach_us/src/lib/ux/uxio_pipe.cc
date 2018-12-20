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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_pipe.cc,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxio_pipe.cc,v $
 * Revision 2.3  94/07/08  16:02:05  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/06  14:12:12  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:16:26  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:10:35  pjg]
 * 
 * Revision 2.4  91/05/05  19:28:46  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:08  dpj]
 * 
 * 	Use sequential versions of I/O operations when appropriate.
 * 	[91/04/28  10:32:20  dpj]
 * 
 * Revision 2.3  90/12/10  09:50:07  jms
 * 	Overrided superclass' ux_map implementation.
 * 	[90/11/20  14:01:13  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:33  jms]
 * 
 * Revision 2.2  90/11/10  00:38:53  dpj
 * 	Subclass of uxio representing pipes.
 * 	[90/10/17  13:11:25  neves]
 * 
 *
 */

#ifndef lint
char * uxio_pipe_rcsid = "$Header: uxio_pipe.cc,v 2.3 94/07/08 16:02:05 mrt Exp $";
#endif	lint

#include <uxio_pipe_ifc.h>

#define BASE uxio
DEFINE_LOCAL_CLASS(uxio_pipe)


uxio_pipe::uxio_pipe()
{
	ux_set_sequential_internal();
}

mach_error_t
uxio_pipe::ux_ftruncate(unsigned int len)
{
        return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxio_pipe::ux_lseek(long int *pos, unsigned int mode)
{
        return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxio_pipe::ux_modify_protection(int uid, int gid, int mode)
{
        return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxio_pipe::ux_map(task_t task, vm_address_t* addr, vm_size_t size,
		  vm_offset_t mask, boolean_t anywhere, 
		  vm_offset_t paging_offset,
		  boolean_t copy, vm_prot_t cprot, vm_prot_t mprot,
		  vm_inherit_t inherit)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}
