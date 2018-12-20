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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_net_cots_recs.cc,v $
 *
 * usNetCOTS_recs: abstract class defining the basic connection-oriented,
 *	      record-oriented network protocol
 *
 * HISTORY
 * $Log:	us_net_cots_recs.cc,v $
 * Revision 2.3  94/07/07  17:25:23  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  14:07:32  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:57:41  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:42:53  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 */

#include <us_net_cots_recs_ifc.h>

DEFINE_ABSTRACT_CLASS_MI(usNetCOTS_recs)
DEFINE_CASTDOWN2(usNetCOTS_recs, usNetCOTS, usRecIO)

void usNetCOTS_recs::init_class(usClass* class_obj)
{
	typedef void(*XX)(usClass*);
	XX px;

//	usNetCOTS:init_class(class_obj);
	px = &usNetCOTS::init_class;
	(*px)(class_obj);
	usRecIO::init_class(class_obj);
}
