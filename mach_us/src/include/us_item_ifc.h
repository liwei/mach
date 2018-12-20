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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_item_ifc.h,v $
 *
 * usItem: abstract class defining the generic operations available for
 *	   all objects.
 *
 *
 * HISTORY:
 * $Log:	us_item_ifc.h,v $
 * Revision 2.4  94/07/08  15:54:31  mrt
 * 	Updated copyright, added comments
 * 
 * Revision 2.3  92/07/05  23:23:35  dpj
 * 	Added ns_get_item_ptr() (from jms).
 * 	[92/06/29  22:42:11  dpj]
 * 
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:45:29  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:01:20  dpj]
 * 
 * 	Made ns_(un)register_agent() into real functions instead of pure
 * 	virtuals, to get around a bug in gcc-2.0.
 * 	[92/03/10  20:29:13  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:08  jms
 * 	Initial C++ revision.
 * 	[91/09/26  17:56:00  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:31:10  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:44:44  pjg]
 * 
 */

#ifndef _us_item_h
#define _us_item_h

#include <top_ifc.h>

extern "C" {
#include <ns_types.h>
}

/*
 * Generic operations available on all objects.
 */

class usItem: public VIRTUAL1 usRemote {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usItem);

	virtual mach_error_t ns_get_item_ptr(usItem**);

REMOTE	virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**) =0;
REMOTE	virtual mach_error_t ns_duplicate(ns_access_t, usItem**) =0;
REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int*) =0;
REMOTE	virtual mach_error_t ns_set_times(time_value_t, time_value_t) =0;
REMOTE	virtual mach_error_t ns_get_protection(ns_prot_t, int*) =0;
REMOTE	virtual mach_error_t ns_set_protection(ns_prot_t, int) =0;
REMOTE	virtual mach_error_t ns_get_privileged_id(int*) =0;
REMOTE	virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *) =0;
REMOTE	virtual mach_error_t ns_get_manager(ns_access_t, usItem **) =0;

/* XXX Temporary, to work around a bug in g++ */
	virtual mach_error_t ns_register_agent(ns_access_t);
	virtual mach_error_t ns_unregister_agent(ns_access_t);

};


EXPORT_METHOD(ns_reference);
EXPORT_METHOD(ns_dereference);
EXPORT_METHOD(ns_authenticate);
EXPORT_METHOD(ns_duplicate);
EXPORT_METHOD(ns_get_attributes);
EXPORT_METHOD(ns_set_times);
EXPORT_METHOD(ns_get_protection);
EXPORT_METHOD(ns_set_protection);
EXPORT_METHOD(ns_get_privileged_id);
EXPORT_METHOD(ns_get_access);
EXPORT_METHOD(ns_get_manager);


/*
 * ns_reference(): obtain one user reference for an object.
 *
 * This function increments the reference count on the specified object,
 * indicating that it should not be deallocated until the that reference
 * is released.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object on which to increment the
 *		reference count.
 *
 * Results:
 *
 *	none.
 *
 * Side effects:
 *
 *	The reference count on obj is incremented by one unit.
 *
 * Note:
 *
 *	This call should only be used in very exceptional cases.
 *	Normally, the NO_MORE_SENDERS mechanism is used to keep track
 *	of user references to objects.
 *
 */
/*
 * ns_dereference(): release one user reference for an object.
 *
 * This function decrements the reference count on the specified object,
 * indicating that the user no longer needs to access that object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object on which to decrement the
 *		reference count.
 *
 * Results:
 *
 *	none.
 *
 * Side effects:
 *
 *	The reference count on obj is decremented by one unit.
 *	If the reference count reaches zero, the object may be destroyed.
 *
 * Note:
 *
 *	This call should only be used in very exceptional cases.
 *	Normally, the NO_MORE_SENDERS mechanism is used to keep track
 *	of user references to objects.
 *
 */
/*
 * ns_authenticate(): return a new handle for the same object, with
 *			a new access and new authentication.
 *
 * This function uses the specified token to obtain new user credentials
 * information from the authentication server. It then returns a new client-side
 * object, corresponding to the same system object as that specified in the
 * call, but associated with the new credentials, and the specified access rights.
 * Access control mediation is performed to verify that the requested rights
 * are acceptable.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object for which a new handle is to be
 *		created.
 *
 *	access [ns_access_t]: access rights desired for the object to be
 *		returned upon successful completion of this operation.
 *
 *	token [ns_token_t]: authentication token representing the desired
 *		user credentials.
 *
 * Results:
 *
 *	newobj [mach_object_t *]: new client-side object created by this call.
 *
 * Side effects:
 *
 *	A new client-side object is instantiated in the address space of
 *	the caller.
 *
 * Note:
 *
 *	The normal primitive specifies a single token for authentication.
 *	Other primitives may specify many parameters, like effective ID,
 *	security level, etc.
 */
