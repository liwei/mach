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
 * $Log:	vn_file.cc,v $
 * Revision 2.4  94/07/27  14:40:32  mrt
 * 	Added code to the vn_file constructor to initialize the
 * 	mapping object with the vn_file's agency.
 * 	[94/07/27  14:13:15  grm]
 * 
 * Revision 2.3  94/07/07  17:25:51  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:31:57  dpj
 * 	First working version.
 * 	[92/06/24  17:26:37  dpj]
 * 
 * Revision 2.4  91/07/01  14:13:00  jms
 * 	Added act_obj_clean method; eliminated act_obj_clean_prepare
 * 	and act_obj_clean_commit methods.
 * 	[91/06/11  11:43:27  roy]
 * 
 * 	Convert to using vn_reference.
 * 	[91/06/07  13:16:46  roy]
 * 
 * 	Significant changes associated with new rule that
 * 	only the process of cleaning may change the state
 * 	of a file whose current state is AOT_STATE_FILE_MUST_CLEAN.
 * 	[91/06/06  17:10:01  roy]
 * 
 * 	Added permanent() and temporary() methods.  Files
 * 	that have been marked as temporary are not cleaned.
 * 	[91/06/05  14:03:37  roy]
 * 
 * Revision 2.3  91/05/05  19:28:14  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:59:45  dpj]
 * 
 * 	Replace invoke_super() with invoke_super_with_base().
 * 	[91/04/28  10:25:04  dpj]
 * 
 * Revision 2.2  90/12/21  13:54:34  jms
 * 	Destroy the io_mgr when file is being deactivated.
 * 	[90/12/18  12:06:03  roy]
 * 
 * 	Initial revision.
 * 	(Equivalent functionality to old fs_file object.)
 * 	[90/12/15  15:11:45  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:21:12  jms]
 * 
 * Revision 2.1  90/12/15  15:10:43  roy
 * Created.
 * 
 * 
 */

#include	<vn_file_ifc.h>

#include	<vn_mgr_ifc.h>

extern "C" {
#include 	<sys/file.h>
#include	<ns_types.h>
#include	<io_types.h>
#include	<fs_types.h>
}

/*
 * Debugging control.
 */
int	vn_file_debug = 1;


DEFINE_CLASS_MI(vn_file)
DEFINE_CASTDOWN2(vn_file,usint_mf,vn_agency);


