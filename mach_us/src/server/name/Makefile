
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
######################################################################
# HISTORY
# $Log:	Makefile,v $
# Revision 1.11  94/06/29  15:03:41  mrt
# 	Updated for odemake. Condensed history
# 	[93/11/28            mrt]
# 
# Revision 1.9  92/07/05  23:34:18  dpj
# 	Removed libmachobj++.
# 	[92/05/10  01:28:34  dpj]
# 
# Revision 1.7  90/11/27  18:21:13  jms
# 	Make MACH3_US the default configuration.
# 	Add use of MACH3_CONFIGURATION environment variable
# 	[90/11/20  14:43:55  jms]
# 
# 
#
######################################################################

DEPENDENCIES	=
SAVE_D		=

CONFIGURATION	=  ${MACH3_CONFIGURATION:UMACH3_US}

IDIR		= /us/bin/

PROGRAMS	= pathnameserver
ILIST		= pathnameserver

OFILES		= name_init.o

LIBS 		= -lproxies -lus++ ${LIBTHREADS} ${LIBMACH3}

INCFLAGS	= -I${UNIX_INCLUDE}

.include <${RULES_MK}>


