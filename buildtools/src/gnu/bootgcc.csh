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
# $Log:	bootgcc.csh,v $
# Revision 2.1.1.1  93/03/18  18:49:03  mrt
# 	Created
# 	[93/02/02            danner]
# 

# 
#
# expected arguments
# $1 - object tree base
# $2 - release tree base
# $3 - source directory
# $4 - gcc target name
# $5 - how many stages to make (0,1,2)
echo bootstraping gcc
set rdir = $2
set conf = $4
set go = $1/gnu/gcc/$conf
set stages = $5
set base = $3
cd $base
echo configuring in gcc for $conf
#
# check for directory existence
#
if ( ! -d $1/gnu/gcc ) then
echo cannot access $1/gnu/gcc 
exit 1
endif
#
# check release out
#
foreach i ( bin lib )
if ( ! -d $rdir/$i ) then
echo cannot access $rdir/$i 
exit
endif
end
#
# make the config directory
#
if ( ! -d $go ) then
echo \[ creating $go \]
mkdir $go
endif
#
# remove dot from the path
# shell help from karl@cs.cmu.edu
# 
@ i = $#path
while ($i > 0)
	if ($path[$i] == .) then
	        echo \[ removing . from path \]
		if ($i == 1) then
			set path=($path[2-])
		else if ($i == $#path) then
			@ i --
			set path=($path[1-$i])
		     else
			@ i = $i - 1
			@ j= $i + 2
			set path=($path[1-$i] $path[$j-])
		endif
	endif
@ i -- 
end
#
# warn about stages
#
echo doing $stages stages
#
# check configuration
#
echo checking configuration in $go
if ( ! -e $go/INSTALLDIR ) then
	touch $go/INSTALLDIR
endif
cd $go 
set configureflag = "--srcdir=$base --target=$conf --prefix=$rdir"
echo $configureflag > INSTALLDIR.new
cmp INSTALLDIR.new INSTALLDIR >& /dev/null
if $status then
   echo configuration has changed - cleaning
   rm -rf * 
   echo $configureflag > INSTALLDIR.new
   echo configuring \[ output to gcc-config \]
   $base/configure $configureflag > gcc-config
endif
mv -f INSTALLDIR.new INSTALLDIR

echo making C compiler with CFLAGS="" CC=gcc LANGUAGES=c FLOAT_H=""
$rdir/bin/gnumake BISON=$rdir/bin/bison CFLAGS="" CC=gcc LANGUAGES=c FLOAT_H=""
if ( $status > 0 ) then
exit 1
endif

unsetenv GCC_EXEC_PREFIX
if ( $stages > 0 ) then
echo making stage 1
$rdir/bin/gnumake CFLAGS="-O2" CC=gcc LANGUAGES=c FLOAT_H="" stage1 
echo bootstraping with stage1
echo $rdir/bin/gnumake CFLAGS="-O2 -Bstage1/" CC=stage1/xgcc LANGUAGES=c FLOAT_H=""
$rdir/bin/gnumake CFLAGS="-O2 -Bstage1/" CC=stage1/xgcc LANGUAGES=c FLOAT_H=""
if ( $status > 0 ) then
exit 1
endif
endif

if ( $stages > 1 ) then
echo making stage 2
$rdir/bin/gnumake CFLAGS="" CC=gcc LANGUAGES=c FLOAT_H="" stage2 
echo bootstraping with stage2
$rdir/bin/gnumake CFLAGS="-O2 -Bstage2/" CC=stage2/xgcc LANGUAGES=c FLOAT_H="" 
if ( $status > 0 ) then
exit 1
endif
endif
#
# install
#
echo making install
$rdir/bin/gnumake CFLAGS="" CC=gcc LANGUAGES=c FLOAT_H="" install
