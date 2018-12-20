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
 * ObjectClass: std_name
 *	User-side object that implements basic prefix and naming operations
 *
 * SuperClass: base
 *
 * Delegated Objects: none
 *
 * ClassMethods:
 *	Exported: 
 *	ns_resolve_fully(),
 *	ns_set_system_prefix(),
 *	ns_set_user_prefix(),
 *	ns_set_token()
 *
 * Notes:
 */ 
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_name_ifc.h,v $
 *
 * Purpose: Generic protection object
 *
 * HISTORY:
 * $Log:	std_name_ifc.h,v $
 * Revision 2.8  94/07/07  17:24:36  mrt
 * 	Updated copyright.
 * 
 * Revision 2.7  94/05/17  14:08:01  jms
 * 	Needed declarations of virtual functions in class std_name for 2.3.3 g++ -modh
 * 	[94/04/28  18:52:53  jms]
 * 
 * Revision 2.5.1.1  94/02/18  11:28:32  modh
 * 	Needed declarations of virtual functions in class std_name for 2.3.3 g++
 * 
 * Revision 2.6  94/04/29  15:47:27  jms
 * 	Made changes to the prefix caching logic, so that the name
 * 	resolution code decides what it wants to cache before it
 * 	starts the resolution.
 * 	[94/04/26  16:36:21  grm]
 * 
 * Revision 2.5  92/07/05  23:28:59  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:08:57  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:12:56  dpj]
 * 
 * Revision 2.4  91/12/20  17:44:25  jms
 * 	Modify signitures of ns_*prefix and ns_resolve* to enable types in prefix
 * 	table for non directories like /dev/null.  (from dpj)
 * 	[91/12/20  15:59:12  jms]
 * 
 * Revision 2.3  91/11/13  17:18:01  dpj
 * 	No changes.
 * 	[91/11/12  17:54:17  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:03  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:56:20  pjg]
 * 
 * Revision 2.4.2.2  91/04/14  18:35:18  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 2.4.2.1  90/11/14  17:11:13  pjg
 * 	Initial C++ revision.
 * 
 * Revision 2.5  90/12/19  11:05:41  jjc
 * 	Added identity version counter.
 * 	[90/09/26            jjc]
 * 
 * Revision 2.4  89/06/30  18:35:37  dpj
 * 	Merged with the mainline.
 * 	[89/05/31  17:45:48  dpj]
 * 
 * 	First cut at complete handling of complex path names, with ".", "..",
 * 	and caching in thee user prefix table.
 * 	[89/05/26  16:17:11  dpj]
 * 
 * Revision 2.3  89/05/17  16:44:11  dorr
 * 	ns_login -> ns_set_token
 * 	[89/05/15  12:18:00  dorr]
 * 
 * Revision 2.2  89/03/17  12:50:01  sanzi
 * 	add clone_init.
 * 	[89/03/07  11:22:34  dorr]
 * 	
 * 	add paths and a token.
 * 	[89/02/25  13:47:03  dorr]
 * 	
 * 	Created.
 * 	[89/02/20  14:41:06  dorr]
 * 
 */

#ifndef	_std_name_ifc_h
#define	_std_name_ifc_h

#include <clone_ifc.h>
#include <us_item_ifc.h>

extern "C" {
#include <ns_types.h>
}


/*
 * Number of entries in the prefix table.
 */
#define	PFX_TABLE_SIZE		10


class std_name: public usClone {
	struct pfx_entry	*pfx_table[PFX_TABLE_SIZE];
	int			pfx_table_len;
	int			last_index;
	struct pfx_entry	*null_prefix;
	ns_token_t		token;
	int			identity_version;
      public:
	DECLARE_LOCAL_MEMBERS(std_name);
	std_name();
	~std_name();

	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_complete();
	virtual mach_error_t clone_abort(mach_port_t);

	virtual mach_error_t ns_set_token(mach_port_t);

	pointer_t    ns_get_prefix_entry(char*, unsigned int, boolean_t);
	mach_error_t ns_set_system_prefix(char*, usItem*, ns_type_t, char*);
	mach_error_t ns_set_user_prefix(char*, char*);
	mach_error_t ns_set_cache_prefix(char*, usItem*, ns_type_t, char*);
	mach_error_t ns_invalidate_prefix(char*, unsigned int);
	mach_error_t ns_flush_cache_prefixes();
	mach_error_t ns_find_prefix(char*, usItem**, ns_type_t*, 
					unsigned int*, char*);

	mach_error_t ns_resolve_simple(usItem**, ns_type_t*, char**, char**,
				       char**,
				       usItem**, char**, char**, 
				       ns_type_t*, char*, char*, ns_mode_t, 
				       ns_access_t, unsigned int);
	mach_error_t ns_resolve_complex(usItem**, ns_type_t*, char**, char**,
					char**,
					usItem**, char**, ns_type_t*, 
					char*, char*, ns_mode_t, ns_access_t,
					unsigned int);
	mach_error_t ns_resolve_fully(char*, ns_mode_t, ns_access_t, 
				      usItem**, ns_type_t*, char*);

        virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
        virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
        virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
        virtual mach_error_t ns_set_times(time_value_t, time_value_t);
        virtual mach_error_t ns_get_protection(ns_prot_t, int*);
        virtual mach_error_t ns_set_protection(ns_prot_t, int);
        virtual mach_error_t ns_get_privileged_id(int*);
        virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);
        virtual mach_error_t ns_get_manager(ns_access_t, usItem **);
      private:
	mach_error_t reauthenticate(struct pfx_entry*);
};

#endif	_std_name_ifc_h

