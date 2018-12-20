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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_recio_ifc.h,v $
 *
 * usRecIO: abstract class defining the record-oriented IO protocol. 
 *
 * HISTORY:
 * $Log:	us_recio_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:47  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:24:01  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:52:18  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:44  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:21:39  pjg]
 * 
 */

#ifndef	_us_recio_h
#define	_us_recio_h

#include <us_item_ifc.h>

extern "C" {
#include <io_types.h>
}

class usRecIO: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usRecIO);

REMOTE	virtual mach_error_t io_read1rec(io_mode_t, io_recnum_t, char*,
					 unsigned int*) =0;
REMOTE	virtual mach_error_t io_write1rec(io_mode_t, io_recnum_t, char*,
					  unsigned int) =0;
REMOTE	virtual mach_error_t io_read1rec_seq(io_mode_t, char*, unsigned int*,
					     io_recnum_t*) =0;
REMOTE	virtual mach_error_t io_write1rec_seq(io_mode_t, char*, unsigned int,
					      io_recnum_t*) =0;
REMOTE	virtual mach_error_t io_get_record_count(io_recnum_t*) =0;
};

EXPORT_METHOD(io_read1rec);
EXPORT_METHOD(io_write1rec);
EXPORT_METHOD(io_read1rec_seq);
EXPORT_METHOD(io_write1rec_seq);
EXPORT_METHOD(io_get_record_count);


/************************************************************************\
 *									*
 *		Record-level I/O operations				*
 *									*
\************************************************************************/

/*
 * io_read1rec():	read one data record into a buffer / random access.
 *
 * Parameters:
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */
	
/*
 * io_write1rec():	write one data record from a buffer / random access.
 *
 * Parameters:
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */
	
/*
 * io_read1rec_seq():	read one data record into a buffer / sequential access.
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	mode [io_mode_t] :
 *
 *	buf [char *] :
 *
 *	len [unsigned int *] :
 *
 *	recnum [io_recnum_t *] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */
	
/*
 * io_write1rec_seq():	write one data record from a buffer / sequential access.
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	mode [io_mode_t] :
 *
 *	buf [char *] :
 *
 *	len [unsigned int] :
 *
 *	recnum [io_recnum_t *] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */
	
/*
 * io_get_record_count():	return the number of records available
 *				in a I/O object.
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	count [io_recnum_t *] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

#endif	_us_recio_h


