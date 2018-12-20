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
# 
# Purpose:  Build the MachUS utilities
# 
# HISTORY: 
# $Log:	Makefile,v $
# Revision 2.4  94/10/27  12:01:05  jms
# 	Add the "us_ps" program.  Prints out exec strings for process ids.
# 	[94/10/26  14:25:08  jms]
# 
# Revision 2.3  94/07/25  15:10:37  mrt
# 	Added init_env.template.csh
# 	[94/07/25            mrt]
# 
# Revision 2.2  94/06/29  15:23:42  mrt
# 	Moved from utils.
# 
# 
# Revision 1.28  94/01/11  18:12:32  jms
# 	Install the "start_net" script
# 	[94/01/10  13:59:31  jms]
# 
# Revision 1.27  93/01/20  17:40:48  jms
# 	Add us_restart and kemul scripts
# 	Remove old libcs.a hack
# 	[93/01/18  17:59:16  jms]
# 
# Revision 1.26  92/07/05  23:37:17  dpj
# 	Added libux for fsadmin, needed for task_master test code.
# 	[92/07/05  19:03:34  dpj]
# 
# 	Add proxy_default script
# 	Enable install of scripts alone to be done first thing on bootstrap
# 	[92/06/24  18:22:48  jms]
# 	Removed libmachobj++.
# 	[92/05/10  01:34:37  dpj]
# 
# 
#

CONFIGURATION		= MACH3_VUS

PROGRAMS		= fsadmin us_ps
SCRIPTS			= proxy_default kemul start_net.template init_env.template
# Scripts that we do not want the comments stripped from 
OTHERS			= multi us_setup.template STARTUP.fsadmin.template


EXPBIN_TARGETS		= export_proxy_default
EXPDIR			= /bin/
IDIR			= /us/bin/
ILIST			= ${PROGRAMS} ${SCRIPTS} ${OTHERS}
STARTUP.fsadmin.template_IDIR	= /us/etc/
start_net.template_IDIR = /us/etc/
us_setup.template_IDIR	= /us/etc/
init_env.template_IDIR	= /us/etc/

LIBS			= -lux -lproxies -lus++ ${LIBTHREADS} -lcs \
			  ${LIBMACH3}

fsadmin.o_CFLAGS	=  -I${UNIX_INCLUDE}
us_ps.o_CFLAGS		=  -I${UNIX_INCLUDE}
VPATH			= ${UNIX_INCLUDE}

.include <${RULES_MK}>

# Hack to remove "-stdinc". Must be read after RULES_MK
DEF_CPlusPlusFLAGS	:= ${DEF_CPlusPlusFLAGS:S/-nostdinc//}


${OTHERS}: ${.TARGET:S/$/.csh/}
	${CP} -p ${${.TARGET:S/$/.csh/}:P} ${.TARGET}
	${CHMOD} +x ${.TARGET}
