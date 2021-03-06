#
# HISTORY
# $Log:	Makefile,v $
# Revision 3.63  93/02/01  10:52:38  mrt
# 	Modified install command to ignore failure of chgrp/chmod
# 	 commands. 
# 	[92/12/15            danner]
# 
#
# Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
# This file is part of GNU Make.
# 
# GNU Make is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# GNU Make is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Make; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

#
#	Makefile for GNU Make
#

CFLAGS = $(defines) -g
LDFLAGS = -g

# Define these for your system as follows:
# 	-DUSG			System V
#	-DUSGr3			SVR3 (also define USG)
#	-DHPUX			HP-UX (also define USG and USGr3 appropriately)
#	-DHAVE_SYS_WAIT		USG, but have <sys/wait.h> and wait3
#	-DHAVE_SIGLIST		USG, but have sys_siglist
#	-DHAVE_DUP2		USG and not USGr3, but have dup2
#	-DNO_MINUS_C_MINUS_O	cc can't handle "cc -c foo.c -o foo.o"
#	-DPOSIX			A 1003.1 system (or trying to be)
#	-DNO_ARCHIVES		To disable `ar' archive support.
#	-DNO_FLOAT		To avoid using floating-point numbers.
#	-DENUM_BITFIELDS	If the compiler isn't GCC but groks enum foo:2.
#				Some compilers apparently accept this
#				without complaint but produce losing code,
#				so beware.
# NeXT 1.0a uses an old version of GCC, which required -D__inline=inline.
defines =

# Define these for your system as follows:
#	-DUMAX		Encore UMAX
#	-DUMAX_43	Encore UMAX 4.3 (also define UMAX)
#	-DNO_LDAV	Disable load-average checking.
# To read /dev/kmem (most Unix systems), define these if different from the
# given defaults:
#	-DKERNEL_FILE_NAME=\"/vmunix\"
#	-DLDAV_SYMBOL=\"_avenrun\"
#	-DLDAV_CVT="(double) load"
# Define:
#	-DNLIST_NAME_UNION	If `struct nlist' has a n_un member.
#	-DNLIST_NAME_ARRAY	If `n_name' is an array.
LOAD_AVG =

# If you don't want archive support, comment these out.
ARCHIVES = arscan.o ar.o
ARCHIVES_SRC = arscan.c ar.c

# If your system needs extra libraries loaded in, define them here.
# System V probably need -lPW for alloca.  HP-UX 7.0's alloca in
# libPW.a is broken on HP9000s300 and HP9000s400 machines.  Use
# alloca.c (below) instead on those machines.  SGI Irix needs -lmld
# for nlist.
LOADLIBES =

# If your system doesn't have alloca, or the one provided is bad,
# get it from the Emacs distribution and define these.
#ALLOCA = alloca.o
#ALLOCASRC = alloca.c

# If there are remote execution facilities defined,
# enable them with switches here (see remote-*.c).
REMOTE =

# Any extra object files your system needs.
extras =

# Comment this out if POSIX.2 glob is installed on your system
# (it's in the GNU C Library, so if you're using that, this is
# not needed at all.)
globdep = glob/libglob.a

# Library containing POSIX.2 `glob' function.
# Comment this line out if it's in the C library (which is the case if you
# are using the GNU C Library), or change it to the appropriate file name
# or -l switch.
globlib = $(globdep)

# Name under which to install GNU make.
instname = make
# Directory to install `make' in.
bindir = $(prefix)/usr/local/bin
# Directory to install the man page in.
mandir = $(prefix)/usr/local/man/manl
# Number to put on the man page filename.
manext = l

# Install make setgid to this group so it can read /dev/kmem.
group = kmem

objs = commands.o job.o dir.o file.o load.o misc.o main.o read.o \
       remake.o remote.o rule.o implicit.o default.o variable.o expand.o \
       function.o vpath.o version.o $(ARCHIVES) $(ALLOCA) $(extras)
srcs = commands.c job.c dir.c file.c load.c misc.c main.c read.c \
       remake.c remote.c rule.c implicit.c default.c variable.c expand.c \
       function.c vpath.c version.c $(ALLOCASRC) $(ARCHIVES_SRC) \
       commands.h dep.h file.h job.h make.h rule.h variable.h


.SUFFIXES:
.SUFFIXES: .o .c .h .ps .dvi .texinfo

.PHONY: all doc
all: make
doc: make.info make.dvi


# Take your pick.
#makeinfo = emacs -batch make.texinfo -f texinfo-format-buffer -f save-buffer
makeinfo = makeinfo make.texinfo

make.info: make.texinfo
	$(makeinfo)


make.dvi: make.texinfo
	-tex make.texinfo
	texindex make.cp make.fn make.ky make.pg make.tp make.vr
	-tex make.texinfo

make.ps: make.dvi
	dvi2ps make.dvi > make.ps

make: $(objs) $(globdep)
	$(CC) $(LDFLAGS) $(objs) $(globlib) $(LOADLIBES) -o make.new
	mv -f make.new make

load.o: load.c
	$(CC) $(CFLAGS) $(LOAD_AVG) -c load.c
remote.o: remote.c
	$(CC) $(CFLAGS) $(REMOTE) -c remote.c

# For some losing Unix makes.
MAKE = make

glob/libglob.a: force
	cd glob; $(MAKE) CC='$(CC)' CFLAGS='$(CFLAGS) -I..' libglob.a
force:

TAGS: $(srcs)
	etags -tw $(srcs)
tags: $(srcs)
	ctags -tw $(srcs)

.PHONY: install
install: $(bindir)/$(instname) $(mandir)/$(instname).$(manext)

$(bindir)/$(instname): make
	cp make $@.new
# These are necessary for load-average checking to work on most Unix machines.
	-chgrp $(group) $@.new && chmod g+s $@.new
	mv $@.new $@

$(mandir)/$(instname).$(manext): make.man
	cp make.man $@

.PHONY: clean realclean
clean: glob-clean
	-rm -f make *.o core
realclean: clean glob-realclean
	-rm -f TAGS tags make.info* make-* make.dvi *~
	-rm -f make.?? make.??s make.log make.toc make.*aux

.PHONY: glob-clean glob-realclean
glob-clean glob-realclean:
	cd glob; $(MAKE) $@

# Automatically generated dependencies will be put at the end of the file.

# Automatically generated dependencies.
commands.o : commands.c make.h dep.h commands.h file.h variable.h job.h 
job.o : job.c make.h commands.h job.h file.h variable.h 
dir.o : dir.c make.h 
file.o : file.c make.h commands.h dep.h file.h variable.h 
load.o : load.c make.h commands.h job.h 
misc.o : misc.c make.h dep.h 
main.o : main.c make.h commands.h dep.h file.h variable.h job.h 
read.o : read.c make.h commands.h dep.h file.h variable.h 
remake.o : remake.c make.h commands.h job.h dep.h file.h 
remote.o : remote.c remote-stub.c make.h commands.h 
rule.o : rule.c make.h commands.h dep.h file.h variable.h rule.h 
implicit.o : implicit.c make.h rule.h dep.h file.h 
default.o : default.c make.h rule.h dep.h file.h commands.h variable.h 
variable.o : variable.c make.h commands.h variable.h dep.h file.h 
expand.o : expand.c make.h commands.h file.h variable.h 
function.o : function.c make.h variable.h dep.h commands.h job.h 
vpath.o : vpath.c make.h file.h variable.h 
version.o : version.c 
arscan.o : arscan.c 
ar.o : ar.c make.h file.h 
