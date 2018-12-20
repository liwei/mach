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
 * $Log:	vn_pager.cc,v $
 * Revision 2.4  94/07/07  17:25:58  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  93/01/20  17:38:43  jms
 * 	Add "pager_may_cache" logic.
 * 	Add "io_pagein" dealloc logic.
 * 	[93/01/18  17:12:02  jms]
 * 
 * Revision 2.2  92/07/05  23:32:08  dpj
 * 	First working version.
 * 	[92/06/24  17:27:30  dpj]
 * 
 * Revision 2.8  91/07/01  14:12:05  jms
 * 	Removed max_count arg to io_pagein().
 * 	[91/06/25  12:02:30  roy]
 * 
 * Revision 2.7  90/12/21  13:53:36  jms
 * 	Added io_pagein(), io_pageout().
 * 	[90/12/15  15:09:37  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:19:33  jms]
 * 
 * Revision 2.6  89/10/30  16:32:59  dpj
 * 	Added a setup method. Keep track of credentials.
 * 	[89/10/27  17:42:32  dpj]
 * 
 * Revision 2.5  89/07/09  14:19:13  dpj
 * 	Added include of base.h.
 * 	[89/07/08  12:56:10  dpj]
 * 
 * Revision 2.4  89/06/30  18:34:43  dpj
 * 	Added credentials for all fss_* calls, obtained from fio_construct().
 * 	[89/06/21  22:22:51  dpj]
 * 
 * Revision 2.3  89/05/17  16:40:58  dorr
 * 	include file cataclysm
 * 
 * Revision 2.2  89/03/17  12:42:39  sanzi
 * 	replace fix lost in vice crash.
 * 	[89/03/07  11:21:12  dorr]
 * 	
 * 	Fix one character bug.
 * 	[89/03/03  11:53:45  dorr]
 * 	
 * 	Incremenet Local(fs_size) when writing past
 * 	known end of fsid.
 * 	[89/03/02  13:55:03  sanzi]
 * 	
 * 	Set size to newsize rather than old size in 
 * 	io_set_size.
 * 	[89/03/01  16:27:30  sanzi]
 * 	
 * 	Implement io_set_size.
 * 	[89/03/01  10:44:01  sanzi]
 * 	
 * 	See the lines surrounding :
 * 	 "#ifdef	no_agent_present_so_how_can_this_work"
 * 	[89/02/24  21:34:30  sanzi]
 * 	
 * 	Check onto branch.
 * 	[89/02/24  17:51:27  sanzi]
 * 
 */

#include	<vn_pager_ifc.h>

#include	<vn_agency_ifc.h>
#include	<agent_ifc.h>
#include	<std_cred_ifc.h>
#include	<fs_cred_ifc.h>


extern "C" {
#include	<io_types.h>
#include	<fs_types.h>
}


/* Do we want the kernel to cache pages when references go away? */
int		vn_pager_may_cache = TRUE;

/*
 * Debugging control.
 */
int	vn_pager_debug = 1;


vn_pager::vn_pager()
	:
	started(FALSE), fsid(0)
{}

vn_pager::~vn_pager()
{
	if (! started) return;

//	if (Local(fsid) != 0)
//		(void) fss_release(Local(fsid));
}


mach_error_t vn_pager::vn_pager_start(fs_id_t fsid)
{
	DEBUG1(vn_pager_debug,(Diag,"vn_pager::start, fsid=0x%x\n",fsid));

	mutex_init(&Local(lock));

	Local(fsid) = fsid;
//	if (Local(fsid) != 0)
//		(void) fss_release(Local(fsid));
//	(void) fss_reference(fsid);

	bzero(&Local(fs_cred_info),sizeof(struct fs_cred_data));
	Local(has_cred) = FALSE;

	started = TRUE;

	return pager_base_start(vn_pager_may_cache);
}


mach_error_t vn_pager::ns_register_agent(
	ns_access_t		access)
{
	DECLARE_FS_CRED(fs_cred_ptr);

	DEBUG1(vn_pager_debug,(Diag,"ns_register_agent, access=0x%x\n",access));

	mutex_lock(&Local(lock));

	/*
	 * XXX For now, we simply store the credentials from the first
	 * agent. We should really store all the credentials for all the
	 * current and past agents, and use whichever credentials are
	 * powerful enough to do what has to be done.
	 */
	if (! Local(has_cred)) {
		SETUP_FS_CRED(fs_cred_ptr);
		Local(fs_cred_info) = *fs_cred_ptr;
		Local(has_cred) = TRUE;
	}

	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}

