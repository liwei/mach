/*
 **********************************************************************
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
 **********************************************************************
 */
/*
 * tty_name.h
 *
 * Temporary tty naming module data structure definitions.
 *
 * Michael B. Jones  --  19-Oct-1988
 */

/*
 * HISTORY: 
 * $Log:	tty_name.h,v $
 * Revision 2.4  94/07/21  16:15:00  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  90/10/02  11:34:25  mbj
 * 	Made it work under any of MACH3_{UNIX,VUS,US} configurations.
 * 	Moved NTTYS computation here from tty_name.c.
 * 	[90/10/01  15:34:53  mbj]
 * 
 * Revision 2.2  90/09/05  09:46:05  mbj
 * 	Simplified name handling to distinguish between externally visible tty
 * 	names and internal (htg) names.  Moved here from lib/us.
 * 	[90/09/04  15:27:28  mbj]
 * 
 * Revision 1.3  89/05/17  16:48:46  dorr
 * 	include file cataclysm
 * 
 * Revision 1.2  88/11/01  14:22:11  mbj
 * Adapt for use by tty_io_mgr class.
 * 
 * Revision 1.1  88/10/28  01:13:20  mbj
 * Initial revision
 * 
 * 19-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

#include "tty_features.h"
#include <sys/types.h>

/*
 * NPURETTYS:  Number of ttys supplied directly by the pure kernel
 */
#if	! MACH3_UNIX
#define	NPURETTYS	NPURE
#else	MACH3_UNIX
#define	NPURETTYS	0
#endif	MACH3_UNIX

/*
 * NHTGTTYS:  Number of ttys for which we talk to the underlying (htg) ttys
 */
#if	! MACH3_US
#define	NHTGTTYS \
	1		/* /dev/console_htg */ + \
	NSERIAL		/* /dev/tty0?_htg */ + \
	(2 * NPTY) 	/* /dev/[tp]ty[p-][0-9a-f]_htg */ + \
	(2 *NCMUPTY)	/* /dev/[tp]ty[P-][0-9a-f]_htg */
#else	MACH3_US
#define	NHTGTTYS 0
#endif	MACH3_US

/*
 * NTTYS:  Total number of ttys supported by configuration
 */
#define NTTYS \
	NPURETTYS	/* /dev/console, /dev/tty0[0-9] */ + \
	(2 * NPTY)	/* /dev/[tp]ty[p-][0-9a-f] */ +	/* Emulated ptys */ \
	(2 * NCMUPTY)	/* /dev/[tp]ty[P-][0-9a-f] */ +	/* Emul cmu ptys */ \
	NHTGTTYS	/* /dev/*_htg */	/* Underlying htg ttys */

typedef struct {
	char	*name;
	char	*int_name;	/* For htg or pure kernel name strings */
	dev_t	cdev_num;	/* Entry in cdevsw */
	dev_t	rdev_num;	/* Entry in file system */
} tty_name;

extern tty_name *tty_name_lookup();
extern tty_name *tty_lookup();
