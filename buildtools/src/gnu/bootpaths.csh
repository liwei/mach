#!/bin/csh -fbe
# 
# Mach Operating System
# Copyright (c) 1993  Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
# any improvements or extensions that they make and grant Carnegie the
# rights to redistribute these changes.
# 
# 
# HISTORY
# $Log:	bootpaths.csh,v $
# Revision 2.1.1.1  93/03/18  18:49:35  mrt
# 	Created
# 	[93/02/02            danner]
# 

#
# expected arguments 
#    object directory tree root 
#    release directory tree root 
#
set odir = $1
set rdir = $2
#
echo creating release directories
makepath $rdir/bin/. 
makepath $rdir/lib/. 
makepath $rdir/info/. 
makepath $rdir/man/man1/.
echo creating object directories
makepath $odir/gnu/gnumake/glob/.
makepath $odir/gnu/bison/.
makepath $odir/gnu/gcc/.
exit 0


