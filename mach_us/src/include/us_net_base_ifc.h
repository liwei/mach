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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_net_base_ifc.h,v $
 *
 * usNetBase: abstract class defining the basic network protocol. 
 *
 * HISTORY:
 * $Log:	us_net_base_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:37  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:42  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:46:10  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:01:33  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:15  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:11:27  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:37:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:44:21  pjg]
 * 
 */

#ifndef	_us_net_base_h
#define	_us_net_base_h

#include <us_item_ifc.h>

extern "C" {
#include <net_types.h>
}

class usNetBase: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usNetBase);

REMOTE	virtual mach_error_t net_get_localaddr(net_addr_t*) =0;
};

EXPORT_METHOD(net_get_localaddr);


/************************************************************************\
 *									*
 *		General-purpose operations				*
 *									*
\************************************************************************/

/*
 * net_get_localaddr():		return the local address associated
 *					with an endpoint
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	addr [net_addr_t*] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

#endif	_us_net_base_h


