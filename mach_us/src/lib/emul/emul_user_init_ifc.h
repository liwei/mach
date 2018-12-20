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
 * File:        emul_usr_init_ifc.h
 *
 * Purpose:
 *	
 *	Interface for file emul_usr_init.cc
 *
 * HISTORY: 
 * $Log:	emul_user_init_ifc.h,v $
 * Revision 2.3  94/07/08  16:57:32  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/06  11:33:19  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:45:17  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:32:03  pjg]
 * 
 */

#ifndef	_EMUL_USR_INIT_IFC_H
#define	_EMUL_USR_INIT_IFC_H

#include <std_name_ifc.h>

extern mach_error_t emul_user_init(ns_identity_t*, ns_token_t*);
extern emul_user_name_space_init(std_name**, ns_token_t);
extern process_args(int*, char***, char*);


#endif	_EMUL_USR_INIT_IFC_H
