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
# $Log:	bootstrap.csh,v $
# Revision 2.1.1.2  93/04/22  15:06:54  mrt
# 	Changed .sh suffixes to .csh.
# 	[93/04/22            mrt]
# 
# Revision 2.1.1.1  93/03/18  18:49:26  mrt
# 	Created
# 	[93/02/02            danner]
# 

#
# this script is for bootstraping the gnu collection.
#
# the following environment variables should be set
#
# Arguments
#	odir - base of the object tree (../@sys/buildtools/) 
#       rdir - base of the release tree (latest/release/@sys) 
#       gcc  - name of the gcc target to build
#       passes - number of stages of gcc to build 
#
# bootstrap gnu make
# assume no shadowing support -- symlink files over
#
set base = $cwd
set odir = $1
set rdir = $2
set gcc  = $3
set passes = $4
echo base is $base
echo OBJECTDIR is  $odir
echo RELEASEDIR is $rdir
echo GCC_TARGET is $gcc
echo $passes stages of gcc
#
# unset potentially obnoxious make variables
echo unsetenv-ing MAKEFLAGS OBJECTDIR RELEASEBASE SOURCEBASE VPATH
unsetenv MAKEFLAGS OBJECTDIR RELEASEBASE SOURCEBASE VPATH
# 
$base/bootpaths.csh $odir $rdir
#
# build gnumake
#
$base/bootgnumake.csh $odir $rdir $base/gnumake
#
# build bison
#
$base/bootbison.csh $odir $rdir $base/bison
#
# build gcc
#
$base/bootgcc.csh $odir $rdir $base/gcc $gcc $passes 
exit 0
