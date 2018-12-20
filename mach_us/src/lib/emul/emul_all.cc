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
 *
 * Purpose:
 *	Common routines for user space emulation of unix system calls
 *
 * 
 * HISTORY: 
 * $Log:	emul_all.cc,v $
 * Revision 2.9  94/10/27  12:01:22  jms
 * 	Add manipulation of the "shared_info" addr.
 * 	[94/10/26  14:39:01  jms]
 * 
 * Revision 2.8  94/07/08  16:56:36  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.7  94/01/11  17:48:34  jms
 * 	Remove "ustty_proxy" special translation. Now default.
 * 	[94/01/09  18:47:26  jms]
 * 
 * Revision 2.6  93/01/20  17:36:37  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  15:56:44  jms]
 * 
 * Revision 2.5  92/07/05  23:24:23  dpj
 * 	Use new us_tm_{root,task,tgrp}_proxy_ifc.h interfaces for the C++ taskmaster.
 * 	[92/06/24  13:59:53  jms]
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	Removed call to _init_task_id(). This is now done lazily.
 * 	Added include of usint_mf_proxy_ifc.
 * 	[92/06/24  15:54:40  dpj]
 * 
 * 	Eliminated diag_format().
 * 	Converted for new C++ RPC package.
 * 	[92/05/10  00:26:56  dpj]
 * 
 * 	Removed include of strings.h (not needed).
 * 	[92/04/17  16:02:57  dpj]
 * 
 * Revision 2.4  92/03/05  14:55:32  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:08:47  jms]
 * 
 * Revision 2.3  91/12/20  17:43:10  jms
 * 	Add support for emul_lib local /dev/null. Modify prefix table to handle
 * 	files instead of just dirs and preload /dev/null.  (from DPJ)
 * 	[91/12/20  14:24:01  jms]
 * 
 * Revision 2.2  91/11/06  11:29:14  jms
 * 	Initial C++ revision.
 * 	[91/09/26  19:24:15  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:20:41  pjg]
 * 
 * Revision 2.14  91/10/06  22:26:33  jjc
 * 	[91/10/01  17:10:35  jjc]
 * 
 *	Changed emul_init_task_master() to only print out 
 *	tm_register_initial_task() info if debugging is on.
 *	Modified emul_init_namespace() to get prefix names and server objects
 *	from the Configuration Server and use them to set up the prefix table.
 *	Changed NS_ROOT_PORT to CONFIG_PORT.
 * 	[91/04/25            jjc]
 * 
 * Revision 2.13  91/07/01  14:06:22  jms
 * 	NoChange
 * 	x
 * 	[91/06/24  15:45:00  jms]
 * 
 * 	Update args to "mach_object_start_handler".
 * 	[91/05/07  11:20:20  jms]
 * 
 * Revision 2.12  91/05/05  19:24:06  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:49:46  dpj]
 * 
 * 	Use only one signal thread, not two.
 * 	Follow transparent symlinks.
 * 	[91/04/28  09:35:46  dpj]
 * 
 * Revision 2.11  90/11/27  18:17:34  jms
 * 	Merge some stuff from the trunk
 * 	[90/11/19  22:34:17  jms]
 * 
 * 	Prepare to merge some changes from US31
 * 	[90/11/12  16:34:33  jms]
 * 
 * 	About to add tty changes from trunk
 * 	[90/11/08  13:38:25  jms]
 * 
 * 	Merge in mbj/jjc changes
 * 	[90/08/20  17:15:23  jms]
 * 
 * 	Mach3 switch changes for the pure kernel version
 * 	[90/08/16  17:20:57  jms]
 * 
 * Revision 2.10  90/11/10  00:37:51  dpj
 * 	Ux_tty_obj variable is now initialized
 * 	to a uxio_tty object, making it possible
 * 	to detect invalid tty operations.
 * 	[90/10/17  12:38:12  neves]
 * 
 * Revision 2.9  90/10/02  11:35:37  mbj
 * 	Dropped unused TTYServer conditionals.
 * 	[90/09/12  13:38:21  mbj]
 * 
 * Revision 2.8  90/09/07  13:43:03  mbj
 * 	Do a real ns_resolve_obj and ux_open to open the initial tty.
 * 	Dropped support for hokey netname "/dev/tty" lookup and kio ttys.
 * 	Return meaningful error codes from emul_init_io().
 * 	[90/09/07  11:13:16  mbj]
 * 
 * Revision 2.7  90/08/14  22:03:02  mbj
 * 	Move tty pgrp initialization to emul_init_more_io() because of order
 * 	dependency upon the Task_Master being initialized.
 * 	[90/08/14  14:41:38  mbj]
 * 
 * Revision 2.6  90/08/13  15:44:04  jjc
 * 	Added call to emul_init_task_master() to initialize 
 * 	interval timer.
 * 	[90/07/23            jjc]
 * 
 * Revision 2.5  90/07/09  17:01:44  dorr
 * 	fix dereference problem when adding prefix object to clone list.
 * 	[90/03/01  14:33:42  dorr]
 * 
 * 	get reid of emul_find_alert().  get rid of alerts.
 * 	[90/02/23  14:39:24  dorr]
 * 
 * 	Add initialization for signal stuff.
 * 	[90/01/11  11:28:55  dorr]
 * 	Add the signal_object and setup to listen for signals.  Also register more
 * 	objects with the clone_master.
 * 	[90/07/06  14:22:56  jms]
 * 
 * Revision 2.4  90/03/21  17:18:38  jms
 * 	Mods for useing the objectified Task Master and TM->emul signal support.
 * 	[90/03/16  16:14:06  jms]
 * 
 * Revision 2.3  90/03/14  17:28:01  orr
 * 	get rid of start_notification_listener.
 * 	[90/03/14  16:51:18  orr]
 * 
 * Revision 2.2  90/01/02  21:33:24  dorr
 * 	switch to uxident object to keep track of our
 * 	identity (tokens represnting our uid/gid combos).
 * 	move remaining uxprot functionality into uxstat
 * 	object or use fs_access objects for converstions.
 * 	fix tty initialization.
 * 
 * Revision 2.1.1.3  90/01/02  14:05:10  dorr
 * 	use uxident and uxstat objects instead of uxprot.
 * 	give uid mapping object (fs_access) to uxident and
 * 	uxstat as part of initialization.  get token from
 * 	our identity for initializing ttys.
 * 
 * Revision 2.1.1.2  89/12/19  17:04:29  dorr
 * 	checkin before christmas
 * 
 * Revision 2.1.1.1  89/12/18  15:40:19  dorr
 * 	initial revision.
 * 
 */
