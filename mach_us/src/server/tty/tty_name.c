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
 * tty_name.c
 *
 * Temporary tty naming module.
 *
 * Michael B. Jones  --  19-Oct-1988
 */

/*
 * HISTORY:
 * $Log:	tty_name.c,v $
 * Revision 1.5  94/07/21  16:14:57  mrt
 * 	Updated copyright
 * 
 * Revision 1.4  90/10/02  11:38:25  mbj
 * 	Made it work under any of MACH3_{UNIX,VUS,US} configurations.
 * 	Moved ns_create code to tty_name_create.c
 * 	Moved NTTYS computation to tty_name.h.
 * 	[90/10/01  15:34:03  mbj]
 * 
 * Revision 1.3  90/09/05  09:46:00  mbj
 * 	Added ns naming code.  Enter ttys backed by the underlying 2.5 system
 * 	with "_htg" appended to their names (e.g. /dev/ttyp0_htg).  Enter
 * 	emulated ptys under normal names.  Dropped indirect /dev/tty support.
 * 	[90/09/04  15:25:42  mbj]
 * 
 * Revision 1.2  88/11/16  11:32:43  mbj
 * Build without -DKERNEL or multitudes of feature .h files.
 * 
 * Revision 1.1  88/10/28  01:28:59  mbj
 * Initial revision
 * 
 * 19-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/conf.h>
#include "tty_name.h"
#include "cdevsw.h"
#include "machine/devnum.h"

static char *hexdigits = "0123456789abcdef";

tty_name tty_names[NTTYS];

char *newstr();

void init_tty_names()
{
    register int i, entry;
    register int bsd_minor;
    char name[80];

    entry = 0;
    bsd_minor = 0;

#if	! MACH3_UNIX
    for (i = 0; i < NPURE; i++) {
	if (i == 0) {
	    tty_names[entry].name = newstr("console");
	} else {
	    sprintf(name, "tty%02d", i - 1);
	    tty_names[entry].name = newstr(name);
	}
	(void) cdev_name_string(makedev(CDEVSW_CONSOLE, i), name);
	tty_names[entry].int_name = newstr(name);
	tty_names[entry].cdev_num = makedev(CDEVSW_CONSOLE, i);
	tty_names[entry].rdev_num = makedev(DEVNUM_CONSOLE, i);
	entry++;
    }
#endif	! MACH3_UNIX

#if	! MACH3_US
    for (i = 0; i < 1; i++) {
	tty_names[entry].name = newstr("console_htg");
	tty_names[entry].int_name = newstr("/dev/console");
	tty_names[entry].cdev_num = makedev(CDEVSW_BSD, bsd_minor++);
	tty_names[entry].rdev_num = makedev(DEVNUM_CONSOLE, i);
	entry++;
    }

    for (i = 0; i < NSERIAL; i++) {
	sprintf(name, "tty%02d_htg", i);
	tty_names[entry].name = newstr(name);
	sprintf(name, "/dev/tty%02d", i);
	tty_names[entry].int_name = newstr(name);
	tty_names[entry].cdev_num = makedev(CDEVSW_BSD, bsd_minor++);
	tty_names[entry].rdev_num = makedev(DEVNUM_SERIAL, i);
	entry++;
    }

    for (i = 0; i < NPTY; i++) {
	sprintf(name, "tty%c%c_htg", 'p' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	sprintf(name, "/dev/tty%c%c", 'p' + i/16, hexdigits[i%16]);
	tty_names[entry].int_name = newstr(name);
	tty_names[entry].cdev_num = makedev(CDEVSW_BSD, bsd_minor++);
	tty_names[entry].rdev_num = makedev(DEVNUM_PTS, i);
	entry++;

	sprintf(name, "pty%c%c_htg", 'p' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	sprintf(name, "/dev/pty%c%c", 'p' + i/16, hexdigits[i%16]);
	tty_names[entry].int_name = newstr(name);
	tty_names[entry].cdev_num = makedev(CDEVSW_BSD, bsd_minor++);
	tty_names[entry].rdev_num = makedev(DEVNUM_PTC, i);
	entry++;
    }

    for (i = 0; i < NCMUPTY; i++) {
	sprintf(name, "tty%c%c_htg", 'P' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	sprintf(name, "/dev/tty%c%c", 'P' + i/16, hexdigits[i%16]);
	tty_names[entry].int_name = newstr(name);
	tty_names[entry].cdev_num = makedev(CDEVSW_BSD, bsd_minor++);
	tty_names[entry].rdev_num = makedev(DEVNUM_CMUPTY, i);
	entry++;

	sprintf(name, "pty%c%c_htg", 'P' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	sprintf(name, "/dev/pty%c%c", 'P' + i/16, hexdigits[i%16]);
	tty_names[entry].int_name = newstr(name);
	tty_names[entry].cdev_num = makedev(CDEVSW_BSD, bsd_minor++);
	tty_names[entry].rdev_num = makedev(DEVNUM_CMUPTYC, i);
	entry++;
    }
#endif	! MACH3_US

    for (i = 0; i < NPTY; i++) {
	sprintf(name, "tty%c%c", 'p' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	tty_names[entry].int_name = "None";
	tty_names[entry].cdev_num = makedev(CDEVSW_PTS, i);
	tty_names[entry].rdev_num = makedev(DEVNUM_PTS, i);
	entry++;

	sprintf(name, "pty%c%c", 'p' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	tty_names[entry].int_name = "None";
	tty_names[entry].cdev_num = makedev(CDEVSW_PTC, i);
	tty_names[entry].rdev_num = makedev(DEVNUM_PTC, i);
	entry++;
    }

    for (i = 0; i < NCMUPTY; i++) {
	sprintf(name, "tty%c%c", 'P' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	tty_names[entry].int_name = "None";
	tty_names[entry].cdev_num = makedev(CDEVSW_CMUPTY, i);
	tty_names[entry].rdev_num = makedev(DEVNUM_CMUPTY, i);
	entry++;

	sprintf(name, "pty%c%c", 'P' + i/16, hexdigits[i%16]);
	tty_names[entry].name = newstr(name);
	tty_names[entry].int_name = "None";
	tty_names[entry].cdev_num = makedev(CDEVSW_CMUPTYC, i);
	tty_names[entry].rdev_num = makedev(DEVNUM_CMUPTYC, i);
	entry++;
    }
}

tty_name *tty_name_lookup(name) char *name;
{
    register int i;

    for (i = 0; i < NTTYS; i++) {
	if (strcmp(name, tty_names[i].name) == 0) return &tty_names[i];
    }
    return 0;
}

tty_name *tty_lookup(dev) dev_t dev;
{
    register int i;

    for (i = 0; i < NTTYS; i++) {
	if (dev == tty_names[i].cdev_num) return &tty_names[i];
    }
    return 0;
}

char *tty_dev_name(dev) dev_t dev;
/*
 * Translate from an opened dev number to the corresponding bsd tty name.
 */
{
    register tty_name *namep;

    if (! (namep = tty_lookup(dev))) return 0;
    return namep->int_name;
}
