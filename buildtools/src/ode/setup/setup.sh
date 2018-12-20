#!/bin/sh
#
# Mach Operating System
# Copyright (c) 1992 Carnegie Mellon University
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
# Copyright (c) 1990, 1991
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
# $Log:	setup.sh,v $
# Revision 2.8  93/08/02  13:40:44  mrt
# 	Added a rm of setup.sh before the copy.
# 	[93/08/01            mrt]
# 
# Revision 2.7  93/05/07  23:53:40  mrt
# 	Added expdir/buildtools/include to INCDIRS as a place to put
# 	include files to override the standard system ones if necessary.
# 	[93/05/07            mrt]
# 
# Revision 2.6  93/03/20  00:14:05  mrt
# 	Change INCDIRS for  expdir/usr/include to expdir/include.
# 	[93/03/13            mrt]
# 
# Revision 2.5  93/02/06  17:22:18  mrt
# 	Changed to toolsdirs to be export/<context>/bin rather than
# 	tools/<context>/bin.
# 	[93/01/08            mrt]
# 
# Revision 2.4  92/07/09  13:51:06  mrt
# 	exported SUBDIR so that the ode/Makeconf could use it to
# 	define MAKEOBJDIR.
# 	[92/07/09            mrt]
# 
# Revision 2.3  92/07/08  18:06:11  mrt
# 	Added a make of wh since it is needed on non-mach systems for
# 	the kernel and server Makefiles
# 	[92/07/08            mrt]
# 
# Revision 2.2  92/05/20  20:16:36  mrt
# 	Started from OSF DCE v-1 ODE 2.1 and modified to work on a Mach system. 
# 
# 	[92/05/16            mrt]
# 

# This is the setup script for building the source tree from scratch
# using as little as possible from the environment already installed on
# the current machine.  The basic process is to create the "environment"
# as we go along, which requires that this script "understand" all of the
# interdependencies between components and their environment.  When porting
# the sources to a "unknown" machine, this script is the place to start
# making changes.  Good Luck!
#
#
# The script assumes that it is in the directory src/${subdir}/ode/setup, 
# where ode/bin contains the sources of the tools to be built and
# ode/mk contains the project *.mk files to be used by make. obj and export
# directories will be created in the directory that src is in, and the 
# programs will be built in obj/$context/$subdir/ode/bin and installed 
# in export/$context/bin. There must be a Makeconf file the directory ${subdir}
#  that specifies MAKEOBJDIR=${OBJECTDIR}/<$subdir>.
# Usage is as follows:
#	cd to the directory that contains the file setup.sh. 
#	setup.sh  [-nomake] <context>
#	   -nomake: do not rebuild the make program
#	    context: is a combination of machine_os that you are building
#			on. 


USAGE="usage: setup.sh [-nomake] <context>"
MAKE=YES
context=NULL

#
#  check that we are running this script in the correct directory
#
srcdir=`pwd`
parent=`expr "${srcdir}" : "\(.*\)/.*"`
ode=`basename "${parent}"`
if [ $ode != "ode" ] 
then
    echo "This script should be exectuted in the ode/setup directory"
    exit 1
fi
while [ $# -ge 1 ] ; do
    case $1 in
	-nomake ) MAKE=NO
		shift
		;;
	* )	context=$1
		shift
		;;
	esac
done
if [ ${context} = NULL  ]
then
  echo "Missing context"
  echo "$USAGE"
  exit 1
fi
export context
#
# Read configuration info
#
if [ ! -r $context/setup.sh ]
then
  echo "Missing $context/setup file"
  exit 1
fi
. $context/setup.sh

parent=`expr "${parent}" : "\(.*\)/.*"`
src=`basename "${parent}"`
subdir=/ode
while [ $src != "src" ] ; do
    subdir=/$src$subdir
    parent=`expr "${parent}" : "\(.*\)/.*"`
    src=`basename "${parent}"`
done
base=`expr "${parent}" : "\(.*\)/.*"`
echo base:$base subdir:$subdir


#
# Setup immediate directory heirarchy
#
obj=${base}/obj
if [ -d ${obj} ]
then
    true
else
    mkdir ${obj}
fi
objdir=${obj}/${context}
if [ -d ${objdir} ]
then
    true
else
    mkdir ${objdir}
fi
exp=${base}/export
if [ -d ${exp} ]
then
    true
else
    mkdir ${exp}
fi
expdir=${exp}/${context}
if [ -d ${expdir} ]
then
    true
else
    mkdir ${expdir}
fi

# Uncomment next line if you want the tools to end up in tools/<context>/bin
# rather than export/<context>/bin
#tools=${base}/tools
tools=${base}/export
if [ -d ${tools} ]
then
    true
else
    mkdir ${tools}
fi
toolsdir=${tools}/${context}
if [ -d ${toolsdir} ]
then
    true
else
    mkdir ${toolsdir}
fi
bindir=${toolsdir}/bin
if [ -d ${bindir} ]
then
    true
else
    mkdir ${bindir}
fi
libdir=${toolsdir}/lib
if [ -d ${libdir} ]
then
    true
