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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/clone_master_ifc.h,v $
 *
 * Purpose: list of clone-able objects
 *
 * HISTORY: 
 * $Log:	clone_master_ifc.h,v $
 * Revision 2.4  94/07/07  17:23:04  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:27:01  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  16:08:29  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:08:24  dpj]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:23:46  pjg]
 * 
 * Revision 2.2  91/11/06  13:45:40  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:49:37  pjg]
 * 
 * 	initial checkin
 * 	[89/12/19  17:12:13  dorr]
 * 
 * Revision 2.2  90/01/02  22:09:47  dorr
 * 	clones!
 * 
 */

#ifndef	_clone_master_ifc_h
#define	_clone_master_ifc_h


#include <top_ifc.h>

class usClone;

extern "C" {
#include <dll.h>
}

typedef struct clone {
	dll_chain_t	qp;
	usClone*	obj;
} * clone_t;


class clone_master: public usTop {
	dll_head_t	q;
      public:
	clone_master();
	virtual ~clone_master();
	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_complete();
	mach_error_t list_add(usClone*);
	mach_error_t list_delete(usClone*);
};

#endif	_clone_master_ifc_h
