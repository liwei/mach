/*
 * bsd_tty.c
 *
 * Dummy terminal driver which uses bsd ttys in raw mode for actual i/o.
 *
 * Michael B. Jones  --  4-Oct-1988
 */

/*
 * HISTORY:
 * $Log:	bsd_tty.c,v $
 * Revision 1.14  92/07/05  23:36:06  dpj
 * 	Eliminated diag_format().
 * 	[92/05/10  01:32:11  dpj]
 * 
 * Revision 1.13  92/03/05  15:14:28  jms
 * 	mach_types.h -> mach/mach_types.h
 * 
 * Revision 1.12  90/11/27  18:22:22  jms
 * 	Get unix select_ttypes.h for MACH3_US not !MACH3_UNIX.
 * 	[90/11/20  15:06:26  jms]
 * 
 * Revision 1.11  90/10/02  11:36:32  mbj
 * 	Make it work under MACH3_VUS.
 * 	[90/10/01  14:57:51  mbj]
 * 
 * 	Added bsd_tty_find_tty and other tweaks to make it work again with
 * 	the newer single-server-derived tty driver files.
 * 	[90/09/12  13:18:35  mbj]
 * 
 * Revision 1.10  90/09/05  09:45:25  mbj
 * 	Simplified name handling to distinguish between externally visible tty
 * 	names and internal (htg) names.
 * 	[90/09/04  15:16:46  mbj]
 * 
 * Revision 1.9  90/08/14  22:03:22  mbj
 * 	Fix longstanding bug which forgot to simulate the transmit interrupt
 * 	after writes.  Also changed some printfs to debugging output.
 * 	[90/08/14  14:46:31  mbj]
 * 
 * Revision 1.8  89/07/09  14:22:03  dpj
 * 	Updated debugging statements for new DEBUG macros.
 * 	[89/07/08  13:18:50  dpj]
 * 
 * Revision 1.7  89/03/17  13:05:16  sanzi
 * 	Removed include of mach_object.h.
 * 	[89/02/22  14:57:36  dpj]
 * 
 * Revision 1.6  88/12/05  02:54:53  mbj
 * Add routine for closing and cleaning up all open bsd ttys.
 * 
 * Revision 1.5  88/11/28  16:09:13  mbj
 * Remove unnecessary machine dependencies.
 * 
 * Revision 1.4  88/11/17  16:09:32  mbj
 * Include mach_types.h for mach_object.h.
 * 
 * Revision 1.3  88/11/04  11:35:05  dorr
 * temp fix to select.
 * 
 * Revision 1.2  88/11/01  14:12:18  mbj
 * Bug fix to synchronization pipe.
 * 
 * Revision 1.1  88/10/28  01:22:53  mbj
 * Initial revision
 * 
 *  4-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it, borrowing some code from the dh11 driver.
 */

/* "kernel" includes and declarations */

#include <base.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#define	KERNEL 1
#include <sys/tty.h>
#undef	KERNEL
#include <sys/uio.h>
#include <sys/conf.h>
#include <sys/user.h>

int ttrstrt(), bsd_tty_start();
void assert_bsd_fd_change();

/* User includes */

#include <stdio.h>
#undef getc		/* Used by the tty drivers themselves */
#undef putc
#undef getchar		/* Prevent accidental use */
#undef putchar
#include <sys/errno.h>
extern int errno;
#include <sys/file.h>
#include <mach/mach_types.h>

#if	MACH3_US
#include <unix_include/sys/select_types.h>	/* For fd_set and friends */
#endif	MACH3_US

extern char *tty_dev_name();
extern char *newstr();

/*
 * Driver definitions
 */

#define	ISPEED	B9600
#define	IFLAGS	(EVENP|ODDP|ECHO)

#define NUNITS 256		/* As many as minor() allows */
#define NBSDFDS FD_SETSIZE	/* Number of possible bsd fds */

struct tty bsd_tty[NUNITS];	/* Driver structures for each terminal */
int bsd_fd[NUNITS];		/* BSD fds for each open terminal */
char *bsd_name_by_unit[NUNITS + 1]; /* BSD names by unit numbers */
int unit_by_bsd_fd[NBSDFDS];	/* Unit numbers for each open bsd fd */

