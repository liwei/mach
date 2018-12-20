# 
# Mach Operating System
# Copyright (c) 1993,1992 Carnegie Mellon University
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
# $Log:	Makefile,v $
# Revision 2.4  93/08/02  13:40:19  mrt
# 	Added pmax_ultrix to PMAX_SUBDIRS.
# 	[93/07/06            mrt]
# 
# Revision 2.3  93/05/07  23:49:34  mrt
# 	Made SUBDIRS dependent on host_machine, so we no longer
# 	export all contexts on all platforms, but only the ones
# 	that might be used there.
# 	[93/04/28            mrt]
# 
# Revision 2.2  93/02/06  17:20:29  mrt
# 	Created
# 	[93/01/14            mrt]
# 
# 


SCRIPTS			= setvar
MANPAGES		= setvar
MANSECTION		= 1

alpha_SUBDIRS		= alpha_mach alpha_osf1
i386_SUBDIRS		= i386_bnr i386_mach
luna88k_SUBDIRS		= luna88k_mach
pc532_SUBDIRS		= pc532_mach
pmax_SUBDIRS		= pmax_mach pmax_mach_X_alpha pmax_ultrix
sun3_SUBDIRS		= sun3_mach
sun4_SUBDIRS		= sun4_mach
vax_SUBDIRS		= vax_mach

SUBDIRS			= ${${host_machine}_SUBDIRS}

EXPBIN_SUBDIRS		= ${SUBDIRS}
EXPBIN_TARGETS		= export_setvar

ILIST			= ${SCRIPTS} setvar.${MANSECTION} setvar.0
IDIR			= /etc/setup/
setvar.${MANSECTION}_IDIR	= /man/man${MANSECTION}/
setvar.0_IDIR		= /man/cat${MANSECTION}/

.include <${RULES_MK}>