else
    mkdir ${libdir}
fi
etcdir=${toolsdir}/etc
if [ -d ${etcdir} ]
then
    true
else
    mkdir ${etcdir}
fi


#
# Constrain search paths
#
PATH="${bindir}:${DEFPATH}"
CPATH="${DEFCPATH}"
LPATH="${DEFLPATH}"
INCDIRS="-I${expdir}/buildtools/include -I${expdir}/include"
LIBDIRS=

export PATH CPATH LPATH  INCDIRS LIBDIRS

CENV="-D_BLD $PORTING_FLAGS"
OWNER="bin"
GROUP="bin"

export CENV OWNER GROUP

#
# Project Definitions
#
PROJECT_NAME=ODE
project_name=ode
RULES_MK=osf.rules.mk
MAKESYSPATH="${srcdir}/../mk"

export PROJECT_NAME project_name RULES_MK MAKESYSPATH

#
# build environment definitions
#
EXPORTBASE=${expdir}
SOURCEBASE=${base}/src
SOURCEDIR=""
OBJECTDIR=${objdir}
SUBDIR=${subdir}

export EXPORTBASE SOURCEBASE SOURCEDIR OBJECTDIR SUBDIR

#
#  create object directory for make. This code
#  only allows a two component or less value of subdir
#

d=${objdir}$subdir
if [ ! -d ${d} ]
then
    mkdir ${d}
    if [ ! -d ${d} ]
    then
	dp=`expr "${d}" : "\(.*\)/.*"`
	mkdir ${dp}
	if [ ! -d ${dp} ]
	then
	    echo "Please create the path for ${d}"
	    exit 1
	else
            mkdir ${d}
	fi
    fi
    if [ ! -d ${d} ]
    then
	echo "Please create the path for ${d}"
	exit 1
    fi
fi
d=$d/bin
if [ ! -d ${d} ]
then
    mkdir ${d}
fi
d=$d/make
if [ ! -d ${d} ]
then
    mkdir ${d}
fi
if [ ! -d ${d} ]
then
     echo "Please create the path for ${d}"
     exit 1
fi

if [ $MAKE = "YES" ]
# bootstrap make program in current environment
then
    MAKETOP=${SOURCEBASE}
    MAKESUB=$subdir/bin/make/
    export MAKETOP MAKESUB

    (cd ${objdir}$subdir/bin/make; sh -x ${MAKETOP}${MAKESUB}bootstrap.sh)
    cp ${objdir}$subdir/bin/make/odemake ${bindir}/odemake
    rm -rf ${objdir}/$subdir/bin/make
fi
MAKEOPTIONS="${MAKEOPTIONS} -r"
MAKE=odemake

# 
#  Copy the *.mk files to ${export}/lib/mk
#
libmkdir=${libdir}/mk
if [ -d ${libmkdir} ]
then
    true
else
    mkdir ${libmkdir}
fi
rm -f ${libmkdir}/*.mk
(cd ../mk; tar -cf - *.mk | (cd ${libmkdir}; tar -xvf -))

#
# create program to support object directory path searches
#
(cd ../bin/genpath; \
 $MAKE ${MAKEOPTIONS}  LIBS= genpath)
cp ${objdir}$subdir/bin/genpath/genpath ${bindir}/genpath
rm -rf ${objdir}$subdir/bin/genpath

#
# program to create paths for files
#
(cd ../bin/makepath; $MAKE ${MAKEOPTIONS} LIBS= makepath)
cp ${objdir}$subdir/bin/makepath/makepath ${bindir}/makepath
rm -rf ${objdir}$subdir/bin/makepath

#
# program to install in non-sandbox environment
#
(cd ../bin/release; \
 $MAKE ${MAKEOPTIONS} OFILES="release.o" LIBS= release)
cp ${objdir}$subdir/bin/release/release ${bindir}/release
rm -rf ${objdir}$subdir/bin/release

#
# md - make dependency post-processor
#

(cd ../bin/md; \
 $MAKE ${MAKEOPTIONS} LIBS=  md)
cp ${objdir}$subdir/bin/md/md ${bindir}/md
rm -rf ${objdir}$subdir/bin/md

#
# wh - file to find program on PATH
#

(cd ../bin/wh; \
  $MAKE ${MAKEOPTIONS} LIBS=  wh )
cp ${objdir}$subdir/bin/wh/wh ${bindir}/wh
rm -rf ${objdir}$subdir/bin/wh

#
#  copy the setvar scripts to ${export}/etc/setup
#
etcsetdir=${etcdir}/setup
if [ -d ${etcsetdir} ]
then
    true
else
    mkdir ${etcsetdir}
fi
if [ -d ${etcsetdir}/${context} ]
then
    true
else
    mkdir ${etcsetdir}/${context}
fi
rm -rf ${etcsetdir}/setvar
cp setvar.csh ${etcsetdir}/setvar
chmod +x ${etcsetdir}/setvar
rm -f ${etcsetdir}/${context}/setvar.csh
cp ${context}/setvar.csh ${etcsetdir}/${context}/setvar.csh

exit 0
