/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/net_endpt_base.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Common base class for network service endpoints.
 *
 * HISTORY
 * $Log:	net_endpt_base.cc,v $
 * Revision 2.3  94/07/07  17:23:50  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:47:15  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:57:28  pjg]
 * 
 * Revision 2.2  91/05/05  19:26:53  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:15  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:13:43  dpj]
 * 
 */

#ifndef lint
char * net_endpt_base_rcsid = "$Header: net_endpt_base.cc,v 2.3 94/07/07 17:23:50 mrt Exp $";
#endif	lint

#include	<net_endpt_base_ifc.h>


#define BASE tmp_agency
DEFINE_ABSTRACT_CLASS(net_endpt_base)


net_endpt_base::net_endpt_base(ns_mgr_id_t mgr_id, access_table *acc_tab)
	: tmp_agency(mgr_id,acc_tab)
{}


#ifdef	NOTDEF
mach_error_t 
net_endpt_base::ns_create_agent(ns_access_t access, std_cred* cred_obj, 
				agent** newobj)
{
	mach_error_t		ret;

	/*
	 * Access check.
	 */
       	ret = ns_check_access(access,cred_obj);
	if (ret != NS_SUCCESS) {
		*newobj = NULL;
		return(ret);
	}

	/*
	 * Create the agent.
	 */
	*newobj = new agent(this, cred_obj, access_tab, access, ret);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(*newobj);
		*newobj = NULL;
		return(ret);
	}

	return(ERR_SUCCESS);
}
#endif	NOTDEF


