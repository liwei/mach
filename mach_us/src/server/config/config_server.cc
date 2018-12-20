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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/config/config_server.cc,v $
 *
 * Purpose: Mach Startup/Admin/Config Server
 *
 * HISTORY:
 * $Log:	config_server.cc,v $
 * Revision 2.5  94/07/07  16:37:44  mrt
 * 	Updated copyright
 * 
 * Revision 2.4  94/06/16  17:36:48  mrt
 * 	Added a wait call before exec the fsadmin program to make sure
 * 	all the servers have been started. Changed the default location
 * 	for prefix.config and rc.us to /mach_servers/us/etc.
 * 
 * Revision 2.3  94/05/17  14:09:23  jms
 * 	Cast args to cthread_fork
 * 	[94/04/28  19:06:36  jms]
 * 
 * Revision 2.2  92/07/05  23:34:06  dpj
 * 	First working version.
 * 	[92/06/24  17:39:37  dpj]
 * 
 * Revision 2.3  92/03/05  15:11:37  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:22:47  jms]
 * 
 * Revision 2.2  91/10/07  00:12:39  jjc
 * 	Added name service by borrowing code from snames.
 * 	Changed init_prefix_table() to use internal name lookup.
 * 	Fork and exec emul_init.
 * 	Changed command line syntax (again) to "config_server -d<debug level>
 * 	-p<prefix config file> -s<startup script> <emulation command line>".
 * 	[91/09/22            jjc]
 * 	Added startup of servers from shell script.
 * 	Changed init_prefix_table() to retry looking up servers after certain
 * 	[91/09/17  15:02:24  jjc]
 * 
 *	amount of time just in case all the servers haven't been started yet.
 * 	Changed command line syntax to "config_server -d<debug level>
 *	-h<host name> -p<prefix config file> <command to execute>".
 * 
 * Revision 2.1.1.2  91/09/06  17:34:43  jjc
 * 	Changed printfs to DEBUG statements.
 * 	Statically allocate table of prefix names and server objects.
 * 
 * Revision 2.1.1.1  91/07/05  10:46:09  jjc
 * 	Created.
 * 	[91/06/20            jjc]
 * 
 *
 */

/*
 *	DOES:
 *		Prefix Table
 *			Initialization (global/per user)
 *		Start All Servers
 *			Replace startup script
 *			Initial access to servers?
 *			Name service
 *
 *	TODO:
 *		Prefix Table
 *			Dynamic changes (mount/umount)
 *		List of All Servers
 *			Shutdown
 *			Reboot
 *			Sync
 *		Start All Servers
 *			Shared library
 *			Handout device ports
 *			Nanny?
 *		Global Functions
 *			settimeofday
 *			hostname, ifconfig, etc.
 *		Resource Limits
 *			Quotas
 *		/dev
 */

#include <sys_agency_ifc.h>
#include <rpcmgr_ifc.h>
#include <us_item_proxy_ifc.h>

extern "C" {
#include <mach.h>
#include <mach/error.h>
#include <mach/message.h>
#include <ns_types.h>
#include <mach/notify.h>
#include <stdio.h>

extern	snames_setup();
extern	snames();
extern 	perror();
extern	do_netname_lookup();
extern	sleep();
extern	atoi();
extern	exit();
extern	unix_fork();
extern	execv();
extern int wait(int *);
}
 


#define	SERVER_NAME		"config_server"

#define	COMMENT_CHAR		'#'	/* comment character for prefix file */

	/* default emulation program */
#define	EMUL_INIT		"/mach_servers/us/bin/emul_init"

	/* default prefix configuration file */
#define	PREFIX_FILE		"/mach_servers/us/etc/prefix.config"

#define	PREFIX_LINESIZE		256	/* max prefix config file line size */

#define MAX_PREFIX		32	/* max number of prefixes */
#define MAX_PREFIX_STRING	256	/* max prefix string length */

