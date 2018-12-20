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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/std_name.cc,v $
 *
 * Purpose: Generic user-side name space object,
 * 	responsible for user-side naming and authentication
 *	duties.
 *
 * HISTORY:
 * $Log:	std_name.cc,v $
 * Revision 2.11  94/07/07  17:24:30  mrt
 * 	Updated copyright.
 * 
 * Revision 2.10  94/05/17  13:48:54  jms
 * 	Fixed resolution code so that it followed transparent
 * 	symlinks correctly.
 * 	[94/05/04  13:56:26  grm]
 * 
 * 	Made changes to the prefix caching logic, so that the name
 * 	[94/04/26  16:34:56  grm]
 * 
 *	resolution code decides what it wants to cache before
 *	it starts the resolution.
 * 
 * Revision 2.9  94/05/04  14:04:42  jms
 * 	Needed implementations of methods for class std_name for 2.3.3 g++ compiler -modh
 * 
 * Revision 2.6.2.1  94/02/18  11:27:42  modh
 * 	Needed implementations of methods for class std_name for 2.3.3 g++ compiler
 * 
 * Revision 2.8  94/04/29  15:47:21  jms
 * 	Made changes to the prefix caching logic, so that the name resolution code
 * 	decides what it wants to cache before it starts the resolution.
 * 	[94/04/26  16:34:56  grm]
 * 
 * Revision 2.7  94/01/11  17:50:10  jms
 * 	Comment out "proxy" debug references/includes.
 * 	[94/01/09  19:40:37  jms]
 * 
 * Revision 2.6  92/07/05  23:28:54  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:08:39  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:57:55  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:12:39  dpj]
 * 
 * Revision 2.5  92/03/05  15:05:46  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:33:47  jms]
 * 
 * Revision 2.4  91/12/20  17:44:21  jms
 * 	Modify "ns_" stuff to handle new mech for non-directories in prefix table
 * 	to enable "/dev/null" prefix.
 * 	[91/12/20  15:53:30  jms]
 * 
 * Revision 2.3  91/11/13  17:17:58  dpj
 * 	Implemented "@sys".
 * 	Allow lookup of path names that exactly match an entire prefix.
 * 	[91/11/12  17:53:57  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:58  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:11:58  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:33:58  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:35:02  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:41:23  pjg]
 * 
 * Revision 2.10  91/05/05  19:27:26  dpj
 * 	Fixed extraneous "\" at the beggining of the canonpath in
 * 	ns_resolve_simple(), which was sometimes causing confusion
 * 	with the "usedlen" return from ns_resolve().
 * 	[91/05/05            dpj]
 * 
 * 	Merged up to US39
 * 	[91/05/04  09:58:09  dpj]
 * 
 * 	Support transparent symlinks.
 * 	Cleaned-up debugging control.
 * 	[91/04/28  10:19:33  dpj]
 * 
 * Revision 2.9  91/04/12  18:48:22  jjc
 * 	Changed CLEAN_PATH to make sure that each component of the path
 * 	and the whole path are less than their respective size limits.
 * 	[91/04/02            jjc]
 * 
 * Revision 2.8  90/12/19  11:05:31  jjc
 * 	Made changes to lazily reauthenticate prefix table entries after
 * 	changing identities:
 * 		1) Created std_name_reauthenticate() using code for 
 * 		   reauthenticating prefix table entries from ns_set_token().
 * 		2) Added identity version counter to this module and added 
 * 		   version stamp to prefix table entries, so we can tell when
 * 		   an entry needs to be reauthenticated.
 * 		3) Added call to std_name_reauthenticate() to ns_find_prefix().
 * 		4) Initialize identity version counter in std_name_initialize()
 * 		   and increment it in ns_set_token().
 * 		5) Changed ns_set_{system,user,cache}_prefix() to update
 * 		   identity version stamp when setting a newly authenticated
 * 		   prefix.
 * 	[90/09/26            jjc]
 * 
 * Revision 2.7  90/07/09  17:09:33  dorr
 * 	raise a bunch of debug output levels.
 * 	[90/03/01  15:00:41  dorr]
 * 	No Further Changes
 * 	[90/07/09  12:51:40  jms]
 * 
 * Revision 2.6  89/07/09  14:19:41  dpj
 * 	Fixed debugging statements for new DEBUG macros.
 * 	Fixed error codes for new unified scheme.
 * 	[89/07/08  12:59:53  dpj]
 * 
 * Revision 2.5  89/06/30  18:35:24  dpj
 * 	Changed ns_set_token() to preserve prefixes for which the user currently
 * 	cannot have any access, by getting NSR_REFERENCE access to them.
 * 	Added NSF_MOUNT where needed.
 * 	Modified the resolving loop to deal with the distinction between NSF_FOLLOW
 * 	and NSF_MOUNT.
 * 	Fixed to avoid prefix matches for a partial component of a path name.
 * 	Fixed to avoid prefix matches for the entire pathname, because that would
 * 	make it impossible to avoid following forwarding entries.
 * 	[89/06/29  00:41:22  dpj]
 * 
 * 	Increment the retrycount for each iteration in the main loop in
 * 	ns_resolve_fully(), so that we exit when the server is not responding.
 * 	[89/06/21  22:25:19  dpj]
 * 
 * 	Fixed cloning operations to deal correctly with invalid entries.
 * 	Fixed ns_set_token() re-authenticated all stored prefixes.
 * 	[89/05/31  17:45:19  dpj]
 * 
 * 	Second try. Reworked to deal with "/" at the end of a path name, and
 * 	to deal with symlinks that point directly to a terminal object (i.e not
 * 	to a directory).
 * 	[89/05/29  17:47:18  dpj]
 * 
 * 	First cut at complete handling of complex path names, with ".", "..",
 * 	and caching in the user prefix table.
 * 	[89/05/26  16:16:20  dpj]
 * 
 * Revision 2.4  89/05/17  16:44:03  dorr
 * 	ns_login() -> ns_set_token().
 * 	[89/05/15  12:17:43  dorr]
 * 
 * Revision 2.3  89/04/04  18:22:41  dorr
 * 	don't deallocate token ports.  taken care of externally.
 * 
 * Revision 2.2  89/03/17  12:49:46  sanzi
 * 	Fixed clone_init() to correctly abort when something fails.
 * 	[89/03/10  23:38:58  dpj]
 * 	
 * 	Temporarily fixed ns_set_user_prefix() to use
 * 	an almost-correct canonical name for the new prefix.
 * 	This will need a better fix later.
 * 	[89/03/08  21:43:43  dpj]
 * 	
 * 	don't leak prefix cats.
 * 	[89/03/07  15:52:03  dorr]
 * 	
 * 	fix cwd & cloning.
 * 	[89/03/07  10:58:37  dorr]
 * 	
 * 	fix reference counting bug.
 * 	[89/02/28  00:16:26  dorr]
 * 	
 * 	add external prefix ops.  fix up prefix stuff.
 * 	[89/02/24  17:10:12  dorr]
 * 	
 * 	Created.
 * 	[89/02/20  14:40:25  dorr]
 * 
 */

/*
 * A note as to how the name resolution code works:
 *
 * An inputpath, the path that is to be resolved, is given to
 * ns_resolve_fully (FULLY).  FULLY cleans up the path (removes './' and
 * '//' components) and then resolves it using two routines:
 * ns_resolve_complex (COMPLEX), and ns_resolve_simple (SIMPLE).
 * 
 * The SIMPLE routine takes an inputpath and resolves it until it reaches
 * a '..' component.  At this point, it returns the object representing
 * the directory before the '..' component is reached, and the point at
 * which the path resolution stopped (a pointer to the '..' component in
 * the inputpath).
 * 
 * The COMPLEX routine takes an inputpath, that can include '..'
 * components, and returns with an object that represents the final
 * component of the inputpath (or an error if the path is invalid).  The
 * COMPLEX routine works by repeatedly calling the SIMPLE procedure and
 * unwinding any '..' components that SIMPLE can't handle.  
 * 
 * The bulk of the name resolution happens in the SIMPLE routine.  It
 * handles forwarding points (symbolic links in unix) by calling COMPLEX
 * on any forwarding point it comes across.  It then continues resolving
 * the inputpath until it either finishes or comes across a '..'
 * component.
 * 
 * For reference, the SIMPLE and COMPLEX procedures and their parameters
 * are described briefly below:
 * 
 * mach_error_t 
 * std_name::ns_resolve_simple(
 *  usItem**  	pfx_obj,	Pointer to current directory object.
 *  ns_type_t*	pfx_type,	Current directory type.
 *  char**	pfx_inputpos,	Ptr to end of currnet dir's inputpath name.
 *  char**	pfx_canonpos,   Ptr to end of currnet dir's canonpath name.
 *  char**	cache_inputpos,	Ptr to end of prefix cache's inputpath name.
 *  usItem**	out_obj,	Ptr to final component's object.
 *  char**	out_inputpos,	Ptr to last resolved point in inputpath.
 *  char**	out_canonpos, 	Ptr to last resolved point in canonpath.
 *  ns_type_t*	out_type,	Final component's type
 *  char*	inputpath, 	Inputpath to be resolved.
 *  char*	canonpath,	Linear representation of inputpath
 *  ns_mode_t	mode,
 *  ns_access_t	access,
 *  unsigned int forwardcount	Mechanism to ensure non-circuitious refs.
 *  )
 * 
 * mach_error_t 
 * std_name::ns_resolve_complex(
 *  usItem**  	pfx_obj,	Pointer to current directory object.
 *  ns_type_t*	pfx_type,	Current directory type.
 *  char**	pfx_inputpos,	Ptr to end of currnet dir's inputpath name.
 *  char**	pfx_canonpos,   Ptr to end of currnet dir's canonpath name.
 *  char**	cache_inputpos,	Ptr to end of prefix cache's inputpath name.
 *  usItem**	out_obj,	Ptr to final component's object.
 *  char**	out_canonpos, 	Ptr to last resolved point in canonpath.
 *  ns_type_t*	out_type,	Final component's type
 *  char*	inputpath, 	Inputpath to be resolved.
 *  char*	canonpath,	Linear representation of inputpath
 *  ns_mode_t	mode,
 *  ns_access_t	access,
 *  unsigned int forwardcount	Mechanism to ensure non-circuitious refs.
 *  )
 * 
 * The name resolution routines all rely on the prefix table.  The prefix
 * table is a table that contains triplets of information: a relative
 * pathname to the current working directory, a linear canonpath
 * representation of this relative pathname, and the usItem object
 * pointer to the path's object.  This table is used to speed-up name
 * resolution.  A user file reference causes a new path to be cached in
 * the prefix table (hopefully, to speed up any subsequent references to
 * the same directory).
 * 
 * What complicates these three routines (FULLY, COMPLEX, and SIMPLE), is
 * that they perform a secondary function as well: they perform the
 * caching of the object's prefix in the prefix table.  This prefix is
 * usually the final object's parent directory's object.  I say usually,
 * because it is sometimes not possible to cache this object due to the
 * way these routines are constructed (to minimize client-server
 * communication).
 * 
 * The FULLY routine determines which component of the inputpath is the
 * parent component of the final object.  If there isn't one, it doesn't
 * attemp to cache a prefix during the resolution.  This component is
 * designated by the cache_inputpos (the position in the inputpath that
 * points to the end of the parent's path).
 * 
 * There is a second path used in these routines, in addition to the
 * inputpath, called the canonpath.  Where the inputpath represents an
 * object with both '..' components and forwarding points, the canonpath
 * is an absolute linear path that represents objects.
 * 
 * Both the COMPLEX and SIMPLE routines are called with parameters that
 * refer to the 'pfx object' (an unfortunate name choice).  This object
 * refers to the "current directory" object which the routines will call
 * with the rest of the inputpath's components.  There are four
 * parameters to this object:  
 * 
 *  - pfx_obj is the usItem object pointer for the object.
 *  - pfx_type is the ns_type_t of the object.
 *  - pfx_inputpos is a pointer into the inputpath that designates the end
 *    of the inputpath string which represents the pfx_obj.
 *  - pfx_cacnonpos is a pointer into the canonpath that designates the end
 *    of the canonpath string which represents the pfx_obj.
 * 
 * The COMPLEX and SIMPLE routines are also called with the
 * cache_inputpos parameter (as described above).  When the resolution
 * process has resolved the inpupath up to (or past) this point, the
 * macro SAVE_CACHE_PREFIX is called to save the prefix.  If this
 * parameter is zero, no prefix saving occurs.
 * 
 * There are out parameters to the COMPLEX and SIMPLE routines as well:
 * 
 *  - out_obj is the usItem pointer to the output object.
 *  - out_type is the ns_type_t of the output object.
 * 
 * Since SIMPLE might not resolve the whole inputpath (whenever inputpath
 * constains a '..' component), it returns a parameter out_inputpos that
 * points to the position in inputpath that it stopped its resolution
 * (either at a '..' component or the end).
 * 
 * Good luck! -grm 4/18/94
 * 
 */