int interest_nfds;	/* Max fd in interest_fds + 1 */
fd_set interest_fds;	/* Set of bsd fds we're interested in */
struct mutex fd_mutex = MUTEX_INITIALIZER; /* Mutex for interest_fds */
struct sgttyb oldmode[NUNITS];
struct sgttyb curmode[NUNITS];
int oldflags[NUNITS];

int pipe_fds[2];	/* Pipe used by select and assert_bsd_fd_change */
struct condition resynchronized = CONDITION_INITIALIZER;
struct mutex resynchronized_mutex = MUTEX_INITIALIZER;

#if	MACH_NO_KERNEL
struct tty *
bsd_tty_find_tty(dev)
	dev_t	dev;
{
	return (&bsd_tty[minor(dev)]);
}
#endif	MACH_NO_KERNEL


bsd_tty_init()
/*
 * Init this module
 */
{
    register int i;
    FD_ZERO(&interest_fds);
    for (i = 0; i < NUNITS; i++) bsd_fd[i] = -1; /* Illegal fds */
    interest_nfds = 0;
    if (pipe(pipe_fds) < 0) perror("pipe error");
    FD_SET(pipe_fds[0], &interest_fds);
    if (pipe_fds[0] + 1 > interest_nfds) interest_nfds = pipe_fds[0] + 1;
    unit_by_bsd_fd[pipe_fds[0]] = NUNITS;	/* Out of normal bounds */
    bsd_name_by_unit[NUNITS] = "synchronization pipe";	/* For debug output */
}

/*
 * Open a bsd_tty "line".
 */
bsd_tty_open(dev, flag) dev_t dev; int flag;
{
    register struct tty *tp = &bsd_tty[minor(dev)];
    register int unit;

    unit = minor(dev);
    if (tp->t_state&TS_XCLUDE && u.u_uid!=0)
	    return (EBUSY);
    tp->t_oproc = bsd_tty_start;
    tp->t_state |= TS_WOPEN;

    /*
     * If this is first open, initialize tty state to default.
     */
    if ((tp->t_state&TS_ISOPEN) == 0) {
	int err;
	if (err = bsd_tty_really_open(dev)) return err;

	ttychars(tp);
	tp->t_ispeed = ISPEED;
	tp->t_ospeed = ISPEED;
	tp->t_flags = IFLAGS;
	bsd_tty_param(unit);
    }
    /*
     * Wait for carrier, then process line discipline specific open.
     */
    return ((*linesw[tp->t_line].l_open)(dev, tp));
}

bsd_tty_close(dev, flag) dev_t dev; int flag;
{
    register struct tty *tp;
    register unit;
    int fd;

    unit = minor(dev);
    tp = &bsd_tty[unit];
    (*linesw[tp->t_line].l_close)(tp);

    mutex_lock(&fd_mutex);
    fd = bsd_fd[unit];
    FD_CLR(fd, &interest_fds);
    unit_by_bsd_fd[fd] = -1;
    bsd_fd[unit] = -1;
    mutex_unlock(&fd_mutex);
    assert_bsd_fd_change();

    if (fcntl(fd, F_SETFL, oldflags[unit]) == -1) {
	perror("F_SETFL oldflags");
    }

    if (ioctl(fd, TIOCSETN, (char *)&oldmode[unit]) < 0) {
	perror("TIOCSETN oldmode");
    }
    close(fd);

    ttyclose(tp);
}

bsd_tty_read(dev, uio) dev_t dev; struct uio *uio;
{
    register struct tty *tp = &bsd_tty[minor(dev)];
    return ((*linesw[tp->t_line].l_read)(tp, uio));
}

bsd_tty_write(dev, uio) dev_t dev; struct uio *uio;
{
    register struct tty *tp = &bsd_tty[minor(dev)];
    return ((*linesw[tp->t_line].l_write)(tp, uio));
}

