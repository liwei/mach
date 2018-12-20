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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxstat_ifc.h,v $
 *
 *
 * HISTORY:
 * $Log:	uxstat_ifc.h,v $
 * Revision 2.7  94/07/08  16:02:30  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.6  94/05/17  14:09:00  jms
 * 	Need dummy  implementations of virtual methods in class uxstat 
 * 		for 2.3.3 g++ -modh
 * 	[94/04/28  19:02:09  jms]
 * 
 * Revision 2.5.1.1  94/02/18  11:35:44  modh
 * 	Need to delcare inherited virtual methods in class uxstat for 2.3.3 g++
 * 
 * Revision 2.5  92/07/05  23:33:03  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:33:16  dpj]
 * 
 * Revision 2.4  91/11/06  14:13:06  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:21:43  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:12:37  pjg]
 * 
 * Revision 2.3  90/01/02  22:18:26  dorr
 * 	add uxprot operations.
 * 
 * Revision 2.2.1.1  90/01/02  14:24:00  dorr
 * 	add internal access object.  add methods for
 * 	mode conversion.
 * 
 * Revision 2.2  89/06/30  18:36:53  dpj
 * 	Initial revision.
 * 	[89/06/29  00:50:21  dpj]
 * 
 */

#ifndef	_uxstat_ifc_h
#define	_uxstat_ifc_h

#include <clone_ifc.h>
#include <us_item_ifc.h>
#include <fs_access_ifc.h>

extern "C" {
#include <sys/types.h>
}


typedef struct map_entry {
	struct map_entry		*next;
	ns_mgr_id_t			id;
	dev_t				dev;
} *map_entry_t;


class uxstat: public usClone {
	struct mutex	lock;
	map_entry_t	head;
	map_entry_t	hint;
	dev_t		next_dev;
	fs_access*	access_obj;
      public:
	uxstat(fs_access* =0);
	virtual ~uxstat();

	mach_error_t uxstat_std_to_unix_mgrid(ns_mgr_id_t, dev_t*);
	mach_error_t uxstat_std_to_unix_attr(ns_attr_t, struct stat*);
	mach_error_t uxprot_unix_to_std_prot(int,int,int,ns_acl_entry_t,int*);
	mach_error_t uxprot_std_to_unix_prot(ns_acl_entry_t,int,int*,int*,int*);

	virtual mach_error_t clone_init(mach_port_t) { return ERR_SUCCESS; }
	virtual mach_error_t clone_abort(mach_port_t) { return ERR_SUCCESS; }
	virtual mach_error_t clone_complete() { return ERR_SUCCESS; }
        virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
        virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
        virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
        virtual mach_error_t ns_set_times(time_value_t, time_value_t);
        virtual mach_error_t ns_get_protection(ns_prot_t, int*);
        virtual mach_error_t ns_set_protection(ns_prot_t, int);
        virtual mach_error_t ns_get_privileged_id(int*);
        virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);
        virtual mach_error_t ns_get_manager(ns_access_t, usItem **);
};

#endif	_uxstat_ifc_h