#include <std_name_ifc.h>
#include <us_name_ifc.h>
//#include <dir_proxy_ifc.h> /* XXX temporary */

//  #include <us_item_proxy_ifc.h>	/* XXX DPJ */
//  #include <us_name_proxy_ifc.h>	/* XXX DPJ */

extern "C" {

#include	<us_error.h>
#include	<ns_types.h>
#include	<ns_error.h>
#include	<mach/message.h>

#include	<machine/us_param.h>	/* Definition of "@sys" */


/*
 * Global variable controlling debugging output.
 *
 * Used as a secondary debugging level: 0="no output" 3="lots of output".
 */
extern int resolve_debug = 1;
}

#define BASE usClone
DEFINE_LOCAL_CLASS(std_name);

/*
 * Prefixes have two major attributes:
 *
 *	- static vs. dynamic: the target for a static prefix is always valid,
 *		while a dynamic prefix may be invalidated by the server
 *		managing the target. In the last case, a dynamic prefix
 *		is re-evaluated from a specified string.
 *
 *	- absolute vs. relative: an absolute prefix is defined independently
 *		of other prefixes. A relative prefix must be re-evaluated
 *		if other prefixes are modified.
 */
typedef enum pfx_type {
		PFX_INVALID,
		PFX_SYSTEM,	/* static, absolute */
		PFX_USER,	/* dynamic, absolute */
		PFX_CACHE	/* dynamic, relative, may be thrown out */
} pfx_type_t;


typedef struct pfx_entry {
	pfx_type_t	type;		/* type of prefix */
	ns_path_t	prefix;		/* prefix string */
	unsigned int	prefix_len;	/* length of prefix string */
	usItem*		target;		/* target object */
	ns_type_t	target_type;
	usClone*	clone_target;
	ns_path_t	canon_path;	/* canonical path */
	int		id_stamp;	/* identity version stamp */
} *pfx_entry_t;

std_name::std_name()
{
	int			i;

	_Local(pfx_table_len) = PFX_TABLE_SIZE;

	for (i = 0; i < _Local(pfx_table_len); i++) {
		_Local(pfx_table)[i] = NULL;
	}

	_Local(last_index) = _Local(pfx_table_len) - 1;
	_Local(null_prefix) = NULL;
	_Local(token) = MACH_PORT_NULL;
	_Local(identity_version) = 0;
}

std_name::~std_name()
{
	int			i;
	pfx_entry_t		entry;

	for (i = 0; i < _Local(pfx_table_len); i++) {
		entry = (pfx_entry_t) _Local(pfx_table)[i];
		if (entry == NULL) {
			continue;
		}
		mach_object_dereference(entry->target);
		Free(entry);
	}

	entry = (pfx_entry_t) _Local(null_prefix);
	if (entry != NULL) {
		mach_object_dereference(entry->target);
		Free(entry);
	}

	if (token) {
		(void)mach_port_deallocate(mach_task_self(), token);
	}
}


mach_error_t std_name::clone_init(mach_port_t child)
{
	mach_error_t		ret;
	mach_error_t		overall_ret = ERR_SUCCESS;
	int			i;
	pfx_entry_t		entry;
	int			DEBUG_CLONE=1;

	for (i = 0; i < _Local(pfx_table_len); i++) {
		entry = (pfx_entry_t) _Local(pfx_table)[i];

		if ((entry == NULL) || (entry->type == PFX_INVALID)) {
			continue;
		}

 		DEBUG1((resolve_debug>1),(entry->target, 
					  "std_name.clone_init: path=%*s\n", 
					  entry->prefix));

		DEBUG2(DEBUG_CLONE,(0,
			"std_name.clone_init:target=0x%x\n",entry->target));
		DEBUG2(DEBUG_CLONE,(0,
			"std_name.clone_init:target->is_a()=0x%x\n",entry->target->is_a()));
		DEBUG2(DEBUG_CLONE,(0,
			"std_name.clone_init:(usTop*)target=0x%x\n",(usTop*)(entry->target)));
//		DEBUG2(DEBUG_CLONE,(0,
//			"std_name.clone_init:(usItem_proxy*)target=0x%x\n",usItem_proxy::castdown(entry->target)));
//		DEBUG2(DEBUG_CLONE,(0,
//			"std_name.clone_init:(usName_proxy*)target=0x%x\n",usName_proxy::castdown(entry->target)));
 
		ret = (entry->clone_target)->clone_init(child);
		if ((ret != ERR_SUCCESS) && 
			(ret != MACH_OBJECT_NO_SUCH_OPERATION)) {
			overall_ret = ret;
		}
	}

	entry = (pfx_entry_t) _Local(null_prefix);
	if ((entry != NULL) && (entry->type != PFX_INVALID)) {
		ret = (entry->clone_target)->clone_init(child);
		if ((ret != ERR_SUCCESS) && 
			(ret != MACH_OBJECT_NO_SUCH_OPERATION)) {
			overall_ret = ret;
		}
	}

	if (overall_ret != ERR_SUCCESS) {
		DEBUG0(DEBUG_CLONE,(0,
			"std_name.clone_init FAILED about to abort:(usTop*)target=0x%x\n",(usTop*)(entry->target)));
		(void) this->clone_abort(child);
		return(overall_ret);
	}

	if (_Local(token) != MACH_PORT_NULL) {
		(void) mach_port_insert_right(child,_Local(token),
					      _Local(token),
					      MACH_MSG_TYPE_COPY_SEND);
	}

	return(ERR_SUCCESS);
}


mach_error_t std_name::clone_complete()
{
	mach_error_t		ret;
	mach_error_t		overall_ret = ERR_SUCCESS;
	int			i;
	pfx_entry_t		entry;

	for (i = 0; i < _Local(pfx_table_len); i++) {
		entry = (pfx_entry_t) _Local(pfx_table)[i];

		if ((entry == NULL) || (entry->type == PFX_INVALID)) {
			continue;
		}

		ret = (entry->clone_target)->clone_complete();
		if ((ret != ERR_SUCCESS) && 
			(ret != MACH_OBJECT_NO_SUCH_OPERATION)) {
			overall_ret = ret;
		}
	}

	entry = (pfx_entry_t) _Local(null_prefix);
	if ((entry != NULL) && (entry->type != PFX_INVALID)) {
		ret = (entry->clone_target)->clone_complete();
		if ((ret != ERR_SUCCESS) && 
			(ret != MACH_OBJECT_NO_SUCH_OPERATION)) {
			overall_ret = ret;
		}
	}

	return(overall_ret);
}


mach_error_t std_name::clone_abort(mach_port_t child)
{
	int			i;
	pfx_entry_t		entry;

	for (i = 0; i < _Local(pfx_table_len); i++) {
		entry = (pfx_entry_t) _Local(pfx_table)[i];

		if ((entry == NULL) || (entry->type == PFX_INVALID)) {
			continue;
		}

		(void) (entry->clone_target)->clone_abort(child);
	}

	entry = (pfx_entry_t) _Local(null_prefix);
	if ((entry != NULL) && (entry->type != PFX_INVALID)) {
		(void) (entry->clone_target)->clone_abort(child);
	}

	return(ERR_SUCCESS);
}


/*
 *	std_name_reauthenticate: reauthenticate the given prefix table entry
 *	if it is valid and its identity version stamp is not up to date
 *	(ie. our effective ID has changed and we have not authenticated under
 *	our new identity yet).
 */
mach_error_t std_name::reauthenticate(pfx_entry_t entry)
{
	mach_error_t		overall_ret = ERR_SUCCESS;
	usItem*		newobj;
	mach_error_t		ret;

	if ((entry == NULL) || (entry->type == PFX_INVALID) 
	    || (entry->id_stamp == identity_version))
		return(ERR_SUCCESS);

	ret = entry->target->ns_authenticate(NSR_LOOKUP,token,&newobj);
	if (ret == US_INVALID_ACCESS) {
		ret = entry->target->ns_authenticate(NSR_REFERENCE,
						      token,&newobj);
	}
	if (ret != ERR_SUCCESS) {
		if (entry->type != PFX_SYSTEM) {
			ret = ns_invalidate_prefix(entry->prefix,
						    entry->prefix_len);
		}
		if (ret != ERR_SUCCESS) {
			overall_ret = ret;
			entry->type = PFX_INVALID;
			mach_object_dereference(entry->target);
			entry->target = NULL;
			entry->clone_target = NULL;
		}
	} else {
		mach_object_dereference(entry->target);
		entry->target = newobj;
		entry->clone_target = usClone::castdown(entry->target);
		ASSERT_EXPR("Invalid castdown to usClone",
						entry->clone_target != NULL);
		entry->id_stamp = identity_version;
	}
	return(overall_ret);
}