#include <clone_ifc.h>
#include <rpcmgr_ifc.h>
#include <emul_all_ifc.h>
#include <us_name_ifc.h>
#include <us_tm_root_proxy_ifc.h>
#include <uxsignal_ifc.h>
#include <us_sys_proxy_ifc.h>
#include <devnull_ifc.h>
#include "emul_base.h"
#include "emul_proc.h"

extern "C" {
#include <us_error.h>
#include "syscall_table.h"
#include <us_ports.h>
#include <sys/types.h>
#include <sys/file.h>		/* For O_RDWR */
#include <sys/ioctl.h>
}

extern int exec_debug_level;
extern void timer_init(void);
#if SHARED_DATA_TIMING_EQUIVALENCE
extern vm_address_t vm_allocate_base_address;
#endif SHARED_DATA_TIMING_EQUIVALENCE

char	* tm_serv = "Task_Master";

mach_error_t emul_init_task_master(
	ns_token_t	token)
{
	mach_error_t	err;
	char		servername[1024];
	char		* hp;
	char		* tm;
	mach_port_t	port;

	struct syscall_val	rv;
	tm_task_id_t		task_id = NULL_TASK_ID;
#if SHARED_DATA_TIMING_EQUIVALENCE
	vm_address_t	shared_addr;	/* the address of the shared space */
#endif SHARED_DATA_TIMING_EQUIVALENCE

	/* change host:name into two separate args */
	
	strcpy( tm = servername, tm_serv );
	if ( hp = index( servername, ':' ) ) {
		tm = hp;
		*tm++ = '\0';
		hp = servername;
	} else
		hp = "";

	
	/*
	 *  set up our global task master connection
	 */
	
	err = netname_look_up(name_server_port,hp,tm,&port);
	if (err) {
		ERROR((Diag,
		  "Can't establish initial Task Master connection %x (%s)\n",
				  err, mach_error_string(err)));
		drop_dead();
	}
	tm_obj = new usTMRoot_proxy;
	tm_obj->set_object_port(port);

	/*
	 * Initialize interval timer
	 */
	timer_init();

	/*
	 * Setup to receive signals on the uxsignal_object
	 */
	rpcmgr::GLOBAL->start_object_handler(0,1,1);

	/*
	 * Enter ourself into the set of managed tasks.
	 */

	DEBUG0(1,(Diag,"tm_register_initial_task(%x, %d, %d -> ",
		tm_obj, mach_task_self(), task_id));

	usItem	*tm_tgrp_item = NULL;
	usItem	*tm_task_item = NULL;
#if SHARED_DATA_TIMING_EQUIVALENCE
	shared_addr = vm_allocate_base_address;
#endif SHARED_DATA_TIMING_EQUIVALENCE
	err = tm_obj->tm_register_initial_task(mach_task_self(), uxsignal_obj,
		token, 
#if SHARED_DATA_TIMING_EQUIVALENCE
		&shared_addr,
#endif SHARED_DATA_TIMING_EQUIVALENCE
		TM_SELF_ACCESS, TM_SELF_ACCESS,	
		&tm_tgrp_item, &task_id, &tm_task_item);
#if SHARED_DATA_TIMING_EQUIVALENCE
	shared_info = (tm_shared_info_t)shared_addr;
#endif SHARED_DATA_TIMING_EQUIVALENCE

	if (exec_debug_level > 98) {
		task_suspend(mach_task_self());
	}

	mach_object_dereference(tm_tgrp_item);

	if (err) {
		ERROR((Diag,"tm_register_initial_task: %#x %s\n", err, 
			mach_error_string(err)));
		drop_dead();
	}
	DEBUG2(emul_debug,(0,"emul_init_task_master: initial task registered\n"));

	tm_task_obj = usTMTask::castdown(tm_task_item);

        tm_parent_obj = tm_task_obj;
	mach_object_reference(tm_parent_obj);

	/*
         * Cloning stuff
	 */
	clone_master_obj->list_add(usClone::castdown(tm_obj));
	clone_master_obj->list_add(usClone::castdown(tm_task_obj));

	return ERR_SUCCESS;
}


