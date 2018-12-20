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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_tm_root.cc,v $
 *
 * usTask: abstract class defining the task protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY
 * $Log:	us_tm_root.cc,v $
 * Revision 2.5  94/07/07  17:25:28  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  93/01/20  17:38:27  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  17:07:38  jms]
 * 
 * Revision 2.3  92/07/06  07:53:53  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.2  92/07/05  23:31:28  dpj
 * 	Define new us_tm_root_ifc.h interfaces for the C++ taskmaster.
 * 	Replaces tm_agency class from us_tm_task_ifc.h(dead file)
 * 	[92/06/24  16:49:35  jms]
 * 
 * 	Created.
 * 	[91/04/14  18:44:01  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 */

#include <us_tm_root_ifc.h>

#define BASE usItem
DEFINE_ABSTRACT_CLASS(usTMRoot);

DEFINE_METHOD_ARGS(tm_task_id_to_task,
	"rpc K<100501>: IN int; IN int; OUT * object<usItem>;");
DEFINE_METHOD_ARGS(tm_kernel_port_to_task,
	"rpc K<100502>: IN p(COPY_SEND); IN int; OUT * object<usItem>;");
DEFINE_METHOD_ARGS(tm_pre_register_forked_task,
	"rpc K<100503>: IN p(COPY_SEND); IN object<usItem>; IN OUT * int;");

#if SHARED_DATA_TIMING_EQUIVALENCE
DEFINE_METHOD_ARGS(tm_post_register_forked_task,
	"rpc K<100504>: IN p(COPY_SEND); IN object<usItem>; IN OUT * int; IN int; OUT * int; OUT * object<usItem>);");
#else
DEFINE_METHOD_ARGS(tm_post_register_forked_task,
	"rpc K<100504>: IN p(COPY_SEND); IN object<usItem>; IN int; OUT * int; OUT * object<usItem>);");
#endif SHARED_DATA_TIMING_EQUIVALENCE

DEFINE_METHOD_ARGS(tm_register_initial_task,
#if SHARED_DATA_TIMING_EQUIVALENCE
		"rpc K<100505>: IN p(COPY_SEND); IN object<usItem>; IN p(COPY_SEND); IN OUT * int; IN int; IN int; IN OUT * object<usItem>; IN OUT * int; OUT * object<usItem>);"
#else
	"rpc K<100505>: IN p(COPY_SEND); IN object<usItem>; IN p(COPY_SEND); IN int; IN int; IN OUT * object<usItem>; IN OUT * int; OUT * object<usItem>);"
#endif SHARED_DATA_TIMING_EQUIVALENCE
	);

DEFINE_METHOD_ARGS(tm_find_tgrp,
	"rpc K<100506>: IN int; IN int; OUT * object<usItem>");
