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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_event.cc,v $
 *
 * usEvent: abstract class defining the event protocol.
 *
 * HISTORY
 * $Log:	us_event.cc,v $
 * Revision 2.6  94/07/15  15:17:13  mrt
 * 	Changed the timeout value for event_post_with_timeout
 * 	from 300ms to 600ms.  Removed the intr field as well,
 * 	since it was never used before (see changes to
 * 	method_info.cc)
 * 	[94/07/08  22:00:00  grm]
 * 
 * Revision 2.5  94/07/07  17:25:08  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/06  07:54:11  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.3  92/07/05  23:29:56  dpj
 * 	Change parent class to usItem
 * 	[92/06/24  16:34:00  jms]
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:18:20  dpj]
 * 
 * 	Specified that some methods are interruptible.
 * 	[92/05/10  01:19:00  dpj]
 * 
 * Revision 2.2  91/11/06  13:49:35  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  13:51:07  pjg]
 * 
 * 	Created.
 * 	[91/04/14  18:41:14  pjg]
 * 
 */

#include <us_event_ifc.h>

#ifdef	GXXBUG_CLONING2
#ifdef	GXXBUG_CLONING1
#define BASE usClone
DEFINE_ABSTRACT_CLASS(usEvent);
#else	GXXBUG_CLONING1
DEFINE_ABSTRACT_CLASS_MI(usEvent);
DEFINE_CASTDOWN2(usEvent,usClone,usItem);
#endif	GXXBUG_CLONING1
#else	GXXBUG_CLONING2
#define BASE usItem
DEFINE_ABSTRACT_CLASS(usEvent);
#endif	GXXBUG_CLONING2

DEFINE_METHOD_ARGS(event_post,"K<101301>: IN p(COPY_SEND); IN int; IN int; IN int;");
DEFINE_METHOD_ARGS(event_post_with_timeout, "rpc<600> K<101302>: IN p(COPY_SEND); IN int; IN int; IN int; IN int;");