#define	MAX_WORDS		16	/* max words per startup command */

#define	SHELL			"/bin/csh"

	/* default secs to sleep on failed lookup */
#define	SLEEPTIME		60

	/* default multi-server startup file */
#define	STARTUP_FILE		"/mach_servers/us/etc/rc.us"


char		*hostname = "";

int		lookup_sleep = 4 * SLEEPTIME;

int		prefix_count;			/* number of prefixes */
ns_name_t	prefix_names[MAX_PREFIX];	/* table of prefix names */
usItem*		server_objects[MAX_PREFIX];	/* table of server objects */

sys_agency*	sys_object;		/* initial sys_agency object */


/*
 *	Initialize prefix table
 *
 *	emul_init will use the sys_get_prefix_table method to get the prefix
 *	table from here.  The prefix table will be initialized from a 
 *	configuration file.  Each line of the prefix table configuration file
 *	should be of the form <prefix> => <server name>.  Mount and unmount
 *	will change the prefix table and everyone will have to callback to 
 *	update their prefix tables.
 */
init_prefix_table(
	char		*filename,
	usItem**	server_objects,
	ns_name_t	*prefix_names,
	int		*prefix_count)
{
	char		arrow[PREFIX_LINESIZE];
	int		count;
	FILE		*fp;
	char		line[PREFIX_LINESIZE];
	char		prefix[PREFIX_LINESIZE];
	mach_error_t	retval;
	char		server[PREFIX_LINESIZE];
	mach_port_t		server_port;

	*prefix_count = 0;
	if ((fp = fopen(filename, "r")) == NULL) {
		perror(filename);
		return(1);
	}

	count = 0;
	while (count < MAX_PREFIX && (fgets(line, PREFIX_LINESIZE, fp) != NULL)) {
		if (line[0] == COMMENT_CHAR)
			continue;		/* skip comments */

		DEBUG1(TRUE,(Diag,"init_prefix_table: got line '%s'\n", line));

		/*
		 *	Get prefix and server names
		 */
		if (sscanf(line, "%s %s %s", prefix, arrow, server) != 3 
		    || arrow[0] != '=' || arrow[1] != '>'
		    || arrow[2] != '\0') {
			fprintf(stderr, "Bad prefix table config line '%s'",
				line);
			continue;
		}
		DEBUG1(TRUE,(Diag,"init_prefix_table: prefix '%s', server '%s'\n",
			prefix, server));
		/*
		 *	Lookup port for server 
		 *	and make an object for the server
		 */
		server_port = MACH_PORT_NULL;
		while (do_netname_look_up(name_server_port, hostname, server,
				&server_port) != KERN_SUCCESS) {
			mach_error("netname_look_up", retval);
			ERROR((Diag,"Can't lookup %s, trying again\n",server));
			/*
			 *	Sleep and retry again until we run out
			 *	of sleep time.
			 */
			if (lookup_sleep <= 0) {
				ERROR((Diag,"Giving up lookup %s\n",server));
				break;
			}
			else {
				sleep(SLEEPTIME);
				lookup_sleep -= SLEEPTIME;
			}
			
		}
		if (server_port == MACH_PORT_NULL)
			continue;

		server_objects[count] = new usItem_proxy;
		DEBUG1(TRUE,(Diag,"init_prefix_table: server object 0x%x, port 0x%x\n",
			server_objects[count], server_port));
		server_objects[count]->set_object_port(server_port);

		strcpy(&prefix_names[count][0], prefix);
		count++;
		DEBUG1(TRUE,(Diag,"init_prefix_table: prefix '%s', server port 0x%x\n",
			prefix, server_port));
	}
	*prefix_count = count;

	if (fclose(fp) != 0) {
		perror(filename);
		return(3);
	}

	return(0);
}


void usage(char	*progname)
{
	fprintf(stderr, "%s -d<debug level> -p<prefix config file> -s<startup script> <emulation command line>\n", progname);
}


