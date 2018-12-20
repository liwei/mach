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
 * File:        emul_init.c
 *	
 * Purpose:
 *      this file contains a user space server that takes requests to
 *	load and run programs.
 *
 * HISTORY: 
 * $Log:	emul_init.cc,v $
 * Revision 2.8  94/07/20  13:10:39  mrt
 * 	If the program to startup in is csh or sh, start it as
 * 	a "login shell".
 * 	[94/07/20            mrt]
 * 
 * Revision 2.7  94/07/08  16:56:57  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.6  94/05/17  15:24:58  jms
 * 	To fix problem for loader_load_program_file for new 2.3.3 g++ compiler
 * 	[94/02/18  11:21:05  modh]
 * 
 * Revision 2.5  92/07/05  23:25:01  dpj
 * 	Don't nee tm_types.h
 * 	[92/06/24  14:07:41  jms]
 * 	Eliminated uses of fd_* classes.
 * 	[92/06/26            dpj]
 * 	Eliminated diag_format().
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:29:50  dpj]
 * 
 * Revision 2.4  92/03/05  14:55:38  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:14:04  jms]
 * 
 * Revision 2.3  91/11/13  16:38:20  dpj
 * 	Explicitly allocate and switch to  a special stack before
 * 	calling emul_initialize(). Remove use of cthreads for stack manipulation,
 * 	since this scheme does not provide enough control over stack sizes.
 * 
 * 	fork() and exec() from main emul_init program, after
 * 	emul_initialize() returns.
 * 
 * 	Use a simple a.out loader instead of libload for the emulation library.
 * 	Start emul_initialize directly with all appropriate arguments instead
 * 	of setting the arguments separately.
 * 
 * 	Clean-up the entire emulation space (minus user stack) before loading
 * 	the new emulation library.
 * 	[91/11/08            dpj]
 * 
 * Revision 2.2  91/11/06  11:29:47  jms
 * 	Upgraded the calls that initialize the Diag system.
 * 	[91/10/03  15:11:41  pjg]
 * 
 * 	Upgraded to US41.
 * 	[91/09/26  19:29:29  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:40:21  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:31:23  pjg]
 * 
 * 	check for emul_type_umount.  get rid of
 * 	some random printf's.
z * 	[90/01/02  14:08:50  dorr]
 * 
 * 	checkin before christmas
 * 	[89/12/19  17:04:53  dorr]
 * 
 * 	initial checkin.
 * 	[89/12/18  15:43:18  dorr]
 * 
 * Revision 2.11  91/10/06  22:26:37  jjc
 * 		Changed debugging statements to only print if the debugging level
 * 		is greater than 0.
 * 	Added call to sys_methods_setup(), so we can use the configuration
 * 	server external methods.
 * 	[91/04/25            jjc]
 * 
 * Revision 2.10  91/07/01  14:06:28  jms
 * 	Added "-stat" switch for statistics gathering.
 * 	[91/06/21  17:07:45  dpj]
 * 	No Further Change
 * 	[91/06/24  15:56:41  jms]
 * 
 * Revision 2.9  91/05/05  19:24:27  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:50:23  dpj]
 * 
 * 	Follow transparent symlinks.
 * 	[91/04/28  09:43:04  dpj]
 * 
 * Revision 2.8  90/11/27  18:17:49  jms
 * 	Modify the "stop" switch to fit server norm.
 * 	[90/11/20  11:23:47  jms]
 * 
 * 	About to add tty changes from trunk
 * 	[90/11/08  13:38:54  jms]
 * 
 * Revision 2.7  90/10/29  17:24:13  dpj
 * 	No changes.
 * 
 * 
 * Revision 2.6  90/10/02  11:35:43  mbj
 * 	Added ability to suspend emul_init with "-S" switch.
 * 	Made argument processing order independent.
 * 	Don't append "_htg" to explicitly specified tty names.
 * 	[90/10/01  14:49:06  mbj]
 * 
 * Revision 2.5  90/09/07  13:43:10  mbj
 * 	Provide the initial tty name to be opened to the emulation init code.
 * 	By default the initial tty is our ttyname.  It may also be allocated
 * 	via the -xterm switch or be explicitly specified with the -t switch.
 * 	This module does the mapping from /dev/tty* to /dev/tty*_htg names.
 * 	[90/09/07  12:02:18  mbj]
 * 
 * Revision 2.4  90/07/09  17:01:59  dorr
 * 	Run emul_initialize on an emulation space stack, not the primary user stack.
 * 	[90/07/06  14:38:45  jms]
 * 
 * Revision 2.3  90/03/21  17:19:56  jms
 * 	Remove tm.h include
 * 
 * Revision 2.2  90/01/02  21:40:41  dorr
 * 	use emul_type_{type} variable for
 * 	library specific initializations.
 * 
 * Revision 2.8  89/10/30  16:40:16  dpj
 * 
 */

