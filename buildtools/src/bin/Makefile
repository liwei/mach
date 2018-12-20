# 
# Mach Operating System
# Copyright (c) 1991,1990 Carnegie Mellon University
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
# Revision 2.3  93/06/16  17:04:48  mrt
# 	Added bnrutils to SUBDIRS for builds on a native
# 	BNR environment.
# 	[93/06/14            mrt]
# 
# Revision 2.2  93/03/20  11:15:24  mrt
# 	Created
# 	[93/01/14            mrt]
# 

.if ${host_context} == "i386_bnr"
SUBDIRS		= bnrutils
EXPBIN_SUBDIRS	= bnrutils
.else
SUBDIRS		= xstrip
EXPBIN_SUBDIRS	= xstrip
.endif

.include <${RULES_MK}>
