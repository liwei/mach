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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_byteio_ifc.h,v $
 *
 * usByteIO: abstract class defining the byte-oriented IO protocol. 
 *
 * HISTORY:
 * $Log:	us_byteio_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:26  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:25  dpj
 * 	Conditionalized virtual base class specification.
 * 	Added EXPORT for io_map().
 * 	[92/06/24  13:25:01  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:02  jms
 * 	Initial C++ revision.
 * 	[91/09/26  17:23:37  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:37:06  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:44:21  pjg]
 * 
 */

#ifndef	_us_byteio_h
#define	_us_byteio_h

#include <us_item_ifc.h>

extern "C" {
#include <io_types.h>
}

class usByteIO: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usByteIO);

REMOTE	virtual mach_error_t io_read(io_mode_t, io_offset_t, pointer_t, 
				     unsigned int*) =0;
REMOTE	virtual mach_error_t io_write(io_mode_t, io_offset_t, pointer_t, 
				      unsigned int*) =0;
REMOTE	virtual mach_error_t io_append(io_mode_t, pointer_t, unsigned int*) =0;
REMOTE	virtual mach_error_t io_read_seq(io_mode_t, char*, unsigned int*,
					 io_offset_t*) =0;
REMOTE	virtual mach_error_t io_write_seq(io_mode_t, char*, unsigned int*,
					  io_offset_t*) =0;
REMOTE	virtual mach_error_t io_set_size(io_size_t) =0;
REMOTE	virtual mach_error_t io_get_size(io_size_t *) =0;
REMOTE	virtual mach_error_t io_map(task_t, vm_address_t*, vm_size_t,
				    vm_offset_t, boolean_t, vm_offset_t,
				    boolean_t, vm_prot_t, vm_prot_t,
				    vm_inherit_t) =0;
};

EXPORT_METHOD(io_read);
EXPORT_METHOD(io_write);
EXPORT_METHOD(io_read_seq);
EXPORT_METHOD(io_write_seq);
EXPORT_METHOD(io_append);
EXPORT_METHOD(io_set_size);
EXPORT_METHOD(io_get_size);
EXPORT_METHOD(io_map);


/************************************************************************\
 *									*
 *		Byte-level I/O operations				*
 *									*
\************************************************************************/

/*
 * io_read(): 	read data from an io object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	mode [io_mode_t] : specifies various read related io options.
 *
 *	offset [io_offset_t] : starting byte location in object.
 *
 *	address [pointer_t] : pre-allocated memory of size 'count'
 *
 *	count [io_count_t *] : byte size of requested amount.
 *
 * Results:
 *
 *	If the IOM_PROBE flag is not set, then data is
 *	transferred into the user provided buffer on return.
 *	The 'inout' count parameter is set with the amount of
 *	data returned.
 *
 *	If the IOM_PROBE flag is set, then no data is transferred
 *	but the 'inout' count parameter is set with the length
 *	that would be returned.
 *
 * Side effects:
 *	None
 *
 * Note:
 *	When the IOM_PROBE bit is set, address should be NULL.
 *
 */

/*
 * io_write():  write data to an io object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	mode [io_mode_t] : specifies various read related io options.
 *
 *	offset [io_offset_t] : starting byte location in object.
 *
 *	address [pointer_t] : data buffer to be written.
 *
 *	count [io_count_t *] : byte size of requested amount.
 *
 * Results:
 *
 *	If the IOM_PROBE flag is not set, then data is
 *	transferred to the object.  The 'inout' count parameter is
 *	set with the amount of data written.
 *
 *	If the IOM_PROBE flag is set, then no data is transferred
 *	but the 'inout' count parameter is set with the length
 *	that would be written.
 *
 * Side effects:
 *	None
 *
 * Note:
 *	When the IOM_PROBE bit is set, address should be NULL.
 *
 */

/*
 * io_append():  append data to the end of an io object.
 *
 * 	This method is obsolete. It should be replaced by io_write_seq().
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	mode [io_mode_t] : specifies various read related io options.
 *
 *	address [pointer_t] : data buffer to be written.
 *
 *	count [io_count_t] : byte size of requested amount.
 *
 * Results:
 *
 *	If the IOM_PROBE flag is not set, then data is
 *	transferred to the object.  The 'inout' count parameter is
 *	set with the amount of data written.
 *
 *	If the IOM_PROBE flag is set, then no data is transferred
 *	but the 'inout' count parameter is set with the length
 *	that would be written.
 *
 * Side effects:
 *	None
 *
 * Note:
 *	When the IOM_PROBE bit is set, address should be NULL.
 *
 */
	
/*
 * io_read_seq():	read data from an I/O object -- sequential access
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	mode [io_mode_t] :
 *
 * 	addr [char*] :
 *
 *	num [unsigned int *] :
 *
 *	offset[io_offset_t*] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * io_write_seq():	write data to an I/O object -- sequential access
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	mode [io_mode_t] :
 *
 *	addr [char *] :
 *
 *	num [unsigned int *] :
 *
 *	offset [io_offset_t *] :
 *
 * Results:
 *
 * Side effects:
 *
 * Note:
 */

/*
 * io_set_size():  set the size of an io object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	size [io_size_t]: new size of the object
 *
 * Results:
 *
 *	Sets the size of the io_object.  Not that
 *	setting the size of some io objects does
 *	not make any sense and will fail.
 * Side effects:
 *
 * Note:
 *
 */

/*
 * io_get_size():  get the size of an io object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	size [io_size_t]: new size of the object
 *
 * Results:
 *
 *	Sets the size of the io_object.  Not that
 *	setting the size of some io objects does
 *	not make any sense and will fail.
 * Side effects:
 *
 * Note:
 *
 */

/*
 * io_map():  map an io object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	task [task_t]
 *
 *	addr [vm_address_t]
 *
 *	size [vm_size_t]
 *
 *	mask [vm_offset_t]
 *
 *	anywhere [boolean_t]
 *
 *	paging_offset [vm_offset_t]
 *
 *	copy [boolean_t]
 *
 *	cprot, mprot [vm_prot_t]
 *
 *	inherit [vm_inherit_t]
 *
 * Results:
 *	
 * Note:
 *
 */

#endif	_us_byteio_h
