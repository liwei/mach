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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio.cc,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxio.cc,v $
 * Revision 2.6  94/07/08  16:01:53  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.5  94/06/29  14:56:58  mrt
 * 	Changed the code to support two i/o object types (byte and
 * 	record i/o.  This was to fix the i/o probe of a datagram 
 * 	socket (a record dev).
 * 	[94/06/29  13:56:57  grm]
 * 
 * Revision 2.4  94/01/11  17:50:30  jms
 * 	Add "select" and "probe" logic for "real" select.
 * 	[94/01/09  19:47:08  jms]
 * 
 * Revision 2.3  92/07/05  23:32:46  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:31:18  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:25:00  dpj]
 * 
 * Revision 2.2  91/11/06  14:11:49  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:15:26  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:08:56  pjg]
 * 
 * Revision 1.10  91/05/05  19:28:38  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:01:52  dpj]
 * 
 * 	Use sequential versions of I/O operations when appropriate.
 * 	Added code to close and replace underlying item.
 * 	[91/04/28  10:31:12  dpj]
 * 
 * Revision 1.9  91/04/12  18:48:34  jjc
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:49:18  jjc]
 * 
 * Revision 1.8.1.2  91/02/05  15:53:10  neves
 * 	Change ux_modify's ns_authenticate to an ns_duplicate.
 * 
 * Revision 1.8.1.1  91/02/05  15:47:23  neves
 * 	Fixed ux_open to set proper file flags.
 * 
 * Revision 1.8  90/12/10  09:49:58  jms
 * 	Added ux_map and ux_get_access methods.
 * 	[90/11/20  13:35:15  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:24  jms]
 * 
 * Revision 1.7  90/11/27  18:20:58  jms
 * 	NoChange
 * 	[90/11/20  14:41:06  jms]
 * 
 * 	Prepare to merge some changes from US31
 * 	[90/11/12  16:35:05  jms]
 * 
 * 	Prepare to merge on stuff from trunk
 * 	[90/11/10  12:43:52  jms]
 * 
 * Revision 1.6  90/11/10  00:38:43  dpj
 * 	Added ux_readv and ux_writev methods.
 * 	[90/10/24  14:36:47  neves]
 * 
 * 	ux_ftruncate no longer ignores its length parameter.
 * 	[90/10/18  15:53:35  neves]
 * 
 * 	Added ux_modify_protection method.
 * 	Moved the ux_ioctl method to uxio_tty object.
 * 	[90/10/17  12:51:51  neves]
 * 
 * Revision 1.5  89/11/28  19:12:09  dpj
 * 	Removed bogus history entry.
 * 	Removed ux_pipe().
 * 	Fixed arguments declaration in ux_open().
 * 	[89/11/20  20:57:03  dpj]
 * 
 * Revision 1.4  89/07/09  14:20:19  dpj
 * 	Updated debugging statements for new DEBUG macros.
 * 	[89/07/08  13:05:14  dpj]
 * 
 * Revision 1.3  89/06/30  18:36:32  dpj
 * 	Removed the conversion between ns_attr and the stat structure in ux_fstat.
 * 	It is now done in the uxstat object.
 * 	[89/06/29  00:48:26  dpj]
 * 
 * Revision 1.2  89/03/17  12:57:26  sanzi
 * 	fix hosed up debug statement.
 * 	[89/03/13  11:30:40  dorr]
 * 	
 * 	conditionalize debug output.
 * 	[89/03/12  20:27:35  dorr]
 * 	
 * 	fix stat uxprot bug.
 * 	[89/03/08  14:40:21  dorr]
 * 	
 * 	fix dlong calling conventions.
 * 	[89/03/07  11:00:09  dorr]
 * 	
 * 	enumerate tty ioctl's.  use dlong's.  convert to io_set_size().
 * 	[89/02/24  17:11:32  dorr]
 * 
 * Revision 1.1  89/02/21  21:54:58  dorr
 * Initial revision
 * 
 * Revision 1.11.1.2  89/02/20  21:09:14  dorr
 * 	rehash for new mach object world.
 * 
 *
 */

#ifndef lint
char * uxio_rcsid = "$Header: uxio.cc,v 2.6 94/07/08 16:01:53 mrt Exp $";
#endif	lint

#include <uxio_ifc.h>
#include <uxident_ifc.h>
#include <uxstat_ifc.h>

