: 
: Mach Operating System
: Copyright (c) 1991-1994 Carnegie Mellon University
: All Rights Reserved.
: 
: Permission to use, copy, modify and distribute this software and its
: documentation is hereby granted, provided that both the copyright
: notice and this permission notice appear in all copies of the
: software, derivative works or modified versions, and any portions
: thereof, and that both notices appear in supporting documentation.
: 
: CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
: CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
: ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
: 
: Carnegie Mellon requests users of this software to return to
: 
:  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
:  School of Computer Science
:  Carnegie Mellon University
:  Pittsburgh PA 15213-3890
: 
: any improvements or extensions that they made and grant Carnegie Mellon the
: rights to redistribute these changes.
:
:
: HISTORY
: $Log:	STARTUP.fsadmin.template.csh,v $
: Revision 2.3  94/10/27  12:01:09  jms
: 	Fix the comment character
: 	[94/10/26  18:04:48  jms]
: 
: Revision 2.2  94/06/16  17:10:43  mrt
: 	Moved from utils/STARTUP.fsadmin.csh
: 
: 
: Revision 2.17.1.1  94/05/20  16:02:22  mrt
: 	Added comments. Removed history.
: 	[94/04/21            mrt]
: 
:	Created
: 	[90/08/17  09:57:41  roy]
: 
:
:  This script is read by fsadmin as the last step in the configuration
:  server's setup procedure. fsadmin executes the following commands
:  by making calls on the servers. The mount calls make entries for
:  the servers into the root nameserver's name space. 
:
: mount the various servers in the root nameservers space
:   arg1 is name in root nameserver's name space
:   arg2 is name in the port-name-space maintained by the
: 	configuration server. arg2 must correspond to the 
:	names used in rc.us
:
mount /ufs ufs_server
mount /slash_superroot ufs_slash
mount /slash_usr ufs_usr
mount /usr1 ufs_usr1
mount /ttys tty_server
mount /pipenet pipenet_server
mount /net net_server
mount /tm task_master
ls /
quit
