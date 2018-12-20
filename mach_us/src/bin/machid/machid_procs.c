/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	machid_procs.c,v $
 * Revision 2.2  94/07/13  16:37:44  mrt
 * 	copied here from the user area
 * 
 * Revision 2.12  93/04/14  11:43:16  mrt
 * 	Added sparc changes from dlc.
 * 	[93/01/08            berman]
 * 
 * Revision 2.11  93/04/09  13:45:40  mrt
 * 	64bit cleanup.
 * 	[92/12/02            af]
 * 
 * Revision 2.10  92/01/25  13:28:11  rpd
 * 	Added casts to task_info, thread_info, thread_get_state calls.
 * 	[92/01/25            rpd]
 * 
 * Revision 2.9  92/01/22  22:51:51  rpd
 * 	Added <servers/machid_lib.h>.
 * 	[92/01/22            rpd]
 * 
 * Revision 2.8  92/01/17  14:23:30  rpd
 * 	Changed vm_region_info_t to vm_region_t.
 * 	[92/01/02            rpd]
 * 
 * 	Moved default_pager_info to machid_dpager_procs.c.
 * 	[91/12/16            rpd]
 * 
 * Revision 2.7  91/08/30  14:52:56  rpd
 * 	Moved machid include files into the standard include directory.
 * 	[91/08/29            rpd]
 * 
 * 	Added vm_statistics.
 * 	[91/08/19            rpd]
 * 	Added host_default_pager, default_pager_info.
 * 	[91/08/15            rpd]
 * 
 * Revision 2.6  91/03/27  17:26:48  mrt
 * 	Changed mach.h include
 * 
 * Revision 2.5  91/03/19  12:30:54  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.4  90/10/08  13:15:40  rpd
 * 	Added thread_policy, processor_set_policy_enable,
 * 	and processor_set_policy_disable.
 * 	[90/10/07            rpd]
 * 
 * Revision 2.3  90/09/23  14:30:48  rpd
 * 	Fixed do_host_threads to use MACH_TYPE_THREAD.
 * 	[90/09/18            rpd]
 * 
 * Revision 2.2  90/09/12  16:31:53  rpd
 * 	Added processor_set_create, task_create, thread_create,
 * 	processor_assign, thread_assign, thread_assign_default,
 * 	task_assign, task_assign_default.
 * 	[90/08/31            rpd]
 * 
 * 	Created.
 * 	[90/06/18            rpd]
 * 
 */

#include <stdio.h>
#include <mach.h>
#include <mach_error.h>
#include <servers/machid_types.h>
#include <servers/machid_lib.h>
#include "machid_internal.h"

