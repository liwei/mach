#!/bin/csh -f -b
# 
# Mach Operating System
# Copyright (c) 1994,1993 Carnegie Mellon University
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

# kemul.csh
#
# Purpose: 
# 	Waste all running emulated/emulator stuff
#
# HISTORY
# $Log:	kemul.csh,v $
# Revision 2.3  94/07/13  16:30:42  mrt
# 	Added copyright.
# 	[94/07/13            mrt]
# 
# Revision 2.2  94/06/29  14:16:14  mrt
# 	Moved from utils to bin.
# 	[94/06/01            mrt]
# 
# Revision 2.3  94/01/11  18:13:17  jms
# 	Kill the net_server
# 	[94/01/10  14:02:57  jms]
# 
# Revision 2.2  93/01/20  17:41:01  jms
# 	First working version
# 	[93/01/18  18:07:13  jms]
# 

ms -t | \
    egrep ' none | emul_init$| config_server$| diag_serv$| cas$| task_master$| ufs$| pathnameserver$| nameserver$| pipenet$| tty_server| net_server$' | \
    sed -e '1,/BSD/d' -e 's/^\([ ]*[0-9][0-9]*\).*/\1/' | \
    sort -nr | \
    mkill ` cat `

ms -t
ps -agxuww >& /dev/null
vmstat
