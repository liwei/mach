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
 * File:        emul_misc.c
 *
 * Purpose:
 *	Common routines for user space emulation of unix system calls
 *
 * HISTORY: 
 * $Log:	emul_misc.cc,v $
 * Revision 2.6  94/07/08  16:57:09  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.5  92/07/05  23:25:15  dpj
 * 	Make the exec_debug_level more broadly available for use as a backdoor
 * 	general debug flag.
 * 	[92/06/24  14:29:02  jms]
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:35:08  dpj]
 * 
 * Revision 2.4  92/03/05  14:55:57  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	Keep a list of existing emul cthread stacks and enable its cleanup after fork.
 * 	Using a stack in the list => one is executing in the emulation lib.
 * 	[92/02/26  17:20:36  jms]
 * 
 * Revision 2.3  91/11/13  16:41:36  dpj
 * 	Set-up the first emulation stack on the stack that emul_initialize is
 * 	called on instead of allocating a new one.
 * 
 * 	Let emul_init do fork() and exec(), after emul_initialize() returns.
 * 
 * 	Pass all appropriate arguments directly to emul_initialize instead
 * 	of using the indirect set/get approach.
 * 	Removed the code to create a startup stack: this is now done
 * 	by calling emul_initialize in a separate cthread.
 * 
 * 	Fixed to correctly dump statistics on exit(), before getting rid
 * 	of the diag object.  
 * 
 * 	Allow negative initial exec debug levels.
 * 	Use new machdep specifications for user stack, remove
 * 	emul_init_proc() and region-related stuff.
 * 	[91/11/08            dpj]
 * 
 * Revision 2.2  91/11/06  11:30:12  jms
 * 	Upgraded the calls that initialize the Diag system.
 * 	[91/10/03  15:12:21  pjg]
 * 
 * 	Upgraded to US41.
 * 	[91/09/26  19:32:56  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:26:30  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:28:38  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:31:50  pjg]
 * 
 * Revision 1.42  91/10/06  22:26:49  jjc
 * 	Added call to sys_methods_setup() to emul_initialize(), so
 * 	we can talk to the configuration server.
 * 	     Call external routine emul_setup() to establish emulation vectors.
 * 	[91/07/06            jjc]
 * 
 * Revision 1.41  91/07/01  14:06:46  jms
 * 	Added statistics gathering logic.
 * 	[91/06/21  17:09:04  dpj]
 * 
 * 	Removed the diag object from the clone list, and arranged
 * 	for the diag connection to be lazily allocated after fork.
 * 	Added emul_host_port(), used by emul_gettimeofday().
 * 	Clean-up the task_master-related objects on exit.
 * 	[91/06/16  20:58:30  dpj]
 * 	Add NoMoreSenders logic.
 * 	Add routine to handle Panics/unhandled emulation lib sych events.
 * 	Reset interrupt system when forking.
 * 	analretentive=>NOT_SOLVED_BY_NO_MORE_SENDERS.
 * 	[91/06/24  16:17:01  jms]
 * 
 * 	No further change
 * 	[91/04/15  17:10:37  jms]
 * 
 * 	Call mach_object_init() not mach_object_tables_init()
 * 	[91/02/21  13:59:11  jms]
 * 
 * Revision 1.40  91/05/05  19:24:43  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:51:01  dpj]
 * 
 * 	Added extra definition of MEMORY_OBJECT_NULL to make compilation
 * 	easier.  
 * 	Changed emul_debug and syscall trace for new tracing scheme.
 * 	Re-enabled the cleanup code in exit() (was "#ifdef analretentive").
 * 	[91/04/28  09:47:15  dpj]
 * 
 * Revision 1.39  91/04/12  18:47:26  jjc
 * 	Replaced path_length_ok() with emul_path() which checks path
 * 	lengths and splits the pathname into directory and file names.
 * 	[91/04/02            jjc]
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:48:12  jjc]
 * 
 * Revision 1.38  91/03/25  14:14:21  jjc
 * 	Hardwired initial emulation stack to be 512K in 
 * 	emul_initial_stack_setup().
 * 	[91/02/05            jjc]
 * 
 * Revision 1.37.1.1  91/02/05  12:21:15  neves
 * 	Added path_length_ok routine to verify pathlengths.
 * 
 * Revision 1.37  90/11/27  18:18:15  jms
 * 	No Change
 * 	[90/11/20  11:29:38  jms]
 * 
 * 	About to add tty changes from trunk
 * 	[90/11/08  13:39:13  jms]
 * 
 * 	Merge in last mbj changes
 * 	[90/08/20  17:19:40  jms]
 * 
 * 	Restructure for running on pure kernel.
 * 	Add pure kernel code first pass
 * 	[90/08/16  17:19:05  jms]
 * 
 * Revision 1.36  90/09/07  13:43:14  mbj
 * 	Accept the initial tty name passed in by emul_init. Clean up
 * 	initialization error handling to pay attention to fatal errors.
 * 	[90/09/07  11:21:44  mbj]
 * 
 * Revision 1.35  90/08/14  22:03:06  mbj
 * 	Put back emul_init_more_io().  The Task_Master must be initialized
 * 	before the tty pgrp can be.
 * 	[90/08/14  14:43:30  mbj]
 * 
 * Revision 1.34  90/07/09  17:15:36  dorr
 * 	Fixed the log.
 * 
 * 
 * Revision 1.33  90/07/09  17:02:09  dorr
 * 	first working version for multiple cthreads.
 * 	[90/03/19  17:25:52  dorr]
 * 
 * 	restore port_cache_resetpush
 * 	[90/03/01  14:50:55  dorr]
 * 
 * 	add stack switch stuff to allocate a new emulation
 * 	stack (and accompanying cthread state).
 * 	[90/02/23  14:41:40  dorr]
 * 
 * 	add clone_master stuff.  
 * 	[90/01/11  11:31:42  dorr]
 * 	Add mechinism to run emul_initialize on a emulation space stack with
 * 	arg coming in the "back door".
 * 
 * Revision 1.32  90/03/21  17:20:22  jms
 * 	[90/01/04  16:30:35  jms]
 * 
 * Revision 1.31  90/01/02  21:46:32  dorr
 * 	restructure.  conditionalize initialization functions.
 * 	call out to optional modules for actual initialization
 * 	routines.  set up syscalls based on a per-library table.
 * 
 * Revision 1.30.1.3  90/01/02  14:11:20  dorr
 * 	get rid of signal and alert methods.
 * 
 * Revision 1.30.1.2  89/12/19  17:05:15  dorr
 * 	checkin before christmas
 * 
 * Revision 1.30.1.1  89/12/18  15:49:27  dorr
 * 	conditionalize.  set up syscalls based on list of those
 * 	to be emulated at initialization time.
 * 
 * Revision 1.25.1.1  89/05/15  12:04:52  dorr
 * 	get rid of emul_init_more_io().  pass in initial
 * 	identity from tm_init.
 * 
 * Revision 1.23.2.1  89/03/31  16:05:07  mbj
 * 	Create mbj_signal branch
 * 
 * Revision 1.24  89/03/31  15:20:38  mbj
 * 	Support fork() of multi-threaded emulation library.
 * 	[89/03/30  15:57:19  mbj]
 * 
 * Revision 1.21.2.2  89/03/30  16:47:21  mbj
 * 	Call emul_init_signals() and display emulation library version.
 *
 * Revision 1.21.2.1  89/03/07  13:38:15  mbj
 * 	Call emul_init_more_io to set tty pgrp after emul_init_tm has finished.
 * 
 * Revision 1.21  88/12/02  15:26:38  dorr
 * reinit debug output on fork.
 * 
 */

