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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_init.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: setup of the pipenet server.
 *
 * HISTORY
 * $Log:	pipenet_init.cc,v $
 * Revision 2.4  94/07/13  17:21:41  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:35:10  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:31:32  dpj]
 * 
 * Revision 2.2  91/11/06  14:21:55  jms
 * 	Upgraded the calls that initialize the Diag system.
 * 	[91/10/03  15:17:53  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/09/27  15:55:14  pjg]
 * 
 * Revision 2.3  91/07/01  14:15:04  jms
 * 	Changed diag initialization so that we get a real name even with
 * 	lazy initialization.
 * 	[91/06/21  18:11:05  dpj]
 * 	Use new args to "mach_object_start_handler"
 * 	[91/06/25  11:43:30  jms]
 * 
 * Revision 2.2  91/05/05  19:33:13  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:39  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:00:23  dpj]
 * 
 */

#include	<rpcmgr_ifc.h>
#include	<pipenet_root_ifc.h>
#include	<diag_ifc.h>


extern "C" {
extern void exit();
}


/*
 * Global ID.
 */
ns_mgr_id_t			pipenet_mgr_id = { 0, 0x01800001};

#define USAGE	"[-diag <diag-name>] [-d<debug-level>] [<base-name>]"

pipenet_usage(char *program, char *args)
{
	fprintf(stderr,"Usage: %s %s\n",program,args);
	exit(1);
}


void      _print_map(void);

/*
 * Global PIPENET object.
 */
pipenet_root		*root;


main(int argc, char *argv[])
{
	int			new_debug_level = us_debug_level;
	char			*diag_name = NULL;
	char			*root_name;
	char			*program = argv[0];
	mach_error_t		ret;

	argc--;
	argv++;
	for (; argc > 0; argc--, argv++) {
		if (!strcmp(argv[0],"-diag")) {
			argc--;
			argv++;
			if (argc <= 0) pipenet_usage(program,USAGE);
			diag_name = argv[0];
			continue;
		} 

		if (!strncmp(argv[0],"-d",2)) {
			if (sscanf(argv[0]+2,"%d",&new_debug_level) != 1) {
				pipenet_usage(program,USAGE);
			}
			continue;
		} 

		break;
	}
	if ((argc > 1) || ((argc == 1) && (argv[0][1] == '-'))) {
		pipenet_usage(program,USAGE);
	}
	if (argc == 1)  {
		root_name = argv[0];
	} else {
		root_name = "pipenet_server";
	}

	cthread_init();
	intr_init();
	rpcmgr::GLOBAL->start_object_handler(2,30,100);

	(void) diag_startup(root_name);
	(void) set_diag_level(new_debug_level);

//	_print_map();

	/*
	 * Create the root directory.
	 */
//	new_object(root,pipenet_root);
//	ret = setup_pipenet_root(root,pipenet_mgr_id);
	root = new pipenet_root(pipenet_mgr_id, &ret);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot setup the root directory",ret);
		exit(1);
	}

	/*
	 * Export the root directory.
	 */
	ret = root->ns_netname_export(root_name);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot export the initial agent",ret);
		exit(1);
	}
}

extern "C" {
void my_task_suspend(void);
mach_error_t abort(void);
}

void my_task_suspend(void)
{
//	task_suspend(mach_task_self());
}

mach_error_t abort(void)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}