bsd_tty_ioctl(dev, cmd, addr, flag) dev_t dev; int cmd; caddr_t addr; int flag;
{
    register int unit = minor(dev);
    register struct tty *tp = &bsd_tty[unit];
    int error;

    error = (*linesw[tp->t_line].l_ioctl)(tp, cmd, addr, flag);
    if (error >= 0)
	return (error);
    error = ttioctl(tp, cmd, addr, flag);
    if (error >= 0) {
	if (cmd == TIOCSETP || cmd == TIOCSETN || cmd == TIOCLBIS ||
	    cmd == TIOCLBIC || cmd == TIOCLSET)
	    bsd_tty_param(unit);
	return (error);
    }
    switch (cmd) {

    case TIOCSBRK:
    case TIOCCBRK:
    case TIOCSDTR:
    case TIOCCDTR:
	if (ioctl(bsd_fd[unit], cmd, 0) < 0)	/* Pass on down */
	    perror("ioctl error");
	break;

    default:
	return (ENOTTY);
    }
    return (0);
}

bsd_tty_start(tp) struct tty *tp;
{
    int s;	/* spl level */
    int nch;	/* Character count */

    /*
     * Must hold interrupts in following code to prevent
     * state of the tp from changing.
     */
    s = spl5();
    /*
     * If it's currently active, or delaying, no need to do anything.
     */
    if (tp->t_state&(TS_TIMEOUT|TS_BUSY|TS_TTSTOP))
	goto out;
    /*
     * If there are sleepers, and output has drained below low
     * water mark, wake up the sleepers.
     */
    if (tp->t_outq.c_cc<=TTLOWAT(tp)) {
	if (tp->t_state&TS_ASLEEP) {
	    tp->t_state &= ~TS_ASLEEP;
	    wakeup(&tp->t_outq_lock);
	}
	if (tp->t_wsel) {
	    selwakeup(tp->t_wsel, tp->t_state & TS_WCOLL);
	    tp->t_wsel = 0;
	    tp->t_state &= ~TS_WCOLL;
	}
    }
    /*
     * Now restart transmission unless the output queue is
     * empty.
     */
    if (tp->t_outq.c_cc == 0)
	goto out;
    if (tp->t_flags & (RAW|LITOUT))
	nch = ndqb(&tp->t_outq, 0);
    else {
	nch = ndqb(&tp->t_outq, 0200);
	/*
	 * If first thing on queue is a delay process it.
	 */
	if (nch == 0) {
	    nch = getc(&tp->t_outq);
	    tp->t_state |= TS_TIMEOUT;
	    timeout(ttrstrt, (caddr_t)tp, (nch&0x7f)+6);
	    goto out;
	}
    }
    /*
     * If characters to transmit, restart transmission.
     */
    if (nch) {
	int written;

	tp->t_state |= TS_BUSY;
	written = write(bsd_fd[minor(tp->t_dev)], tp->t_outq.c_cf, nch);
	if (written > 0) {
	    ndflush(&tp->t_outq, written);	/* Flush chars from queue */
	} else if (written < 0 && errno != EWOULDBLOCK) {
	    DEBUG0(1,(Diag,"write error %d\n", errno));
	} else {
	    DEBUG0(1,(Diag,"write returned 0\n"));
	}
	tp->t_state &= ~TS_BUSY;

	/*
	 * Simulate the transmit interrupt which would occur after the splx
	 * on actual hardware.  Perform the "interrupt" inside the spl since
	 * the priority level would be automatically set by real hardware.
	 */
	bsd_tty_xint(minor(tp->t_dev));
    }
out:
    splx(s);
}

/*ARGSUSED*/
bsd_tty_stop(tp, flag) struct tty *tp; int flag;
{
    /* Can't stop Unix writes in a good clean way -- don't try */

    DEBUG0(1,(Diag,"bsd_tty_stop\n")); /* XXX */
}

bsd_tty_select(dev, rw) dev_t dev; int rw;
{
    register struct tty *tp = &bsd_tty[minor(dev)];
    return ((*linesw[tp->t_line].l_select)(dev, rw));
}

