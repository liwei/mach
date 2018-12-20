/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 * File: us/lib/threads/time.c
 *
 * Purpose:	Operations on time values.
 *
 * HISTORY:
 * $Log:	time.c,v $
 * Revision 1.4  94/07/08  14:14:47  mrt
 * 	Updated copyright
 * 
 * Revision 1.3  90/07/09  17:06:52  dorr
 * 	Header Fixed
 * 	[90/07/06  17:25:16  jms]
 * 
 */

#include <cthreads.h>
#include "cthread_internals.h"
#include <sys/time.h>

/*
 * C library imports:
 */
extern gettimeofday();

#define timeval_cmp(t1, t2)	((t1)->tv_sec == (t2)->tv_sec \
				 ? (t1)->tv_usec - (t2)->tv_usec \
				 : (t1)->tv_sec - (t2)->tv_sec)

/*
 * Compare two timeval structures.
 * The current time of day is used
 * in place of t1 or t2 if it is null.
 * Result is <, ==, or > 0, according to whether t1 is <, ==, or > t2.
 */
int
time_compare(t1, t2)
	register struct timeval *t1, *t2;
{
	struct timeval now;

	if (t1 == 0 || t2 == 0) {
		(void) gettimeofday(&now, (struct timezone *) 0);
		if (t1 == 0)
			t1 = &now;
		if (t2 == 0)
			t2 = &now;
	}
	return timeval_cmp(t1, t2);
}

/*
 * result := t1 + t2
 * where t1 and t2 are struct timeval pointers.
 * The current time of day is used
 * in place of t1 or t2 if it is null.
 * t1 or t2 can also be used as the result.
 */
void
time_plus(t1, t2, result)
	register struct timeval *t1, *t2, *result;
{
	struct timeval now, r;

	if (t1 == 0 || t2 == 0) {
		(void) gettimeofday(&now, (struct timezone *) 0);
		if (t1 == 0)
			t1 = &now;
		if (t2 == 0)
			t2 = &now;
	}
	r.tv_sec = t1->tv_sec + t2->tv_sec;
	r.tv_usec = t1->tv_usec + t2->tv_usec;
	if (r.tv_usec >= 1000000) {
		r.tv_usec -= 1000000;
		r.tv_sec += 1;
	}
	*result = r;
}

/*
 * result := t1 - t2
 * where t1 and t2 are struct timeval pointers.
 * The current time of day is used
 * in place of t1 or t2 if it is null.
 * t1 or t2 can also be used as the result.
 */
void
time_diff(t1, t2, result)
	register struct timeval *t1, *t2, *result;
{
	struct timeval now, r;

	if (t1 == 0 || t2 == 0) {
		(void) gettimeofday(&now, (struct timezone *) 0);
		if (t1 == 0)
			t1 = &now;
		if (t2 == 0)
			t2 = &now;
	}
	r.tv_sec = t1->tv_sec - t2->tv_sec;
	r.tv_usec = t1->tv_usec - t2->tv_usec;
	if (r.tv_usec < 0) {
		r.tv_usec += 1000000;
		r.tv_sec -= 1;
	}
	*result = r;
}

/*
 * Convert milliseconds to struct timeval.
 */
void
time_msec_to_timeval(msec, result)
	register long msec;
	register struct timeval *result;
{
	result->tv_sec = msec / 1000;
	result->tv_usec = (msec % 1000) * 1000;
}

/*
 * Convert struct timeval to milliseconds.
 * Microseconds are truncated, not rounded, to nearest millisecond.
 * No overflow check.
 */
int
time_timeval_to_msec(t)
	register struct timeval *t;
{
	return t->tv_sec * 1000 + t->tv_usec / 1000;
}
