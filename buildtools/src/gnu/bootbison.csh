#!/bin/csh -fb
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
# $Log:	bootbison.csh,v $
# Revision 2.1.1.1  93/03/18  18:49:16  mrt
# 	Created
# 	[93/02/02            danner]
# 

#
# expected arguments
# $1 - object tree base
# $2 - release tree base
# $3 - source directory
echo bootstrapping in bison
set odir = $1/gnu/bison
set rdir = $2
set base = $3
#
# check things out in object land
#
if ( ! -d $odir ) then
echo cannot access $odir
exit 1
endif
#
# check things out in release land
#
foreach i ( bin lib info man/man1 )
if ( ! -d $rdir/$i ) then
echo cannot access $rdir/$i
exit 1
endif
end
#
# things look ok
# 
cd $odir
#
# check configuration
#
echo checking configuration in $odir
if ( ! -e INSTALLDIR ) then
	touch INSTALLDIR
endif
set configureflag = "--srcdir=$base --prefix=$rdir"
echo $configureflag > INSTALLDIR.new
cmp INSTALLDIR.new INSTALLDIR >& /dev/null
if $status then
   echo configuration has changed - cleaning 
   rm -rf * 
   echo $configureflag > INSTALLDIR.new
   echo configuring \[ output to bison-config \]
   $base/configure $configureflag > bison-config
endif
mv -f INSTALLDIR.new INSTALLDIR
echo making in bison "(CFLAGS=-O LDFLAGS=)"
$rdir/bin/gnumake CFLAGS=-O LDFLAGS="" install
echo completed bison bootstrap
exit 0
