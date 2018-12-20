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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/tmp_prop.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Base agency for objects in the volatile name space,
 *	that disappear from that name space when not in use.
 *
 * HISTORY
 * $Log:	tmp_prop.cc,v $
 * Revision 2.4  94/07/07  17:24:56  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  94/01/11  17:50:17  jms
 * 	Handle the possibility of a "Null" tmporary object in tmp_shutdown_internal
 * 	[94/01/09  19:42:25  jms]
 * 
 * Revision 2.2  92/07/05  23:29:34  dpj
 * 	Converted for new C++-based package.
 * 	[92/07/05  18:57:48  dpj]
 * 
 * 	Mechinism built from a previous revision on of "tmp_agency" to supply a
 * 	way to make an agency temporary.  This means that it will go away iff
 * 	there are no more stronglinks or agents left for the agency and the
 * 	the "last_chance" checks succeed.  These "last_chance" checks are controlled
 * 	by any given agency which wishes to be a tmp_prop. (See tmp_dir/tmp_agency)
 * 	[92/06/24  16:31:29  jms]
 * 
 */

#ifndef lint
char * tmp_prop_rcsid = "$Header: tmp_prop.cc,v 2.4 94/07/07 17:24:56 mrt Exp $";
#endif	lint


#include	<tmp_prop_ifc.h>
#include	<dir_ifc.h>

/*
 * Methods.
 */

tmp_prop::tmp_prop()
	:
	active(TRUE), agent_count(0), stronglink_count(0),
	tmplink_count(0), temporary_obj(0), parent_dir(0), parent_tag(0)
{
	mutex_init(&Local(lock));
}

tmp_prop::~tmp_prop()
{
	mach_object_dereference(Local(parent_dir));
}


/*
 * This routine is called internally when both the agent_count
 * and the stronglink_count are found to be zero.
 *
 * It is responsible for getting rid of all the tmplinks.
 */
mach_error_t tmp_prop::tmp_shutdown_internal(void)
{
	/* MUST BE CALLED WITH THE OBJECT ALREADY LOCKED */
	mach_error_t	ret;

	if (! Local(active)) return(ERR_SUCCESS);
	if (! Local(temporary_obj)) {
		Local(active) = NULL;
		return(ERR_SUCCESS);  /* XXX should never happen.  but did */
	}

	if (ERR_SUCCESS != (ret = temporary_obj->ns_tmp_last_chance())) {
		return(ret);
        }

	Local(active) = FALSE;

	(void) temporary_obj->ns_tmp_cleanup_for_shutdown();

	if (Local(tmplink_count) > 0) {
		Local(tmplink_count)--;
		mutex_unlock(&Local(lock));
		(void) parent_dir->ns_remove_tmplink(parent_tag);
		mach_object_dereference(Local(parent_dir));
		Local(parent_dir) = NULL;
		Local(parent_tag) = 0;
		mutex_lock(&Local(lock));
	}
	mach_object_dereference(Local(temporary_obj));

	return(ERR_SUCCESS);
}

/*
 * Called from the parent the establish a link to this object, and
 * indicate how ns_remove_tmplink() should be called to remove the
 * object from its parent.
 *
 * The parent calls this routine in lieu of getting a MachObjects
 * reference to this object.
 */
