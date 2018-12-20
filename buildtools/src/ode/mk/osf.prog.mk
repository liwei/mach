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
# $Log:	osf.prog.mk,v $
# Revision 2.3  93/03/20  00:37:00  mrt
# 	Added an echo "" to make long log files more readable.
# 	[93/01/07            mrt]
# 
# Revision 2.2  92/05/20  20:16:10  mrt
# 	First checkin
# 


.if !defined(_OSF_PROG_MK_)
_OSF_PROG_MK_=

#
#  Build rules
#
.if defined(PROGRAMS)

_LDLPATH_ = ${_CCTYPE_:S/host/LPATH="${_host_LPATH_}"; export LPATH;/:S/ansi//}
${PROGRAMS}: $${_OFILES_}
	@echo " "
	(${_LDLPATH_} ${_CC_} ${_LDFLAGS_} -o ${.TARGET}.X ${_OFILES_} ${_LIBS_})
	${MV} ${.TARGET}.X ${.TARGET}
.endif

.endif
