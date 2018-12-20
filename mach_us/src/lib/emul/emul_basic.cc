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
 * File:        emul_basic.cc
 *
 * Purpose:
 *	User space emulation of unix process management primitives
 *
 * HISTORY:
 * $Log:	emul_basic.cc,v $
 * Revision 2.5  94/07/08  16:56:41  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  92/07/05  23:24:28  dpj
 * 	No Change
 * 	[92/06/24  14:02:49  jms]
 * 
 * Revision 2.3  91/11/13  16:36:19  dpj
 * 	Removed references to libload.
 * 	Moved exec() code to emul_exec.c.
 * 	[91/11/08            dpj]
 * 
 * Revision 2.2  91/11/06  11:29:25  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:25:11  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:21:37  pjg]
 * 
 * 	initial checkin.
 * 	[89/12/18  15:41:51  dorr]
 * 
 * Revision 2.10  91/07/01  14:06:25  jms
 * 	Special code the measure the speed of the loader code,
 * 	under LOADERTEST.
 * 	[91/06/16  20:57:06  dpj]
 * 	No Further Change
 * 	[91/06/24  15:45:56  jms]
 * 
 * 	Bug fix, to many args to a vm_allocate call.
 * 	[91/05/07  11:21:42  jms]
 * 
 * Revision 2.9  91/05/05  19:24:12  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:49:58  dpj]
 * 
 * 	Follow transparent symlinks.
 * 	Fixed vm_allocate() arguments bug (from Paulo Guedes @ OSF).
 * 	Increased the debug level for obreak warning.
 * 	[91/04/28  09:37:56  dpj]
 * 
 * Revision 2.8  90/12/19  11:04:16  jjc
 * 	Merged up to US34.
 * 	[90/12/05            jjc]
 * 	Changed emul_exec_load() to pre-allocate a chunk of memory
 * 	for obreak().
 * 	[90/11/21            jjc]
 * 
 * Revision 2.7  90/12/10  09:49:09  jms
 * 	Fixed vm_allocate overlap problem in emul_obreak.
 * 	[90/11/19  18:06:34  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:37:50  jms]
 * 
 * Revision 2.6  90/11/27  18:17:44  jms
 * 	Include mach/machine/vm_param.h not mach/vm_param.h
 * 	[90/11/19  22:40:15  jms]
 * 
 * 	About to add tty changes from trunk
 * 	[90/11/08  13:38:49  jms]
 * 
 * Revision 2.5  90/09/07  13:43:06  mbj
 * 	Return meaningful error codes from emul_init_proc().
 * 	[90/09/07  11:14:22  mbj]
 * 
 * Revision 2.4  90/07/09  17:01:53  dorr
 * 	add signal_reset() after exec.
 * 	[90/03/01  14:34:07  dorr]
 * 
 * 	*_region -> region_*.  get rid of emul_reinit_proc.
 * 	change to exec for use with stack switch.
 * 	[90/02/23  14:40:50  dorr]
 * 	No Further Changes
 * 	[90/07/06  14:35:49  jms]
 * 
 * Revision 2.3  90/03/21  17:18:46  jms
 * 	Remove all Task Master reference
 * 	[90/03/16  16:16:36  jms]
 * 
 * Revision 2.2  90/01/02  21:38:10  dorr
 * 	initial revision
 * 
 * Revision 1.47  89/10/30  16:28:51  dpj
 * 	Merged with the mainline (1.46).
 * 	Fixed argument block allocation in emul_execve().
 * 	Fixed exec_debug technology to only suspend the process once.
 * 	[89/10/27  16:38:45  dpj]
 * 
 * Revision 1.46  89/07/19  11:30:14  dorr
 * 	fix getuid/geteuid. switch to dyn_object program
 * 	loading.  allocate brk area 2M at a crack.
 * 
 * Revision 1.43.1.3  89/07/11  12:56:06  dorr
 * 	fix getuid/geteuid.
 * 
 * Revision 1.43.1.2  89/07/11  10:28:14  dorr
 * 	fix brk to ask for 2 meg at a time.
 * 
 * Revision 1.43.1.1  89/07/10  15:42:32  dorr
 * 	convert to new executable loading technique.
 * 
 * Revision 1.45  89/07/09  14:17:40  dpj
 * 	Fixed a call to diag_format().
 * 	[89/07/08  12:37:51  dpj]
 * 
 * Revision 1.44  89/06/30  18:31:31  dpj
 * 	Added a diag statement to print the actual value of the
 * 	argument to emul_obreak() before rounding it to a page boundary.
 * 	[89/06/29  19:26:00  dpj]
 * 
 * Revision 1.43  89/06/05  17:17:53  dorr
 * 	in exec, add interlock to get rid of race in
 * 	exec.  
 * 	[89/06/05  16:56:33  dorr]
 * 
 * Revision 1.42  89/05/24  10:38:13  dorr
 * 	incorporate mbj changes.  add calls for setreuid & friends.
 * 	[89/05/15  12:06:00  dorr]
 * 
 * Revision 1.41  89/05/17  16:15:13  dorr
 * 	include file cataclysm
 * 
 * Revision 1.40  89/05/04  17:25:11  mbj
 * 	Began work on emulating wait/wait3, only to have to back out pending
 * 	kernel changes to htg_unix_syscall for wait3's non-standard parameter
 * 	passing mechanism.
 * 	[89/04/25  23:45:09  mbj]
 * 
 * 	Merge up to U3.
 * 	[89/04/17  15:14:43  mbj]
 * 
 * Revision 1.39  89/04/04  18:19:44  dorr
 * 	clean up memory leaks.  add list of regions in emulation library to
 * 	proc image of program about to be exec'd.  get rid of emul_initialize()
 * 	calls.
 * 	[89/04/02  20:45:13  dorr]
 * 
 * Revision 1.36.1.1  89/03/31  16:05:45  mbj
 * 	Create mbj_signal branch
 * 
 * Revision 1.33.1.3  89/03/30  16:49:47  mbj
 * 	New interface to tm_register_task.  Remove emul_kill and emul_killpg.
 * 	Some cleanup as well.
 * 
 * Revision 1.38  89/03/31  15:20:46  mbj
 * 	Support fork() of multi-threaded emulation library.
 * 	[89/03/30  15:58:02  mbj]
 * 
 * 	condense history.  move exec() code here from task master.
 * 	[89/03/31  10:24:02  dorr]
 * 
 */