mach_error_t vn_pager::io_pagein(
	vm_offset_t		offset,
	vm_address_t*		data,
	vm_size_t*		count,
	boolean_t*		deallocate)
{
	mach_error_t		ret;

        DEBUG1(vn_pager_debug,(Diag,"io_pagein, offset=%d, size=%d\n",
			     offset,*count));

	*deallocate = TRUE;
	ret = fss_pagein(Local(fsid),offset,data,count,&Local(fs_cred_info));

	DEBUG1(vn_pager_debug,(Diag,"io_pagein, count=%d\n",
                             *count));

	if (ret != ERR_SUCCESS) {
		ERROR((Diag,"io_pagein: %s\n", mach_error_string(ret)));
	}

	return(ret);
}


mach_error_t vn_pager::io_pageout(
	vm_offset_t		offset,
	vm_address_t		data,
	vm_size_t*		count)	/* inout */
{
	mach_error_t		ret;
	
	DEBUG1(vn_pager_debug,(Diag,"io_pageout, offset=%d, size=%d\n",
			     offset,*count));

	ret = fss_pageout(Local(fsid),offset,data,count,&Local(fs_cred_info));

	DEBUG1(vn_pager_debug,(Diag,"io_pageout, count=%d\n",
                             *count));

	if (ret != ERR_SUCCESS) {
		ERROR((Diag,"io_pageout: %s\n", mach_error_string(ret)));
	}

	return(ret);

}


mach_error_t vn_pager::io_get_size(
	io_size_t*		size)
{
	struct fs_attr		fs_attr;
	mach_error_t		ret;

	DEBUG1(vn_pager_debug,(Diag,"io_get_size, fsid=0x%x\n",
			     Local(fsid)));

	ret = fss_getattr(Local(fsid),&fs_attr,&Local(fs_cred_info));
	if (ret == ERR_SUCCESS) {
    		UINT_TO_IO_SIZE(fs_attr.va_size,size);
		DEBUG1(vn_pager_debug,(Diag,"io_get_size, size=%d\n",
							fs_attr.va_size));
	} else {
    		UINT_TO_IO_SIZE(0,size);
		ERROR((Diag,"io_get_size.fss_getattr: %s\n",
						mach_error_string(ret)));
	}

	return(ret);
}


mach_error_t vn_pager::io_set_size(
	io_size_t		newsize)
{
	mach_error_t    	ret = ERR_SUCCESS;
	struct fs_attr  	fs_attr;

	/*
	 * XXX Possible optimization: remember the last value
	 * of the size, and do not set it again if it is
	 * unchanged.
	 *
	 * Problem: the underlying file system might change that size
	 * without our knowledge (NFS).
	 */
	fs_attr.va_type = (enum vtype)-1;
	fs_attr.va_mode = (unsigned int)-1;
	fs_attr.va_uid = -1;
	fs_attr.va_gid = -1;
	fs_attr.va_fsid = -1;
	fs_attr.va_nodeid = -1;
	fs_attr.va_nlink = -1;
	fs_attr.va_size = (unsigned int)-1;
	fs_attr.va_blocksize = -1;
	fs_attr.va_atime.tv_sec = -1;
	fs_attr.va_atime.tv_usec = -1;
	fs_attr.va_mtime.tv_sec = -1;
	fs_attr.va_mtime.tv_usec = -1;
	fs_attr.va_ctime.tv_sec = -1;
	fs_attr.va_ctime.tv_usec = -1;
	fs_attr.va_rdev = -1;
	fs_attr.va_blocks = -1;

	IO_SIZE_TO_UINT(newsize, &fs_attr.va_size);

	DEBUG1(vn_pager_debug,(Diag,"io_set_size, size=%d\n",
							fs_attr.va_size));

	ret = fss_setattr(Local(fsid), &fs_attr, &Local(fs_cred_info));

	if (ret != ERR_SUCCESS) {
		ERROR((Diag,"io_set_size.fss_setattr: %s\n",
						mach_error_string(ret)));
	}

	return(ret);
}