/*
 * ns_duplicate(): return a new handle for the same object, with
 *			a new access and the same authentication.
 *
 * This function returns a new client-side object, corresponding to the same
 * system object as that specified in the call, but for which different access
 * rights are enabled. The user credentials are the same as those for the
 * object on which the call is performed.
 *
 * Access control mediation is performed to verify that the requested rights
 * are acceptable.
 *
 * Even if the desired access rights are the same as those already enabled
 * in the original object, a new independent object is always returned.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object for which a new handle is to be
 *		created.
 *
 *	access [ns_access_t]: access rights desired for the object to be
 *		returned upon successful completion of this operation.
 *
 * Results:
 *
 *	newobj [mach_object_t *]: new client-side object created by this call.
 *
 * Side effects:
 *
 *	A new client-side object is instantiated in the address space of
 *	the caller.
 *
 * Note:
 *
 */
/*
 * ns_get_attributes(): get a standard attributes structure for an object.
 *
 * This function returns a standard structure containing a set of commonly
 * used attributes for the specified object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object for which to return the attributes.
 *
 * Results:
 *
 *	attr [ns_attr_t]: attributes structure for obj. This structure is stored
 *		at the address given by the caller.
 *
 *	attrlen [int *]: number of entries in the attr structure.
 *
 * Side effects:
 *
 *	none.
 *
 * Note:
 *
 *	There is currently no check to verify that the caller has allocated
 *	enough space to receive the result structure.
 *
 */
/*
 * ns_get_protection(): get the protection information for an object.
 *
 * This function returns a structure containing all the protection information
 * for the specified object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object for which to return the protection.
 *
 * Results:
 *
 *	prot [ns_prot_t]: protection structure for obj. This structure is stored
 *		at the address given by the caller.
 *
 *	protlen [int *]: number of entries in the prot structure.
 *
 * Side effects:
 *
 *	none.
 *
 * Note:
 *
 *	There is currently no check to verify that the caller has allocated
 *	enough space to receive the result structure.
 *
 */
/*
 * ns_set_protection(): replace the protection for an object.
 *
 * This function overwrites the protection structure associated with the
 * specified object.
 * 
 * To avoid concurrency problems when multiple users are modifying the
 * protection structure, this function does not effect any change unless
 * the generation number in the new protection structure is equal to the one
 * in the existing strucuture plus one (this feature may not be available with
 * all servers, in which case the call always succeeds).
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object for which the protection must be changed.
 *
 *	prot [ns_prot_t]: new protection structure.
 *
 *	protlen [int]: number of entries in the prot structure.
 *
 * Results:
 *
 *	none.
 *
 * Side effects:
 *
 *	The protection for the specified entry may be changed.
 *
 * Note:
 *
 */
/*
 * ns_get_privileged_id(): find out the privileged authentication ID for
 *			an object.
 *
 * This function returns the authentication ID that has complete, unrevokable
 * power to operate on the specified object. Once the user knows this ID, he
 * can determine if he can acquire an authentication token suitable to
 * operate on the object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object under consideration.
 *
 * Results:
 *
 *	privid [int *]: privileged authentication ID.
 *
 * Side effects:
 *
 *	none.
 *
 * Note:
 *
 */
/*
 * ns_get_access(): list the user access and credentials for an object.
 *
 * This function returns the set of access rights enabled for the
 * specified object, and the user credentials associcated with that
 * object.
 *
 * Parameters:
 *
 *	obj [mach_object_t]: the object under consideration.
 *
 * Results:
 *
 *	access [ns_access_t *]: access rights enabled for obj.
 *
 *	cred [ns_cred_t]: credentials structure for obj. This structure is stored
 *		at the address given by the caller.
 *
 *	credlen [int *]: number of entries in the cred structure.
 *
 * Side effects:
 *
 *	none.
 *
 * Note:
 *
 *	There is currently no check to verify that the caller has allocated
 *	enough space to receive the result structure.
 */
/*
 * ns_get_manager():
 *
 * Return a handle for a "manager object" exporting operations to
 * control the whole set of agencies in one server or subtree.
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	access [ns_access_t] :
 *
 *	newobj [usItem **] :
 *
 */
/*
 * ns_set_times():
 *
 * Parameters:
 *
 *	obj [mach_object_t] :
 *
 *	atime [time_value_t] :
 *
 *	mtime [time_value_t] :
 *
 */

#endif _us_item_h