bsd_tty_rint(unit, buffer, count) int unit; char *buffer; int count;
{
    register struct tty *tp;
    int c;
    int n;

    tp = &bsd_tty[unit];
    for (n = 0; n < count; n++) {
	c = buffer[n];
	if ((tp->t_state&TS_ISOPEN)==0) {
	    wakeup((caddr_t)&tp->t_rawq);
		continue;
	}
	(*linesw[tp->t_line].l_rint)(c, tp);
    }
}

bsd_tty_xint(unit) int unit;
{
    register struct tty *tp;

    tp = &bsd_tty[unit];
    if (tp->t_line)
	(*linesw[tp->t_line].l_start)(tp);
    else
	bsd_tty_start(tp);
}

/*
 * Set parameters from open or stty into the bsd_tty "hardware"
 * registers.
 */
bsd_tty_param(unit)
    register int unit;
{
    register struct tty *tp;
    int s;

    tp = &bsd_tty[unit];
    /*
     * Block interrupts so parameters will be set
     * before the line interrupts.
     */
    s = spl5();
    if ((tp->t_ispeed)==0) {
	tp->t_state |= TS_HUPCLS;
	splx(s);
	return;
    }

    curmode[unit].sg_ispeed = tp->t_ispeed;
    curmode[unit].sg_ospeed = tp->t_ospeed;
    curmode[unit].sg_flags &= ~ANYP;
    curmode[unit].sg_flags |= tp->t_flags & ANYP;
    if (ioctl(bsd_fd[unit], TIOCSETN, (char *)&curmode[unit]) < 0) {
	perror("TIOCSETN");
    }

    /* Set delay flags? XXX */

    splx(s);
}

/*
 * Open a bsd tty and set up the associated driver parameters.
 */
bsd_tty_really_open(dev)
    dev_t dev;
{
    register struct tty *tp;
    register int unit;
    char *name;

    unit = minor(dev);
    tp = &bsd_tty[unit];
    name = tty_dev_name(dev);
    bsd_name_by_unit[unit] = name;
    bsd_fd[unit] = open(name, O_RDWR, 0);
    unit_by_bsd_fd[bsd_fd[unit]] = unit;
    if (bsd_fd[unit] < 0) {
	int err = errno;
	perror("open error");
	return err;
    }
    if (! isatty(bsd_fd[unit])) return ENOTTY;

    tp->t_state |= TS_CARR_ON;

    if (ioctl(bsd_fd[unit], TIOCGETP, (char *)&oldmode[unit]) < 0) {
	perror("TIOCGETP");
    }
    curmode[unit] = oldmode[unit];
    curmode[unit].sg_flags |= (RAW);
    curmode[unit].sg_flags &= ~ECHO;
    if (ioctl(bsd_fd[unit], TIOCSETN, (char *)&curmode[unit]) < 0) {
	perror("TIOCSETN");
    }

    if ((oldflags[unit] = fcntl(bsd_fd[unit], F_GETFL, 0)) == -1) {
	perror("F_GETFL");
    }

    if (fcntl(bsd_fd[unit], F_SETFL, FNDELAY) == -1) {
	perror("F_SETFL FNDELAY");
    }

    mutex_lock(&fd_mutex);
    FD_SET(bsd_fd[unit], &interest_fds);
    if (bsd_fd[unit] + 1 > interest_nfds) interest_nfds = bsd_fd[unit] + 1;
    mutex_unlock(&fd_mutex);
    assert_bsd_fd_change();

    return 0;
}

#define BUF_SIZE 256	/* A reasonable number of characters */

