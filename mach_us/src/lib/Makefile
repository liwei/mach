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
#
# File:        Makefile
#
# Purpose: Build file for MachUS libraries
#
# HISTORY: 
# $Log:	Makefile,v $
# Revision 2.8  94/06/29  15:00:52  mrt
# 	Updated for odemake.
# 	[94/02/02            mrt]
# 
# Revision 2.7  94/01/11  17:48:26  jms
# 	Add "proxies" lib
# 	[94/01/09  18:22:35  jms]
# 
# Revision 2.6  92/07/05  23:24:14  dpj
# 	Removed lib/us from automatic build.
# 	[92/07/05  18:50:55  dpj]
# 
# 	Removed machobj++ (merged into us++).
# 	[92/05/10  00:20:26  dpj]
# 
# Revision 2.5  92/03/05  14:55:20  jms
# 	Stop building "server" and "bnw".
# 	[92/02/26  16:52:06  jms]
# 
# Revision 2.4  91/11/13  16:32:42  dpj
# 	Removed lib/load from standard list of libraries.
# 	[91/08/25  18:05:27  dpj]
# 
# Revision 2.3  91/11/06  11:28:54  jms
# 	Added directories us++ and c++, and changed machobj to machobj++
# 	[91/09/26  18:46:02  pjg]
# 
# Revision 2.2  90/08/22  18:07:28  roy
# 	First version.
# 	[90/08/22  12:09:23  roy]
# 
#
#


CONFIGURATION		=

SUBDIRS			= threads mach3 c++ us++ proxies ux emul
EXPINC_SUBDIRS		= mach3 us++ proxies ux
EXPLIB_SUBDIRS		= ${SUBDIRS}

.include	<${RULES_MK}>