/*
 *	ns_set_token: register an authentication token to be used
 *	to obtain new objects.
 */
mach_error_t std_name::ns_set_token(mach_port_t tok)
{

	identity_version++;	/* new token means new ID */
	token = tok;
	return(ns_flush_cache_prefixes());
}


/*
 * ns_get_prefix_entry: find an existing prefix table entry,
 * or create a new one if appropriate.
 *
 * Return NULL if the entry is not found.
 */
pointer_t std_name::ns_get_prefix_entry(char *path, unsigned int pfxlen, boolean_t okcreate)
{
	int			curindex;
	pfx_entry_t		curentry;
	int			newindex;

	if (pfxlen == 0) {
		curentry = _Local(null_prefix);
		if (curentry == NULL) {
			curentry = (pfx_entry_t) New(struct pfx_entry);
			_Local(null_prefix) = curentry;
			curentry->type = PFX_INVALID;
			curentry->prefix[0] = '\0';
			curentry->prefix_len = 0;
			curentry->target = NULL;
			curentry->clone_target = NULL;
			curentry->target_type = NST_INVALID;
			curentry->canon_path[0] = '\0';
		}
		return((pointer_t) curentry);
	}

	/*
	 * Scan the prefix table, starting at last_index.
	 */
	curindex = _Local(last_index);
	newindex = -1;
	do {
		if (++curindex >= _Local(pfx_table_len)) {
			curindex = 0;
		}
		curentry = (pfx_entry_t) _Local(pfx_table)[curindex];

		if ((curentry == NULL) || (curentry->type == PFX_INVALID)) {
			newindex = curindex;
			continue;
		}

		if ((curentry->prefix_len == pfxlen) &&
			(bcmp(curentry->prefix,path,pfxlen) == 0)) {
			return((pointer_t) curentry);
		}

		if ((newindex < 0) && (curentry->type == PFX_CACHE)) {
			newindex = curindex;
		}
	} while (curindex != _Local(last_index));

	/*
	 * The prefix has not been found. Think about
	 * creating a new entry.
	 */
	if (!okcreate) {
		return(NULL);
	}
	if (newindex < 0) {
		ERROR((0,"*** Prefix Table Full ***"));
		return(NULL);
	}

	/*
	 * Clean-up an existing entry if needed;
	 * allocate a new entry if needed.
	 */
	curentry = (pfx_entry_t) _Local(pfx_table)[newindex];
	if (curentry) {
		mach_object_dereference(curentry->target);
	} else {
		curentry = (pfx_entry_t) New(struct pfx_entry);
		_Local(pfx_table)[newindex] = (struct pfx_entry *) curentry;
	}
	curentry->type = PFX_INVALID;
	bcopy(path,curentry->prefix,pfxlen);
	curentry->prefix_len = pfxlen;
	curentry->target = NULL;
	curentry->clone_target = NULL;
	curentry->target_type = NST_INVALID;
	curentry->canon_path[0] = '\0';

	/*
	 * Update the GC sweeper.
	 */
	_Local(last_index) = newindex;

	return((pointer_t) curentry);
}


/*
 *	ns_set_system_prefix: enter and validate a permanent prefix
 *	in the prefix table
 */
mach_error_t 
std_name::ns_set_system_prefix(char *pfxpath, usItem* pfxobj, 
				ns_type_t objtype, char *canonpath)
{
	mach_error_t		err;
	pfx_entry_t		entry;
	unsigned int		pfxlen = strlen(pfxpath);

	/*
	 * Find an appropriate entry.
	 */
	entry = (pfx_entry_t) ns_get_prefix_entry(pfxpath,pfxlen,TRUE);
	if (entry == NULL) {
		return(NS_PREFIX_OVERFLOW);
	}

	/*
	 * Setup the entry.
	 */
	mach_object_dereference(entry->target);
	entry->type = PFX_SYSTEM;
	strcpy(entry->canon_path,canonpath);
	entry->target_type = objtype;

	/*
	 * Get an authenticated channel.
	 */
	err = pfxobj->ns_authenticate(NSR_LOOKUP,_Local(token),
				       &entry->target);

	if (err != ERR_SUCCESS) {
		entry->type = PFX_INVALID;
		entry->target = NULL;
		entry->clone_target = NULL;
		entry->target_type = NST_INVALID;
		return (err);
	}

	entry->clone_target = usClone::castdown(entry->target);
	ASSERT_EXPR("Invalid castdown to usClone",
						entry->clone_target != NULL);

	/*
	 * Update the identity stamp for this prefix, since we just
	 * authenticated successfully.
	 */
	entry->id_stamp = identity_version;

	DEBUG0(resolve_debug,(0,
		"ns_set_system_prefix: pfxpath=\"%s\", canonpath=\"%s\" \n",
		pfxpath,canonpath));

	(void) ns_flush_cache_prefixes();

	return(ERR_SUCCESS);
}


/*
 *	ns_set_user_prefix: enter a user prefix into the prefix
 *	table
 */
mach_error_t std_name::ns_set_user_prefix(char *pfxpath, char *targetpath)
{
	pfx_entry_t		entry;
	unsigned int		pfxlen = strlen(pfxpath);
	usItem*		pfxobj;
	ns_type_t		pfxtype;
	ns_path_t		canonpath;
	mach_error_t		ret;

	/*
	 * Find the target object.
	 */
	ret = ns_resolve_fully(targetpath,NSF_FOLLOW_ALL,
				NSR_LOOKUP,&pfxobj,&pfxtype,canonpath);
	if (ret != ERR_SUCCESS) {
		return (ret);
	}

	/*
	 * Find an appropriate entry.
	 */
	entry = (pfx_entry_t) ns_get_prefix_entry(pfxpath,pfxlen,TRUE);
	if (entry == NULL) {
		mach_object_dereference(pfxobj);
		return(NS_PREFIX_OVERFLOW);
	}

	/*
	 * Setup the entry.
	 */
	mach_object_dereference(entry->target);
	entry->type = PFX_USER;
	entry->target = pfxobj;
	entry->clone_target = usClone::castdown(entry->target);
	ASSERT_EXPR("Invalid castdown to usClone",
						entry->clone_target != NULL);
	entry->target_type = pfxtype;
	strcpy(entry->canon_path,canonpath);

	/*
	 * Update the identity stamp for this prefix.
	 * We're assuming that this prefix was authenticated
	 * as it was being resolved.
	 */
	entry->id_stamp = identity_version;

	DEBUG0(resolve_debug,(0,
		"ns_set_user_prefix: pfxpath=\"%s\", canonpath=\"%s\" \n",
		pfxpath,canonpath));

	(void) ns_flush_cache_prefixes();

	return(ERR_SUCCESS);
}


/*
 *	ns_set_cache_prefix: enter a cache prefix into the prefix
 *	table.
 */
mach_error_t 
std_name::ns_set_cache_prefix(char *pfxpath, usItem* pfxobj, 
				ns_type_t objtype, char *canonpath)
{
	pfx_entry_t		entry;
	unsigned int		pfxlen = strlen(pfxpath);

	/*
	 * Find an appropriate entry.
	 */
	entry = (pfx_entry_t) ns_get_prefix_entry(pfxpath,pfxlen,TRUE);
	if (entry == NULL) {
		return(NS_PREFIX_OVERFLOW);
	}

	/*
	 * Setup the entry.
	 */
	mach_object_dereference(entry->target);
	entry->type = PFX_CACHE;
	strcpy(entry->canon_path,canonpath);
	entry->target = pfxobj;
	entry->clone_target = usClone::castdown(entry->target);
	ASSERT_EXPR("Invalid castdown to usClone",
						entry->clone_target != NULL);
	entry->target_type = objtype;
	mach_object_reference(pfxobj);

	/*
	 * Update the identity stamp for this prefix.
	 * We're assuming that this prefix was authenticated
	 * as it was being resolved.
	 */
	entry->id_stamp = identity_version;

	DEBUG0(resolve_debug,(0,
		"ns_set_cache_prefix: pfxpath=\"%s\", canonpath=\"%s\" \n",
		pfxpath,canonpath));

	return(ERR_SUCCESS);
}


/*
 *	ns_invalidate prefix: invalidate a (dynamic) prefix.
 *
 *	If this is a cache prefix, get rid of it.
 *	If this is a user prefix, re-evaluate it.
 *	If this is a system prefix, print a warning, but do nothing
 *	more, since system prefixes can never be invalid.
 *
 * 	The prefix table must be locked before invoking this function.
 */
mach_error_t std_name::ns_invalidate_prefix(char *path, unsigned int pfxlen)
{
	ns_path_t		pfxpath;
	ns_path_t		canonpath;
	pfx_entry_t		entry;
	mach_error_t		ret;

	bcopy(path,pfxpath,pfxlen);
	pfxpath[pfxlen] = '\0';

	DEBUG1(resolve_debug,(0,
		"ns_invalidate_prefix: pfxpath=\"%s\"\n",pfxpath));

	entry = (pfx_entry_t) ns_get_prefix_entry(pfxpath,pfxlen,FALSE);

	if (entry == NULL) {
		return(ERR_SUCCESS);
	}
	
	DEBUG1((resolve_debug>1),(0,
		"ns_invalidate_prefix: found prefix, type=%d\n",entry->type));

	switch(entry->type) {
		case PFX_SYSTEM:
			ERROR((0,
		"Warning: attempt to invalidate a system prefix: \'%s\'",
				pfxpath));
			return(ERR_SUCCESS);

		case PFX_USER:
			strcpy(canonpath,entry->canon_path);
			entry->type = PFX_INVALID;
			mach_object_dereference(entry->target);
			entry->target = NULL;
			entry->clone_target = NULL;
			entry->target_type = NST_INVALID;
			ret = ns_set_user_prefix(pfxpath,canonpath);
			return(ret);

		case PFX_CACHE:
			entry->type = PFX_INVALID;
			mach_object_dereference(entry->target);
			entry->target = NULL;
			entry->clone_target = NULL;
			entry->target_type = NST_INVALID;
			return(ERR_SUCCESS);

		case PFX_INVALID:
			return(ERR_SUCCESS);

		default:
			bcopy(path,pfxpath,pfxlen);
			pfxpath[pfxlen] = '\0';
			ERROR((0,
			"ns_invalidate_prefix: unknown prefix type: %d (%s)",
				entry->type,pfxpath));
			entry->type = PFX_INVALID;
			mach_object_dereference(entry->target);
			entry->target = NULL;
			entry->clone_target = NULL;
			entry->target_type = NST_INVALID;
			return(ERR_SUCCESS);
	}
}


