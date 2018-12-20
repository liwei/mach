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
 * $Log:	mf_mgr.cc,v $
 * Revision 2.4  94/07/27  14:40:27  mrt
 * 	Added code to the mf_mgr constructor to initialize
 * 	the mapping's agency.
 * 	[94/07/27  14:10:38  grm]
 * 
 * Revision 2.3  94/07/07  17:23:39  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:27:56  dpj
 * 	Converted for use as a "property" instead of a full base class.
 * 	[92/06/24  16:30:01  dpj]
 * 
 * Revision 1.15  92/03/05  15:06:41  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:42:07  jms]
 * 
 * Revision 1.14  91/07/01  14:12:08  jms
 * 	Merge to US40.
 * 	[91/06/12  14:28:42  roy]
 * 
 * 	No need to write size out in io_deactivate.
 * 	[91/06/06  17:01:58  roy]
 * 
 * 	Added io_set_size method.  io_clean now always writes the 
 * 	size to backing store.
 * 	[91/06/05  13:53:48  roy]
 * 
 * Revision 1.13  91/05/05  19:26:40  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:54:10  dpj]
 * 
 * 	Increased the debug level for the "io_deactivate: not idle" message. 
 * 	[91/04/28  10:12:31  dpj]
 * 
 * Revision 1.12  90/10/29  17:32:33  dpj
 * 	Fixed old bug never caught with CMUCS cpp.
 * 	[90/10/27  17:59:21  dpj]
 * 
 * 	Fixed old bug never caught with CMUCS cpp.
 * 	[90/10/21  21:28:02  dpj]
 * 
 * Revision 1.11  90/07/09  17:09:28  dorr
 * 	raise a bunch of debug output levels.
 * 	[90/03/01  14:59:55  dorr]
 * 	No Further Changes
 * 	[90/07/06  17:40:09  jms]
 * 
 * Revision 1.10  89/10/30  16:34:04  dpj
 * 	Reorganization of MF system.
 * 	Fixed a deadlock in io_clean().
 * 	[89/10/27  19:00:32  dpj]
 * 
 * Revision 1.9  89/05/24  10:42:28  dorr
 * 	minor reformatting.
 * 
 * Revision 1.8  89/05/12  14:09:55  sanzi
 * 	iop_clean() objects when registering new agents.
 * 	[89/05/04  13:41:53  sanzi]
 * 
 * Revision 1.7  89/03/30  12:06:28  dpj
 * 	Updated to new syntax for invoke_super().
 * 	[89/03/26  18:51:52  dpj]
 * 
 * Revision 1.6  89/03/17  12:46:41  sanzi
 * 	Latest round of setting file size changes.
 * 	[89/03/15  17:00:21  sanzi]
 * 	
 * 	Changes for supporting mf_cat.
 * 	[89/03/11  16:23:12  sanzi]
 * 	
 * 	Intermediate check in.  Do not use.
 * 	[89/03/10  10:09:37  sanzi]
 * 	
 * 	Turn off checking for multiple readers/writers/anything.  
 * 	[89/03/08  22:25:40  sanzi]
 * 	
 * 	Clean mf_mem object in terminate.
 * 	[89/03/08  14:42:06  sanzi]
 * 	
 * 	De-lint.
 * 	[89/03/06  22:17:33  sanzi]
 * 	
 * 	Add declaration of ns_unregister_agent. 
 * 	[89/03/02  13:57:29  sanzi]
 * 	
 * 	Implement ns_unregister_agent.
 * 	Add missing unlock in iop_get_port.
 * 	Add missing locks. 
 * 	[89/03/01  10:50:47  sanzi]
 * 	
 * 	Check onto branch.
 * 	[89/02/24  17:28:24  sanzi]
 * 
 */

#include	<mf_mgr_ifc.h>
#include	<pager_base_ifc.h>
#include	<top_ifc.h>
#include	<agent_ifc.h>


int mf_mgr_debug = 1;