#include <rpcmgr_ifc.h>
#include <emul_all_ifc.h>
#include <diag_ifc.h>
#include "emul_base.h"
#include "emul_proc.h"

extern "C" {

#include <tm_types.h>
#include "syscall_table.h"
#include "interrupt.h"

#include <sys/syscall.h>
#include <sys/errno.h>
#include <dll.h>

void drop_dead(void);

vm_offset_t	cproc_stack();
ur_cthread_t	cthread_create_phantom();
}

#ifndef	MEMORY_OBJECT_NULL
#define	MEMORY_OBJECT_NULL	MACH_PORT_NULL
#endif	MEMORY_OBJECT_NULL

std_name*		prefix_obj;

int			emul_debug = 0xffffffff;
int			emul_statistics = 0;

/* Is this task about to die? (simplifies some object cleanup) */
boolean_t		emul_task_terminating = FALSE;

/*
 * Cache the host port to avoid making a trap each time
 * (e.g. for gettimeofday().
 */
mach_port_t		emul_host_port_cache = MACH_PORT_NULL;

extern mach_error_t	find_initial_user_stack();
extern 			emul_exec_debug(boolean_t, int);


open_debug_connection(int id)
{
    	char 		open_string[256];

	sprintf(open_string,"bsd emulation <%d>", id);

	(void) diag_startup_lazy(open_string);
}


extern int	cthread_stack_size;

extern "C" {
boolean_t emul_stacks_member(vm_offset_t);
}


