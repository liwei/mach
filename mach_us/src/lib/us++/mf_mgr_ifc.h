/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992 Carnegie Mellon University
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
 * HISTORY
 * $Log:	mf_mgr_ifc.h,v $
 * Revision 2.4  94/07/27  14:40:30  mrt
 * 	Added a constructor for the mf_mgr that takes a
 * 	usItem * as a parameter to initialize the mapping's
 * 	agency pointer.
 * 	[94/07/27  14:11:36  grm]
 * 
 * Revision 2.3  94/07/07  17:23:40  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:27:59  dpj
 * 	Converted for use as a "property" instead of a full base class.
 * 	[92/06/24  16:30:26  dpj]
 * 
 * Revision 1.7  91/07/01  14:12:13  jms
 * 	Added io_set_size method.
 * 	[91/06/05  13:54:44  roy]
 * 
 * Revision 1.6  89/10/30  16:34:11  dpj
 * 	Reorganization of MF system.
 * 	[89/10/27  19:02:04  dpj]
 * 
 * Revision 1.5  89/03/17  12:46:55  sanzi
 * 	Add 
 * 		declare_method(mf_get_client_state, mf_mgr, mach_error_t);
 * 		declare_method(mf_terminate_mapped_access, mf_mgr, boolean_t);
 * 	[89/03/15  17:02:19  sanzi]
 * 	
 * 	Add changes to support mf_cat.
 * 	[89/03/11  16:26:54  sanzi]
 * 	
 * 	Intermediate check in.  Do not use.
 * 	[89/03/10  10:10:18  sanzi]
 * 	
 * 	Added 
 * 		declare_method(ns_unregister_agent, mf_mgr, mach_error_t);	
 * 	[89/03/02  13:58:40  sanzi]
 * 	
 * 	Check onto branch.
 * 	[89/02/24  17:29:03  sanzi]
 * 
 */

#ifndef	_mf_mgr_ifc_h
#define	_mf_mgr_ifc_h

#include	<mf_mem_ifc.h>
#include	<us_item_ifc.h>

class pager_base;
class usRemote;

class mf_mgr: public mf_mem {
	boolean_t		ready;
	struct mutex		lock;
	pager_base*		backing_obj;	/* !! NO REFERENCE !! */

      protected:
	virtual mach_error_t	init_upcall();
	virtual usRemote*	remote_object();

      public:
				mf_mgr();
				mf_mgr(usItem *);

	mach_error_t		mf_mgr_start(pager_base*);

	mach_error_t		io_clean();
	mach_error_t		io_deactivate(boolean_t);

	virtual mach_error_t	io_get_mf_state(ns_access_t*,
						io_size_t*,mach_port_t*);
	virtual mach_error_t	io_set_size(io_size_t);
};

#define	mf_mgr_lock()		mutex_lock(&Local(lock));
#define	mf_mgr_unlock()		mutex_unlock(&Local(lock));


/*
 * Macro for inclusion in a class declaration.
 */
#define	DECLARE_MF_MGR_PROP(field_name)					\
      private:								\
	mf_mgr		field_name;					\
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
	virtual mach_error_t	io_get_mf_state(			\
					ns_access_t*	access,		\
					io_size_t*	size,		\
					mach_port_t*	pager)		\
		{ return field_name.io_get_mf_state(access,size,pager); }


/*
 * Macro for inclusion in a init_class() method.
 */
#define	SETUP_MF_MGR_PROP(class_name)					\
        SETUP_METHOD_WITH_ARGS(class_name,io_read);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_write);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_append);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_set_size);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_get_size);			\
        SETUP_METHOD_WITH_ARGS(class_name,io_get_mf_state);


	
#endif	_mf_mgr_ifc_h
