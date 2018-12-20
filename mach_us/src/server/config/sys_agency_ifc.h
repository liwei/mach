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
 * File:  sys_agency_ifc.h
 *
 * Purpose: Mach Startup/Admin/Config Server agency
 *
 * HISTORY
 * $Log:	sys_agency_ifc.h,v $
 * Revision 2.6  94/07/07  16:38:04  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/05/17  14:09:27  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/28  19:06:53  jms]
 * 
 * Revision 2.4  92/07/05  23:34:16  dpj
 * 	Converted to C++.
 * 	[92/06/24  17:40:27  dpj]
 * 
 * Revision 2.3  92/03/05  15:12:27  jms
 * 	mach_types.h -> mach/mach_types.h
 * 
 * Revision 2.2  91/11/06  14:19:29  jms
 * 	Moved from lib/us
 * 	[91/11/04  17:46:00  jms]
 * 
 * Revision 2.2  91/10/06  22:48:21  jjc
 * 	Created.
 * 	[91/06/20            jjc]
 * 
 *
 */
#ifndef	_sys_agency_ifc_h
#define	_sys_agency_ifc_h

#include	<vol_agency_ifc.h>
#include	<us_sys_ifc.h>

extern "C" {
#include	<ns_types.h>
}


// XXX Using "virtual" keywords crashes the 1.37.1 compiler.
class sys_agency: public vol_agency, public usSys {
// class sys_agency: public virtual usSys, public virtual vol_agency {
	ns_name_t*	prefix_names;		/* table of prefix names */
	usItem**	server_objects;		/* table of server objects */
	int		prefix_count;		/* number of prefixes */
      public:
	DECLARE_MEMBERS(sys_agency);
				sys_agency();
				sys_agency(
					usItem**,
					ns_name_t*,
					int,
					mach_error_t*);
				~sys_agency();

REMOTE virtual mach_error_t	sys_get_prefix_table(
					usItem**,
					int*,
					ns_name_t**,
					int*);
};

#endif	_sys_agency_ifc_h