mach_error_t
emul_init_namespace(ns_identity_t as_ident, ns_token_t as_token)
{
	mach_error_t		err;

	/* get a uid mapping object */
	access_obj = new fs_access;

	/* standard authorization object */
	std_auth* auth_obj = new std_auth;

	/* start up our prefix table object */
	prefix_obj = new std_name;

	/*
	 * make an initial identity for ourself 
	 */
	std_ident* id = new std_ident(as_ident, as_token);

	/*
	 * get a unix identity
	 */
	uxident_obj = new uxident(access_obj, auth_obj, id);
	mach_object_dereference(id);

	/* sync up the i/o system */
	(void)emul_io_change_identity(uxident_obj);


	/*
	 * create the initial (unauthenticated) sys agent
	 */
	usSys* sys_agent = new usSys_proxy;
	sys_agent->set_object_port(CONFIG_PORT);

	usItem			*server_objects[MAX_PREFIX];
	ns_name_t		*prefix_names;
	int			server_count;
	int			prefix_count;

	err = sys_agent->sys_get_prefix_table(server_objects, &server_count,
					&prefix_names, &prefix_count);
	if (err != ERR_SUCCESS) {
		ERROR((Diag, "sys_get_prefix_table: %s\n",
				  mach_error_string(err)));
	}

	/*
	 * enter the prefix names and server agents into the prefix table
	 * (authenticating it)
	 */
	DEBUG1(TRUE,(Diag,"emul_init_namespace: prefix count %d\n",
			prefix_count));
	register i;
	for (i = 0; i < prefix_count; i++) {
		DEBUG1(TRUE,(Diag,"emul_init_namespace: prefix '%s' server 0x%x\n",
				prefix_names[i], server_objects[i]));
		err = prefix_obj->ns_set_system_prefix(prefix_names[i],
					   server_objects[i],NST_DIRECTORY,
					   prefix_names[i]);
		if (err != ERR_SUCCESS) {
			ERROR((Diag, "ns_set_prefix: couldn't set prefix %s: %x %s\n",
					  prefix_names[i],
					  err,
					  mach_error_string(err)));
		}
	}

	/*
	 * Setup an initial CWD.
	 */
	err = prefix_obj->ns_set_user_prefix("","/");
	if (err != ERR_SUCCESS) {
		ERROR((Diag, "ns_set_prefix: couldn't set CWD prefix %x %s\n",
				  err,
				  mach_error_string(err)));
	}

	/*
	 * Setup /dev/null.
	 */
	usItem *devnull_obj = new devnull;
	err = prefix_obj->ns_set_system_prefix("/dev/null",
					devnull_obj,NST_FILE,"/dev/null");
	if (err != ERR_SUCCESS) {
		ERROR((Diag,
		"ns_set_prefix: couldn't set /dev/null prefix: %x %s\n",
					  err,mach_error_string(err)));
	}

	/* set up unix-style protections */
	uxstat_obj = new uxstat(access_obj);

	clone_master_obj->list_add(uxident_obj);
	clone_master_obj->list_add(prefix_obj);
	clone_master_obj->list_add(uxstat_obj);

	return err;
}


