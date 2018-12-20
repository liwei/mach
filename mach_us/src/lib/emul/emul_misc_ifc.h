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
 * File:        emul_misc_ifc.h
 *
 * Purpose:
 *	
 *	Interface for file emul_misc.cc
 *
 * HISTORY: 
 * $Log:	emul_misc_ifc.h,v $
 * Revision 2.4  94/07/08  16:57:11  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  91/11/13  16:42:38  dpj
 * 	Changed for new initialization sequence through the normal entry point
 * 	in the a.out file.
 * 	[91/11/12  17:48:24  dpj]
 * 
 * Revision 2.2  91/11/06  11:30:16  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:44:45  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:29:03  pjg]
 * 
 */

#ifndef	_EMUL_MISC_IFC_H
#define	_EMUL_MISC_IFC_H

extern "C" mach_error_t emul_initialize(ns_identity_t, ns_token_t,
					int, int, int, int, char*);
extern open_debug_connection(int);
extern mach_error_t emul_pre_fork(task_t);
extern mach_error_t emul_post_fork(void);
extern mach_error_t emul_cleanup_on_exit(void);
mach_error_t emul_path(char*, char*, int, char*, int);
mach_port_t emul_host_port();


#endif	_EMUL_MISC_IFC_H
