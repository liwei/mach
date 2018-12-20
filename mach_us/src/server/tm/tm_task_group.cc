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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tm/tm_task_group.cc,v $
 *
 * Purpose: Task object implementation.  Interface to the task master for 
 *		manipulating / querying specific tasks
 *
 * HISTORY:
 * $Log:	tm_task_group.cc,v $
 * Revision 2.4  94/10/27  12:02:00  jms
 * 	Task reference bug fix.
 * 	[94/10/26  14:55:50  jms]
 * 
 * Revision 2.3  94/07/13  17:33:37  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  92/07/05  23:35:46  dpj
 * 	Cleaned-up uses of GXXBUG_VIRTUAL2.
 * 	[92/07/05  19:02:40  dpj]
 * 
 * 	Use new us_tm_tgrp_ifc.h interfaces for the C++ taskmaster.
 * 	Translated to G++.
 * 	Implement task groups as tmp_dirs in the tm/GROUPS directory.
 * 	[92/06/24  18:12:31  jms]
 * 
 * Revision 2.4  91/07/01  14:15:26  jms
 * 	Change tm_event_to_tgrp so that it doe not send a signal to the caller
 * 	and tells the caller if it was contained in said tgrp.
 * 	[91/06/25  11:54:15  jms]
 * 
 * Revision 2.3  90/07/09  17:12:15  dorr
 * 	No Changes
 * 	[90/07/09  11:21:53  jms]
 * 
 * Revision 2.2  90/03/21  17:29:53  jms
 * 	Comment and locking fixes
 * 	[90/03/16  17:38:44  jms]
 * 
 * 	First objectified Task Master checkin
 * 	[89/12/19  16:24:46  jms]
 * 
 */

#include	<tm_root_ifc.h>
#include	<tm_task_ifc.h>
#include	<tm_task_group_ifc.h>
#include	<dir_ifc.h>

#ifdef	GXXBUG_VIRTUAL2
#define BASE tmp_dir
DEFINE_CLASS(tm_task_group);
#else	GXXBUG_VIRTUAL2
DEFINE_CLASS_MI(tm_task_group);
DEFINE_CASTDOWN2(tm_task_group, tmp_dir, usTMTgrp);
#endif	GXXBUG_VIRTUAL2

/*
 * Class Methods
 */
void tm_task_group::init_class(usClass* class_obj)
{
#ifdef	GXXBUG_VIRTUAL2
#else	GXXBUG_VIRTUAL2
    usTMTgrp::init_class(class_obj);
#endif	GXXBUG_VIRTUAL2
    tmp_dir::init_class(class_obj);

    BEGIN_SETUP_METHOD_WITH_ARGS(tm_task_group);
    SETUP_METHOD_WITH_ARGS(tm_task_group, tm_get_tgrp_id);
    SETUP_METHOD_WITH_ARGS(tm_task_group, tm_event_to_tgrp);
    END_SETUP_METHOD_WITH_ARGS;
}

/*
 * Housekeeping methods.
 */
char* tm_task_group::remote_class_name() const
{
	return "tm_tgrp_proxy";
}

tm_task_group::tm_task_group(){0;};

tm_task_group::tm_task_group(
    tm_tgrp_id_t	group_id_a,
    ns_mgr_id_t		mgr_id,
    access_table	*acctab)
    :
    group_id(group_id_a),
    tmp_dir(mgr_id,acctab)
{
}

tm_task_group::~tm_task_group()
{
}


/****************************************\
 *					*
 *	   Task Group Methods   	*
 *					*
\****************************************/

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
 * Note: Only usable internally to the task master
 *
 */
mach_error_t tm_task_group::tm_add_tgrp_task(
	usTMTask	*task)
{
    ns_prot_t		prot;
    int			prot_len;
    char		task_path[TASK_ID_STR_MAX+TASK_DIR_PATH_LEN] = 
					TASK_DIR_PATH;
    char		*task_name = (char*)task_path+TASK_DIR_PATH_LEN;
    mach_error_t	ret;
    tm_task_id_t	task_id;

    (void)task->tm_get_task_id(&task_id);
    sprintf(task_name, "%d", task_id);

    ret = ns_insert_entry(task_name, task);
    return(ret);
}

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
mach_error_t tm_task_group::tm_remove_tgrp_task(usTMTask *task)
{
    char		task_id_str[TASK_ID_STR_MAX];
    tm_task_id_t	task_id;
    mach_error_t	ret;

    (void)task->tm_get_task_id(&task_id);
    sprintf(task_id_str, "%d", task_id);
    ret = ns_remove_entry(task_id_str);
    return(ret);
}

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
mach_error_t tm_task_group::tm_event_to_tgrp(
    mach_error_t	event,
    int			code,
    int			subcode,
    tm_task_id_t	own_task_id,
    boolean_t		*is_in)
{
    mach_error_t	ret;
    dir_iterator	*iterator;
    dir_entry_t		entry;

    tm_task_id_t	tmp_task_id;
    *is_in = FALSE;

    usItem		*task_item;
    tm_task		*task;

    iterator = new dir_iterator(this);
    while (1) {
	ret = iterator->dir_iterate(&entry);
	if ((ERR_SUCCESS != ret) || (NULL == entry) || (NULL == entry->obj)) {
	    Free(iterator);
	    return(ret);
	}

	task = tm_task::castdown(entry->obj);
	/* assert? */

	(void) task->tm_get_task_id(&tmp_task_id);
	if (own_task_id == tmp_task_id) {
		*is_in = TRUE;
	}
	else {
	    (void)(task)->tm_event_to_task(event, code, subcode);
	}
    }
    return(ERR_SUCCESS);
}

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
mach_error_t tm_task_group::tm_get_tgrp_id(tm_tgrp_id_t	*tgrp_id)
{
    *tgrp_id = group_id;
    return(ERR_SUCCESS);
}
