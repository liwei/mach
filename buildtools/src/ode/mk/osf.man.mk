#
# Distributed as part of the Mach Operating System
#
#
# @OSF_FREE_COPYRIGHT@
# 
# Copyright (c) 1990, 1991
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
# HISTORY
# $Log:	osf.man.mk,v $
# Revision 2.2  92/05/20  20:16:03  mrt
# 	First checkin
# 
# 	$EndLog$
# 	[92/05/20  14:50:21  mrt]
# 

.if !defined(_OSF_MAN_MK_)
_OSF_MAN_MK_=

#
# default nroff program to run
#
NROFF?=nroff

#
# default flags to nroff
#
DEF_NROFFFLAGS?=-man -h

#
# all flags for nroff
#
_NROFFFLAGS_=${${.TARGET}_DEF_NROFFFLAGS:U${DEF_NROFFFLAGS}} ${${.TARGET}_NROFFENV:U${NROFFENV}} ${${.TARGET}_NROFFFLAGS:U${NROFFFLAGS}} ${${.TARGET}_NROFFARGS:U${NROFFARGS}}

#
#  Default single suffix compilation rules
#
.SUFFIXES: .0 .${MANSECTION} .man

#
#  Default double suffix compilation rules
#
.man.${MANSECTION}:
	${SED} -e '/^\.\\"/d'\
	 ${.IMPSRC} > ${.TARGET}.X
	${MV} -f ${.TARGET}.X ${.TARGET}


.man.0:
	${NROFF} ${_NROFFFLAGS_} ${.IMPSRC} > ${.TARGET}.X
	${MV} -f ${.TARGET}.X ${.TARGET}

.endif
