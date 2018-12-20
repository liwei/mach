#
# Distributed as part of the Mach Operating System
#
#
# Copyright (c) 1990, 1991, 1992  
# Open Software Foundation, Inc. 
#  
# Permission is hereby granted to use, copy, modify and freely distribute 
# the software in this file and its documentation for any purpose without 
# fee, provided that the above copyright notice appears in all copies and 
# that both the copyright notice and this permission notice appear in 
# supporting documentation.  Further, provided that the name of Open 
# Software Foundation, Inc. ("OSF") not be used in advertising or 
# publicity pertaining to distribution of the software without prior 
# written permission from OSF.  OSF makes no representations about the 
# suitability of this software for any purpose.  It is provided "as is" 
# without express or implied warranty. 
#
#
# ODE 2.1.1

# HISTORY
# $Log:	osf.lib.mk,v $
# Revision 2.5  93/05/14  23:14:30  mrt
# 	Moved .s_p.o rule to osf.mach3.mk along with the rest of
# 	the assembly rules.
# 	[93/05/14            mrt]
# 
# Revision 2.4  93/05/11  18:04:22  mrt
# 	Added rules to build profiled libraries.
# 	[93/05/08            mrt]
# 
# 	Added a .ORDER target to LIBRARIES to keep multiple ranlibs
# 	from being done at the same time when multiple libraries
# 	are being built in one directory.
# 	[93/04/22            mrt]
# 
# Revision 2.3  93/03/20  00:36:46  mrt
# 	Added rule for LIB_LONG_NAMES case for use by most Mach
# 	libaries.
# 	[92/12/31            mrt]
# 
# Revision 2.2  92/05/20  20:15:57  mrt
# 	First checkin


.if !defined(_OSF_LIB_MK_)
_OSF_LIB_MK_=


.if defined(LIBRARIES) || defined(LIBRARIES_P)
#
#  Build rules
#
.if defined(ORDER_LIBRARIES)
COFF_LORDER=`lorder *.o | tsort`
AIXCOFF_LORDER=`lorder *.o | tsort`
A_OUT_LORDER=${_OFILES_}
MACHO_LORDER=${_OFILES_}
.endif

#_LIBRARY_OFILES_=${_OFILES_:@.OF.@${.TARGET}(${.OF.})@}
#${LIBRARIES}: $${_LIBRARY_OFILES_}

.ORDER:	${_LIBRARIES_}

.if defined(LIBRARIES_P)
.SUFFIXES:	_p.o

.c_p.o:
	@echo " "
	${_CC_} -c ${_CCFLAGS_} -pg -g -DGPROF  ${.IMPSRC} -o ${.TARGET}


.endif

.if defined(LIB_LONG_NAMES)
#
#  This is rule for libaries where the component file names
#  are long enough to be truncated by ar. To solve the problem
#  of partially rearchived libraries having multiple copies of
#  objects with truncated names, we just keep all the .o files
#  around a re-archive the whole library each time.
#
${LIBRARIES}: $${_OFILES_} MAKELIB
${LIBRARIES_P}: $${_OFILES_P_} MAKELIB

MAKELIB: .USE
	${RM} -f ${.TARGET}
	${_AR_} cq ${.TARGET} ${.ALLSRC}
	${_RANLIB_} ${.TARGET}
	@echo "${_RANLIB_} ${.TARGET} done"
.else
${LIBRARIES}: $${.TARGET}($${_OFILES_}) MAKELIB
${LIBRARIES_P}: $${.TARGET}($${_OFILES_P_}) MAKELIB

MAKELIB: .USE
	@echo ""
	@echo .OODATE ${.OODATE}
	${_AR_} ${DEF_ARFLAGS} ${.TARGET} ${.OODATE}
.if defined(ORDER_LIBRARIES)
	${RM} -rf tmp
	mkdir tmp
	cd tmp && { \
	    ${_AR_} x ../${.TARGET}; \
	    ${RM} -f __.SYMDEF __________ELELX; \
	    ${_AR_} cr ${.TARGET} ${${OBJECT_FORMAT}_LORDER}; \
	}
	${MV} -f tmp/${.TARGET} .
	${RM} -rf tmp
.endif
	${_RANLIB_} ${.TARGET}
	@echo "${_RANLIB_} ${.TARGET} done"
	${RM} -f ${.OODATE}

.endif

.endif

.if defined(SHARED_LIBRARIES)

${SHARED_LIBRARIES}: $${_OFILES_}
	${_LD_} ${_SHLDFLAGS_} -o ${.TARGET}.X ${_OFILES_} ${_LIBS_}
	${MV} ${.TARGET}.X ${.TARGET}

.endif

.endif
