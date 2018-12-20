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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/us_name_ifc.h,v $
 *
 * usName: abstract class defining the naming protocol.
 *
 *
 * HISTORY:
 * $Log:	us_name_ifc.h,v $
 * Revision 2.4  94/07/08  15:51:51  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:23:37  dpj
 * 	Insert comments for all methods.  These comments come for the MachObjects
 * 	version of the file and were not copied when the code was translated.
 * 	[92/06/24  13:47:00  jms]
 * 	Conditionalized virtual base class specifications.
 * 	[92/06/24  15:45:55  dpj]
 * 
 * Revision 2.2  91/11/06  11:28:11  jms
 * 	Initial C++ revision.
 * 	[91/09/26  17:55:15  pjg]
 * 
 * 	Upgraded to US38.
 * 	[91/04/15  14:38:29  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:45:06  pjg]
 * 
 */

#ifndef	_us_name_h
#define	_us_name_h

#include <us_item_ifc.h>

/*
 * Directory operations.
 */
class usName: public VIRTUAL2 usItem {
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usName);

REMOTE	virtual mach_error_t ns_resolve(char*, ns_mode_t, ns_access_t, 
					usItem** , ns_type_t*, char*, 
					int*, ns_action_t*) =0;
REMOTE	virtual mach_error_t ns_create(char*, ns_type_t, ns_prot_t, int,
				       ns_access_t, usItem**) =0;
REMOTE	virtual mach_error_t ns_create_anon(ns_type_t, ns_prot_t, int,
					    ns_access_t, char*, usItem**) =0;
REMOTE	virtual mach_error_t ns_create_transparent_symlink(ns_name_t,ns_prot_t,
							   int, char *) =0;
REMOTE	virtual mach_error_t ns_insert_entry(char*, usItem*) =0;
REMOTE	virtual mach_error_t ns_insert_forwarding_entry(char*, ns_prot_t, int,
							usItem*, char*) =0;
REMOTE	virtual mach_error_t ns_read_forwarding_entry(usItem**, char*) =0;
REMOTE	virtual mach_error_t ns_remove_entry(char*) =0;
REMOTE	virtual mach_error_t ns_rename_entry(char*, usItem*, char*) =0;
REMOTE	virtual mach_error_t ns_list_entries(ns_mode_t, ns_name_t**,
					     unsigned int*, ns_entry_t*,
					     unsigned int*) =0;
REMOTE	virtual mach_error_t ns_list_types(ns_type_t**, int*) =0;
REMOTE	virtual mach_error_t ns_allocate_unique_name(char*) =0;
};

/*
 * ns_resolve(): one step in the resolving loop.
 *
 * This function traverses a path name, starting at the specified directory,
 * until either the end object is found, or a point is encountered at which
 * the name server prefers to give the initiative back to the client (e.g.
 * a mount point or symbolic link). In the latter case, the client may elect
 * to initiate a new ns_resolve() operation, with different parameters and
 * a different starting point.
 *
 * The decision to return to the client upon encountering a forwarding
 * point is entirely up to the server. Some servers may choose to follow
 * those forwarding points internally (and silently).
 *
 * Parameters:
 *
 *	path [char *]: zero-terminated string specifying the path name
 *		to be traversed starting at obj.
 *
 * 	mode [ns_mode_t]: options for the resolving operation:
 *		NSF_ACCESS: simply check that the specified access could
 *			be obtained, but do not actually return any object.
 *		NSF_FOLLOW: if the last component of the path name is a
 *			forwarding point, return the contents of that entry.
 *			If this flag is false, ns_resolve() returns the
 *			forwarding object itself instead of its contents.
 *
 *	access [ns_access_t]: access rights desired for the object to be
 *		returned upon successful completion of this operation.
 *
 * Results:
 *
 *	If the desired object is found, it is returned in
 *	newobj [usItem **], with the access specified.
 *	newtype [ns_type_t *] specifies the type of the object found.
 *
 * 	If a forwarding point is encountered, its contents are returned
 *	in newobj and/or newpath [char *], and usedlen [int *] specifies
 *	how many characters were used in the original path before finding
 *	this forwarding point.
 *
 * 	action [ns_action_t *] specifies what to do next:
 *		NSA_FORWARD: apply the forwarding information returned in
 *			newobj/newpath, and restart the resolving loop at
 *			that point.
 *		NSA_AUTHENTICATE: obtain a new authenticated object to
 *			replace the object returned in newobj before
 *			continuing. If this flag is false, newobj already
 *			carries the same user credentials as those carried
 *			by obj.
 *		NSA_NOCACHE: do not cache the results of this call on the
 *			client side. If this flag is false, and NSA_FORWARD
 *			is true, the caller is allowed to cache the information
 *			about this forwarding point for use in future resolving
 *			operations.
 *
 * Side effects:
 *
 *	A new client-side object may be instantiated in the address space
 *	of the caller.
 *
 * Note:
 *
 * 	Contrary to the old interface, newpath is simply the contents
 * 	of the symlink, not the concatenation of the symlink with the remaining
 * 	path name.
 *
 *	newobj will be of a class appropriate for the intended use of this
 *	object.
 */
