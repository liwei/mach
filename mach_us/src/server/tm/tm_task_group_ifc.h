/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 * File:  tm_task_group_ifc.h
 *
 * Purpose:  Mach Task Master task group object definition
 *
 * J. Mark Stevenson
 *
 * 12/19/89
 */

/*
 * HISTORY
 * $Log:	tm_task_group_ifc.h,v $
 * Revision 2.6  94/07/13  17:33:40  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  92/07/05  23:35:48  dpj
 * 	Cleaned-up uses of GXXBUG_VIRTUAL2.
 * 	[92/07/05  19:02:58  dpj]
 * 
 * 	Use new us_tm_tgrp_ifc.h interface for the C++ taskmaster.
 * 	Translate to G++
 * 	Implement as tmpdir's in tm/GROUPS which contain task objects
 * 	[92/06/24  18:14:15  jms]
 * 
 * Revision 2.4  92/03/05  15:13:23  jms
 * 	Switch mach_types.h => mach/mach_types.h
 * 	[92/02/26  19:33:46  jms]
 * 
 * Revision 2.3  90/10/29  18:13:03  dpj
 * 	Fixed an old CPP syntax error.
 * 	[90/10/27  18:21:22  dpj]
 * 
 * 	Fixed an old CPP syntax error.
 * 	[90/10/21  23:29:02  dpj]
 * 
 * Revision 2.2  90/03/21  17:29:59  jms
 * 	Comment fixes
 * 	[90/03/16  17:39:15  jms]
 * 
 * 	First objectified Task Master checkin
 * 	[89/12/19  16:24:58  jms]
 * 
 */
#ifndef	_tm_task_group_ifc_h
#define	_tm_task_group_ifc_h

extern "C" {
#include	<mach/mach_types.h>
#include	<dll.h>
#include	"tm_types.h"
}

#include	<agency_ifc.h>
#include	<us_tm_tgrp_ifc.h>
#include	<us_tm_task_ifc.h>
#include	<us_item_ifc.h>
#include	<tmp_dir_ifc.h>
#include	"ns_types.h"

typedef struct tm_task_chain {
	usTMTask	* task;
	dll_chain_t	task_chain;
} *tm_task_chain_t;

#ifdef	GXXBUG_VIRTUAL2
class tm_task_group: public tmp_dir {
#else	GXXBUG_VIRTUAL2
class tm_task_group: public tmp_dir, public usTMTgrp {
#endif	GXXBUG_VIRTUAL2
      public:
	DECLARE_MEMBERS(tm_task_group);

      private:
	tm_tgrp_id_t	group_id;

      public:
	virtual char* remote_class_name() const;

	tm_task_group();
	tm_task_group(
	    tm_tgrp_id_t,	/* group_id */
	    ns_mgr_id_t,	/* mgr_id */
	    access_table *);	/* acctab */

	~tm_task_group();

	/*
	 * tm_add_tgrp_task: Add a task to a task group
	 *
	 * Parameters:
	 *	task [usTMTask *]:		Task to add to the group
	 *
	 * Results:
	 *
	 * Side effects:
	 *
	 * Note:
	 *
	 */
	mach_error_t tm_add_tgrp_task(
	    usTMTask *);	/* task */

	/*
	 * tm_remove_tgrp_task: Remove a task from a task group
	 *
	 * Parameters:
	 *	task [usTMTask *]:		Task to remove from the group
	 *
	 * Results:
	 *
	 * Side effects:
	 *
	 * Note:
	 *
	 */
	mach_error_t tm_remove_tgrp_task(
	    usTMTask *);		/* task */

	/*
	 * Get the job group id of a task group. See usTMTgrp.
	 */
REMOTE	virtual mach_error_t tm_get_tgrp_id(
	    tm_tgrp_id_t *);	/* out tgrp_id */

	/*
	 * Post an event to each task in a task group. See usTMTgrp.
	 */
REMOTE	virtual mach_error_t tm_event_to_tgrp(
	    mach_error_t,	/* event */
	    int,		/* code */
	    int,		/* subcode */
	    tm_task_id_t,	/* own_task_id */
	    boolean_t *);	/* out is_in */
};
#endif	_tm_task_group_ifc_h
