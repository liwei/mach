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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/sig_error.h,v $
 *
 * Purpose: Definitions of signal numbers
 *
 * HISTORY:
 * $Log:	sig_error.h,v $
 * Revision 2.4  94/07/08  15:51:33  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  90/07/09  17:00:52  dorr
 * 	no change.
 * 	[90/03/01  14:31:12  dorr]
 * 
 * 	SIG -> UNIX_SIG.  This may be a mistake because
 * 	we want generic signal conditions ... stay tuned.
 * 	[90/02/23  14:32:57  dorr]
 * 
 * 	SIG() internal macro -> unix_sig_err() macro
 * 	[90/01/11  11:19:29  dorr]
 * 
 * 	iniitial revision
 * 	[90/01/07  16:18:12  dorr]
 * 
 * 	created.
 * 	[89/08/11  14:57:27  dorr]
 * 
 * 	no change.
 * 	[90/03/01  14:31:12  dorr]
 * 
 * 	SIG -> UNIX_SIG.  This may be a mistake because
 * 	we want generic signal conditions ... stay tuned.
 * 	[90/02/23  14:32:57  dorr]
 * 
 * 	SIG() internal macro -> unix_sig_err() macro
 * 	[90/01/11  11:19:29  dorr]
 * 
 * 	iniitial revision
 * 	[90/01/07  16:18:12  dorr]
 * 
 * 	created.
 * 	[89/08/11  14:57:27  dorr]
 * 	No Further Changes
 * 	[90/07/06  13:37:37  jms]
 * 
 * Revision 2.2  90/03/21  16:32:42  jms
 * 	Dummy checkin of version from branch dorr_signals
 * 	[90/03/14  17:03:02  jms]
 * 
 */

#ifndef	_sig_error_h
#define	_sig_error_h

#include <mach/error.h>

/* unix signals (they map onto unix signal #'s */
#define	UNIX_SIG_ERR_SUB	14
#define	unix_sig_err(x)		(err_sub(UNIX_SIG_ERR_SUB)|x)

#define	UNIX_SIG_NO_SIGNAL	unix_sig_err(0)

#define	UNIX_SIG_HUP		unix_sig_err(1)
#define	UNIX_SIG_INT		unix_sig_err(2)
#define	UNIX_SIG_QUIT		unix_sig_err(3)
#define	UNIX_SIG_KILL		unix_sig_err(9)
#define	UNIX_SIG_SYS		unix_sig_err(12)
#define	UNIX_SIG_PIPE		unix_sig_err(13)
#define	UNIX_SIG_ALRM		unix_sig_err(14)
#define	UNIX_SIG_TERM		unix_sig_err(15)
#define	UNIX_SIG_URG		unix_sig_err(16)
#define	UNIX_SIG_STOP		unix_sig_err(17)
#define	UNIX_SIG_TSTP		unix_sig_err(18)
#define	UNIX_SIG_CONT		unix_sig_err(19)
#define	UNIX_SIG_CHLD		unix_sig_err(20)
#define	UNIX_SIG_TTIN		unix_sig_err(21)
#define	UNIX_SIG_TTOU		unix_sig_err(22)
#define	UNIX_SIG_IO		unix_sig_err(23)
#define	UNIX_SIG_XCPU		unix_sig_err(24)
#define	UNIX_SIG_XFSZ		unix_sig_err(25)
#define	UNIX_SIG_VTALRM		unix_sig_err(26)
#define	UNIX_SIG_PROF		unix_sig_err(27)
#define	UNIX_SIG_WINCH		unix_sig_err(28)
#define	UNIX_SIG_USR1		unix_sig_err(30)
#define	UNIX_SIG_USR2		unix_sig_err(31)

#endif	_sig_error_h
