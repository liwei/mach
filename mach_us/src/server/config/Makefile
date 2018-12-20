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
######################################################################
# HISTORY
# $Log:	Makefile,v $
# Revision 2.6  94/06/29  15:03:35  mrt
# 	Updated for odemake.
# 	[94/03/02            mrt]
# 
# Revision 2.3  91/11/06  14:17:49  jms
# 	Add the "sys_agency.o"
# 	[91/11/04  17:43:59  jms]
# 
# Revision 2.2  91/10/07  00:12:37  jjc
# 	Added snames files, nprocs.o  and snames.o, to list of OFILES.
# 	Defined DATAFILES to be prefix.config and rc.us and install them in lib and
# 	bin, respectively.
# 	[91/10/02  21:55:48  jjc]
# 
# 	Created.
# 	[91/06/28            jjc]
# 
#
######################################################################

DEPENDENCIES	=
SAVE_D		=

# Chose MACH3_VUS since the diag server is not running yet.
# Assume the bootstrap server is providing
# the ability to write on the console via the unix write trap

CONFIGURATION	=  MACH3_VUS

IDIR		= /us/etc/
ILIST		= ${PROGRAMS} ${DATAFILES}


PROGRAMS	= config_server
config_server_IDIR = /us/bin/

OFILES		= config_server.o sys_agency.o nprocs.o snames.o

LIBS		= -lproxies -lus++ ${LIBTHREADS} ${LIBMACH3}

DATAFILES	= prefix.config.template rc.us.template

.include <${RULES_MK}>