/*
 *	emul_io_init:  called the very first time the emulation library
 *	is run to initialize i/o data structures 
 */

#if	KernelFileIO

#if notdef
#include <ftab_methods.h>
#include <uxio_methods.h>

#if MACH3_UNIX || MACH3_VUS
#include <kio_methods.h>
#endif MACH3_UNIX || MACH3_VUS

#include <cat_methods.h>			
#include <io_methods.h>			
#endif notdef

#include <uxio_tty_ifc.h>
#endif	KernelFileIO

static uxio_tty* ux_tty_obj;
extern char emul_init_tty_name[40];

mach_error_t
emul_init_io(void)
{
	register int		fd;
	int			new_fd;
	mach_error_t		err, xx;
	usItem*		cat_obj = 0;
	ns_type_t		type;
	struct sgttyb sgttyb;

	DEBUG2(emul_debug,(0,"emul_init_io called\n"));

	/*
	 * statically load a few popular classes
	 */

	DEBUG2(emul_debug,(0,"opening tty %s\n", emul_init_tty_name));

	err = prefix_obj->ns_resolve_fully(emul_init_tty_name,NSF_FOLLOW_ALL,
					NSR_READ | NSR_WRITE | NSR_GETATTR,
					&cat_obj, &type, NULL);
	if (err != ERR_SUCCESS) goto finish;

	/* make a new object that understands unix ops */
	ux_tty_obj = new uxio_tty;

	if ((usByteIO::castdown(cat_obj)) == 0) {
		DEBUG0(emul_debug,(0,"emul_init_io\n"));
		err =  MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}

	err = ux_tty_obj->ux_open(cat_obj, O_RDWR, 
				   NSR_READ | NSR_WRITE | NSR_GETATTR);
	if (err) goto finish;

	xx =  ux_tty_obj->ux_ioctl(TIOCGETP, (int*)&sgttyb);
	DEBUG2(emul_debug,(0,"emul_init_io: ux_ioctl(0x%0x) -> 0x%0x, %s\n", TIOCGETP, xx, mach_error_string(xx)));
	sgttyb.sg_flags &= ~RAW;
	sgttyb.sg_flags |= (ECHO|CRMOD);
	xx = ux_tty_obj->ux_ioctl(TIOCSETP, (int*)&sgttyb);
	DEBUG2(emul_debug,(0,"emul_init_io: ux_ioctl(0x%0x) -> 0x%0x, %s\n", TIOCGETP, xx, mach_error_string(xx)));

	/* hoke up 0-2 */

	for( fd=0; fd<3; fd++ ) {
		(void)ftab_obj->ftab_add_obj(ux_tty_obj, &new_fd);
	}

finish:
	/* free resources allocated during open */
	mach_object_dereference(cat_obj);	/* "done" with the cat */

	if (err == ERR_SUCCESS) {
		DEBUG0(1,(Diag,"emul_init_io succeeded\n"));
	} else {
		ERROR((Diag,"emul_init_io: Error opening tty %s %s(%x)\n",
			emul_init_tty_name, mach_error_string(err), err));
	}

	return err;
}