//extern "C" {
//#include <mach/mach_types.h>
//}

#include <us_byteio_ifc.h>
#include <us_name_ifc.h>
#include <diag_ifc.h>
#include <emul_user_init_ifc.h>
#include <machine/machine_address_space.h>
#include <loader_info.h>
#include <loader.h>

extern "C" {

#include <base.h>
#include <debug.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

/* #include <mach/mach_types.h> */
#include <servers/netname.h>
#include <mach_error.h>
#include <mach/message.h>

#include <sys/syscall.h>
#ifndef KERNEL
#define KERNEL 0
#endif
#include <sys/param.h>

#include <ns_types.h>

#include "us_ports.h"

#include "emul_config.h"


#if Authentication
#include <auth_defs.h>
#endif Authentication

extern char 		*mach_error_string();
extern void 		get_library();
extern char *ttyname();

extern mach_port_t 		name_server_port;

}


extern mach_error_t	find_initial_user_stack();
extern mach_error_t	clean_for_init();
extern mach_error_t	adjust_loadinfo_for_emulator(struct loader_info*);

#define	MAX_EMULATION	0xffffffff

static std_name* pfx_obj;

char *			emul_lib;

mach_port_t			public_tm_port = MACH_PORT_NULL;
mach_port_t			public_as_port = MACH_PORT_NULL;
mach_port_t			public_ns_port = MACH_PORT_NULL;

char * usage = 
#if	! MACH3_US
"usage: emul_init [-stop(to suspend)] [-ddebuglevel] [-Dexecdebuglevel] [-t ttyname] [-xterm] [-stat level] program ...\n";
#else	MACH3_US
"usage: emul_init [-stop(to suspend)] [-ddebuglevel] [-Dexecdebuglevel] [-t ttyname] [-stat level] program ...\n";
#endif	MACH3_US


int			exec_debug_level = 0;
int			statistics_level = 0;

/*
 * Size for emulation stacks.
 *
 * Must be a power of 2.
 */
#define	EMUL_STACK_SIZE		512 * 1024


/*
 * Global variables for calling emul_initialize on a different stack.
 */
mach_error_fn_t		emul_initialize;
ns_identity_t		as_ident;
ns_token_t		as_token;
char			*tty_name = 0;


/*
 * Call emul initialize in order to:
 *   tell the emulation library where its text regions are.
 *   give it a prefix object to clone.
 *
 * Return when everything is ready, before starting to client process.
 *
 * Must be running on a special emulation stack.
 */
extern "C" call_emul_initialize()
{
	emul_initialize(as_ident, as_token,
		us_debug_level, exec_debug_level, statistics_level, 
		MAX_EMULATION, tty_name);
}


boolean_t
resolve_ok(char* fname)
{
	mach_error_t		err;
	static unsigned int	access = NSR_READ | NSR_EXECUTE | NSR_GETATTR;
	usItem* obj;
	ns_type_t		type;

	err = pfx_obj->ns_resolve_fully(fname,
					NSF_ACCESS | NSF_FOLLOW_ALL,
					access, &obj, &type, NULL);
	/*
	 * XXX C++ dereference the object ??
	 */
	mach_object_dereference(obj);
	if (err == ERR_SUCCESS) {
		if (us_debug_level > 0) printf("%s: bingo\n", fname);
		return 0;
	} else {
		return 1;
	}
}