extern "C" {
#include <base.h>
#include <debug.h>

#include <us_byteio_ifc.h>
#include <ns_types.h>
#include <io_types.h>
#include <dlong.h>
/*
#include <cat_methods.h>
#include <io_methods.h>
#include <ns_methods.h>
#include <uxprot_methods.h>
#include <uxstat_methods.h>
#include <uxident_methods.h>
*/

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/errno.h>

#include <sys/ioctl.h>

#include "uxio.h"
}

#include <clone_ifc.h>

static struct dlong_t dlong_zero = { 0, };

extern int			emul_debug;
extern uxident*		uxident_obj;
extern uxstat*		uxstat_obj;

extern "C" any_t uxio_forked_probe(any_t arg);

/*
 * INTERNAL States relative to "select"
 */

/* SEL_PROBE_NONE:  Null state */
#define SEL_PROBE_NONE		0
					
/* SEL_PROBE_REQUESTED: Doing (about to do) a probe */
#define SEL_PROBE_REQUESTED	1

/* SEL_PROBE_SUCCEDED: Did a suscessful probe */
#define SEL_PROBE_SUCCEEDED	2

/* SEL_PROBE_FAILED: Did a failed probe */
#define SEL_PROBE_FAILED	3

/*
 *	define the methods for the base file descriptor object
 */

#define BASE usTop
DEFINE_LOCAL_CLASS(uxio)

uxio::uxio() : cur_offset(dlong_zero), flags(FILE_FLAG_NONE), 
	       obj(0), byteio_obj(0), recordio_obj(0)
{}

uxio::~uxio()
{
	mach_object_dereference(_Local(obj));
	DEBUG1(TRUE,(0, "uxio::~uxio\n"));
}


mach_error_t 
uxio::ux_open(usItem* cat_obj, unsigned int ux_access,ns_access_t open_access)
{
	mach_error_t		err = ERR_SUCCESS;

	if (Local(obj) != NULL) {
		mach_object_dereference(Local(obj));
	}
	mach_object_reference(cat_obj);
	Local(obj) = cat_obj;

	/* One of these will be nonzero! terrible hack.... */
	byteio_obj = usByteIO::castdown(cat_obj);
	recordio_obj = usRecIO::castdown(cat_obj);

	/*
	 * XXX Special hack to remember the "sequential" attribute.
	 */
	if (Local(flags) & FILE_SEQUENTIAL) {
		Local(flags) = FILE_SEQUENTIAL;
	} else {
		Local(flags) = FILE_FLAG_NONE;
	}

	if (ux_access & O_NDELAY) Local(flags) |= FILE_NDELAY;
	if (ux_access & O_APPEND) Local(flags) |= FILE_APPEND;

	if (! (ux_access & O_NDELAY) ) {
#if	soon
		/* some objects like to block on open */
		err = io_block_until_open(obj);
		if (mio_not_supported(err))
			err = ERR_SUCCESS;
		if (err) {
			return err;
		}
#endif	soon
	}

	if (ux_access & O_TRUNC) {
		DEBUG1(TRUE,(0, "open: truncating"));
		if (!byteio_obj && !recordio_obj) {
			DEBUG1(TRUE,(0, "uxio::ux_open\n"));
			return MACH_OBJECT_NO_SUCH_OPERATION;
		}
		if (byteio_obj)
			err = byteio_obj->io_set_size(dlong_zero);
		else
			return(US_INVALID_ARGS);
		if (err) return err;
	}

	return err;

}

mach_error_t uxio::ux_read(char* buf, unsigned int* len)
{
	mach_error_t		err;
	unsigned int		mode;

	mode = IOM_TRUNCATE | ((Local(flags) & FILE_NDELAY) ? 0 : IOM_WAIT);
	DEBUG1(TRUE,(0, "about to read %d bytes from %d\n", 
		    *len, Local(cur_offset) ));


	/* XXX RESTARTABLE */

	/* Clear pending read data */
	if (Local(select_state)[SEL_TYPE_READ] == SEL_PROBE_SUCCEEDED) {
		Local(select_state)[SEL_TYPE_READ] = SEL_PROBE_NONE;
	}

	if (Local(flags) & FILE_SEQUENTIAL) {
		err = io_read_seq_internal(mode,buf,len,&Local(cur_offset));
	} else {
		err = io_read_internal(mode, cur_offset, (pointer_t)buf, len);
	}
	if (err == ERR_SUCCESS ) {
		/* update our seek pos */
		ADD_U_INT_TO_DLONG(&Local(cur_offset),*len);
	} else
		*len = 0;

	DEBUG1(TRUE,(0, "io_obj_read: got back %d rc=%s\n", 
		    *len, err ? (char *)mach_error_string(err) : "ok"));
	DEBUG1(TRUE,(0, "io_obj_read: returning '%*.10s'\n", *len > 30 ? 30 : *len, buf));

	return err;
}

