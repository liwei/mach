/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992 Carnegie Mellon University
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
 * HISTORY:
 * $Log:	ufs_main.cc,v $
 * Revision 2.6  94/07/21  17:53:35  mrt
 * 	Changed include of "sys/file.h" to "file.h" to get local
 * 	version instead of the one from unix_include/sys. Updated
 * 	copyright.
 * 	[94/07/21            mrt]
 * 
 * Revision 2.5  94/06/29  14:57:04  mrt
 * 	Added the mount option as a parameter to the new of vn_dir.
 * 	This allows read-only mounted devices to remain read-only!
 * 	[94/06/29  13:59:35  grm]
 * 
 * Revision 2.4  94/05/17  14:11:07  jms
 * 	Cast args to cthread_fork
 * 	[94/04/29  13:52:07  jms]
 * 
 * Revision 2.3  93/01/20  17:40:38  jms
 * 	Use multiple ufs threads.  Take optional count on command line.
 * 	[93/01/18  17:57:46  jms]
 * 
 * Revision 2.2  92/07/05  23:37:05  dpj
 * 	First working version.
 * 	[92/06/24  17:46:51  dpj]
 * 
 */

extern "C" {
#include "base.h"
#include <hash.h>
}

#include <top_ifc.h>
#include <vn_dir_ifc.h>
#include <rpcmgr_ifc.h>

extern "C" {
#include <stdio.h>
#include <ns_types.h>
#include <fs_types.h>

#include "param.h"
#include "user.h"
#include "conf.h"

#include "fs.h"
#include "inode.h"
#include "buf.h"
#include "systm.h"
#include "uio.h"

#include <mach.h>
#include <mach_error.h>
#include "mach_init.h"

#include "file.h"
#include <mach/time_value.h>
#include <logging.h>

extern struct inode * open_file();
extern struct user *ustruct_init();

struct linesw linesw[1];
struct bdevsw bdevsw[1];
struct cdevsw cdevsw[1];

struct user	u;

int	diagnostic_level;
extern int	nbuf;
extern int	ninode;

extern void zone_init();
extern void disk_init();
extern void data_init();
extern void ihinit();
extern void binit();
extern mach_error_t disk_open();
extern mach_error_t mountfs();
extern void panic();
extern void exit();
extern void bcopy();
extern mach_error_t mach_get_time();

extern int time_of_day_thread();

extern void mach3_output_console();
}

ns_mgr_id_t			ufs_mgr_id = { 0, 0x03000001};


#define USAGE()	fprintf(stderr,"Usage: %s [-d<debug-level>] [-rw <device_name>|-ro <device_name>] [-console] [-stop] [-threads <min_idle> <max_idle> <max>] [<root-name>]\n",program)

/*
 * How many service threads should be run
 */
int min_idle_threads = 2;
int max_idle_threads = 10;
int max_service_threads = 10;


main(
	int	argc,
	char**	argv)
{
	mach_error_t	err;
	int		cnt;
	int		cbuf[10];
	int		i;
	int		new_debug_level = us_debug_level;
	char		*root_name, *program;
	vn_dir*		root_agency;
	boolean_t	dev_specified = FALSE;
	int		dev_mode;
	char		dev_name[256];
	int		stop = 0;
	struct		fs *fs;
	int		error;
	time_value_t 	tv;

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
			stop = 1;
			continue;
		} else if (!strncmp(argv[0],"-ro",3) ||
			 !strncmp(argv[0],"-rw",3)) {
			if (!strncmp(argv[0],"-rw",3))
			        dev_mode = O_RDWR;
		        else
				dev_mode = O_RDONLY;
			argc--; argv++;
			strcpy(dev_name, argv[0]);
			dev_specified = TRUE;
			continue;
		} else if (!strncmp(argv[0],"-threads",7)) {
			if (0 > sscanf(argv[1],"%d", &min_idle_threads)) {
				USAGE();
				exit(-1);
			}
			if (0 > sscanf(argv[2],"%d", &max_idle_threads)) {
				USAGE();
				exit(-1);
			}
			if (0 > sscanf(argv[3],"%d", &max_service_threads)) {
				USAGE();
				exit(-1);
			}
			argc -= 3 ; argv += 3 ;
			continue;
		}
		else {
			USAGE();
			exit(1);
		}
	}

	if (!dev_specified) {
		/* must specify a device */
		USAGE();  
		exit(1);
	} 
	if (argc > 1) {
		USAGE();
		exit(1);
	} 
	else if (argc == 1) 
		root_name = argv[0];
	else 
		root_name = "ufs_server";

	/*
	 * Construct a unique mgr_id
	 */
	(void) mach_get_time(&tv);
	ufs_mgr_id.v1 = tv.microseconds;

	cthread_init();
	intr_init();
	rpcmgr::GLOBAL->start_object_handler(min_idle_threads,
				max_idle_threads, max_service_threads);

	err = diag_startup(root_name);
	if (err) mach_error("diag_starup",err);
	err = set_diag_level(new_debug_level);
	if (err) mach_error("set_diag_level",err);

	if (stop) {
		fprintf(stderr,
		"ufs server suspended. Awaiting restart from debugger.\n");
#if !defined(MACH3_UNIX)
		task_suspend(mach_task_self());
#else
		task_suspend(mach_task_self());
#endif	!defined(MACH3_UNIX)
	}

	nblkdev = 1;

	/*
	 * Initialize the various subsystems of the server.
	 */
 	zone_init();    /* disk_init() needs this */
	disk_init();	/* disk subsystem */
	data_init();	/* file data subsystem */
	ihinit(ninode);	/* inode table */
	binit(nbuf);   	/* buffer cache */

	/*
	 * Open the root device
	 */
	if (disk_open(dev_name, dev_mode) != ERR_SUCCESS) 
		exit(1);
	
	/*
	 * Count swap devices, and adjust total swap space available.
	 * Some of this space will not be available until a vswapon()
	 * system is issued, usually when the system goes multi-user.
	 */
	error = mountfs( /* rootdev */ 0, 0, (struct inode *)0, &fs);
	if (fs == 0)
		panic("iinit");
	bcopy("/", fs->fs_fsmnt, 2);
	error = iget(/* rootdev */ 0, fs, (ino_t)ROOTINO, &rootdir);
	u.u_rdir = rootdir;
	IUNLOCK(rootdir);

	cthread_detach(cthread_fork((cthread_fn_t)time_of_day_thread,0));
	
	ustruct_init(&u);

	/*
	 * Startup the front-end.
	 */
	root_agency = new vn_dir((fs_id_t) rootdir,ufs_mgr_id, dev_mode, &err);
	if (err != ERR_SUCCESS) {
		mach_error("Cannot setup the root directory",err);
		exit(1);
	}

	err = root_agency->ns_netname_export(root_name);
	if (err != ERR_SUCCESS) {
		mach_error("Cannot export the initial agent",err);
		exit(1);
	}

#if not
	if (new_debug_level > 0) {
		mach_object_t		debug_agent = MACH_OBJECT_NULL;

		(void) ns_create_initial_agent(root_agency,&debug_agent);
		test_us_driver(debug_agent);
	}
#endif not
}


