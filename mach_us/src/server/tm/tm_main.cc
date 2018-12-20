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
 * tm_main.c
 *
 * $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tm/tm_main.cc,v $
 *
 */
/*
 * Mach tm Client.
 *
 * Startup routine.
 *
 */

/*
 * HISTORY: 
 * $Log:	tm_main.cc,v $
 * Revision 2.3  94/07/13  17:33:15  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  92/07/05  23:35:34  dpj
 * 	Converted for new C++ RPC package.
 * 	[92/07/05  19:00:35  dpj]
 * 
 * 	Don't register twice with nameserver.
 * 	[92/06/25  11:24:04  jms]
 * 
 * 	Translated from tm_main.c
 * 	[92/06/24  17:30:02  jms]
 * 
 * Revision 1.25  91/07/01  14:15:15  jms
 * 	Added setup for mgr_methods.
 * 	[91/06/21  18:11:16  dpj]
 * 	Use new args to "mach_object_start_handler"
 * 	[91/06/25  11:45:58  jms]
 * 
 * Revision 1.24  90/11/27  18:21:55  jms
 * 	Pull death watch code (move it to tm_agency.c.
 * 	Add -e switch.  Iff given, kernel exceptions are not managed by the task_master
 * 	and leak out of the Multi-server system.  Required for gdb breakpoints!
 * 	[90/11/20  14:57:09  jms]
 * 
 * 	take out testbed for pure kernel use.
 * 	[90/08/20  17:41:41  jms]
 * 
 * Revision 1.23  90/07/09  17:12:02  dorr
 * 	Formatting change
 * 	[90/07/09  11:18:05  jms]
 * 
 * Revision 1.22  90/03/21  17:29:16  jms
 * 	Misc debugging code added
 * 	[90/03/16  17:18:39  jms]
 * 
 * 	first objectified checkin
 * 	[89/12/19  16:23:42  jms]
 * 
 */

#include	<diag_ifc.h>
#include	<tm_root_ifc.h>
#include	<rpcmgr_ifc.h>

int		tm_debug = 1;	/* Debugging global switch x*/
boolean_t	forward_kernel_exceptions = TRUE;
extern		void init_proxies(void);

/*
 * Misc tm root object stuff.
 */
ns_mgr_id_t	tm_mgr_id = { 0, 0x06000001};
tm_root		*tm_obj;

#define USAGE()	\
	fprintf(2,"Usage: %s [-d<debug-level>] [-e] [<server-name>]\n",argv[0])

main(int argc, char *argv[])
{
	int			new_debug_level = us_debug_level;
	mach_error_t		ret;
	char			*tm_name;
	cthread_t		new_thread;


	argv++;
	argc--;
	while ((argc > 0) && ((*argv)[0] == '-')) {
		switch((*argv)[1]) {
			case 'd':
				new_debug_level = atoi((*argv) + 2);
				break;

			case 'e':
				forward_kernel_exceptions = FALSE;
				break;

			default:
				USAGE();
				exit(1);
			}
		argv++;
		argc--;
	}
	if (argc > 0) {
		tm_name = argv[0];
		argv++;
		argc--;
	} else {
		tm_name = "Task_Master";
	}

	if (argc > 1) {
		USAGE();
		exit(1);
	}

	/*
	 * General Setup
	 */
	cthread_init();
	intr_init();
	rpcmgr::GLOBAL->start_object_handler(1,2,0);

	(void) diag_startup(tm_name);
	(void) set_diag_level(new_debug_level);
	(void) init_proxies();
	/*
	 * Create the initial access object
	 */
	tm_obj = new tm_root(tm_mgr_id, &ret);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot setup the root directory",ret);
		exit(1);
	}

	/*
	 * Export it to the nameserver for general use.  We are open for
	 * business.
	 */
	ret = tm_obj->ns_netname_export(tm_name);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot export the initial agent",ret);
		exit(1);
	}
}

#include	<us_event_proxy_ifc.h>

boolean_t _insert_class(char*, void*);
void*     _lookup_class(char*);

void init_proxies(void)
{
	INSERT_CLASS_IN_MAP(usEvent_proxy, "usEvent_proxy");
}