mach_error_t uxio::ux_readv(struct iovec *iov, int iovcnt, unsigned int *len)
{
	int			i;
	mach_error_t		err;
	unsigned int		mode;

	mode = IOM_TRUNCATE | ((Local(flags) & FILE_NDELAY) ? 0 : IOM_WAIT);
	DEBUG1(TRUE,(0, "about to read %d bytes from %d\n", 
		    *len, Local(cur_offset) ));

	*len = 0;
	for (i = 0; i < iovcnt; i++) {
	  	int		rlen = iov[i].iov_len;

		/* XXX RESTARTABLE */
		if (Local(flags) & FILE_SEQUENTIAL) {
			err = io_read_seq_internal(mode,iov[i].iov_base,
						&rlen,&Local(cur_offset));
		} else {
			err = io_read_internal(mode, cur_offset,
					     (pointer_t)iov[i].iov_base,&rlen);
		}
		if (err == ERR_SUCCESS ) {
			/* update our seek pos */
			ADD_U_INT_TO_DLONG(&Local(cur_offset),rlen);
			*len += rlen;
		}

		DEBUG1(TRUE,(0, "io_obj_read: got back %d rc=%s\n", 
		    *len, err ? (char *)mach_error_string(err) : "ok"));
		DEBUG1(TRUE,(0, "io_obj_read: returning '%*.10s'\n",
		    *len > 30 ? 30 : *len, iov[i].iov_base));

		if (err != ERR_SUCCESS) break;
	}

	return err;
}

int uxio::ux_write(char *buf, unsigned int *len)
{
	mach_error_t		err;
	unsigned int		mode;

	DEBUG1(TRUE,(Local(obj),"io_obj_write called\n"));

	mode = IOM_TRUNCATE 
		| ((Local(flags) & FILE_NDELAY) ? 0 : IOM_WAIT);

	if (byteio_obj == 0) {
		DEBUG1(TRUE,(0, "uxio::ux_write\n"));
		return MACH_OBJECT_NO_SUCH_OPERATION;
	}
	/* XXX RESTARTABLE */
	if (_Local(flags) & FILE_APPEND) {

		/* append mode */
		err = byteio_obj->io_append(mode, (pointer_t)buf, len);

	} else {
		if (Local(flags) & FILE_SEQUENTIAL) {
			err = byteio_obj->io_write_seq(mode, buf, len,
						   &Local(cur_offset));
		} else {
			err = byteio_obj->io_write(mode, cur_offset,
					       (pointer_t)buf,len);
		}
	}

	if (err == ERR_SUCCESS ) {
		/* XXX lock */
		ADD_U_INT_TO_DLONG(&Local(cur_offset),*len);
	} else {
		*len = 0;
	}

	return err;
}


mach_error_t
uxio::ux_writev(struct iovec *iov, int iovcnt, unsigned int *len)
{
	mach_error_t		err;
	unsigned int		mode;
	int			i;

	DEBUG1(TRUE,(Local(obj),"ux_writev: io_obj_write called\n"));

	mode = IOM_TRUNCATE 
		| ((Local(flags) & FILE_NDELAY) ? 0 : IOM_WAIT);

	*len = 0;
	if (byteio_obj == 0) {
		DEBUG1(TRUE,(0, "uxio::ux_write\n"));
		return MACH_OBJECT_NO_SUCH_OPERATION;
	}
	for( i = 0; i < iovcnt; i++) {
		int		rlen = iov[i].iov_len;

		/* XXX RESTARTABLE */
		if (Local(flags) & FILE_APPEND) {
			/* append mode */
			err = byteio_obj->io_append(mode, 
					     (pointer_t)iov[i].iov_base,&rlen);
		} else {
			if (Local(flags) & FILE_SEQUENTIAL) {
				err = byteio_obj->io_write_seq(mode,
						iov[i].iov_base, &rlen,
						&Local(cur_offset));
			} else {
				err = byteio_obj->io_write(mode, cur_offset,
					    (pointer_t)iov[i].iov_base, &rlen);
			}
		}

		if (err == ERR_SUCCESS ) {
			/* XXX lock */
			ADD_U_INT_TO_DLONG(&Local(cur_offset),rlen);
			*len += rlen;
		} else {
			break;
		}
	}

	return err;
}

