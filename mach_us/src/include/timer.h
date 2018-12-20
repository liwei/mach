/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/timer.h,v $
 *
 * Purpose:
 *	Interval timer definitions
 *
 * HISTORY
 * $Log:	timer.h,v $
 * Revision 2.3  94/07/08  15:51:41  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  90/08/13  15:43:38  jjc
 * 	Created.
 * 	[90/07/19            jjc]
 * 
 *
 */
#ifndef	_TIMER_H
#define _TIMER_H

#include	<base.h>
#include <mach/time_value.h>


/*
 *	Interval timer types
 */
#define	TIMER_REAL	0	/* real time */
#define	TIMER_VIRTUAL	1	/* virtual time */
#define	TIMER_PROFILED	2	/* system + user time */
typedef int	timer_type_t;

#define	TIMER_INVALID_ID	0
typedef	long	timer_id_t;

#define	TIMER_MAXSEC	100000000	/* max seconds timer can be set to */


#define TIMER_CLEAR(tvp)         (tvp)->seconds = (tvp)->microseconds = 0

#define	TIMER_CMP(tvp, uvp, cmp) \
	((tvp)->seconds cmp (uvp)->seconds || \
	 ((tvp)->seconds == (uvp)->seconds && (tvp)->microseconds cmp (uvp)->microseconds))

#define	TIMER_ISSET(tvp)	((tvp)->seconds || (tvp)->microseconds)


struct timer_value {
	time_value_t	interval;
	time_value_t	time;
};

typedef struct timer_value	*timer_value_t;

typedef char	*timer_addr_t;

#endif	_TIMER_H
