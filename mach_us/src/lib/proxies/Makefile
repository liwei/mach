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

# File:        Makefile
#
# 
# Purpose:  Build file to create all of the Client side proxies
# 
# HISTORY:
# $Log:	Makefile,v $
# Revision 2.3  94/06/29  15:02:21  mrt
# 	Updated for odemake
# 	[94/01/12            mrt]
# 
# Revision 2.2  94/01/11  17:49:18  jms
# 	Initial Version
# 	[94/01/09  18:51:42  jms]
# 
#

DEPENDENCIES	=
SAVE_D		=

CONFIGURATION	= ${MACH3_CONFIGURATION:UMACH3_US}

SPEC_PROXY_IFCS	= tm_task_proxy_ifc.h tm_tgrp_proxy_ifc.h \
		  us_item_proxy_ifc.h usint_mf_proxy_ifc.h
DFLT_PROXY_IFCS = ${NAME_PROXY_NAME}_ifc.h ${SYS_PROXY_NAME}_ifc.h \
		  ${BYTEIO_PROXY_NAME}_ifc.h ${RECIO_PROXY_NAME}_ifc.h \
		  ${TTY_PROXY_NAME}_ifc.h ${TM_ROOT_PROXY_NAME}_ifc.h \
		  ${TM_TASK_PROXY_NAME}_ifc.h ${TM_TGRP_PROXY_NAME}_ifc.h \
		  ${EVENT_PROXY_NAME}_ifc.h ${NET_BASE_PROXY_NAME}_ifc.h \
		  ${NET_NAME_PROXY_NAME}_ifc.h ${NET_CONNECTOR_PROXY_NAME}_ifc.h \
		  ${NET_CLTS_PROXY_NAME}_ifc.h ${NET_CLTS_RECS_PROXY_NAME}_ifc.h \
		  ${NET_COTS_PROXY_NAME}_ifc.h ${NET_COTS_RECS_PROXY_NAME}_ifc.h \
		  ${NET_COTS_BYTES_PROXY_NAME}_ifc.h

INCLUDES	= ${SPEC_PROXY_IFCS} ${DFLT_PROXY_IFCS}
EXPDIR		= /us/include/
	
IDIR		= /us/lib/
LIBRARIES	= libproxies.a
EXPLIB_TARGETS	= export_libproxies.a
libproxies.a_EXPDIR = /us/lib/
ILIST		= ${LIBRARIES}

OFILES		=  \
		useventpxy.o \
		us_item_proxy.o us_name_pxy.o \
		us_byteio_pxy.o us_recio_pxy.o us_tty_pxy.o \
		netbasepxy.o netnamepxy.o netconnpxy.o \
		netcltspxy.o netcltsrpxy.o \
		netcotspxy.o netcotsbpxy.o netcotsrpxy.o \
		us_sys_proxy.o \
		usintmfpxy.o \
		ustmrootpxy.o ustmtaskpxy.o ustmtgrppxy.o \
		tm_task_proxy.o tm_tgrp_proxy.o

CPlusPlusFLAGS	= -DNeXT
VPATH		= ${UNIX_INCLUDE}

ustmrootpxy.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
ustmtaskpxy.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
ustmtgrppxy.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
tm_task_proxy.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
tm_tgrp_proxy.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
#usintmfpxy.o_CFLAGS	= ${CPlusPlusFLAGS} -I. -I${MAKETOP}lib/us++

tm_tgrp_proxy.o: us_tm_tgrp_proxy_ifc.h

#GARBAGE			= ${SHORTNAMES}


SHORTNAMES		= usintmfpxy.cc

usintmfpxy.cc_LONGNAME		= usint_mf_proxy.cc


${SHORTNAMES}: $${${.TARGET}_LONGNAME}
	${RM} -f ${.TARGET}
	${CP} ${.OODATE} ${.TARGET}


