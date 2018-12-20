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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_sys_ifc.h,v $
 *
 * Purpose: System(config) mgr interface
 *
 * HISTORY:
 * $Log:	us_sys_ifc.h,v $
 * Revision 2.4  94/07/08  15:51:57  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:24:03  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:52:33  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:46  jms
 * 	First C++ version of the config servers "sys" object interface.
 * 	[91/11/04  16:59:44  jms]
 * 
 * Revision 2.2  91/10/06  22:26:30  jjc
 * 	Defined MAX_PREFIX.
 * 	[91/07/26            jjc]
 * 	Created.
 * 	[91/06/24            jjc]
 * 
 *
 */

#ifndef	_us_sys_h
#define	_us_sys_h

#define	MAX_PREFIX	32

#include <top_ifc.h>
#include <us_item_ifc.h>

extern "C" {
#include <cthreads.h>
}

/*
 * Sys operations
 */
class usSys: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usSys);

/*
 * sys_get_prefix_table: get a set of prefixes for initialization of
 *	a prefix table
 *
 * Parameters:
 *
 * Results:
 *
 *	objects [usItem **]:		array of objects (proxies)
 *					corresponding to each of the prefixes
 *					to be entered in the prefix table
 *
 *	object_count [int *]:		number of entries in "objects" array
 *
 *	names [ns_name_t **]:		array of strings specifying each
 *					of the prefixes to be entered in the
 *					prefix table (in the same order as
 *					in "objects")
 *
 *	name_count [int *]:		number of strings in "names" array
 *					(equal to "object_count")
 *
 * Side effects:
 *
 *	None.
 *
 * Note:
 *
 *	"objects" is an inline array, that must be pre-allocated by the
 *	caller (maximum size MAX_PREFIX). "names" is an out-of-line array.
 */
REMOTE	virtual mach_error_t sys_get_prefix_table(
	usItem **,		/* out objects */
	int *,			/* out object_count */
	ns_name_t **,		/* out names */
	int *) =0;		/* out name_count */
};

EXPORT_METHOD(sys_get_prefix_table);

#endif	_us_sys_h
