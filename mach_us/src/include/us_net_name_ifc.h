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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_net_name_ifc.h,v $
 *
 * usName: abstract class defining the naming protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY:
 * $Log:	us_net_name_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:46  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:59  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:52:04  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:41  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:20:08  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:38:29  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:45:06  pjg]
 * 
 */

#ifndef	_us_net_name_h
#define	_us_net_name_h

#include <us_name_ifc.h>
#include <us_net_base_ifc.h>

extern "C" {
#include <net_types.h>
}

class usNetName: public VIRTUAL2 usName, public VIRTUAL2 usNetBase {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usNetName);

REMOTE	virtual mach_error_t net_create(net_addr_t *, int *, ns_prot_t, int,
					ns_access_t, usItem **, ns_type_t *,
					net_info_t *) =0;
REMOTE	virtual mach_error_t net_lookup(net_addr_t *, ns_access_t, usItem **,
					ns_type_t *, net_info_t *) =0;
REMOTE	virtual mach_error_t net_cots_lookup(net_addr_t *, net_addr_t *,
					     ns_access_t, usItem **,
					     ns_type_t *, net_info_t *) =0;
};

EXPORT_METHOD(net_create);
EXPORT_METHOD(net_lookup);
EXPORT_METHOD(net_cots_lookup);


/************************************************************************\
 *									*
 *		Operation on protocol directories			*
 *									*
\************************************************************************/

/*
 * net_create():	create a new CLTS or connector endpoint
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	localaddr [net_addr_t *] : INOUT 
 *
 *	qmax [int *] : INOUT
 *
 *	prot [ns_prot_t] :
 *
 *	protlen [int] :
 *
 *	access [ns_access_t] :
 *
 *	newobj [usItem **] : OUT
 *
 *	newtype [ns_type_t *] : OUT
 *
 *	info [net_info_t *] : OUT
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_lookup():	look-up an existing CLTS or connector endpoint
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	localaddr [net_addr_t *] : INOUT
 *
 *	access [ns_access_t] :
 *
 *	newobj [usItem **] :  OUT
 *
 *	newtype [ns_type_t *] : OUT
 *
 *	info [net_info_t *] : OUT
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_cots_lookup():	look-up an existing COTS endpoint
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	localaddr [net_addr_t *] : INOUT 
 *
 *	peeraddr [net_addr_t *] :
 *
 *	access [ns_access_t]
 *
 *	newobj [usItem **] : OUT
 *
 *	newtype [ns_type_t *] : OUT
 *
 *	info [net_info_t *] : OUT
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

#endif	_us_net_name_h