mach_error_t
find_emul_lib(char *fname, usByteIO** lib_obj)
{
	ns_type_t		type;
	usItem*			a_obj = NULL;
	static unsigned int	access = NSR_READ | NSR_EXECUTE | NSR_GETATTR;
	mach_error_t		err;
	char			pathname[1024];
	char			* path;

	*lib_obj = 0;

#if USE_GETENV
	path = (char *)getenv("EMULPATH");
	if (path == NULL)
		path = "/lib";
#else USE_GETENV
	path = "/lib";
#endif USE_GETENV

	if ( searchp(path,fname,pathname,resolve_ok) < 0 ) {
		extern int errno;
		ERROR((Diag,"%s: not found on path '%s'\n",
			    fname, path));
		err = unix_err(errno);
		goto finish;
	}

	err = pfx_obj->ns_resolve_fully(pathname, NSF_FOLLOW_ALL,
					 access, &a_obj, &type, NULL);
	if (err) goto finish;
	if ((*lib_obj = usByteIO::castdown(a_obj)) == 0) {
		DEBUG0 (TRUE, (0, "map_it\n"));
		err = MACH_OBJECT_NO_SUCH_OPERATION;
		goto finish;
	}
	a_obj = NULL;
	return(ERR_SUCCESS);

    finish:
	if (a_obj)
		mach_object_dereference(a_obj);
	if (*lib_obj) {
		mach_object_dereference(*lib_obj);
		*lib_obj = 0;
	}

	return err;

}


