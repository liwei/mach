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
 * $Log:	logging.h,v $
 * Revision 2.3  94/07/08  18:43:11  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  93/01/20  17:39:03  jms
 * 	First version (orig from netmsgsvr)
 * 	[93/01/18  17:23:37  jms]
 * 
 *
 */

/*
 * Macros used for debugging, tracing and reporting errors. 
 */

#ifndef	_LOGGING_H
#define	_LOGGING_H

#ifndef LOGGING_ENABLED
#define	LOGGING_ENABLED		0
#endif LOGGING_ENABLED

#include	<cthreads.h>

/*
 * Definition for a log record.
 */
typedef	struct	{
	long	code;
	long	thread;
	long	a1;
	long	a2;
	long	a3;
	long	a4;
	long	a5;
	long	a6;
} log_rec_t;

typedef	log_rec_t	*log_ptr_t;


/*
 * Global definitions for the log area.
 */
extern 	log_rec_t	*log_cur_ptr;
extern	log_rec_t	*log_end_ptr;
extern	mutex_t		log_lock;


/*
 * External functions in the logging module.
 */
extern int	log_init();
extern void	log_reset();
extern void	log_dump();


/*
 * Logging macros.
 */

#if	LOGGING_ENABLED

#define	LOG0(cond,dcode) {					\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr++;				\
		mutex_unlock(log_lock);				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
	}							\
}
#define	LOG1(cond,dcode,da1) {					\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr++;				\
		mutex_unlock(log_lock);				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
		lp->a1 = (long)da1;				\
	}							\
}
#define	LOG2(cond,dcode,da1,da2) {				\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr++;				\
		mutex_unlock(log_lock);				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
		lp->a1 = (long)da1;				\
		lp->a2 = (long)da2;				\
	}							\
}
#define	LOG3(cond,dcode,da1,da2,da3) {				\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr++;				\
		mutex_unlock(log_lock);				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
		lp->a1 = (long)da1;				\
		lp->a2 = (long)da2;				\
		lp->a3 = (long)da3;				\
	}							\
}
#define	LOG4(cond,dcode,da1,da2,da3,da4) {			\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr++;				\
		mutex_unlock(log_lock);				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
		lp->a1 = (long)da1;				\
		lp->a2 = (long)da2;				\
		lp->a3 = (long)da3;				\
		lp->a4 = (long)da4;				\
	}							\
}
#define	LOG5(cond,dcode,da1,da2,da3,da4,da5) {			\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr++;				\
		mutex_unlock(log_lock);				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
		lp->a1 = (long)da1;				\
		lp->a2 = (long)da2;				\
		lp->a3 = (long)da3;				\
		lp->a4 = (long)da4;				\
		lp->a5 = (long)da5;				\
	}							\
}
#define	LOG6(cond,dcode,da1,da2,da3,da4,da5,da6) {		\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr++;				\
		mutex_unlock(log_lock);				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
		lp->a1 = (long)da1;				\
		lp->a2 = (long)da2;				\
		lp->a3 = (long)da3;				\
		lp->a4 = (long)da4;				\
		lp->a5 = (long)da5;				\
		lp->a6 = (long)da6;				\
	}							\
}
#define	LOG_STRING(cond,dcode,ds) {				\
	if (cond) {						\
		register log_rec_t *lp;				\
		mutex_lock(log_lock);				\
		lp = log_cur_ptr;				\
		log_cur_ptr += 4;				\
		lp->code = dcode;				\
		lp->thread = (long) cthread_self();		\
		(void)strncpy((char *)&lp->a1,ds,110);		\
		((char *)&lp->a1)[110] = '\0';			\
		mutex_unlock(log_lock);				\
	}							\
}



/*
 * LOGCHECK makes sure that the log does not overflow, and resets it if 
 * necessary.
 */
#define	LOGCHECK {				\
	if (log_cur_ptr >= log_end_ptr)		\
		(void)log_reset();		\
}

#else	LOGGING_ENABLED

#define	LOG0(cond,dcode)				/**/
#define	LOG1(cond,dcode,da1)				/**/
#define	LOG2(cond,dcode,da1,da2)			/**/
#define	LOG3(cond,dcode,da1,da2,da3)			/**/
#define	LOG4(cond,dcode,da1,da2,da3,da4)		/**/
#define	LOG5(cond,dcode,da1,da2,da3,da4,da5)		/**/
#define	LOG6(cond,dcode,da1,da2,da3,da4,da5,da6)	/**/
#define	LOG_STRING(cond,dcode,ds)			/**/
#define	LOGCHECK

#endif	LOGGING_ENABLED

#endif	_LOGGING_H
