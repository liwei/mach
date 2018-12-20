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
 * File: tty_server.c
 *
 * Purpose: Main program for tty server.
 *
 * Author: Michael B. Jones  --  1-Nov-1988
 */

/*
 *
 * HISTORY:
 * $Log:	tty_server.cc,v $
 * Revision 2.6  94/07/21  16:15:08  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/01/11  18:12:09  jms
 * 	Fix server directory protections.
 * 	[94/01/10  13:52:35  jms]
 * 
 * Revision 2.4  92/07/05  23:36:34  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:34:12  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:57:53  dpj]
 * 
 * Revision 2.3  92/03/05  15:15:32  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:48:08  jms]
 * 
 * Revision 2.2  91/11/06  14:24:23  jms
 * 	Update to use C++.
 * 	[91/09/17  14:24:04  jms]
 * 
 * Revision 1.16  91/10/07  00:12:55  jjc
 * 		Changed printf of device server port to a DEBUG0 statement.
 * 	[91/04/25            jjc]
 * 
 * Revision 1.15  91/07/01  14:15:45  jms
 * 	Use new args to "mach_object_start_handler"
 * 	[91/06/25  13:30:30  jms]
 * 
 * Revision 1.14  90/10/02  11:38:42  mbj
 * 	Made it work under any of MACH3_{UNIX,VUS,US} configurations.
 * 	Added ability to suspend tty_server with "-S" switch.
 * 	Made argument processing order independent.
 * 	[90/10/01  15:38:05  mbj]
 * 
 * 	Made it work again under MACH3_UNIX after Paul Neves' pure kernel
 * 	tty_server changes.
 * 	[90/09/12  13:22:58  mbj]
 * 
 * Revision 1.13  90/09/07  13:43:34  mbj
 * 	Dropped hokey netname "/dev/tty" agent checkin.
 * 	[90/09/07  11:31:23  mbj]
 * 
 * Revision 1.12  90/09/05  09:46:11  mbj
 * 	Reworked for the addition of ns naming interface for ttys.
 * 	[90/09/04  15:29:04  mbj]
 * 
 * Revision 1.11  89/10/30  16:40:24  dpj
 * 	Fixed test code.
 * 	[89/10/27  19:52:12  dpj]
 * 
 * Revision 1.10  89/07/09  14:22:18  dpj
 * 	Added "-d<debug-level>" switch, and disabled call to
 * 	test_us if the debug level is not greater than 0.
 * 	[89/07/08  15:46:45  dpj]
 * 
 * 	Fixed initialization of Diag system.
 * 	[89/07/08  13:20:25  dpj]
 * 
 * Revision 1.9  89/03/21  14:44:13  mbj
 * 	Merge mbj_pgrp branch onto mainline.
 * 
 * Revision 1.8  89/03/17  13:06:10  sanzi
 * 	Removed a bunch of useless includes.
 * 	Reorganized initialization to work with libus.
 * 
 * Revision 1.7.1.2  89/03/07  13:50:44  mbj
 * 	Call init_signals().
 * 
 * Revision 1.7.1.1  89/03/02  11:53:22  mbj
 * Print usage message for zero args.  Check if tty_name arg exists.
 * 
 * Revision 1.7  89/01/19  16:46:51  mbj
 * Slight syntax changes to satisfy new Sun compiler.
 *
 * Revision 1.6.1.1  89/02/22  15:00:03  dpj
 * 	Removed a bunch of useless includes.
 * 	Reorganized initialization to work with libus.
 * 
 * Revision 1.6  88/12/09  11:35:20  mbj
 * Add -xterm switch and slave_xterm capibility.  Rationalized arg parsing.
 * 
 * Revision 1.5  88/12/05  02:55:44  mbj
 * Add SIGINT handler.  Close all open bsd ttys upon a SIGINT and then exit.
 * 
 * Revision 1.4  88/11/16  11:32:54  mbj
 * Build without -DKERNEL or multitudes of feature .h files.
 * 
 * Revision 1.3  88/11/04  11:34:16  dorr
 * add -d option.
 * 
 * Revision 1.2  88/11/01  17:03:14  mbj
 * Also check in name given on cmd line as "/dev/tty" for now.
 * 
 * Revision 1.1  88/11/01  14:14:46  mbj
 * Initial revision
 * 
 */

#include <rpcmgr_ifc.h>
#include <tty_dir_ifc.h>
#include <diag_ifc.h>

extern "C" {

#include <base.h>
#include <stdio.h>
/* #include <mach/mach_types.h> */
#include <mach.h>
/* #include <servers/netname.h>  */
#include <sys/systm.h>

#if	! MACH3_US
#include <sys/types.h>
#include <sys/signal.h>
#endif	! MACH3_US

#include <device/device_types.h>
#include <uxkern/device.h>
#include <mach_privileged_ports.h>

#include <auth_defs.h>

#include <ns_types.h>

extern char *rindex();
extern ns_authid_t fs_access_default_root_authid;

extern void
    init_structs(),
    init_tty_names(),
    bsd_tty_init(),
    init_signals(),
#if	! MACH3_US
    close_all_bsd_ttys(),
#endif	! MACH3_US

    zone_init(),
    dev_utils_init(),
    start_tty_threads();

void exit();

}

extern mach_port_t
    name_server_port;

/*
 * Global ID.
 */
#define	TTY_MGR_ID	{ 0, 0x04000001 }
ns_mgr_id_t	tty_mgr_id = TTY_MGR_ID;