mach_error_t
uxio::ux_fstat(struct stat *buf)
{
	struct ns_attr		ns_attrs;
	unsigned int		attrlen = 0;
	mach_error_t		err;

	err = ns_get_attributes_internal(&ns_attrs, &attrlen);
	if (err) return err;

	err = uxstat_obj->uxstat_std_to_unix_attr(&ns_attrs,buf);
	if (err) return err;

	return(ERR_SUCCESS);

}

mach_error_t
uxio::ux_lseek(long int *pos, unsigned int mode)
{
	mach_error_t		err = ERR_SUCCESS;
	struct ns_attr		ns_attr;
	unsigned int		attrlen = 0;
	ns_size_t		end;

	switch(mode) {
	    case L_SET:
		INT_TO_DLONG(&Local(cur_offset),*pos);
		break;

	    case L_XTND:
		err = ns_get_attributes_internal(&ns_attr, &attrlen);
		if (err != ERR_SUCCESS)
			end = dlong_zero;
		else
			end = ns_attr.size;

		INT_TO_DLONG(&Local(cur_offset),*pos);
		DLONG_ADD(&Local(cur_offset),Local(cur_offset),end);
		break;

	    case L_INCR:
		ADD_INT_TO_DLONG(&Local(cur_offset),*pos);
		break;

	    default:
		err = unix_err(EINVAL);
		break;
	}

	*pos = DLONG_TO_U_INT(Local(cur_offset));

	return err;
}

mach_error_t
uxio::ux_fcntl(int cmd, int *arg)
{
	mach_error_t		err = ERR_SUCCESS;

	switch(cmd) {
	    case F_GETFL: {
		int  a = 0;
		if (Local(flags) & FILE_NDELAY)
			a |= FNDELAY;
		if (Local(flags) & FILE_APPEND)
			a |= FAPPEND;
		if (Local(flags) & FILE_ASYNC)
			a |= FASYNC;
		*arg = a;
	        }
		break;
		
	    case F_SETFL:
		if (*arg & FNDELAY)
			Local(flags) |= FILE_NDELAY;
		if (*arg & FAPPEND)
			Local(flags) |= FILE_APPEND;
		if (*arg & FASYNC)
			Local(flags) |= FILE_ASYNC;
		break;
		
	    case F_GETOWN:
	    case F_SETOWN:
	    default:
		/* crap out, for now */
		err = unix_err(EINVAL);
		break;
	}

	return err;
}

mach_error_t uxio::ux_ioctl(int, int*)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}


mach_error_t
uxio::ux_ftruncate(unsigned int len)
{
	mach_error_t		err;
	io_size_t		new_size;

	UINT_TO_IO_SIZE(len,&new_size);

	/* Should this be only for byte devices? XXX */
	if (byteio_obj == 0) {
		DEBUG1(TRUE,(0, "uxio::ux_ftruncate\n"));
		return MACH_OBJECT_NO_SUCH_OPERATION;
	}
	err = byteio_obj->io_set_size(new_size);

	return(err);
}

#define MAX_SET_PROT_RETRIES	10

