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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/pipenet/pipenet_upipe_bytes.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Standard uni-directional, byte-level pipe object.
 *
 * HISTORY:
 * $Log:	pipenet_upipe_bytes.cc,v $
 * Revision 2.3  94/07/13  17:22:03  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  91/11/06  14:23:45  jms
 * 	Initial C++ revision.
 * 	[91/09/27  15:57:08  pjg]
 * 
 * Revision 2.2  91/05/05  19:33:33  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:09:15  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  11:02:42  dpj]
 * 
 */

#ifndef lint
char * pipenet_upipe_bytes_rcsid = "$Header: pipenet_upipe_bytes.cc,v 2.3 94/07/13 17:22:03 mrt Exp $";
#endif	lint

#include	<pipenet_upipe_bytes_ifc.h>

DEFINE_CLASS_MI(pipenet_upipe_bytes)
DEFINE_CASTDOWN2(pipenet_upipe_bytes, bytestream, tmp_agency)

/*
 * Maximum size of a pipe (in bytes).
 */
int		pipenet_upipe_bytes_bufsize = 4096;



void pipenet_upipe_bytes::init_class(usClass* class_obj)
{
	bytestream::init_class(class_obj);
	tmp_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(pipenet_upipe_bytes);
	SETUP_METHOD_WITH_ARGS(pipenet_upipe_bytes,ns_get_attributes);
	END_SETUP_METHOD_WITH_ARGS;
}


pipenet_upipe_bytes::pipenet_upipe_bytes(ns_mgr_id_t mgr_id, 
					 access_table *access_tab,
					 default_iobuf_mgr *iobuf_mgr)
:
 bytestream(iobuf_mgr,pipenet_upipe_bytes_bufsize,IOS_ENABLED,0),
 tmp_agency(mgr_id, access_tab),
 num_readers(0), num_writers(0)
{
	mutex_init(&Local(lock));

//	add_delegate(Self,bytestream);
//	(void) setup_bytestream(Base,iobuf_mgr,
//				pipenet_upipe_bytes_bufsize,IOS_ENABLED,0);

}

char* pipenet_upipe_bytes::remote_class_name() const
{
	return "usByteIO_proxy";
}


mach_error_t 
pipenet_upipe_bytes::ns_get_attributes(ns_attr_t attr, int *attrlen)
{
	mach_error_t		ret;

//	ret = invoke_super(Super,mach_method_id(ns_get_attributes),
//							attr,attrlen);
	ret = tmp_agency::ns_get_attributes(attr, attrlen);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	attr->type = NST_UPIPE_BYTES;
	ret = this->io_get_size(&attr->size);
	if (ret == ERR_SUCCESS) {
		attr->valid_fields |= NS_ATTR_SIZE;
	}

	return(ERR_SUCCESS);
}


mach_error_t pipenet_upipe_bytes::ns_register_agent(ns_access_t access)
{
	mach_error_t		ret;
	mach_error_t		ret1 = ERR_SUCCESS;
	mach_error_t		ret2 = ERR_SUCCESS;

	mutex_lock(&Local(lock));

	if (access & NSR_READ) {
		Local(num_readers)++;
		if (Local(num_readers) == 1) {
			ret1 = this->io_set_write_strategy(
					IOS_ENABLED | IOS_WAIT_ALLOWED);
		}
	}

	if (access & NSR_WRITE) {
		Local(num_writers)++;
		if (Local(num_writers) == 1) {
			ret2 = this->io_set_read_strategy(
					IOS_ENABLED | IOS_WAIT_ALLOWED);
		}
	}

	mutex_unlock(&Local(lock));

	if (ret1 != ERR_SUCCESS) return(ret1);
	if (ret2 != ERR_SUCCESS) return(ret2);

//	ret = invoke_super(Super,mach_method_id(ns_register_agent),access);
	ret = tmp_agency::ns_register_agent(access);
	if (ret == ERR_SUCCESS) {
		ret = bytestream::ns_register_agent(access);
	}
	return(ret);
}


mach_error_t pipenet_upipe_bytes::ns_unregister_agent(ns_access_t access)
{
	mach_error_t		ret;
	mach_error_t		ret1 = ERR_SUCCESS;
	mach_error_t		ret2 = ERR_SUCCESS;

	mutex_lock(&Local(lock));

	if (access & NSR_READ) {
		Local(num_readers)--;
		if (Local(num_readers) == 0) {
			ret1 = this->io_set_write_strategy(0);
		}
	}

	if (access & NSR_WRITE) {
		Local(num_writers)--;
		if (Local(num_writers) == 0) {
			ret2 = this->io_set_read_strategy(IOS_ENABLED);
		}
	}

	mutex_unlock(&Local(lock));

	if (ret1 != ERR_SUCCESS) return(ret1);
	if (ret2 != ERR_SUCCESS) return(ret2);

//	ret = invoke_super(Super,mach_method_id(ns_unregister_agent),access);
	ret = bytestream::ns_unregister_agent(access);
	if (ret == ERR_SUCCESS) {
		ret = tmp_agency::ns_unregister_agent(access);
	}
	return(ret);
}
