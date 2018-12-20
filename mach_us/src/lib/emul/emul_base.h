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
 * File:        emul_base.h
 *
 * Purpose:
 *	
 *	base include file for user space emulation library
 *
 * HISTORY: 
 * $Log:	emul_base.h,v $
 * Revision 1.23  94/07/08  16:56:39  mrt
 * 	Updated copyrights.
 * 
 * Revision 1.22  91/11/13  16:35:10  dpj
 * 	Removed references to libload and the proc_obj.
 * 	[91/11/08            dpj]
 * 
 * Revision 1.21  91/11/06  11:29:20  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:41:51  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:21:16  pjg]
 * 
 * Revision 1.19  91/04/12  18:46:55  jjc
 * 	Replaced Paul Neves' EFAULT handling with macros for copying
 * 	arguments in and out of system calls to figure out whether
 * 	they're good or not.
 * 	[91/04/01            jjc]
 * 	Picked up Paul Neves' changes.
 * 	[91/03/29  15:42:22  jjc]
 * 
 * Revision 1.18.1.1  91/01/25  10:22:34  neves
 * 	Added VALID_ADDRESS macro for system call EFAULT
 * 	for bad user-provided addresses generation.
 * 
 * Revision 1.20  91/05/05  19:24:10  dpj
 * 	Changed the formal parameters to the COPY* macros to avoid
 * 	conflicts with existing identifiers.
 * 	Merged up to US39.
 * 	[91/04/30            dpj]
 * 	Refined debugging control for syscall entry/exit.
 * 	[91/04/28  09:36:27  dpj]
 * 
 * Revision 1.18  90/11/27  18:17:39  jms
 * 	No Change
 * 	[90/11/19  22:39:00  jms]
 * 
 * 	About to add tty changes from trunk
 * 	[90/11/08  13:38:36  jms]
 * 
 * Revision 1.17  90/10/02  11:32:46  mbj
 * 	Dropped unused TTYServer conditionals.
 * 	[90/09/12  13:43:03  mbj]
 * 
 * Revision 1.16  90/07/09  17:01:49  dorr
 * 	get rid of alerts.  add a proc_object.
 * 	[90/02/23  14:39:48  dorr]
 * 
 * 	switch to DEBUG[012].  add emul_debug and syscall_trace stuff.
 * 	[90/01/11  11:29:34  dorr]
 * 	No Further Changes
 * 	[90/07/06  14:30:38  jms]
 * 
 * Revision 1.15  90/01/02  21:36:44  dorr
 * 	conditionalize.  make all common objects external
 * 	to avoid incorrect library archive behavior.
 * 
 * Revision 1.14.1.3  90/01/02  14:06:24  dorr
 * 	add KernelFileIO default value.
 * 	add uxident_obj and access_obj.
 * 
 * Revision 1.14.1.2  89/12/19  17:04:39  dorr
 * 	checkin before christmas
 * 
 * Revision 1.14.1.1  89/12/18  15:55:19  dorr
 * 	get rid of unnecessary include files.
 * 	conditionalize.
 * 	base conditionalization on what type of
 * 	emulation is being done.
 * 	make object common definitions into external
 * 	declarations to facilitate librarization.
 * 
 * Revision 1.14  89/07/19  11:35:25  dorr
 * 	get rid of emul_objects.  add file table object.
 * 
 * Revision 1.12.1.1  89/06/21  15:55:13  dorr
 * 	make file table an object.
 * 
 * Revision 1.13  89/07/09  14:17:04  dpj
 * 	Removed dbg_port.
 * 	[89/07/08  12:34:09  dpj]
 * 
 * Revision 1.12  89/05/17  16:12:17  dorr
 * 	include file cataclysm
 * 
 * Revision 1.11.1.1  89/05/15  11:31:32  dorr
 * 	add global uxprot object (to hold our identity as well
 * 	as a placeholder for uxprot conversion operations
 * 
 * Revision 1.11  89/03/17  12:22:50  sanzi
 * 	conditionally compile syscall trace messages.
 * 	[89/03/11  23:24:33  dorr]
 * 	
 * 	declare prefix object and umask prot structures.
 * 	[89/02/25  13:54:05  dorr]
 * 
 */

#ifndef	_EMUL_BASE_H
#define	_EMUL_BASE_H

#ifndef	ASSEMBLER

#ifdef __cplusplus

#include <std_name_ifc.h>
#include <clone_master_ifc.h>
#include <fs_access_ifc.h>
//#include <net_prot_ifc.h>

#include <uxident_ifc.h>
//#include <uxprot_ifc.h>
#include <uxstat_ifc.h>
#include <uxsignal_ifc.h>
#include <ftab_ifc.h>

#include <emul_all_ifc.h>
#include <emul_basic_ifc.h>
#include <emul_error_ifc.h>
#include <emul_misc_ifc.h>
#include <emul_io_ifc.h>


extern "C" {
#endif __cplusplus

#include <base.h>
#include <mach/std_types.h>
#include <ns_types.h>

#include <syscall_val.h>

#ifdef __cplusplus
#include <syscalls_prototypes.h>
}
#endif __cplusplus

#endif	ASSEMBLER

#include "emul_config.h"

/*  define which semantics emulation incorporates */
#if	(emul_type_umount)
#define	StdName	0
#define	Signals	0
#define	ComplexIO 0
#define TaskMaster 0
#define	Sockets 0
#define	FileIO 0
#else
#define	KernelFileIO 1
#endif	Umount

/* overridable configuration options */
#ifndef	StdName
#define	StdName 1
#endif	StdName
#ifndef	TaskMaster
#define	TaskMaster 1
#endif	TaskMaster
#ifndef	Signals
#define Signals 1
#endif	Signals
#ifndef	Sockets
#define Sockets 1
#endif	Sockets
#ifndef	ComplexIO
#define	ComplexIO 1
#endif	ComplexIO
#ifndef	FileOps
#define FileOps 1
#endif	Fileops
#ifndef	FullProcessEmulation
#define	FullProcessEmulation 1
#endif	FullProcessEmulation
#ifndef	KernelFileIO
#define	KernelFileIO 1
#endif	KernelFileIO


#define	SyscallTrace(x)	\
	DEBUG0(emul_debug&EMUL_DEBUG_SYSTRACE,(Diag,"emulation-syscall: %s\n", x));

#define INIT_UMASK	022

#ifndef	ASSEMBLER
extern ns_prot_t		umask_prot;	/* umask */
extern int			umask_protlen; 

#ifdef __cplusplus

clone_master*	clone_master_obj;	/* global clone object */

std_name*	prefix_obj;	/* global prefix table */
uxident*	uxident_obj;	/* global identity object */
//uxprot*	uxprot_obj;	/* global protection object */
fs_access*	access_obj;	/* global uid-space */
ftab*		ftab_obj;	/* file table */
//net_prot*	net_protocol_obj; /* network protocol object */
uxstat*		uxstat_obj;	/* stat object */
uxsignal*	uxsignal_obj;	/* signal object */

#endif __cplusplus

boolean_t		emul_debug;
boolean_t		syscall_trace;

/*
 * Macros for copying in and out arguments to emulated system calls
 *
 * These macros may or may not really copy anything.  They may just
 * check for legal addresses, or touch every page just to make sure
 * there is really some memory there.  If they really copy something,
 * they must allocate memory for what they copy into or out of.
 * Usually, what you copy into or out of can be just be statically
 * allocated, but these macros are supposed to be very generic.
 */
#ifdef __cplusplus
extern "C" {
#endif __cplusplus

#include <mach/machine/vm_param.h>	/* for VM_{MIN,MAX}_ADDRESS */

#ifdef __cplusplus
}
#endif __cplusplus

typedef	char	*copyinstr_t;

#define	COPYIN(_from, _to, _count, _type, _rvp, _errcode)	\
{								\
	if ((char *)(_from) == NULL || (vm_offset_t)(_from) < VM_MIN_ADDRESS \
	    || (vm_offset_t)(_from) >= VM_MAX_ADDRESS) {	\
		(_rvp)->rv_val1 = (_errcode);			\
		return (unix_err((_errcode)));			\
	}							\
	(_to) = (_from);					\
}

#define	COPYIN_DONE(_from, _to, _count, _type, _rvp, _errcode)


#define	COPYOUT_INIT(_from, _to, _count, _type, _rvp, _errcode)	\
{								\
	if ((char *)(_to) == NULL || (vm_offset_t)(_to) < VM_MIN_ADDRESS \
	    || (vm_offset_t)(_to) >= VM_MAX_ADDRESS) {		\
		(_rvp)->rv_val1 = (_errcode);			\
		return (unix_err((_errcode)));			\
	}							\
	(_from) = (_to);					\
}

#define	COPYOUT(_from, _to, _count, _type, _rvp, _errcode)


#define	COPYINSTR(_from, _to, _len, _rvp, _errcode)		\
{								\
	if ((char *)(_from) == NULL || (vm_offset_t)(_from) < VM_MIN_ADDRESS \
	    || (vm_offset_t)(_from) >= VM_MAX_ADDRESS) { 	\
		(_rvp)->rv_val1 = (_errcode);			\
		return (unix_err((_errcode)));			\
	}							\
	(_to) = (copyinstr_t)(_from);				\
}

#define	COPYINSTR_DONE(_from, _to, _len, _rvp, _errcode)



#define VALID_ADDRESS(_ptr, _rvp, _errcode)			\
{								\
	if ((char *)(_ptr) == NULL || (vm_offset_t)(_ptr) < VM_MIN_ADDRESS \
	    || (vm_offset_t)(_ptr) >= VM_MAX_ADDRESS) { 	\
		(_rvp)->rv_val1 = (_errcode);			\
		return (unix_err((_errcode)));			\
        }							\
}

extern int			emul_debug;
#define	EMUL_DEBUG_SYSTRACE		0x1
#define	EMUL_DEBUG_SYSENTER		0x2
#define	EMUL_DEBUG_SYSEXIT		0x4

#endif	ASSEMBLER

#endif	_EMUL_BASE_H