mach_error_t
uxio::ux_modify_protection(int uid, int gid, int mode)
{
	int			cur_mode, cur_uid, cur_gid;
	int			prot_data[DEFAULT_NS_PROT_LEN];
	int			protlen;
	int			retry = MAX_SET_PROT_RETRIES;
	mach_error_t		err;
	usItem*		new_obj = 0;
//	mach_object_t		new_obj = NULL;
	ns_access_t		access;
	ns_prot_t		prot = (ns_prot_t)prot_data;

	/*
         * Get NSR_ADMIN and NSR_GETATTR privileges.
	 */

	access = NSR_ADMIN | NSR_GETATTR;
	err = obj->ns_duplicate(access, &new_obj);
	if (err != ERR_SUCCESS) return err;

	/*
	 * If there is a mismatch in the standard protection
	 * structure retrieved and the one stored at server,
	 * then we get the new one and try again.
	 */
	do {
		err = new_obj->ns_get_protection(prot, &protlen);
		if (err != ERR_SUCCESS) break;
		
		uxstat_obj->uxprot_std_to_unix_prot(prot->acl, 
						     prot->head.acl_len,
						     &cur_mode, &cur_uid, 
						     &cur_gid);
	    
		/* Change specified values */
		if (uid != -1) cur_uid = uid;
		if (gid != -1) cur_gid = gid;
		if (mode != -1) cur_mode = mode;

		uxstat_obj->uxprot_unix_to_std_prot(cur_mode, cur_uid,cur_gid,
						     prot->acl, 
						     &prot->head.acl_len);

		err = new_obj->ns_set_protection(prot, protlen);
	} while (retry-- > 0 && err == NS_INVALID_GENERATION);
	
	mach_object_dereference(new_obj);
	if (retry == 0)
		err = unix_err(EIO);

	return err;
}

mach_error_t
uxio::ux_map(task_t task, vm_address_t* addr, vm_size_t size,
	      vm_offset_t mask, boolean_t anywhere, vm_offset_t paging_offset,
	      boolean_t copy, vm_prot_t cprot, vm_prot_t mprot,
	      vm_inherit_t inherit)
{
	/* Should this be only for byte devices? XXX */
	if (byteio_obj) {
		return byteio_obj->io_map(task,addr,size,mask,anywhere,
				       paging_offset,copy,cprot,mprot,inherit);
	} else {
		return MACH_OBJECT_NO_SUCH_OPERATION;
	}
}

mach_error_t uxio::ux_get_access(ns_access_t* access)
{
	int			cred_data[DEFAULT_NS_CRED_LEN];
	ns_cred_t		cred = (ns_cred_t) &cred_data;
	int			credlen = DEFAULT_NS_CRED_LEN;
	
	return obj->ns_get_access(access, cred, &credlen);
}

mach_error_t uxio::ux_close_internal()
{
	mach_object_dereference(Local(obj));
	Local(obj) = NULL;

	return(ERR_SUCCESS);
}


