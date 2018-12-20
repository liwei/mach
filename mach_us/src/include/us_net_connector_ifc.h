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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_net_connector_ifc.h,v $
 *
 * usNetBase: abstract class defining the basic protocol for 
 *	      establishing network connections. 
 *
 * HISTORY:
 * $Log:	us_net_connector_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:43  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:48  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:50:57  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:01:46  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:29  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:17:20  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:37:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:44:21  pjg]
 * 
 */

#ifndef	_us_net_connector_h
#define	_us_net_connector_h

#include <us_net_base_ifc.h>

extern "C" {
#include <io_types.h>
#include <net_types.h>
}

class usNetConnector: public VIRTUAL2 usNetBase {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usNetConnector);

REMOTE	virtual mach_error_t net_connect(net_addr_t*, net_options_t*,
					 char*, unsigned int, char*,
					 unsigned int*, ns_prot_t,
					 unsigned int, ns_access_t,
					 usItem**, ns_type_t*) =0;
REMOTE	virtual mach_error_t net_listen(io_mode_t, net_addr_t*, 
					net_options_t*, char*,
					unsigned int*, int*) =0;
REMOTE	virtual mach_error_t net_accept(int, net_options_t*, char*,
					unsigned int, ns_prot_t,
					unsigned int, ns_access_t,
					usItem**, ns_type_t*) =0;
REMOTE	virtual mach_error_t net_reject(int, char*, unsigned int) =0;
REMOTE	virtual mach_error_t net_get_connect_qinfo(int*, int*) =0;
REMOTE	virtual mach_error_t net_set_connect_qmax(int) =0;

};

EXPORT_METHOD(net_connect);
EXPORT_METHOD(net_listen);
EXPORT_METHOD(net_accept);
EXPORT_METHOD(net_reject);
EXPORT_METHOD(net_get_connect_qinfo);
EXPORT_METHOD(net_set_connect_qmax);


/************************************************************************\
 *									*
 *		Connector operations for connection establishment	*
 *									*
\************************************************************************/

/*
 * net_connect():		create a new COTS endpoint
 *
 * Parameters:
 *	obj [mach_object_t]:		CLTS or connector endpoint on which
 *					to make the new connection
 *	peeraddr [net_addr_t *]:	peer address for the new endpoint
 *	options [net_options_t *]:	desired options associated
 *					with the connection
 *	in_udata [char *]:		user-specified data accompanying
 *					the connection request
 *	in_udatalen [int]:		length of in_udata
 *	prot [ns_prot_t]:		initial protection for the new endpoint
 *	protlen [int]:			length of prot structure
 *	access [ns_access_t]:		access desired for initial agent
 *
 * Results:
 *	options [net_options_t *]:	negotiated options associated
 *					with the connection
 *	out_udata [char *]:		user-specified data provided by
 *					the peer when accepting the connection
 *	out_udatalen [int *]:		length of out_udata
 *	newobj [mach_object_t *]:	proxy for the new endpoint
 *	newtype [ns_type_t]:		type of endpoint created
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_listen():		wait for a connection request on a connector
 *
 * Parameters:
 *	obj [mach_object_t]:		connector endpoint to receive the
 *					connection request
 *	mode [io_mode_t]:		options for this operation:
 *					IOM_WAIT: wait until a connection
 *						request arrives
 *
 * Results:
 *	peeraddr [net_addr_t *]:	peer address for the connection request
 *	options [net_options_t *]:	desired options associated
 *					with the connection
 *	udata [char *]:			user-specified data provided by
 *					with the connection request
 *	udatalen [int *]:		length of udata
 *	seqno [int *]:			sequence number idientifying this
 *					connection request, to be used
 *					in the response
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_accept():	accept a connection request and establish connection
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	seqno [int] :
 *
 *	options [net_options_t *] :
 *
 *	udata [char *] :
 *
 *	udatalen [unsigned int ]
 *
 *	prot [ns_prot_t] :
 *
 *	protlen [unsigned int] :
 *
 *	access [ns_access_t] :
 *
 *	newobj [usItem **] :
 *
 *	newtype [ns_type_t *]
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_reject():	reject a connection request
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	seqno [int] :
 *
 *	udata [char *] :
 *
 *	udatalen [unsigned int] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_get_connect_qinfo():	get the status of the queue of connection
 *				requests on a connector endpoint
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	qsize [int *] :
 *
 *	qmax [int *] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * net_set_connect_qmax():	set the maximum number of allowed connection
 *				requests on a connector endpoint
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	qmax [int] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

#endif	_us_net_connector_h


