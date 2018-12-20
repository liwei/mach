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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/tm_task_proxy_ifc.h,v $
 *
 * Purpose: Proxy for task objects.
 *
 * HISTORY:
 * $Log:	tm_task_proxy_ifc.h,v $
 * Revision 2.3  94/07/07  17:59:33  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:26  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:54:00  jms]
 * 
 * Revision 2.3  92/07/05  23:29:15  dpj
 * 	Use new us_tm_task_ifc.h interface for the C++ taskmaster.
 * 	[92/06/24  16:09:04  jms]
 * 	Eliminated _init_task_id().
 * 	[92/05/10  01:01:06  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:45  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:57:33  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:38:44  pjg]
 * 
 */

#ifndef	_tm_task_proxy_ifc_h
#define	_tm_task_proxy_ifc_h

#include <us_tm_task_proxy_ifc.h>

class tm_task_proxy: public usTMTask_proxy {
	tm_task_id_t task_id;
      public:
	DECLARE_PROXY_MEMBERS(tm_task_proxy);
	tm_task_proxy();

REMOTE	virtual mach_error_t tm_get_task_id(tm_task_id_t*);
};
	
#endif	_tm_task_proxy_ifc_h
