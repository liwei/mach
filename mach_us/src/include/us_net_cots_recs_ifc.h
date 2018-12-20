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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_net_cots_recs_ifc.h,v $
 *
 * usNetCOTS_recs: abstract class defining the basic connection-oriented,
 *	      record-oriented network protocol
 *
 * HISTORY:
 * $Log:	us_net_cots_recs_ifc.h,v $
 * Revision 2.4  94/07/08  15:51:56  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:23:57  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:51:51  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:38  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:19:16  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:37:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:44:21  pjg]
 * 
 */

#ifndef	_us_net_cots_recs_h
#define	_us_net_cots_recs_h

#include <us_net_cots_ifc.h>
#include <us_recio_ifc.h>


class usNetCOTS_recs: public VIRTUAL2 usNetCOTS, public VIRTUAL2 usRecIO {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usNetCOTS_recs);
};


#endif	_us_net_cots_recs_h