mach_error_t uxio::ux_set_sequential_internal()
{
	Local(flags) |= FILE_SEQUENTIAL;
	return(ERR_SUCCESS);
}
mach_error_t
uxio::ux_select_one(uxselect *selector, ux_select_fd_t select_fd,
		ux_select_type_t select_type, boolean_t local_only)
{
	mach_error_t		ret = ERR_SUCCESS;
	unsigned int		mode;
	cthread_fn_t		fork_fn = NULL;
	uxio_fork_rec_t		fork_rec = NULL;
	uxio_select_info_t	select_info;

	int sindex=-1;
	int i;
	uxselect	*tmp_selector;
	mach_port_t	thread;

	mode = IOM_PROBE | ((Local(flags) & FILE_NDELAY) ? 0 : IOM_WAIT);

	/* XXX just signal immediately for files? */

	mutex_lock(&(Local(lock)));

	if ((SEL_PROBE_SUCCEEDED != Local(select_state[select_type])) &&
	    (local_only)) {
		mutex_unlock(&(Local(lock)));
		return(IO_WOULD_WAIT);
	}

	/* 
	 * Find the next empty record 
	 * and fill it out
	 */
	for (i=0;i<UXIO_MAX_SELECT_COUNT;i++) {
		tmp_selector = ((Local(active_selects))[i]).selector;
		if (NULL == tmp_selector) {
			sindex = i;
			break;
		}

		/* check to see if the select is done and it was not one with
			a waiting cthread */
		thread = ((Local(active_selects))[i]).thread;
		if ((thread == NULL) && (tmp_selector->select_completed())) {
			mach_object_dereference(tmp_selector);
			((Local(active_selects))[i]).selector = NULL;
			sindex = i;
			break;
		}
	}

	if (0 > sindex) {
		/* to many active selects at one time */
		mutex_unlock(&(Local(lock)));
		return(US_RESOURCE_EXHAUSTED);
	}

	select_info = &((Local(active_selects))[sindex]);
	select_info->select_type = select_type;
	select_info->selector = selector;
	select_info->thread = MACH_PORT_NULL;
	mach_object_reference(selector);

	select_info->mode = mode;
	select_info->select_fd = select_fd;

	/*
	 * What now:	1) we are done, signal
	 *		2) some other thread is doing the work,
	 *		3) we must do the work.
	 */

	/* Have we recieved notification of pending data without telling anyone yet? */
	if (SEL_PROBE_SUCCEEDED == Local(select_state[select_type])) {
		/* data drained since probe? Check again no_wait? XXX XXX */
		mutex_unlock(&(Local(lock)));
		ux_select_one_done(sindex, ERR_SUCCESS);
		return(ERR_SUCCESS);
	}

	/* If we are in progress, just sit in the queue and let the other guy deal */
	if (SEL_PROBE_REQUESTED == Local(select_state[select_type])) {
		mutex_unlock(&(Local(lock)));
		return(ERR_SUCCESS);
	}

	/*
	 * We are going to be the active probe!
	 */
	Local(select_state[select_type]) = SEL_PROBE_REQUESTED;
	select_info->thread = cthread_self();
	mutex_unlock(&(Local(lock)));

	/* Fork a thread to probe (if waiting and fork supplied) */
	if (mode & IOM_WAIT) {
		fork_fn = uxio_fork_probe_routine_internal();
	}
	/* are we the only selected fd with no timeout? ifso, we dont need to cthread_fork? XXX */
	if (fork_fn) {
		fork_rec = (uxio_fork_rec_t)malloc(sizeof(struct uxio_fork_rec));
		/* XXX Leak on fork? */
		fork_rec->self_obj = this;
		fork_rec->active_select_index = sindex;
		select_info->thread = cthread_fork(fork_fn, fork_rec);
		return(ERR_SUCCESS);
	}
	else {
		ret = uxio_probe_internal(sindex);
		if (ERR_SUCCESS != ret) {
			return(ret);
		}
		return(ret);
	}
}

/*
 * return the function to be used by cthread_fork to correctly castdown
 * the "this" to the right type after the fork.
 */
cthread_fn_t 
uxio::uxio_fork_probe_routine_internal()
{
	return(uxio_forked_probe);
}

any_t
uxio_forked_probe(any_t arg)
{
	uxio			*sobj;
	int			sindex;
	mach_error_t		ret = ERR_SUCCESS;
	
	/* Copy to shrink the memory leak window if there is a ux_fork before
		this probe is done XXX */

	sobj = ((uxio_fork_rec_t)arg)->self_obj;
	sindex = ((uxio_fork_rec_t)arg)->active_select_index;
	free((uxio_fork_rec_t)arg);

	ret = sobj->uxio_probe_internal(sindex);

	/* We have done our thing, time to die (by just returning)*/
	return(ERR_SUCCESS);
}

/*
 * uxio_probe_internal
 * 	Probe the item in question and signal the select upon completion.
 *	Always complete away for plain files.
 *	Should be over-ridden for every other type of uxio object.
 */
mach_error_t
uxio::uxio_probe_internal(int active_select_index)
{
	io_mode_t		mode;

	mach_error_t	ret = ERR_SUCCESS;
	char		dummy_buf[32768];
	unsigned int	dummy_size;
	io_offset_t	dummy_offset;
	io_recnum_t	dummy_recnum;
	ux_select_type_t	select_type;

	DEBUG0(TRUE,(0, "uxio::uxio_probe_internal %d\n", active_select_index));
	/* XXX Lockit?  Here our info should be safe till we complete */
	mode = ((Local(active_selects))[active_select_index]).mode;
	select_type = ((Local(active_selects))[active_select_index]).select_type;
	mode |= IOM_PROBE;

	/* XXX If this is just a file, no probe needed */
	/*
	 * Do the probe
	 */
	switch (select_type) {
	    case SEL_TYPE_READ:
		dummy_size = 1;
		if (byteio_obj) {
			ret = byteio_obj->io_read_seq(mode, dummy_buf,
						      &dummy_size,
						      &dummy_offset);
		}else{
			ret = recordio_obj->io_read1rec_seq(mode, dummy_buf,
							    &dummy_size,
							    &dummy_recnum);
		}
		break;

	    case SEL_TYPE_WRITE:
		dummy_size = 1;
		if (byteio_obj) {
			ret = byteio_obj->io_write_seq(mode, dummy_buf,
						       &dummy_size,
						       &dummy_offset);
		}else{
			ret = recordio_obj->io_write1rec_seq(mode, dummy_buf,
							     dummy_size,
							     &dummy_recnum);
		}
		break;

	    case SEL_TYPE_EXCEPT:
	    default:
		/* XXX except probes not supported for now, pretend no probe */
		ret = ERR_SUCCESS;
		break;
	}

	DEBUG0(TRUE,(0, "uxio::uxio_probe_internal, probe_done %d\n", active_select_index));
	ux_select_one_done(active_select_index, ret);
	DEBUG0(TRUE,(0, "uxio::uxio_probe_internal, sel_one_done %d\n", active_select_index));
	return(ret);
}

