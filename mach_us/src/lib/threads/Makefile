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
# Makefile
#
# $Source: /afs/cs.cmu.edu/project/mach3-rcs/us/lib/threads/Makefile,v $
#
# 
# Purpose:  Makefile for threads package
# 
# HISTORY:
# $Log:	Makefile,v $
# Revision 1.23  94/06/29  15:02:26  mrt
# 	Updated for odemake. Changed to build only threads version
# 	of library. Removed old history
# 	[94/02/04            mrt]
# 
# Revision 1.22  94/05/17  14:06:27  jms
# 	Waste INLINE awk crud
# 	Misc random KP
# 	Needed -x assembler-with-cpp for 2.3.3 g++ compiler -modh
# 	[94/04/28  18:21:28  jms]
# 
# Revision 1.21  94/01/11  17:49:39  jms
# 	Add cthread_fakes.c
# 	[94/01/09  19:29:04  jms]

DEPENDENCIES	=
VPATH		= ${target_cpu}
LIBRARIES	= libthreads.a
EXPLIB_TARGETS	= export_libthreads.a
EXPDIR		= /us/lib/

IDIR		= /us/lib/
ILIST		= ${LIBRARIES}

IMPL		= MTHREAD
CONFIGURATION	= ${MACH3_CONFIGURATION:UMACH3_US}

CFLAGS	 	= -D${IMPL}

INLINE_FILES	= sync.o cthreads.o cthread_data.o  mig_support.o \
		  malloc.o stack.o

OFILES		= cprocs.o cthreads.o malloc.o \
		  mig_support.o stack.o stack_base.o sync.o \
		  time.o unix.o fork.o \
		  thread.o cthread_data.o \
		  cthread_fakes.o lock.o ${INLINE_FILES}



.include <${RULES_MK}>