/*
 *	ns_flush_cache_prefixes: get rid of all cache prefixes.
 */
mach_error_t std_name::ns_flush_cache_prefixes()
{
	int			i;
	pfx_entry_t		entry;

	DEBUG0(resolve_debug,(0,
		"ns_flush_cache_prefixes: cleaning everything \n"));

	for(i = 0; i < _Local(pfx_table_len); i++) {
		entry = (pfx_entry_t) _Local(pfx_table)[i];
		if ((entry != NULL) && (entry->type == PFX_CACHE)) {
			entry->type = PFX_INVALID;
			mach_object_dereference(entry->target);
			entry->target = NULL;
			entry->clone_target = NULL;
			entry->target_type = NST_INVALID;
		}
	}

	return(ERR_SUCCESS);
}


/*
 *	ns_find_prefix: find the longest initial prefix in a path.
 *	return the naming object associated with that prefix, the
 *	length of the prefix found and the canonical path.
 */
mach_error_t 
std_name::ns_find_prefix(char *path, usItem** pfxobj, ns_type_t* objtype,
			  unsigned int *pfxlen, char *canonpath)
{
	unsigned int		best_len;
	pfx_entry_t		best_entry;
	pfx_entry_t		cur_entry;
	int			i, j;

	DEBUG1((resolve_debug>1),(0,
		"ns_find_prefix: path=\"%s\"\n",path));
	DEBUG2(resolve_debug,(0,
		"ns_find_prefix: path=\"%s\"\n",path));

	/*
	 * First find the right entry in the prefix table.
	 *
	 * XXX We use a simple linear search for now, but this should
	 * be improved with some trie algorithm.
	 */
	best_len = 0;
	best_entry = (pfx_entry_t) _Local(null_prefix);
	for (i = 0; i < _Local(pfx_table_len); i++) {
		if ((cur_entry = (pfx_entry_t) _Local(pfx_table)[i]) == NULL)
			continue;

		if (cur_entry->type == PFX_INVALID)
			continue;

		if (cur_entry->prefix_len < best_len)
			continue;

		for (j = 0; j < cur_entry->prefix_len; j++) {
			if (path[j] != cur_entry->prefix[j])
				goto next_entry;
		}
		if ((path[j] != '/') && (path[j-1] != '/')
				&& (path[j] != '\0')) /* XXX full match OK? */
			continue;

		best_len = cur_entry->prefix_len;
		best_entry = cur_entry;

		next_entry:	;
	}
	
	/*
	 * Return the results.
	 */
	if (best_entry == NULL) {
		*pfxobj = NULL;
		*pfxlen = 0;
		return(NS_INVALID_PREFIX);
	}
	/*
	 * Reauthenticate this prefix if our identity has changed
	 * since it was last authenticated.
	 */
	if (best_entry->id_stamp != identity_version)
		reauthenticate(best_entry);

	mach_object_reference(best_entry->target);

	*pfxobj = best_entry->target;
	*objtype = best_entry->target_type;
	*pfxlen = best_len;
	if (canonpath != NULL) {
		strcpy(canonpath, best_entry->canon_path);
	}

	DEBUG1((resolve_debug>1),(0,
"ns_find_prefix: found entry: type=%d, pfxpath=\"%s\", canonpath=\"%s\"\n",
		best_entry->type,best_entry->prefix,best_entry->canon_path));
	DEBUG2(resolve_debug,(0,
"ns_find_prefix: found entry: type=%d, pfxpath=\"%s\", canonpath=\"%s\"\n",
		best_entry->type,best_entry->prefix,best_entry->canon_path));

	if (*pfxobj != NULL) {
		return(ERR_SUCCESS);
	} else {
		return(NS_INVALID_PREFIX);
	}
}


/*
 * Limits to prevent infinite loops.
 */
#define	MAX_RETRIES		10
#define	MAX_FORWARDS		100


/*
 * Extract the next component out of a path name.
 */
#define FIND_COMPONENT(_cp,_len,_atp) {				\
	char	*_np;						\
								\
	(_atp) = 0;						\
	while ((*(_cp) != '\0') && (*(_cp) == '/')) (_cp)++;	\
	_np = (_cp);						\
	while ((*_np != '\0') && (*_np != '/')) {		\
		if (*_np == '@') (_atp) = _np;			\
		_np++;						\
	}							\
	(_len) = _np - (_cp);					\
}


/*
 * Backspace one full component from the current location in a path name.
 *
 * Take care not to delete the initial "/" in an absolute path name.
 */
#define	BACKSPACE_COMPONENT(_cp,_start) {			\
	(_cp)--;						\
	while (((_cp) > (_start)) && (*(_cp) == '/')) {		\
		(_cp)--;					\
	}							\
	while (((_cp) > (_start)) && (*(_cp) != '/')) {		\
		(_cp)--;					\
	}							\
	while (((_cp) > (_start)) && (*(_cp) == '/')) {		\
		(_cp)--;					\
	}							\
	(_cp)++;						\
}


/*
 * Advance a character pointer past any sequence of slashes.
 */
#define	SKIP_SLASH(_cp) {					\
	while ((*(_cp) == '/') && (*(_cp) != '\0')) (_cp)++;	\
}

/*
 * Make sure there is a "/" at the end of a string.
 */
#define	ADD_SLASH(_cp) {					\
	if (*((_cp) - 1) != '/') *(_cp)++ = '/';		\
}


/*
 * Copy inpath into outpath and eliminate extraneous characters.
 *
 * outpath is guaranteed to contain only a single slash between
 * each component, and no "." components. There is a slash at
 * the end of outpath if the end of inpath is "/" or "/.".
 * If inpath is ".", "./" or "", outpath is "".
 * Make sure that each component is less than compsize and the
 * whole path is less than pathsize.
 */
#define	CLEAN_PATH(inpath,outpath,pathsize,compsize,ret) {	\
	char		*_ip = inpath;				\
	char		*_op = outpath;				\
	int		_ps = 0;				\
	int		_cs = 0;				\
								\
	/*							\
	 * Beginning slash.					\
	 */							\
	if (*_ip == '/') {					\
		*_op++ = '/';					\
		_ps++;						\
	}							\
								\
	/*							\
	 * Copy all components.					\
	 */							\
	while (*_ip != '\0' && _ps < pathsize && _cs < compsize) { \
		if ((_op != outpath) && (*(_op-1) != '/')) {	\
			if (++_ps < pathsize)			\
				*_op++ = '/';			\
			else					\
				break;				\
		}						\
		SKIP_SLASH(_ip);				\
		if ((*_ip == '.') &&				\
			((*(_ip+1) == '/') || (*(_ip+1) == '\0'))) { \
			_ip++;					\
		} else {					\
			_cs = 0;				\
			while ((*_ip != '/') && (*_ip != '\0')) { \
				if (++_cs < compsize && ++_ps < pathsize) \
					*_op++ = *_ip++;	\
				else				\
					break;			\
			}					\
		}						\
	}							\
	if (_ps < pathsize && _cs < compsize) {			\
		*_op = '\0';					\
		ret = ERR_SUCCESS;				\
	}							\
	else							\
		ret = NS_PATH_TOO_LONG;				\
}


/*
 * Decide if a given component is "." or "..".
 */
#define	IS_DOT(_cp,_len)					\
		(((_len) == 1) && (*(_cp) == '.'))
#define	IS_DOTDOT(_cp,_len)					\
		(((_len) == 2) && (*(_cp) == '.') && (*((_cp) + 1) == '.'))

/*
 * The FIXUP_INPOS and SAVE_CACHE_PREFIX are changes made by grm
 * to do the aggressive caching of prefixs.
 */
/*
 * The purpose of FIXUP_INPOS is to make sure that we are 
 * pointing to the position before the trailing slash of the cache
 * path in the inputpath.
 *
 * Types of paths:  inpath = '/' , 'foo/bar', '/foo/bar'
 */
#define FIXUP_INPOS(_ip, _start) {				\
	if (_ip != _start) {					\
		(_ip)--;					\
		if (*(_ip) == '/')				\
			(_ip)--;				\
		if ((_ip) <= (_start))				\
			(_ip) = (_start);			\
		(_ip)++;					\
	}							\
}	

#define SAVE_CACHE_PREFIX(inpath, inpos, canpath, canpos, cobj, ctype) {\
	ns_path_t ipath;						\
	unsigned int il;						\
	ns_path_t cpath;						\
	unsigned int cl;						\
	ns_path_t tbuf;							\
	int tret;							\
	usItem* tobj;							\
	ns_type_t ttype;						\
	unsigned int tlen;                                              \
		                                                        \
	il = (unsigned int)(inpos - inpath);				\
	bcopy(inpath, ipath, il);					\
		                                                        \
	cl = (unsigned int)(canpos - canpath);				\
	bcopy(canpath, cpath, cl);					\
	ipath[il] = '\0';						\
	cpath[cl] = '\0';						\
		                                                        \
	DEBUG2(resolve_debug,						\
	       (0,"SCP: inpath (0x%x) '%s'\n", inpath, inpath));	\
	DEBUG2(resolve_debug,						\
	       (0,"SCP: canpath (0x%x) '%s'\n", canpath, canpath));	\
	DEBUG2(resolve_debug,						\
	       (0,"SCP: ipath (0x%x) '%s'\n", ipath, ipath));		\
	DEBUG2(resolve_debug,						\
	       (0,"SCP: cpath (0x%x) '%s'\n", cpath, cpath));		\
		                                                        \
	if (!il || !cl) {						\
		DEBUG2(resolve_debug,					\
		       (0, "SCP: invalid input or canon path.\n"));	\
	}else{								\
		tret = ns_find_prefix(ipath, &tobj, &ttype, &tlen, tbuf); \
		if ((tlen == il) && (!strncmp(tbuf, cpath, tlen))) {	\
			if (ttype != PFX_CACHE) {			\
				DEBUG2(resolve_debug,			\
				       (0, "SCP: obj in pfx table, not cache type.\n")); \
			}else if (tobj == curobj) {			\
				DEBUG2(resolve_debug,			\
				       (0,"SCP: obj already cached.\n")); \
			}else{						\
				(void) ns_set_cache_prefix(ipath,	\
							   cobj,	\
							   ctype,	\
							   cpath);	\
			}						 \
		}							 \
	}								 \
		                                                         \
	*cache_inputpos = 0;						 \
}			