/*
 * Called whenever a probe completes
 */
mach_error_t
uxio::ux_select_one_done(int active_select_index, mach_error_t probe_ret)
{
	uxselect *selector;
	ux_select_fd_t select_fd;
	ux_select_type_t select_type, completed_select_type;

	int		i;
	boolean_t	count_signaled = 0;
	boolean_t	signaled;

	mutex_lock(&(Local(lock)));

	/* signal all "active" selects of the right type*/
	completed_select_type = 
		((Local(active_selects))[active_select_index]).select_type;
	Local(select_state[completed_select_type]) = SEL_PROBE_SUCCEEDED;

	for (i = 0; i < UXIO_MAX_SELECT_COUNT; i++) {
		selector = ((Local(active_selects))[i]).selector;
		/* Do we care about this one ? */
		if (NULL == ((Local(active_selects))[i]).selector) continue;

		select_type = ((Local(active_selects))[i]).select_type;
		if (completed_select_type != select_type) continue;

		select_fd = ((Local(active_selects))[i]).select_fd;

		signaled = FALSE;
		(void) selector->signal(select_fd, select_type, probe_ret, 
				&signaled);
		
		mach_object_dereference(selector);
		((Local(active_selects))[i]).selector = NULL;
		if (signaled) count_signaled++;
	}

	if (0 < count_signaled) {
		Local(select_state[completed_select_type]) = SEL_PROBE_NONE;
	}

	mutex_unlock(&(Local(lock)));
	return(ERR_SUCCESS);
}

mach_error_t
uxio::clone_init(mach_port_t child)
{
	if (NULL != obj) {
		return usClone::castdown(obj)->clone_init(child);
	}
	else {
		return ERR_SUCCESS;
	}
}

mach_error_t
uxio::clone_complete()
{
	if (NULL != obj) {
		return usClone::castdown(obj)->clone_complete();
	}
	else {
		return ERR_SUCCESS;
	}
}

mach_error_t
uxio::clone_abort(mach_port_t child)
{
	if (NULL != obj) {
		return usClone::castdown(obj)->clone_abort(child);
	}
	else {
		return ERR_SUCCESS;
	}
}


mach_error_t 
uxio::io_read_internal(int mode, io_offset_t io_start, pointer_t buf, 
			unsigned int* num)
{
	/* Should this be only for byte devices? XXX */
	if (byteio_obj == 0) {
		DEBUG1(TRUE,(0, "uxio::ux_read_internal\n"));
		return MACH_OBJECT_NO_SUCH_OPERATION;
	} else
		return byteio_obj->io_read(mode, io_start, buf, num);
}

mach_error_t 
uxio::io_read_seq_internal(io_mode_t mode, char *addr, unsigned int *num, 
			   io_offset_t *offset)
{
	/* Should this be only for byte devices? XXX */
	if (byteio_obj == 0) {
		DEBUG1(TRUE,(0, "uxio::ux_read_internal\n"));
		return MACH_OBJECT_NO_SUCH_OPERATION;
	} else
		return byteio_obj->io_read_seq(mode, addr, num, offset);
}

mach_error_t 
uxio::ns_get_attributes_internal(ns_attr_t attrs, int *attrlen)
{
	if (!byteio_obj) {
		DEBUG1(TRUE,(0, "uxio::ns_get_attributes_internal\n"));
		return MACH_OBJECT_NO_SUCH_OPERATION;
	}else
		return byteio_obj->ns_get_attributes(attrs, attrlen);
}

