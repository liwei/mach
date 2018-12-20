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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_tty_ifc.h,v $
 *
 * Purpose: Generic tty_bsd_proxy object.
 *
 * HISTORY:
 * $Log:	us_tty_ifc.h,v $
 * Revision 2.3  94/07/08  15:54:49  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.2  94/01/11  17:48:24  jms
 * 	Initial Version
 * 	[94/01/09  18:20:09  jms]
 * 
 * Revision 2.3  92/07/05  23:29:45  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  17:17:09  dpj]
 * 
 * Revision 2.2  91/11/06  13:49:17  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:58:24  pjg]
 * 
 * 	Created.
 * 	[91/04/14  18:39:54  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:16:14  pjg]
 * 
 */

#ifndef	_us_tty_ifc_h
#define	_us_tty_ifc_h

#include <us_byteio_ifc.h>

class usTTY: public VIRTUAL2 usByteIO {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usTTY);

REMOTE	virtual mach_error_t tty_bsd_ioctl(int, char*) =0;
};

EXPORT_METHOD(tty_bsd_ioctl);

/*
 * tty_bsd_ioctl():
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	cmd [int] :
 *
 *	arg [char *] : inout
 *
 */

#endif	_us_tty_ifc_h

