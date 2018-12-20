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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_cots_recs.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: endpoint for local "pipe-style" connection-oriented endpoints
 *		-- record-level I/O
 *
 * HISTORY
 * $Log:	pipenet_cots_recs.cc,v $
 * Revision 2.4  94/07/13  17:21:29  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:35:03  dpj
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  01:31:07  dpj]
 * 
 * Revision 2.2  91/11/06  14:21:21  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:54:01  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:02  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:19  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:59:04  dpj]
 * 
 *
 */

#ifndef lint
char * pipenet_cots_recs_rcsid = "$Header: pipenet_cots_recs.cc,v 2.4 94/07/13 17:21:29 mrt Exp $";
#endif	lint

#include	<pipenet_cots_recs_ifc.h>
#include	<recordstream_ifc.h>


/*
 * Initial (default) buffer size.
 */
int	pipenet_cots_recs_bufsize = 100;

DEFINE_CLASS_MI(pipenet_cots_recs)
DEFINE_CASTDOWN2(pipenet_cots_recs, pipenet_cots_base, pipenet_recio)

void pipenet_cots_recs::init_class(usClass* class_obj)
{
	usRecIO::init_class(class_obj);
	pipenet_cots_base::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_cots_recs);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_recs,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_recs,io_read1rec_seq);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_recs,io_write1rec_seq);
	END_SETUP_METHOD_WITH_ARGS;
}

pipenet_cots_recs::pipenet_cots_recs(ns_mgr_id_t mgr_id, 
				     access_table *access_tab,
				     pipenet_dir_base *_parent_dir,
				     net_addr_t *_localaddr,
				     net_addr_t *_peeraddr,
				     net_endpt_base *_connector,
				     net_endpt_base *_peerobj,
				     default_iobuf_mgr *_iobuf_mgr)
:
 pipenet_cots_base(mgr_id, access_tab, _parent_dir, _localaddr, _peeraddr,
		   _connector, _peerobj,
		   new recordstream(_iobuf_mgr, pipenet_cots_recs_bufsize,
				    IOS_ENABLED | IOS_WAIT_ALLOWED,
				    IOS_ENABLED | IOS_WAIT_ALLOWED))
{
	net_options_null_init(&Local(null_options));
}


pipenet_cots_recs::~pipenet_cots_recs()
{
	DESTRUCTOR_GUARD();
	net_options_destroy(&Local(null_options));
}


char* pipenet_cots_recs::remote_class_name() const
{
	return "usNetCOTS_recs_proxy";
}

/*
 * Exported client interface.
 */

mach_error_t 
pipenet_cots_recs::io_read1rec_seq(io_mode_t mode, char *buf,
				   unsigned int *len, io_recnum_t *recnum)
{
	mach_error_t		ret;

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_read1rec_seq(mode, buf, len, recnum);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	return(ret);
}


mach_error_t 
pipenet_cots_recs::io_write1rec_seq(io_mode_t mode, char *buf, 
				    unsigned int len, io_recnum_t *recnum)
{
	mach_error_t		ret;

	if (costate != NET_COSTATE_CONNECTED) {
		return(NET_NOT_CONNECTED);
	}

	/*
	 * Send the local address and options, because the other
	 * end might be a CLTS endpoint.
	 */
	pipenet_recio *p = pipenet_recio::castdown(peerobj);
	if (p) {
		ret = p->pipenet_clts_write1rec_upcall(mode, buf, len, recnum,
						    &localaddr, &null_options);
	} else {
		ret = _notdef();
	}
	return(ret);
}


mach_error_t 
pipenet_cots_recs::io_getrec_seq(io_mode_t mode, io_record_t *rec,
				 io_count_t *count, io_recnum_t *recnum)
{
	mach_error_t		ret;

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_getrec_seq(mode, rec, count, recnum);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return(ret);
}


mach_error_t 
pipenet_cots_recs::io_putrec_seq(io_mode_t mode, io_record_t *rec,
				 io_count_t *count, io_recnum_t *recnum)
{
	mach_error_t		ret;

	if (*count != 1) {
		us_internal_error("io_putrec_seq() with count > 1",
							US_NOT_IMPLEMENTED);
		return(US_NOT_IMPLEMENTED);
	}

	if (costate != NET_COSTATE_CONNECTED) {
		return(NET_NOT_CONNECTED);
	}

	/*
	 * Send the local address and options, because the other
	 * end might be a CLTS endpoint.
	 */

	pipenet_recio *p = pipenet_recio::castdown(peerobj);
	if (p) {
		ret = p->pipenet_clts_put1rec_upcall(mode, *rec, recnum, 
						     &localaddr,&null_options);
	} else {
		ret = _notdef();
	}
	return(ret);
}


mach_error_t 
pipenet_cots_recs::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

//	ret = invoke_super(Super,mach_method_id(ns_get_attributes),
//							attr,attrlen);
	ret = tmp_agency::ns_get_attributes(attr,attrlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	attr->type = NST_COTS_RECS;

	/*
	 * XXX Should return the number of available records.
	 */

	return(ERR_SUCCESS);
}


/*
 * Upcalls from the other side of the connection.
 */

mach_error_t 
pipenet_cots_recs::pipenet_clts_write1rec_upcall(io_mode_t mode, char *buf,
						 unsigned int len,
						 io_recnum_t *recnum,
						 net_addr_t *addr,
						 net_options_t *options)
{
	mach_error_t		ret;

	/*
	 * Ignore the address and options in connection-oriented operation.
	 */

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_write1rec_seq(mode, buf, len, recnum);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return(ret);
}


mach_error_t 
pipenet_cots_recs::pipenet_clts_put1rec_upcall(io_mode_t mode, io_record_t rec,
					       io_recnum_t *recnum, 
					       net_addr_t *addr,
					       net_options_t *options)
{
	mach_error_t		ret;
	io_count_t		count = 1;

	/*
	 * Ignore the address and options in connection-oriented operation.
	 */

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_putrec_seq(mode, &rec, &count, recnum);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return(ret);
}

mach_error_t 
pipenet_cots_recs::io_read1rec(io_mode_t, io_recnum_t, char*,
			       unsigned int*)
{
	return _notdef();
}

mach_error_t 
pipenet_cots_recs::io_write1rec(io_mode_t, io_recnum_t, char*,
					  unsigned int)
{
	return _notdef();
}

mach_error_t pipenet_cots_recs::io_get_record_count(io_recnum_t*)
{
	return _notdef();
}

