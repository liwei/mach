/* 
 * Mach Operating System
 * Copyright (c) 1994,1993 Carnegie Mellon University
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
 * Purpose: Definitions for statistics gathering. Available from all files.
 *	Complex manipulation is done in diag.cc.
 *
 * HISTORY: 
 * $Log:	us_statistics.h,v $
 * Revision 2.3  94/07/08  18:43:27  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  94/06/16  17:20:34  mrt
 * 	First checkin of statistics logging stuff from DPJ.
 * 	[94/05/25  13:20:31  jms]
 * 
 */

#ifndef	_US_STATISTICS_H
#define	_US_STATISTICS_H

#if	_STATISTICS_

#include <macro_help.h>
#include <cthreads.h>


/*
 * Statistics definitions.
 */

#define	USSTATS_TOP_ALLOC	0
#define	USSTATS_TOP_DEALLOC	1
#define	USSTATS_RAWOBJ_ALLOC	2
#define	USSTATS_RAWOBJ_DEALLOC	3
#define	USSTATS_INCOMING_RPC	4
#define	USSTATS_OUTGOING_RPC	5
#define USSTATS_ABORT_RPC	6
#define	USSTATS_NOTIF_NMS	7
#define	USSTATS_NOTIF_PDEAD	8
#define	USSTATS_CASTDOWN	9

#define	USSTATS_MAX		9


/*
 * Global variables accessible from all modules, C or C++.
 */

extern struct mutex	us_stats_lock;
extern int	us_stats[];
extern char* 	us_stats_names[];


/*
 * Macro to update statistics.
 */

#define	USSTATS(_code)						\
	MACRO_BEGIN						\
		mutex_lock(&us_stats_lock);			\
		us_stats[_code]++;				\
		mutex_unlock(&us_stats_lock);			\
	MACRO_END

#else	_STATISTICS_

#define	USSTATS(_code)	/* */

#endif	_STATISTICS_


#endif	_US_STATISTICS_H
