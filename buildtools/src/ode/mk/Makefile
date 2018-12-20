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
# Revision 2.2  93/03/20  00:10:43  mrt
# 	Created
# 	[93/01/14            mrt]
# 
# 


DATAFILES		= osf.depend.mk osf.doc.mk osf.lib.mk \
			  osf.mach3.mk osf.mach3.passes.mk \
			  osf.man.mk osf.mig.mk osf.obj.mk \
			  osf.ode.mk osf.ode.passes.mk \
			  osf.prog.mk osf.rules.mk osf.script.mk \
			  osf.std.mk sys.mk

MANPAGES		= makefiles
MANSECTION		= 5

EXPBIN_TARGETS		= ${DATAFILES:S/^/export_/g}

ILIST			= ${DATAFILES} makefiles.${MANSECTION} makefiles.0
IDIR			= /lib/mk/
makefiles.${MANSECTION}_IDIR	= /man/man${MANSECTION}/
makefiles.0_IDIR		= /man/cat${MANSECTION}/


.include <${RULES_MK}>
