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
 * File:        proc_mgt.c
 *
 * Purpose:
 *	map all sorts of mach errors into unix errors.
 *
 * HISTORY
 * $Log:	emul_error.cc,v $
 * Revision 2.5  94/07/08  16:56:46  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  94/05/17  13:35:46  jms
 * 	Add new error message to task master.
 * 	[94/05/11  14:40:53  modh]
 * 
 * Revision 2.3  92/07/05  23:24:49  dpj
 * 	JOB_GROUP->TASK_GROUP
 * 	[92/06/24  14:04:44  jms]
 * 
 * Revision 2.2  91/11/06  11:29:35  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:26:49  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:22:38  pjg]
 * 
 * Revision 1.9  91/05/05  19:24:16  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:50:02  dpj]
 * 
 * 	US_INVALID_ACCESS corresponds to EACCES, not EROFS.
 * 	Added missing default "EFAULT" return in US_MODULE.
 * 	Added network-related codes.
 * 	[91/04/28  09:40:42  dpj]
 * 
 * Revision 1.8  90/03/21  17:18:53  jms
 * 	Comment corrections
 * 	[90/03/16  16:18:34  jms]
 * 
 * Revision 1.7  90/01/02  21:39:04  dorr
 * 	add us_error codes.
 * 
 * Revision 1.6.1.1  89/12/18  15:42:06  dorr
 * 	add us_error codes.
 * 
 * Revision 1.6  89/07/19  11:27:22  dorr
 * 	get rid of old loader error codes.
 * 
 * Revision 1.5  89/07/09  14:17:10  dpj
 * 	Fixed error translations for new unified scheme.
 * 	[89/07/08  12:34:45  dpj]
 * 
 * Revision 1.4  89/05/17  16:12:58  dorr
 * 	include file cataclysm
 * 
 * Revision 1.3  89/03/17  12:23:22  sanzi
 * 	io_invalid_recnum -> io_invalid_offset
 * 	[89/02/25  13:53:20  dorr]
 * 
 *  4-May-88  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	Created.
 *
 */

#include <emul_error_ifc.h>

extern "C" {
#include <tm_types.h>
#include <io_error.h>
#include <ns_error.h>
#include <us_error.h>
#include <net_error.h>

#include <sys/errno.h>
}

#define	unix_err_sub	0x03

int emul_error_to_unix(mach_error_t err)
{

    int             sub, system, code, module;
    sub = err_get_sub(err);
    system = err_get_system(err);
    code = err_get_code(err);

    module = err_system(system) | err_sub(sub);

    if ((system == err_kern) && (sub == unix_err_sub))
	return (code);

    switch (module) {
		
	    case NS_MODULE:
	    	switch (err) {
			case NS_INVALID_HANDLE:
				return(EAGAIN);
				
		    	case NS_NAME_NOT_FOUND:
				return(ENOENT);

			case NS_NAME_EXISTS:
				return(EEXIST);
				
			case NS_NAME_TOO_LONG:
			case NS_PATH_TOO_LONG:			
				return(ENAMETOOLONG);
				
			case NS_INVALID_NAME:
				return(EINVAL);

			case NS_NOT_DIRECTORY:
				return(ENOTDIR);
				
			case NS_IS_DIRECTORY:
				return(EISDIR);
				
			case NS_DIR_NOT_EMPTY:
				return(ENOTEMPTY);
				
			case NS_INFINITE_RETRY:
			case NS_INFINITE_FORWARD:
				return(ELOOP);

			case NS_INVALID_PREFIX:
				return(EINVAL);

			case NS_PREFIX_OVERFLOW:
				return(ENOSPC);

			case NS_BAD_DIRECTORY:
			case NS_UNKNOWN_ENTRY_TYPE:
				return(EFAULT);

			case NS_INVALID_GENERATION:
				return(EAGAIN);

			default:
				return(EFAULT);
		} /* end switch err */

	    case IO_MODULE:
	    	switch (err) {
			case IO_INVALID_OFFSET:
			case IO_INVALID_SIZE:
			case IO_INVALID_MODE:
				return(EINVAL);

			default:
				return(EFAULT);
		} /* end switch err */

	    case TM_ERR:
	    	switch (err) {
			case TM_INVALID_TASK:
			case TM_INVALID_TASK_ID:
			case TM_INVALID_TASK_GROUP:
				return(EINVAL);

			case TM_INVALID_KERNEL_PORT:
				return(EPERM);

			case TM_TOO_MANY_TASKS_IN_SESSION:
				return(EAGAIN);

			default:
				return(EFAULT);
		}

	    case US_MODULE:
		switch (err) {
			case US_UNKNOWN_ERROR:
			case US_OBJECT_NOT_FOUND:
			case US_OBJECT_EXISTS:
				return(EFAULT);

			case US_OBJECT_BUSY:
				return(EBUSY);

			case US_OBJECT_NOT_STARTED:
			case US_OBJECT_DEAD:
				return(EFAULT);

			case US_INVALID_ARGS:
				return(EINVAL);

			case US_INVALID_ACCESS:
				return(EACCES);

			case US_INVALID_FORMAT:
			case US_INVALID_BUFFER_SIZE:
				return(EINVAL);

			case US_ACCESS_DENIED:
				return(EACCES);

			case US_RESOURCE_EXHAUSTED:
				return(ENOSPC);

			case US_QUOTA_EXCEEDED:
				return(EDQUOT);

			case US_LIMIT_EXCEEDED:
				return(EFBIG);

			case US_NOT_IMPLEMENTED:
			case US_UNSUPPORTED:
				return(EOPNOTSUPP);

			case US_HARDWARE_ERROR:
				return(EIO);

			case US_RETRY_REQUIRED:
				return(EAGAIN);

			case US_NOT_AUTHENTICATED:
				return(EACCES);

			case US_EXCLUSIVE_ACCESS:
				return(ETXTBSY);

			case US_TIMEOUT:
				return(ETIMEDOUT);

			case US_BAD_REFCOUNT:
				return(ETOOMANYREFS);

			case US_INTERNAL_ERROR:
				return(EFAULT);

			default:
				return(EFAULT);
		}

	    case NET_MODULE: 
	    	switch (err) {
			case NET_INVALID_ADDR_FLAVOR:
			case NET_INVALID_ADDR_VALUE:
				return(EADDRNOTAVAIL);

			case NET_IS_CONNECTED:
				return(EISCONN);

			case NET_NOT_CONNECTED:
				return(ENOTCONN);

			case NET_CONNECTION_REFUSED:
				return(ECONNREFUSED);

			default:
				return(EFAULT);
		}

	    default:
		return(EFAULT);
		
    } /* end switch module */
    
}
