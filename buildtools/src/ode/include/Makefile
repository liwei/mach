#
# Mach Operating System
# Copyright (c) 1993-1989 Carnegie Mellon University
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
# Revision 2.3  93/06/01  22:18:07  mrt
# 	Changed IDIR to /include/.
# 	[93/06/01            mrt]
# 
# Revision 2.2  93/05/08  00:28:18  mrt
# 	Created.
# 	[93/05/07            mrt]
# 
#
#	Makefile to export the include files that are
#	needed by the buildtools to compile. Whenever possible
#	you should use the .h files that are native to your system.
#	These files are here, in case you do not have some them or
#	need a reference file that the buildtools can compile against
#
#	This file is actually only useful for reference and internal
#	testing since at the time you need to install the files, you
#	don't have odemake built.


SUBDIRS		= sys ${target_cpu}

EXPINC_SUBDIRS	= ${SUBDIRS}
EXPORT_USING_TAR=

OTHERS		= machine
IDIR		= /include/
INCLUDES	= a.out.h ar.h ctype.h dirent.h errno.h fcntl.h grp.h \
		  limits.h nlist.h pwd.h ranlib.h stdlib.h stdio.h string.h \
		  strings.h time.h varargs.h ${OTHERS}


.include <${RULES_MK}>

machine:
	${RM{ ${_RMFLAGS_} machine
	${LN} -s ${target_cpu} machine
