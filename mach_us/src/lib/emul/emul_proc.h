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
 * emul_proc.h
 *
 * Exported definitions for emulation of unix process management primitives.
 *
 * Michael B. Jones  --  15-Sep-1988
 */
/*
 * HISTORY:
 * $Log:	emul_proc.h,v $
 * Revision 1.10  94/10/27  12:01:36  jms
 * 	Shared_info type change
 * 	[94/10/26  14:45:39  jms]
 * 
 * Revision 1.9  94/07/08  16:57:16  mrt
 * 	Updated copyrights.
 * 
 * Revision 1.8  93/01/20  17:36:53  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  15:59:58  jms]
 * 
 * Revision 1.7  92/07/05  23:25:27  dpj
 * 	Use new us_tm_{root,task,tgrp}_ifc.h interfaces for the C++ taskmaster.
 * 	[92/06/24  14:33:25  jms]
 * 
 * Revision 1.6  91/11/06  11:30:24  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:45:03  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:29:42  pjg]
 * 
 * Revision 1.5  90/07/09  17:02:28  dorr
 * 	Add tm_job_group_obj.
 * 	[90/07/06  14:58:51  jms]
 * 
 * Revision 1.4  90/03/21  17:21:08  jms
 * 	Name changes.
 * 	[90/03/16  16:28:56  jms]
 * 
 * 	first object based checkin
 * 	[89/12/19  16:08:08  jms]
 * 
 *
 * Revision 1.3  89/05/04  17:25:24  mbj
 * 	Merge up to U3.
 * 	[89/04/17  15:15:07  mbj]
 * 
 * Revision 1.2.1.1  89/03/31  16:07:16  mbj
 * 	Create mbj_signal branch
 * 
 * Revision 1.1.1.1  89/03/30  16:50:21  mbj
 * 	Add tm_notification_port declaration.
 * 
 * Revision 1.2  89/03/21  14:17:47  mbj
 * 	Merge mbj_pgrp branch onto mainline.
 * 
 * Revision 1.1  88/09/15  15:12:35  mbj
 * Initial revision
 */

#include <us_tm_root_ifc.h>
#include <us_tm_task_ifc.h>
#include <us_tm_tgrp_ifc.h>
#include <uxsignal_ifc.h>

/*
 * Though currently noops, they many not always be so...
 */

#define pid_to_task_id(pid) (pid)
#define task_id_to_pid(task_id) (task_id)

#define pgrp_to_job_group(pgrp) (pgrp)
#define job_group_to_pgrp(job_group) (job_group)


usTMRoot*	tm_obj;	    	/* Public connection to Task Master */
usTMTask*	tm_task_obj;	/* Task Master object for this task */
usTMTask*	tm_parent_obj;	/* Parent Task object for this task */
uxsignal*	uxsignal_obj;	/* object to send events to task */
#if SHARED_DATA_TIMING_EQUIVALENCE
tm_shared_info_t shared_info;	/* the address of the shared space */
#endif SHARED_DATA_TIMING_EQUIVALENCE

