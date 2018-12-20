#!/bin/csh
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
# Copyright (c) 1991
# Open Software Foundation, Inc.
#
# Permission is hereby granted to use, copy, modify and freely distribute
# the software in this file and its documentation for any purpose without
# fee, provided that the above copyright notice appears in all copies and
# that both the copyright notice and this permission notice appear in
# supporting documentation.  Further, provided that the name of Open
# Software Foundation, Inc. ("OSF") not be used in advertising or
# publicity pertaining to distribution of the software without prior
# written permission from OSF.  OSF makes no representations about the
# suitability of this software for any purpose.  It is provided "as is"
# without express or implied warranty.
#
#
# HISTORY
# $Log:	setvar.csh,v $
# Revision 2.7  93/06/16  17:05:07  mrt
# 	Added code to create all the top-level object directories
# 	Made definition of OWNER and GROUP conditional so it could
# 	be overidden by the context dependent scripts.
# 	[93/06/15            mrt]
# 
# 	Added the local release directory to MAKESYSPATH and INCDIRS.
# 	Added a fix to create the export/../special directory.
# 	[93/06/10  14:46:07  mrt]
# 
#	Created all the objdir subdirectories.
# 	Put in some more "" to keep BNR2 csh happy
# 	[93/06/09            mrt]
# 
# Revision 2.6  93/05/14  23:16:18  mrt
# 	Added to evaluation of progdir so it would work with BNR2
# 	expr program. Exported host_context. 
# 	[93/05/07            mrt]
# 
# Revision 2.5  93/03/20  00:14:13  mrt
# 	setenv DEFCPATH 
# 	Changed -mastersrc argument to -masterbase.
# 	[93/02/06            mrt]
# 
# 	Wrapped optional `(cd $dir; pwd)` checks with 'if -d $dir'.
# 	Removed superfluous () from inside `( ... )`
# 	Replaced `unalias cd; cd ...` with `chdir`
# 	[93/02/02            jfriedl]
# 
# 	Fixed for cross building. Removed setting of CPP, CC and
# 	ASCPP. They are now set in osf.mach3.mk.
# 	[92/12/16            mrt]
# 
# Revision 2.3  92/07/08  18:06:16  mrt
# 	Added the release area to INCDIRS and LIBDIRS after the export
# 	area. Defined CPP conditionally so  that it can be set in
# 	the context specific setvar.csh
# 	[92/06/18            mrt]
# 
# Revision 2.2  92/05/20  20:16:38  mrt
# 	First checkin.
# 	[92/05/20  14:24:46  mrt]
# 
#
#    File:	setvar.csh
#    Copied from: setup.sh
#    Date:	May, 1990
#
#    Abstract:
#	This script sets all the environment variables used by make
#	and leaves you in a csh shell so that you can just type
#	make <program>. This script can be used instead of using the build
#	program and rc-files. This script initializes the environment to 
#	build for the Mach3 project, not the ODE tools. 
#
#
# The script assumes that it is in the directory ode/setup.
#
# Usage is as follows:
#	setvar.csh [-basedir <dir>] [-masterbase <dir> ]
#		   [-systembase <dir> ] [-makesyspath <dir>]
#		   [-target <machine>] <host_context> 
#
#	    -basedir <dir>: the base of the tree which contains the src,obj
#		and export directories. Defaults to the parent
#		of the current directory.
#
#	    -masterbase <dir>: if you are shadowing set this to the
#		base of the source and release directories you are shadowing.
#		e.g. /afs/cs/project/mach3/latest.
#
#	    -systembase <dir>: is the base of the tree that contains a
#		current system release. If you are shadowing a fully built
#		release area you probably do not need to set this variable, 
#		since anything that in not in the local area should be found 
#		in either the master/release area or on the default PATH. 
#		If you are not shadowing and this variable is not set, then 
#		a bootstrap script should be run to get the
#		the system tools and include files generated and released
#		into the local export directory.
#
#	    -makesyspath <dir>: is a list of directories (separated by ':'s)
#		that is searched by odemake to find the .mk rules files.
#		Defaults to <basedir>/export/<host_context>/lib/mk, followed
#		by masterbase/release if shadowing, and systembase if it 
#		was given.
#
#	    -target <machine> should only be used when you are cross-building 
#		and the target_machine is different from the host_machine.
#		Otherwise target_machine is set from the host_context
#
#	    -savepath if set, the current PATH will be added to the end
#		of the PATH that is set here. By default PATH will only
#		contain the minimum necessary to build.
#
#	    -help - prints a brief help message
#
#	    host_context is host_machine_os specification that selects
#		a sub setvar.csh that sets machine specific variables
#		such as HOST_MACHINE, target_machine, target_cpu and
#		default search paths.
#
#

