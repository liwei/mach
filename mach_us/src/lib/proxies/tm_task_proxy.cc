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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/proxies/tm_task_proxy.cc,v $
 *
 * 
 * Purpose:  client proxy for a task
 * 
 * HISTORY:
 * $Log:	tm_task_proxy.cc,v $
 * Revision 2.3  94/07/07  17:59:31  mrt
 * 	Updated copyrights
 * 
 * Revision 2.2  94/01/11  17:49:22  jms
 * 	Proxy moved from .../lib/us++
 * 	[94/01/09  18:53:32  jms]
 * 
 * Revision 2.3  92/07/05  23:29:11  dpj
 * 	Fixed-up the mess with the RPC constructor.
 * 	Use NULL_TASK_ID explicitly (NOT 0).
 * 	[92/07/05  18:55:45  dpj]
 * 
 * 	Use new us_tm_{root,task,tgrp}_ifc.h interfaces for the C++ taskmaster.
 * 	Remove stuff for "tm_agency" and "tm_jgrp"
 * 	Init task_id to NULL_TASK_ID which != 0
 * 	[92/06/24  16:02:22  jms]
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:00:50  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:42  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:40:28  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:38:29  pjg]
 * 
 */

#include <tm_task_proxy_ifc.h>


#define BASE usTMTask_proxy
_DEFINE_CLASS(tm_task_proxy);
_DEFINE_INSTANTIATION(tm_task_proxy);
_DEFINE_CASTDOWN(tm_task_proxy);
_DEFINE_CONVERTER(tm_task_proxy);


void tm_task_proxy::init_class(usClass* class_obj)
{
        BASE::init_class(class_obj);

        BEGIN_SETUP_METHOD_WITH_ARGS(tm_task_proxy);
        SETUP_METHOD_WITH_ARGS(tm_task_proxy,tm_get_task_id);
        END_SETUP_METHOD_WITH_ARGS;
}


tm_task_proxy::tm_task_proxy()
: 
  task_id(NULL_TASK_ID)
{
}

tm_task_proxy::tm_task_proxy(void* p, const usClass& c)
: 
	task_id(NULL_TASK_ID),
	usTMTask_proxy(p,c)
{
}


mach_error_t tm_task_proxy::tm_get_task_id(tm_task_id_t* t_id)
{
	mach_error_t	ret = ERR_SUCCESS;

	if (task_id == NULL_TASK_ID)
		ret = outgoing_invoke(mach_method_id(tm_get_task_id),&task_id);
	*t_id = task_id;
	return ret;
}
