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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_cots_bytes.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: endpoint for local "pipe-style" connection-oriented endpoints
 *		-- byte-level I/O
 *
 * HISTORY
 * $Log:	pipenet_cots_bytes.cc,v $
 * Revision 2.3  94/07/13  17:21:24  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  91/11/06  14:20:59  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:53:40  pjg]
 * 
 * Revision 2.2  91/05/05  19:32:56  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:08:10  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:58:30  dpj]
 * 
 *
 */

#ifndef lint
char * pipenet_cots_bytes_rcsid = "$Header: pipenet_cots_bytes.cc,v 2.3 94/07/13 17:21:24 mrt Exp $";
#endif	lint

#include	<pipenet_cots_bytes_ifc.h>
#include	<bytestream_ifc.h>

/*
 * Initial (default) buffer size.
 */
int	pipenet_cots_bytes_bufsize = 4096;


DEFINE_CLASS_MI(pipenet_cots_bytes)
DEFINE_CASTDOWN2(pipenet_cots_bytes, pipenet_byteio, pipenet_cots_base)

void pipenet_cots_bytes::init_class(usClass* class_obj)
{
	pipenet_byteio::init_class(class_obj);
	pipenet_cots_base::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_cots_bytes);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_bytes,ns_get_attributes);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_bytes,io_read_seq);
	SETUP_METHOD_WITH_ARGS(pipenet_cots_bytes,io_write_seq);
	END_SETUP_METHOD_WITH_ARGS;
}

pipenet_cots_bytes::pipenet_cots_bytes(ns_mgr_id_t mgr_id, 
				       access_table *access_tab,
				       pipenet_dir_base *_parent_dir,
				       net_addr_t *_localaddr,
				       net_addr_t *_peeraddr,
				       net_endpt_base *_connector,
				       net_endpt_base *_peerobj,
				       default_iobuf_mgr *iobuf_mgr)
:
 pipenet_cots_base(mgr_id, access_tab, _parent_dir, _localaddr, _peeraddr,
		   _connector, _peerobj, 
		   new bytestream(iobuf_mgr,pipenet_cots_bytes_bufsize,
				  IOS_ENABLED | IOS_WAIT_ALLOWED,
				  IOS_ENABLED | IOS_WAIT_ALLOWED))
{}


char* pipenet_cots_bytes::remote_class_name() const
{
	return "usNetCOTS_bytes_proxy";
}

/*
 * Exported client interface.
 */

mach_error_t 
pipenet_cots_bytes::io_read_seq(io_mode_t mode, char *buf, io_count_t *count,
				io_offset_t *offset)
{
	mach_error_t		ret;

	bytestream *r = bytestream::castdown(incoming_stream);
	if (r) {
		ret = r->io_read_seq(mode, buf, count, offset);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	return(ret);
}


mach_error_t 
pipenet_cots_bytes::io_write_seq(io_mode_t mode, char *buf, io_count_t *count,
				 io_offset_t *offset)
{
	mach_error_t		ret;

	if (costate != NET_COSTATE_CONNECTED) {
		return(NET_NOT_CONNECTED);
	}
	pipenet_byteio *p = pipenet_byteio::castdown(peerobj);
	if (p) {
		ret = p->pipenet_write_upcall(mode,buf,count,offset);
	} else {
		ret = _notdef();
	}

	return(ret);
}


mach_error_t 
pipenet_cots_bytes::io_getbytes_seq(io_mode_t mode, io_block_t *blk,
				    io_count_t *count, io_offset_t *offset)
{
	mach_error_t		ret;

	bytestream *r = bytestream::castdown(incoming_stream);
	if (r) {
		ret = r->io_getbytes_seq(mode,blk,count,offset);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	return(ret);
}


mach_error_t 
pipenet_cots_bytes::io_putbytes_seq(io_mode_t mode, io_block_t *blk,
				    io_count_t *count, io_offset_t *offset)
{
	mach_error_t		ret;

	if (costate != NET_COSTATE_CONNECTED) {
		return(NET_NOT_CONNECTED);
	}

	pipenet_byteio *p = pipenet_byteio::castdown(peerobj);
	if (p) {
		ret = p->pipenet_putbytes_upcall(mode,blk,count,offset);
	} else {
		ret = _notdef();
	}

	return(ret);
}


mach_error_t 
pipenet_cots_bytes::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

//	ret = invoke_super(Super,mach_method_id(ns_get_attributes),
//							attr,attrlen);
	ret = tmp_agency::ns_get_attributes(attr,attrlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	attr->type = NST_COTS_BYTES;
	bytestream *r = bytestream::castdown(incoming_stream);
	if (r) {
		ret = r->io_get_size(&attr->size);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}

	if (ret == ERR_SUCCESS) {
		attr->valid_fields |= NS_ATTR_SIZE;
	}

	return(ERR_SUCCESS);
}


/*
 * Upcalls from the other side of the connection.
 */

mach_error_t 
pipenet_cots_bytes::pipenet_write_upcall(io_mode_t mode, char *buf,
					 io_count_t *count,
					 io_offset_t *offset)
{
	mach_error_t		ret;

	bytestream *r = bytestream::castdown(incoming_stream);
	if (r) {
		ret = r->io_write_seq(mode,buf,count,offset);

	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	return(ret);
}


mach_error_t 
pipenet_cots_bytes::pipenet_putbytes_upcall(io_mode_t mode, io_block_t *blk,
					    io_count_t *count,
					    io_offset_t *offset)
{
	mach_error_t		ret;

	bytestream *r = bytestream::castdown(incoming_stream);
	if (r) {
		ret = r->io_putbytes_seq(mode,blk,count,offset);

	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	return(ret);
}


mach_error_t 
pipenet_cots_bytes::io_read(io_mode_t, io_offset_t, pointer_t, 
				     unsigned int*)
{
	return _notdef();
}
mach_error_t 
pipenet_cots_bytes::io_write(io_mode_t, io_offset_t, pointer_t, unsigned int*)
{
	return _notdef();
}
mach_error_t 
pipenet_cots_bytes::io_append(io_mode_t, pointer_t, unsigned int*)
{
	return _notdef();
}
mach_error_t 
pipenet_cots_bytes::io_set_size(io_size_t)
{
	return _notdef();
}
mach_error_t 
pipenet_cots_bytes::io_get_size(io_size_t *)
{
	return _notdef();
}
mach_error_t 
pipenet_cots_bytes::io_map(task_t, vm_address_t*, vm_size_t,
			   vm_offset_t, boolean_t, vm_offset_t,
			   boolean_t, vm_prot_t, vm_prot_t,
			   vm_inherit_t)
{
	return _notdef();
}
       