/*ARGSUSED*/
void bsd_tty_select_thread(arg) int arg;
/*
 * Sit waiting on all open terminals for input or other events.
 * Call the appropriate device interrupt routine when an event occurs.
 */
{
    int nfound;
    int nfds;
    fd_set read_fds;
    fd_set except_fds;

    while (1) {
	mutex_lock(&fd_mutex);
	nfds = interest_nfds;
	read_fds = interest_fds;	/* Set of fds we're interested in */
	except_fds = interest_fds;
	mutex_unlock(&fd_mutex);

	nfound = select(nfds, &read_fds, (fd_set *)0, &except_fds,
	    (struct timeval *) 0);

	if (nfound > 0) {
	    int count;
	    register int bsdfd;
	    int available;
	    int read_data = 0;
	    char buffer[BUF_SIZE];
	    int unit;

	    for (bsdfd = 0; bsdfd < nfds; bsdfd++) {
		if (! (FD_ISSET(bsdfd, &read_fds))) continue;

		unit = unit_by_bsd_fd[bsdfd];

		if (ioctl(bsdfd, FIONREAD, (char *)&available) < 0) {
		    DEBUG0(1,(Diag,"%s: FIONREAD error %d\n", bsd_name_by_unit[unit], errno));
		    continue;
		}
		if (available <= 0) {
		    DEBUG0(TRUE,(Diag,"select reports data on %s, but none available\n",
			bsd_name_by_unit[unit]));
		    continue;
		}

#if DEBUG_BSD_TTY
		DEBUG1(TRUE,(Diag,"%s has %d chars available\r\n",
		    bsd_name_by_unit[unit], available));
#endif DEBUG_BSD_TTY
		if (unit < NUNITS)
		    DEBUG1(TRUE,(Diag,"%d", available));/* Chars ready */
		else
		    DEBUG1(TRUE,(Diag,"S"));	/* Synchronization */
		fflush(stdout);

check_input:
		if ((count = read(bsdfd, buffer, BUF_SIZE)) > 0) {
		    if (unit >= 0 && unit < NUNITS) {
			/*
			 * Call the bsd_tty receive interrupt routine.
			 */
			bsd_tty_rint(unit, buffer, count);
		    }
		    else if (unit == NUNITS) {
			/* Read on synchronization pipe */
			mutex_lock(&resynchronized_mutex);
			condition_broadcast(&resynchronized);
			mutex_unlock(&resynchronized_mutex);
			goto restart_select;
		    } else {
			DEBUG0(1,(Diag,"Illegal unit number %d\n", unit));
		    }

		    read_data = 1;
		    if (count == BUF_SIZE) {
			goto check_input;	/* Probably more to read */
		    }
		} else if (count < 0 && errno != EWOULDBLOCK) {
		    char errbuf[100];
		    (void) sprintf(errbuf, "%s: read error", bsd_name_by_unit[unit]);
		    perror(errbuf);
		} else {
		    /* printf("%s: count = 0\r\n", bsd_name_by_unit[unit]); */
		}
	    }
	    if (! read_data) {
		DEBUG0(TRUE,(Diag,"spurious reads\r\n"));
	    }

	    for (bsdfd = 0; bsdfd < nfds; bsdfd++) {
		if (! (FD_ISSET(bsdfd, &except_fds))) continue;

		unit = unit_by_bsd_fd[bsdfd];

		DEBUG0(TRUE,(Diag,"select exception on unit %d, bsd_fd %d, %s\n",
			unit, bsdfd, bsd_name_by_unit[unit]));
	    }

	} else {
	    perror("select error");
	    sleep(1);		/* XXX Slow down error output */
	}
restart_select:;
    }
}

void assert_bsd_fd_change()
/*
 * Cause all threads with knowledge of bsd fds to re-evaluate their set of fds.
 */
{
    mutex_lock(&resynchronized_mutex);
    write(pipe_fds[1], "X", 1);		/* Send synchronization "message" */
    condition_wait(&resynchronized, &resynchronized_mutex);
    mutex_unlock(&resynchronized_mutex);
}

void close_all_bsd_ttys()
/*
 * Close all open bsd ttys.  Used by server exit cleanup code.
 */
{
    register int unit;

    for (unit = 0; unit < NUNITS; unit++) {
	if (bsd_tty[unit].t_state & TS_ISOPEN) bsd_tty_close(unit, 0);
    }
}

#if	MACH3_UNIX
/*ARGSUSED*/
struct tty *
nulltty(dev)
	dev_t	dev;
{
#if	CMUCS
	extern struct tty sytty[1];
	return &sytty[0];
#else	CMUCS
	return ((struct tty *)0);
#endif	CMUCS
}
#endif	MACH3_UNIX
