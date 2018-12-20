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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/diag_ifc.h,v $
 *
 * Purpose: Diagnostic and error messages system.
 *
 * HISTORY:
 * $Log:	diag_ifc.h,v $
 * Revision 2.5  94/07/07  17:23:09  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  94/06/16  17:19:10  mrt
 * 	Add USSTATS stuff from DPJ.
 * 	[94/05/25  13:14:23  jms]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:05:13  pjg]
 * 
 * Revision 2.3  92/07/05  23:27:08  dpj
 * 	Conditionalized virtual base class specifications.
 * 	Added cloning functions under GXXBUG_CLONING1.
 * 	[92/06/24  16:10:57  dpj]
 * 
 * 	Made usTop a virtual base class instead of a plain base class.
 * 	Fixed prototype for diag_format().
 * 	[92/05/10  00:52:39  dpj]
 * 
 * Revision 2.2  91/11/06  13:45:51  jms
 * 	Upgraded to US41 and fixed some bugs introduced by the conversion
 * 	to C++.
 * 	[91/10/03  15:07:07  pjg]
 * 
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:50:01  pjg]
 * 
 * Revision 2.4  91/07/01  14:11:48  jms
 * 	Added mgr_dump_machobj_statistics(), mgr_reset_machobj_statistics().
 * 	[91/06/21  17:21:06  dpj]
 * 
 * 	Removed cloning methods. We now use diag_post_fork() instead.
 * 	[91/06/16  21:07:03  dpj]
 * 
 * Revision 2.3  90/10/29  17:32:18  dpj
 * 	Merged-up to U25
 * 	[90/09/02  20:02:19  dpj]
 * 
 * Revision 2.2  89/07/09  14:18:53  dpj
 * 	Initial revision.
 * 	[89/07/08  12:53:32  dpj]
 * 
 */

#ifndef	_diag_ifc_h
#define	_diag_ifc_h

#include <top_ifc.h>

class diag: public VIRTUAL3 usTop {
	int	global_diag_level;
	char	global_diag_name[256];
	mach_port_t	diag_port;
      public:
	diag();

	mach_error_t set_diag_level(int);
	mach_error_t get_diag_level(int*);
	mach_error_t set_diag_name(char*);
	mach_error_t get_diag_name(char*);

	mach_error_t diag_init_mesg(char**, int);
	void _us_internal_error(char *, mach_error_t, char *, int);

	void usstats_reset();
	void usstats_dump();

	virtual mach_error_t clone_init(mach_port_t);
#ifdef	GXXBUG_CLONING1
	virtual mach_error_t	clone_abort(mach_port_t);
	virtual mach_error_t	clone_complete();
#endif	GXXBUG_CLONING1
};

extern "C" {
/* 
 * Functions exported by the Diag system
 */
void us_init_diag();
mach_error_t diag_startup(char*);
mach_error_t diag_startup_lazy(char*);
mach_error_t diag_startup_printf(char*);
void diag_mach3_init();
int set_diag_level(int);
void diag_format(/*char *m ...*/);
void diag_us_internal_error(char *, mach_error_t, char *, int);
}

#endif	_diag_ifc_h