/* stuff for stack "free_list" */
typedef struct {
	vm_offset_t	next;
} * free_stack_t;
free_stack_t		emul_stack_free_list;

extern "C" {
free_stack_t emul_stack_alloc(void);
}

/*
 * The first emulation space cthread stack allocated.
 * Used by children to find themselves
 */
free_stack_t		emul_first_cthread_stack = NULL;

/*
 * set of emul_stacks and test for membership
 */
typedef struct emul_stack_chain {
	vm_offset_t	stack;
	dll_chain_t	stack_chain;
} *emul_stack_chain_t;

/* dll of all existing emulator stacks */
static dll_head_t	emul_stacks;
static struct mutex	emul_stacks_lock;

static void emul_stacks_init()
{
    mutex_init(&emul_stacks_lock);
    dll_init(&emul_stacks);
}

static void emul_stacks_add(vm_offset_t stack) 
{
	emul_stack_chain_t	stack_link;

	stack_link = (emul_stack_chain_t) malloc(sizeof(struct emul_stack_chain));
	stack_link->stack = stack & cthread_stack_mask;

	mutex_lock(&emul_stacks_lock);
	dll_enter_first(&emul_stacks, stack_link, emul_stack_chain_t,  stack_chain);
	mutex_unlock(&emul_stacks_lock);
}

/*
 * emul_stacks_reset: Zero and free the list of stacks and then recreate it
 *			containing only the given stack.
 */
static void emul_stacks_reset(vm_offset_t stack)
{
	emul_stack_chain_t	stack_link;

	/* waste the stack in the list */
	while (! dll_empty(&emul_stacks)) {
		dll_remove_first(&emul_stacks, stack_link, 
				emul_stack_chain_t, stack_chain);
		free(stack_link);
	}

	/* Rebuild it */
	mutex_init(&emul_stacks_lock);
	emul_stacks_add(stack);
}

boolean_t emul_stacks_member(vm_offset_t stack)
{
	emul_stack_chain_t	stack_link;
	vm_offset_t stack_base = stack & cthread_stack_mask;
	boolean_t		foundit;

	mutex_lock(&emul_stacks_lock);

	/* Find the stack in the list */
	stack_link  = (emul_stack_chain_t)dll_first(&emul_stacks);
	foundit = FALSE;
	while (! dll_end(&emul_stacks, (dll_entry_t)stack_link)) {
		if (stack_base == stack_link->stack) {
			foundit = TRUE;
			break;
		}
		stack_link = (emul_stack_chain_t)dll_next(&(stack_link->stack_chain));
	}

	/* Detect the "not in the list" failure */
	if (! foundit) {
		mutex_unlock(&emul_stacks_lock);
		return(FALSE);
	}

	/* Take it from the list and put it in the front */
	dll_remove(&emul_stacks, stack_link, emul_stack_chain_t,  stack_chain);
	dll_enter_first(&emul_stacks, stack_link, emul_stack_chain_t,  stack_chain);
	mutex_unlock(&emul_stacks_lock);
	return(TRUE);	
}

/*
 *	emul_stack_alloc:  allocate a cthread-safe stack.  this routine
 *	knows a LOT more about cthreads than it ought to.
 *
 * 	note:  this routine depends on creating a cproc structure with
 *	a NULL thread_port.  so the emulation library can't use cthreads
 *	facilites that require this port to be valid.
 */
free_stack_t
emul_stack_alloc(void)
{
	ur_cthread_t		ur;
	free_stack_t			stack;

	ur = cthread_create_phantom();

	stack = (free_stack_t)(cproc_stack(ur)-sizeof(vm_offset_t));
	stack->next = 0;

	if (NULL == emul_first_cthread_stack) {
		emul_first_cthread_stack = stack;
	}
        emul_stacks_add((vm_offset_t)(stack));
	return stack;
}

/*
 *	emul_stack_init:  setup the initial emulation stack.
 *
 *	This routine is similar to emul_stack_alloc(), except that
 *	it uses the current stack.
 */
free_stack_t
emul_stack_init()
{
	ur_cthread_t		ur;
	free_stack_t		stack;

	ur = ur_cthread_self();

	stack = (free_stack_t)(cproc_stack(ur)-sizeof(vm_offset_t));
	stack->next = 0;

	if (NULL == emul_first_cthread_stack) {
		emul_first_cthread_stack = stack;
	}

	emul_stacks_init();
        emul_stacks_add((vm_offset_t)(stack));
	return stack;
}

/*
 *	emul_stack_init:  setup the initial emulation stack.
 *
 *	This routine is used to clean up lists of emul_stacks
 *	after forking.  It is assumed that all of the stacks have
 *	been wasted by the cthread_fork_child call and we just need
 *	to clean up the emulation lib records of them.
 *	emul_first_cthread_stack still remains and is believed to be the
 *	current stack.
 */
