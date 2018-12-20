# 
# Mach Operating System
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
# HISTORY
# 31-Dec-91  Mary Thompson (mrt) at Carnegie-Mellon University
#	Updated to work with CMU Makefile-common
#
# $Log:	Make.inc,v $
#

LIBMACH= -lmach
LIBTHREADS = -lthreads
LIBCPU = -lcpu
DEP_FILE = ./Makedep
MD_SUFFIX = d

VAX_M=
IBMRT_M=
SUN_M=
SUN4_M=
PMAX_M=-Dpmax
I386_M=

STRIP = ${NO_STRIP?"":-s}
DEF_CFLAGS=${DEF_CFLAGS?${DEF_CFLAGS}:-O -MD $($(TARGET_MACHINE)_M)}
DEF_MIGFLAGS =    -MD
_MIGFLAGS_=${${@}_MIGFLAGS?${${@}_MIGFLAGS}:${MIGFLAGS}} ${${@}_MIGENV?${${@}_MIGENV}:${MIGENV}} ${${@}_MIGARGS?${${@}_MIGARGS}:${MIGARGS}}  ${${@}_DEF_MIGFLAGS?${${@}_DEF_MIGFLAGS}:${DEF_MIGFLAGS}}

MIG=mig
MD=md
CPP=${CPP?${CPP}:`wh -Lq cpp`}

RELEASE=release

_MIG_HDRS_=${MIG_DEFS:.defs=.h}
_MIG_USRS_=${MIG_DEFS:.defs=User.c}
_MIG_SRVS_=${MIG_DEFS:.defs=Server.c}

_MIG_HDRS_DEPS_=${${@}_DEPS?${${@}_DEPS}:${@:.h=.defs}}
_MIG_USRS_DEPS_=${${@}_DEPS?${${@}_DEPS}:${@:User.c=.defs}}
_MIG_SRVS_DEPS_=${${@}_DEPS?${${@}_DEPS}:${@:Server.c=.defs}}

#DOC_OBJS	= $(DOC:=.ps) $(DOC:=.doc)
DOC_MSS_OBJS	= $(FSRC:.fsrc=.mss)
INSTALL_INCS	= $(INCS:=.install)
#INSTALL_DOCS	= $(DOC:=.install) $(TDOC:=.install)

include  ../../../Makefile-common


$(TARGET_DIRS):
	mkdir $@

${_MIG_HDRS_}: $${_MIG_HDRS_DEPS_}
	${MIG} ${_MIGFLAGS_} ${_MIG_HDRS_DEPS_} \
		-server /dev/null -user /dev/null
	md -u $(DEP_FILE) -d $(@:.h=.d)

${_MIG_USRS_}: $${_MIG_USRS_DEPS_}
	${MIG} ${_MIGFLAGS_} ${_MIG_USRS_DEPS_} \
		 -server /dev/null
	md -u $(DEP_FILE) -d $(@:User.c=.d)

${_MIG_SRVS_}: $${_MIG_SRVS_DEPS_}
	${MIG} ${_MIGFLAGS_} ${_MIG_SRVS_DEPS_} \
		 -user /dev/null
	md -u $(DEP_FILE) -d $(@:Server.c=.d)


.EXIT:
	${MD} -u ${DEP_FILE} -f -d *.d



