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
######################################################################
# HISTORY
# $Log:	Makefile,v $
# Revision 2.6  94/06/29  15:04:32  mrt
# 	Updated for odemake. Condensed history
# 	[94/03/03            mrt]
# 
# Revision 2.5  93/01/20  17:39:19  jms
# 	Commented out "malloc" libs line
# 	[93/01/18  17:31:49  jms]
# 
#
######################################################################

DEPENDENCIES		=
SAVE_D			=

CONFIGURATION		= MACH3_US

IDIR			= /us/bin/

PROGRAMS		= pipenet
ILIST			= pipenet

OFILES			= pipenet_init.o pipenet_root.o \
			  pipenet_internal.o \
			  pipenet_byteio.o pipenet_recio.o \
			  pipenet_endpt_base.o pipenet_dir_base.o \
			  pipenet_cldir_recs.o pipenet_codir_bytes.o \
			  pipenet_clts_base.o pipenet_cots_base.o \
			  pipenet_clts_recs.o pipenet_cots_recs.o \
			  pipenet_cots_bytes.o pipenet_connector.o \
			  pipenet_updir_bytes.o pipenet_upipe_bytes.o


# Load with libmalloc when looking for a memory leak
.if defined(DEBUG)
DEBUG_LIB	= -lmalloc
.else
DEBUG_LIB	=
.endif
LIBS 		= ${DEBUG_LIB} -lus++ ${LIBTHREADS} ${LIBMACH3}


.include <${RULES_MK}>

