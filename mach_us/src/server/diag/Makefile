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
######################################################################
# HISTORY
# $Log:	Makefile,v $
# Revision 1.9  94/06/29  15:03:38  mrt
# 	Updated for odemake. Condensed history
# 	[94/03/02            mrt]
# 
# Revision 1.8  92/03/05  15:12:29  jms
# 	Change from MACH3_UNIX => MACH3_VUS
# 	[92/02/26  19:25:50  jms]
#
######################################################################
DEPENDENCIES		=
SAVE_D			=

# Chose MACH3_VUS since we can't direct our output to
# the diag server. Assume the bootstrap server is providing
# the ability to write on the console via the unix write trap

CONFIGURATION		= MACH3_VUS

IDIR			= /us/bin/

PROGRAMS		= diag_serv
ILIST			= diag_serv

DEFS			= ${EXPORTBASE}/us/include/servers

# Also causes ${DEFS} to be added to VPATH
INCFLAGS		= -I${DEFS}

MIG_SRVS		= mach_diagServer.c

OFILES			= diag_main.o mach_diagServer.o

# XXX Bad circular dependencies: 
#		diag_main needs server_loop needs cthread_fork
LIBS		= ${LIBTHREADS} ${LIBMACH3}

.include <${RULES_MK}>

