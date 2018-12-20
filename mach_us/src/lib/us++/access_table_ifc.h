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
 * ObjectClass: access_table
 *	Table mapping method IDs for operations exported by
 *	system agents and agencies into the standard access rights
 *	that an application must hold to be allowed to perform those
 *	operations.
 *
 * SuperClass: base
 * 	
 * Delegated Objects
 *
 * ClassMethods:
 * 
 *	Exported:
 *
 *	ns_find_required_access: find the access rights associated with
 *		a given method ID.
 *
 *	Internal:
 *
 *	setup_access_table: initialize the object with given data.
 *	setup_access_table_default: initialize the object with
 *		a default table.
 *
 * Notes:
 *
 *	This object is typically initialized from a structure defined in
 *	some specification file, using the macros defined in ns_types.h.
 * 
 * Bugs:
 * 
 * Features:
 * 
 */
 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/access_table_ifc.h,v $
 *
 * HISTORY:
 * $Log:	access_table_ifc.h,v $
 * Revision 2.4  94/07/07  17:22:36  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:26:32  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:46:42  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	Added macros for defining access table entries (moved from ns_types.h).
 * 	[92/04/17  16:07:05  dpj]
 * 
 * Revision 2.2  91/11/06  13:33:33  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:48:24  pjg]
 * 
 * Revision 2.3.1.3  91/04/14  18:17:01  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.3.1.2  91/04/14  16:43:24  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.3.1.1  90/11/14  16:58:55  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.3  89/10/30  16:30:05  dpj
 * 	Added setup methods, with support for default access_table.
 * 	[89/10/27  16:51:10  dpj]
 * 
 * Revision 2.2  89/03/17  12:35:17  sanzi
 * 	First cut.
 * 	[89/02/07  16:24:57  dpj]
 * 
 */

#ifndef	_access_table_ifc_h
#define	_access_table_ifc_h

#include <top_ifc.h>

extern "C" {
#include	<ns_types.h>
}

typedef struct ns_access_table_entry {
	mach_method_id_t	method_id;
	ns_access_t		access;
} *ns_access_table_t;

class access_table: public usTop {
	hash_table_t	table;
      private:
	mach_error_t setup_access_table(ns_access_table_t);
      public:
	DECLARE_LOCAL_MEMBERS(access_table);	
	access_table();
	access_table(mach_error_t*);
	virtual ~access_table();

	virtual mach_error_t ns_find_required_access(mach_method_id_t,
						     ns_access_t*);
};


/*
 * Macros for establishing default initializations.
 */

#define	begin_access_table(name)			\
	struct ns_access_table_entry name[] = {

#define	end_access_table(name)				\
	, {(mach_method_id_t)0, (ns_access_t)0}};

#define	access_entry(name,rights)			\
	{ mach_method_id(name),(rights)}


#endif	_access_table_ifc_h