void emul_stack_fork_child()
{

	/* Reset the free list */
	emul_stack_free_list = 0;  /* Forget saved list of free emul stacks */
		
	/* Reset the list of stacks */
	emul_stacks_reset((vm_offset_t)emul_first_cthread_stack);
}

mach_error_t emul_panic(intr_cthread_id_t, mach_error_t, int, int);

static boolean_t	emul_init = FALSE;

/*
 * Global variable for the controlling terminal.
 */
char			emul_init_tty_name[40];
int			exec_debug_level;
/*
 * Called once only at emulation library initialization time.
 */
mach_error_t emul_initialize(ns_identity_t as_ident,ns_token_t as_token,
			int debug_level,int exec_debug_level_val,
			int statistics_level,
			int emul_mask,char* tty_name)
{
	mach_error_t		err = ERR_SUCCESS;
	int        		i;
	extern char		version[];
	
	void      _init_user_proxies(void);
	void      _print_map(void);
	mach_error_t emul_socket_init();
	diag* diag_obj;

	if (emul_init) return err;

	exec_debug_level = exec_debug_level_val;

	mach_init();	/* Get standard mach port set */

	if (exec_debug_level > 100)
		task_suspend(mach_task_self());

	cthread_init();
	emul_stack_free_list = emul_stack_init();

	/* stop real early if we want to */
	if (exec_debug_level > 99)
		task_suspend(mach_task_self());

	emul_statistics = statistics_level;
	__main();
	_init_user_proxies();
//	_print_map();

#if MACH3_UNIX || MACH3_VUS
	{
		syscall_val_t	rv;
		htg_getpid(&rv);
		open_debug_connection(rv.rv_val1);
	}

#else MACH3_UNIX || MACH3_VUS
	open_debug_connection(0);

#endif MACH3_UNIX || MACH3_VUS

	set_diag_level(debug_level);
	emul_debug = TRUE;
	syscall_trace = TRUE;

	err = find_initial_user_stack();
	if (err) goto finish;

	strcpy(emul_init_tty_name, tty_name);

	clone_master_obj = new clone_master;
//	clone_master_obj->list_add(diag_obj);


	DEBUG0(emul_debug,(Diag,"emul_initialize\n"));
	INFO((Diag, "%s\n", version));	/* Display emulation library version */

	prefix_obj = 0;
	if ( err = emul_init_namespace(as_ident, as_token) )
		goto finish;

#if	ComplexIO
	emul_io_init();

	if (err = emul_init_io()) 
		goto finish;
#if	Sockets
	if ( err = emul_socket_init() )
		/* goto finish -- Ignore socket problems for now */;
#endif	Sockets

#endif    ComplexIO

#if	Signals
	emul_init_signals();
#endif	Signals

#if	TaskMaster
	/* some implementations depend on signal objects */
	if ( err = emul_init_task_master(as_token) )
		goto finish;
#endif	TaskMaster

#if	ComplexIO
	if (err = emul_init_more_io()) 
		/* goto finish */;
#endif	ComplexIO

#if MACH3_US
	/* Register debugging with the correct id */
	{
		tm_task_id_t	id = 0;
		if (ERR_SUCCESS != tm_task_obj->tm_get_task_id(&id)) {
			DEBUG0(emul_debug,(Diag,"Failed to get initial taskid\n"));
			drop_dead();
		}
		open_debug_connection(id);
	}
#endif MACH3_US

	/* 
	 * Set emulation lib "panic" routine.  Called when synchroniously
	 * generated interrupts (mach exceptions) occure within emul lib.
	 */
	intr_set_sync_default(emul_panic);
	
	DEBUG2(emul_debug,(Diag,"emul_initialize: trap syscalls\n"));
	emul_setup();

	DEBUG1(emul_debug,(Diag,"emul_initialize done\n"));

	emul_init = TRUE;	/* Set only after initialization is done */

	if (exec_debug_level != 0) {
		if (err == ERR_SUCCESS)
			emul_exec_debug(TRUE,exec_debug_level);
	}

	return(0);
	
    finish:
	ERROR((Diag,"emul_initialize: Giving up due to error %s (%x)\n",
		mach_error_string(err), err));
	drop_dead();
}


/*
 * Exit immediately without cleanup.
 */
