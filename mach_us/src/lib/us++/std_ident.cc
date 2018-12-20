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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_ident.cc,v $
 *
 * Purpose: user identity holder.  used to encapsulate
 * identity and for reference counting purposes.
 *
 * HISTORY:
 * $Log:	std_ident.cc,v $
 * Revision 2.4  94/07/07  17:24:26  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/03/05  15:05:44  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:33:22  jms]
 * 
 * Revision 2.2  91/11/06  13:47:50  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:10:45  pjg]
 * 
 * 	[91/04/14  18:31:00  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:40:44  pjg]
 * 
 * Revision 2.1  89/05/15  14:41:47  dorr
 * Created.
 * 
 */

#ifndef lint
char * std_identpp_rcsid = "$Header: std_ident.cc,v 2.4 94/07/07 17:24:26 mrt Exp $";
#endif	lint

#include <std_ident_ifc.h>

#define BASE usTop
DEFINE_LOCAL_CLASS(std_ident);

std_ident::std_ident(ns_identity_t id, ns_token_t tok)
	:
	ident(id), token (tok)
{}


std_ident::~std_ident()
{
	(void) mach_port_deallocate(mach_task_self(),_Local(token));
	(void) mach_port_deallocate(mach_task_self(),_Local(ident));
}

mach_error_t std_ident::clone_init(mach_port_t child)
{
	DEBUG2(TRUE, (0, "std_ident::_clone_init\n"));
	(void) mach_port_insert_right(child,_Local(ident),_Local(ident),
				      MACH_MSG_TYPE_COPY_SEND);
	(void) mach_port_insert_right(child,_Local(token),_Local(token),
				      MACH_MSG_TYPE_COPY_SEND);
	return ERR_SUCCESS;
}

mach_error_t std_ident::ns_get_token(ns_token_t *token)
{
	*token = _Local(token);
	return(ERR_SUCCESS);
}

mach_error_t std_ident::ns_get_identity(ns_identity_t *identity)
{
	*identity = _Local(ident);
	return(ERR_SUCCESS);
}