#if	! MACH3_US
int cleanup_tty_server();
#endif	! MACH3_US

mach_port_t device_server_port;

char   *remote_netname_host;

#define USdh	"[-S(to suspend)] [-d<debug_level] [-h<diag_host_name>]"
#define Urxl	"[-r<remote_netname_host>] [-x<device_host_name] [-l[<device_port_name>]]"
#define USAGE()	printf("Usage: %s %s %s\n", program, USdh, Urxl);

main(int argc, char *argv[])
{
    char *program, *diag_host;
    char *tty_server = "tty_server";

    char dev_host_name[80];
    char dev_port_name[80];

    int new_debug_level = us_debug_level;
    mach_error_t ret;

    ns_prot_t		dir_prot = NULL;
    int			acl_len = 2;

    /* Default Arguments */

    diag_host = NULL;
    strcpy(dev_host_name, "");
    strcpy(dev_port_name, "");

    cthread_init();
    intr_init();
    condition_init(&selwait_lock);

    program = argv[0];
    for (argc--,argv++; (argc > 0) && (argv[0][0] == '-'); argc--,argv++) {
	if (strncmp(argv[0],"-S",2) == 0) {
	    task_suspend(mach_task_self());
	    continue;
	}
	else
	if (strncmp(argv[0],"-d",2) == 0) {
	    sscanf(argv[0]+2,"%d",&new_debug_level);
	    continue;
	}
	else
	if (strncmp(argv[0],"-h",2) == 0) {
	    diag_host = argv[0]+2;
	    continue;
	}
	else
	if (strncmp(argv[0],"-x",2) == 0) {
	    (void) strcpy(dev_host_name, argv[0]+2);
	    continue;
	}
	else
	if (strncmp(argv[0],"-l",2) == 0) {
	    if (argv[0][2] != 0)
		(void) strcpy(dev_port_name, argv[0]+2);
	    else
		(void) strcpy(dev_port_name, "mach_device_port");

	    continue;
	}
	else

	if (strncmp(argv[0],"-r",2) == 0) {
	    mach_port_t server_port;
	    remote_netname_host = argv[0]+2;
	    ret = netname_look_up(name_server_port, remote_netname_host,
				  "new_network_name_server", &server_port);
	    if (ret)
		mach_error("new_network_name_server lookup failed", ret);
	    else
		name_server_port = server_port;
	    continue;
	}
	else
	{
	    USAGE();
	    exit(1);
	}
    }

    if (argc > 0) {
	USAGE();
	exit(1);
    }


    device_server_port = MACH_PORT_NULL;
    if (device_server_port == MACH_PORT_NULL) {
	if ((device_server_port = mach_device_server_port()) == MACH_PORT_NULL) {
	    printf("Error: Can't get device_server_port\n");
	    exit(1);
	}
    }
    printf("device_server_port = %d\n", device_server_port);

    /*
     * Initialize subsystems.
     */
    rpcmgr::GLOBAL->start_object_handler(1,2,100);

    (void) diag_startup(tty_server);
    (void) set_diag_level(new_debug_level);

    /*
     * Initialize tty subsystems.
     */

    spl_init();
    zone_init();
    dev_utils_init();
    device_reply_hdlr();
    init_structs();
    init_tty_names();
#if	CMUCS
    cmuptyinit();
#endif	CMUCS
#if	! MACH3_US
    bsd_tty_init();
#endif	! MACH3_US
    init_signals();
    start_tty_threads();

    tty_dir *root = new tty_dir(tty_mgr_id, &ret);
    if (ret != ERR_SUCCESS) {
	mach_error("Cannot setup the root directory",ret);
	exit(1);
    }

    dir_prot = (ns_prot_t)malloc(sizeof(struct ns_prot_head) +
			(acl_len * sizeof(struct ns_acl_entry)));

    dir_prot->head.version = NS_PROT_VERSION;
    dir_prot->head.generation = 0;
    dir_prot->head.acl_len = acl_len;

    dir_prot->acl[0].authid = fs_access_default_root_authid;
    dir_prot->acl[0].rights = NSR_ADMIN |
	NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;
    dir_prot->acl[1].authid = NS_AUTHID_WILDCARD;
    dir_prot->acl[1].rights =
	NSR_REFERENCE | NSR_READ | NSR_GETATTR | NSR_LOOKUP;

    ret = root->ns_set_protection(dir_prot,
			     NS_PROT_SIZE(dir_prot) / sizeof(int));
    free(dir_prot);

    if (ret != ERR_SUCCESS) {
	us_internal_error("ns_set_protection(tty_dir)",ret);
    }

    ret = root->ns_netname_export(tty_server);
    if (ret != ERR_SUCCESS) {
	mach_error("Cannot export the initial agent",ret);
	exit(1);
    }

#if	! MACH3_US
    (void) signal(SIGINT, cleanup_tty_server);
#endif	! MACH3_US

    printf("All dressed up, and no where to go.\n");
    while (1) sleep(1000000);
    exit(0);
}

#if	! MACH3_US
static cleanup_in_progress = 0;
int cleanup_tty_server()
/*
 * Called before exiting from the sigint handler.
 */
{
    if (! cleanup_in_progress) {
	cleanup_in_progress = 1;
	close_all_bsd_ttys();	/* Close any open bsd ttys */
    }
    exit(0);
}
#endif	! MACH3_US

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