void vn_file::init_class(usClass* class_obj)
{
	usint_mf::init_class(class_obj);
	vn_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(vn_file);
	SETUP_METHOD_WITH_ARGS(vn_file,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(vn_file,io_read_seq);
	SETUP_METHOD_WITH_ARGS(vn_file,io_write_seq);
	SETUP_METHOD_WITH_ARGS(vn_file,io_map);
	SETUP_MF_MGR_PROP(vn_file);
	END_SETUP_METHOD_WITH_ARGS;
}


char* vn_file::remote_class_name() const
{
	return "usint_mf_proxy";
}


vn_file::vn_file()
{
}


vn_file::vn_file(
	fs_id_t			fsid,
	fs_access*		_fs_access,
	ns_mgr_id_t		mgr_id,
	access_table*		_access_table,
	vn_mgr*			_vn_mgr)
:
	vn_agency(fsid,_fs_access,mgr_id,_access_table,_vn_mgr),
	mapping_obj(this)
{
	Local(num_writers) = 0;
	Local(io_active) = FALSE;
	Local(temporary) = FALSE;
	Local(writable)  = FALSE;
}


vn_file::~vn_file()
{
}


/*
 * Take a reference to the file (simply to prevent deactivation).
 * Called with the aot_lock held.
 */
void vn_file::vn_reference()
{
	mutex_lock(&vn_lock);
	vn_refcount += 1;
	mutex_unlock(&vn_lock);
}


/*
 * Release a reference to the file (obtained via vn_reference).
 * Not called with any locks held.
 */
void vn_file::vn_dereference()
{
	/*
	 * Simply pretend as if we're unregistering an agent that
	 * has no special access rights.
	 */
	(void) ns_unregister_agent(NSR_NONE);
}


/*
 * Called by the aot with the aot_lock on the object held.
 */
mach_error_t vn_file::vn_destroy()
{
	mach_error_t		ret;

	mutex_lock(&vn_lock);

	if (vn_refcount == 0) 
		if (Local(io_active)) {
			ret = mapping_obj.io_deactivate(Local(temporary));
			if (ret == ERR_SUCCESS) io_active = FALSE;
		} else
			ret = ERR_SUCCESS;
	else
		ret = US_OBJECT_BUSY;

	mutex_unlock(&vn_lock);
		
	/*
	 * If we're returning ERR_SUCCESS then this file object
	 * can be freed.  (Since the refcount is zero and the aot_lock
	 * is held, we are guaranteed that there are no referencers.)
	 */
	return(ret);
}


/*
 * Called by the aot WITHOUT the aot_lock on the object held.
 */
mach_error_t vn_file::vn_clean()
{
	mach_error_t		ret;
	void*			tag;

	if ((ret = vn_mgr_obj->aot_lock((aot_key_t) fsid, &tag))
	    != ERR_SUCCESS)
		CRITICAL((0,"vn_file::vn_clean"));
	mutex_lock(&vn_lock);

	if (Local(num_writers) == 0) 
		Local(writable) = FALSE;  /* file is not writable */

	/*
	 * Clean the file, but only if it's not temporary.
	 */
	if (Local(temporary) == FALSE) {
		/*
		 * Release locks while cleaning.
		 * We are guaranteed that the file will not change states
		 * (and possibly be deactivated) because only this routine
		 * can change the state of a cleanable file.
		 */
		mutex_unlock(&vn_lock);
		vn_mgr_obj->aot_unlock(tag);
		
		(void) mapping_obj.io_clean();

		/* reacquire locks */
		if ((ret = vn_mgr_obj->aot_lock((aot_key_t) fsid, &tag))
		    != ERR_SUCCESS)
			CRITICAL((0,"vn_file::vn_clean"));
		mutex_lock(&vn_lock);
	}

	/*
	 * After cleaning is the only time a cleanable file's state
	 * can be changed to something other than MUST_CLEAN.  There
	 * are three cases when we want to do this:
	 * - the file has no active referencers
	 * - the file has been marked temporary, or
	 * - the file had no active writers when we began to clean, and we 
	 *   are guaranteed that no writes have taken place since then.
	 */
	if (vn_refcount == 0) {
		/*
		 * If no referencers then change the state to inactive.	
		 * We can deactivate here as an optimization, but this
		 * may cause us to lose our internal window sooner than
		 * necessary.
		 */
		if (Local(io_active) != TRUE) 
			CRITICAL((0,"vn_file::vn_clean"));
//		ret = mapping_obj.io_deactivate(Local(temporary));
//		if (ret == ERR_SUCCESS) io_active = FALSE;
		vn_mgr_obj->aot_set_state(tag, AOT_STATE_INACTIVE);

	} else if (Local(writable) == FALSE || Local(temporary) == TRUE) 
		vn_mgr_obj->aot_set_state(tag, AOT_STATE_FILE_ACTIVE);

	mutex_unlock(&vn_lock);
	vn_mgr_obj->aot_unlock(tag);

	return(ERR_SUCCESS);
}


/*
 * Register a new agent (with the supplied access rights) with the file.
 */
mach_error_t vn_file::ns_register_agent(
	ns_access_t		access)
{
	mach_error_t		ret;
	void*			tag;

	/*
	 * Must synchronize with the aot who initiates destroying and
	 * cleaning.  => get the aot_lock first before the vn_file_lock,
	 * and then diddle the refcount.
	 */
	if ((ret = vn_mgr_obj->aot_lock((aot_key_t) fsid, &tag))
	    != ERR_SUCCESS)
		CRITICAL((0,"vn::file_ns_register_agent"));

    	mutex_lock(&vn_lock);

	/*
	 * If we will need to perform I/O make sure we have an I/O manager.
	 */
	if (access & (NSR_READ | NSR_WRITE | NSR_EXECUTE)) {
		if (! io_active) {
			/*
			 * Create an I/O manager object.
			 */
			ret = backing_obj.vn_pager_start(fsid);
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&vn_lock);
				vn_mgr_obj->aot_unlock(tag);
				return(ret);
			}
			ret = mapping_obj.mf_mgr_start(&backing_obj);
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&vn_lock);
				vn_mgr_obj->aot_unlock(tag);
				return(ret);
			}
			io_active = TRUE;
		}

		/* 
		 * Notify the I/O manager of the new agent.
		 */
		(void) backing_obj.ns_register_agent(access);
	}

	vn_refcount += 1;  			/* bump reference count */

	if (access & NSR_WRITE) {
		Local(num_writers) += 1;  	/* keep track of writers */
		Local(writable) = TRUE;		/* only reset during cleaning */
	}
	
	/*
	 * All active files should be in the ACTIVE state unless they are
	 * writable in which case they should be in the MUST_CLEAN state.
	 * Remember that only the process of cleaning is allowed to change
	 * the state of a file that's currently in the MUST_CLEAN state.
	 */
	if (Local(io_active) && Local(num_writers) > 0) 
		vn_mgr_obj->aot_set_state(tag, AOT_STATE_FILE_MUST_CLEAN);
	else if (vn_mgr_obj->aot_get_state(tag) != AOT_STATE_FILE_MUST_CLEAN)
		vn_mgr_obj->aot_set_state(tag, AOT_STATE_FILE_ACTIVE);

	mutex_unlock(&vn_lock);  
	vn_mgr_obj->aot_unlock(tag);

	return(ERR_SUCCESS);
}


