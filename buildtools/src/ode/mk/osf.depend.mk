#
# Distributed as part of the Mach Operating System
#
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

#
# HISTORY
# $Log:	osf.depend.mk,v $
# Revision 2.3  93/03/20  00:36:43  mrt
# 	Jan-6-93  Mary Thompson (mrt) at Carnegie-Mellon University
# 	Created a .depend link to depend.mk since md and make
# 	don't use the same defaults.
# 	Added the -I flags to MD_FLAGS and made sure that these
# 	directories were also added to VPATH so that make could
# 	find the relative dependencies.
# 
# Revision 2.2  92/05/20  20:15:52  mrt
# 	First checkin
# 


.if !defined(_OSF_DEPEND_MK_)
_OSF_DEPEND_MK_=

#CMU-CS md DEF_MDFLAGS?=-u depend.mk -f -d
#ODE md DEF_MDFLAGS=-rm
DEF_MDFLAGS?=-std

MD_SUFFIX = d

# The following flags are for the OSF's version of md.
# Any directories
# appearing before the -I- should be on VPATH so that make can
# find the relative dependencies that are generated. The directories
# /usr/include and /usr/cs/include are probably on the CPATH in a Mach
# system and we want to see if we found anything there, but not generate
# a dependency

.if defined(VPATH)
_VPATH_ 	:= ${VPATH:S/:/ /g}
_VFLAGS_	= ${_VPATH_:S/^/-I/g}
.else
_VFLAGS_ 	=
.endif
_GENVFLAGS_	= ${_VFLAGS_:!${GENPATH} ${_VFLAGS_}!} \
		   ${_GENINC_} ${_CC_INCDIRS_}

# Be sure the same directories that are stripped off of the
# the dependencies are searched for to find the dependencies

VPATH	+= ${_GENINC_:S/-I//g} ${_CC_INCDIRS_:S/-I//g}

# Get cpath values to be recognized as standard places
# (appear after the -I- flag) and generate commented out
# dependencies (-K flag)

TSD 		= ${CPATH:S/:/ /g}
STD_INCDIRS	= ${TSD:S/^/-I/g} ${TSD:S/^/-K/g}

_MDFLAGS_=\
	${${.TARGET}_DEF_MDFLAGS:U${DEF_MDFLAGS}}\
	${_GENVFLAGS_} -I- \
	${STD_INCDIRS} \
	${${.TARGET}_MDFLAGS:U${MDFLAGS}}\
	${${.TARGET}_MDENV:U${MDENV}}\
	${${.TARGET}_MDARGS:U${MDARGS}}

nodep_all:
	${RM} ${_RMFLAGS_} depend.mk

.if !empty(.TARGETS:Mnodep_*.o)
${TARGETS:Mnodep_*.o}:
	${RM} -f ${.TARGET:S/^nodep_//} ${.TARGET:S/^nodep_//:.o=.d}
 	echo "${.TARGET:S/^nodep_//}: ${.TARGET:S/^nodep_//}" \
		> ${.TARGET:S/^nodep_//:.o=.d}
.endif

# md generates a depend.mk file, make includes .depend
.if !defined(TOSTAGE)
.EXIT:
	-@${MD} ${_MDFLAGS_} .
	-@${RM} -f .depend
	-@${LN} -s depend.mk .depend
.if defined(SAVE_D)
	-@[ -d tmpd ] || mkdir tmpd
	-@mv *.d tmpd
.else
	@${RM} -f *.d
.endif
.endif

.endif
