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
# HISTORY
# $Log:	Makefile,v $
# Revision 1.2  94/06/29  15:00:49  mrt
# 	Updated for odemake
# 	[93/12/16            mrt]
# 
# Revision 1.1  89/03/20  11:14:56  dorr
# Initial revision
#
# These are the include files that should be exported to users of
# the multi-server system

######################################################################

EXPDIR		= /us/include/
IDIR		= /us/include/
ILIST		= ${INCLUDES}

INCLUDES	= auth_defs.h base.h class_info_ifc.h \
		  clone_ifc.h \
		  diag_defs.h exception_error.h hash.h \
		  interrupt.h io_error.h io_types.h \
		  io_types2.h mach_object_error.h method_info_ifc.h \
		  net_error.h net_types.h ns_error.h ns_types.h \
		  sig_error.h sunrpc_error.h syscall_val.h \
		  timer.h tm_types.h top_ifc.h us_byteio_ifc.h \
		  us_error.h us_event_ifc.h us_io_ifc.h us_item_ifc.h \
		  us_name_ifc.h us_net_base_ifc.h us_net_clts_ifc.h \
		  us_net_clts_recs_ifc.h us_net_connector_ifc.h \
		  us_net_cots_bytes_ifc.h us_net_cots_ifc.h \
		  us_net_cots_recs_ifc.h us_net_name_ifc.h \
		  us_recio_ifc.h us_sys_ifc.h us_tm_root_ifc.h \
		  us_tm_task_ifc.h us_tm_tgrp_ifc.h us_tty_ifc.h


.include <${RULES_MK}>