int debug_resolve_count = 0;

/*
 *	ns_resolve_simple: resolve a path name as far as possible,
 *	and exit when encountering a ".." component. Return the last
 *	useful prefix found before exiting.
 */
/*
 * A rundown of the routine's calling and return paramters are given
 * here.  Please refer to the long comment at the top of the file for
 * more information on the name resolution routines.
 *
 * mach_error_t 
 * std_name::ns_resolve_simple(
 *  usItem**  	pfx_obj,	Pointer to current directory object.
 *  ns_type_t*	pfx_type,	Current directory type.
 *  char**	pfx_inputpos,	Ptr to end of currnet dir's inputpath name.
 *  char**	pfx_canonpos,   Ptr to end of currnet dir's canonpath name.
 *  char**	cache_inputpos,	Ptr to end of prefix cache's inputpath name.
 *  usItem**	out_obj,	Ptr to final component's object.
 *  char**	out_inputpos,	Ptr to last resolved point in inputpath.
 *  char**	out_canonpos, 	Ptr to last resolved point in canonpath.
 *  ns_type_t*	out_type,	Final component's type
 *  char*	inputpath, 	Inputpath to be resolved.
 *  char*	canonpath,	Linear representation of inputpath
 *  ns_mode_t	mode,
 *  ns_access_t	access,
 *  unsigned int forwardcount	Mechanism to ensure non-circuitious refs.
 *  )
 */
mach_error_t 
std_name::ns_resolve_simple(usItem** pfx_obj, ns_type_t* pfx_type,
			    char** pfx_inputpos,
			    char** pfx_canonpos,
			    char** cache_inputpos,
			    usItem** out_obj,
			    char** out_inputpos, char** out_canonpos, 
			    ns_type_t* out_type, char* inputpath, 
			    char* canonpath, ns_mode_t mode,
			    ns_access_t access, unsigned int forwardcount)
{
	mach_error_t		ret;
	int			retrycount = 0;
	unsigned int		pfxlen;
	char			*curinputpos = *pfx_inputpos;
	char			*curcanonpos = *pfx_canonpos;
	usItem*			curobj;
	ns_type_t		curtype;
	char			*newinputpos;
	char			*newcanonpos;
	usItem*			newobj = NULL;
	char			*maxinputpos;
	char			*maxcanonpos;
	unsigned int		len;
	ns_type_t		newtype;
	ns_path_t		newpath;
	unsigned int		usedlen;
	ns_action_t		action;
	ns_access_t		actual_access;
	ns_mode_t		actual_mode;
	char*			atpos;
	char			*remember_inputpos;
	char			*remember_canonpos;
	int			our_debug_count = ++debug_resolve_count;

#define	ABORT(ret) {				\
	mach_object_dereference(curobj);	\
	mach_object_dereference(newobj);	\
	mach_object_dereference(*pfx_obj);	\
	*pfx_obj = NULL;		\
	*out_obj = NULL;		\
	DEBUG1(resolve_debug,(0,"ns_resolve_simple: abort, rc=%s\n",	\
		(char *)mach_error_string(ret)));			\
	return(ret);				\
}

#define	SET_CUR_PREFIX {					\
	mach_object_dereference(*pfx_obj);			\
	*pfx_obj = curobj;					\
	mach_object_reference(*pfx_obj);			\
	*pfx_type = curtype;					\
	*pfx_inputpos = curinputpos;				\
	*pfx_canonpos = curcanonpos;				\
}

