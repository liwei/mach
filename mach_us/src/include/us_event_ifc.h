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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_event_ifc.h,v $
 *
 * usEvent: abstract class defining the event protocol.
 *
 * HISTORY:
 * $Log:	us_event_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:28  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:30  dpj
 * 	Make usEvent derive from usItem vs usRemote
 * 	Export event_post_with_timeout (bugfix)
 * 	[92/06/24  13:40:47  jms]
 * 	Conditionalized virtual base class specification.
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  15:43:13  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:05  jms
 * 	Initial C++ revision.
 * 	[91/09/26  17:54:22  pjg]
 * 
 * 	Created.
 * 	[91/04/15  14:40:58  pjg]
 * 
 */

#ifndef	_us_event_h
#define	_us_event_h

#include <us_item_ifc.h>

extern "C" {
#include <cthreads.h>
}

/*
 * Event operations
 */
#ifdef	GXXBUG_CLONING2
#include	<clone_ifc.h>
#ifdef	GXXBUG_CLONING1
class usEvent: public VIRTUAL2 usClone {
#else	GXXBUG_CLONING1
class usEvent: public VIRTUAL2 usClone, public VIRTUAL2 usItem {
#endif	GXXBUG_CLONING1
#else	GXXBUG_CLONING2
class usEvent: public VIRTUAL2 usItem {
#endif	GXXBUG_CLONING2
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usEvent);

REMOTE	virtual mach_error_t event_post(thread_t, mach_error_t, int, int) =0;
REMOTE	virtual mach_error_t event_post_with_timeout(thread_t, mach_error_t, int, int, int) =0;
};

EXPORT_METHOD(event_post);
EXPORT_METHOD(event_post_with_timeout);

/*
 * event_post():
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	[thread_t] :
 *
 *	[mach_error_t] :
 *
 *	[int] :
 *
 *	[int] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */

/*
 * event_post_with_timeout():
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	[thread_t] :
 *
 *	[mach_error_t] :
 *
 *	[int] :
 *
 *	[int] :
 *
 *	[int] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 *
 */

#endif _us_event_h