EXPORT_METHOD(ns_resolve);


/*
 * ns_create(): create an arbitrary object in a directory.
 *
 * This function creates a new object, and enters it in a given directory
 * under a given name. If desired, a client-side object for the new object
 * is returned.
 *
 * Any type that is valid in that directory can be requested, but no
 * type-specific information can be specified with this call. If any such
 * information is needed for that particular type, the object will initially
 * be "empty" until some specific call is performed to initialize it.
 *
 *
 * Parameters:
 *
 *	name [char *]: name for the new entry in the obj directory.
 *		This name should be a single component, not a full path name.
 *
 *	type [ns_type_t]: type of the object to be created.
 *
 *	prot [ns_prot_t]: initial protection for the new object.
 *
 *	protlen [int]: number of entries in the prot structure.
 *
 *	access [ns_access_t]: access rights for the client-side object to be
 *		returned by this call. If this parameter is zero, the object
 *		is created and inserted in the directory, but no client-side
 *		object is returned to the caller.
 *
 * Results:
 *
 *	If the access parameter is non-zero, newobj [usItem **] contains
 *	a pointer to a user-side object for the newly created object, with the
 *	required access.
 *
 * Side effects:
 *
 *	A new object is created, and an entry for it is inserted in the
 * 	obj directory.
 *
 *	A new client-side object may be instantiated in the address space
 *	of the caller.
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_create);


/*
 * ns_create_anon(): create an arbitrary object in a directory,
 *	under a new unique name.
 *
 * This function is identical to ns_create(), except that it allocates
 * a new unique name for the entry, instead of using a name specified
 * by the caller.
 *
 * Parameters:
 *
 *	type [ns_type_t]: type of the object to be created.
 *
 *	prot [ns_prot_t]: initial protection for the new object.
 *
 *	protlen [int]: number of entries in the prot structure.
 *
 *	access [ns_access_t]: access rights for the client-side object to be
 *		returned by this call. If this parameter is zero, the object
 *		is created and inserted in the directory, but no client-side
 *		object is returned to the caller.
 *
 * Results:
 *
 *	name [char *] contains the new name allocated for the entry.
 *
 *	If the access parameter is non-zero, newobj [usItem **] contains
 *	a pointer to a user-side object for the newly created object, with the
 *	required access.
 *
 * Side effects:
 *
 *	A new object is created, and an entry for it is inserted in the
 * 	obj directory.
 *
 *	A new client-side object may be instantiated in the address space
 *	of the caller.
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_create_anon);


/*
 * ns_create_transparent_symlink(): create a transparent symlink
 *	in a directory.
 *
 * This function creates a new transparent symlink with the specified
 * information, and inserts it in the specified directory.
 *
 * Parameters:
 *
 *	name [char *]: name for the new entry in the obj directory.
 *		This name should be a single component, not a full path name.
 *
 *	prot [ns_prot_t]: initial protection for the new object.
 *
 *	protlen [int]: number of entries in the prot structure.
 *
 *	fwdpath [char *]: path name to be used to modify the current path
 *		name when this forwarding point is encountered during
 *		resolving.
 *
 * Results:
 *
 *	none.
 *
 * Side effects:
 *
 *	A new transparent symlink is created, and an entry for it is inserted
 * 	in the obj directory.
 *
 * Note:
 */
EXPORT_METHOD(ns_create_transparent_symlink);


/*
 * ns_insert_entry(): make a hard link to an already existing object.
 *
 * This function creates a new entry in the specified directory, pointing
 * to an already existing object.
 *
 * This function is optional: some servers may choose not to implement for
 * some or all of their directories, and for some or all of the possible
 * target objects. In particular, no application should ever expect to be
 * able to insert such an entry for an object that in not managed by the
 * same server that manages the specified directory.
 *
 * Parameters:
 *
 *	name [char *]: name for the new entry in the obj directory.
 *		This name should be a single component, not a full path name.
 *
 *	target [usItem]: object to be designated by the new entry.
 *
 * Results:
 *
 *	none.
 *
 * Side effects:
 *
 *	A new entry is created in the obj directory.
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_insert_entry);


/*
 * ns_insert_forwarding_entry(): create a forwarding entry in a directory.
 *
 * This function creates a new forwarding object with the specified information,
 * and inserts it in the specified directory.
 *
 * Parameters:
 *
 *	name [char *]: name for the new entry in the obj directory.
 *		This name should be a single component, not a full path name.
 *
 *	prot [ns_prot_t]: initial protection for the new object.
 *
 *	protlen [int]: number of entries in the prot structure.
 *
 *	fwdobj [usItem]: object to be placed in the forwarding object.
 *		This object indicates where resolving should proceed when that
 *		forwarding point is encountered. If this parameter is NULL,
 *		the forwarding object is a simple symbolic link.
 *
 *	fwdpath [char *]: path name to be used to modify the current path
 *		name when this forwarding point is encountered during resolving.
 *		If this parameter is NULL or a null string, the forwarding
 *		object is a simple mount point.
 *
 * Results:
 *
 *	none.
 *
 * Side effects:
 *
 *	A new forwarding object is created, and an entry for it is inserted
 * 	in the obj directory.
 *
 * Note:
 *
 *	This function makes no assumptions on whether or not the fwdobj
 *	is pre-authenticated. Users beware!
 *
 */