mf_mgr::mf_mgr()
	:
	ready(FALSE)
{
	this->my_agency = NULL;
	mutex_init(&Local(lock));
}

mf_mgr::mf_mgr(usItem* this_agency)
	:
	ready(FALSE)
{
	this->my_agency = this_agency;
	mutex_init(&Local(lock));
}


mach_error_t mf_mgr::mf_mgr_start(pager_base* _backing_obj)
{
	Local(backing_obj) = _backing_obj;
	ready = TRUE;

    	return(ERR_SUCCESS);
}

mach_error_t mf_mgr::init_upcall()
{
	mach_error_t		ret;
	io_size_t		size;
	mach_port_t		pager_port;

	ret = backing_obj->io_get_size(&size);
	if (ret != ERR_SUCCESS) {
		mach_error("mf_mgr_start.io_get_size(backing_object)",ret);
		return(ret);
	}

	ret = backing_obj->iop_get_port(&pager_port);
	if (ret != ERR_SUCCESS) {
		mach_error("mf_mgr_start.iop_get_port(backing_obj)",ret);
		return(ret);
	}

	return mf_mem_start_mgr(size,pager_port);
} 

usRemote* mf_mgr::remote_object()
{
	us_internal_error("mf_mgr::remote_object",US_INTERNAL_ERROR);
	return(new usRemote);
} 


mach_error_t mf_mgr::io_get_mf_state(
	ns_access_t*		access,			/* OUT */
	io_size_t*		size,			/* OUT */
	mach_port_t*		pager_port)		/* OUT */
{
	mach_error_t		ret;
	int			cred_data[DEFAULT_NS_CRED_LEN];
	ns_cred_t		cred = (ns_cred_t) &cred_data;
	int			credlen = DEFAULT_NS_CRED_LEN;

	/*
	 * Called from some user-side client.
	 */

	if (! ready) {
		*pager_port = MACH_PORT_NULL;
		return(US_OBJECT_NOT_STARTED);
	}

	/*
	 * Make sure that the mf_mem mgr is started, and get
	 * the data from it. May cause an upcall to init_upcall().
	 */
	ret = mf_mem::io_get_mf_state_internal(size,pager_port);
	if (ret != ERR_SUCCESS) {
		*pager_port = MACH_PORT_NULL;
		return(ret);
	}

	ret = agent::base_object()->ns_get_access(access,cred,&credlen);
	if ((ret != ERR_SUCCESS) && (ret != NS_NOT_ENOUGH_ROOM)) {
		*pager_port = MACH_PORT_NULL;
		return(ret);
	}

	ret = mf_mem::io_get_size(size);
	if (ret != ERR_SUCCESS) {
		*pager_port = MACH_PORT_NULL;
		return(ret);
	}

	ret = backing_obj->iop_get_port(pager_port);
	if (ret != ERR_SUCCESS) {
		*pager_port = MACH_PORT_NULL;
	}

	return(ERR_SUCCESS);
}


mach_error_t mf_mgr::io_clean()
{
	mach_error_t		ret;
	boolean_t		dirty;
	io_size_t		size;

	DEBUG1(mf_mgr_debug,(Diag,"io_clean called\n"));

	if (! ready) return(ERR_SUCCESS);

	mf_mgr_lock();

	/*
	 * Freeze I/O operations and get the current size.
	 */
	ret = io_clean_prepare(&dirty,&size);
	if (ret != ERR_SUCCESS) {
		mf_mgr_unlock();
		return(ret);
	}

	if (! dirty) {
		(void) io_clean_complete();
		mf_mgr_unlock();
		return(ERR_SUCCESS);
	}

	/*
	 * Force the pager to write back all dirty pages.
	 * Leave the pager frozen.
	 */
	ret = backing_obj->iop_clean(&dirty);
	if (ret != ERR_SUCCESS) {
		(void) io_clean_complete();
		mf_mgr_unlock();
		return(ret);
	}

	/*
	 * Write the size to backing store.
	 *
	 * Note that the size of the file could have been modified 
	 * by io_write and/or io_set_size (truncate).  Hence, even
	 * if no pageouts occurred during iop_clean we must account
	 * for the size changing as the result of io_set_size.  Since
	 * the size of the underlying data on disk can change without
	 * this module's knowledge (during pageout), an attempt to 
	 * optimize and avoid unnecessary size updates to disk should
	 * be done at a lower level.  Or, perhaps preferable, would be
	 * to rewrite mf_mem, mf_mgr, pager to make them more tightly
	 * coupled (and easily share true size vs. pageout size knowledge).
	 */
	ret = backing_obj->io_set_size(size);

	/*
	 * Resume pager and I/O operations.
	 */
	(void) backing_obj->iop_resume();
	(void) io_clean_complete();

	mf_mgr_unlock();

	return(ret);
}


