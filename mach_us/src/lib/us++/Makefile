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
# Purpose:  Build the various server and OS independent MachUS classes
# 
# HISTORY:
# $Log:	Makefile,v $
# Revision 2.7  94/06/29  15:02:30  mrt
# 	Updated for odemake. Cleaned up some history
# 	[94/01/12            mrt]
# 
# Revision 2.6  94/05/17  14:07:00  jms
# 	No more awk inline lock stuff
# 	[94/04/28  18:39:18  jms]
# 
# Revision 2.5  94/01/11  17:49:49  jms
# 	Move proxies out to .../lib/proxies
# 	add us_tty.o
# 	[94/01/09  19:33:01  jms]
# 
# Revision 2.4  93/01/20  17:37:40  jms
# 	Add CPP_FLAGS to proxy_default.
# 	Add null_pager.o logging.o
# 	[93/01/18  16:26:32  jms]
# 
# Revision 2.3  92/07/05  23:26:26  dpj
# 	Auto-generate taskmaster and event default proxies with "proxy_default"
# 		script
# 
# 	New objects:	tmp_prop.o, us_tm_{root,task,tgrp}_[proxy_].o
# 			tm_tgrp_proxy.o
# 
# 	Dead objs:	us_task.o, us_task_proxy.o, tm_jgrp_proxy.o
# 
# 	Add mess'o short names
# 	[92/06/24  15:19:46  jms]
# 	Reorganized for new library structure:
# 		- new vn_classes
# 		- no more active_object
# 		- usint_mf
# 		- usClone
# 	[92/06/24  15:57:17  dpj]
# 
# 	Eliminated diag_format().
# 	Added support for inlining some files.
# 	[92/05/10  00:45:31  dpj]
# 
# Revision 2.2  91/11/06  13:33:22  jms
# 	C++ revision - upgraded to US41
# 	[91/09/27  15:07:01  pjg]
# 

DEPENDENCIES	=
SAVE_D		=

CONFIGURATION	= ${MACH3_CONFIGURATION:UMACH3_US}

EXPDIR		= /us/include/

INCLUDES	= access_table_ifc.h agency_ifc.h agent_ifc.h \
		  bytestream_ifc.h clone_master_ifc.h \
		  default_iobuf_mgr_ifc.h diag_ifc.h \
		  dir_ifc.h fs_access_ifc.h fs_auth_ifc.h \
		  fs_cred_ifc.h iobuf_user_ifc.h mf_mem_ifc.h \
		  mf_mgr_ifc.h mf_user_ifc.h mountpt_ifc.h \
		  net_dir_base_ifc.h net_endpt_base_ifc.h \
		  null_pager_ifc.h pager_base_ifc.h recordstream_ifc.h \
		  rpcmgr_ifc.h std_auth_ifc.h std_cred_ifc.h \
		  std_ident_ifc.h std_name_ifc.h std_prot_ifc.h \
		  stream_base_ifc.h symlink_ifc.h tmp_agency_ifc.h \
		  tmp_dir_ifc.h tmp_prop_ifc.h tsymlink_ifc.h \
		  usint_mf_ifc.h usint_mfio_ifc.h vn_agency_ifc.h \
		  vn_dir_ifc.h vn_file_ifc.h vn_mgr_ifc.h \
		  vn_pager_ifc.h vn_symlink_ifc.h vn_tsymlink_ifc.h \
		  vol_agency_ifc.h


LIBRARIES	= libus++.a
EXPLIB_TARGETS	= export_libus++.a
libus++.a_EXPDIR = /us/lib/

ILIST		= ${LIBRARIES}
IDIR		= /us/lib/

OFILES		= diag.o \
		top.o remote.o class_info.o method_info.o \
		rpcmgr.o interrupt.o clone.o \
		us_item.o us_name.o us_byteio.o us_recio.o \
		us_net_base.o us_net_name.o us_net_conn.o \
		us_net_clts.o us_net_cots.o \
		us_tm_root.o us_tm_task.o us_tm_tgrp.o \
		us_event.o us_tty.o \
		netcltsrecs.o netcotsrecs.o netcotsbytes.o \
		agent.o agency.o class_map.o  clone_master.o \
		access_table.o \
		std_cred.o std_name.o std_prot.o std_auth.o std_ident.o \
		vol_agency.o dir.o symlink.o tsymlink.o mountpt.o \
		stream_base.o bytestream.o recordstream.o \
		tmp_prop.o tmp_agency.o net_dir_base.o net_end_base.o \
		df_iobuf_mgr.o iobuf_user.o \
		io_types.o net_types.o \
		us_sys.o \
		fs_access.o fs_cred.o fs_auth.o \
		vn_agency.o vn_mgr.o vn_symlink.o vn_tsymlink.o \
		vn_dir.o vn_file.o vn_pager.o \
		mf_mem.o mf_user.o mf_mgr.o \
		usint_mf.o \
		pager_base.o null_pager.o \
		tmp_dir.o \
		logging.o

INLINE_FILES	= top.o remote.o class_info.o method_info.o rpcmgr.o \
		  diag.o agent.o agency.o us_item.o

OFILES += ${INLINE_FILES}

CPlusPlusFLAGS = -DNeXT

# ${UNIX_INCLUDE} needs to be put on VPATH to find the dependencies
# from the foo.cc files.
VPATH	+= ../proxies ${UNIX_INCLUDE}

access_table.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
us_tm.o_CFLAGS		= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
fs_access.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}

us_tm_root.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
us_tm_task.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
us_tm_tgrp.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
fs_cred.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
fs_auth.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
vn_mgr.o_CFLAGS		= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
vn_agency.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
vn_symlink.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
vn_tsymlink.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
vn_dir.o_CFLAGS		= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
vn_file.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
vn_pager.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}
null_pager.o_CFLAGS	= ${CPlusPlusFLAGS} -I${UNIX_INCLUDE}

# These need the us_item_proxy_ifc.h
#class_map.o_CFLAGS	= ${CPlusPlusFLAGS}
class_map.o_INCFLAGS	=  -I../proxies ${INCFLAGS}


SHORTNAMES		= df_iobuf_mgr.cc net_end_base.cc us_net_conn.cc \
			  netcltsrecs.cc netcltsbytes.cc \
			  netcotsrecs.cc netcotsbytes.cc


df_iobuf_mgr.cc_LONGNAME	= default_iobuf_mgr.cc
net_end_base.cc_LONGNAME	= net_endpt_base.cc
us_net_conn.cc_LONGNAME		= us_net_connector.cc
netcltsrecs.cc_LONGNAME 	= us_net_clts_recs.cc
netcotsrecs.cc_LONGNAME  	= us_net_cots_recs.cc
netcotsbytes.cc_LONGNAME  	= us_net_cots_bytes.cc

.include <${RULES_MK}>

${SHORTNAMES}: $${${.TARGET}_LONGNAME}
	${RM} -f ${.TARGET}
	${CP} ${${${.TARGET}_LONGNAME}:P} ${.TARGET}

