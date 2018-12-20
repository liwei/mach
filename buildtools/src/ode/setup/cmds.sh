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
# @OSF_FREE_COPYRIGHT@
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
# $Log:	cmds.sh,v $
# Revision 2.2  92/05/20  20:16:23  mrt
# 	First checkin
# 	[92/05/20  14:22:27  mrt]
# 
#
#   Abstract:
#	This script can be used to make the ode subdirectory using
#	make rather than the setup.sh script. It will build all the
#	targets in the subtree rather than just selected ones.
#	It should be run before install.sh which only installs targetes
#	that are alreade built. It builds in the bootstrap envirnoment
#	e.g. -D_BLD is set.
#   Usage is:
#	cd to the directory that contains the file cmds.sh. 
#	cmds.sh  <context> 
#

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

#
# Read the configuration information for this host
#
if [ $# -ne 1 ]
then
  USAGE="usage: cmds.sh <context>"
  echo "ERROR: improper number of arguments"
  echo $USAGE
  exit 1
fi
context=$1
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

#
# Setup immediate directory heirarchy
#

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
tools=${base}/tools
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

#
# Constrain search paths
#
PATH="${bindir}:${DEFPATH}"
CPATH="${DEFCPATH}"
LPATH="${DEFLPATH}"
INCDIRS="-I${expdir}/usr/include"
if [ -n "$NEED_L_FLAG_SPACE" ]
then
  LIBDIRS="-L ${expdir}/usr/lib"
else
  LIBDIRS="-L${expdir}/usr/lib"
fi

export PATH CPATH LPATH INCDIRS LIBDIRS

#
# Site/Environment stuff
#
CENV="-D_BLD $PORTING_FLAGS"
OWNER="cs"
GROUP="cs"

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
BLD=BLD

export EXPORTBASE SOURCEBASE SOURCEDIR OBJECTDIR BLD

MAKEOPTIONS="${MAKEOPTIONE} -ri"


(cd ../bin; make ${MAKEOPTIONS} build_all)

exit 0
