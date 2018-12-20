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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_tty.cc,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxio_tty.cc,v $
 * Revision 2.4  94/07/08  16:02:13  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  94/01/11  17:50:56  jms
 * 	Use new usTTY_proxy definition.
 * 	[94/01/09  19:52:05  jms]
 * 
 * Revision 2.2  91/11/06  14:12:41  jms
 * 	Make TTYs sequential files "ux_set_sequential_internal"
 * 	[91/09/17  14:13:50  jms]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:11:07  pjg]
 * 
 *
 * Revision 2.1.1.2  91/09/26  20:19:54  pjg
 * 	Upgraded to US41
 * 
 * Revision 2.1.1.1  91/04/15  10:11:07  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.3  90/12/10  09:50:15  jms
 * 	Overrided superclass' ux_map implementation.
 * 	[90/11/20  14:25:12  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:40  jms]
 * 
 * Revision 2.2  90/11/10  00:39:00  dpj
 * 	Subclass of uxio representing ttys.
 * 	[90/10/17  13:21:39  neves]
 * 
 *
 */

#include <uxio_tty_ifc.h>
#include <us_tty_proxy_ifc.h>

extern "C" {
#include <base.h>
#include <debug.h>

#include <dlong.h>

/*
#include <io_types.h>
#include <tty_methods.h>
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
}

#define BASE uxio
DEFINE_LOCAL_CLASS(uxio_tty)

static struct dlong_t dlong_zero = { 0, };

extern int			emul_debug;


/*
 *	define the methods for the tty file descriptor object
 */


uxio_tty::uxio_tty()
{
	ux_set_sequential_internal();
}

mach_error_t
uxio_tty::ux_lseek(long int *pos, unsigned int mode)
{
        return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
uxio_tty::ux_ioctl(int cmd, int *arg)
{
	mach_error_t		err = ERR_SUCCESS;
	char			ioctlbuf[128];
	int			len;


	switch( cmd ) {
/*
	    case TIOCGSIZE:
	    case TIOCSSIZE:
*/
	    case TIOCGETD:
	    case TIOCSETD:
	    case TIOCHPCL:
	    case TIOCMODG:
	    case TIOCMODS:
	    case TIOCGETP:
	    case TIOCSETP:
	    case TIOCSETN:
	    case TIOCEXCL:
	    case TIOCNXCL:
	    case TIOCFLUSH:
	    case TIOCSETC:
	    case TIOCGETC:
		/* locals, from 127 down */
#if	CMUCS
	    case TIOCGLOC:
	    case TIOCCSET:
	    case TIOCCLOG:
	    case TIOCCHECK:
	    case TIOCATTACH:
	    case TIOCLOGINDEV:
	    case TIOCSLOC:
	    case TIOCVCONS:
	    case TIOCGCONS:
#endif	CMUCS
	    case TIOCLBIS:
	    case TIOCLBIC:
	    case TIOCLSET:
	    case TIOCLGET:
	    case TIOCSBRK:
	    case TIOCCBRK:
	    case TIOCSDTR:
	    case TIOCCDTR:
	    case TIOCGPGRP:
	    case TIOCSPGRP:
	    case TIOCSLTC:
	    case TIOCGLTC:
	    case TIOCOUTQ:
	    case TIOCSTI:
	    case TIOCNOTTY:
	    case TIOCPKT:
	    case TIOCSTOP:
	    case TIOCSTART:
	    case TIOCMSET:
	    case TIOCMBIS:
	    case TIOCMBIC:
	    case TIOCMGET:
	    case TIOCREMOTE:
		
	    case TIOCGWINSZ:
	    case TIOCSWINSZ:
	    case TIOCUCNTL:
		
#if	CMUCS
	    case TIOCCONS:
#endif	CMUCS
	    default:
		/*
		 * copy the data in...
		 */
		len = (cmd >> 16) & IOCPARM_MASK;
		if ((cmd & IOC_INOUT) && arg)
			bcopy(arg,ioctlbuf,len);

		DEBUG2(TRUE, (0, "%s::ux_ioctl: cmd=0x%0x, args=0x%0x, len=%d, cmd&IOC_INOUT=0x%0x\n",
			      class_name(), cmd, arg, len, cmd&IOC_INOUT));
		/*
		 * XXX C++ Must cast down the pointer to a tty object
		 */
		usTTY_proxy* t = usTTY_proxy::castdown(this->obj);
		if (t == 0) {
			DEBUG0(TRUE, (0, "uxio_tty::ux_ioctl\n"));
			err = MACH_OBJECT_NO_SUCH_OPERATION;
		}
		else {
			err = t->tty_bsd_ioctl(cmd, ioctlbuf);
		}
		/*
		 * copy the results out
		 */
		if ( (err == ERR_SUCCESS) && (cmd & IOC_OUT) )
			bcopy(ioctlbuf,arg,len);

		if (err == MACH_OBJECT_NO_SUCH_OPERATION 
		    && !(cmd & IOC_OUT)) 
			err = ERR_SUCCESS;
		break;
	}

	return err;
}

mach_error_t
uxio_tty::ux_map(task_t task, vm_address_t* addr, vm_size_t size,
		  vm_offset_t mask, boolean_t anywhere, 
		  vm_offset_t paging_offset,
		  boolean_t copy, vm_prot_t cprot, vm_prot_t mprot,
		  vm_inherit_t inherit)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}