NAME_PROXY_NAME = us_name_proxy
NAME_SHORT_NAME = us_name_pxy.cc
NAME_CLASS_DEPS = us_name_ifc.h us_item_proxy_ifc.h
$(NAME_SHORT_NAME) $(NAME_PROXY_NAME)_ifc.h $(NAME_PROXY_NAME).cc : ${NAME_PROXY_NAME}
${NAME_PROXY_NAME}: $(NAME_CLASS_DEPS)
	${RM} -f $(NAME_PROXY_NAME).cc $(NAME_PROXY_NAME)_ifc.h $(NAME_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NAME_PROXY_NAME)
	${LN} $(NAME_PROXY_NAME).cc $(NAME_SHORT_NAME)
	touch ${NAME_PROXY_NAME}

SYS_PROXY_NAME = us_sys_proxy
SYS_SHORT_NAME = us_sys_pxy.cc
SYS_CLASS_DEPS = us_sys_ifc.h us_item_proxy_ifc.h
$(SYS_SHORT_NAME) $(SYS_PROXY_NAME)_ifc.h $(SYS_PROXY_NAME).cc : ${SYS_PROXY_NAME}
${SYS_PROXY_NAME}:  $(SYS_CLASS_DEPS)
	${RM} -f $(SYS_PROXY_NAME).cc $(SYS_PROXY_NAME)_ifc.h $(SYS_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(SYS_PROXY_NAME)
	${LN} $(SYS_PROXY_NAME).cc $(SYS_SHORT_NAME)
	touch ${SYS_PROXY_NAME}

BYTEIO_PROXY_NAME = us_byteio_proxy
BYTEIO_SHORT_NAME = us_byteio_pxy.cc
BYTEIO_CLASS_DEPS = us_byteio_ifc.h us_item_proxy_ifc.h
$(BYTEIO_SHORT_NAME) $(BYTEIO_PROXY_NAME)_ifc.h $(BYTEIO_PROXY_NAME).cc : ${BYTEIO_PROXY_NAME}
${BYTEIO_PROXY_NAME}: $(BYTEIO_CLASS_DEPS)
	${RM} -f $(BYTEIO_PROXY_NAME).cc $(BYTEIO_PROXY_NAME)_ifc.h $(BYTEIO_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(BYTEIO_PROXY_NAME)
	${LN} $(BYTEIO_PROXY_NAME).cc $(BYTEIO_SHORT_NAME)
	touch ${BYTEIO_PROXY_NAME}

RECIO_PROXY_NAME = us_recio_proxy
RECIO_SHORT_NAME = us_recio_pxy.cc
RECIO_CLASS_DEPS = us_recio_ifc.h us_item_proxy_ifc.h
$(RECIO_SHORT_NAME) $(RECIO_PROXY_NAME)_ifc.h $(RECIO_PROXY_NAME).cc : ${RECIO_PROXY_NAME}
${RECIO_PROXY_NAME}: $(RECIO_CLASS_DEPS)
	${RM} -f $(RECIO_PROXY_NAME).cc $(RECIO_PROXY_NAME)_ifc.h $(RECIO_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(RECIO_PROXY_NAME)
	${LN} $(RECIO_PROXY_NAME).cc $(RECIO_SHORT_NAME)
	touch ${RECIO_PROXY_NAME}

TTY_PROXY_NAME = us_tty_proxy
TTY_SHORT_NAME = us_tty_pxy.cc
TTY_CLASS_DEPS = us_tty_ifc.h us_byteio_proxy_ifc.h
$(TTY_SHORT_NAME) $(TTY_PROXY_NAME)_ifc.h $(TTY_PROXY_NAME).cc : ${TTY_PROXY_NAME}
${TTY_PROXY_NAME}: $(TTY_CLASS_DEPS)
	${RM} -f $(TTY_PROXY_NAME).cc $(TTY_PROXY_NAME)_ifc.h $(TTY_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(TTY_PROXY_NAME)
	${LN} $(TTY_PROXY_NAME).cc $(TTY_SHORT_NAME)
	touch ${TTY_PROXY_NAME}

TM_ROOT_PROXY_NAME = us_tm_root_proxy
TM_ROOT_SHORT_NAME = ustmrootpxy.cc
TM_ROOT_CLASS_DEPS = us_tm_root_ifc.h us_item_proxy_ifc.h
$(TM_ROOT_SHORT_NAME) $(TM_ROOT_PROXY_NAME)_ifc.h $(TM_ROOT_PROXY_NAME).cc : ${TM_ROOT_PROXY_NAME}
${TM_ROOT_PROXY_NAME}: $(TM_ROOT_CLASS_DEPS)
	${RM} -f $(TM_ROOT_PROXY_NAME).cc $(TM_ROOT_PROXY_NAME)_ifc.h $(TM_ROOT_SHORT_NAME)
	proxy_default -cpp ${_CPP_} -v ${DEF_CPPFLAGS} ${.ALLSRC} $(TM_ROOT_PROXY_NAME)
	${LN} $(TM_ROOT_PROXY_NAME).cc $(TM_ROOT_SHORT_NAME)
	touch ${TM_ROOT_PROXY_NAME}

TM_TASK_PROXY_NAME = us_tm_task_proxy
TM_TASK_SHORT_NAME = ustmtaskpxy.cc
TM_TASK_CLASS_DEPS = us_tm_task_ifc.h us_item_proxy_ifc.h
$(TM_TASK_SHORT_NAME) $(TM_TASK_PROXY_NAME)_ifc.h $(TM_TASK_PROXY_NAME).cc : ${TM_TASK_PROXY_NAME}
${TM_TASK_PROXY_NAME}: $(TM_TASK_CLASS_DEPS)
	${RM} -f $(TM_TASK_PROXY_NAME).cc $(TM_TASK_PROXY_NAME)_ifc.h $(TM_TASK_SHORT_NAME)
	proxy_default -cpp ${_CPP_} -v ${DEF_CPPFLAGS} ${.ALLSRC} $(TM_TASK_PROXY_NAME)
	${LN} $(TM_TASK_PROXY_NAME).cc $(TM_TASK_SHORT_NAME)
	touch ${TM_TASK_PROXY_NAME}

TM_TGRP_PROXY_NAME = us_tm_tgrp_proxy
TM_TGRP_SHORT_NAME = ustmtgrppxy.cc
TM_TGRP_CLASS_DEPS = us_tm_tgrp_ifc.h us_name_proxy_ifc.h
$(TM_TGRP_SHORT_NAME) $(TM_TGRP_PROXY_NAME)_ifc.h $(TM_TGRP_PROXY_NAME).cc : ${TM_TGRP_PROXY_NAME}
${TM_TGRP_PROXY_NAME}: $(TM_TGRP_CLASS_DEPS)
	${RM} -f $(TM_TGRP_PROXY_NAME).cc $(TM_TGRP_PROXY_NAME)_ifc.h $(TM_TGRP_SHORT_NAME)
	proxy_default -cpp ${_CPP_} -v ${DEF_CPPFLAGS} ${.ALLSRC} $(TM_TGRP_PROXY_NAME)
	${LN} $(TM_TGRP_PROXY_NAME).cc $(TM_TGRP_SHORT_NAME)
	touch ${TM_TGRP_PROXY_NAME}

EVENT_PROXY_NAME = us_event_proxy
EVENT_SHORT_NAME = useventpxy.cc
EVENT_CLASS_DEPS = us_event_ifc.h us_item_proxy_ifc.h
$(EVENT_SHORT_NAME) $(EVENT_PROXY_NAME)_ifc.h $(EVENT_PROXY_NAME).cc : ${EVENT_PROXY_NAME}
${EVENT_PROXY_NAME}: $(EVENT_CLASS_DEPS)
	${RM} -f $(EVENT_PROXY_NAME).cc $(EVENT_PROXY_NAME)_ifc.h $(EVENT_SHORT_NAME)
	proxy_default -cpp ${_CPP_} -v ${DEF_CPPFLAGS} ${.ALLSRC} $(EVENT_PROXY_NAME)
	${LN} $(EVENT_PROXY_NAME).cc $(EVENT_SHORT_NAME)
	touch ${EVENT_PROXY_NAME}

NET_BASE_PROXY_NAME = us_net_base_proxy
NET_BASE_SHORT_NAME = netbasepxy.cc
NET_BASE_CLASS_DEPS = us_net_base_ifc.h us_item_proxy_ifc.h
$(NET_BASE_SHORT_NAME) $(NET_BASE_PROXY_NAME)_ifc.h $(NET_BASE_PROXY_NAME).cc : ${NET_BASE_PROXY_NAME}
${NET_BASE_PROXY_NAME}: $(NET_BASE_CLASS_DEPS)
	${RM} -f $(NET_BASE_PROXY_NAME).cc $(NET_BASE_PROXY_NAME)_ifc.h $(NET_BASE_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_BASE_PROXY_NAME)
	${LN} $(NET_BASE_PROXY_NAME).cc $(NET_BASE_SHORT_NAME)
	touch ${NET_BASE_PROXY_NAME}

NET_NAME_PROXY_NAME = us_net_name_proxy
NET_NAME_SHORT_NAME = netnamepxy.cc
NET_NAME_CLASS_DEPS = us_net_name_ifc.h us_name_proxy_ifc.h us_net_base_proxy_ifc.h
$(NET_NAME_SHORT_NAME) $(NET_NAME_PROXY_NAME)_ifc.h $(NET_NAME_PROXY_NAME).cc : ${NET_NAME_PROXY_NAME}
${NET_NAME_PROXY_NAME}: $(NET_NAME_CLASS_DEPS)
	${RM} -f $(NET_NAME_PROXY_NAME).cc $(NET_NAME_PROXY_NAME)_ifc.h $(NET_NAME_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_NAME_PROXY_NAME)
	${LN} $(NET_NAME_PROXY_NAME).cc $(NET_NAME_SHORT_NAME)
	touch  ${NET_NAME_PROXY_NAME}

NET_CONNECTOR_PROXY_NAME = us_net_connector_proxy
NET_CONNECTOR_SHORT_NAME = netconnpxy.cc
NET_CONNECTOR_CLASS_DEPS = us_net_connector_ifc.h us_net_base_proxy_ifc.h
$(NET_CONNECTOR_SHORT_NAME) $(NET_CONNECTOR_PROXY_NAME)_ifc.h $(NET_CONNECTOR_PROXY_NAME).cc : ${NET_CONNECTOR_PROXY_NAME}
${NET_CONNECTOR_PROXY_NAME}: $(NET_CONNECTOR_CLASS_DEPS)
	${RM} -f $(NET_CONNECTOR_PROXY_NAME).cc $(NET_CONNECTOR_PROXY_NAME)_ifc.h $(NET_CONNECTOR_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_CONNECTOR_PROXY_NAME)
	${LN} $(NET_CONNECTOR_PROXY_NAME).cc $(NET_CONNECTOR_SHORT_NAME)
	touch  ${NET_CONNECTOR_PROXY_NAME}

NET_CLTS_PROXY_NAME = us_net_clts_proxy
NET_CLTS_SHORT_NAME = netcltspxy.cc
NET_CLTS_CLASS_DEPS = us_net_clts_ifc.h us_net_connector_proxy_ifc.h
$(NET_CLTS_SHORT_NAME) $(NET_CLTS_PROXY_NAME)_ifc.h $(NET_CLTS_PROXY_NAME).cc : ${NET_CLTS_PROXY_NAME}
${NET_CLTS_PROXY_NAME}: $(NET_CLTS_CLASS_DEPS)
	${RM} -f $(NET_CLTS_PROXY_NAME).cc $(NET_CLTS_PROXY_NAME)_ifc.h $(NET_CLTS_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_CLTS_PROXY_NAME)
	${LN} $(NET_CLTS_PROXY_NAME).cc $(NET_CLTS_SHORT_NAME)
	touch ${NET_CLTS_PROXY_NAME}

NET_CLTS_RECS_PROXY_NAME = us_net_clts_recs_proxy
NET_CLTS_RECS_SHORT_NAME = netcltsrpxy.cc
NET_CLTS_RECS_CLASS_DEPS = us_net_clts_recs_ifc.h us_net_clts_proxy_ifc.h us_recio_proxy_ifc.h
$(NET_CLTS_RECS_SHORT_NAME) $(NET_CLTS_RECS_PROXY_NAME)_ifc.h $(NET_CLTS_RECS_PROXY_NAME).cc : ${NET_CLTS_RECS_PROXY_NAME}
${NET_CLTS_RECS_PROXY_NAME}: $(NET_CLTS_RECS_CLASS_DEPS)
	${RM} -f $(NET_CLTS_RECS_PROXY_NAME).cc $(NET_CLTS_RECS_PROXY_NAME)_ifc.h $(NET_CLTS_RECS_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_CLTS_RECS_PROXY_NAME)
	${LN} $(NET_CLTS_RECS_PROXY_NAME).cc $(NET_CLTS_RECS_SHORT_NAME)
	touch ${NET_CLTS_RECS_PROXY_NAME}

NET_COTS_PROXY_NAME = us_net_cots_proxy
NET_COTS_SHORT_NAME = netcotspxy.cc
NET_COTS_CLASS_DEPS = us_net_cots_ifc.h us_net_base_proxy_ifc.h
$(NET_COTS_SHORT_NAME) $(NET_COTS_PROXY_NAME)_ifc.h $(NET_COTS_PROXY_NAME).cc : ${NET_COTS_PROXY_NAME} 
${NET_COTS_PROXY_NAME}: $(NET_COTS_CLASS_DEPS)
	${RM} -f $(NET_COTS_PROXY_NAME).cc $(NET_COTS_PROXY_NAME)_ifc.h $(NET_COTS_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_COTS_PROXY_NAME)
	${LN} $(NET_COTS_PROXY_NAME).cc $(NET_COTS_SHORT_NAME)
	touch ${NET_COTS_PROXY_NAME}

NET_COTS_BYTES_PROXY_NAME = us_net_cots_bytes_proxy
NET_COTS_BYTES_SHORT_NAME = netcotsbpxy.cc
NET_COTS_BYTES_CLASS_DEPS = us_net_cots_bytes_ifc.h us_net_cots_proxy_ifc.h us_byteio_proxy_ifc.h
$(NET_COTS_BYTES_SHORT_NAME) $(NET_COTS_BYTES_PROXY_NAME)_ifc.h $(NET_COTS_BYTES_PROXY_NAME).cc : ${NET_COTS_BYTES_PROXY_NAME}
${NET_COTS_BYTES_PROXY_NAME}: $(NET_COTS_BYTES_CLASS_DEPS)
	${RM} -f $(NET_COTS_BYTES_PROXY_NAME).cc $(NET_COTS_BYTES_PROXY_NAME)_ifc.h $(NET_COTS_BYTES_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_COTS_BYTES_PROXY_NAME)
	${LN} $(NET_COTS_BYTES_PROXY_NAME).cc $(NET_COTS_BYTES_SHORT_NAME)
	touch ${NET_COTS_BYTES_PROXY_NAME}

NET_COTS_RECS_PROXY_NAME = us_net_cots_recs_proxy
NET_COTS_RECS_SHORT_NAME = netcotsrpxy.cc
NET_COTS_RECS_CLASS_DEPS = us_net_cots_recs_ifc.h  us_net_cots_proxy_ifc.h us_recio_proxy_ifc.h
$(NET_COTS_RECS_SHORT_NAME) $(NET_COTS_RECS_PROXY_NAME)_ifc.h $(NET_COTS_RECS_PROXY_NAME).cc : ${NET_COTS_RECS_PROXY_NAME}
${NET_COTS_RECS_PROXY_NAME}: $(NET_COTS_RECS_CLASS_DEPS)
	${RM} -f $(NET_COTS_RECS_PROXY_NAME).cc $(NET_COTS_RECS_PROXY_NAME)_ifc.h $(NET_COTS_RECS_SHORT_NAME)
	proxy_default -cpp ${_CPP_} ${DEF_CPPFLAGS} ${.ALLSRC} $(NET_COTS_RECS_PROXY_NAME)
	${LN} $(NET_COTS_RECS_PROXY_NAME).cc $(NET_COTS_RECS_SHORT_NAME)
	touch  ${NET_COTS_RECS_PROXY_NAME}

.include <${RULES_MK}>
