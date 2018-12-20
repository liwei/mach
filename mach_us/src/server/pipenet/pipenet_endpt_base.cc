/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_endpt_base.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for directories in the pipenet server.
 *
 * HISTORY
 * $Log:	pipenet_endpt_base.cc,v $
 * Revision 2.3  94/07/13  17:21:37  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  91/11/06  14:21:41  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:54:45  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:09  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:26  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:59:41  dpj]
 * 
 */

#ifndef lint
char * pipenet_endpt_base_rcsid = "$Header: pipenet_endpt_base.cc,v 2.3 94/07/13 17:21:37 mrt Exp $";
#endif	lint

#include	<pipenet_endpt_base_ifc.h>

#define BASE net_endpt_base
DEFINE_ABSTRACT_CLASS(pipenet_endpt_base)

pipenet_endpt_base::pipenet_endpt_base(ns_mgr_id_t mgr_id,
				       access_table *access_tab)
:
 net_endpt_base(mgr_id,access_tab)
{}
