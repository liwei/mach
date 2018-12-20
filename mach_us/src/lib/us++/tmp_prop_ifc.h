/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tmp_prop_ifc.h,v $
 *
 * Author: Daniel P. Julin, J. Mark Stevenson
 *
 * Purpose: A mech. to supply the property of temporaryness to an object so
 *		it can be held in a directory without the directory actually
 *		holding a reference to it.  Hence it "dissapears" from the
 *		directory when no one else references it.
 *
 *		For a simple usage example, see tmp_agency{.cc,_ifc.h}
 * HISTORY
 * $Log:	tmp_prop_ifc.h,v $
 * Revision 2.3  94/07/07  17:24:58  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:29:36  dpj
 * 	Mechinism built from a previous revision on of "tmp_agency" to supply a
 * 	way to make an agency temporary.  This means that it will go away iff
 * 	there are no more stronglinks or agents left for the agency and the
 * 	the "last_chance" checks succeed.  These "last_chance" checks are controlled
 * 	by any given agency which wishes to be a tmp_prop. (See tmp_dir/tmp_agency)
 * 	[92/06/24  16:31:56  jms]
 * 
 */

#ifndef	_tmp_prop_ifc_h
#define	_tmp_prop_ifc_h

#include	<vol_agency_ifc.h>
#include	<dir_ifc.h>

class tmp_prop {
      private:
	struct mutex	lock;
	boolean_t	active;
	int		agent_count;
	int		stronglink_count;
	int		tmplink_count;
	vol_agency	*temporary_obj;	/* temp_obj (we are a member of it) */
	dir		*parent_dir;	/* the dir containing the temp_obj */
	int		parent_tag;

      protected:
	virtual mach_error_t tmp_shutdown_internal(void);

	/*
	 * THE FOLLOWING "TMP" ROUTINES MAY BE SUPPLIED BY THE TEMPORARY_OBJ
	 * WHICH HAS THE TMP_PROP.  They are used to inform the temporary_obj
	 * of significant tmp_prop state and give it a chance return an
	 * error on the state change.  Normally these routines should just
	 * return ERR_SUCCESS. 
	 *
	 * NOTE: These routines should not sleep or take too long, because
	 * all kinds of locks may be held while they execute (even possibly
	 * in the parent)
	 */

	/* 
	 * ns_tmp_last_link:
	 *  Called by ns_unregister_{tmp,strong}link before a transition to 0
	 *  of the sum of their counts.  Upon error return the "unregister"
	 *  will abort and return the error value.
	 */
//	virtual mach_error_t ns_tmp_last_link(void);	

	/* 
	 * ns_tmp_last_chance:
	 *  Called by tmp_shutdown_internal before the temporary_obj goes away.
	 *  Upon error return the "shutdown" will abort
	 *  and return the error value.
	 */
//	virtual mach_error_t ns_tmp_last_chance(void);

	/*
	 * ns_tmp_cleanup_for_shutdown:
	 *  This routine is called internally during the shutdown phase, after
	 *  the object has committed to be destroyed, but before releasing the
	 *  tmp_prop lock. It is intended to deal with whatever state needs to
	 *  be cleaned-up before the parent can be allowed to create a new 
	 *  object with similar attributes.
	 */
//	virtual mach_error_t ns_tmp_cleanup_for_shutdown(void);

      public:
	tmp_prop();
	tmp_prop(mach_error_t *());
	virtual ~tmp_prop();

	virtual mach_error_t ns_register_tmplink(vol_agency *, dir *, int);
	virtual mach_error_t ns_unregister_tmplink(int);
	virtual mach_error_t ns_reference_tmplink(void);
	virtual mach_error_t ns_register_stronglink(void);
	virtual mach_error_t ns_unregister_stronglink(void);
	virtual mach_error_t ns_register_agent(ns_access_t);
	virtual mach_error_t ns_unregister_agent(ns_access_t);

	virtual mach_error_t ns_get_nlinks_attribute(ns_attr_t, int *);
};

/*
 * This macro should be called in the class declaration of objects which
 * which to be temprorary objects.
 */
#define DECLARE_TMP_PROP(field_name) \
      private: \
	friend class	tmp_prop; \
	tmp_prop	field_name; \
\
      public: \
	virtual mach_error_t ns_register_tmplink(dir *parent, int tag); \
	virtual mach_error_t ns_unregister_tmplink(int tag) ; \
	virtual mach_error_t ns_reference_tmplink(void); \
	virtual mach_error_t ns_register_stronglink(void); \
	virtual mach_error_t ns_unregister_stronglink(void); \
	virtual mach_error_t ns_register_agent(ns_access_t access); \
	virtual mach_error_t ns_unregister_agent(ns_access_t access)

/*
 * This macro should be called in the class definition of objects which
 * which to be temprorary objects.
 */
#define DEFINE_TMP_PROP(class_name, field_name) \
	mach_error_t class_name::ns_register_tmplink(dir *parent, int tag) \
			{return(field_name.ns_register_tmplink( \
					(vol_agency* )this, \
					parent, tag));} \
	mach_error_t class_name::ns_unregister_tmplink(int tag)  \
			{return(field_name.ns_unregister_tmplink(tag));} \
	mach_error_t class_name::ns_reference_tmplink(void) \
			{return(field_name.ns_reference_tmplink());} \
	mach_error_t class_name::ns_register_stronglink(void) \
			{return(field_name.ns_register_stronglink());} \
	mach_error_t class_name::ns_unregister_stronglink(void) \
			{return(field_name.ns_unregister_stronglink());} \
	mach_error_t class_name::ns_register_agent(ns_access_t access) \
			{return(field_name.ns_register_agent(access));} \
	mach_error_t class_name::ns_unregister_agent(ns_access_t access) \
			{return(field_name.ns_unregister_agent(access));}

#endif	_tmp_prop_ifc_h
