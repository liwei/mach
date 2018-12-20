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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_pipe_ifc.h,v $
 *
 *
 * HISTORY
 * $Log:	uxio_pipe_ifc.h,v $
 * Revision 2.6  94/07/08  16:02:06  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.5  91/11/06  14:12:22  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:16:36  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:10:46  pjg]
 * 
 * Revision 2.4  91/05/05  19:28:49  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:13  dpj]
 * 
 * 	Added initialize() method.
 * 	[91/04/28  10:32:44  dpj]
 * 
 * Revision 2.3  90/12/10  09:50:11  jms
 * 	Added ux_map method declaration.
 * 	[90/11/20  14:01:25  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:37  jms]
 * 
 * Revision 2.2  90/11/10  00:38:55  dpj
 * 	Subclass of uxio representing pipes.
 * 	[90/10/17  13:19:47  neves]
 * 
 */

#ifndef	_uxio_pipe_ifc_h
#define	_uxio_pipe_ifc_h


#include <uxio_ifc.h>

class uxio_pipe: public uxio {
      public:
	DECLARE_LOCAL_MEMBERS(uxio_pipe);
	uxio_pipe();
	virtual mach_error_t ux_lseek(long int*, unsigned int);
	virtual mach_error_t ux_ftruncate(unsigned int);
	virtual mach_error_t ux_modify_protection(int, int, int);
	virtual mach_error_t ux_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t);
};
#endif	_uxio_pipe_ifc_h