#define	CLEAR_CUR_PREFIX {					\
	if (curobj == *pfx_obj) {				\
		(void) ns_invalidate_prefix(inputpath, 	\
				*pfx_inputpos - inputpath);	\
		mach_object_dereference(*pfx_obj);		\
		*pfx_obj = NULL;			\
		*pfx_type = NST_INVALID;			\
	}							\
}

	DEBUG2(resolve_debug,(0,
	       "ns_resolve_simple: BEGIN (#%d) with:\n", our_debug_count));
	DEBUG2(resolve_debug,(0,
	       "*pfx_obj (0x%x), *pfx_type (0x%x)\n",
	       *pfx_obj, *pfx_type));
	DEBUG2(resolve_debug,
	       (0, "*pfx_inputpos (0x%x) '%s'\n", 
		*pfx_inputpos, *pfx_inputpos));
	DEBUG2(resolve_debug,
	       (0, "*pfx_canonpos (0x%x) '%s'\n",
	       *pfx_canonpos, *pfx_canonpos));
	DEBUG2(resolve_debug,
	       (0, "inputpath (0x%x) '%s'\n",
		inputpath, inputpath));
	DEBUG2(resolve_debug,
	       (0, "canonpath (0x%x) '%s'\n",
		canonpath, canonpath));
	DEBUG2(resolve_debug,
	       (0, "cache_inputpos (0x%x) '%s'\n",
		(cache_inputpos ? *cache_inputpos : 0), 
		(cache_inputpos && *cache_inputpos ? *cache_inputpos :
		 "ZERO")));
	DEBUG2(resolve_debug,
	       (0, "mode (0x%x), access (0x%x), forwardcount (0x%x) \n\n",
		mode, access, forwardcount));

	curobj = *pfx_obj;
	mach_object_reference(curobj);
	curtype = *pfx_type;
	*curcanonpos = '\0';
	DEBUG1((resolve_debug>1),(0,
		"ns_resolve_simple: input=\"%s\", canonpath=\"%s\" %s\n",
		curinputpos,canonpath));

	/*
	 * Make sure we do have a prefix.
	 */
	if (curobj == NULL) {
		ret = ns_find_prefix(inputpath,&curobj,&curtype,
							&pfxlen,canonpath);
		if (ret != ERR_SUCCESS) {
			ABORT(ret);
		}
		curinputpos = inputpath + pfxlen;
		curcanonpos = canonpath + strlen(canonpath);
		SET_CUR_PREFIX;
	}

		

	/*
	 * Advance past possible slash at the beginning of the path.
	 * There can be at most a single slash (clean path), and it
	 * must be reproduced in the canonpath.
	 *
	 * This will result in a "/" at the end of the current path
	 * only in two cases:
	 *	- if the whole current path is "/": we do want to
	 *		perform one last resolve step in that case.
	 *	- if the current path begins with "/.." : this cannot
	 *		happen, since the initial "/" would be
	 *		consumed either by the prefix table, or by
	 *		the section of code in ns_resolve_complex()
	 *		that deals with sequences of ".." components.
	 */
	if (*curinputpos == '/') {
		curinputpos++;
		ADD_SLASH(curcanonpos);
	}

	/*
	 * grm's code below.  Remember the current input and canonpos
	 * here.
	 */
	if (cache_inputpos) {
		/* curobj has the prefix for this position. */
		remember_inputpos = curinputpos;
		remember_canonpos = curcanonpos;
		DEBUG2(resolve_debug,
		       (0, "nsS: RC1 ri (0x%x) '%s'\n",
			remember_inputpos, remember_inputpos));
		DEBUG2(resolve_debug,
		       (0, "nsS: RC1 rc (0x%x) '%s'\n",
			remember_canonpos, remember_canonpos));
	}

	/*
	 * Decide how far to go in the input path.
	 *
	 * Copy a sequence of inputpath components
	 * up to the first ".." to the end of canonpath.
	 * Do not include a "/" at the end of the path, unless
	 * that '/' constitutes the whole current path.
	 */

	newinputpos = curinputpos;
	newcanonpos = curcanonpos;
	for (;;) {
		maxinputpos = newinputpos;
		FIND_COMPONENT(newinputpos,len,atpos);

		if ((len == 0) || (IS_DOTDOT(newinputpos,len))) {
			break;
		}

		ADD_SLASH(newcanonpos);

		/*
		 * Substitute "@sys".
		 *
		 * XXX What if there are several "@" characters in the
		 * component?
		 */
		if ((atpos) && (! bcmp(atpos,"@sys",4))) {
			int prelen = atpos - newinputpos;
			if (prelen) {
				bcopy(newinputpos,newcanonpos,prelen);
				newinputpos += prelen;
				newcanonpos += prelen;
			}
			bcopy(ATSYS_STRING,newcanonpos,ATSYS_STRING_LEN);
			newinputpos += 4;
			newcanonpos += ATSYS_STRING_LEN;
			int postlen = len - prelen - 4;
			if (postlen) {
				bcopy(newinputpos,newcanonpos,postlen);
				newinputpos += postlen;
				newcanonpos += postlen;
			}
		} else {
			bcopy(newinputpos,newcanonpos,len);
			newinputpos += len;
			newcanonpos += len;
		}
	}
	*newcanonpos = '\0';
	maxcanonpos = newcanonpos;
	newinputpos = maxinputpos;

	/*
	 * We want the strings at curcanonpos and at curinputpos to
	 * be exactly identical, so that the "usedlen" return from
	 * ns_resolve() can be applied to both.
	 *
	 * The code above may have added a slash at the beginning of
	 * curcanonpos to allow the concatenation of the new inputpath
	 * components. Since curinputpos does not itself begin with
	 * a slash (see above), adjust curcanonpos accordingly.
	 */
	if (*curcanonpos == '/') {
		curcanonpos++;
	}

	/*
	 * Decide what access and mode to use in resolving.
	 * Anywhere but at the end of the input path, use
	 * NSR_LOOKUP, and follow forwarding points.
	 */
	if (*newinputpos == '\0') {
		actual_mode = mode;
		actual_access = access;
	} else {
		actual_mode = NSF_FOLLOW_ALL;
		actual_access = NSR_LOOKUP;
	}

	for(;;) {
		/*
		 * At this point, curobj represents NSR_LOOKUP access
		 * for the location designated by curcanonpos
		 * and curinputpos.
		 */

		/*
		 * Resolve the new string built in canonpath.
		 */
		if (*curcanonpos == '\0') {
			DEBUG1((resolve_debug>1),(0,
			"ns_resolve_simple: calling ns_duplicate()\n"));
			ret = curobj->ns_duplicate(actual_access,&newobj);
			newtype = curtype;
			action = 0;
			DEBUG1((resolve_debug>1),(0,
			"ns_resolve_simple: ns_duplicate() returned %d (%s)\n",
				ret,mach_error_string(ret)));
			/*
			 * grm's code here.  So that we don't freak out
			 * below.	
			 */
			usedlen = 0;

		} else {
			DEBUG1((resolve_debug>1),(0,
				"ns_resolve_simple: calling ns_resolve(%s)\n",
				curcanonpos));
			usName* x = usName::castdown(curobj);
			if (x == 0) {
				ret = MACH_OBJECT_NO_SUCH_OPERATION;
			} else {

				DEBUG2(resolve_debug,
				       (0, "nrs: b nr1: curcanonpos (0x%x) '%s', actual_mode (%d)\n",
					curcanonpos, curcanonpos, actual_mode));
				DEBUG2(resolve_debug,
				       (0, "actual_access (0x%x)\n\n",
					actual_access));

				ret = x->ns_resolve(curcanonpos, actual_mode, 
					     actual_access, &newobj, &newtype,
					     newpath, (int*)&usedlen, &action);

				DEBUG2(resolve_debug,
				       (0, "\nnrs: a nr1: curcanonpos (0x%x) '%s', actual_mode (%d)\n",
					curcanonpos, curcanonpos, actual_mode));
				DEBUG2(resolve_debug,
				       (0, "actual_access (0x%x) , new_obj (0x%x), newtype (%d)\n",
					actual_access, newobj, newtype));
				DEBUG2(resolve_debug,
				       (0, "newpath (0x%x) '%s', usedlen (%d), action (%d)\n\n",
					newpath, newpath, usedlen, action));

				DEBUG1((resolve_debug>1),(0,
"ns_resolve_simple: ns_resolve() returned %d (%s), action=0x%x, usedlen=%d\n",
					ret,mach_error_string(ret),
					action,usedlen));
			}
		}
		switch (ret) {
			case ERR_SUCCESS:
				break;

			case NS_NAME_NOT_FOUND:
			case US_INVALID_ACCESS:
			case US_ACCESS_DENIED:
			case MACH_OBJECT_NO_SUCH_OPERATION:
				ABORT(ret);

			case US_NOT_AUTHENTICATED:
				if (retrycount++ > MAX_RETRIES) {
					ABORT(NS_INFINITE_RETRY);
				}
				CLEAR_CUR_PREFIX;
				ret = curobj->ns_authenticate(NSR_LOOKUP,
							       _Local(token),
							       &newobj);
				if (ret != ERR_SUCCESS) {
					ABORT(ret);
				}
				mach_object_dereference(curobj);
				curobj = newobj;
				continue;

			case MACH_SEND_INVALID_DEST:
				ret = NS_INVALID_HANDLE;
			case NS_INVALID_HANDLE:
				CLEAR_CUR_PREFIX;
				ABORT(ret);

			default:
				ABORT(ret);
		}

		/*
		 * grm's code here:
		 *
		 * Check to see if the ns_resolve went past cache
		 * inputpos, if it did, then just cache what we
		 * remember.
		 */
		if (cache_inputpos && *cache_inputpos) {

			DEBUG2(resolve_debug,
			       (0, "nsS: RC2 curinputpos (0x%x) + usedlen (0x%x) = (0x%x) '%s'\n",
				curinputpos, usedlen, (curinputpos + usedlen),
				(curinputpos + usedlen)));
			
			if (*cache_inputpos < (curinputpos + usedlen)) {
				DEBUG2(resolve_debug,
				       (0, "nsS: save_cache_prefix #3: curobj (0x%x), curtype (%d).\n",
					curobj, curtype));
				FIXUP_INPOS(remember_inputpos, inputpath);
				FIXUP_INPOS(remember_canonpos, canonpath);
				SAVE_CACHE_PREFIX(inputpath,
						  remember_inputpos,
						  canonpath,
						  remember_canonpos,
						  curobj,
						  curtype);
			}else{
				remember_inputpos = curinputpos + usedlen;
				remember_canonpos = curcanonpos + usedlen;
				DEBUG2(resolve_debug,
				       (0, "nsS: RC2.1 ri (0x%x) '%s'\n",
					remember_inputpos, remember_inputpos));
				DEBUG2(resolve_debug,
				       (0, "nsS: RC2.1 rc (0x%x) '%s'\n",
					remember_canonpos, remember_canonpos));
			}
		}

		/*
		 * Update the input position, and make sure that we
		 * are not in an infinite loop.
		 */
		if (action & (NSA_FORWARD | NSA_TFORWARD)) {
			newinputpos = curinputpos + usedlen;
			newcanonpos = curcanonpos + usedlen;
			if (*(newinputpos - 1) == '/') {
				newinputpos--;
				newcanonpos--;
			}
		}
		if (forwardcount++ > MAX_FORWARDS) {
			if (action & NSA_TFORWARD) {
				ABORT(NS_TFORWARD_FAILURE);
			} else {
				ABORT(NS_INFINITE_FORWARD);
			}
		}

		/*
		 * Make sure the new object returned by ns_resolve()
		 * is properly authenticated.
		 */
		if (action & NSA_AUTHENTICATE) {
			usItem*	authobj;
			ns_access_t	auth_access;

			if (
				(action & (NSA_FORWARD | NSA_TFORWARD))
				&&
				(
					(*newinputpos != '\0')
				 	||
					(
						(newpath != NULL)
						&&
						(newpath[0] != '\0')
					)
				)
			) {
				auth_access = NSR_LOOKUP;
			} else {
				auth_access = actual_access;
			}

			ret = newobj->ns_authenticate(auth_access,
						       _Local(token),&authobj);
			if (ret != ERR_SUCCESS) {
				ABORT(ret);
			}
			mach_object_dereference(newobj);
			newobj = authobj;
		}

		/*
		 * If the new object returned does not correspond to a
		 * forwarding point, we are done.
		 */
		if (!(action & (NSA_FORWARD | NSA_TFORWARD))) {
			/*
			 * grm's code below:
			 *
			 * If this spot is the right spot, then cache
			 * it, but I don't think it ever can be.
			 */
			if (cache_inputpos && *cache_inputpos) {
				DEBUG2(resolve_debug,
				       (0, "nsS: RC3 curinputpos (0x%x) + usedlen (0x%x) = (0x%x) '%s'\n",
					curinputpos, usedlen,
					(curinputpos + usedlen)));
				
				if (*cache_inputpos == 
				    (curinputpos + usedlen)) {
					DEBUG2(resolve_debug,
					       (0, "nsS: save_cache_prefix #4: curobj (0x%x), curtype (%d).\n",
						newobj, newtype));
					SAVE_CACHE_PREFIX(inputpath, 
							  remember_inputpos,
							  canonpath,
							  remember_canonpos,
							  newobj, newtype);
				}
			}
			break;
		}

		/*
		 * Handle forwarding points.
		 */
		if ((newpath != NULL) && (newpath[0] != '\0')) {
			/*
			 * Resolve the symlink before continuing.
			 */
			ns_path_t		fwd_path;
			char			*fwd_inputpos;
			char			*fwd_canonpos;
			ns_mode_t		fwd_mode;
			ns_access_t		fwd_access;

#if	0
			/*
			 * grm's changes here:
			 *
			 */
			if (cache_inputpos) {
				remember_inputpos = newinputpos + 1;
				DEBUG2(resolve_debug,
				       (0, "nsS: RC4 ri (0x%x) '%s'\n",
					remember_inputpos, remember_inputpos));
			}
#endif

			if (newobj == NULL) {
				/*
				 * Absolute symlink.
				 */
				fwd_canonpos = canonpath;
			} else {
				/*
				 * Relative symlink.
				 *
				 * Assume that newobj represents the
				 * directory containing the symlink,
				 * and go back one path component
				 * to get its canonical path.
				 */
				fwd_canonpos = newcanonpos;
				BACKSPACE_COMPONENT(fwd_canonpos,canonpath);
			}

			if (*newinputpos == '\0') {
				fwd_mode = actual_mode;
				fwd_access = actual_access;
			} else {
				fwd_mode = NSF_FOLLOW_ALL;
				fwd_access = NSR_LOOKUP;
			}

			mach_object_dereference(curobj);
			curobj = newobj;
			curtype = newtype;
			newobj = NULL;
			CLEAN_PATH(newpath,fwd_path,
				   sizeof(ns_path_t),sizeof(ns_name_t),ret);
			if (ret != ERR_SUCCESS)
				ABORT(ret);
			fwd_inputpos = fwd_path;
			*fwd_canonpos = '\0';

			DEBUG2(resolve_debug,(0,
			       "nrs (2): calling nsr_complex.\n"));
			ret = ns_resolve_complex(&curobj,&curtype,
						 &fwd_inputpos,&fwd_canonpos,
						 (char **)0,
						 &newobj,&newcanonpos,
						 &newtype,
						 fwd_path,canonpath,
						 fwd_mode,fwd_access,
						 forwardcount);
			DEBUG2(resolve_debug,(0,
			       "nrs (2): exited nsr_complex.\n"));
			if (ret != ERR_SUCCESS) {
				/*
				 * XXX Special hack for transparent symlinks.
				 */
				if ((ret == NS_NAME_NOT_FOUND) && 
						(action & NSA_TFORWARD)) {
					ABORT(NS_TFORWARD_FAILURE);
				} else {
					ABORT(ret);
				}
			}

			/*
			 * grm's changes here:
			 *
			 * If we were resolving the object pointed to
			 * by the cache inputpos, then we've got back
			 * the right object from nsC.  Cache it!
			 */
			if (cache_inputpos && *cache_inputpos) {
				remember_canonpos = newcanonpos;
				DEBUG2(resolve_debug,
				       (0, "nsS: RC5 ri (0x%x) '%s'\n",
					remember_inputpos, remember_inputpos));

				if (remember_inputpos == *cache_inputpos) {
					DEBUG2(resolve_debug,
					       (0,"nsS: save_cache_prefix #5: curobj (0x%x), curtype (%d).\n",
						newobj, newtype));
					FIXUP_INPOS(remember_inputpos,
						    inputpath);
					SAVE_CACHE_PREFIX(inputpath,
							  remember_inputpos,
							  canonpath,
							  remember_canonpos,
							  newobj,
							  newtype);
				}
			}

			/*
			 * Copy the remainder of the input path at the
			 * end of the expanded canonical path.
			 * We are guaranteed that there is a '/' at
			 * newinputpos if there is anything at all.
			 */
			if (newinputpos < maxinputpos) {
				bcopy(newinputpos,newcanonpos,
						maxinputpos - newinputpos);
				maxcanonpos = newcanonpos + 
				(unsigned int) (maxinputpos - newinputpos);
			}

			/*
			 * grm's change:
			 * 
			 * For now just twiddle this....
			 *
			 */
			if (fwd_canonpos < curcanonpos) {
				*pfx_canonpos = fwd_canonpos;
			}

		} else {
			/*
			 * Simple mount point. Nothing more to do.
			 */
		}

		mach_object_dereference(curobj);
		curobj = NULL;
		curtype = NST_INVALID;

		/*
		 * At this point, newobj, newinputpos and newcanonpos
		 * correspond to the end of the forwarding point.
		 * Prepare for a new iteration, unless we are finished.
		 */
		if (newinputpos >= maxinputpos) {
			break;
		} else {
			curobj = newobj;
			curtype = newtype;
			newobj = NULL;
			curcanonpos = newcanonpos;
			curinputpos = newinputpos;
			newinputpos = maxinputpos;
			newcanonpos = maxcanonpos;
			*newcanonpos = '\0';
			SET_CUR_PREFIX;
		}
	}

	/*
	 * Setup all the return arguments.
	 */
	*out_obj = newobj;
	*out_inputpos = newinputpos;
	*out_canonpos = newcanonpos;
	*out_type = newtype;

	DEBUG1((resolve_debug>1),(0,
		"ns_resolve_simple: returning (rc=%d),canonpath=\'%s\"\n",
		ret,canonpath));

	DEBUG2(resolve_debug,
	       (0, "ns_resolve_simple: END (#%d) with:\n", our_debug_count));
	DEBUG2(resolve_debug,
	       (0, "*pfx_obj (0x%x), *pfx_type (0x%x)\n",
		*pfx_obj, *pfx_type));
	DEBUG2(resolve_debug,
	       (0, "*pfx_inputpos (0x%x) '%s'\n", 
		*pfx_inputpos, *pfx_inputpos));
	DEBUG2(resolve_debug,
	       (0, "*pfx_canonpos (0x%x) '%s'\n",
	       *pfx_canonpos, *pfx_canonpos));
	DEBUG2(resolve_debug,
	       (0, "*out_obj (0x%x), *out_type (0x%x)\n",
		*out_obj, *out_type));
	DEBUG2(resolve_debug,
	       (0, "*out_inputpos (0x%x) '%s'\n", 
		*out_inputpos, *out_inputpos));
	DEBUG2(resolve_debug,
	       (0, "*out_canonpos (0x%x) '%s'\n",
	       *out_canonpos, *out_canonpos));
	DEBUG2(resolve_debug,
	       (0, "inputpath (0x%x) '%s'\n",
		inputpath, inputpath));
	DEBUG2(resolve_debug,
	       (0, "canonpath (0x%x) '%s'\n",
		canonpath, canonpath));
	DEBUG2(resolve_debug,
	       (0, "cache_inputpos (0x%x) '%s'\n",
		(cache_inputpos ? *cache_inputpos : 0), 
		(cache_inputpos && *cache_inputpos ? *cache_inputpos :
		 "ZERO")));
	DEBUG2(resolve_debug,
	       (0, "mode (0x%x), access (0x%x), forwardcount (0x%x) \n\n",
		mode, access, forwardcount));

	return(ret);

