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
# HISTORY: 
# $Log:	Makefile,v $
# Revision 1.27  94/06/29  15:04:36  mrt
# 	Updated for odemake. Condensed history
# 	[93/11/28            mrt]
# 

# Revision 1.26  94/05/17  13:35:57  jms
# 	Compile new file for keeping track of num of tasks in a session.
# 	[94/05/11  14:43:17  modh]
# 
# Revision 1.25  94/01/11  18:11:28  jms
# 	Use the new "proxies" dir/lib
# 	[94/01/10  13:30:33  jms]
# 
# Revision 1.24  93/01/20  17:39:23  jms
# 	Commented out "malloc" libs line
# 	[93/01/18  17:35:40  jms]
# 
# Revision 1.23  92/07/05  23:35:31  dpj
# 	tm_agency.o => tm_root.o
# 	tm_job_group* => tm_task_group*
# 	Add tm_task_timer.o
# 	[92/06/24  17:27:29  jms]
# 
#
######################################################################
DEPENDENCIES	=
SAVE_D		=

CONFIGURATION	= ${MACH3_CONFIGURATION:UMACH3_US}

IDIR		= /us/bin/

PROGRAMS	= task_master
ILIST		= task_master

OFILES		= tm_main.o \
		  tm_root.o \
		  tm_session.o \
		  tm_task.o \
		  tm_task_group.o \
		  tm_task_timer.o \
		  tm_mapping.o

# Load with -lmalloc when looking for memory leaks.
.if defined(DEBUG)
DEBUG_LIB	= -lmalloc
.else
DEBUG_LIB	=
.endif

LIBS 	= ${DEBUG_LIB} -lproxies -lus++ ${LIBTHREADS} ${LIBMACH3}  ${LIBCS} 

CFLAGS		= -DTaskMaster=1 -DComplexIO=1 -DAuthentication=1 \

CPlusPlusFLAGS	= ${CFLAGS} 

INCFLAGS	= -I${UNIX_INCLUDE}
# tm_mapping.c is now being built with UNIX_INCLUDE

.include <${RULES_MK}>
