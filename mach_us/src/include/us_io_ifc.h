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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_io_ifc.h,v $
 *
 * usIO: abstract class defining the IO protocol. 
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY:
 * $Log:	us_io_ifc.h,v $
 * Revision 2.3  94/07/08  15:51:49  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:23:32  dpj
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:45:12  dpj]
 * 
 * Revision 2.1  90/11/14  17:44:11  pjg
 * Created.
 * 
 */

#ifndef	_us_io_h
#define	_us_io_h

#include <us_item_ifc.h>

extern "C" {
#undef __cplusplus
#include <io_types.h>
#define __cplusplus
}

class usIO: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS(usIO);
	static void initClass(usClass*);

REMOTE	virtual mach_error_t _io_read(int, io_offset_t, pointer_t, 
				      unsigned int*);
REMOTE	virtual mach_error_t _io_write(int, io_offset_t, pointer_t, 
				       unsigned int*);
REMOTE	virtual mach_error_t _io_append(int, pointer_t, unsigned int*);
REMOTE	virtual mach_error_t _io_set_size(io_size_t);
REMOTE	virtual mach_error_t _io_get_size(io_size_t *);
REMOTE	virtual mach_error_t _io_map(task_t, vm_address_t*, vm_size_t,
				     vm_offset_t, boolean_t, vm_offset_t,
				     boolean_t, vm_prot_t, vm_prot_t,
				     vm_inherit_t);
};

EXPORT_METHOD(io_read,
	      "rpc: IN int; IN word[4]; OUT * char[*:32768]; IN OUT * int;");
EXPORT_METHOD(io_write,
	      "rpc: IN int; IN word[4]; IN  * char[*:32768]; IN OUT * int;");
EXPORT_METHOD(io_append, "rpc: IN int;  IN  * char[*:32768]; IN OUT * int;");
EXPORT_METHOD(io_set_size, "rpc: IN word[4];");
EXPORT_METHOD(io_get_size,"rpc: OUT * word[4];");

/*
 * io_read(): 	read data from an io object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]
 *
 *	mode [io_mode_t] : specifies various read related io options.
 *
 *	start [io_offset_t] : starting byte location in object.
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
 *	start [io_offset_t] : starting byte location in object.
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
 * io_set_size():  set the size of an io object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]
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
 *	task [task_t]
 *	addr [vm_address_t]
 *	size [vm_size_t]
 *	mask [vm_offset_t]
 *	anywhere [boolean_t]
 *	paging_offset [vm_offset_t]
 *	copy [boolean_t]
 *	cprot, mprot [vm_prot_t]
 *	inherit [vm_inherit_t]
 *
 * Results:
 *	
 * Note:
 *
 */

#endif	_us_io_h
