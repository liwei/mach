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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/mf_user_ifc.h,v $
 *
 * HISTORY
 * $Log:	mf_user_ifc.h,v $
 * Revision 2.4  94/07/07  17:23:43  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:04  dpj
 * 	Converted for use as a "property" instead of a full base class.
 * 	[92/06/24  16:31:26  dpj]
 * 
 * 	No changes.
 * 	[92/05/10  00:55:33  dpj]
 * 
 * Revision 2.2  91/11/06  13:46:51  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  14:53:51  pjg]
 * 
 * Revision 1.7.1.2  91/04/14  18:28:50  pjg
 * 	Upgraded to US38
 * 
 * 
 * Revision 1.7.1.1  90/11/14  17:08:59  pjg
 * 	Initial C++ revision.
 * 
 * 
 */

#ifndef	_mf_user_ifc_h
#define	_mf_user_ifc_h

#include <mf_mem_ifc.h>
#include <usint_mf_ifc.h>

class usRemote;

class mf_user: public mf_mem {
	usRemote*		rem_object;	/* !! NO REFERENCE !! */

      public:
				mf_user();	/* ??? */
				mf_user(usRemote*);

      protected:
	virtual mach_error_t	init_upcall();
	virtual usRemote*	remote_object() { return rem_object; }
};


/*
 * Macro for inclusion in a class declaration.
 */
#define	DECLARE_MF_USER_PROP(field_name)				\
      private:								\
	mf_user		field_name;					\
      public:								\
	virtual mach_error_t	io_read(				\
					io_mode_t	mode,		\
					io_offset_t	start,		\
					pointer_t	addr,		\
					unsigned int*	num)		\
			{ return field_name.io_read(mode,start,addr,num); }\
	virtual mach_error_t 	io_write(				\
					io_mode_t	mode,		\
					io_offset_t	start,		\
					pointer_t	addr,		\
					unsigned int*	num)		\
			{ return field_name.io_write(mode,start,addr,num); }\
	virtual mach_error_t	io_append(				\
					io_mode_t	mode,		\
					pointer_t	addr,		\
					unsigned int*	num)		\
			{ return field_name.io_append(mode,addr,num); }	\
	virtual mach_error_t	io_set_size(io_size_t size)		\
			{ return field_name.io_set_size(size); }	\
	virtual mach_error_t	io_get_size(io_size_t* size)		\
			{ return field_name.io_get_size(size); }	\
	virtual mach_error_t	io_map(					\
					task_t		task,		\
					vm_address_t*	addr,		\
					vm_size_t	size, 		\
					vm_offset_t	mask,		\
					boolean_t	anywhere,	\
					vm_offset_t	paging_offset,	\
					boolean_t	copy,		\
					vm_prot_t	cprot,		\
					vm_prot_t	mprot, 		\
					vm_inherit_t	inherit)	\
			{ return field_name.io_map(task,addr,size,mask,	\
					anywhere,paging_offset,copy,	\
					cprot,mprot,inherit); }


/*
 * Macro for inclusion in a init_class() method.
 */
#define	SETUP_MF_USER_PROP(class_name)					\
        SETUP_METHOD_WITH_ARGS(class_name,io_read);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_write);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_append);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_set_size);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_get_size);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_map);			\
	method_info::GLOBAL->_define_method(mach_method_id(io_get_mf_state));


#endif	_mf_user_ifc_h