#undef	ABORT
}


/*
 *	ns_resolve_complex: resolve a complex path name, including
 *	".." components. Return the object for the end of the path
 *	name, and the last useful prefix.
 */
/*
 * A rundown of the routine's calling and return paramters are given
 * here.  Please refer to the long comment at the top of the file for
 * more information on the name resolution routines.
 *
 * mach_error_t 
 * std_name::ns_resolve_complex(
 *  usItem**  	pfx_obj,	Pointer to current directory object.
 *  ns_type_t*	pfx_type,	Current directory type.
 *  char**	pfx_inputpos,	Ptr to end of currnet dir's inputpath name.
 *  char**	pfx_canonpos,   Ptr to end of currnet dir's canonpath name.
 *  char**	cache_inputpos,	Ptr to end of prefix cache's inputpath name.
 *  usItem**	out_obj,	Ptr to final component's object.
 *  char**	out_canonpos, 	Ptr to last resolved point in canonpath.
 *  ns_type_t*	out_type,	Final component's type
 *  char*	inputpath, 	Inputpath to be resolved.
 *  char*	canonpath,	Linear representation of inputpath
 *  ns_mode_t	mode,
 *  ns_access_t	access,
 *  unsigned int forwardcount	Mechanism to ensure non-circuitious refs.
 *  )
 */
mach_error_t 
std_name::ns_resolve_complex(usItem** pfx_obj, ns_type_t* pfx_type,
			     char** pfx_inputpos,
			     char** pfx_canonpos,
			     char** cache_inputpos,
			     usItem** out_obj,
			     char** out_canonpos, ns_type_t* out_type,
			     char* inputpath, char* canonpath, ns_mode_t mode,
			     ns_access_t access, unsigned int forwardcount)
{
	mach_error_t		ret;
	char			*curinputpos = *pfx_inputpos;
	char			*curcanonpos = *pfx_canonpos;
	char			*remember_inputpos;
	char			*remember_canonpos;
	usItem*			curobj = *pfx_obj;
	ns_type_t		curtype = *pfx_type;
	char			*newinputpos;
	char			*newcanonpos;
	char			*maxinputpos;
	char			*maxcanonpos;
	usItem*			newobj = NULL;
	unsigned int		len;
	ns_type_t		newtype;
	char*			atpos;
	int			our_debug_count = ++debug_resolve_count;

#define	ABORT(ret) {				\
	mach_object_dereference(curobj);	\
	mach_object_dereference(newobj);	\
	*pfx_obj = NULL;		\
	*out_obj = NULL;		\
	DEBUG1(resolve_debug,(0,"ns_resolve_complex: abort, rc=%s\n",\
		(char *)mach_error_string(ret)));			\
	return(ret);				\
}

	DEBUG1((resolve_debug>1),(0,
		"ns_resolve_complex: input=\"%s\", canonpath=\"%s\" %s\n",
		curinputpos,canonpath));

	DEBUG2(resolve_debug,
	       (0, "ns_resolve_COMPLEX: BEGIN (#%d) with:\n",
		our_debug_count));
	DEBUG2(resolve_debug,
	       (0, "*pfx_obj (0x%x), *pfx_type (0x%x)\n",
	       *pfx_obj, *pfx_type));
	DEBUG2(resolve_debug,
	       (0, "*pfx_inputpos (0x%x) '%s'\n", 
		*pfx_inputpos, *pfx_inputpos));
	DEBUG2(resolve_debug,
	       (0, "*pfx_canonpos (0x%x) '%s'\n",
	       *pfx_canonpos, *pfx_canonpos));
	DEBUG2(resolve_debug,
	       (0, "inputpath (0x%x) '%s'\n",
		inputpath, inputpath));
	DEBUG2(resolve_debug,
	       (0, "canonpath (0x%x) '%s'\n",
		canonpath, canonpath));
	DEBUG2(resolve_debug,
	       (0, "cache_inputpos (0x%x) '%s'\n",
		(cache_inputpos ? *cache_inputpos : 0), 
		(cache_inputpos && *cache_inputpos ? *cache_inputpos :
		 "ZERO")));
	DEBUG2(resolve_debug,
	       (0, "mode (0x%x), access (0x%x), forwardcount (0x%x) \n\n",
		mode, access, forwardcount));

	for (;;) {
		DEBUG1((resolve_debug>1),(0,
			"ns_resolve_complex: top of the loop\n"));

		/*
		 * Resolve as far as possible without worrying
		 * about special ".." components.
		 */
		DEBUG2(resolve_debug,(0,
		       "nrc: (1) calling ns_simple.\n"));
		ret = ns_resolve_simple(&curobj, &curtype, 
					&curinputpos, &curcanonpos,
					cache_inputpos,
					&newobj, &newinputpos,&newcanonpos,
					&newtype, inputpath,canonpath, mode,
					access,forwardcount);
		DEBUG2(resolve_debug,(0,
		       "nrc: (1) exited ns_simple.\n"));
		if (ret != ERR_SUCCESS) {
			ABORT(ret);
		}

		/*
		 * At this point, we have processed inputpath as
		 * far as possible with simple resolving steps.
		 * We have either reached the end, or we have reached
		 * a ".." component which must be handled specially.
		 *
		 * If we have reached the end of the input, exit.
		 * newobj and curobj represent the desired object
		 * and the best prefix, respectively.
		 */
		DEBUG2(resolve_debug,(0,
				"ns_resolve_complex: rest of input: \"%s\"\n",
				newinputpos));
		if (*newinputpos == '\0') {
			break;
		}

		/*
		 * Handling of ".." components:
		 *
		 * process all consecutive ".." components before
		 * resolving again.
		 */

		/*
		 * grm's changes here:
		 *
		 */
		if (cache_inputpos) {
			remember_inputpos = curinputpos;
			remember_canonpos = curcanonpos;
			DEBUG2(resolve_debug,
			       (0, "nsC: RC6 ri (0x%x) '%s'\n",
				remember_inputpos, remember_inputpos));
			DEBUG2(resolve_debug,
			       (0, "nsC: RC6 rc (0x%x) '%s'\n",
				remember_canonpos, remember_canonpos));
		}

		/*
		 *	Advance in inputpath and backspace in canonpath
		 *	for all consecutive ".." components.
		 *
		 *	Do not advance past a "/" at the end of the
		 *	sequence; this "/" is to be handled by the
		 *	next call to ns_resolve_simple(). This
		 *	includes the special case of a "/" at the very
		 *	end of the input path.
		 */
		maxcanonpos = newcanonpos;
		for (;;) {
			maxinputpos = newinputpos;
			FIND_COMPONENT(newinputpos,len,atpos);
			if (len == 0) {
				break;
			}
			if (! IS_DOTDOT(newinputpos,len)) {
				break;
			}
			BACKSPACE_COMPONENT(newcanonpos,canonpath);
			newinputpos += len;
		}
		*newcanonpos = '\0';
		newinputpos = maxinputpos;
		curinputpos = newinputpos;
		DEBUG1((resolve_debug>1),(0,
	"ns_resolve_complex: new canonical prefix after backspacing: \"%s\"\n",
			canonpath));

		/* 
		 * grm's changes here:
		 *
		 * If the cache inputpos pointed before one of the ..'s
		 * then we've got a wierd path, and we need to set the
		 * cache prefix to be the last thing we remember at this
		 * point.
		 */
		if (cache_inputpos && *cache_inputpos) {
			DEBUG2(resolve_debug,
			       (0, "nsC: RC7 curinputpos (0x%x) '%s'\n",
				curinputpos, curinputpos));

			if (*cache_inputpos < curinputpos) {
				DEBUG2(resolve_debug,
				       (0, "nsC: save_cache_prefix #1: curobj (0x%x), curtype (%d).\n",
					curobj, curtype));
				SAVE_CACHE_PREFIX(inputpath, remember_inputpos,
						  canonpath, remember_canonpos,
						  curobj, curtype);
			}

			remember_inputpos = curinputpos;
			remember_canonpos = newcanonpos;
			DEBUG2(resolve_debug,
			       (0, "nsC: RC7 ri (0x%x) '%s'\n",
				remember_inputpos, remember_inputpos));

			DEBUG2(resolve_debug,
			       (0, "nsC: RC7 rc (0x%x) '%s'\n",
				remember_canonpos, remember_canonpos));
		}

		/*
		 *	Find the object corresponding to the end of the
		 *	sequence of ".." components in inputpath.
		 *	A path for that object is now stored in canonpath,
		 *	and it does not contain any ".." components.
		 */

		if (newcanonpos == maxcanonpos) {
			/*
			 * We have not moved back in the canonpos
			 * (special case of terminating "/" in path).
			 */
			mach_object_dereference(curobj);
			curobj = newobj;
			curtype = newtype;
			newobj = NULL;
			curcanonpos = newcanonpos;
			DEBUG1((resolve_debug>1),(0,
	"ns_resolve_complex: no backspacing - use last object found\n"));
			continue;
		} else {
			mach_object_dereference(newobj);
			newobj = NULL;
		}

		if (newcanonpos != curcanonpos) {
			/*
			 * Unless we have backspaced exactly
			 * up to the last known prefix, we must re-evaluate
			 * a new prefix.
			 */
			ns_path_t	newpath;
			char		*pfxpos1;
			char		*pfxpos2;

			if (newcanonpos < curcanonpos) {
				/*
				 * We have backspaced past the last known.
				 * prefix. Get rid of it.
				 */
				mach_object_dereference(curobj);
				curobj = NULL;
				curtype = NST_INVALID;
				curcanonpos = canonpath;
			}

			pfxpos1 = newpath +
				(unsigned int) (curcanonpos - canonpath);
			bcopy(canonpath,newpath,newcanonpos - canonpath + 1);
			DEBUG1((resolve_debug>1),(0,
	"ns_resolve_complex: looking for new prefix after processing \"..\" sequence: \"%s\"\n",
					pfxpos1));
			DEBUG2(resolve_debug,
			       (0, "nrc: (2) calling ns_simple.\n"));
			ret = ns_resolve_simple(&curobj,&curtype,
						&pfxpos1,&curcanonpos,
						(char **)0,
						&newobj,
						&pfxpos2,&newcanonpos,
						&newtype,
						newpath,canonpath,
						NSF_FOLLOW_ALL,
						NSR_LOOKUP,0);
			DEBUG2(resolve_debug,
			       (0, "nrc: (2) exited ns_simple.\n"));
			if (ret != ERR_SUCCESS) {
				ABORT(ret);
			}

			mach_object_dereference(curobj);
			curobj = newobj;
			curtype = newtype;
			newobj = NULL;
			curcanonpos = newcanonpos;

			/* 
			 * grm's changes here:
			 *
			 * If we're at the right input spot, cache the
			 * new prefix returned from ns_S.
			 */
			if (cache_inputpos && *cache_inputpos) {

				DEBUG2(resolve_debug,
				       (0, "nsC: RC8 curinputpos (0x%x) '%s'\n",
					curinputpos, curinputpos));

				if (*cache_inputpos == curinputpos) {
					DEBUG2(resolve_debug,
					       (0, "nsC: save_cache_prefix #2: curobj (0x%x), curtype (%d).\n",
						curobj, curtype));
					SAVE_CACHE_PREFIX(inputpath,
							  curinputpos,
							  canonpath,
							  curcanonpos,
							  curobj, curtype);
				}
			}
		}
	}

	/*
	 * Setup all the return arguments.
	 */
	*pfx_obj = curobj;
	*pfx_type = curtype;
	*pfx_inputpos = curinputpos;
	*pfx_canonpos = curcanonpos;
	*out_obj = newobj;
	*out_canonpos = newcanonpos;
	*out_type = newtype;

	DEBUG1((resolve_debug>1),(0,
		"ns_resolve_complex: returning (rc=%d),canonpath=\'%s\"\n",
		ret,canonpath));

	DEBUG2(resolve_debug,(0,
	       "ns_resolve_COMPLEX: END (#%d) with:\n", our_debug_count));
	DEBUG2(resolve_debug,
	       (0, "*pfx_obj (0x%x), *pfx_type (0x%x)\n",
		*pfx_obj, *pfx_type));
	DEBUG2(resolve_debug,
	       (0, "*pfx_inputpos (0x%x) '%s'\n", 
		*pfx_inputpos, *pfx_inputpos));
	DEBUG2(resolve_debug,
	       (0, "*pfx_canonpos (0x%x) '%s'\n",
	       *pfx_canonpos, *pfx_canonpos));
	DEBUG2(resolve_debug,
	       (0, "*out_obj (0x%x), *out_type (0x%x)\n",
		*out_obj, *out_type));
	DEBUG2(resolve_debug,
	       (0, "*out_canonpos (0x%x) '%s'\n",
	       *out_canonpos, *out_canonpos));
	DEBUG2(resolve_debug,
	       (0, "inputpath (0x%x) '%s'\n",
		inputpath, inputpath));
	DEBUG2(resolve_debug,
	       (0, "canonpath (0x%x) '%s'\n",
		canonpath, canonpath));
	DEBUG2(resolve_debug,
	       (0, "cache_inputpos (0x%x) '%s'\n",
		(cache_inputpos ? *cache_inputpos : 0), 
		(cache_inputpos && *cache_inputpos ? *cache_inputpos :
		 "ZERO")));
	DEBUG2(resolve_debug,
	       (0, "mode (0x%x), access (0x%x), forwardcount (0x%x) \n\n",
		mode, access, forwardcount));

	return(ret);

