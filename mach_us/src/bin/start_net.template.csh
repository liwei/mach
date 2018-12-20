#!/bin/csh -f -b
# 
# Mach Operating System
# Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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

#
# start_net.csh
#
# Purpose: start the net_server with the appropriate configuration, as
#	determined from the /etc/NETWORKS and /etc/GATEWAYS files.
#
# HISTORY
# $Log:	start_net.template.csh,v $
# Revision 2.3  94/07/13  16:30:46  mrt
# 	Added copyright.
# 	[94/07/13            mrt]
# 
# Revision 2.2  94/06/16  17:11:04  mrt
# 	Moved from utils to bin. Simplified to include host specific
# 	address rather than using NETWORKS and GATEWAYS.
# 	[94/06/01            mrt]
# 
# Revision 2.3  94/05/17  13:36:17  jms
# 	Check in mach_servers/us/lib for NETWORKS.US and not just in /etc.
# 	[94/05/11  14:57:33  modh]
# 
# Revision 2.2  94/01/11  18:13:23  jms
# 	Derived from start_xkernel
# 	[94/01/10  14:05:35  jms]
# 
# Revision 2.3  91/11/13  17:21:38  dpj
# 	Default executable for net_server in /mach_servers/us/bin
# 	[91/11/12            dpj]
# 
# Revision 2.2  91/05/05  19:36:01  dpj
# 	Merged up to US39
# 	[91/05/04  10:11:00  dpj]
# 
# 	First working version.
# 	[91/04/28  11:13:53  dpj]
# 
#

echo "Set these variables for your host and site."
set hostaddr="IP address"
set hostifc="one of pc0,et0,wd0.el0.de0"
set gateway="local gateway IP"
echo "  hostaddr=$hostaddr"
echo "  hostifc=$hostifc"
echo "  gateway=$gateway"

set stopswitch=
if ("$1" == "-stop") then
	set stopswitch=-stop
	shift
endif

set net_serv = /mach_servers/us/bin/net_server
if (! -f $net_serv) then
	set net_serv = "`wh -q net_server`"
	if ($status:q != 0) then
		echo "*** Cannot find net_server ***"
		exit 1
	endif
endif

set cmd = "$net_serv $stopswitch -I $hostifc:q -i $hostaddr:q -g $gateway:q -diag net_server $*"

echo $cmd
exec $cmd
