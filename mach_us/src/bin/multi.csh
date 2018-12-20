#!/bin/csh -x -f -b
# 
# Mach Operating System
# Copyright (c) 1994 Carnegie Mellon University
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
#  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they made and grant Carnegie Mellon the
# rights to redistribute these changes.
#

# HISTORY
# $Log:	multi.csh,v $
# Revision 2.3  94/07/13  16:30:37  mrt
# 	Added copyright.
# 	[94/07/13            mrt]
# 
# Revision 2.2  94/06/29  14:16:26  mrt
# 	Created.
# 	[94/06/27            mrt]
# 
#
#  Command to start the multi-servers and create a
#  multi-server csh talking to the window associated
#  with the pty specified by the tty arguments
#
#  usage:  multi [-l logfile] tty
#	logfile is an alternate to ~/multi.log for a log file
#	tty is of the form ttyp0 and is the tty on which the
#	   multi server shell will do I/O

set logfile = ~/multi.log
set tee_it = 1

if ($#argv == 0 ) then
  echo "usage: multi [-l logfile ] tty"
  exit
endif
if ( $argv[1] == "-l" ) then
    set tee_it = 0
    set logfile = "$argv[2]"
    shift; shift
endif
if ($#argv >= 1) then
    set tty = $argv[1]
else
  echo "usage: multi [-l logfile ] tty"
  exit
endif

setpath -i99 /mach_servers/us

set cf_cmd = "config_server /mach_servers/us/bin/emul_init -t /dev/${tty}_htg /bin/csh"

echo cleaning up old emulation...
kemul >& /dev/null

echo running...
if (! $tee_it) then
    eval $cf_cmd >& $logfile
else
    eval $cf_cmd |& tee $logfile
endif
