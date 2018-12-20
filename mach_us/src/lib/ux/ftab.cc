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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/ftab.cc,v $
 *
 * Purpose: Unix file table
 *
 * HISTORY: 
 * $Log:	ftab.cc,v $
 * Revision 2.8  94/07/08  16:01:45  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.7  94/05/17  14:08:30  jms
 * 	Needed implementations of virtual methods in class ftab for 2.3.3 g++ -modh
 * 	[94/04/28  18:56:52  jms]
 * 
 * Revision 2.5.2.1  94/02/18  11:31:34  modh
 * 	Needed implementations of virtual methods in class ftab for 2.3.3 g++
 * 
 * Revision 2.6  94/01/11  17:50:28  jms
 * 	Add debugging code
 * 	[94/01/09  19:45:34  jms]
 * 
 * Revision 2.5  93/01/20  17:38:51  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  17:20:30  jms]
 * 
 * Revision 2.4  92/07/05  23:32:34  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:24:32  dpj]
 * 
 * Revision 2.3  91/12/20  17:45:02  jms
 * 	Close on exec bug fix. (dpj)
 * 	[91/12/20  16:14:26  jms]
 * 
 * Revision 2.2  91/11/06  14:10:26  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:11:32  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:03:41  pjg]
 * 
 * 	created.
 * 	[89/06/21  15:49:12  dorr]
 * 
 * Revision 2.3  90/12/10  09:49:50  jms
 * 	Fixed an fcntl F_DUPFD max_open_file bug.
 * 	[90/11/20  13:21:41  neves]
 * 	Merge for Paul Neves of neves_US31
 * 	[90/12/06  17:38:20  jms]
 * 
 * Revision 2.2  89/07/19  11:07:35  dorr
 * 	switch to new style debugging.
 * 
 */

#include <ftab_ifc.h>

extern "C" {
#include <base.h>
#include <errno.h>
}
#if SHARED_DATA_TIMING_EQUIVALENCE
#include <us_tm_task_ifc.h>
extern	usTMTask*	tm_task_obj;	/* XXX emul lib copy of task object */
#endif SHARED_DATA_TIMING_EQUIVALENCE

boolean_t	ftab_debug = TRUE;

/*
 * Utilities
 */


#define	Get_fp(fd,nfp)							\
(									\
		(((fd) < 0) || ((fd) >= Count(_Local(files))) ||	\
			((*(nfp) = _Local(files)[(fd)].fp) == FP_NULL))	\
		? unix_err(EBADF) : ERR_SUCCESS 			\
)

#define	Get_ftab(fd,nftab)						\
(									\
		(((fd) < 0) || ((fd) >= Count(_Local(files))) ||	\
			(((*(nftab) = &_Local(files)[(fd)]))->fp == FP_NULL))\
		? unix_err(EBADF) : ERR_SUCCESS 			\
)



/*
 * Methods.
 */
ftab::ftab()
{
	int		fd;

	mutex_init(&_Local(lock));
	dll_init(&_Local(free_file_objects));
	dll_init(&_Local(in_use_objects));

	/* put everyone on the free list */
	for( fd=0; fd<Count(_Local(files)); fd++ ) {
		_Local(files)[fd].fp = FP_NULL;
		_Local(files)[fd].flags = FD_FLAG_NONE;
		dll_enter(&_Local(free_file_objects), &_Local(active_file_objects)[fd], 
			  fp_t, chain);
	}
}

ftab::~ftab()
{

	dll_entry_t	f;

	/*
	 *	release all in-use i/o objects.  
	 */

	for( f=dll_first(&_Local(in_use_objects)); 
	     !dll_end(&_Local(in_use_objects),f); 
	     f=dll_next(f)) {
			mach_object_dereference(((fp_t)f)->obj);
		}
}


mach_error_t ftab::ftab_get_obj(int fd, uxio** obj)
{
	mach_error_t	err;
	fp_t		fp;
#if SHARED_DATA_TIMING_EQUIVALENCE
	unsigned int	prev_shared_status = FTSS_NULL;
	tm_task_id_t	tid;
#endif SHARED_DATA_TIMING_EQUIVALENCE

	mutex_lock(&_Local(lock));
	{
		*obj = NULL;
		if( (err = Get_fp(fd, &fp)) == ERR_SUCCESS ) {
			*obj = fp->obj;
			mach_object_reference(*obj);
#if SHARED_DATA_TIMING_EQUIVALENCE
			prev_shared_status = fp->shared_status;
			fp->shared_status &= FTSS_TOUCHED;
#endif SHARED_DATA_TIMING_EQUIVALENCE
		}
	}
	mutex_unlock(&_Local(lock));

#if SHARED_DATA_TIMING_EQUIVALENCE
	/* If we have a newly touched inherited FD, then TM touch it */
	if ((! (FTSS_TOUCHED & prev_shared_status)) &&
	    (FTSS_INHERITED & prev_shared_status)) {
		tm_task_obj->tm_get_task_id(&tid);
		tm_task_obj->tm_touch_shared(tid);
	}
#endif SHARED_DATA_TIMING_EQUIVALENCE

	return err;
}


