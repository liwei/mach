# 
# Mach Operating System
# Copyright (c) 1992 Carnegie Mellon University
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
# $Log:	osf.mig.mk,v $
# Revision 2.2  93/03/20  00:10:28  mrt
# 	Fixed USEMIGGCPP for gcc.2.3.2
# 	[93/01/10            mrt]
# 
# 	Created
# 	[92/08/14            mrt]
# 

.if !defined(_OSF_MIG_MK_)
_OSF_MIG_MK_=

DEF_MIGFLAGS	?= -MD
MIG		?= mig
USEMIGCPP 	?= -cc "${ansi_CPP}"


_MIGFLAGS_=\
	${USEMIGCPP}\
	${DEF_MIGFLAGS}\
	${${.TARGET}_MIGENV:U${MIGENV}}\
	${${.TARGET}_MIGFLAGS:U${MIGFLAGS}}\
	${${.TARGET}_MIGARGS:U${MIGARGS}}\
	${_CC_NOSTDINC_} ${_GENINC_} ${_CC_INCDIRS_}

_MIGH_DEF_ = ${${.TARGET}_MIGHDEF:U${.TARGET:.h=.defs}}
${MIG_HDRS}: $${_MIGH_DEF_}
	@echo ""
	${MIG} ${_MIGFLAGS_} \
		-header ${.TARGET} -server /dev/null -user /dev/null \
		${${_MIGH_DEF_}:P} \

_MIGU_DEF_ = ${${.TARGET}_MIGUDEF:U${.TARGET:S/User.c/.defs/}}
${MIG_USRS}: $${_MIGU_DEF_}
	@echo ""
	${MIG} ${_MIGFLAGS_} \
		-user ${.TARGET} -server /dev/null \
		${${_MIGU_DEF_}:P} 

_MIGS_DEF_ = ${${.TARGET}_MIGSDEF:U${.TARGET:S/Server.c/.defs/}}
${MIG_SRVS}: $${_MIGS_DEF_}
	@echo ""
	${MIG} ${_MIGFLAGS_} \
		-header /dev/null -user /dev/null -server ${.TARGET} \
		${${_MIGS_DEF_}:P} 
.endif