EXPORT_METHOD(ns_insert_forwarding_entry);


/*
 * ns_read_forwarding_entry(): fetch the contents of a forwarding object.
 *
 * This function returns the contents (new object and new path)
 * of the specified forwarding object.
 *
 * Parameters:
 *
 * Results:
 *
 *	newobj [usItem **]: the "new object" field in the forwarding
 *		object. This field is NULL for a pure symbolic link.
 *		The object returned is exactly that contained in the
 *		forwarding object. No implicit re-authentication is
 *		performed by the server.
 *
 *	newpath [char *]: the "new path" field in the forwarding object.
 *		This field is null for a pure mount point.
 *
 * Side effects:
 *
 *	none.
 *
 * Note:
 *
 * The object returned is not necessarily on the same server as that
 * handling this call. For this reason, this function returns both a
 * MachObjects reference and a "ns_reference" for *newobj.
 * On the server side, *newobj should be released with the sequence
 * "ns_dereference(*newobj); usItem_dereference(*newobj)".
 * On the client side, simply releasing the last MachObjects reference
 * for the CAT will take care of things normally.
 * This ugliness will go away when the "no-more-senders" mechanism
 * becomes operational.
 *
 */
EXPORT_METHOD(ns_read_forwarding_entry);


/*
 * ns_remove_entry(): remove an entry from a directory.
 *
 * This function removes the given entry in the specified directory, and
 * decrements the reference count for the object designated by that entry.
 * 
 * Parameters:
 *
 *	name [char *]: name for the entry to be removed from the obj directory.
 *		This name should be a single component, not a full path name.
 *
 * Results:
 *	none.
 *
 * Side effects:
 *
 *	The specified entry is removed from the directory.
 *
 *	The reference count for the object designated by the specified
 *	entry is decremented.
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_remove_entry);


/*
 * ns_rename_entry(): change the name and parent directory of an entry.
 *
 * This function removes the specified entry from the given directory,
 * and inserts a new entry designating the same object under a new name
 * in the target directory. This primitive is only guaranteed to be atomic
 * within a single directory. In other cases, the server is free to
 * perform the operation non-atomically, or to reject the request
 * by reporting that this primitive is not supported.
 *
 * Parameters:
 *
 *	name [char *]: old name of the entry.
 *
 *	newdir [usItem]: directory object to hold the new version of the
 *	entry.
 *
 *	newname [char *]: new name of the entry.
 *
 * Results:
 *
 *	none.
 *
 * Side effects:
 *
 *	An entry is moved from the old directory to the new, and its name
 *	is changed.
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_rename_entry);


/*
 * ns_list_entries(): list all the entries in a directory.
 *
 * This functions returns brief information for each of the entries in the
 * given directory.
 *
 * Parameters:
 *
 *	mode [ns_mode_t]: options for this operation.
 *		This parameter is currently not in use.
 *
 * Results:
 *
 *	names [char * **]: array containing the names of all the entries
 *		in the directory, in no particular order.
 *
 *	names_count [unsigned int *]: number of entries in the names array.
 *
 *	entries [ns_entry_t *]: array containing an entry structure for
 *		each entry in the directory, in the same order as that
 *		used in the names array.
 *
 *	entries_count [unsigned int *]: number of entries in the entries array.
 *		This number will always be equal to names_count.
 *
 *	Both the names and entries arrays are returned in freshly vm_allocated()
 *	memory. It is up to the caller to deallocate this memory when it is no
 *	longer needed.
 *
 * Side effects:
 *
 *	Two memory zones appear in the caller's address space, containing the
 *	names and entries arrays.
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_list_entries);


/*
 * ns_list_types(): list the object types valid in a given directory.
 *
 * This function returns a list of all the object types for which a
 * ns_create() would succeed in the specified directory, i.e. the types
 * supported by the server managing that directory.
 *
 * Parameters:
 *
 * Results:
 *
 *	types [ns_type_t **]: array containing the list of all types
 *		supported by the designated directory, in no particular
 *		order.
 *
 *	count [int *]: number of entries in the types array.
 *
 *	The types array is returned in freshly vm_allocated() memory.
 *	It is up to the caller to deallocate this memory when it is no
 *	longer needed.
 *
 * Side effects:
 *
 *	One memory zone appears in the caller's address space, containing the
 *	types array.
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_list_types);


/*
 * ns_allocate_unique_name(): generate a new unique name in a directory.
 *
 * This function returns a name that is guaranteed to be unique and
 * available for creation of a new entry in the target directory. It
 * does not affect the target directory in any visible way.
 *
 * Parameters:
 *
 * Results:
 *
 *	name [char *] contains the new name.
 *
 * Side effects:
 *
 * Note:
 *
 */
EXPORT_METHOD(ns_allocate_unique_name);

#endif	_us_name_h
