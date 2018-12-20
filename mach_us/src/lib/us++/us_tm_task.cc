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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_tm_task.cc,v $
 *
 * usTask: abstract class defining the task protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY
 * $Log:	us_tm_task.cc,v $
 * Revision 2.6  94/10/27  12:01:49  jms
 * 	Add tm_get_shared_info method
 * 	[94/10/26  14:49:01  jms]
 * 
 * Revision 2.5  94/07/07  17:25:29  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  93/01/20  17:38:30  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  17:07:52  jms]
 * 
 * Revision 2.3  92/07/06  07:53:55  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.2  92/07/05  23:31:30  dpj
 * 	Implement new us_tm_task_ifc.h interfaces for the C++ taskmaster.
 * 	[92/06/24  16:50:55  jms]
 * 
 * 	Created.
 * 	[91/04/14  18:44:01  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 * Revision 2.2  91/11/06  14:09:35  jms
 * 	Upgraded to US41.
 * 	[91/10/07  13:55:57  pjg]
 * 
 */

#ifndef lint
char * us_tm_task_rcsid = "$Header: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_tm_task.cc,v 2.6 94/10/27 12:01:49 jms Exp $";
#endif	lint

#include <us_tm_task_ifc.h>

#define BASE usItem
DEFINE_ABSTRACT_CLASS(usTMTask);

DEFINE_METHOD_ARGS(tm_get_task_id, "rpc K<100601>: OUT * int;");
DEFINE_METHOD_ARGS(tm_get_kernel_port, "rpc K<100602>: OUT * p(COPY_SEND);");
DEFINE_METHOD_ARGS(tm_change_task_auth, "rpc K<100603>: IN p(COPY_SEND);");

DEFINE_METHOD_ARGS(tm_debug_children_of, "rpc K<100604>: IN int;");

DEFINE_METHOD_ARGS(tm_get_parent, "rpc K<100605>: IN int; OUT * object<usItem>;");
DEFINE_METHOD_ARGS(tm_get_tgrp, "rpc K<100606>: IN int; OUT * object<usItem>;");
DEFINE_METHOD_ARGS(tm_set_tgrp, "rpc K<100607>: IN object<usItem>;");

DEFINE_METHOD_ARGS(tm_get_task_emul_status, "rpc K<100608>: OUT * int;");
DEFINE_METHOD_ARGS(tm_set_task_emul_status, "rpc K<100609>: IN int; IN int;");
DEFINE_METHOD_ARGS(tm_hurtme, "rpc K<100610>: IN int; IN int; IN int;");
DEFINE_METHOD_ARGS(tm_event_to_task, "rpc intr K<100611>: IN int; IN int; IN int;");

DEFINE_METHOD_ARGS(tm_timer_get, "rpc K<100612>: IN int; IN int; OUT *int [4];");
DEFINE_METHOD_ARGS(tm_timer_set, "rpc K<100613>: IN int; IN int; IN int; IN int; IN OUT *int; IN *int [4];");
DEFINE_METHOD_ARGS(tm_timer_delete, "rpc K<100614>: IN int; OUT *int [4];");

#if SHARED_DATA_TIMING_EQUIVALENCE
    DEFINE_METHOD_ARGS(tm_touch_shared, "rpc K<100615>: IN int;");
    DEFINE_METHOD_ARGS(tm_get_shared_info, "rpc K<100616>: OUT * int; OUT * int; OUT * int; OUT string;");
#endif SHARED_DATA_TIMING_EQUIVALENCE