main(int argc, char** argv, char** envp)
{
	mach_error_t 		err;
	usByteIO*		lib_obj;
	struct loader_info	li;
	int			new_brk;
	int			entry[2];
	unsigned int		entry_count;
	vm_address_t		emul_stack;
	boolean_t		stop = FALSE;
	mach_error_t map_it(char*, vm_offset_t*, vm_size_t*);
	void      _init_user_proxies(void);
	void      _print_map(void);
	char		*progname, *cp;
	char		shellname[6] = "-";

	if (stop) task_suspend(mach_task_self());

	cthread_init();

#if	! MACH3_US
	char tty_name_htg[40];
#endif	! MACH3_US

	(void) diag_startup("init");

	emul_lib = malloc(strlen(EMUL_NAME) + strlen(".lib") + 1);
	strcpy(emul_lib, EMUL_NAME);
	strcat(emul_lib, ".lib");

	_init_user_proxies();
//	_print_map();

#if	( ! emul_type_umount )
	process_args(&argc, &argv, usage);
	
	emul_user_init(&as_ident,&as_token);
#else
	++argv;
	--argc;
#endif	( !umount )

	for (; (argc > 0) && (argv[0][0] == '-'); argc--,argv++) {
	    if (strncmp(argv[0],"-stop",5) == 0) {
		stop = TRUE;
		continue;
	    }
	    else
	    if (strncmp(argv[0],"-d",2) == 0) {
		sscanf(argv[0]+2,"%d",&us_debug_level);
		continue;
	    }
	    else
	    if (strncmp(argv[0],"-D",2) == 0) {
		sscanf(argv[0]+2,"%d",&exec_debug_level);
		continue;
	    }
	    else
	    if (strncmp(argv[0],"-t",2) == 0) {
		if (argv[0][2] != 0) {
		    tty_name = & argv[0][2];
		} else {
		    argv++; argc--;
		    tty_name = argv[0];
		}
		continue;
	    }
	    else
	    if (strncmp(argv[0],"-stat",5) == 0) {
		argv++; argc--;
		sscanf(argv[0],"%d",&statistics_level);
		continue;
	    }
	    else
#if	! MACH3_US
	    if ( strcmp(argv[0], "-xterm") == 0) {
		static char *xterm_args[] = {"-name", "tty_server", "-n", "tty_server %t_htg", 0};
		int error;
		int tty_fd;

		error = slave_xterm(xterm_args, &tty_name, &tty_fd);
		if (error) {
		    if (error == -1)
			fprintf(stderr, "slave_xterm error -- no more ptys!\n");
		    else {
			extern int errno;	/* XXX Wants strerror() */
			fprintf(stderr, "slave_xterm error -- bsd error %d: ",
			    error);
			errno = error; perror(""); /* XXX Wants strerror() */
		    }
		    exit(1);
		} else {
		    strcpy(tty_name_htg, tty_name);
		    strcat(tty_name_htg, "_htg");
		    tty_name = tty_name_htg;
		}
		continue;
	    }
	    else
#endif	! MACH3_US
	    {
		fprintf(stderr, "%s", usage);
	    }
	}

	if (argc <= 0) {
		fprintf(stderr, "%s", usage);
		fprintf(stderr, "No program to exec!\n");
		exit(1);
	}

	if (tty_name == 0) {
#if	! MACH3_US
	    tty_name = ttyname(0);
	    if (tty_name == 0) {
		fprintf(stderr, "ttyname(0) can't find the current tty\n");
		exit(1);
	    }
	    strcpy(tty_name_htg, tty_name);
	    strcat(tty_name_htg, "_htg");
	    tty_name = tty_name_htg;
#else	MACH3_US
	    tty_name = "/dev/console";
#endif	MACH3_US
	}

	/* emul_init for debugging */
	if (stop) task_suspend(mach_task_self());

	/*
	 * Decide where the useful part of the user stack is before
	 * destroying our address space.
	 */
	if (err = find_initial_user_stack()) {
		mach_error("find_initial_user_stack", err);
		exit(1);
	}

	/*
	 * Get rid of everything but the user program, to make
	 * space for the new emulation library.
	 *
	 * We can't count on the previous emulation after this!
	 */
	if (err = clean_for_init()) {
		mach_error("clean_for_init", err);
		exit(1);
	}

	/*
	 * open up the shared emulation library
	 */

	/*
	 * use the emulated name space
	 */
	if (err = emul_user_name_space_init(&pfx_obj, as_token)) {
		mach_error("user name space init", err);
		exit(1);
	}

	if (err = find_emul_lib(emul_lib,&lib_obj)) {
		mach_error("exec object open", err);
		goto out;
	}
	
	err = loader_ex_get_header(lib_obj,&li);
	if (err) {
		mach_error("loader_ex_get_header",err);
		goto out;
	}

	err = adjust_loadinfo_for_emulator(&li);
	if (err) {
		mach_error("adjust_loadinfo_for_emulator",err);
		goto out;
	}

	err = loader_load_program_file(lib_obj,&li,(vm_address_t *)&new_brk);
	if (err) {
		mach_error("load shared lib", err);
		goto out;
	}

	/*
	 * Find the emulation library entry point.
	 */
	entry_count = 2;
	err = loader_set_entry_address(&li,entry,&entry_count);
	if (err) {
		mach_error("loader_set_entry_address",err);
		goto out;
	}
	
	emul_initialize = (mach_error_fn_t) entry[0];

	/*
	 * clean up some old objects
	 */
/* XXX C++ */
	pfx_obj->ns_set_token(MACH_PORT_NULL);
	mach_object_dereference(pfx_obj);
	pfx_obj = NULL;
	mach_object_dereference(lib_obj);
	lib_obj = 0;

	/*
	 * Allocate a first emulation stack.
	 */

	emul_stack = EMULATION_BASE_DATA_ADDRESS;
	err = vm_map(mach_task_self(),&emul_stack,EMUL_STACK_SIZE,0,TRUE,
			MEMORY_OBJECT_NULL, 0, FALSE, VM_PROT_ALL,
			VM_PROT_ALL, VM_INHERIT_DEFAULT);
	if (err) {
		mach_error("vm_map(emul_stack)",err);
		goto out;
	}

	/*
	 * Call emul_initialize on the emulation stack.
	 * Make sure to reserve some space at the top of this stack
	 * for housekeeping information in the emulation library.
	 */
#ifdef	STACK_GROWTH_UP
	err = emul_init_switch_stack(emul_stack + (10 * sizeof(vm_offset_t)));
#else	STACK_GROWTH_UP
	err = emul_init_switch_stack(emul_stack + 
				EMUL_STACK_SIZE - (10 * sizeof(vm_offset_t)));
#endif	STACK_GROWTH_UP
	if (err) {
		mach_error("emul_init_switch_stack",err);
		goto out;
        }

	/* if the program to start is sh or csh start it as a "login shell" */
	progname = argv[0];
	cp = rindex(progname,'/');
	if ( cp ) cp++;
	else cp = progname;
	if ( !(strcmp(cp,"csh") && strcmp(cp,"sh"))) 
	{
	    strcat(shellname,cp);
	    argv[0] = shellname;
	}
	/*
	 * Now that the new emulation library is ready, exec()
	 * the first "real" emulated process.
	 */
        (void)mach3_syscall(SYS_execve, progname, argv, 0);

	/*
	 * We should never reach here...
	 */

    out:
	if (err) {
		mach_error("emul_init failed",err);
		exit (1);
	} else {
		fprintf(stderr,"emul_init: exec() returned\n");
		exit(0);
	}
}




