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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/emul/emul_exec.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: BSD exec() emulation.
 *
 * Portions of the code in this file were copied from the equivalent module
 * in POE, containing the following entries:
 *	Author:	Joseph S. Barrera III, Randall W. Dean
 *	Copyright (c) 1990 Joseph S. Barrera III, Randall W. Dean
 *
 * Some other portions come from the "BSD single-server" emulation library.
 *	(no explicit author)
 *
 * HISTORY
 * $Log:	emul_exec.cc,v $
 * Revision 2.7  94/10/27  12:01:24  jms
 * 	Copy the exec string into the "shared_info" space, shared between the
 * 	emulated processes and the task_master.
 * 	[94/10/26  14:41:14  jms]
 * 
 * Revision 2.6  94/07/08  16:56:52  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.5  92/07/05  23:24:53  dpj
 * 	No changes.
 * 	[92/06/29  22:44:00  dpj]
 * 
 * 	Eliminated diag_format().
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:27:45  dpj]
 * 
 * 	Skip spaces after '#!".
 * 	[92/03/12  13:45:35  dpj]
 * 
 * Revision 2.4  92/03/05  14:55:35  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:11:32  jms]
 * 
 * Revision 2.3  91/12/20  17:43:13  jms
 * 	Increase INITIAL_BRK_SIZE 0x80000 => 0x200000 (From dpj)
 * 	[91/12/20  14:27:37  jms]
 * 
 * Revision 2.2  91/11/13  16:37:37  dpj
 * 	Added support for shell scripts (#!).
 * 	[91/11/12  18:02:03  dpj]
 * 
 * 	First working version.
 * 	[91/11/12  17:45:07  dpj]
 * 
 */

#include <base.h>

extern "C" {
#include <mach.h>
}

#include <us_name_ifc.h>
#include <loader_info.h>
#include <loader.h>
#include <tm_types.h>

#include <errno.h>

#include "emul_base.h"
#include "emul_proc.h"

#define	NBPW		sizeof(int)	/* XXX */


/*
 * UNIX brk emulation.
 * The actual work is done in emul_basic.
 */
#define	INITIAL_BRK_SIZE		((vm_size_t)0x200000)
extern mach_error_t emul_init_brk(vm_address_t, vm_address_t);

extern mach_error_t clean_for_exec();
extern vm_offset_t set_arg_addr(vm_size_t);
extern mach_error_t reset_user_stack();


/*
 * Debugging control.
 */
boolean_t	exec_debug = FALSE;
int		exec_count = 0;


/*
 *  emul_exec_debug: suspend processes for debugging,
 *  starting with the "count"th process.
 */
emul_exec_debug(boolean_t onoff,int count) 
{
	DEBUG0(1,(Diag,"emul_exec_debug: %s count=%d\n",
			  onoff?"on":"off",
			  count));
	exec_debug = onoff;
	exec_count = count - 1;
	return(0);
}


/*
 * Copy zero-terminated string and return its length,
 * including the trailing zero.  If longer than max_len,
 * return -1.
 */
/*
 * Copy zero-terminated string and return its length,
 * including the trailing zero.  If longer than max_len,
 * return -1.
 */
int copystr(
	register char	*from,
	register char	*to,
	register int	max_len)
{
	register int	count;

	count = 0;
       	while (count < max_len) {
	    count++;
	    if ((*to++ = *from++) == 0) {
		return (count);
	    }
	}
	return (-1);
}


mach_error_t copy_args_flat(
	char	**argp,
	char	*buff,
	int	buff_size)
{
	char		**app = argp;
	char		*ap;
	char		*cp = buff;
	int		cc = 0;
	int		len;

	while ((ap = *app++) != 0) {
	    if (0 != cc) {
		/* Put the space after the previous arg */
		cp[-1] = ' ';
	    }

	    if (0 > (len = copystr(ap, cp, buff_size - cc))) {
			return(ERR_SUCCESS);
	    }
	    cc += len;
	    cp += len;
	}
	return(ERR_SUCCESS);
}

mach_error_t copy_args(
	register char	**argp,
	int		*arg_count,	/* OUT */
	vm_offset_t	*arg_addr,	/* IN/OUT */
	vm_size_t	*arg_size,	/* IN/OUT */
	unsigned int	*char_count)	/* IN/OUT */
{
	register char		*ap;
	register int		len;
	register unsigned int	cc = *char_count;
	register char		*cp = (char *)*arg_addr + cc;
	register int		na = 0;

	while ((ap = *argp++) != 0) {
	    na++;
	    while ((len = copystr(ap, cp, *arg_size - cc)) < 0) {
		/*
		 * Allocate more
		 */
		vm_offset_t	new_arg_addr = 0;

		if (vm_allocate(mach_task_self(),
				&new_arg_addr,
				(*arg_size) * 2,
				TRUE) != KERN_SUCCESS)
		    return(unix_err(E2BIG));
		(void) vm_copy(mach_task_self(),
				*arg_addr,
				*arg_size,
				new_arg_addr);
		(void) vm_deallocate(mach_task_self(),
				*arg_addr,
				*arg_size);
		*arg_addr = new_arg_addr;
		*arg_size *= 2;

		cp = (char *)*arg_addr + cc;
	    }
	    cc += len;
	    cp += len;
	}

	*arg_count = na;
	*char_count = cc;
	return (ERR_SUCCESS);
}

extern "C" mach_error_t emul_exec_call(
	char		*fname,
	char		**argp,
	char		**envp,
	vm_offset_t	*new_arg_addr,	/* OUT */
	vm_offset_t	*entry,		/* pointer to OUT array */
	unsigned int	*entry_count)	/* OUT */
{
	mach_error_t	err = ERR_SUCCESS;
	usItem*		a_obj = NULL;
	usByteIO*	exec_obj = NULL;
	unsigned int	exec_access = NSR_READ | NSR_EXECUTE | NSR_GETATTR;
	ns_type_t	exec_type;
	struct loader_info		li;
	vm_offset_t	arg_addr = 0;
	vm_size_t	arg_size;
	int		arg_count, env_count;
	unsigned int	char_count = 0;
	int		error;
	vm_offset_t	arg_start;
	char		cfname[256];
	char		cfarg[256];
	ns_path_t	save_fname;
	vm_address_t	new_brk;

	SyscallTrace("execve");
	
	DEBUG0(exec_debug,(Diag,"execve (%s) \n", fname));
	
	if (exec_count < -100)
		task_suspend(mach_task_self());

	/*
	 * Find the executable.
	 *
	 * XXX Need to detect set-uid execs().
	 */
	err = prefix_obj->ns_resolve_fully(fname,NSF_FOLLOW_ALL,
					exec_access,&a_obj,&exec_type,NULL);
	if (err) goto out;	/* XXX set-uid ... */
	if ((exec_type != NST_FILE) || 
			((exec_obj = usByteIO::castdown(a_obj)) == 0)) {
		err = unix_err(EACCES);	/* XXX is that the right code ? */
		goto out;
	}
	a_obj = NULL;
	err = loader_ex_get_header(exec_obj,&li);
	if (err == unix_err(ENOEXEC)) {
		/*
		 * Look for a shell script.
		 */

		char			cmdbuf[256];
		io_offset_t		offset;
		unsigned int		len;
		char*			cp;
		char*			ap;

		/*
		 * Read the first line of the script.
		 */
		UINT_TO_IO_OFF(0,&offset);
		len = sizeof(cmdbuf) - 1;
		err = exec_obj->io_read(IOM_TRUNCATE,offset,
						(pointer_t)cmdbuf,&len);
		if (err) goto out;
		cmdbuf[len] = '\0';

		/*
		 * Check the "magic" string.
		 */
		if ((cmdbuf[0] != '#') || (cmdbuf[1] != '!')) {
			err = unix_err(ENOEXEC);
			goto out;
		}

		/*
		 * Construct the real executable name and the argument.
		 */
		cp = &cmdbuf[2];
		ap = &cfname[0];
		while ((*cp == ' ') || (*cp == '\t')) cp++;
		while ((*cp != '\n') && (*cp != ' ') &&
				(*cp != '\t') && (*cp != '\0')) {
			*ap++ = *cp++;
		}
		*ap++ = '\0';
		ap = &cfarg[0];
		while ((*cp == ' ') || (*cp == '\t')) cp++;
		while ((*cp != '\n') && (*cp != '\0')) {
			*ap++ = *cp++;
		}
		*ap++ = '\0';

		/*
		 * Find the real executable.
		 */
		mach_object_dereference(exec_obj);
		err = prefix_obj->ns_resolve_fully(cfname,NSF_FOLLOW_ALL,
					exec_access,&a_obj,&exec_type,NULL);
		if (err) goto out;	/* XXX set-uid ... */
		if ((exec_type != NST_FILE) || 
			((exec_obj = usByteIO::castdown(a_obj)) == 0)) {
			err = unix_err(EACCES);	/* XXX */
			goto out;
		}
		a_obj = NULL;
		err = loader_ex_get_header(exec_obj,&li);
	} else {
		cfname[0] = '\0';
		cfarg[0] = '\0';
	}

	if (err) goto out;

	/*
	 * At this point, we are fairly sure that we have a valid executable.
	 *
	 * Save all the arguments and environment before destroying
	 * our user address space.
	 */

	if (exec_debug && exec_count >= 0)
		exec_count--;

	/*
	 * Copy the argument and environment strings into
	 * contiguous memory.  Since most argument lists are
	 * small, we allocate a page to start, and add more
	 * if we need it.
	 */
	arg_size = vm_page_size;
	(void) vm_allocate(mach_task_self(),
			   &arg_addr,
			   arg_size,
			   TRUE);

	if (argp) {

	    /* Copy exec string to shared space for "ps" functionallity */
	    copy_args_flat(argp, shared_info->exec_string, SHARED_EXEC_STR_MAX);
	    shared_info->touch = shared_info->id + 1000;
	    /* Copy args for next exec */
	    if (copy_args(argp, &arg_count,
			&arg_addr, &arg_size, &char_count) != 0)
		return(unix_err(E2BIG));
	}
	else {
	    arg_count = 0;
	}

	if (envp) {
	    if (copy_args(envp, &env_count,
			&arg_addr, &arg_size, &char_count) != 0)
		return(unix_err(E2BIG));
	}
	else {
	    env_count = 0;
	}

	/*
	 * Save the file name in case a command file needs it.
	 * (The file name is in the old program address space,
	 * and will disappear if the exec is successful.)
	 */
	strcpy(save_fname, fname);

	/*
	 * Clean our address space and load the new executable.
	 *
	 * XXX Make sure that we don't have other threads beside
	 * the "primary" happily running around in our user space
	 * right now.
	 *
	 * After this, we have lost our old executable, so we cannot
	 * gracefully handle errors.
	 *
	 * XXX Just return for now in case of error, and let the process
	 * crash horribly. Should die more gracefully?
	 */
	err = clean_for_exec();
	if (err) goto out;
	err = loader_load_program_file(exec_obj,&li,&new_brk);
	if (err) goto out;

	/*
	 * Create a new user stack.
	 */
	err = reset_user_stack();
	if (err) goto out;

	/*
	 * Set up new argument list.  If command file name and argument
	 * have been found, use them instead of argv[0].
	 */
	{
	    register char	**ap;
	    register char	*cp;
	    register char	*argstrings = (char *)arg_addr;
	    register int	total_args;
	    register int	len;
	    char		*cmd_args[4];
	    register char	**xargp = 0;

	    total_args = arg_count + env_count;
	    if (cfname[0] != '\0') {
		/*
		 * argv[0] becomes 'cfname'; skip real argv[0].
		 */
		len = strlen(argstrings) + 1;
		argstrings += len;
		char_count -= len;

		xargp = cmd_args;
		*xargp++ = cfname;
		char_count += (strlen(cfname) + 1);

		if (cfarg[0] != '\0') {
		    *xargp++ = cfarg;
		    char_count += (strlen(cfarg) + 1);
		    total_args++;
		}
		*xargp++ = save_fname;
		char_count += (strlen(save_fname) + 1);
		total_args++;

		*xargp = 0;
		xargp = cmd_args;
	    }
	    char_count = (char_count + NBPW - 1) & ~(NBPW - 1);

	    arg_start = set_arg_addr(total_args*NBPW + 3*NBPW + 
							char_count + NBPW);

	    ap = (char **)arg_start;
	    cp = (char *)arg_start + total_args*NBPW + 3*NBPW;

	    *ap++ = (char *)(total_args - env_count);
	    for (;;) {

		if (total_args == env_count)
		    *ap++ = 0;
		if (--total_args < 0)
		    break;
		*ap++ = cp;
		if (xargp && *xargp)
		    len = copystr(*xargp++, cp, (unsigned)char_count);
		else {
		    len = copystr(argstrings, cp, (unsigned)char_count);
		    argstrings += len;
		}
		cp += len;
		char_count -= len;
	    }
	    *ap = 0;
	}

#ifdef	STACK_GROWTH_UP
	*new_arg_addr = ((vm_offset_t) cp + NBPW - 1) & ~(NBPW - 1);
#else	STACK_GROWTH_UP
	*new_arg_addr = arg_start;
#endif	STACK_GROWTH_UP

	/*
	 * Reset emulation library state.
	 */

	/*
	 * Set-up brk emulation.
	 *
	 * XXX pre-allocate a large chunk of memory for obreak() XXX
	 * Need to do this because random droppings like out of line
	 * messages can end up landing right where obreak() wants
	 * to extend to.
	 */
	err = vm_allocate(mach_task_self(), &new_brk, INITIAL_BRK_SIZE, FALSE);
	if (err) goto out;
	err = emul_init_brk(new_brk,new_brk + INITIAL_BRK_SIZE);
	if (err) goto out;

#if	ComplexIO
	(void)emul_io_exec();
#endif	ComplexIO

#if	Signals
	/*
	 * XXX delete errant threads
	 * reset the signal vector and register the primary thread.
	 */
	(void)signal_reset(uxsignal_obj, mach_thread_self());
#endif	Signals

#if	MULTI_THREADED_EXEC
	/*
	 * XXX Start up any non-primary threads
	 * specified in a non a.out file. (NOT IMPLEMENTED)
	 */
#endif	MULTI_THREADED_EXEC

	/*
	 * Find the program entry point.
	 */
	err = loader_set_entry_address(&li,(int*)entry,entry_count);
	if (err) goto out;

	if (exec_count < 0)
		task_suspend(mach_task_self());

out:
	if (arg_addr)
		(void) vm_deallocate(mach_task_self(), arg_addr, arg_size);

	/*
	 * Release the file object for the executable.
	 *
	 * The file itself will not go away, since it is mapped
	 * in our address space. 
	 *
	 * XXX Keep a proper reference with appropriate cloning instead?
	 */	 
	mach_object_dereference(exec_obj);

	if (a_obj)
		mach_object_dereference(a_obj);
	return (err);
}

