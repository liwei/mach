/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * name_init.c
 *
 * $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/name/name_init.cc,v $
 *
 */
/*
 * 
 * Purpose:  "Main" module for the name server.
 * 
 * HISTORY: 
 * $Log:	name_init.cc,v $
 * Revision 2.6  94/07/13  17:10:05  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/06/16  17:27:58  mrt
 * 	Changed default name from root_server to pathname_server.
 * 	[94/06/02            mrt]
 * 
 * Revision 2.4  94/01/11  18:07:05  jms
 * 	Fix server directory protections.
 * 	[94/01/09  20:01:39  jms]
 * 
 * Revision 2.3  92/07/05  23:34:21  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:28:49  dpj]
 * 
 * Revision 2.2  91/11/06  14:19:49  jms
 * 	Upgraded the calls that initialize the Diag system.
 * 	[91/10/03  15:17:04  pjg]
 * 
 * 	Upgraded to US41.
 * 	[91/09/27  16:01:14  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:48:51  pjg]
 * 
 * Revision 1.14  91/07/01  14:14:57  jms
 * 	No Further Changes
 * 	[91/06/25  11:40:31  jms]
 * 
 * 	Update args to "mach_object_start_handler".
 * 	Add "-stop" command line arg for debugging
 * 	[91/05/07  11:36:54  jms]
 * 
 * Revision 1.13  91/05/05  19:31:26  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:06:19  dpj]
 * 
 * 	Use more than one MachObjects handler thread.
 * 	[91/04/28  10:52:50  dpj]
 * 
 * Revision 1.12  90/08/22  18:14:36  roy
 * 	No change.
 * 	[90/08/17  13:15:02  roy]
 * 
 * 	No change.
 * 	[90/08/15  16:01:56  roy]
 * 
 * 	Added -console option.
 * 	[90/08/14  12:40:20  roy]
 * 
 * Revision 1.11  89/10/30  16:38:29  dpj
 * 	Cleaned-up initialization. Most of the work is done by the
 * 	agency at the root of the tree.
 * 	[89/10/27  19:39:39  dpj]
 * 
 * Revision 1.10  89/07/09  14:20:57  dpj
 * 	Added "-d<debug-level>" switch.
 * 	[89/07/08  15:45:01  dpj]
 * 
 * Revision 1.9  89/06/30  18:38:39  dpj
 * 	Changed to use the US utility routines, and to explicitly
 * 	allocate the mgr ID.
 * 	Removed the "-i" flag.
 * 	Introduced a "-d" flag for debugging.
 * 	Added a call to diag_startup().
 * 	[89/06/29  00:55:01  dpj]
 * 
 * Revision 1.8  89/03/17  13:01:42  sanzi
 * 	add initialization stuff
 * 	[89/02/28  00:09:12  dorr]
 * 	
 * 	rework to use mach_objects.  all of the code
 * 	disappears.  it's magic.
 * 	[89/02/16  16:28:39  dorr]
 * 
 */

#include <rpcmgr_ifc.h>
#include <dir_ifc.h>
#include <diag_ifc.h>

extern "C" {
#include	<base.h>
#include	<stdio.h>
#include	<ns_types.h>
}

extern ns_authid_t fs_access_default_root_authid;

/*
 * Global ID.
 */
ns_mgr_id_t			name_mgr_id = { 0, 0x01000001};


#define USAGE()	\
	fprintf(stderr,"Usage: %s [-stop] [-d<debug-level>] [-console] [<base-name>]\n",program)

main(int argc, char **argv)
{
	int			new_debug_level = us_debug_level;
	char			*root_name;
	char 			*program;
	mach_error_t		ret;
	dir*			root;
	ns_prot_t		subdir_prot = NULL;
	int			acl_len = 2;

	boolean_t		stop = FALSE;
	void      _init_user_proxies(void);
	void      _print_map(void);

	program = argv[0];
	for (argc--,argv++; (argc > 0) && (argv[0][0] == '-'); argc--,argv++) {
		if (!strncmp(argv[0],"-d",2)) {
			sscanf(argv[0]+2,"%d",&new_debug_level);
			continue;
		} else if (!strncmp(argv[0],"-console",8)) {
			/* all printf output goes to console */
			mach3_output_console();  
			continue;
		} else if (!strncmp(argv[0],"-stop",5)) {
			/* Stop when we are done parsing */
			stop = TRUE;
			continue;
		} else {
			USAGE();
			exit(1);
		}
	}
	if (argc > 1) {
		USAGE();
		exit(1);
	}
	else if (argc == 1) 
		root_name = argv[0];
	else 
		root_name = "pathname_server";


	if (stop) task_suspend(mach_task_self());

	cthread_init();
	intr_init();
	rpcmgr::GLOBAL->start_object_handler(2,4,10);

	(void) diag_startup(root_name);
	(void) set_diag_level(new_debug_level);

	_init_user_proxies();
//	_print_map();

	root = new dir(name_mgr_id, &ret);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot setup the root directory",ret);
		exit(1);
	}

	subdir_prot = (ns_prot_t)malloc(sizeof(struct ns_prot_head) +
				(acl_len * sizeof(struct ns_acl_entry)));

	subdir_prot->head.version = NS_PROT_VERSION;
	subdir_prot->head.generation = 0;
	subdir_prot->head.acl_len = acl_len;

	subdir_prot->acl[0].authid = fs_access_default_root_authid;
	subdir_prot->acl[0].rights = NSR_ADMIN |
		NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;
	subdir_prot->acl[1].authid = NS_AUTHID_WILDCARD;
	subdir_prot->acl[1].rights =
		NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;

	ret = root->ns_set_protection(subdir_prot,
				     NS_PROT_SIZE(subdir_prot) / sizeof(int));
	if (ret != ERR_SUCCESS) {
		us_internal_error("ns_set_protection(root_name_dir)",ret);
	}
	free(subdir_prot);

	ret = root->ns_netname_export(root_name);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot export the initial agent",ret);
		exit(1);
	}
/*
	if (new_debug_level > 0) {
//		mach_object_t		debug_agent = MACH_OBJECT_NULL;

		(void) ns_create_initial_agent(root,&debug_agent);
		test_us_driver(debug_agent);
	}
*/
}


#include <us_name_proxy_ifc.h>

boolean_t _insert_class(char*, void*);
void*     _lookup_class(char*);

void _init_user_proxies(void)
{
	INSERT_CLASS_IN_MAP(usName_proxy, "++dir_proxy");
	INSERT_CLASS_IN_MAP(usName_proxy, "cat");
}

extern "C" {
void my_task_suspend(void);
}

void my_task_suspend(void)
{
//	task_suspend(mach_task_self());
}
