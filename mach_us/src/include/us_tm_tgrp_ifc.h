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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_tm_tgrp_ifc.h,v $
 *
 * usTMTgrp: abstract class defining the task group protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY:
 * $Log:	us_tm_tgrp_ifc.h,v $
 * Revision 2.3  94/07/08  15:52:04  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:24:11  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/29            dpj]
 * 
 * 	External task group interface replacing the task_group class from 
 * 	us_task_ifc.h (dead file).
 * 	[92/06/24  13:56:32  jms]
 * 
 * Revision 2.1  91/04/15  14:41:08  pjg
 * Created.
 * 
 * 
 */

#ifndef	_us_tm_tgrp_h
#define	_us_tm_tgrp_h

#include <us_name_ifc.h>
#include <us_event_ifc.h>

extern "C" {
#include <tm_types.h>
}

/****************************************\
 *					*
 *	   TM Tgrp Methods	   	*
 *					*
\****************************************/

class usTMTgrp: public VIRTUAL2 usName {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usTMTgrp);
//	static void initClass(usClass*);

/*
 * tm_get_tgrp_id: Get the task group id of a task group.
 *
 * Parameters:
 *
 * Results:
 *	tgrp_id [tm_tgrp_id_t *]:	Id of the group, -1 if unset
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_get_tgrp_id(
	    tm_tgrp_id_t *) = 0;	/* out tgrp_id */

/***************************
 *
 * Affect the members of a task group
 *
 ***************************/
/*
 * tm_event_to_tgrp: Post event to each task in a task group.  If
 *	own task in in the group, don't post to it and return "is_in" TRUE.
 *
 * Parameters:
 *	event [mach_error_t]:		Event being convied to the task group
 *	code [int]:			It's event/error/exception code
 *	subcode [int]:			It's subcode
 *	own_task_id [tm_task_id_t]:	Own task id.
 *					NULL_TASK_ID iff apriori not in group.
 * Results:
 *	is_in [boolean_t *]		Is own_task_obj in the task group?
 *					NULL iff apriori not in group.
 *
 * Side effects:
 *
 * Note:
 *
 */
REMOTE	virtual mach_error_t tm_event_to_tgrp(
	    mach_error_t,	/* event */
	    int,		/* code */
	    int,		/* subcode */
	    tm_task_id_t,	/* own_task_id */
	    boolean_t *) = 0;	/* out is_in */

}; /* end class usTMTgrp */

/*
 * Export those methods
 */
EXPORT_METHOD(tm_get_tgrp_id);
EXPORT_METHOD(tm_event_to_tgrp);
#endif	_us_tm_tgrp_h
