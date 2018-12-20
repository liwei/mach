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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_clts_recs.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: local "pipe-style" connection-less endpoint and pseudo-connector
 *		-- record-level I/O
 *
 * HISTORY
 * $Log:	pipenet_clts_recs.cc,v $
 * Revision 2.4  94/07/13  17:20:56  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:34:42  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:30:19  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:39:52  dpj]
 * 
 * Revision 2.2  91/11/06  14:20:22  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:52:15  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:33  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:07:14  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:55:16  dpj]
 * 
 *
 */

#ifndef lint
char * pipenet_clts_recs_rcsid = "$Header: pipenet_clts_recs.cc,v 2.4 94/07/13 17:20:56 mrt Exp $";
#endif	lint

#include	<pipenet_clts_recs_ifc.h>
#include	<pipenet_recio_ifc.h>
#include	<recordstream_ifc.h>

extern "C" {
#include	<io_types2.h>
}


/*
 * Initial (default) buffer size.
 */
int	pipenet_clts_recs_bufsize = 100;

DEFINE_CLASS_MI(pipenet_clts_recs)
DEFINE_CASTDOWN2(pipenet_clts_recs, pipenet_recio, pipenet_clts_base)


void pipenet_clts_recs::init_class(usClass* class_obj)
{
	pipenet_clts_base::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_clts_recs);
	/* 
	 * These should be in pipenet_clts_base, but that needs the
	 * new definition of the macro that doesn't work
	 * due to the error in pointers to members in g++. 
	 */
	SETUP_METHOD_WITH_ARGS(pipenet_clts_recs,net_get_localaddr);
	SETUP_METHOD_WITH_ARGS(pipenet_clts_recs,net_connect);
	/*
	 * These belong here
	 */
	SETUP_METHOD_WITH_ARGS(pipenet_clts_recs,net_clts_read1rec);
	SETUP_METHOD_WITH_ARGS(pipenet_clts_recs,net_clts_write1rec);
	SETUP_METHOD_WITH_ARGS(pipenet_clts_recs,ns_get_attributes);
	END_SETUP_METHOD_WITH_ARGS;
}

char* pipenet_clts_recs::remote_class_name() const
{
	return "usNetCLTS_recs_proxy";
}

pipenet_clts_recs::pipenet_clts_recs(ns_mgr_id_t mgr_id, 
				     access_table *access_tab,
				     pipenet_dir_base *_parent_dir,
				     net_addr_t *_localaddr,
				     default_iobuf_mgr *_iobuf_mgr)
: pipenet_clts_base(mgr_id, access_tab, _parent_dir, _localaddr,
		    new recordstream(_iobuf_mgr, pipenet_clts_recs_bufsize,
				     IOS_ENABLED | IOS_WAIT_ALLOWED, 0))
{
//	recordstream *incoming_stream;

//	new_object(incoming_stream,recordstream);
//	(void) invoke(incoming_stream,mach_method_id(setup_recordstream),
//					iobuf_mgr,pipenet_clts_recs_bufsize,
//					IOS_ENABLED | IOS_WAIT_ALLOWED,0);

	
//	(void) pipenet_clts_base_setup_pipenet_clts_base(self,
//					mgr_id,access_table,parent_dir,
//					localaddr,incoming_stream);

//	mach_object_dereference(incoming_stream);

//	add_delegate(Self,iobuf_user);
//	(void) invoke(Self,mach_method_id(setup_iobuf_user),iobuf_mgr);
}


/*
 * Exported client interface.
 */

mach_error_t 
pipenet_clts_recs::net_clts_read1rec(io_mode_t mode, char *buf, 
				     unsigned int *len, io_recnum_t *recnum,
				     net_addr_t *peeraddr, 
				     net_options_t *options)
{
	mach_error_t		ret;
	pipenet_recinfo_t	recinfo;

	io_recinfo_init_default(&recinfo);

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_read1rec_seq_with_info(mode, buf, len, recnum, 
						   (io_recinfo_t*)&recinfo);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	if ((mode & IOM_PROBE) == 0) {
		net_addr_copy(&recinfo.addr,peeraddr);
		net_options_copy(&recinfo.options,options);
	}

	io_recinfo_destroy(&recinfo);

	return(ERR_SUCCESS);
}


