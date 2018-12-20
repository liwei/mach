/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they made and grant Carnegie Mellon the
 * rights to redistribute these changes.
 */
/*
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/us_tm_tgrp.cc,v $
 *
 * usTask: abstract class defining the task protocol.
 *
 * All operations are defined here as returning MACH_OBJECT_NO_SUCH_OPERATION.
 * They should be redefined in the subclasses.
 *
 * HISTORY
 * $Log:	us_tm_tgrp.cc,v $
 * Revision 2.4  94/07/07  17:25:31  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/06  07:53:57  dpj
 * 	Use numeric method ids for RPC instead of method names.
 * 
 * Revision 2.2  92/07/05  23:31:32  dpj
 * 	Implement new us_tm_tgrp_ifc.h interfaces for the C++ taskmaster.
 * 	Replaces some code from us_task.cc.
 * 	[92/06/24  16:58:15  jms]
 * 
 * 	Created.
 * 	[91/04/14  18:44:01  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  16:05:55  pjg]
 * 
 * Revision 2.2  91/11/06  14:09:35  jms
 * 	Upgraded to US41.
 * 	[91/10/07  13:55:57  pjg]
 * 
 */

#ifndef lint
char * us_tm_tgrp_rcsid = "$Header: us_tm_tgrp.cc,v 2.4 94/07/07 17:25:31 mrt Exp $";
#endif	lint

#include <us_tm_tgrp_ifc.h>

#define BASE usItem
DEFINE_ABSTRACT_CLASS(usTMTgrp);

DEFINE_METHOD_ARGS(tm_get_tgrp_id, "rpc K<100701>: OUT * int;");
DEFINE_METHOD_ARGS(tm_event_to_tgrp, "rpc intr K<100702>: IN int; IN int; IN int; IN int; OUT *int;");

