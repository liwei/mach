#
# Distributed as part of the Mach Operating System
#
# 
# Mach Operating System
# Copyright (c) 1993,1992 Carnegie Mellon University
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
# HISTORY
# $Log:	osf.cmu_machus.mk,v $
# Revision 2.2  94/06/30  14:27:14  mrt
# 	Created.
# 	[94/06/30            mrt]
# 

.if !defined(CONFIGURATION) || empty(CONFIGURATION)
    COMMON_DEFINES ?= -DSHARED_DATA_TIMING_EQUIVALENCE=1
.else
    COMMON_DEFINES ?= -DSHARED_DATA_TIMING_EQUIVALENCE=1 -D${CONFIGURATION}=1
.endif

DEBUGFLAGS	?= -D_DEBUG_=1
_MACH3_DEFINES_	= -DMACH -DCMU -DCMUCS -DMACH3=1 -DMACH_IPC_COMPAT=0 ${COMMON_DEFINES}


# Moved -g from DEBUGFLAGS to CC_OPT_LEVEL to keep it away from cpp since
# some version of cpp die on it.
CC_OPT_LEVEL	?= -O -g
DEF_CCFLAGS	?= -traditional  -MD ${DEBUGFLAGS} ${_MACH3_DEFINES_} 
DEF_CPlusPlusFLAGS ?=  -MD -nostdinc ${DEBUGFLAGS} ${_MACH3_DEFINES_}
DEF_MIGFLAGS	?= -MD ${_MACH3_DEFINES_} 
DEF_CPPFLAGS	?= ${DEBUGFLAGS} ${_MACH3_DEFINES_} -I${MACH3_INCLUDES}
# same as _CCFLAGS_ except for losing CC_OPT_LEVEL
${TARGET_MACHINE}_CPP_FLAGS ?=\
	${_CC_CFLAGS_}\
	${${.TARGET}_CENV:U${CENV}}\
	${${.TARGET}_CFLAGS:U${CFLAGS}} ${TARGET_FLAGS}\
	${${.TARGET}_CARGS:U${CARGS}}\
	${_CC_NOSTDINC_} ${_GENINC_} ${_CC_INCDIRS_} ${_CC_PICLIB_}
INCFLAGS	+= -I${MACH3_INCLUDES}
#CC-E		?= ${CC} -E ${DEF_CPPFLAGS}}

#CRT0+			= /afs/cs/project/mach-5/jms/i386_env/g++/lib/crt0+.o

${TARGET_MACHINE}_CPlusPlus	?= ${${TARGET_MACHINE}_CC}
${HOST_MACHINE}_CPlusPlus	?= ${${HOST_MACHINE}_CC}
ansi_CPlusPlus		= ${${TARGET_MACHINE}_CPlusPlus}
host_CPlusPlus		= ${${HOST_MACHINE}_CPlusPlus}
_CPlusPlus_		= ${${_CCTYPE_}_CPlusPlus}

${TARGET_MACHINE}_CPlusPlusFLAGS	?=
_CPlusPlusFLAGS_=\
	${DEF_CPlusPlusFLAGS}\
	${_CC_OL_}\
	${${.TARGET}_CENV:U${CPlusPlusENV}}\
	${${.TARGET}_CFLAGS:U${CPlusPlusFLAGS}} ${TARGET_FLAGS}\
	${${.TARGET}_CARGS:U${CPlusPlusARGS}}\
	${_CC_NOSTDINC_} ${_GENINC_} ${_CC_INCDIRS_} 


#LIBCPlusPlus	= -lc++
_MACH3_US_LIBS_	= -lmach3_us -lc++ -lgcc
_MACH3_VUS_LIBS_= -lmach3_vus -lc++ -lgcc -lc
.if !defined(CONFIGURATION) || empty(CONFIGURATION)
LIBMACH3	= -lmach3_us
.else
LIBMACH3	= ${_${CONFIGURATION}_LIBS_}
.endif

.SUFFIXES: .cc .o

.cc.o:
	@echo ""
	${_CPlusPlus_} -c ${_CPlusPlusFLAGS_} ${.IMPSRC}

.include <osf.mach3.mk>
