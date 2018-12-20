/* 
 **********************************************************************
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
 **********************************************************************
 */

/*
 * HISTORY:
 * $Log:	tty_signal.cc,v $
 * Revision 2.4  94/07/21  16:15:10  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:36:38  dpj
 * 	Use new us_tm_{root,task,tgrp}_ifc.h interfaces for the C++ taskmaster.
 * 	Use new us_tm_{root,task,tgrp}_proxy_ifc.h interfaces for the C++ taskmaster.
 * 	[92/06/24  18:21:55  jms]
 * 	Eliminated diag_format().
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:34:25  dpj]
 * 
 * Revision 2.2  91/11/06  14:24:28  jms
 * 	Update to use C++ language and US object structure.
 * 	[91/09/17  14:25:29  jms]
 * 
 * 	Use Task_Master signal messages.
 * 	[89/03/06            mbj]
 * 
 * Revision 2.9  91/10/07  00:12:57  jjc
 * 		Changed diag_format calls to DEBUG0 and ERROR statements.
 * 	[91/04/25            jjc]
 * 
 * Revision 2.8  91/07/01  14:15:47  jms
 * 	Update to new args for tm_event_to_tgrp
 * 	[91/06/25  13:31:10  jms]
 * 
 * Revision 2.7  90/10/02  11:38:48  mbj
 * 	Added psignal() routine.
 * 	[90/10/01  15:38:51  mbj]
 * 
 * Revision 2.6  90/08/14  22:03:37  mbj
 * 	Converted to use new Task_Master interface.
 * 	[90/08/14  14:50:59  mbj]
 * 
 * Revision 2.5  90/07/09  17:12:44  dorr
 * 	No Further Changes
 * 	[90/07/09  11:24:38  jms]
 * 
 * Revision 2.4  89/05/18  10:34:53  dorr
 * 	include file cataclysm
 * 
 * Revision 2.3  89/05/04  17:59:28  mbj
 * 	Added support for signal messages.
 * 	[89/04/26  00:12:36  mbj]
 * 
 * Revision 2.2  89/03/21  14:44:51  mbj
 * 	Merge mbj_pgrp branch onto mainline.
 * 
 */
extern "C" {
#include <mach_error.h>
#include <sig_error.h>

/* #include <ns_types.h> */
#include <tm_types.h>
#include <debug.h>
#include <sys/proc.h>
}

#include <us_tm_root_ifc.h>
#include <us_tm_task_ifc.h>
#include <us_tm_tgrp_ifc.h>
#include <us_event_ifc.h>
#include <us_tm_root_proxy_ifc.h>
#include <tm_task_proxy_ifc.h>
#include <tm_tgrp_proxy_ifc.h>

#define pgrp_to_job_group(pgrp) (pgrp)	/* XXX currently in emul_proc.h */
#define pid_to_task_id(pid) (pid)	/* XXX currently in emul_proc.h */

usTMRoot	*tm_obj;	    /* Public connection to Task_Master */

boolean_t _insert_class(char*, void*);
void*     _lookup_class(char*);

/* make it all "C" callable */
extern "C" {

void init_signals()
/*
 * Initialize tty_server signal processing code.
 */
{
    mach_error_t err;
    register int i;
    mach_port_t port;

    for (i = 0; ; i++) {
	/*
	 *  set up our global task master connection
	 */
	
	err = netname_look_up(name_server_port, "", "Task_Master",
				&port);

	if (err == ERR_SUCCESS) {
	    INFO((Diag,"Task_Master found at port %d\n", port));

	    tm_obj = new usTMRoot_proxy;
	    tm_obj->set_object_port(port);
	    break;
	}

	if (i < 6) {
	    INFO((Diag,"Waiting %d seconds for Task_Master\n",
		1 << i));
	    sleep(1<<i);
	} else {
	    INFO((Diag,"Running without Task_Master: %s (%#x)\n", 
		   mach_error_string(err), err));
	    tm_obj = NULL;
	    break;	/* Give up */
	}

    }

    INSERT_CLASS_IN_MAP(tm_task_proxy, "tm_task_proxy");
    INSERT_CLASS_IN_MAP(tm_tgrp_proxy, "tm_tgrp_proxy");
}

void gsignal(int pgrp, int sig)
/*
 * Send a signal to a process group.
 */
{
    mach_error_t 	err;

    tm_tgrp_id_t	req_jgrp_id;
    usTMTgrp		*tm_tgrp_obj;
    usItem		*tm_tgrp_item;
    int			dummy_post_id;

    if (tm_obj == NULL) return;	/* No Task_Master! */

    /* job_group_id was given */
    req_jgrp_id = pgrp_to_job_group(pgrp);
    err = tm_obj->tm_find_tgrp(req_jgrp_id, TM_DEFAULT_ACCESS, &tm_tgrp_item);
    if (err) {
	return;
    }
    tm_tgrp_obj = usTMTgrp::castdown(tm_tgrp_item);

    err = tm_tgrp_obj->tm_event_to_tgrp(unix_sig_err(sig), 0, 0, 
				NULL_TASK_ID, &dummy_post_id);

    if (err) {
	mach_object_dereference(tm_tgrp_obj);
	return;
    }    

    mach_object_dereference(tm_tgrp_obj);
    return;
}

void psignal(struct proc *p, int sig)
{
    mach_error_t	err;
    usTMTask		*tm_task_obj;
    usItem		*tm_task_item;

    if (tm_obj == NULL) return;	/* No Task_Master! */

    err = tm_obj->tm_task_id_to_task(pid_to_task_id(p->p_pid), TM_DEFAULT_ACCESS, &tm_task_item);
    if (err) {
	return;
    }
    tm_task_obj = usTMTask::castdown(tm_task_item);

    err = tm_task_obj->tm_event_to_task(unix_sig_err(sig), 0, 0);
    if (err) {
	mach_object_dereference(tm_task_obj);
	return;
    }

    mach_object_dereference(tm_task_obj);
    return;
}

} /* extern "C" */
