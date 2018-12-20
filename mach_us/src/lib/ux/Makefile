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

# File:        Makefile
#
# 
# Purpose:  Build the various Server independent, UNIX specific support
#	classes.  Mostly used by the emulation lib.
# 
# HISTORY:
# $Log:	Makefile,v $
# Revision 2.15  94/06/29  15:02:33  mrt
# 	Updated for odemake. Removed ancient history.
# 	[94/02/24            mrt]
# 
# Revision 2.14  94/01/11  17:50:23  jms
# 	Use .../lib/proxies.
# 	Add uxselect.o
# 	[94/01/09  19:44:31  jms]
# 

DEPENDENCIES	=
SAVE_D		=

CONFIGURATION	= ${MACH3_CONFIGURATION:UMACH3_US}

LIBRARIES	= libux.a

INCLUDES	= devnull_ifc.h uxio.h uxio_socket_ifc.h \
		  uxstat_ifc.h ftab_ifc.h uxio_dir_ifc.h uxio_tty_ifc.h \
		  ux_exception.h uxio_ifc.h uxselect_ifc.h \
		  uxident_ifc.h uxio_pipe_ifc.h uxsignal_ifc.h

EXPDIR		= /us/include/
IDIR		= /us/lib/
ILIST		= ${LIBRARIES}
EXPLIB_TARGETS	= export_libux.a
libux.a_EXPDIR	= /us/lib/


MACH3_COMMON_OFILES = \
		uxio.o \
		uxstat.o \
		uxident.o \
		uxsignal.o \
		uxio_pipe.o \
		uxio_tty.o \
		uxio_socket.o \
		uxio_dir.o \
		uxselect.o \
		devnull.o

#		uxdir.o 
#		uxprot.o 

MACH3_VUS_OFILES = \
		ftab.o \
		kio.o \
		${MACH3_COMMON_OFILES}			


MACH3_US_OFILES	= \
		ftab.o \
		${MACH3_COMMON_OFILES}

OFILES = ${${CONFIGURATION}_OFILES}

CPlusPlusFLAGS	= -I${UNIX_INCLUDE}
VPATH		= ${UNIX_INCLUDE}

.include <${RULES_MK}>



