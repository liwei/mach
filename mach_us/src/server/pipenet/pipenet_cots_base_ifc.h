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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_cots_base_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose:	common base class for local "pipe-style" connection-oriented
 *		endpoints.
 *
 * HISTORY
 * $Log:	pipenet_cots_base_ifc.h,v $
 * Revision 2.6  94/07/13  17:21:22  mrt
 * 	Updated copyright
 * 
 * Revision 2.5  94/05/17  14:10:11  jms
 * 	Change the order of multiple inheritance of to implementation class followed
 * 	by abstract class.  This change was needed because the other order does not
 * 	work with gcc2.3.3.
 * 
 * 	Need to declare dummy  virtual functions in pipenet_cots_base for 2.3.3 g++ -modh
 * 	[94/04/29  13:41:23  jms]
 * 
 * Revision 2.4.1.1  94/02/18  11:38:04  modh
 * 	Need to declare virtual functions in pipenet_cots_base for 2.3.3 g++
 * 
 * Revision 2.4  92/07/05  23:35:01  dpj
 * 	tmp_cleanup_for_shutdown -> ns_tmp_cleanup_for_shutdown
 * 	[92/06/24  17:23:42  jms]
 * 	Added dummy routines to avoid undefined virtuals.
 * 	[92/04/17  16:54:36  dpj]
 * 
 * Revision 2.3  91/11/06  14:20:49  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:53:21  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:54  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:57  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:57:39  dpj]
 * 
 */

#ifndef	_pipenet_cots_base_ifc_h
#define	_pipenet_cots_base_ifc_h

#include	<us_net_cots_ifc.h>
#include	<pipenet_endpt_base_ifc.h>
#include	<pipenet_dir_base_ifc.h>
#include	<stream_base_ifc.h>


class pipenet_cots_base: public pipenet_endpt_base, public usNetCOTS {
      protected:
	net_costate_t		costate;
	net_addr_t		localaddr;
	net_addr_t		peeraddr;
	net_endpt_base		*connector;
	pipenet_dir_base	*parent_dir;
	net_endpt_base		*peerobj;
	stream_base		*incoming_stream;
	char			*disconn_data;
	unsigned int		disconn_datalen;
      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(pipenet_cots_base);
	pipenet_cots_base(ns_mgr_id_t =null_mgr_id, access_table * =0,
			  pipenet_dir_base * =0, net_addr_t * =0,
			  net_addr_t * =0, net_endpt_base * =0,
			  net_endpt_base * =0, stream_base * =0);
	virtual ~pipenet_cots_base();

REMOTE	virtual mach_error_t net_get_localaddr(net_addr_t*);

REMOTE	virtual mach_error_t net_get_peeraddr(net_addr_t*);
REMOTE	virtual mach_error_t net_snddis(char*, unsigned int);
REMOTE	virtual mach_error_t net_rcvdis(char*, unsigned int*);

	virtual mach_error_t pipenet_snddis_upcall(net_addr_t *, char *,
						   unsigned int);
	virtual mach_error_t ns_tmp_cleanup_for_shutdown(void);

	net_endpt_base *set_peerobj(net_endpt_base*);

#ifdef	notdef
	virtual mach_error_t pipenet_connect_upcall(pipenet_conninfo_t a)
				{ return _notdef(); };
	virtual mach_error_t pipenet_terminate_connection(
						net_addr_t * a1,
						net_endpt_base *)
				{ return _notdef(); };
#else	notdef
	virtual mach_error_t pipenet_connect_upcall(pipenet_conninfo_t a)
				{ return MACH_OBJECT_NO_SUCH_OPERATION; };
	virtual mach_error_t pipenet_terminate_connection(
						net_addr_t * a1,
						net_endpt_base *)
				{ return MACH_OBJECT_NO_SUCH_OPERATION; };
#endif
};

#endif	_pipenet_cots_base_ifc_h
