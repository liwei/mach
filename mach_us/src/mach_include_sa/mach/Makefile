# 
# Mach Operating System
# Copyright (c) 1993-1988 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon 
# the rights to redistribute these changes.
#  
#  
# HISTORY
# $Log:	Makefile,v $
# Revision 2.2  94/06/29  15:02:55  mrt
# 	Created for odemake.
# 	[93/12/17            mrt]
# 
#

EXPDIR		= /us/include/mach/

EXPINC_SUBDIRS	= ${KERN_MACHINE_DIR}

EXPORT_USING_TAR=

MACH_INCLUDES=	boolean.h error.h exception.h \
		kern_return.h mach.h mach_traps.h \
		mach_types.h machine.h memory_object.h \
		message.h mig_errors.h mig_support.h msg_type.h \
		notify.h port.h std_types.h syscall_sw.h \
		task_info.h task_special_ports.h thread_info.h \
		thread_special_ports.h thread_status.h \
		time_value.h vm_inherit.h \
		vm_prot.h vm_statistics.h vm_attributes.h \
		host_info.h mach_param.h policy.h \
		processor_info.h thread_switch.h \
		mach_types.defs std_types.defs

MACH_DEFS=	exc.defs mach.defs \
		mach_host.defs mach_port.defs notify.defs \
		memory_object.defs memory_object_default.defs \
		default_pager_types.defs default_pager.defs

MACH_HDRS=	exc.h mach_interface.h memory_object_user.h \
		mach_host.h mach_port.h \
		memory_object_default.h \
		default_pager_types.h default_pager.h

INCLUDES	= ${MACH_INCLUDES} ${MACH_DEFS} ${MACH_HDRS} machine

.include <${RULES_MK}>

machine: ${ALWAYS}
	${RM} ${_RMFLAGS_} machine
	${LN} -s ${KERN_MACHINE_DIR} machine
