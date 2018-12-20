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
 * tm_types.h
 *
 * Mach Task Master type definitions.
 *
 * Michael B. Jones
 *
 * 07-Jul-1988
 */

/*
 * HISTORY:
 * $Log:	tm_types.h,v $
 * Revision 1.12  94/10/27  12:01:16  jms
 * 	Add the tm_shared_info_struct for holdin info in mem shared between
 * 	the client and the task_master.
 * 	[94/10/26  14:35:26  jms]
 * 
 * Revision 1.11  94/07/08  15:51:42  mrt
 * 	Updated copyright.
 * 
 * Revision 1.10  94/05/17  13:35:41  jms
 * 	New Error Message for too many tasks in session
 * 	[94/05/11  14:38:48  modh]
 * 
 * Revision 1.9  92/07/05  23:23:18  dpj
 * 	Change Job groups to Task groups.
 * 	Waste bogus tm_{user,group}_id stuff
 * 	Define MAXes of ascii reps of task/group ids
 * 	[92/06/24  13:38:53  jms]
 * 
 * Revision 1.8  91/11/06  11:27:58  jms
 * 	Initial C++ revision.
 * 	[91/09/26  17:20:38  pjg]
 * 
 * Revision 1.7  90/11/27  18:17:05  jms
 * 	No change
 * 	[90/11/19  22:23:29  jms]
 * 
 * 	Create the TM_NO_MORE_TASK_IDS taskmaster error
 * 	[90/08/20  17:02:52  jms]
 * 
 * Revision 1.6  90/03/21  16:32:55  jms
 * 	Add some NULL group id values
 * 	[90/03/16  16:02:56  jms]
 * 
 * 	first objectified Task Master checkin
 * 	[89/12/19  16:07:26  jms]
 * 
 * Revision 1.5  89/07/09  14:16:46  dpj
 * 	Updated error codes for new unified scheme.
 * 	[89/07/08  12:31:57  dpj]
 * 
 * Revision 1.4  89/05/17  16:05:02  dorr
 * 	include file cataclysm
 * 
 * Revision 1.3  89/05/04  17:24:46  mbj
 * 	Merge up to U3.
 * 	[89/04/17  15:12:15  mbj]
 * 
 * Revision 1.2.1.1  89/03/31  16:01:55  mbj
 * 	Create mbj_signal branch
 * 
 * Revision 1.1.1.2  89/03/30  16:39:11  mbj
 * 	Add NULL_* constants and a correct tm_notice_string_t.
 * 
 * Revision 1.2  89/03/21  14:05:41  mbj
 * 	Merged in mbj_pgrp changes.
 * 
 * Revision 1.1.1.1  89/03/02  10:16:59  mbj
 * 	Added tm_action_t type, TM_ACTION_* constants and more error numbers.
 * 
 *  7-Jul-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

/*****************************************************************************\
*									      *
*			      Type Definitions				      *
*									      *
\*****************************************************************************/

#ifndef _TM_TYPES_
#define _TM_TYPES_

#include <mach/mach_types.h>		/* Basic mach types */
#include <mach_error.h>
#include <cthreads.h>

typedef int tm_task_id_t;	/* Task identifier (local to particular tm) */

typedef int tm_tgrp_id_t;	/* a task's tgrp ID (BSD) (local to a tm) */

typedef int tm_priority_t;	/* Priority number (within priority class?) */
typedef int tm_result_t;	/* Result from a terminated process */

/*
 * Various constants for above types.
 */

#define NULL_TASK_ID		((tm_task_id_t) -1)
#define NULL_TGRP_ID		((tm_tgrp_id_t) -1)

typedef char file_name_t[1024];	/* File name (for now) */

/*
 * Argument and environment lists are structured as follows:
 *
 *	First 4 * argc bytes:
 *	    length in bytes of each argument (without null) in 4 byte net order
 *	Next 4 bytes:
 *	    0xffffffff to indicate the end of the argument counts
 *	Finally, for each argument:
 *	    argument string, '\0', plus enough pad bytes to start the next
 *	    argument on a 4 byte boundary.
 *
 * While this structure places a small burden of assembly upon the sender, it
 * means that only the client need actually scan each string.  The receiver
 * can simply do address arithmetic in order to set up an argv or envp.
 */

typedef char *arg_list_t;
typedef arg_list_t env_list_t;

/*
 * Task_Master error definitions
 */

#define TM_ERR				(err_server|err_sub(5))

#define TM_INVALID_TASK			(TM_ERR|0x0001)
#define TM_INVALID_TASK_ID		(TM_ERR|0x0002)
#define TM_INVALID_KERNEL_PORT		(TM_ERR|0x0003)
#define TM_INVALID_TASK_GROUP		(TM_ERR|0x0004)
#define TM_NO_MORE_TASK_IDS		(TM_ERR|0x0005)
#define TM_INVALID_TASK_GROUP_ID	(TM_ERR|0x0006)
#define TM_TOO_MANY_TASKS_IN_SESSION    (TM_ERR|0x0007)

/*
 * Sizes of strings used to contain numeric ids
 */
#define TASK_ID_STR_MAX 64
#define TASK_GROUP_ID_STR_MAX TASK_ID_STR_MAX

#if SHARED_DATA_TIMING_EQUIVALENCE
/* 
 * Types for space shared between the taskmaster and any emulation lib.
 */
/* The amount of space to allocate for sharing (larger than struct) */
#define SHARED_SIZE vm_page_size
#define SHARED_EXEC_STR_MAX 1024
typedef struct tm_shared_info_struct {
	tm_task_id_t	id;		/* id of target task, never changes */
	int		touch;		/* place to touch */
	struct mutex	lock;		/* lock for rest of struct */
	tm_task_id_t	parent_id;	/* id for parent of task */
	char		exec_string[SHARED_EXEC_STR_MAX]; 
					/* string for last exec of job */
} *tm_shared_info_t;
#endif SHARED_DATA_TIMING_EQUIVALENCE
#endif _TM_TYPES_
