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


# HISTORY
# $Log:	Makefile,v $
# Revision 2.6  94/06/29  15:01:14  mrt
# 	Updated for odemake. Stopped building libmach3_sa.
# 	[94/02/09            mrt]
# 
# Revision 2.5  92/03/05  14:56:31  jms
# 	Stop building/useing libmach3_unix
# 	[92/02/26  17:39:54  jms]
# 
# Revision 2.4  91/12/20  17:43:30  jms
# 	Add "include" supdir in order to install "include" files before building
# 	the rest of the stuff.
# 	[91/12/20  15:23:30  jms]
# 
# Revision 2.3  90/07/26  18:24:06  dpj
# 	Added libmach3_unix.
# 
# Revision 2.2  90/07/26  12:35:31  dpj
# 	First version
# 	[90/07/24  14:18:38  dpj]
# 
#
######################################################################

CONFIGURATION	=

SUBDIRS		= include  libmach3_vus libmach3_us
EXPINC_SUBDIRS	= include
EXPLIB_SUBDIRS	= libmach3_vus libmach3_us

.include <${RULES_MK}>