mach_error_t ftab::ftab_add_obj(uxio* obj, int *fd)
{
	mutex_lock( &_Local(lock) );
	{
		*fd = falloc(0);
		if (*fd < 0) {
			mutex_unlock(&_Local(lock));
			return unix_err(EBADF);
		}
		_Local(files)[*fd].fp->obj = obj;
		mach_object_reference(obj);
	}
	mutex_unlock( &_Local(lock) );

	return ERR_SUCCESS;
}


mach_error_t ftab::ftab_exec()
{
	ftab_t		fdp;

	/*
	 * run through the file table and close everyone with the close-on-exec
	 * flag set
	 */
	mutex_lock(&_Local(lock));
	{
		for( fdp = _Local(files); 
		     fdp < Endof(_Local(files));
		     fdp++ ) {
			if (fdp->flags & FD_MASK(FD_CLOSE_ON_EXEC))
				(void)close_internal(fdp);
		}
	}
	mutex_unlock(&_Local(lock));

	return ERR_SUCCESS;
}

/*
 *	fd manipulation methods
 */

mach_error_t ftab::ftab_close(int fd)
{
	mach_error_t	err;
	ftab_t		ftab;

	DEBUG0(ftab_debug,(0, "ftab_close: closing %d", fd));

	mutex_lock(&_Local(lock));
	{
		if ( (err = Get_ftab(fd, &ftab)) ) {
			mutex_unlock( &_Local(lock) );
			return err;
		}
		err = close_internal(ftab);
	}
	mutex_unlock(&_Local(lock));

	return err;
}

mach_error_t ftab::ftab_set_flag(int fd, int flag, boolean_t val)
{
	mach_error_t	err;
	ftab_t		ftab;

	mutex_lock( &_Local(lock) );
	{
		err = Get_ftab(fd, &ftab);
		if (err) {
			mutex_unlock( &_Local(lock) );
			return err;
		}
				  
		if (val) 
			ftab->flags |= FD_MASK(flag);
		else
			ftab->flags &= ~FD_MASK(flag);
	}
	mutex_unlock( &_Local(lock) );

	return err;
}

mach_error_t ftab::ftab_get_flags(int fd, int *flags)
{
	mach_error_t	err;
	ftab_t		ftab;

	mutex_lock( &_Local(lock) );
	{
		err = Get_ftab(fd, &ftab);
		if (err) {
			mutex_unlock( &_Local(lock) );
			return err;
		}
				  
		*flags = ftab->flags;
	}
	mutex_unlock( &_Local(lock) );

	return err;
}


/*
 *	cloning operations:  clone active i/o objects into
 *	the child of a fork operation
 */
mach_error_t ftab::clone_init(mach_port_t child)
{
	
	dll_entry_t	f;
	mach_error_t	err;
	
	DEBUG0(ftab_debug,(0,"ftab::clone-init"));

	mutex_lock( &_Local(lock) );
	{
		/* clone each active i/o object into the child */
		for( f=dll_first(&_Local(in_use_objects)); 
		     !dll_end(&_Local(in_use_objects),f); f=dll_next(f)) {
			DEBUG1(ftab_debug,(0,"ftab::clone-init: fd %d \n", f));			err = (((fp_t)f)->obj)->clone_init(child);
			if (err == MACH_OBJECT_NO_SUCH_OPERATION)
				err = ERR_SUCCESS;
			if (err) break;
		}
		/*
		 *  on error, undo what you just did
		 */
		if (err) {
			DEBUG0(ftab_debug,(0,"ftab::clone-init failed %d \n", f));
			for( f=dll_first(&_Local(in_use_objects)); 
			     !dll_end(&_Local(in_use_objects),f); 
			     f=dll_next(f)) {
				DEBUG0(ftab_debug,(0,"ftab::clone-init abort: fd %d \n", f));
				(((fp_t)f)->obj)->clone_abort(child);
			}
		
		}
	}
	mutex_unlock( &_Local(lock) );
	
	return err;
}


/*
 *	clone_abort: something went wrong... undo the clone
 */
