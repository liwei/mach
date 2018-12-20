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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_net_clts_ifc.h,v $
 *
 * usNetCLTS: abstract class defining the basic connection-less 
 *	      network protocol. 
 *
 * HISTORY:
 * $Log:	us_net_clts_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:42  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:44  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:49:54  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:21  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:15:18  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:37:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:44:21  pjg]
 * 
 */

#ifndef	_us_net_clts_h
#define	_us_net_clts_h

#include <us_net_connector_ifc.h>

/*
 * XXX C++ Methods net_clts_read1rec and net_write1rec should be
 * defined in usNetCLTS_recs. That doesn't work due to g++ bugs
 * dealing with virtual base classes. Sigh.
 */

class usNetCLTS: public VIRTUAL2 usNetConnector {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usNetCLTS);

REMOTE	virtual mach_error_t net_clts_read1rec(io_mode_t, char*, unsigned int*,
					       io_recnum_t*, net_addr_t*,
					       net_options_t*) =0;
REMOTE	virtual mach_error_t net_clts_write1rec(io_mode_t, char*, unsigned int,
						io_recnum_t*, net_addr_t*,
						net_options_t*) =0;
};

EXPORT_METHOD(net_clts_read1rec);
EXPORT_METHOD(net_clts_write1rec);


/************************************************************************\
 *									*
 *	Operations on connection-less transport endpoints (CLTS)	*
 *									*
\************************************************************************/

/*
 * net_clts_read1rec():		receive one unit of data on a clts endpoint
 *		-- copy to a user buffer
 *
 * Parameters:
 * 
 *	obj [mach_object_t] :
 *
 *	mode [io_mode_t] :
 *
 *	buf [char*] :
 *
 *	len [unsigned int*] :
 *
 *	recnum [io_recnum_t*] :
 *
 *	peeraddr [net_addr_t*] :
 *
 *	options [net_options_t*] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */
	
/*
 * net_clts_write1rec():	send one unit of data on a clts endpoint
 *		-- copy from a user buffer
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	mode [io_mode_t] :
 *
 *	buf [char*] :
 *
 *	len [unsigned int] :
 *
 *	recnum [io_recnum_t*] :   OUT
 *
 *	peeraddr [net_addr_t*] :
 *
 *	options [net_options_t*] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */


#endif	_us_net_clts_h


