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
 * File:        emul_all_ifc.h
 *
 * Purpose:
 *	
 *	Interface for file emul_all.cc
 *
 * HISTORY: 
 * $Log:	emul_all_ifc.h,v $
 * Revision 2.4  94/07/08  16:56:38  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  92/07/05  23:24:26  dpj
 * 	Add ns_token_t arg to emul_init_task_master for correct initial task
 * 	registration.
 * 	[92/06/24  14:01:43  jms]
 * 
 * Revision 2.2  91/11/06  11:29:17  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:41:22  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:21:02  pjg]
 * 
 */

#ifndef	_EMUL_ALL_IFC_H
#define	_EMUL_ALL_IFC_H

#include <std_name_ifc.h>

extern mach_error_t emul_init_task_master(ns_token_t);
extern mach_error_t emul_init_namespace(ns_identity_t, ns_token_t);
extern mach_error_t emul_init_io(void);
extern mach_error_t emul_init_more_io(void);
extern emul_init_signals(void);

#endif	_EMUL_ALL_IFC_H
