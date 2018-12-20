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
# Revision 1.39  94/06/30  15:00:28  mrt
# 	Added mk directory to EXPBIN_SUBDIRS and SUBDIRS
# 	[94/06/30            mrt]
# 
# Revision 1.38  94/06/29  15:00:46  mrt
# 	Updated for odemake. Removed old history.
# 	[94/01/10            mrt]
# 

CONFIGURATION		=

EXPBIN_SUBDIRS		= mk bin
EXPINC_SUBDIRS		= include local_include mach_include_sa \
			  sa_include unix_include lib server

EXPLIB_SUBDIRS		= lib pkg

SUBDIRS			= mk include lib bin pkg server 



.include <${RULES_MK}>
