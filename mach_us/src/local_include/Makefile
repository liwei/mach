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
#	local_include - needed by servers and proxies, but not the emulation library
# 
# HISTORY
# $Log:	Makefile,v $
# Revision 1.2  94/06/29  15:02:36  mrt
# 	updated for odemake
# 	[93/12/16            mrt]
# 
# Revision 1.1  89/03/20  11:14:42  dorr
# Initial revision
#
######################################################################

EXPDIR			= /us/include/
EXPORT_USING_TAR	=

SUBDIRS			= i386_mach
EXPINC_SUBDIRS		= ${SUBDIRS}

# Export local version of chtreads.h
VPATH			= ../lib/threads

OTHERS			= machine
INCLUDES		= base_ifc.h cthreads.h debug.h dll.h \
			  dlong.h fs_types.h logging.h \
			  macro_help.h net_internal.h queue.h \
			  stdarg.h us_ports.h us_statistics.h ${OTHERS}

.include <${RULES_MK}>

machine:
	${RM} ${_RMFLAGS_} machine
	${LN} -s ${KERN_MACHINE_DIR} machine

