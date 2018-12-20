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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_net_cots_ifc.h,v $
 *
 * usNetCOTS: abstract class defining the basic connection-oriented
 *	      network protocol
 *
 * HISTORY:
 * $Log:	us_net_cots_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:45  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:54  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:51:39  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:02:07  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:35  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:18:42  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:37:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:44:21  pjg]
 * 
 */

#ifndef	_us_net_cots_h
#define	_us_net_cots_h

#include <us_net_base_ifc.h>

extern "C" {
#include <net_types.h>
}

class usNetCOTS: public VIRTUAL2 usNetBase {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usNetCOTS);

REMOTE	virtual mach_error_t net_get_peeraddr(net_addr_t*) =0;
REMOTE	virtual mach_error_t net_snddis(char*, unsigned int) =0;
REMOTE	virtual mach_error_t net_rcvdis(char*, unsigned int*) =0;
};

EXPORT_METHOD(net_get_peeraddr);
EXPORT_METHOD(net_snddis);
EXPORT_METHOD(net_rcvdis);


/************************************************************************\
 *									*
 *	Operations on connection-oriented transport endpoints (COTS)	*
 *									*
\************************************************************************/

/*
 * net_get_peeraddr():		return the peer address associated
 *					with a COTS endpoint
 *
 * Parameters:
 * 
 *	obj [mach_object_t] :
 *
 *	addr [net_addr_t *] : the output parameter for the peer address.
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_snddis():	send an abortive connection release indication
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	udata [char *] :
 *
 *	udatalen [int] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_rcvdis():	receive an abortive connection release indication
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	udata [char *] :
 *
 *	udatalen [int] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

#endif	_us_net_cots_h


