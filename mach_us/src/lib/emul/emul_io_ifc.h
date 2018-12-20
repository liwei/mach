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
 * File:        emul_io_ifc.h
 *
 * Purpose:
 *	
 *	Interface for file emul_io.cc
 *
 * HISTORY: 
 * $Log:	emul_io_ifc.h,v $
 * Revision 2.3  94/07/08  16:57:05  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/06  11:30:05  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:43:39  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:24:33  pjg]
 * 
 */

#ifndef	_emul_io_ifc_h
#define	_emul_io_ifc_h

#include <uxident_ifc.h>

extern emul_io_change_identity(uxident*);
extern void emul_io_init(void);
extern mach_error_t emul_io_exec(void);
extern mach_error_t emul_io_umask(int, ns_prot_t, int*);

#endif	_emul_io_ifc_h
