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
# $Log:	bootgnumake.csh,v $
# Revision 2.1.1.1  93/03/18  18:49:43  mrt
# 	Created
# 	[93/02/02            danner]
# 

#
# expected arguments
# $1 - object tree base
# $2 - release tree base
# $3 - source directory
echo bootstraping gnumake
set go = $1/gnu/gnumake
set rdir = $2
cd $3
#
# check directory object existence
#
if ( ! -d  $go ) then
echo cannot access $go
exit 1
endif
#
# check release directory existence
#
foreach i ( bin man/man1 )
if ( ! -d $rdir/$i ) then
echo cannot access $rdir/$i
exit 1
endif
end
#
#
#
echo relinking source
cd $3
set links = `find . -type f -print`
( cd $go ; rm -f $links )
foreach i ( $links )
#	echo \[ linking $base/gnumake/$i to $go/$i \]
	ln -s $3/$i $go/$i
	endif
end
#
#
#
if  ( ! -w $go/INSTALLDIR ) then 
	echo $rdir > $go/INSTALLDIR.new
endif
#
#
#
echo $rdir > $go/INSTALLDIR.new
cmp $go/INSTALLDIR.new $go/INSTALLDIR >& /dev/null
if $status then
	echo INSTALLDIR has changed - deleteing objects
	( cd $go &&  rm -f `find . -name '*.o' -print` )
endif
#
#
#
mv -f $go/INSTALLDIR.new $go/INSTALLDIR
#
#
#
echo making gnumake with CFLAGS=-O bindir=$rdir/bin\
	mandir=$rdir/man/man1 manext=1 instname=gnumake
#
# protect ourselves against cmu make
#
unsetenv RELEASEDIR
unsetenv OBJECTDIR
(cd $go ; \
unsetenv OBJECTDIR  ; \
make CFLAGS=-O \
   bindir=$rdir/bin  \
   mandir=$rdir/man/man1 \
   manext=1 \
   instname=gnumake install )
exit 0
