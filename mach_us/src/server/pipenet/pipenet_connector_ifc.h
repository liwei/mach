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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_connector_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Connector for local "pipe-style" connections.
 *
 * HISTORY
 * $Log:	pipenet_connector_ifc.h,v $
 * Revision 2.6  94/07/13  17:21:15  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/05/17  14:10:09  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 	[94/04/29  13:40:25  jms]
 * 
 * Revision 2.4  92/07/05  23:34:56  dpj
 * 	Fixed argument types for pipenet_terminate_connection().
 * 	Added dummy pipenet_snddis_upcall() to avoid undefined virtual.
 * 	[92/04/17  16:54:00  dpj]
 * 
 * Revision 2.3  91/11/06  14:20:43  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:53:07  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:45  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:50  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:56:51  dpj]
 * 
 */

#ifndef	_pipenet_connector_ifc_h
#define	_pipenet_connector_ifc_h

#include	<us_net_connector_ifc.h>
#include	<pipenet_endpt_base_ifc.h>
#include	<pipenet_internal.h>

class pipenet_dir_base;

// XXX making usNetConnector virtual crashes g++-1.37
class pipenet_connector: public pipenet_endpt_base,
			 public /* virtual */ usNetConnector {
			 
	struct mutex		lock;
	dll_head_t		queue;
	int			qmax;
	int			qsize;
	int			seqno_seed;
	struct condition	listen_cond;
	net_addr_t		localaddr;
	pipenet_dir_base	*parent_dir;
      public:
	DECLARE_MEMBERS(pipenet_connector);
	pipenet_connector();
	pipenet_connector(ns_mgr_id_t, access_table *, pipenet_dir_base *, 
			  net_addr_t *, int);
	virtual ~pipenet_connector();
	virtual char* remote_class_name() const;

REMOTE	virtual mach_error_t ns_get_attributes(ns_attr_t, int *);

REMOTE	virtual mach_error_t net_get_localaddr(net_addr_t *);
REMOTE	virtual mach_error_t net_connect(net_addr_t*, net_options_t*,
					 char*, unsigned int, char*,
					 unsigned int*, ns_prot_t,
					 unsigned int, ns_access_t,
					 usItem**, ns_type_t*);
REMOTE	virtual mach_error_t net_listen(io_mode_t, net_addr_t*, 
					net_options_t*, char*,
					unsigned int*, int*);
REMOTE	virtual mach_error_t net_accept(int, net_options_t*, char*,
					unsigned int, ns_prot_t,
					unsigned int, ns_access_t,
					usItem**, ns_type_t*);
REMOTE	virtual mach_error_t net_reject(int, char*, unsigned int);
REMOTE	virtual mach_error_t net_get_connect_qinfo(int*, int*);
REMOTE	virtual mach_error_t net_set_connect_qmax(int);

	virtual mach_error_t pipenet_connect_upcall(pipenet_conninfo_t);
	virtual mach_error_t pipenet_terminate_connection(net_addr_t *,
							  net_endpt_base *);

	virtual mach_error_t pipenet_snddis_upcall(
					net_addr_t * a1,
					char * a2,
					unsigned int a3) 
				{ return _notdef(); };

};

#endif	_pipenet_connector_ifc_h
