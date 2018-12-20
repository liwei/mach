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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/clone_ifc.h,v $
 *
 * Purpose: usClone: Abstract class for objects that can be cloned
 *	    across address spaces.
 *
 * HISTORY:
 * $Log:	clone_ifc.h,v $
 * Revision 2.3  94/07/08  15:49:29  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  94/01/11  17:48:21  jms
 * 	Moved from .../lib/us++/clone_ifc.h
 * 	[94/01/09  18:19:18  jms]
 * 
 * Revision 2.2  92/07/05  23:26:56  dpj
 * 	First working version.
 * 	[92/06/24  16:07:48  dpj]
 * 
 */

#ifndef _clone_h
#define _clone_h

#include <top_ifc.h>


#ifdef	GXXBUG_CLONING1
#include <us_item_ifc.h>
class usClone: public VIRTUAL2 usItem {
#else	GXXBUG_CLONING1
class usClone: public VIRTUAL2 usRemote {
#endif	GXXBUG_CLONING1
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usClone);

	virtual mach_error_t clone_init(mach_port_t)=0;
	virtual mach_error_t clone_abort(mach_port_t)=0;
	virtual mach_error_t clone_complete()=0;
};

#endif	_clone_h