mach_error_t 
pipenet_clts_recs::net_clts_write1rec(io_mode_t mode, char *buf,
				      unsigned int len, io_recnum_t *recnum,
				      net_addr_t *peeraddr,
				      net_options_t *options)
{
	net_endpt_base		*obj;
	mach_error_t		ret;

	FIND_PEER(peeraddr,obj,ret);
	if (ret != ERR_SUCCESS) return(ret);
	pipenet_recio *p = pipenet_recio::castdown(obj);
	if (p) {
		ret = p->pipenet_clts_write1rec_upcall(mode,buf,len,recnum,
						 &Local(localaddr),options);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	if (ret == ERR_SUCCESS) {
		RELEASE_PEER(obj);
	} else {
		CLEAR_PEER(obj);
	}

	return(ret);
}


mach_error_t 
pipenet_clts_recs::net_clts_get1rec(io_mode_t mode, io_record_t *data,
				    io_recnum_t *recnum, net_addr_t *peeraddr,
				    net_options_t *options)
{
	mach_error_t		ret;
	io_record_t		rec = NULL;
	int			count = 1;
	pipenet_recinfo_t	*recinfo;

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_getrec_seq(mode, &rec, &count, recnum);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	recinfo = (pipenet_recinfo_t *) &rec->recinfo;

	if ((mode & IOM_PROBE) == 0) {
		net_addr_copy(&recinfo->addr,peeraddr);
		net_options_copy(&recinfo->options,options);
	}

	/*
	 * No need to destroy the recinfo now; it will be taken care
	 * of when the whole record is freed.
	 */

	*data = rec;

	return(ret);
}


mach_error_t 
pipenet_clts_recs::net_clts_put1rec(io_mode_t mode, io_record_t rec,
				    io_recnum_t *recnum, net_addr_t *peeraddr,
				    net_options_t *options)
{
	net_endpt_base		*obj;
	mach_error_t		ret;

	FIND_PEER(peeraddr,obj,ret);
	if (ret != ERR_SUCCESS) return(ret);
	pipenet_recio *p = pipenet_recio::castdown(obj);
	if (p) {
		ret = p->pipenet_clts_put1rec_upcall(mode,rec,recnum,
					       &Local(localaddr),options);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	if (ret == ERR_SUCCESS) {
		RELEASE_PEER(obj);
	} else {
		CLEAR_PEER(obj);
	}

	return(ret);
}


mach_error_t 
pipenet_clts_recs::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

//	ret = invoke_super(Super,mach_method_id(ns_get_attributes),
//							attr,attrlen);
	ret = net_endpt_base::ns_get_attributes(attr,attrlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	attr->type = NST_CLTS_RECS;

	/*
	 * XXX Should return the number of available records.
	 */

	return(ERR_SUCCESS);
}


/*
 * Upcalls from the other side of the connection.
 */

mach_error_t 
pipenet_clts_recs::pipenet_clts_write1rec_upcall(io_mode_t mode, char *buf,
						 unsigned int len,
						 io_recnum_t *recnum,
						 net_addr_t *addr,
						 net_options_t *options)
{
	mach_error_t		ret;
	net_endpt_base		*obj;
	pipenet_recinfo_t	recinfo;

	FIND_CONN_ENDPT(addr,obj);
	if (obj != NULL) {
		pipenet_recio *p = pipenet_recio::castdown(obj);
		if (p) {
			ret = p->pipenet_clts_write1rec_upcall(mode,buf,len,
							 recnum,addr,options);
		} else {
			ret = MACH_OBJECT_NO_SUCH_OPERATION;
		}
		RELEASE_CONN_ENDPT(obj);
		if ((ret == ERR_SUCCESS) || (ret != IO_REJECTED))
			return(ret);
	}

	pipenet_recinfo_init(&recinfo,addr,options);

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_write1rec_seq_with_info(mode, buf, len, recnum,
						    (io_recinfo_t*)&recinfo);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	return(ret);
}


mach_error_t 
pipenet_clts_recs::pipenet_clts_put1rec_upcall(io_mode_t mode, io_record_t rec,
					       io_recnum_t *recnum,
					       net_addr_t *addr,
					       net_options_t *options)
{
	mach_error_t		ret;
	net_endpt_base		*obj;
	io_count_t		count = 1;

	FIND_CONN_ENDPT(addr,obj);
	if (obj != NULL) {
		pipenet_recio *p = pipenet_recio::castdown(obj);
		if (p) {
			ret = p->pipenet_clts_put1rec_upcall(mode,rec,recnum,
							     addr,options);
		} else {
			ret = MACH_OBJECT_NO_SUCH_OPERATION;
		}
		RELEASE_CONN_ENDPT(obj);
		if ((ret == ERR_SUCCESS) || (ret != IO_REJECTED))
			return(ret);
	}

	if (rec->infosize < sizeof(struct pipenet_recinfo)) {
		/*
		 * Need a new record. Copy the old data (block references).
		 */
		io_record_t	orec = rec;

		ret = incoming_stream->io_alloc_record(0,&rec);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
		rec->first_block = orec->first_block;
		rec->last_block = orec->last_block;
		orec->first_block = NULL;
		orec->last_block = NULL;
		(void) incoming_stream->io_free_record(orec);
	} else {
		io_recinfo_destroy(&rec->recinfo);
	}
	pipenet_recinfo_init(&rec->recinfo,addr,options);

	recordstream *r = recordstream::castdown(incoming_stream);
	if (r) {
		ret = r->io_putrec_seq(mode, &rec, &count, recnum);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return(ret);
}

mach_error_t 
pipenet_clts_recs::io_read1rec(io_mode_t, io_recnum_t, char*, unsigned int*)
{
	return _notdef();
}
mach_error_t 
pipenet_clts_recs::io_write1rec(io_mode_t, io_recnum_t, char*, unsigned int)
{
	return _notdef();
}

mach_error_t 
pipenet_clts_recs::io_read1rec_seq(io_mode_t, char*,unsigned int*,io_recnum_t*)
{
	return _notdef();
}

mach_error_t 
pipenet_clts_recs::io_write1rec_seq(io_mode_t, char*,unsigned int,io_recnum_t*)
{
	return _notdef();
}

mach_error_t 
pipenet_clts_recs::io_get_record_count(io_recnum_t*)
{
	return _notdef();
}