void drop_dead(void)
{
    DEBUG1(TRUE,(0,"Drop_dead\n"));
#if MACH3_UNIX
    syscall_val_t rv;
    htg_getpid(&rv);
    htg_kill(rv.rv_val1, 9, &rv);
#else	MACH3_UNIX

	/* Must happen before we let go of the diag object. */
	if (emul_statistics > 0) {
		INFO((Diag,"MachObject Statistics on process exit():\n"));
//		mach_object_dump_statistics();
	}

#if (NOT_SOLVED_BY_NO_MORE_SENDERS)
	mach_object_dereference(us_diag_object);
#endif (NOT_SOLVED_BY_NO_MORE_SENDERS)

    task_terminate(mach_task_self());
#endif	MACH3_UNIX
}


mach_error_t
emul_pre_fork(task_t child_task)
{
	DEBUG1(emul_debug,(Diag,"emul_pre_fork called\n"));
	return clone_master_obj->clone_init(child_task);
}

/*
 * Called by fork emulation to reinitialize emulation library.
 */
mach_error_t
emul_post_fork(void)
{
	mach_error_t		err;

	/* clear now invalid cached host port */
	emul_host_port_cache = MACH_PORT_NULL;

	/* reinit mach object port cache */
//	port_cache_reset();
	rpcmgr::GLOBAL->clone_complete();
	
	/* reinit the interupt mechinism */
	intr_reset();
	
	DEBUG1(emul_debug,(Diag,"emul_post_fork called\n"));

	err = clone_master_obj->clone_complete();

	DEBUG1(emul_debug,(Diag,"emul_post_fork done\n"));
	return err;
}

mach_error_t
emul_cleanup_on_exit(void)
{
#if (NOT_SOLVED_BY_NO_MORE_SENDERS)
	mach_object_dereference(clone_master);

	mach_object_dereference(ftab_obj);
	mach_object_dereference(prefix_obj);
	mach_object_dereference(uxident_obj);
	mach_object_dereference(uxstat_obj);
	mach_object_dereference(uxsignal_obj);

	mach_object_dereference(tm_obj);
	mach_object_dereference(tm_task_obj);
	mach_object_dereference(tm_parent_obj);
#endif (NOT_SOLVED_BY_NO_MORE_SENDERS)
}

mach_port_t emul_host_port()
{
	if (emul_host_port_cache == MACH_PORT_NULL) {
		emul_host_port_cache = mach_host_self();
	}

	return(emul_host_port_cache);
}

mach_error_t
emul_panic(intr_cthread_id_t intr_cthread_id, mach_error_t err,
	   int code, int subcode)
{
	printf("PANIC: emulation error: cthread_id = %d, err = %d, code = %d, subcode = %d\n", intr_cthread_id, err, code, subcode);
	
	if (emul_debug) {
		printf("PANIC: stopping for debugging");
		task_suspend(mach_task_self());
	}
	drop_dead();
}


/*
 * Splits the given path name into directory and file names, 
 * verifying that each component is less than leafsize and the 
 * directory name is less than dirsize in the process.
 */
mach_error_t
emul_path(char* path, char* dir, int dirsize, char* leaf, int leafsize)
{
	register char	*cp, *dp, *lp;
	register int	i, dsize, lsize;

	cp = path;
	dp = dir;
	lp = leaf;
	dsize = lsize = 0;

	while (*cp != '\0') {
		while (*cp != '/' && *cp != '\0')	/* component */
			/*
			 * component must be smaller than leafsize
			 */
			if (lsize + 1 < leafsize) {
				*lp++ = *cp++;
				lsize++;
			}
			else {
				return(unix_err(ENAMETOOLONG));
			}
		if (*cp == '/') {			/* slash */
			cp++;
			if (lsize) {
				/*
				 * add to directory name
				 */
				if (dsize && dp[-1] != '/') {
					/*
					 * append slash
					 */
					if (dsize + 1 < dirsize) {
						*dp++ = '/';
						dsize++;
					}
					else
						return(-1);
				}
				if (lsize + dsize < dirsize) {
					/*
					 * append next component
					 */
					for (i = 0; i < lsize; i++)
						*dp++ = leaf[i];
					dsize += lsize;
					lp = leaf;
					lsize = 0;
				}
				else
					return(unix_err(ENAMETOOLONG));
			}
			else if (dsize == 0) {		/* beginning slash */
				*dp++ = '/';
				dsize++;
			}
		}
	}

	*dp = '\0';
	*lp = '\0';

	/*
	 * A null directory or file name is returned as ".".
	 */
	if (dir[0] == '\0') {
		dir[0] = '.';
		dir[1] = '\0';
	}
	if (leaf[0] == '\0') {
		leaf[0] = '.';
		leaf[1] = '\0';
	}
	return(ERR_SUCCESS);
}