/*
 * Attempt to deactivate.  'destroy' flag says to forcibly destroy and not
 * worry about cleaning, setting the size, etc.
 */
mach_error_t mf_mgr::io_deactivate(
	boolean_t		destroy)
{
    	mach_error_t		ret;
	boolean_t		dirty;
	io_size_t		size;

	DEBUG1(mf_mgr_debug,(Diag,"io_deactivate called\n"));

	if (! ready) return(ERR_SUCCESS);

	mf_mgr_lock();

	/*
	 * Check with the pager if it is OK to go away.
	 */
	(void) io_unmap_window();

	ret = backing_obj->iop_deactivate(destroy, &dirty);
	if (ret != ERR_SUCCESS) {
		mf_mgr_unlock();
		DEBUG1(mf_mgr_debug,(Diag,"io_deactivate: not idle\n"));
		return(ret);
	}
	
	/*
	 * If destroy flag is false (implying that the file is not
	 * temporary), and pageouts have occurred to the file since
	 * iop_clean was last called (dirty flag is true) then 
	 * something is fatally wrong because we should not be
	 * trying to deactivate such files.
	 */
	if (dirty && !destroy) 
		CRITICAL((0,"mf_mgr_io_deactivate: dirty!"));
	/*****
	if (dirty && !destroy) {
		 *
		 * Get the current size, and write it to backing store.
		 *
		ret = io_get_size(Diag,&size);
		if (ret == ERR_SUCCESS) {
			ret = io_set_size(Local(backing_obj),size);
		}
		if (ret != ERR_SUCCESS) {
			mf_mgr_unlock();		
			return(ret);
		}
	}
	*****/

	ready = FALSE;

	mf_mgr_unlock();		

	DEBUG1(mf_mgr_debug,(Diag,"io_deactivate: destroying the object"));

	return(ERR_SUCCESS);
}


mach_error_t mf_mgr::io_set_size(
	io_size_t		size)
{
    	mach_error_t		ret;
	vm_size_t		newsize;

	mf_mgr_lock();

	/* mf_mem superclass keeps track of the true size */
	ret = mf_mem::io_set_size(size);
	if (ret != ERR_SUCCESS) {
		mf_mgr_unlock();
		return(ret);
	}

	/*
	 * Flush any memory pages associated with a file that has shrunk.
	 * We round up to a page boundary to avoid flushing modified data
	 * that is still valid.
	 *
	 * It's important that flushing of pages is synchronized with
	 * writing so as to avoid flushing of new data.  Since io_set_size
	 * is invoked on the account of Unix truncate and truncate must
	 * be synchronized with Unix write, the proper time to flush is now
	 * (e.g., at clean time we are not sync. with write, resulting 
	 * in a race).
	 * 
	 * Note also that the act of setting the "true" size above must
	 * be synchronized with cleaning (which resets the dirty flag).
	 * This is accomplished via the mf_mgr_lock.
	 */
	IO_SIZE_TO_UINT(size, &newsize);
	newsize = round_page(newsize);

	ret = backing_obj->iop_flush(newsize);
	if (ret != ERR_SUCCESS) {
		mf_mgr_unlock();
		return(ret);
	}

	mf_mgr_unlock();

	return(ERR_SUCCESS);
}