mach_error_t ftab::clone_abort(mach_port_t child)
{
	
	dll_entry_t	f;
	
	DEBUG0(ftab_debug,(0,"ftab::clone-abort"));

	mutex_lock( &_Local(lock) );
	{
		/* abort the cloning operation */
		for( f=dll_first(&_Local(in_use_objects)); 
		    !dll_end(&_Local(in_use_objects),f); 
		    f=dll_next(f)) {
			(void)(((fp_t)f)->obj)->clone_abort(child);
		}
		
	}
	mutex_unlock( &_Local(lock) );
	
	return ERR_SUCCESS;
}

 
mach_error_t ftab::clone_complete()
{
	mach_error_t	err = ERR_SUCCESS;

	DEBUG0(ftab_debug,(0,"ftab::clone-complete"));

	mutex_lock(&_Local(lock));
	{
		dll_entry_t	f;

		/* complete the clone operation */
		for( f=dll_first(&_Local(in_use_objects)); 
		     !dll_end(&_Local(in_use_objects),f); 
		     f=dll_next(f)) {
#if SHARED_DATA_TIMING_EQUIVALENCE
			((fp_t)f)->shared_status &= FTSS_INHERITED;
			((fp_t)f)->shared_status &= ~FTSS_TOUCHED;
#endif SHARED_DATA_TIMING_EQUIVALENCE
			err = (((fp_t)f)->obj)->clone_complete();
			if (err == MACH_OBJECT_NO_SUCH_OPERATION)
				err = ERR_SUCCESS;
			if (err)
				break;
		}
	}
	mutex_unlock(&_Local(lock));
	return err;
}



/*
 *	falloc: return an unused fd slot.  assume this is called
 *	with the lock set
 */
int ftab::falloc(int fd)
{
	fp_t		fp;

	if (dll_empty(&_Local(free_file_objects)) )
		return -1;

	for( ; fd<Count(_Local(files)); fd++ )
		if( _Local(files)[fd].fp == FP_NULL ) break;

	if (fd < Count(_Local(files))) {
		fp = (fp_t)dll_first(&_Local(free_file_objects));

		dll_remove(&_Local(free_file_objects), fp, fp_t, chain);
		dll_enter(&_Local(in_use_objects), fp, fp_t, chain);
#if SHARED_DATA_TIMING_EQUIVALENCE
		fp->shared_status = FTSS_NULL;
#endif SHARED_DATA_TIMING_EQUIVALENCE
		fp->ref_cnt = 1;
		_Local(files)[fd].fp = fp;
		_Local(files)[fd].flags = FD_FLAG_NONE;

		return fd;
	}
	else
		return -1;
}

/*
 *  close_internal:  called with the lock set, this does the
 *  real work of close
 */

mach_error_t ftab::close_internal(ftab_t ftabp)
{
	fp_t			fp;

	fp = ftabp->fp;

	ftabp->fp = FP_NULL;
	ftabp->flags = FD_FLAG_NONE;

	if (fp == FP_NULL) return ERR_SUCCESS;

	if ( --(fp->ref_cnt) == 0) {

		/* get rid of the underlying object */
		mach_object_dereference(fp->obj);

		/* take off the active queue and put on the inactive queue */
		dll_remove(&_Local(in_use_objects), fp, fp_t, chain);
		dll_enter(&_Local(free_file_objects), fp, fp_t, chain);
	}
	return ERR_SUCCESS;
}

/*
 *	dup & co.
 */
mach_error_t ftab::ftab_cdup(int source, int *targ, boolean_t alloc)
{
	mach_error_t		err = ERR_SUCCESS;
	fp_t			fp;

	mutex_lock( &_Local(lock) );
	{
		ftab_t			targ_ftab;

	        if ((! alloc) && (*targ < 0 || *targ >= MAX_OPEN_FILES)) {
			err = unix_err(EINVAL);
			goto finish;
		}

		if ( (err = Get_fp(source, &fp)) ) {
			goto finish;
		}
		
		/* bump our ref count */
		++(fp->ref_cnt);
	
		if ( alloc ) {
			/* find an available fd */
			for ( targ_ftab = _Local(files); 
			      targ_ftab < Endof(_Local(files));
			      targ_ftab++ )
				if ( targ_ftab->fp == FP_NULL ) break;

			if ( targ_ftab == Endof(_Local(files)) ) {
				err = unix_err(EBADF);
				goto finish;
			}

		} else {

			if (*targ < 0 || *targ >= Count(_Local(files))) {
				err = unix_err(EBADF);
				goto finish;
			}

			targ_ftab = &_Local(files)[*targ];

			(void)close_internal(targ_ftab);
		}
	
		targ_ftab->fp = fp;
		targ_ftab->flags = FD_FLAG_NONE;

		*targ = targ_ftab - _Local(files);

	    finish: ;
	}
	mutex_unlock( &_Local(lock) );

	if (err && fp) {
		--(fp->ref_cnt);
	}

	DEBUG1(ftab_debug,(0, "ftab::c-dup: %d->%d err=%s", source, *targ,
			  mach_error_string(err)));

	return err;
}

mach_error_t
ftab::ns_authenticate(ns_access_t access, ns_token_t t, usItem** obj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_duplicate(ns_access_t access, usItem** newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_set_times(time_value_t atime, time_value_t mtime)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_get_protection(ns_prot_t prot, int* protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_set_protection(ns_prot_t prot, int protlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_get_privileged_id(int* id)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_get_access(ns_access_t *access, ns_cred_t cred, int *credlen)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t
ftab::ns_get_manager(ns_access_t access, usItem **newobj)
{
  return MACH_OBJECT_NO_SUCH_OPERATION;
}