/*
 * Unregister an agent.
 *
 * Note:  vn_file::vn_dereference calls this routine, and depends
 * on the fact that nothing special happens (other than checking for
 * write access, decrementing the reference count, and possibly changing
 * the state).
 */
mach_error_t vn_file::ns_unregister_agent(
	ns_access_t		access)
{
	mach_error_t		ret;
	void			*tag;

	/*
	 * Must synchronize with the aot who initiates destroying and
	 * cleaning, and handles lookup requests from directory objects.  
	 * => get the aot_lock before the vn_file_lock, and then diddle
	 * the refcount.
	 */
	if ((ret = vn_mgr_obj->aot_lock((aot_key_t) fsid, &tag))
	    != ERR_SUCCESS)
		CRITICAL((0,"vn_file_ns_unregister_agent"));
	mutex_lock(&vn_lock);

	if (access & NSR_WRITE) 
		Local(num_writers) -= 1;  	/* keep track of writers */
	
	/*
	 * Are we attempting to release the last reference?
	 */
	vn_refcount -= 1;
	if (vn_refcount == 0) {
		/*
		 * If the io_mgr is active, attempt to deactivate it.  If the
		 * file is marked as temporary, then tell it to destroy
		 * itself.  Note that calling io_mgr_deactivate here is
		 * purely an optimization.  All this work is deferred until 
		 * clean time if the file is in the MUST_CLEAN state.
		 */
		if (Local(io_active)) {
			if (vn_mgr_obj->aot_get_state(tag) !=
						AOT_STATE_FILE_MUST_CLEAN) {
//				ret = mapping_obj.io_deactivate(
//							Local(temporary));
//				if (ret == ERR_SUCCESS) io_active = FALSE;
				vn_mgr_obj->aot_set_state(tag,
							AOT_STATE_INACTIVE);
			}
		} else {
			/*
			 * Releasing the last reference for which an io_mgr
			 * is not active implies that the file is not in 
			 * the MUST_CLEAN state.
			 */
			if (vn_mgr_obj->aot_get_state(tag) ==
						AOT_STATE_FILE_MUST_CLEAN)
				CRITICAL((0,"vn_file_ns_unregister_agent"));
			vn_mgr_obj->aot_set_state(tag, AOT_STATE_INACTIVE);
		}
	}
	mutex_unlock(&vn_lock);
	vn_mgr_obj->aot_unlock(tag);
	
	return(ERR_SUCCESS);
}


mach_error_t vn_file::ns_get_attributes(
	ns_attr_t		attr,		/* out */
	int*			attrlen)	/* out */
{
	mach_error_t    	ret;

	ret = vn_agency::ns_get_attributes(attr,attrlen);

	if (ret != ERR_SUCCESS) {
		return (ret);
	}

	mutex_lock(&vn_lock);

	if (Local(io_active)) {
		ret = mapping_obj.io_get_size(&(attr->size));
	}

	mutex_unlock(&vn_lock);

	return(ret);
}


/*
 * The file is now "temporary", implying that it should not be cleaned.  
 * Caller must have a reference to the object.
 */
void vn_file::vn_mark_temporary()
{
	mach_error_t		ret;

	mutex_lock(&vn_lock);
	/*
	 * Record the fact that it's temporary.  This will cause a
	 * file in the MUST_CLEAN state to be changed to a new state the 
	 * next time it is cleaned.  Otherwise, there's no effect on
	 * its state.
	 */
	Local(temporary) = TRUE;

	mutex_unlock(&vn_lock);
}


/*
 * The file is "permanent".
 * Caller must have a reference to the object.
 */
void vn_file::vn_mark_permanent()
{
	mach_error_t		ret;
	void*			tag;

	if ((ret = vn_mgr_obj->aot_lock((aot_key_t) fsid, &tag))
	    != ERR_SUCCESS)
		CRITICAL((0,"vn_file::vn_mark_permanent"));
	mutex_lock(&vn_lock);

	/*
	 * Mark the file as temporary.  If there are writers to the file
	 * then put it in the MUST_CLEAN state.  Otherwise, there's no
	 * effect on its state.
	 */
	Local(temporary) = FALSE;

	if (Local(num_writers) > 0) 
		vn_mgr_obj->aot_set_state(tag, AOT_STATE_FILE_MUST_CLEAN);

	mutex_unlock(&vn_lock);
	vn_mgr_obj->aot_unlock(tag);
}


