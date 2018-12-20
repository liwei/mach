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
# HISTORY
# $Log:	Makefile,v $
# Revision 1.11  94/06/29  15:03:32  mrt
# 	Updated for odemake. Condensed history.
# 	[94/04/25            mrt]
# 
# Revision 1.10  92/03/05  15:10:58  jms
# 	Change configuration from MACH3_UNIX to MACH3_VUS.
# 	Stop building "las".  Not Used.
# 	[92/02/26  19:04:03  jms]
# 
# Revision 1.9  91/11/06  14:17:46  jms
# 	Put asdatabase in the release/lib
# 	[91/11/04  17:43:10  jms]
# 
# Revision 1.3  89/03/30  12:06:54  dpj
# 	Added asdatabase.install target.
# 	Modified cas_db.o to be generated with explicit DATABASEDIR 
# 	specification.
# 	[89/03/26  18:57:56  dpj]
# 
# 

DEPENDENCIES		=
SAVE_D			=

MAKETOP			?= ../../}

# Chose MACH3_VUS because it may try to read the asdatbase info
# from stdin if no file is given.

CONFIGURATION		= MACH3_VUS

DEFS			= ${EXPORTBASE}/us/include/servers

# Causes ${DEFS} to be put on VPATH
INCFLAGS		= -I${DEFS}

PROGRAMS		= cas encrypt_db 
IDIR			= /us/bin/
asdatabase_IDIR		= /us/lib/

ILIST			= ${PROGRAMS} asdatabase

MIG_HDRS		= auth.h c_auth.h
MIG_SRVS		= authServer.c c_authServer.c
OTHERS			= ${MIG_HDRS} asdatabase

cas_OFILES		= cas_db.o cas_main.o cas_procs.o cas_utils.o \
			  clas_procs.o newdes.o authServer.o c_authServer.o

encrypt_db_OFILES	= encrypt_db.o cas_utils.o newdes.o

astest_OFILES		= astest.o 

LIBS			= ${LIBMACH3}


DATABASEDIR 		= /afs/cs.cmu.edu/project/mach/us/common

CFLAGS 			=  -DUseStaticMsgType=0 -DDATABASEDIR=\"$(DATABASEDIR)\"

.include <${RULES_MK}>

test:			$(ASTEST)


asdatabase:	asdatabase.src encrypt_db
	${RM} -f ${.TARGET}
	./encrypt_db ${asdatabase.src:P} > ${.TARGET}