kern_return_t
do_mach_type(server, auth, id, typep)
    mach_port_t server;
    mach_port_t auth;
    mach_id_t id;
    mach_type_t *typep;
{
    *typep = type_lookup(id);
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_mach_register(server, auth, port, type, idp)
    mach_port_t server;
    mach_port_t auth;
    mach_port_t port;
    mach_type_t type;
    mach_id_t *idp;
{
    *idp = name_lookup(port, type);
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_mach_lookup(server, auth, name, atype, anamep)
    mach_port_t server;
    mach_port_t auth;
    mach_id_t name;
    mach_type_t atype;
    mach_id_t *anamep;
{
    *anamep = assoc_lookup(name, atype);
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_mach_port(server, auth, name, portp)
    mach_port_t server;
    mach_port_t auth;
    mach_id_t name;
    mach_port_t *portp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(name, auth, mo_Port, &port);
    if (kr != KERN_SUCCESS)
	return kr;

    *portp = port;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_ports(server, auth, hostp, phostp)
    mach_port_t server;
    mach_port_t auth;
    mhost_t *hostp;
    mhost_priv_t *phostp;
{
    mhost_t host;
    mhost_priv_t phost;

    host = name_lookup(mach_host_self(), MACH_TYPE_HOST);
    phost = name_lookup(mach_host_priv_self(), MACH_TYPE_HOST_PRIV);

    assoc_create(host, MACH_TYPE_HOST_PRIV, phost);
    assoc_create(phost, MACH_TYPE_HOST, host);
    assoc_create(phost, MACH_TYPE_HOST_PRIV, phost);

    *hostp = host;
    *phostp = phost;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_processor_sets(server, auth, host, mprocsetsp, mprocsetsCntp)
    mach_port_t server;
    mach_port_t auth;
    mhost_priv_t host;
    mprocessor_set_array_t *mprocsetsp;
    natural_t *mprocsetsCntp;
{
    processor_set_name_t *procsets;
    natural_t procsetsCnt;
    unsigned int i;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST_PRIV;
	return kr;
    }

    /* the ports always come back out-of-line */

    kr = host_processor_sets(port, &procsets, &procsetsCnt);
    if (kr != KERN_SUCCESS) {
	port_consume(port);
	return kr;
    }

    /* convert from unprivileged to privileged proc set ports */

    for (i = 0; i < procsetsCnt; i++) {
	mprocessor_set_name_t mprocsetname;
	mprocessor_set_t mprocset;
	processor_set_t procset;

	kr = host_processor_set_priv(port, procsets[i], &procset);
	if (kr != KERN_SUCCESS)
	    procset = MACH_PORT_DEAD;

	mprocsetname = name_lookup(procsets[i], MACH_TYPE_PROCESSOR_SET_NAME);
	mprocset = name_lookup(procset, MACH_TYPE_PROCESSOR_SET);

	assoc_create(mprocsetname, MACH_TYPE_HOST_PRIV, host);
	assoc_create(mprocset, MACH_TYPE_HOST_PRIV, host);
	assoc_create(mprocsetname, MACH_TYPE_PROCESSOR_SET, mprocset);
	assoc_create(mprocset, MACH_TYPE_PROCESSOR_SET_NAME, mprocsetname);

	((mprocessor_set_t *) procsets)[i] = mprocset;
    }
    port_consume(port);

    /* return data in-line if possible */

    if (procsetsCnt < *mprocsetsCntp) {
	if (procsetsCnt != 0) {
	    bcopy((char *) procsets, (char *) *mprocsetsp,
		  procsetsCnt * sizeof *procsets);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) procsets,
			       (vm_size_t) (procsetsCnt * sizeof *procsets));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*mprocsetsp = (mprocessor_set_t *) procsets;
    }
    *mprocsetsCntp = procsetsCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_processor_set_names(server, auth, host, mprocsetsp, mprocsetsCntp)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    mprocessor_set_name_array_t *mprocsetsp;
    natural_t *mprocsetsCntp;
{
    processor_set_name_t *procsets;
    natural_t procsetsCnt, i;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    /* the ports always come back out-of-line */

    kr = host_processor_sets(port, &procsets, &procsetsCnt);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    for (i = 0; i < procsetsCnt; i++)
	((mprocessor_set_name_t *) procsets)[i] =
		name_lookup(procsets[i], MACH_TYPE_PROCESSOR_SET_NAME);

    /* return data in-line if possible */

    if (procsetsCnt < *mprocsetsCntp) {
	if (procsetsCnt != 0) {
	    bcopy((char *) procsets, (char *) *mprocsetsp,
		  procsetsCnt * sizeof *procsets);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) procsets,
			       (vm_size_t) (procsetsCnt * sizeof *procsets));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*mprocsetsp = (mprocessor_set_t *) procsets;
    }
    *mprocsetsCntp = procsetsCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_tasks(server, auth, host, mtasksp, mtasksCntp)
    mach_port_t server;
    mach_port_t auth;
    mhost_priv_t host;
    mtask_array_t *mtasksp;
    natural_t *mtasksCntp;
{
    processor_set_name_t *procsets;
    natural_t procsetsCnt;
    task_t **tasks_list;
    natural_t *tasksCnts;
    natural_t total_tasks, i, j, k;
    mtask_t *mtasks;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST_PRIV;
	return kr;
    }

    /* get a list of the processor sets, out-of-line */

    kr = host_processor_sets(port, &procsets, &procsetsCnt);
    if (kr != KERN_SUCCESS) {
	port_consume(port);
	return kr;
    }

    /* allocate space for lists of the tasks in each processor set */

    tasks_list = (task_t **) malloc(procsetsCnt * sizeof *tasks_list);
    if (tasks_list == NULL)
	quit(1, "machid: malloc failed\n");

    tasksCnts = (natural_t *) malloc(procsetsCnt * sizeof *tasksCnts);
    if (tasksCnts == NULL)
	quit(1, "machid: malloc failed\n");

    for (total_tasks = i = 0; i < procsetsCnt; i++) {
	processor_set_t procset;
	mprocessor_set_t mprocset;
	mprocessor_set_name_t mprocsetname;

	kr = host_processor_set_priv(port, procsets[i], &procset);
	if (kr != KERN_SUCCESS)
	    procset = MACH_PORT_DEAD;

	if (!MACH_PORT_VALID(procset) ||
	    (processor_set_tasks(procset, &tasks_list[i],
				 &tasksCnts[i]) != KERN_SUCCESS)) {
	    tasks_list[i] = NULL;
	    tasksCnts[i] = 0;
	}

	mprocset = name_lookup(procset, MACH_TYPE_PROCESSOR_SET);
	mprocsetname = name_lookup(procsets[i], MACH_TYPE_PROCESSOR_SET_NAME);

	assoc_create(mprocset, MACH_TYPE_HOST_PRIV, host);
	assoc_create(mprocsetname, MACH_TYPE_HOST_PRIV, host);
	assoc_create(mprocset, MACH_TYPE_PROCESSOR_SET_NAME, mprocsetname);
	assoc_create(mprocsetname, MACH_TYPE_PROCESSOR_SET, mprocset);

	total_tasks += tasksCnts[i];
    }
    port_consume(port);

    /* we don't need the processor sets anymore */

    if (procsetsCnt != 0) {
	kr = vm_deallocate(mach_task_self(), (vm_offset_t) procsets,
			   (vm_size_t) (procsetsCnt * sizeof *procsets));
	if (kr != KERN_SUCCESS)
	    quit(1, "machid: vm_deallocate: %s\n", mach_error_string(kr));
    }

    /* get space for the combined task list */

    if (total_tasks < *mtasksCntp) {
	/* use in-line space */

	mtasks = *mtasksp;
    } else {
	kr = vm_allocate(mach_task_self(), (vm_offset_t *) &mtasks,
			 (vm_size_t) (total_tasks * sizeof *mtasks), TRUE);
	if (kr != KERN_SUCCESS)
	    quit(1, "machid: vm_allocate: %s\n", mach_error_string(kr));
    }

    /* lookup task ports */

    for (k = i = 0; i < procsetsCnt; i++) {
	for (j = 0; j < tasksCnts[i]; j++)
	    assoc_create(mtasks[k++] = name_lookup(tasks_list[i][j],
						   MACH_TYPE_TASK),
			 MACH_TYPE_HOST_PRIV, host);

	if (tasksCnts[i] != 0) {
	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) tasks_list[i],
			       (vm_size_t) (tasksCnts[i] *
					    sizeof *tasks_list[i]));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n", mach_error_string(kr));
	}
    }
    free((char *) tasks_list);
    free((char *) tasksCnts);

    *mtasksp = mtasks;
    *mtasksCntp = total_tasks;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_threads(server, auth, host, mthreadsp, mthreadsCntp)
    mach_port_t server;
    mach_port_t auth;
    mhost_priv_t host;
    mthread_array_t *mthreadsp;
    natural_t *mthreadsCntp;
{
    processor_set_name_t *procsets;
    natural_t procsetsCnt;
    thread_t **threads_list;
    natural_t *threadsCnts;
    natural_t total_threads, i, j, k;
    mthread_t *mthreads;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST_PRIV;
	return kr;
    }

    /* get a list of the processor sets, out-of-line */

    kr = host_processor_sets(port, &procsets, &procsetsCnt);
    if (kr != KERN_SUCCESS) {
	port_consume(port);
	return kr;
    }

    /* allocate space for lists of the threads in each processor set */

    threads_list = (thread_t **) malloc(procsetsCnt * sizeof *threads_list);
    if (threads_list == NULL)
	quit(1, "machid: malloc failed\n");

    threadsCnts = (natural_t *) malloc(procsetsCnt * sizeof *threadsCnts);
    if (threadsCnts == NULL)
	quit(1, "machid: malloc failed\n");

    for (total_threads = i = 0; i < procsetsCnt; i++) {
	processor_set_t procset;
	mprocessor_set_t mprocset;
	mprocessor_set_name_t mprocsetname;

	kr = host_processor_set_priv(port, procsets[i], &procset);
	if (kr != KERN_SUCCESS)
	    procset = MACH_PORT_DEAD;

	if (!MACH_PORT_VALID(procset) ||
	    (processor_set_threads(procset, &threads_list[i],
				   &threadsCnts[i]) != KERN_SUCCESS)) {
	    threads_list[i] = NULL;
	    threadsCnts[i] = 0;
	}

	mprocset = name_lookup(procset, MACH_TYPE_PROCESSOR_SET);
	mprocsetname = name_lookup(procsets[i], MACH_TYPE_PROCESSOR_SET_NAME);

	assoc_create(mprocset, MACH_TYPE_HOST_PRIV, host);
	assoc_create(mprocsetname, MACH_TYPE_HOST_PRIV, host);
	assoc_create(mprocset, MACH_TYPE_PROCESSOR_SET_NAME, mprocsetname);
	assoc_create(mprocsetname, MACH_TYPE_PROCESSOR_SET, mprocset);

	total_threads += threadsCnts[i];
    }
    port_consume(port);

    /* we don't need the processor sets anymore */

    if (procsetsCnt != 0) {
	kr = vm_deallocate(mach_task_self(), (vm_offset_t) procsets,
			   (vm_size_t) (procsetsCnt * sizeof *procsets));
	if (kr != KERN_SUCCESS)
	    quit(1, "machid: vm_deallocate: %s\n", mach_error_string(kr));
    }

    /* get space for the combined thread list */

    if (total_threads < *mthreadsCntp) {
	/* use in-line space */

	mthreads = *mthreadsp;
    } else {
	kr = vm_allocate(mach_task_self(), (vm_offset_t *) &mthreads,
			 (vm_size_t) (total_threads * sizeof *mthreads), TRUE);
	if (kr != KERN_SUCCESS)
	    quit(1, "machid: vm_allocate: %s\n", mach_error_string(kr));
    }

    /* lookup thread ports */

    for (k = i = 0; i < procsetsCnt; i++) {
	for (j = 0; j < threadsCnts[i]; j++)
	    assoc_create(mthreads[k++] = name_lookup(threads_list[i][j],
						   MACH_TYPE_THREAD),
			 MACH_TYPE_HOST_PRIV, host);

	if (threadsCnts[i] != 0) {
	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) threads_list[i],
			       (vm_size_t) (threadsCnts[i] *
					    sizeof *threads_list[i]));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n", mach_error_string(kr));
	}
    }
    free((char *) threads_list);
    free((char *) threadsCnts);

    *mthreadsp = mthreads;
    *mthreadsCntp = total_threads;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_processors(server, auth, host, mprocsp, mprocsCntp)
    mach_port_t server;
    mach_port_t auth;
    mhost_priv_t host;
    mprocessor_array_t *mprocsp;
    natural_t *mprocsCntp;
{
    processor_t *procs;
    natural_t procsCnt, i;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST_PRIV;
	return kr;
    }

    /* get a list of the processors, out-of-line */

    kr = host_processors(port, &procs, &procsCnt);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    for (i = 0; i < procsCnt; i++)
	assoc_create(procs[i] = name_lookup(procs[i], MACH_TYPE_PROCESSOR),
		     MACH_TYPE_HOST_PRIV, host);

    /* return data in-line if possible */

    if (procsCnt < *mprocsCntp) {
	if (procsCnt != 0) {
	    bcopy((char *) procs, (char *) *mprocsp,
		  procsCnt * sizeof *procs);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) procs,
			       (vm_size_t) (procsCnt * sizeof *procs));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*mprocsp = (mprocessor_t *) procs;
    }
    *mprocsCntp = procsCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_threads(server, auth, task, mthreadsp, mthreadsCntp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mthread_array_t *mthreadsp;
    natural_t *mthreadsCntp;
{
    mhost_priv_t host;
    thread_t *threads;
    natural_t threadsCnt, i;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    /* get a list of the threads, out-of-line */

    kr = task_threads(port, &threads, &threadsCnt);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* propogate host from task to threads */
    host = assoc_lookup(task, MACH_TYPE_HOST_PRIV);

    for (i = 0; i < threadsCnt; i++) {
	mthread_t thread = name_lookup(threads[i], MACH_TYPE_THREAD);

	((mthread_t *) threads)[i] = thread;
	assoc_create(thread, MACH_TYPE_TASK, task);
	assoc_create(thread, MACH_TYPE_HOST_PRIV, host);
    }

    /* return data in-line if possible */

    if (threadsCnt < *mthreadsCntp) {
	if (threadsCnt != 0) {
	    bcopy((char *) threads, (char *) *mthreadsp,
		  threadsCnt * sizeof *threads);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) threads,
			       (vm_size_t) (threadsCnt * sizeof *threads));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*mthreadsp = (mthread_t *) threads;
    }
    *mthreadsCntp = threadsCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_threads(server, auth, pset, mthreadsp, mthreadsCntp)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_t pset;
    mthread_array_t *mthreadsp;
    natural_t *mthreadsCntp;
{
    mhost_priv_t host;
    thread_t *threads;
    natural_t threadsCnt, i;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    /* get a list of the threads, out-of-line */

    kr = processor_set_threads(port, &threads, &threadsCnt);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* propogate host from pset to threads */
    host = assoc_lookup(pset, MACH_TYPE_HOST_PRIV);

    for (i = 0; i < threadsCnt; i++) {
	mthread_t thread = name_lookup(threads[i], MACH_TYPE_THREAD);

	((mthread_t *) threads)[i] = thread;
	assoc_create(thread, MACH_TYPE_PROCESSOR_SET, pset);
	assoc_create(thread, MACH_TYPE_HOST_PRIV, host);
    }

    /* return data in-line if possible */

    if (threadsCnt < *mthreadsCntp) {
	if (threadsCnt != 0) {
	    bcopy((char *) threads, (char *) *mthreadsp,
		  threadsCnt * sizeof *threads);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) threads,
			       (vm_size_t) (threadsCnt * sizeof *threads));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*mthreadsp = (mthread_t *) threads;
    }
    *mthreadsCntp = threadsCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_tasks(server, auth, pset, mtasksp, mtasksCntp)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_t pset;
    mtask_array_t *mtasksp;
    natural_t *mtasksCntp;
{
    mhost_priv_t host;
    task_t *tasks;
    natural_t tasksCnt, i;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    /* get a list of the tasks, out-of-line */

    kr = processor_set_tasks(port, &tasks, &tasksCnt);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* propogate host from pset to tasks */
    host = assoc_lookup(pset, MACH_TYPE_HOST_PRIV);

    for (i = 0; i < tasksCnt; i++) {
	mtask_t task = name_lookup(tasks[i], MACH_TYPE_TASK);

	((mtask_t *) tasks)[i] = task;
	assoc_create(task, MACH_TYPE_PROCESSOR_SET, pset);
	assoc_create(task, MACH_TYPE_HOST_PRIV, host);
    }

    /* return data in-line if possible */

    if (tasksCnt < *mtasksCntp) {
	if (tasksCnt != 0) {
	    bcopy((char *) tasks, (char *) *mtasksp,
		  tasksCnt * sizeof *tasks);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) tasks,
			       (vm_size_t) (tasksCnt * sizeof *tasks));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*mtasksp = (mtask_t *) tasks;
    }
    *mtasksCntp = tasksCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_basic_info(server, auth, host, infop)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    host_basic_info_data_t *infop;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = host_info(port, HOST_BASIC_INFO, infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_sched_info(server, auth, host, infop)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    host_sched_info_data_t *infop;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = host_info(port, HOST_SCHED_INFO, infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_load_info(server, auth, host, infop)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    host_load_info_data_t *infop;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = host_info(port, HOST_LOAD_INFO, infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_default(server, auth, host, psetp)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    mprocessor_set_name_t *psetp;
{
    processor_set_name_t pset;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    kr = processor_set_default(port, &pset);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    *psetp = name_lookup(pset, MACH_TYPE_PROCESSOR_SET_NAME);
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_kernel_version(server, auth, host, version)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    kernel_version_t version;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    kr = host_kernel_version(port, version);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_basic_info(server, auth, proc, hostp, infop)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_t proc;
    mhost_t *hostp;
    processor_basic_info_data_t *infop;
{
    host_t host;
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(proc, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = processor_info(port, PROCESSOR_BASIC_INFO, &host, infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(proc, MACH_TYPE_HOST,
		 *hostp = name_lookup(host, MACH_TYPE_HOST));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_basic_info(server, auth, pset, hostp, infop)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_name_t pset;
    mhost_t *hostp;
    processor_set_basic_info_data_t *infop;
{
    host_t host;
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET_NAME;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = processor_set_info(port, PROCESSOR_SET_BASIC_INFO,
			    &host, infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(pset, MACH_TYPE_HOST,
		 *hostp = name_lookup(host, MACH_TYPE_HOST));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_sched_info(server, auth, pset, hostp, infop)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_name_t pset;
    mhost_t *hostp;
    processor_set_sched_info_data_t *infop;
{
    host_t host;
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET_NAME;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = processor_set_info(port, PROCESSOR_SET_SCHED_INFO,
			    &host, infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(pset, MACH_TYPE_HOST,
		 *hostp = name_lookup(host, MACH_TYPE_HOST));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_unix_info(server, auth, task, pidp, comm, commCntp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    unix_pid_t *pidp;
    unix_command_t comm;
    natural_t *commCntp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    if (pid_by_task(port, pidp, comm, commCntp) != 0) {
	*pidp = 0;
	*commCntp = 0;
    }

    port_consume(port);
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_basic_info(server, auth, task, infop)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    task_basic_info_data_t *infop;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = task_info(port, TASK_BASIC_INFO, (task_info_t) infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_thread_times_info(server, auth, task, timesp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    task_thread_times_info_data_t *timesp;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    count = sizeof *timesp / sizeof(natural_t);
    kr = task_info(port, TASK_THREAD_TIMES_INFO, (task_info_t) timesp, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_basic_info(server, auth, thread, infop)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    thread_basic_info_data_t *infop;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = thread_info(port, THREAD_BASIC_INFO, (thread_info_t) infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_sched_info(server, auth, thread, infop)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    thread_sched_info_data_t *infop;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *infop / sizeof(natural_t);
    kr = thread_info(port, THREAD_SCHED_INFO, (thread_info_t) infop, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

#ifdef	mips
kern_return_t
do_mips_thread_state(server, auth, thread, statep)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    mips_thread_state_t *statep;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Read, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *statep / sizeof(natural_t);
    kr = thread_get_state(port, MIPS_THREAD_STATE,
			  (thread_state_t) statep, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
#endif

#ifdef	sun3
kern_return_t
do_sun3_thread_state(server, auth, thread, statep)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    sun3_thread_state_t *statep;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Read, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *statep / sizeof(natural_t);
    kr = thread_get_state(port, SUN_THREAD_STATE_REGS,
			  (thread_state_t) statep, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
#endif

#ifdef	sun4
kern_return_t
do_sparc_thread_state(server, auth, thread, statep)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    sparc_thread_state_t *statep;
{
    mach_port_t port;
    unsigned int count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Read, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *statep / sizeof(int);
    kr = thread_get_state(port, SPARC_THREAD_STATE_REGS,
			  (thread_state_t) statep, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
#endif	sun4

#ifdef	vax
kern_return_t
do_vax_thread_state(server, auth, thread, statep)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    vax_thread_state_t *statep;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Read, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *statep / sizeof(natural_t);
    kr = thread_get_state(port, VAX_THREAD_STATE,
			  (thread_state_t) statep, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
#endif

#ifdef	i386
kern_return_t
do_i386_thread_state(server, auth, thread, statep)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    i386_thread_state_t *statep;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Read, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *statep / sizeof(natural_t);
    kr = thread_get_state(port, i386_THREAD_STATE,
			  (thread_state_t) statep, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
#endif

#ifdef	alpha
kern_return_t
do_alpha_thread_state(server, auth, thread, statep)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    alpha_thread_state_t *statep;
{
    mach_port_t port;
    natural_t count;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Read, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    count = sizeof *statep / sizeof(natural_t);
    kr = thread_get_state(port, ALPHA_THREAD_STATE,
			  (thread_state_t) statep, &count);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}
#endif

kern_return_t
do_task_terminate(server, auth, task)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = task_terminate(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_suspend(server, auth, task)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = task_suspend(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_resume(server, auth, task)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = task_resume(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_terminate(server, auth, thread)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_terminate(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_suspend(server, auth, thread)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_suspend(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_resume(server, auth, thread)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_resume(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_abort(server, auth, thread)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_abort(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_destroy(server, auth, pset)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_t pset;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = processor_set_destroy(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_start(server, auth, processor)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_t processor;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(processor, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR;
	return kr;
    }

    kr = processor_start(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_exit(server, auth, processor)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_t processor;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(processor, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR;
	return kr;
    }

    kr = processor_exit(port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_vm_region(server, auth, task, addr, infop)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    vm_offset_t addr;
    vm_region_t *infop;
{
    vm_region_t info;
    memory_object_name_t name;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = vm_region(port, &addr, &info.vr_size,
		   &info.vr_prot, &info.vr_max_prot, &info.vr_inherit,
		   &info.vr_shared, &name, &info.vr_offset);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    info.vr_address = addr;
    assoc_create(info.vr_name = name_lookup(name, MACH_TYPE_OBJECT_NAME),
		 MACH_TYPE_HOST_PRIV,
		 assoc_lookup(task, MACH_TYPE_HOST_PRIV));

    *infop = info;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_vm_read(server, auth, task, addr, size, datap, dataCntp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    vm_offset_t addr;
    vm_size_t size;
    pointer_t *datap;
    natural_t *dataCntp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Read, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = vm_read(port, addr, size, datap, dataCntp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_priority(server, auth, thread, priority, set_max)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    int priority;
    boolean_t set_max;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_priority(port, priority, set_max);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_max_priority(server, auth, thread, pset, max_pri)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    mprocessor_set_t pset;
    int max_pri;
{
    mach_port_t tport, psport;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &tport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = port_lookup(pset, auth, mo_Write, &psport);
    if (kr != KERN_SUCCESS) {
	port_consume(tport);
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = thread_max_priority(tport, psport, max_pri);
    port_consume(tport);
    port_consume(psport);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_priority(server, auth, task, priority, change_threads)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    int priority;
    boolean_t change_threads;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = task_priority(port, priority, change_threads);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_max_priority(server, auth, pset, max_pri, change_threads)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_t pset;
    int max_pri;
    boolean_t change_threads;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = processor_set_max_priority(port, max_pri, change_threads);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_names(server, auth, task, namesp, namesCntp, typesp, typesCntp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_array_t *namesp;
    natural_t *namesCntp;
    mach_port_type_array_t *typesp;
    natural_t *typesCntp;
{
    mach_port_t *names;
    mach_port_type_t *types;
    natural_t namesCnt, typesCnt;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    /* get the data, out-of-line */

    kr = mach_port_names(port, &names, &namesCnt, &types, &typesCnt);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* return data in-line if possible */

    if (namesCnt < *namesCntp) {
	if (namesCnt != 0) {
	    bcopy((char *) names, (char *) *namesp,
		  namesCnt * sizeof *names);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) names,
			       (vm_size_t) (namesCnt * sizeof *names));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*namesp = names;
    }
    *namesCntp = namesCnt;

    if (typesCnt < *typesCntp) {
	if (typesCnt != 0) {
	    bcopy((char *) types, (char *) *typesp,
		  typesCnt * sizeof *types);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) types,
			       (vm_size_t) (typesCnt * sizeof *types));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*typesp = types;
    }
    *typesCntp = typesCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_type(server, auth, task, name, typep)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_t name;
    mach_port_type_t *typep;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_port_type(port, name, typep);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_get_refs(server, auth, task, name, right, refsp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_t name;
    mach_port_right_t right;
    mach_port_urefs_t *refsp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_port_get_refs(port, name, right, refsp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_get_receive_status(server, auth, task, name, statusp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_t name;
    mach_port_status_t *statusp;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = mach_port_get_receive_status(port, name, statusp);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_port_get_set_status(server, auth, task, name, membersp, membersCntp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mach_port_t name;
    mach_port_array_t *membersp;
    natural_t *membersCntp;
{
    mach_port_t *members;
    natural_t membersCnt;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    /* get a list of the member ports, out-of-line */

    kr = mach_port_get_set_status(port, name, &members, &membersCnt);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* return data in-line if possible */

    if (membersCnt < *membersCntp) {
	if (membersCnt != 0) {
	    bcopy((char *) members, (char *) *membersp,
		  membersCnt * sizeof *members);

	    kr = vm_deallocate(mach_task_self(), (vm_offset_t) members,
			       (vm_size_t) (membersCnt * sizeof *members));
	    if (kr != KERN_SUCCESS)
		quit(1, "machid: vm_deallocate: %s\n",
		     mach_error_string(kr));
	}
    } else {
	*membersp = members;
    }
    *membersCntp = membersCnt;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_get_assignment(server, auth, proc, psetp)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_t proc;
    mprocessor_set_name_t *psetp;
{
    processor_set_name_t pset;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(proc, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR;
	return kr;
    }

    kr = processor_get_assignment(port, &pset);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(proc, MACH_TYPE_PROCESSOR_SET_NAME,
		 *psetp = name_lookup(pset, MACH_TYPE_PROCESSOR_SET_NAME));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_get_assignment(server, auth, thread, psetp)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    mprocessor_set_name_t *psetp;
{
    processor_set_name_t pset;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_get_assignment(port, &pset);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(thread, MACH_TYPE_PROCESSOR_SET_NAME,
		 *psetp = name_lookup(pset, MACH_TYPE_PROCESSOR_SET_NAME));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_get_assignment(server, auth, task, psetp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mprocessor_set_name_t *psetp;
{
    processor_set_name_t pset;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = task_get_assignment(port, &pset);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(task, MACH_TYPE_PROCESSOR_SET_NAME,
		 *psetp = name_lookup(pset, MACH_TYPE_PROCESSOR_SET_NAME));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_processor_set_priv(server, auth, host, psetn, psetp)
    mach_port_t server;
    mach_port_t auth;
    mhost_priv_t host;
    mprocessor_set_name_t psetn;
    mprocessor_set_t *psetp;
{
    mprocessor_set_t name;
    processor_set_t pset;
    mach_port_t hport, psport;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &hport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST_PRIV;
	return kr;
    }

    kr = port_lookup(psetn, auth, mo_Info, &psport);
    if (kr != KERN_SUCCESS) {
	port_consume(hport);
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET_NAME;
	return kr;
    }

    kr = host_processor_set_priv(hport, psport, &pset);
    port_consume(hport);
    port_consume(psport);
    if (kr != KERN_SUCCESS)
	return kr;

    name = name_lookup(pset, MACH_TYPE_PROCESSOR_SET);
    assoc_create(psetn, MACH_TYPE_HOST_PRIV, host);
    assoc_create(name, MACH_TYPE_HOST_PRIV, host);
    assoc_create(psetn, MACH_TYPE_PROCESSOR_SET, name);
    assoc_create(name, MACH_TYPE_PROCESSOR_SET_NAME, psetn);

    *psetp = name;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_create(server, auth, host, psetp, psetnp)
    mach_port_t server;
    mach_port_t auth;
    mhost_t host;
    mprocessor_set_t *psetp;
    mprocessor_set_name_t *psetnp;
{
    processor_set_t pset;
    processor_set_name_t psetn;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST;
	return kr;
    }

    kr = processor_set_create(port, &pset, &psetn);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(*psetp = name_lookup(pset, MACH_TYPE_PROCESSOR_SET),
		 MACH_TYPE_HOST_PRIV,
		 assoc_lookup(host, MACH_TYPE_HOST_PRIV));
    assoc_create(*psetnp = name_lookup(psetn, MACH_TYPE_PROCESSOR_SET_NAME),
		 MACH_TYPE_HOST_PRIV,
		 assoc_lookup(host, MACH_TYPE_HOST_PRIV));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_create(server, auth, parent, inherit, taskp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t parent;
    boolean_t inherit;
    mtask_t *taskp;
{
    task_t task;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(parent, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = task_create(port, inherit, &task);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(*taskp = name_lookup(task, MACH_TYPE_TASK),
		 MACH_TYPE_HOST_PRIV,
		 assoc_lookup(parent, MACH_TYPE_HOST_PRIV));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_create(server, auth, task, threadp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mthread_t *threadp;
{
    thread_t thread;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = thread_create(port, &thread);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    assoc_create(*threadp = name_lookup(thread, MACH_TYPE_THREAD),
		 MACH_TYPE_HOST_PRIV,
		 assoc_lookup(task, MACH_TYPE_HOST_PRIV));

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_assign(server, auth, processor, pset, wait)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_t processor;
    mprocessor_set_t pset;
    boolean_t wait;
{
    mach_port_t pport, psport;
    kern_return_t kr;

    kr = port_lookup(processor, auth, mo_Write, &pport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR;
	return kr;
    }

    kr = port_lookup(pset, auth, mo_Write, &psport);
    if (kr != KERN_SUCCESS) {
	port_consume(pport);
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = processor_assign(pport, psport, wait);
    port_consume(pport);
    port_consume(psport);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_assign(server, auth, thread, pset)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    mprocessor_set_t pset;
{
    mach_port_t thport, psport;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &thport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = port_lookup(pset, auth, mo_Write, &psport);
    if (kr != KERN_SUCCESS) {
	port_consume(thport);
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = thread_assign(thport, psport);
    port_consume(thport);
    port_consume(psport);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_assign_default(server, auth, thread)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
{
    mach_port_t thport;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &thport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_assign_default(thport);
    port_consume(thport);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_assign(server, auth, task, pset, assign_threads)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mprocessor_set_t pset;
    boolean_t assign_threads;
{
    mach_port_t tport, psport;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Write, &tport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = port_lookup(pset, auth, mo_Write, &psport);
    if (kr != KERN_SUCCESS) {
	port_consume(tport);
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = task_assign(tport, psport, assign_threads);
    port_consume(tport);
    port_consume(psport);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_assign_default(server, auth, task, assign_threads)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    boolean_t assign_threads;
{
    mach_port_t tport;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Write, &tport);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = task_assign_default(tport, assign_threads);
    port_consume(tport);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_policy(server, auth, thread, policy, data)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    int policy;
    int data;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    kr = thread_policy(port, policy, data);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_policy_enable(server, auth, pset, policy)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_t pset;
    int policy;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = processor_set_policy_enable(port, policy);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_set_policy_disable(server, auth, pset, policy, change_threads)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_set_t pset;
    int policy;
    boolean_t change_threads;
{
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(pset, auth, mo_Write, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR_SET;
	return kr;
    }

    kr = processor_set_policy_disable(port, policy, change_threads);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_default_pager(server, auth, host, default_pagerp)
    mach_port_t server;
    mach_port_t auth;
    mhost_priv_t host;
    mdefault_pager_t *default_pagerp;
{
    mach_port_t default_pager_port;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST_PRIV;
	return kr;
    }

    default_pager_port = MACH_PORT_NULL;
    kr = vm_set_default_memory_manager(port, &default_pager_port);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    *default_pagerp = name_lookup(default_pager_port, MACH_TYPE_DEFAULT_PAGER);
    assoc_create(*default_pagerp, MACH_TYPE_HOST_PRIV, host);
    assoc_create(host, MACH_TYPE_DEFAULT_PAGER, *default_pagerp);
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_vm_statistics(server, auth, task, datap)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    vm_statistics_data_t *datap;
{
    vm_statistics_data_t data;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    kr = vm_statistics(port, &data);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    *datap = data;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_host_kernel_task(server, auth, host, kernel_taskp)
    mach_port_t server;
    mach_port_t auth;
    mhost_priv_t host;
    mtask_t *kernel_taskp;
{
    mtask_t kernel_task;
    task_t tasks_buf[1024];
    task_t *tasks = tasks_buf;
    natural_t count = sizeof tasks_buf/sizeof tasks_buf[0];
    processor_set_t pset;
    processor_set_name_t psetname;
    mprocessor_set_t mpset;
    mprocessor_set_name_t mpsetname;
    mach_port_t port;
    natural_t i;
    kern_return_t kr;

    kr = port_lookup(host, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_HOST_PRIV;
	return kr;
    }

    /* get the default processor set name port */

    kr = processor_set_default(port, &psetname);
    if (kr != KERN_SUCCESS) {
	port_consume(port);
	return kr;
    }

    /* convert to privileged processor set port */

    kr = host_processor_set_priv(port, psetname, &pset);
    port_consume(port);
    mpsetname = name_lookup(psetname, MACH_TYPE_PROCESSOR_SET_NAME);
    assoc_create(mpsetname, MACH_TYPE_HOST_PRIV, host);
    if (kr != KERN_SUCCESS)
	return kr;

    /* get tasks in the processor set */

    kr = processor_set_tasks(pset, &tasks, &count);
    mpset = name_lookup(pset, MACH_TYPE_PROCESSOR_SET);
    assoc_create(mpset, MACH_TYPE_HOST_PRIV, host);
    assoc_create(mpsetname, MACH_TYPE_PROCESSOR_SET, mpset);
    assoc_create(mpset, MACH_TYPE_PROCESSOR_SET_NAME, mpsetname);
    if (kr != KERN_SUCCESS)
	return kr;

    /* process the tasks */

    kernel_task = 0;
    for (i = 0; i < count; i++) {
	mtask_t mtask = name_lookup(tasks[i], MACH_TYPE_TASK);
	assoc_create(mtask, MACH_TYPE_HOST_PRIV, host);

	/* take the first task, if it exists, as the kernel task */

	if (i == 0)
	    kernel_task = mtask;
    }

    if ((tasks != tasks_buf) && (count != 0)) {
	kr = vm_deallocate(mach_task_self(), (vm_offset_t) tasks,
			   (vm_size_t) (count * sizeof *tasks));
	if (kr != KERN_SUCCESS)
	    quit(1, "machid: vm_deallocate: %s\n",
		 mach_error_string(kr));
    }

    *kernel_taskp = kernel_task;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_task_host(server, auth, task, hostp)
    mach_port_t server;
    mach_port_t auth;
    mtask_t task;
    mhost_t *hostp;
{
    processor_set_basic_info_data_t info;
    natural_t count;
    processor_set_name_t psetname;
    mprocessor_set_name_t mpsetname;
    host_t host;
    mhost_t mhost;
    mhost_priv_t mhostpriv;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(task, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_TASK;
	return kr;
    }

    /* convert from task to processor set name port */

    kr = task_get_assignment(port, &psetname);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* convert from processor set to host port */

    count = sizeof info/sizeof(natural_t);
    kr = processor_set_info(psetname, PROCESSOR_SET_BASIC_INFO,
			    &host, &info, &count);
    mpsetname = name_lookup(psetname, MACH_TYPE_PROCESSOR_SET_NAME);
    if (kr != KERN_SUCCESS)
	return kr;

    mhost = name_lookup(host, MACH_TYPE_HOST);
    mhostpriv = assoc_lookup(mhost, MACH_TYPE_HOST_PRIV);
    assoc_create(task, MACH_TYPE_HOST_PRIV, mhostpriv);

    *hostp = mhost;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_thread_host(server, auth, thread, hostp)
    mach_port_t server;
    mach_port_t auth;
    mthread_t thread;
    mhost_t *hostp;
{
    processor_set_basic_info_data_t info;
    natural_t count;
    processor_set_name_t psetname;
    mprocessor_set_name_t mpsetname;
    host_t host;
    mhost_t mhost;
    mhost_priv_t mhostpriv;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(thread, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_THREAD;
	return kr;
    }

    /* convert from thread to processor set name port */

    kr = thread_get_assignment(port, &psetname);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* convert from processor set to host port */

    count = sizeof info/sizeof(natural_t);
    kr = processor_set_info(psetname, PROCESSOR_SET_BASIC_INFO,
			    &host, &info, &count);
    mpsetname = name_lookup(psetname, MACH_TYPE_PROCESSOR_SET_NAME);
    if (kr != KERN_SUCCESS)
	return kr;

    mhost = name_lookup(host, MACH_TYPE_HOST);
    mhostpriv = assoc_lookup(mhost, MACH_TYPE_HOST_PRIV);
    assoc_create(thread, MACH_TYPE_HOST_PRIV, mhostpriv);

    *hostp = mhost;
    auth_consume(auth);
    return KERN_SUCCESS;
}

kern_return_t
do_processor_host(server, auth, processor, hostp)
    mach_port_t server;
    mach_port_t auth;
    mprocessor_t processor;
    mhost_t *hostp;
{
    processor_set_basic_info_data_t info;
    natural_t count;
    processor_set_name_t psetname;
    mprocessor_set_name_t mpsetname;
    host_t host;
    mhost_t mhost;
    mhost_priv_t mhostpriv;
    mach_port_t port;
    kern_return_t kr;

    kr = port_lookup(processor, auth, mo_Info, &port);
    if (kr != KERN_SUCCESS) {
	if (kr == KERN_INVALID_NAME)
	    kr = KERN_INVALID_PROCESSOR;
	return kr;
    }

    /* convert from processor to processor set name port */

    kr = processor_get_assignment(port, &psetname);
    port_consume(port);
    if (kr != KERN_SUCCESS)
	return kr;

    /* convert from processor set to host port */

    count = sizeof info/sizeof(natural_t);
    kr = processor_set_info(psetname, PROCESSOR_SET_BASIC_INFO,
			    &host, &info, &count);
    mpsetname = name_lookup(psetname, MACH_TYPE_PROCESSOR_SET_NAME);
    if (kr != KERN_SUCCESS)
	return kr;

    mhost = name_lookup(host, MACH_TYPE_HOST);
    mhostpriv = assoc_lookup(mhost, MACH_TYPE_HOST_PRIV);
    assoc_create(processor, MACH_TYPE_HOST_PRIV, mhostpriv);

    *hostp = mhost;
    auth_consume(auth);
    return KERN_SUCCESS;
}