mach_error_t
emul_init_more_io(void)
{
	tm_task_id_t	tid;
	int		pid;

	DEBUG0(1,(Diag,"emul_init_more_io called\n"));

	if (ux_tty_obj == NULL) return ERR_SUCCESS;

	/*
	 * Start with terminal pgrp == pid to make csh
	 * and other job-control applications happy.
	 */

	tm_task_obj->tm_get_task_id(&tid);
	pid = task_id_to_pid(tid);
	(void) ux_tty_obj->ux_ioctl(TIOCSPGRP, &pid);

	mach_object_dereference(ux_tty_obj);
	return ERR_SUCCESS;
}

#if	Signals

extern "C" { void* mach_external_object; }

emul_init_signals(void)
{
	DEBUG0(1,(Diag,"emul_init_signals\n"));
	
	/*
	 * create the uxsignal object.
	 */
	uxsignal_obj = new uxsignal;
	/*
	 * hook 'em up
	 */
	/*
	 * XXX C++ Connect the C++ external_object wrapper
	 */
//	external_obj = new external_object(mach_external_object);
//	clone_master_obj->list_add(external_obj);
	clone_master_obj->list_add(uxsignal_obj);
}
#endif	Signals

#include <us_name_proxy_ifc.h>
#include <us_net_name_proxy_ifc.h>
#include <mf_user_ifc.h>
#include <tm_task_proxy_ifc.h>
#include <tm_tgrp_proxy_ifc.h>

#include <us_byteio_proxy_ifc.h>
#include <us_recio_proxy_ifc.h>
#include <us_net_name_proxy_ifc.h>
#include <us_net_connector_proxy_ifc.h>

#include <usint_mf_proxy_ifc.h>

#include <us_net_clts_recs_proxy_ifc.h>
#include <us_net_cots_recs_proxy_ifc.h>
#include <us_net_cots_bytes_proxy_ifc.h>

boolean_t _insert_class(char*, void*);
void*     _lookup_class(char*);

void _init_user_proxies(void)
{
	INSERT_CLASS_IN_MAP(usName_proxy, "++dir_proxy");
	INSERT_CLASS_IN_MAP(usName_proxy, "cat");
	INSERT_CLASS_IN_MAP(usNetName_proxy, "usNetName_proxy");
	INSERT_CLASS_IN_MAP(tm_task_proxy, "tm_task_proxy");
	INSERT_CLASS_IN_MAP(tm_tgrp_proxy, "tm_tgrp_proxy");
	INSERT_CLASS_IN_MAP(mf_user, "++mf_user_proxy");
	INSERT_CLASS_IN_MAP(mf_user, "mf_cat");
	INSERT_CLASS_IN_MAP(usByteIO_proxy, "usByteIO_proxy");
}

extern "C" {
void my_task_suspend(void);
}

void my_task_suspend(void)
{
//	task_suspend(mach_task_self());
}


