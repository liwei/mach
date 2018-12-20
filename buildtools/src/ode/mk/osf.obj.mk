#
# Distributed as part of the Mach Operating System
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

#
# HISTORY
# $Log:	osf.obj.mk,v $
# Revision 2.4  93/05/11  18:04:34  mrt
# 	Added _OFILES_P_ for profiled objects.
# 	[93/05/10            mrt]
# 
# Revision 2.3  93/03/20  00:36:56  mrt
# 	Added an echo "" to the rules so that long log files
# 	could be read easier.
# 	[93/01/07            mrt]
# 
# Revision 2.2  92/05/20  20:16:04  mrt
# 	First checkin
# 


.if !defined(_OSF_OBJ_MK_)
_OSF_OBJ_MK_=

#
# definitions for compilation
#
_ALL_OFILES_=${PROGRAMS:D${PROGRAMS:@.PROG.@${${.PROG.}_OFILES:U${.PROG.}.o}@}:U${OFILES}}
_OFILES_=${${.TARGET}_OFILES:U${OFILES:U${.TARGET}.o}}
_OFILES_P_= ${${.TARGET:S/_p.a$/.a/}_OFILES:S/.o$/_p.o/g:U${OFILES:S/.o$/_p.o/g}}

#
#  Default double suffix compilation rules
#

.c.o:
	@echo " "
	${_CC_} -c ${_CCFLAGS_} ${.IMPSRC}

.y.o:
	@echo " "
	${YACC} ${_YFLAGS_} ${.IMPSRC}
	${_CC_} -c ${_CCFLAGS_} y.tab.c
	-${RM} -f y.tab.c
	${MV} -f y.tab.o ${.TARGET}

.y.c:
	@echo " "
	${YACC} ${_YFLAGS_} ${.IMPSRC}
	${MV} -f y.tab.c ${.TARGET}
	${RM} -f y.tab.h

.y.h:
	@echo " "
	${YACC} -d ${_YFLAGS_} ${.IMPSRC}
	${MV} -f y.tab.h ${.TARGET}
	${RM} -f y.tab.c

.l.o:
	@echo " "
	${LEX} ${_LFLAGS_} ${.IMPSRC}
	${_CC_} -c ${_CCFLAGS_} lex.yy.c
	-${RM} -f lex.yy.c
	${MV} -f lex.yy.o ${.TARGET}

.l.c:
	@echo " "
	${LEX} ${_LFLAGS_} ${.IMPSRC}
	${MV} -f lex.yy.c ${.TARGET}

.c.pp:
	@echo " "
	${_CC_} -E ${_CCFLAGS_} ${.IMPSRC} > ${.TARGET}

.if defined(OFILES) || defined(PROGRAMS)
${_ALL_OFILES_}: ${HFILES}
.endif

.endif