#include <us_byteio_ifc.h>
#include <us_name_ifc.h>
#include <uxsignal_ifc.h>

#include "emul_base.h"
#include "emul_proc.h"

extern "C" {
#include <base.h>
#include <debug.h>

#include <cthreads.h>
#include <mach_init.h>
#include <mach/machine/vm_param.h>
#include <machine/machine_address_space.h>

#include <errno.h>

#include "us_ports.h"		/* Well-known port number definitions */
}

static vm_address_t	cur_brk = 0xdeadbeef;	/* emulated task's brk */
static vm_address_t	max_brk = 0xdeadbeef;

/*
 * emul_init_brk(): set-up the brk emulation.
 */
mach_error_t emul_init_brk(
	vm_address_t		cur,
	vm_address_t		max)
{
	cur_brk = cur;
	max_brk = max;

	return(ERR_SUCCESS);
}

emul_getpagesize(syscall_val_t* rv)
{
	rv->rv_val1 = 4096;	/* XXX unix default page size (might vary for some machine) */
	return(ERR_SUCCESS);
}

mach_error_t
emul_sbrk(unsigned int delta, syscall_val_t *rv)
{
    mach_error_t    	err;
    vm_address_t    	ocur_brk = cur_brk;

    SyscallTrace("sbrk");

    if ((err = emul_obreak( cur_brk + delta, rv)) == 0) {
	rv->rv_val1 = ocur_brk;
	return (KERN_SUCCESS);
    }

    return(err);
}

mach_error_t
emul_obreak(vm_address_t addr, syscall_val_t *rv)
{
	mach_error_t   		err;
	
	SyscallTrace("obreak");
	
	rv->rv_val1 =  0;
	
	if ( addr < max_brk ) {
		DEBUG1(1,(Diag,"obreak: noop for %#x < max_brk %#x\n",
							  addr, max_brk));
		if (addr > cur_brk)
			cur_brk = addr;

		err = ERR_SUCCESS;
	} else {
		static vm_size_t	twomeg = 1024*1024*2;
		vm_size_t		brk_size;

		/* extend max_brk to include the given break value */

		cur_brk = trunc_page(cur_brk);
		if (round_page(addr)-cur_brk > twomeg)
			brk_size = round_page(addr)-cur_brk;
		else
			brk_size = twomeg;

		while (TRUE) {

			DEBUG0(emul_debug, (0,"obreak: allocating %#x(%#x)\n", 
					    max_brk, brk_size));
			err = vm_allocate(mach_task_self(),
						&max_brk, brk_size, FALSE);

			if (err == ERR_SUCCESS) {
				max_brk += brk_size;
				cur_brk = addr;

				DEBUG0(emul_debug, (0,
			      "obreak: ok, cur=%#x max=%#x\n", addr, max_brk));

				break;
			} else {
				/* try a smaller value */
				brk_size >>= 1;

				if (brk_size < (round_page(addr)-cur_brk))
					break;
			}
		}

	}
	
	rv->rv_val1 = emul_error_to_unix(err);

	return( err );
}
