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
 * File:        emul_id.c
 *
 * Purpose:
 *	User space emulation of unix process management primitives
 *
 * HISTORY:
 * $Log:	emul_uid.cc,v $
 * Revision 2.5  94/07/08  16:57:26  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  92/07/05  23:25:38  dpj
 * 	Dump old tm_set_task_{user/group}_ids calls and replaces with
 * 	tm_change_task_auth.
 * 	[92/06/24  14:38:58  jms]
 * 
 * Revision 2.3  91/11/13  16:44:34  dpj
 * 	Removed references to libload.
 * 	[91/11/08            dpj]
 * 
 * Revision 2.2  91/11/06  11:33:09  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:38:00  pjg]
 * 
 * 	Upgraded to US39.
 * 	[91/04/16  18:30:38  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:30:56  pjg]
 * 
 * 	change_identity ifdef'd to ComplexIO.
 * 	fix args to set_reuid.
 * 	change_identity takes a uxident instead of a token.
 * 	fill in setregid(), setgroups() and getgroups().
 * 	[90/01/02  14:12:58  dorr]
 * 
 * 	initial checkin.
 * 	[89/12/18  15:51:35  dorr]
 * 
 * Revision 2.5  91/04/12  18:47:55  jjc
 * 	Replaced Paul Neves' EFAULT handling with macros for copying
 * 	arguments in and out of system calls to figure out whether
 * 	they're good or not.
 * 	[91/04/01            jjc]
 * 	Picked up Paul Neves' changes
 * 	[91/03/29  15:48:53  jjc]
 * 
 * Revision 2.4.1.1  91/02/05  15:35:12  neves
 * 	Inserted VALID_ADDRESS macro where appropriate.
 * 
 * Revision 2.4  90/12/19  11:04:22  jjc
 * 	Removed some old debugging stuff.
 * 	[90/12/07            jjc]
 * 	Changed emul_setre{uid,gid} to return without doing any work
 * 	if uxident_set_re{uid,gid} indicate that the IDs are set
 * 	correctly already.
 * 	Also, modified them to tell the task master when the uids or
 * 	gids change.
 * 	[90/11/14            jjc]
 * 	Changed debugging statements.
 * 	[90/11/12            jjc]
 * 	Convert emulation errors to Unix ones before returning.
 * 	Changed emul_getgroups() to return number of groups gotten
 * 	if successful.
 * 	Cleaned up.
 * 	[90/09/14            jjc]
 * 
 * Revision 2.3  90/03/21  17:21:19  jms
 * 	Grabed from dorr branch
 * 	[90/03/16  16:38:46  jms]
 * 
 * Revision 2.2  90/01/02  21:54:15  dorr
 * 	add set_regid(), setgroups() and getgroups().
 * 
 */

#include <us_byteio_ifc.h>
#include <us_name_ifc.h>
#include <uxident_ifc.h>
#include "emul_base.h"
#include "emul_proc.h"

extern "C" {
#include <base.h>
#include <debug.h>

#include <errno.h>

}


static int r_uid;
static int r_gid;
static int e_uid;
static int e_gid;

mach_error_t emul_getuid(syscall_val_t *rv)
{
	SyscallTrace("getuid");

	rv->rv_val1 = r_uid;
	rv->rv_val2 = e_uid;
	return ERR_SUCCESS;
}

mach_error_t emul_getgid(syscall_val_t *rv)
{
	SyscallTrace("getgid");

	rv->rv_val1 = r_gid;
	rv->rv_val2 = e_gid;
	return ERR_SUCCESS;
}

mach_error_t emul_setreuid(int ruid, int euid, syscall_val_t *rv)
{
	mach_error_t		err = ERR_SUCCESS;
	ns_token_t		token;

	SyscallTrace("setreuid");
	
	err = uxident_obj->uxident_set_reuid(ruid, euid);
	if (err) {
		/*
		 * If the uids are set already, skip rest of work
		 * and return success.
		 */
		if (err == US_OBJECT_EXISTS)
			err = ERR_SUCCESS;
		goto finish;
	}

	if (ruid != -1)
		r_uid = ruid;
	if (euid != -1)
		e_uid = euid;

	(void)uxident_obj->uxident_get_token(&token);
	(void)tm_task_obj->tm_change_task_auth(token);

	if (err)
		goto finish;


#if	ComplexIO
	err = emul_io_change_identity(uxident_obj);
	/* if (err) we're in trouble */
#endif	ComplexIO

finish:
	if (err) {
		if (err == US_INVALID_ACCESS)
			rv->rv_val1 = EPERM;
		else
			rv->rv_val1 = emul_error_to_unix(err);
	}
	else
		rv->rv_val1 = 0;

	return(err);
}


mach_error_t emul_setregid(int rgid, int egid, syscall_val_t *rv)
{
	mach_error_t		err;
	ns_token_t		token;

	SyscallTrace("setregid");
	
	err = uxident_obj->uxident_set_regid(rgid, egid);
	if (err) {
		/*
		 * If the gids are set already, skip rest of work
		 * and return success.
		 */
		if (err == US_OBJECT_EXISTS)
			err = ERR_SUCCESS;
		goto finish;
	}
	
	if (rgid != -1)
		r_gid = rgid;
	if (egid != -1)
		e_gid = egid;

	(void)uxident_obj->uxident_get_token(&token);
	(void)tm_task_obj->tm_change_task_auth(token);

	if (err)
		goto finish;

#if	ComplexIO
	err = emul_io_change_identity(uxident_obj);
	/* if (err) we're in trouble */
#endif	ComplexIO
	
    finish:
	if (err) {
		if (err == US_INVALID_ACCESS)
			rv->rv_val1 = EPERM;
		else
			rv->rv_val1 = emul_error_to_unix(err);
	}
	else
		rv->rv_val1 = 0;

	return(err);
}

mach_error_t emul_getgroups(int len, int *gidset, syscall_val_t *rv)
{
	mach_error_t		err;
	int			length;
	int			*set;

	SyscallTrace("getgroups");

	COPYOUT_INIT(set, gidset, len, int, rv, EFAULT);

	length = len;
	err = uxident_obj->uxident_get_groups(set, &length);
	
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = length;

	COPYOUT(set, gidset, len, int, rv, EFAULT);

	return(err);
}

mach_error_t emul_setgroups(int len, int *gidset, syscall_val_t *rv)
{
	mach_error_t		err;
	int			*set;

	SyscallTrace("setgroups");
	
	COPYIN(gidset, set, len, int, rv, EFAULT);

	err = uxident_obj->uxident_set_groups(set, len);
	if (err) goto finish;

#if	ComplexIO
	err = emul_io_change_identity(uxident_obj);
	/* if (err) we're in trouble */
#endif	ComplexIO

finish:	
	if (err)
		rv->rv_val1 = emul_error_to_unix(err);
	else
		rv->rv_val1 = 0;
	
	COPYIN_DONE(gidset, set, len, int, rv, EFAULT);

	return(err);
}

