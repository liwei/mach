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
# File:        Makefile
#
# HISTORY:
# $Log:	Makefile,v $
# Revision 2.2  94/06/30  14:26:57  mrt
# 	Created.
# 	[94/06/30            mrt]
# 
# 


DATAFILES	= osf.cmu_machus.mk osf.cmu_machus.passes.mk

EXPBIN_TARGETS	= ${DATAFILES:S/^/export_/g}
ILIST		= ${DATAFILES}
IDIR		= /lib/mk/

project_name	= mach3

.include <${RULES_MK}>
