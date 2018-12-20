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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/local_include/base_ifc.h,v $
 *
 * Purpose: Base class for the user side of all I/O objects.
 *
 * HISTORY:
 * $Log:	base_ifc.h,v $
 * Revision 1.5  94/07/08  18:42:46  mrt
 * 	Updated copyright
 * 
 * Revision 1.4  89/07/09  14:20:33  dpj
 * 	Removed include of base_methods.h.
 * 	Added include of base.h.
 * 	Reorganized to work with the new Diag object.
 * 	[89/07/08  13:07:10  dpj]
 * 
 * Revision 1.3  89/03/17  12:58:35  sanzi
 * 	Add debugging port.
 * 	[89/03/07  08:53:55  sanzi]
 * 	
 * 	Derive from MachObject instead of top_level_class.
 * 	[89/02/14  18:03:02  dpj]
 * 
 * Revision 1.2  88/11/15  19:05:17  dorr
 * making defunct
 * 
 * Revision 1.1  88/11/01  16:56:46  dorr
 * Initial revision
 * 
 */

#ifndef	_base_ifc_h
#define	_base_ifc_h

#include <mach_object.h>
#include <MachObject.h>
#include <base.h>

begin_class_locals(base, MachObject)
	int			diag_level;
	char			*diag_name;
end_class_locals(base, MachObject);

	declare_method(initialize, base, mach_error_t);

	declare_method(set_diag_level, base, mach_error_t);
	declare_method(get_diag_level, base, mach_error_t);
	declare_method(set_diag_name, base, mach_error_t);
	declare_method(get_diag_name, base, mach_error_t);
	declare_method(diag_init_mesg, base, mach_error_t);

#endif	_base_ifc_h