main(
	int	argc,
	char	*argv[])
{
	char		*cmd[MAX_WORDS];
	char		*emul_cmd[MAX_WORDS];
	int		i, j;
	int		new_debug_level = us_debug_level;
	int		pid, chpid;
	int		wstatus;
	char		*prefix_file;
	mach_port_t	previous;
	mach_error_t	retval;
	char		*server_name;
	char		*startup_file = STARTUP_FILE;

	/*
	 *	Set default prefix and startup file names, 
	 *	and emulation command line.
	 */
	prefix_file = PREFIX_FILE;
	startup_file = STARTUP_FILE;

	emul_cmd[0] = EMUL_INIT;
	emul_cmd[1] = "-t";
	emul_cmd[2] = "/dev/console";
	emul_cmd[3] = "/bin/csh";
	emul_cmd[4] = 0;

	/*
	 *	Parse switches
	 */
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-')
			switch (argv[i][1]) {
			case 'd':
				new_debug_level = atoi(&argv[i][2]);
				break;
			case 'p':
				prefix_file = &argv[i][2];
				break;
			case 's':
				startup_file = &argv[i][2];
				break;
			default:
				usage(argv[0]);
				exit(1);
			}
		else {
			/* 
			 *	Assume rest of command line is 
			 *	what we're supposed to exec to
			 *	start the emulation
			 */
			for (j = 0; j < MAX_WORDS; i++, j++)
				emul_cmd[j] = argv[i];
			emul_cmd[j] = 0;
		}
	}

	server_name = SERVER_NAME;

	/*
	 *	Setup name server
	 */
	snames_setup(new_debug_level);

	/*
	 *	Startup servers
	 */
	cmd[0] = SHELL;
	cmd[1] = startup_file;
	cmd[2] = 0;
	pid = unix_fork();
	if (pid == 0) {
		(void)execv(cmd[0], cmd);
		fprintf(stderr, "Can't exec startup script\n");
		exit(2);
	}
	else if (pid == -1) {
		fprintf(stderr, "fork() failed\n");
		exit(3);
	}

	/*
	 *	Start name server, initialize Mach Objects, start our request
	 *	handler, and setup any external methods that we're going to use
	 *	and/or service.
	 */

	cthread_init();
	intr_init();

	cthread_detach(cthread_fork((cthread_fn_t)snames, 0));

	rpcmgr::GLOBAL->start_object_handler(0,1,1);

	(void)diag_startup(server_name);
	(void)set_diag_level(new_debug_level);
	
	/*
	 *	Initialize prefix table
	 */
	if (init_prefix_table(prefix_file, server_objects, prefix_names, 
				&prefix_count) != 0) {
		fprintf(stderr, "init_prefix_table() failed\n");
		exit(4);
	}

	/*
	 *	Create initial agent, set it up by giving it the prefix names
	 *	and server objects, and register its name with the nameserver.
	 */
	sys_object = new sys_agency(server_objects,
					prefix_names,prefix_count,&retval);
	if (retval != ERR_SUCCESS) {
		mach_error("Can't setup sys_agency",retval);
		exit(5);
	}

	retval = sys_object->ns_netname_export(server_name);
	if (retval != ERR_SUCCESS) {
		mach_error("Can't export the initial agent",retval);
		exit(6);
	}

	/*
	 * 	Check to see that fsadmin is done starting servers
	 */
	chpid = 0;
	while (chpid != pid) {
		chpid = wait(&wstatus);
	}
	if ( wstatus != 0 ) {
		fprintf(stderr,"fsadmin script failed\n");
		exit(10);
	}
	/*
	 *	Start emulation
	 */

	pid = fork();
	if (pid == 0) {
		(void)execv(emul_cmd[0], emul_cmd);
		fprintf(stderr, "Can't exec emulation command\n");
		exit(7);
	}
	else if (pid == -1) {
		fprintf(stderr, "fork() failed\n");
		exit(8);
	}
}
