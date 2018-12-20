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
 * HISTORY
 * $Log:	logging.c,v $
 * Revision 2.3  94/07/07  17:23:32  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  93/01/20  17:37:49  jms
 * 	Fisrt multi-server revision
 * 	[93/01/18  16:27:50  jms]
 * 
 */
#include	<logging.h>

#if LOGGING_ENABLED

#include	<cthreads.h>

#if	MACH3_VUS
#include	<stdio.h>
#include	<sys/file.h>
#else	MACH3_VUS
	!!! DIE PIG DIE !!!
#endif	MACH3_VUS


/*
 * Size of each of the current and the old log.
 */
#define	LOG_SIZE	10000
#define	LOG_LIMIT	9000

/*
 * Log variables.
 */
log_rec_t	log1[LOG_SIZE];
log_rec_t	log2[LOG_SIZE];
log_rec_t	*log_cur_ptr = log1;	/* next free record in the log */
log_rec_t	*log_end_ptr = log1 + LOG_SIZE;	/* end of the log */
struct mutex	log_lock_data = MUTEX_INITIALIZER;
mutex_t		log_lock = &log_lock_data;	/* lock for the log */

static	log_rec_t	*old_log_start_ptr = log2;	/* first record of the old log */
static	log_rec_t	*cur_log_start_ptr = log2 + LOG_SIZE;	/* first record of the current log */
PRIVATE int		old_log_length = 0;	/* size of old log */


/*
 * log_init
 *
 * Logging module initialization.
 *
 * Parameters: none
 *
 * Results:
 *
 * Side effects:
 *
 * Allocates space for the log and initializes all the relevant variables.
 *
 * Design:
 *
 * Note:
 *
 */
int log_init()
{
#ifdef	notdef
	vm_size_t	size;

	size = LOG_SIZE * sizeof(log_rec_t);
	if ((cur_log_start_ptr = (log_rec_t *)malloc((unsigned int)size)) == (log_rec_t *)0) {
		printf("log_init.malloc failed\n");
	}
	if ((old_log_start_ptr = (log_rec_t *)malloc((unsigned int)size)) == (log_rec_t *)0) {
		printf("log_init.malloc failed\n");
	}
	log_cur_ptr = cur_log_start_ptr;
	log_end_ptr = cur_log_start_ptr + LOG_LIMIT;
	log_lock = &log_lock_data;
	mutex_init(log_lock);
	mutex_set_name(log_lock, "log_lock");
#endif	notdef
	return(0);
}


/*
 * log_reset
 *
 * Parameters:
 *
 * Results: none
 *
 * Side effects:
 *
 * Swaps the current and old logs.  Reinitialises the new current log.
 *
 * Note:
 *	Locks the log_lock whilst manipulating the logs.
 *
 */
/*ARGSUSED*/
void log_reset()
{
	register log_rec_t	*temp_ptr;

	mutex_lock(log_lock);

	old_log_length = log_cur_ptr - cur_log_start_ptr;

	temp_ptr = cur_log_start_ptr;
	cur_log_start_ptr = old_log_start_ptr;
	old_log_start_ptr = temp_ptr;

	log_cur_ptr = cur_log_start_ptr;
	log_end_ptr = cur_log_start_ptr + LOG_LIMIT;

	mutex_unlock(log_lock);
}


/*
 * log_dump
 *
 * Parameters: 
 *
 * Results: none
 *
 * Side effects:
 *
 * Writes both the old and the current logs into a file named "USLOG".
 * Resets the logs.
 *
 * Note:
 *	Locks the log_lock whilst manipulating the logs.
 *
 */
/*ARGSUSED*/
void log_dump()
{
#if	MACH3_VUS
	int	fd;
	extern int errno;

	fd = open("USLOG", O_WRONLY|O_CREAT, 0644);
	if (fd < 0) {
		fprintf(stderr, "fd = %d, errno = %d.\n", fd, errno);
	    	perror("Cannot open file to dump the log");
		_exit(1);
	}
	mutex_lock(log_lock);

	if (old_log_length)
		(void)write(fd, (char *)old_log_start_ptr, old_log_length * sizeof(log_rec_t));
	(void)write(fd, (char *)cur_log_start_ptr, (log_cur_ptr - cur_log_start_ptr) * sizeof(log_rec_t));
	(void)close(fd);

	log_cur_ptr = cur_log_start_ptr;
	old_log_length = 0;

	mutex_unlock(log_lock);

#endif	MACH3_VUS

}

#endif LOGGING_ENABLED
