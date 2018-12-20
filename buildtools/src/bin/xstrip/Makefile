# 
# Mach Operating System
# Copyright (c) 1993-1990 Carnegie Mellon University
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
# Revision 2.3  93/03/23  10:16:08  mrt
# 	Added DEPENDENCIES. Assume this is built after the ode tools.
# 	[93/03/23            mrt]
# 
# Revision 2.2  93/02/03  18:00:03  mrt
# 	Updated for ODE make.
# 	[93/02/03            mrt]
# 
# Revision 2.7  92/03/05  22:49:22  rpd
# 	Changed to use Makefile-man.
# 	[92/02/29            rpd]
# 
# Revision 2.6  92/01/22  22:52:54  rpd
# 	Revised for new Makefile organization.
# 	[92/01/17            rpd]
# 


PROGRAMS	= xstrip
MANPAGES	= xstrip
MANSECTION	= 1

DEPENDENCIES	=

EXPBIN_TARGETS	= export_xstrip

ILIST		= xstrip xstrip.${MANSECTION} xstrip.0
IDIR		= /bin/
xstrip.${MANSECTION}_IDIR	= /man/man${MANSECTION}/
xstrip.0_IDIR	= /man/cat${MANSECTION}/

.include <${RULES_MK}>