#undef	ABORT
}


/*
 *	ns_resolve_fully: completely resolve any path name, starting
 *	from scratch, and taking care of all caching operations
 *	internally. Return the desired object.
 */
mach_error_t 
std_name::ns_resolve_fully(char* path, ns_mode_t mode, ns_access_t access,
			    usItem** newobj, ns_type_t* newtype,
			    char* canonpath)
{
	int			retrycount = 0;
	int			forwardcount = 0;
	mach_error_t		ret;
	ns_path_t		inputpath;
	char			*canonpath_ptr;
	ns_path_t		canonpath_buf;
	usItem*			curobj = NULL;
	ns_type_t		curtype = NST_INVALID;
	char			*curinputpos;
	char			*curcanonpos;
	char			*newcanonpos;
	usItem*			cacheobj = NULL;
	usItem*			pfxobj = NULL;
	ns_type_t		pfxtype = NST_INVALID;
	unsigned int		pfxlen;
	char			*cache_inputpos;

retry:

	DEBUG1((resolve_debug>1),(0,
		"ns_resolve_fully: path=\"%s\", mode=0x%x, access=0x%x\n",
		path,mode,access));

	if (canonpath == NULL) {
		canonpath_ptr = canonpath_buf;
	} else {
		canonpath_ptr = canonpath;
	}

	/*
	 * Eliminate any bad formatting in the path.
	 */
	CLEAN_PATH(path,inputpath,sizeof(ns_path_t),sizeof(ns_name_t),ret);
	if (ret != ERR_SUCCESS)
		return(ret);

	do {
		/*
		 * Find an initial prefix.
		 */
		ret = ns_find_prefix(inputpath,&pfxobj,&pfxtype,
						&pfxlen,canonpath_ptr);
		if (ret != ERR_SUCCESS) {
			break;
		}
		mach_object_reference(pfxobj);
		curobj = pfxobj;
		curtype = pfxtype;
		curinputpos = inputpath + pfxlen;
		curcanonpos = canonpath_ptr + strlen(canonpath_ptr);
		*newobj = NULL;
		*newtype = NST_INVALID;

		/*
		 * grm's changes here:
		 *
		 * figure out where the last good place to look for a
		 * prefix cache object is:
		 */
		cache_inputpos = inputpath + (strlen(inputpath));

		if (cache_inputpos > curinputpos)
			cache_inputpos--;
		while((cache_inputpos > curinputpos) && 
		      (*cache_inputpos == '/'))
			cache_inputpos--;
		while((cache_inputpos > curinputpos) && 
		      (*cache_inputpos != '/'))
			cache_inputpos--;

		if (*cache_inputpos == '/')
			cache_inputpos++;

		if ((strlen(inputpath) < 2) || 
		    (cache_inputpos <= inputpath)) {
			cache_inputpos = 0;
			DEBUG2(resolve_debug,
			       (0, "nsF: no starting cache_inputpos.\n"));
		}else{
			DEBUG2(resolve_debug,
			       (0, "nsF: cache_inputpos = (0x%x) '%s'\n",
				cache_inputpos, cache_inputpos));
		}

		/*
		 * Resolve the whole path name.
		 */
		DEBUG2(resolve_debug,(0,
		      "nrfully (1): calling nr_complex.\n"));
		ret = ns_resolve_complex(&curobj, &curtype,
					 &curinputpos, &curcanonpos,
					 &cache_inputpos,
					 newobj, &newcanonpos, newtype,
					 inputpath, canonpath_ptr,
					 mode, access, forwardcount);

		DEBUG2(resolve_debug,(0,
		      "nrfully (1): exiting nr_complex.\n"));


	} while ((ret == NS_INVALID_HANDLE) && (++retrycount < MAX_RETRIES));

	if (retrycount >= MAX_RETRIES) {
		ret = NS_INFINITE_RETRY;
	}

	/*
	 * My changes have already put the correct prefix into the table.
	 */
	if (cache_inputpos) 
		DEBUG2(resolve_debug,
		       (0, "nsF: no cache prefix saved in resolution. :-(\n"));

	mach_object_dereference(curobj);
	mach_object_dereference(pfxobj);

	DEBUG1(resolve_debug,(0,
		"ns_resolve_fully: path=\"%s\", mode=0x%x, access=0x%x -> %s, canonpath=\"%s\"\n",
		path,mode,access,mach_error_string(ret),canonpath_ptr));

	/*
	 * Special hack for failures at transparent symlinks:
	 * rather than let the user see the failure, return the
	 * transparent symlink itself so that the user will have
	 * something to stat(), etc.
	 */
	if ((ret == NS_TFORWARD_FAILURE) && (mode & NSF_TFOLLOW)) {
		DEBUG0(resolve_debug,(0,"ns_resolve_fully: TFOLLOW problems (path=\"%s\") -- retrying without NSF_TFOLLOW\n",path));
		mode &= ~NSF_TFOLLOW;
		goto retry;
	}
	return(ret);
}

mach_error_t
std_name::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_duplicate(ns_access_t access, usItem** newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_set_times(time_value_t atime, time_value_t mtime)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_get_protection(ns_prot_t prot, int* protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_set_protection(ns_prot_t prot, int protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_get_privileged_id(int* id)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
std_name::ns_get_manager(ns_access_t access, usItem **newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