set USAGE="usage: setvar.csh [-basedir <dir>] [-masterbase <dir>] [-systembase <dir>] [-makesyspath <dir>] [-coll < <mk|ux|user|bsdss|buildtools>[-target <machine>] [-savepath] [-help] <context>]"
if ( $#argv < 1 ) then
   echo "$USAGE"
   exit (1)
endif
set progdir = `expr "$0" : "\(.*\)/.*"`
if ( $progdir == "" || $progdir == "0") then
   set progdir = "."
endif
set base=NULL
set makesyspath=NULL
set system_base=NULL
set master_base=NULL
set save_path=NULL
if ($#argv == 1 && "$1" == "-help") then
	echo "$USAGE"
	echo "-basedir -- base of your source and object area"
	echo "-masterbase -- if shadowing, base of master source and release area"
	echo "-systembase -- location of released system area"
	echo "-makesyspath -- location of odemake .mk files"
	echo "-target -- target machine if different for host machine"
	echo "-savepath -- set to add current PATH to new PATH"
	echo "-help -- this message"
	echo "context - <hostmachine>_<host_os>"
	exit (0); 
endif

while ($#argv > 1)
   switch ( "$1" )
	case "-basedir":
		set base=$2
		shift; shift
		breaksw
	case "-masterbase":
		set master_base=$2
		shift; shift;
		breaksw
	case "-systembase":
		set system_base=$2
		shift; 	shift
		breaksw
	case "-makesyspath":
		set makesyspath=$2
		shift; 	shift
		breaksw
	case "-target":
		setenv target_machine $2
		shift; 	shift
		breaksw
	case "-savepath":
		set save_path=1
		shift
		breaksw
	default:
		echo "unrecognized argument: $1"
		echo "$USAGE"
		exit (1)
	endsw
end
if ( $#argv == 1 ) then
    setenv host_context $1
else 
    echo "$USAGE"
    exit (1)
endif
#
# Read configuration info
#
if ( ! $?target_machine ) then 
    set cross_building=0
    set cross_context=$host_context
else
    set cross_building=1
    set cross_context=${host_context}_X_${target_machine}
    echo "target_machine was set to $target_machine"
    echo "Setting up for a CROSS BUILD from $host_context to $target_machine"
endif
if ( ! -r $progdir/$cross_context/setvar.csh ) then
  echo "Missing $progdir/$cross_context/setvar.csh file"
  exit (1)
else
  echo "[reading configuration from $progdir/$cross_context/setvar.csh]"
endif

source $progdir/$cross_context/setvar.csh


#
#  Attempt to find the base directory, assuming it is a superior directory
#
if ( $base == NULL ) then
   set parent = `pwd`
   set src = $parent:t
   while ( ( $src != "" ) && ( $src != "src" ) )
	set parent=$parent:h
	set src=$parent:t
   end
   if ( $src == "" ) then
	echo "Can't determine base directory. Please input -basedir <dir>"
	exit (1)
   else
	set base = $parent:h
   endif
endif
echo "base directory: $base"
echo "host context: $host_context  target context: $target_context cross context: $cross_context"

# Note at this point if we are cross building, host, target context
# are all different. If we are building native they are all the 
# same. In setting all the directory names only the cross build case is
# interesting.

#
# Setup immediate directory hierarchy
#

#   objdir is where all objects are placed during the
#   build process. Some of these are host objects but
#   most are target objects.
set objdir = ${base}/obj
if ( ! -d $objdir ) then
    mkdir $objdir
endif
set objdir = ${base}/obj/${cross_context}
if ( ! -d $objdir ) then
    mkdir $objdir
endif

foreach tdir ( $objdir/mk $objdir/ux $objdir/bsdss $objdir/user $objdir/buildtools )
    if ( ! -d $tdir ) then
	mkdir $tdir
    endif
end

#   expdir contains pieces of the target system that are
#   used during later parts of the build
set expdir = ${base}/export
if ( ! -d $expdir ) then
    mkdir $expdir
endif

# The export bin directory contains tools that are used during building
# These tools work on the host machine and host os, but may also depend
# on the target machine.

set texpdir = ${base}/export/${target_context}
set hexpdir = ${base}/export/${host_context}
set cexpdir = ${base}/export/${cross_context}

foreach tdir ( $texpdir $hexpdir $cexpdir ${texpdir}/include ${hexpdir}/include )
    if ( ! -d $tdir ) then
	mkdir $tdir
    endif
end

foreach tdir ( ${cexpdir}/include  ${cexpdir}/bin ${hexpdir}/bin ${cexpdir}/lib ${cexpdir}/special )
    if ( ! -d $tdir ) then
	mkdir $tdir
    endif
end

#    treleasedir contains target system  files: executables
#    libs, and includes. creleasedir is used for cross builds

set releasedir = ${base}/release
if ( ! -d $releasedir ) then
    mkdir $releasedir
endif
set treleasedir = ${base}/release/${target_context}
set creleasedir = ${base}/release/${cross_context}
foreach tdir ( $treleasedir $creleasedir )
    if ( ! -d $tdir ) then
	mkdir $tdir
    endif
end

#
# Constrain search paths
#    CPATH should locate generic Unix include files
#    It  may not be used by all Unix compilers
#    LPATH is used by wh to find migcom and crt0.o 
#    and by gcc to find crt0.o.
#    (we should fix wh to
#    take an LIBDIRS arg and then we would not need to set
#    LPATH in additions to LIBDIRS.

set SAVEPATH=$PATH

setenv PATH "${cexpdir}/bin:${creleasedir}/bin"
setenv LPATH "${cexpdir}/lib:${creleasedir}/lib"


# if we are shadowing, pick up release and tools from the master
if ( $master_base != NULL ) then
    setenv PATH "${PATH}:$master_base/release/$cross_context/bin"
    setenv LPATH "${LPATH}:$master_base/release/$cross_context/lib"
endif

# if a systembase was given, pick up binaries, libs and headers from there
# as well. DEFCPATH is used by osf.depend.mk to get commented dependency
# lines for .h files found on the DEFCPATH


if ( $system_base != NULL ) then
    setenv PATH "${PATH}:${system_base}/bin"
    setenv LPATH "${LPATH}:${system_base}/lib"
# DEFCPATH is going to be used for dependencies, so we need to expand symlinks
    if (-d  "${system_base}/include") then
      set tpath = `chdir ${system_base}/include; /bin/pwd`
      setenv DEFCPATH "${tpath}:${DEFCPATH}"
    endif
endif

if ( $cross_building == 1 ) then
# add host_contexts to PATH so if cross tool doesn't exist the host one will be used
    setenv PATH  "${PATH}:${hexpdir}/bin"
    if ( $master_base != NULL ) then
	setenv PATH "${PATH}:$master_base/release/${host_context}/bin"
    endif
endif

setenv PATH "${PATH}:${DEFPATH}"
setenv LPATH "${LPATH}:${DEFLPATH}"
setenv HOST_LPATH "${DEFLPATH}"
setenv CPATH "${DEFCPATH}"

if ( $save_path != NULL ) then
   setenv PATH "${PATH}:${SAVEPATH}"
endif
echo PATH is $PATH

# INCDIRS and LIBDIRS are where programs that are to be run on the target machine
# search for includes and libs. 
# programs that run on the HOST_MACHINE generally look only in CPATH and LPATH
# but may need to look in HOST_INCDIRS as well

# INCDIRS is going to be used for dependencies, so we need to expand symlinks
set tpath = ""
set tpath = `chdir ${cexpdir}/include; pwd`
setenv INCDIRS "-I${tpath}"

if ( -d ${creleasedir}/include ) then
   set tpath = ""
   set tpath = `chdir ${creleasedir}/include; pwd`
   setenv INCDIRS "${INCDIRS} -I${tpath}"
endif

set tpath = ""
set tpath = `chdir ${hexpdir}/include ; pwd`
setenv HOST_INCDIRS "-I${tpath}"

setenv LIBDIRS "-L${cexpdir}/lib"
setenv HOST_LIBDIRS "-L${hexpdir}/lib"

# if we are shadowing, pick up release from the master

if ( $master_base  != NULL ) then
    if (-d "$master_base/release/${target_context}/include") then
      set tpath = `chdir $master_base/release/${target_context}/include; pwd`
      setenv INCDIRS "${INCDIRS} -I${tpath}"
    endif
    if (-d "${master_base}/release/${host_context}/include") then
      set tpath = `chdir ${master_base}/release/${host_context}/include; pwd`
      setenv HOST_INCDIRS "${HOST_INCDIRS} -I${tpath}"
    endif
    setenv LIBDIRS "${LIBDIRS} -L${master_base}/release/${cross_context}/lib"
    setenv HOST_LIBDIRS "${HOST_LIBDIRS} -L${master_base}/release/${host_context}/lib"
endif

#
# Site/Environment stuff
# OWNER and GROUP are used only during install step<
#

if ( ! $?OWNER ) then
   setenv OWNER "cs"
endif
if (! $?GROUP ) then
   setenv GROUP "cs"
endif

#
# New build environment definitions
#
setenv PROJECT_NAME MACH3
setenv project_name mach3
if ( ! $?KERN_MACHINE_DIR ) then
    setenv KERN_MACHINE_DIR $target_cpu
endif
setenv RULES_MK osf.rules.mk
if ( $makesyspath != NULL ) then
    setenv MAKESYSPATH $makesyspath
else
# if we are shadowing, pick up system rules from the master
    if ( $master_base != NULL ) then
	setenv MAKESYSPATH "${hexpdir}/lib/mk:${creleasedir}/lib/mk:${master_base}/release/${host_context}/lib/mk"
    else
	setenv MAKESYSPATH ${hexpdir}/lib/mk:${creleasedir}/lib/mk
    endif
# if a system base was given look there as well
    if ( $system_base != NULL ) then
	setenv MAKESYSPATH "${MAKESYSPATH}:${system_base}/lib/mk"
    endif
endif

# EXPORTBASE is used by make during export pass
# SOURCEBASE, SOURCEDIR and OBJECTDIR are needed by genpath
# and also used by the Makeconf files to set the Make variables

set tpath = ""
set tpath = `chdir $cexpdir; pwd`
setenv EXPORTBASE $tpath

set tpath = ""
set tpath = `chdir ${base}/src; pwd`
setenv SOURCEBASE $tpath

if ( $master_base != NULL ) then
    setenv SOURCEDIR $master_base/src
else
    setenv SOURCEDIR ""
endif

if ( ! $?OBJECTDIR )then
    set tpath = ""
    set tpath = `chdir $objdir; pwd`
    setenv OBJECTDIR $tpath
endif

setenv MAKEFLAGS "-r"
#setenv TOSTAGE $treleasedir
# TOSTAGE keeps PROGRAMS from building

cd $base/src
if ( $?SHELL ) then
   $SHELL -i
else
  csh -i
endif