mach_error_t tmp_prop::ns_register_tmplink(
	vol_agency *temporary,
	dir *parent,
	int tag)
{
	mutex_lock(&Local(lock));

	if (!Local(active)) {
		mutex_unlock(&Local(lock));
		return(US_OBJECT_DEAD);
	}

	if (Local(parent_dir) != NULL) {
		/*
		 * We do not allow more than one tmplink to the object
		 * (for now).
		 */
		mutex_unlock(&Local(lock));
		return(NS_INVALID_LINK_COUNT);
	}

	Local(tmplink_count)++;
	Local(temporary_obj) = temporary;
	mach_object_reference(temporary);

	Local(parent_dir) = parent;
	mach_object_reference(parent);
	Local(parent_tag) = tag;

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


/*
 * Called from the parent to undo the effect of a previous call
 * to ns_register_tmplink(). After this routine returns ERR_SUCCESS,
 * ns_remove_tmplink() may no longer be called on the parent.
 */
mach_error_t tmp_prop::ns_unregister_tmplink(int tag)
{
	mach_error_t	ret;

	mutex_lock(&Local(lock));

	if (!Local(active)) {
		mutex_unlock(&Local(lock));
		return(US_OBJECT_DEAD);
	}

	if (Local(parent_dir) == NULL) {
		/*
		 * Must be a tmplink.
		 * We do not allow more than one tmplink to the object
		 * (for now).
		 */
		mutex_unlock(&Local(lock));
		return(NS_INVALID_LINK_COUNT);
	}

	if ((0 == Local(stronglink_count)) && (1 == Local(tmplink_count))) {
		if (ERR_SUCCESS != (ret = temporary_obj->ns_tmp_last_link())) {
			mutex_unlock(&Local(lock));
			return(ret);
		}
        }

	Local(tmplink_count)--;
	mach_object_dereference(Local(parent_dir));
	Local(parent_dir) = NULL;
	Local(parent_tag) = 0;
	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


/*
 * Called from the parent to check that a link is still valid before
 * completing a lookup/resolve operation and letting a new "client"
 * gain access to this object.
 *
 * When successful this routines generates a new MachObjects reference,
 * to be given to the new client.
 */
mach_error_t tmp_prop::ns_reference_tmplink(void)
{
	mach_error_t	ret;

	mutex_lock(&Local(lock));

	/*
	 * Check that maybe we had no references from the start.
	 * (i.e. we never went through a transition from non-zero to zero).
	 */
	if ((Local(agent_count) == 0) && (Local(stronglink_count) == 0)) {
		(void)tmp_shutdown_internal();
	}

	if (Local(active)) {
		mach_object_reference(Local(temporary_obj));
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	} else {
		mutex_unlock(&Local(lock));
		return(US_OBJECT_DEAD);
	}
}


mach_error_t tmp_prop::ns_register_stronglink(void)
{
	mutex_lock(&Local(lock));
	Local(stronglink_count)++;
	mutex_unlock(&Local(lock));

	mach_object_reference(Local(temporary_obj));

	return(ERR_SUCCESS);
}


mach_error_t tmp_prop::ns_unregister_stronglink(void)
{
	mach_error_t	ret;

	mutex_lock(&Local(lock));

	if ((1 == Local(stronglink_count)) && (0 == Local(tmplink_count))) {
		if (ERR_SUCCESS != (ret = temporary_obj->ns_tmp_last_link())) {
			mutex_unlock(&Local(lock));
			return(ret);
		}
        }

	Local(stronglink_count)--;

	if ((Local(agent_count) == 0) && (Local(stronglink_count) == 0)) {
		(void)tmp_shutdown_internal();
	}

	mutex_unlock(&Local(lock));

	mach_object_dereference(Local(temporary_obj));

	return(ERR_SUCCESS);
}


mach_error_t tmp_prop::ns_register_agent(ns_access_t access)
{
	mach_error_t		ret;

	mutex_lock(&Local(lock));
	Local(agent_count)++;
	mutex_unlock(&Local(lock));

//	ret = invoke_super_with_base(Super,Base,
//				mach_method_id(ns_register_agent),access);

	return(ERR_SUCCESS);
}


mach_error_t tmp_prop::ns_unregister_agent(ns_access_t access)
{
	mach_error_t		ret;

//	ret = invoke_super_with_base(Super,Base,
//				mach_method_id(ns_unregister_agent),access);

	mutex_lock(&Local(lock));

#if USE_LAST_AGENT
	if (ERR_SUCCESS != (ret = temporary_obj->ns_tmp_last_agent())) {
		mutex_unlock(&Local(lock));
		return(ret);
        }
#endif USE_LAST_AGENT

	Local(agent_count)--;

	if ((Local(agent_count) == 0) && (Local(stronglink_count) == 0)) {
		(void) tmp_shutdown_internal();
	}
	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


/*
 * Fill out the NS_ATTR_LINKS field of a attribute structure
 */
mach_error_t tmp_prop::ns_get_nlinks_attribute(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret = ERR_SUCCESS;

	mutex_lock(&Local(lock));
	attr->valid_fields |= NS_ATTR_NLINKS;
	attr->nlinks = Local(stronglink_count) + Local(tmplink_count);
	mutex_unlock(&Local(lock));

	return(ret);
}
