# 
# Mach Operating System
# Copyright (c) 1993,1992 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
# any improvements or extensions that they make and grant Carnegie the
# rights to redistribute these changes.

#
# 
# HISTORY
# $Log:	Makefile,v $
# Revision 2.2  94/07/13  16:38:07  mrt
# 	copied here from the user area
# 
# Revision 2.14  93/04/14  11:42:53  mrt
# 	Updated for ODE make
# 	[92/08/24            mrt]
# 

DEPENDENCIES	=

LIBS 		= ${LIBMACHID} ${LIBSERVICE} ${LIBNETNAME} ${LIBMACH} ${LIBCMUCS}

PROGRAMS 	= machid

ILIST 		= machid

IDIR 		= /us/bin/

machid_OFILES	= machid.o machid_procs.o machid_debug_procs.o \
		  machid_dpager_procs.o machid_notify_procs.o

# dump -traditional flag that is defined in cmu_machus DEF_CCFLAGS
DEF_CCFLAGS	= -MD -DMACH -DCMU -DCMUCS

.include	<${RULES_MK}>
