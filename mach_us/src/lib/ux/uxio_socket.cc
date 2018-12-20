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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_socket.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose:  The implementation for IO socket objects
 * 
 * HISTORY: 
 * $Log:	uxio_socket.cc,v $
 * Revision 2.5  94/07/08  16:02:08  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  94/01/11  17:50:43  jms
 * 	Add "select" and "probe" logic for "real" select.
 * 	Add tcp logic.
 * 	Correct socket permissions.
 * 	General KP.
 * 	[94/01/09  19:50:25  jms]
 * 
 * Revision 2.3  92/07/05  23:32:51  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:31:51  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:25:14  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:20:05  dpj]
 * 
 * Revision 2.2  91/11/06  14:12:28  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:19:08  pjg]
 * 
 * Revision 2.2  91/05/05  19:28:54  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:18  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:33:03  dpj]
 * 
 */

#include <uxio_socket_ifc.h>
#include <std_name_ifc.h>
#include <us_recio_ifc.h>
#include <us_net_clts_ifc.h>
#include <us_net_cots_ifc.h>
#include <us_net_connector_ifc.h>
#include <clone_ifc.h>
#include <uxio.h>

extern "C" {

#include <base.h>
#include <io_types.h>
#include <io_types2.h>
#include <ns_types.h>


#include <sys/errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

}


/*
 * Debugging switch.
 */
int	socket_debug = 1;


extern "C" any_t uxio_socket_forked_probe(any_t arg);

#define BASE uxio
DEFINE_LOCAL_CLASS(uxio_socket)

uxio_socket::uxio_socket(usNetName *_dir_proxy, ns_path_t dir_path, int sotype,
			 int sofamily, ns_prot_t prot, int protlen,
			 mach_error_t *ret)
{
	*ret = ERR_SUCCESS;

	Local(dir_proxy) = _dir_proxy;
	mach_object_reference(_dir_proxy);

	Local(dir_pathlen) = strlen(dir_path);
	bcopy(dir_path,Local(dir_path),Local(dir_pathlen));
	if (Local(dir_path)[Local(dir_pathlen) - 1] != '/') {
		Local(dir_path)[Local(dir_pathlen)] = '/';
		Local(dir_pathlen)++;
	}
	Local(dir_path)[Local(dir_pathlen)] = '\0';

	Local(sotype) = sotype;
	Local(sofamily) = sofamily;
	Local(sostate) = SOSTATE_READ_ENABLED | SOSTATE_WRITE_ENABLED;
	Local(prot) = (ns_prot_t)Local(prot_data);
	bcopy(prot,Local(prot),protlen * sizeof(int));
	Local(protlen) = protlen;
	switch (sofamily) {
	case AF_INET:
		net_addr_inet_init_default(&Local(localaddr));
		net_addr_inet_init_default(&Local(peeraddr));
		break;
	case AF_UNIX:
		net_addr_pipe_init_default(&Local(localaddr));
		net_addr_pipe_init_default(&Local(peeraddr));
		Local(af_unix).cache.path[0] = '\0';
		net_addr_pipe_init_default(&Local(af_unix).cache.addr);
		break;
	default:
		*ret = unix_err(EADDRNOTAVAIL);
		return;
	}
	net_options_null_init(&Local(null_options));

	Local(af_unix).basedir = NULL;

	(void) ux_set_sequential_internal();
}


uxio_socket::~uxio_socket()
{
	mach_object_dereference(Local(dir_proxy));
	Local(dir_proxy) = NULL;

	if ((Local(sofamily) == AF_UNIX) && (Local(sostate) & SOSTATE_BOUND)) {
		(void) af_unix.basedir->ns_remove_entry(af_unix.leafname);
	}
	mach_object_dereference(Local(af_unix).basedir);
	Local(af_unix).basedir = NULL;

	net_addr_destroy(&Local(localaddr));
	net_addr_destroy(&Local(peeraddr));
	net_options_destroy(&Local(null_options));

	if (Local(sofamily) == AF_UNIX) {
		net_addr_destroy(&Local(af_unix).cache.addr);
	}
}



/*
 * Support functions.
 */

/*
 * Convert a UNIX-format address to a standard net_addr_t.
 */
mach_error_t 
uxio_socket::uxsock_cvt_sockaddr_internal(struct sockaddr *name,
					  int namelen,
					  net_addr_t *addr/* OUT */)
{
#if CHECK_BIND_FAMILY
	/* Applications assume this field is not checked and don't set it */
	if (name->sa_family != Local(sofamily)) {
		return(unix_err(EAFNOSUPPORT));
	}
#endif CHECK_BIND_FAMILY

	switch (Local(sofamily)) {
	case AF_INET:
		{
			struct sockaddr_in	*inaddr =
						(struct sockaddr_in *)name;
			union {
				struct in_addr		in;
				ipaddr_t		net;
				long			lng;
			}			ipaddr;

			net_addr_destroy(addr);
			net_addr_inet_init_default(addr);
			if (inaddr->sin_addr.s_addr != INADDR_ANY) {
				ipaddr.lng = inaddr->sin_addr.s_addr;
				net_addr_inet_set_ipaddr(addr,ipaddr.net);
			}
			if (inaddr->sin_port != 0) {
				net_addr_inet_set_port(addr,inaddr->sin_port);
			}
		}
		return(ERR_SUCCESS);

	case AF_UNIX:
		{
			struct sockaddr_un	*unaddr =
						(struct sockaddr_un *)name;
			extern std_name	*prefix_obj;
			usItem		*aobj =0;
			ns_type_t		type;
			mach_error_t		ret;

			/*
			 * Check for default address.
			 */
			if (unaddr->sun_path[0] == '\0') {
				net_addr_destroy(addr);
				net_addr_pipe_init_default(addr);
				return(ERR_SUCCESS);
			}

			/*
			 * Check for direct pipenet address.
			 */
			if (((namelen - sizeof(short)) > Local(dir_pathlen)) &&
				(! bcmp(unaddr->sun_path,
					Local(dir_path),Local(dir_pathlen)))) {
				ret = net_addr_pipe_set_stringname(addr,
					&unaddr->sun_path[Local(dir_pathlen)]);
				if (ret != ERR_SUCCESS) {
					return(ret);
				}
			}

			/*
			 * Look in our cache.
			 *
			 * XXX There is no cache invalidation mechanism.
			 */
			/* XXX LOCKING */
			if (! strcmp(unaddr->sun_path,
						Local(af_unix).cache.path)) {
				net_addr_copy(&Local(af_unix).cache.addr,addr);
				return(ERR_SUCCESS);
			}

			/*
			 * Translate the address by finding the endpoint
			 * (through whatever transparent symlinks) and
			 * getting its "real" address.
			 */
			ret = prefix_obj->ns_resolve_fully(unaddr->sun_path,
						NSF_FOLLOW_ALL,NSR_GETATTR,
						&aobj,&type,NULL);
			if (ret != ERR_SUCCESS) {
				return(ret);
			}
			usNetBase *net_obj = usNetBase::castdown(aobj);
			if (net_obj) {
				ret = net_obj->net_get_localaddr(addr);
			} else {
				ret = MACH_OBJECT_NO_SUCH_OPERATION;
			}
			mach_object_dereference(aobj);
			if (ret != ERR_SUCCESS) {
				us_internal_error(
			"uxsock_cvt_sockaddr_internal.ns_get_localaddr()",
									ret);
				return(NET_INVALID_ADDR_VALUE);
			}
			/* XXX LOCKING */
			strcpy(Local(af_unix).cache.path,unaddr->sun_path);
			net_addr_destroy(&Local(af_unix).cache.addr);
			net_addr_copy(addr,&Local(af_unix).cache.addr);
			return(ERR_SUCCESS);
		}

	default:
		{
			char		errmsg[200];
			sprintf(errmsg,"cvt_sockaddr -- address family = %d",
							Local(sofamily));
			us_internal_error(errmsg,US_NOT_IMPLEMENTED);
		}
		return(US_NOT_IMPLEMENTED);
	}
}


/*
 * Convert a standard net_addr_t to a UNIX-format address.
 */
mach_error_t 
uxio_socket::uxsock_cvt_netaddr_internal(net_addr_t *addr, 
					 struct sockaddr *name, /* OUT */
					 int *namelen	/* INOUT */)
{
	mach_error_t		ret;

	switch (Local(sofamily)) {
	case AF_INET:
		if (!net_addr_inet_p(addr)) return(unix_err(EADDRNOTAVAIL));
		{
			struct sockaddr_in	*inaddr =
						(struct sockaddr_in *)name;
			union {
				struct in_addr		in;
				ipaddr_t		net;
				long			lng;
			}			ipaddr;

			if (*namelen < sizeof(struct sockaddr_in))
				return(US_INVALID_BUFFER_SIZE);

			inaddr->sin_family = AF_INET;
			*namelen = sizeof(struct sockaddr_in);
			ipaddr.net = net_addr_inet_get_ipaddr(addr);
			inaddr->sin_addr.s_addr = ipaddr.lng;
			inaddr->sin_port = net_addr_inet_get_port(addr);
		}
		return(ERR_SUCCESS);

	case AF_UNIX:
		if (!net_addr_pipe_p(addr)) return(unix_err(EADDRNOTAVAIL));
		{
			struct sockaddr_un	*unaddr =
						(struct sockaddr_un *)name;

			if (*namelen < sizeof(struct sockaddr_un))
				return(US_INVALID_BUFFER_SIZE);

			unaddr->sun_family = AF_UNIX;
			*namelen = sizeof(struct sockaddr_un);
			bcopy(Local(dir_path),unaddr->sun_path,
							Local(dir_pathlen));
			ret = net_addr_pipe_get_stringname(addr,
				&unaddr->sun_path[Local(dir_pathlen)],100);
			if (ret != ERR_SUCCESS) {
				return(ret);
			}
		}
		return(ERR_SUCCESS);

	default:
		{
			char		errmsg[200];
			sprintf(errmsg,"cvt_netaddr -- address family = %d",
							Local(sofamily));
			us_internal_error(errmsg,US_NOT_IMPLEMENTED);
		}
		return(US_NOT_IMPLEMENTED);
	}
}


/*
 * Get a CLTS endpoint with the local address specified in the object.
 *
 * If the local address is initially under-specified, update it
 * to reflect the full address assigned by the network server.
 */
mach_error_t uxio_socket::uxsock_get_clts_internal()
{
	mach_error_t		ret;
	ns_access_t		base_access;
	ns_access_t		access;
	ns_access_t		orig_access;
	usItem			*endpt =0;
	ns_type_t		type;
	int			qmax = 0;
	net_info_t		info;
	int			retry_count = 3;

	net_info_null_init(&info);

	switch (Local(sotype)) {
	case SOCK_DGRAM:
//		base_access = NSR_READ | NSR_WRITE | NSR_INSERT | NSR_GETATTR;
		base_access = NSR_INSERT | NSR_GETATTR;
		break;
	case SOCK_STREAM:
		base_access = NSR_INSERT | NSR_GETATTR;
		break;
	}

retry:
	/*
	 * First assume the endpoint does not yet exist.
	 */
	access = base_access | NSR_ADMIN;
	orig_access = (Local(prot))->acl[0].rights;
	(Local(prot))->acl[0].rights |= access;

	ret = dir_proxy->net_create(&Local(localaddr),&qmax,
				    Local(prot),Local(protlen),
				    access,&endpt,&type,&info);
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,(0,"net_create: %s",
						mach_error_string(ret)));
		(Local(prot))->acl[0].rights = orig_access;

		if (ret != NS_ENTRY_EXISTS) {
			return(ret);
		}

		/*
		 * There is already an endpoint with this address.
		 * Try looking it up.
		 */
		access = base_access;
		ret = dir_proxy->net_lookup(&Local(localaddr),
					    access,&endpt,&type,&info);
		if (ret != ERR_SUCCESS) {
			DEBUG0(socket_debug,(0,"net_lookup: %s",
						mach_error_string(ret)));
			if (ret != NS_NOT_FOUND) {
				return(ret);
			}

			/*
			 * We must be in a race with some other client
			 * around the same endpoint. Try again...
			 */
			if (retry_count-- > 0) {
				goto retry;
			} else {
				us_internal_error(
					"uxsock_get_clts: create/lookup race",
					US_INTERNAL_ERROR);
				return(US_INTERNAL_ERROR);
			}
		}
	}

	switch (Local(sotype)) {
	case SOCK_DGRAM:
		if (type != NST_CLTS_RECS) {
			us_internal_error("uxsock_get_clts: invalid type",
							US_INTERNAL_ERROR);
			mach_object_dereference(endpt);
			return(US_INTERNAL_ERROR);
		}
		break;

	case SOCK_STREAM:
		if (type != NST_CONNECTOR) {
			us_internal_error("uxsock_get_clts: invalid type",
							US_INTERNAL_ERROR);
			mach_object_dereference(endpt);
			return(US_INTERNAL_ERROR);
		}
		break;
	}

	/*
	 * Note: ux_open() automatically releases any previous
	 * endpoint if needed (SOCK_DGRAM et al).
	 */
	ret = ux_open(endpt,O_RDWR,access);
	mach_object_dereference(endpt);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}
	if (Local(sofamily) == AF_UNIX) {
		Local(sostate) |= SOSTATE_READY;
	} else {
		Local(sostate) |= SOSTATE_READY | SOSTATE_BOUND;
	}
	Local(sostate) &= ~SOSTATE_CONNECTED;


	return(ERR_SUCCESS);
}


/*
 * Get a COTS endpoint with the local and peer addresses specified
 * in the object
 *
 * If the local address is initially under-specified, update it
 * to reflect the full address assigned by the network server.
 */
mach_error_t uxio_socket::uxsock_get_cots_internal()
{
	mach_error_t		ret;
	ns_access_t		base_access;
	ns_access_t		access;
	usItem		*endpt =0;
	ns_type_t		type;
	net_info_t		info;
	unsigned int		udatalen;
	int			retry_count = 3;

	net_info_null_init(&info);

	switch (Local(sotype)) {
	case SOCK_DGRAM:
		base_access = NSR_READ | NSR_WRITE | NSR_GETATTR;
		break;
	case SOCK_STREAM:
		base_access = NSR_READ | NSR_WRITE | NSR_GETATTR;
		break;
	}

retry:

	/*
	 * First assume the endpoint does not yet exist.
	 */
//	access = base_access | NSR_ADMIN;
	access = base_access;
	udatalen = 0;
	usNetConnector *p = usNetConnector::castdown(obj);
	if (p) {
		ret = p->net_connect(&Local(peeraddr),&Local(null_options),
				     NULL,0,NULL,&udatalen,
				     Local(prot),Local(protlen),
				     access,&endpt,&type);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
 	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,(0,"net_connect: %s",
						mach_error_string(ret)));
 		if (ret != NS_ENTRY_EXISTS) {
			return(ret);
 		}

		/*
 		 * There is already an endpoint with this address.
  		 * Try looking it up.
  		 */
		access = base_access;
 		ret = dir_proxy->net_cots_lookup(&Local(localaddr),
						 &Local(peeraddr),
						 access,&endpt,&type,&info);
		if (ret != ERR_SUCCESS) {
			DEBUG0(socket_debug,(0,"net_cots_lookup: %s",
						mach_error_string(ret)));
			if (ret != NS_NOT_FOUND) {
				return(ret);
			}

			/*
			 * We must be in a race with some other client
			 * around the same endpoint. Try again...
			 */
			if (retry_count-- > 0) {
				goto retry;
			} else {
				us_internal_error(
					"uxsock_get_cots: create/lookup race",
					US_INTERNAL_ERROR);
				return(US_INTERNAL_ERROR);
			}
		}
	}

	switch (Local(sotype)) {
	case SOCK_DGRAM:
		if (type != NST_COTS_RECS) {
			us_internal_error("uxsock_get_cots: invalid type",
							US_INTERNAL_ERROR);
			mach_object_dereference(endpt);
			return(US_INTERNAL_ERROR);
		}
		break;

	case SOCK_STREAM:
		if (type != NST_COTS_BYTES) {
			us_internal_error("uxsock_get_cots: invalid type",
							US_INTERNAL_ERROR);
			mach_object_dereference(endpt);
			return(US_INTERNAL_ERROR);
		}
		break;
	}

	/*
	 * Note: ux_open() automatically releases any previous
	 * endpoint if needed (SOCK_DGRAM et al).
	 */
	ret = ux_open(endpt,O_RDWR,access);
	mach_object_dereference(endpt);
	if (ret != ERR_SUCCESS) {
		return(ret);
	}
	if (Local(sofamily) == AF_UNIX) {
		Local(sostate) |= SOSTATE_READY | SOSTATE_CONNECTED;
	} else {
		Local(sostate) |=
			SOSTATE_READY | SOSTATE_BOUND | SOSTATE_CONNECTED;
	}

	return(ERR_SUCCESS);
}


/*
 * Finish an accept() by telling the newly-created socket about
 * the world.
 */
mach_error_t 
uxio_socket::uxsock_accept_internal(usItem *endpt, net_addr_t *localaddr,
				    net_addr_t *peeraddr)
{
	mach_error_t		ret;

	if (Local(sofamily) == AF_UNIX) {
		Local(sostate) = SOSTATE_READY |
				SOSTATE_CONNECTED |
				SOSTATE_READ_ENABLED | SOSTATE_WRITE_ENABLED;
	} else {
		Local(sostate) = SOSTATE_READY | SOSTATE_BOUND |
				SOSTATE_CONNECTED |
				SOSTATE_READ_ENABLED | SOSTATE_WRITE_ENABLED;
	}
	net_addr_copy(localaddr,&Local(localaddr));
	net_addr_copy(peeraddr,&Local(peeraddr));

	ret = ux_open(endpt,O_RDWR,
		      NSR_READ | NSR_WRITE | NSR_GETATTR);
//		      NSR_READ | NSR_WRITE | NSR_GETATTR | NSR_ADMIN);

	return(ret);
}


/*
 * UNIX system calls.
 */

mach_error_t uxio_socket::ux_bind(struct sockaddr *name, int namelen)
{
	mach_error_t		ret;

	if (Local(sostate) & SOSTATE_BOUND) {
		DEBUG0(socket_debug,(0,"ux_bind: already bound"));
		return(unix_err(EINVAL));
	}

#if CHECK_BIND_FAMILY
	/* Applications assume this field is not checked and don't set it */
	if (name->sa_family != Local(sofamily)) {
		return(unix_err(EAFNOSUPPORT));
	}
#endif CHECK_BIND_FAMILY

	if (Local(sofamily) == AF_UNIX) {
		struct sockaddr_un	*unaddr = (struct sockaddr_un *)name;
		extern std_name	*prefix_obj;
		ns_path_t		basename;
		ns_type_t		type;
		struct sockaddr_un	real_unaddr;
		unsigned int		real_unaddrlen;
		usItem 			*aobj =0;
		/*
		 * Prepare the info for setting a transparent symlink
		 * for this endpoint.
		 */
		path(unaddr->sun_path,basename,Local(af_unix).leafname);
		if (Local(af_unix).basedir != NULL) {
			mach_object_dereference(Local(af_unix).basedir);
			Local(af_unix).basedir = NULL;
		}
		ret = prefix_obj->ns_resolve_fully(basename,NSF_FOLLOW_ALL,
					NSR_INSERT,
//					NSR_INSERT | NSR_DELETE | NSR_ADMIN,
					&aobj,&type,NULL);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
		if (type != NST_DIRECTORY) {
			mach_object_dereference(aobj);
			Local(af_unix).basedir = NULL;
			return(unix_err(ENOENT));
		}
		af_unix.basedir = usName::castdown(aobj);
		if (af_unix.basedir == 0) {
			mach_object_dereference(aobj);
			return(MACH_OBJECT_NO_SUCH_OPERATION);
		}
		/*
		 * Make sure we have an endpoint, regardless of the
		 * AF_UNIX name.
		 */
		if (! (Local(sostate) & SOSTATE_READY)) {
			ret = uxsock_get_clts_internal();
			if (ret != ERR_SUCCESS) {
				return(ret);
			}
		}

		/*
		 * Establish the "real" name.
		 */
		real_unaddrlen = sizeof(struct sockaddr_un);
		ret = uxsock_cvt_netaddr_internal(&Local(localaddr),
					       (struct sockaddr*)&real_unaddr,
					       &real_unaddrlen);
		if (ret != ERR_SUCCESS) {
			us_internal_error("ux_bind: cannot make real name",
									ret);
			return(ret);
		}

		/*
		 * Set-up a transparent symlink.
		 */
		ret = af_unix.basedir->ns_create_transparent_symlink(
						Local(af_unix).leafname,
						Local(prot),Local(protlen),
						real_unaddr.sun_path);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
	} else {
		ret = uxsock_cvt_sockaddr_internal(name,namelen,
						   &Local(localaddr));
		if (ret != ERR_SUCCESS) {
			DEBUG0(socket_debug,
				(0,"ux_bind cannot convert address: %s",
						mach_error_string(ret)));
			return(ret);
		}

		ret = uxsock_get_clts_internal();
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
	}

	Local(sostate) |= SOSTATE_BOUND;

	return(ERR_SUCCESS);
}


mach_error_t 
uxio_socket::ux_connect(struct sockaddr *name, int namelen)
{
	mach_error_t		addr_ret;
	mach_error_t		ret;

	/*
	 * XXX Should update transparent symlink to the endpoint
	 * in AF_UNIX case.
	 */

	addr_ret = uxsock_cvt_sockaddr_internal(name,namelen,
						&Local(peeraddr));
	if (addr_ret == ERR_SUCCESS) {
		switch(Local(sofamily)) {
		case AF_INET:
			if (net_addr_inet_ipaddr_default_p(&Local(peeraddr))
			   || net_addr_inet_port_default_p(&Local(peeraddr))) {
				addr_ret = unix_err(EADDRNOTAVAIL);
			}
			break;
		case AF_UNIX:
			if (net_addr_pipe_default_p(&Local(peeraddr))) {
				addr_ret = unix_err(EADDRNOTAVAIL);
			}
			break;
		default:
			{
				char		errmsg[200];
				sprintf(errmsg,
					"ux_connect() -- address family = %d",
					Local(sofamily));
				us_internal_error(errmsg,US_INTERNAL_ERROR);
			}
			return(US_INTERNAL_ERROR);
		}
	}

	if (Local(sostate) & SOSTATE_CONNECTED) {
		switch (Local(sotype)) {
		case SOCK_DGRAM:
			/*
			 * OK to reconnect. Use invalid address to
			 * disconnect.
			 */
			if (addr_ret != ERR_SUCCESS) {
				ret = uxsock_get_clts_internal();
				if (ret != ERR_SUCCESS) {
					return(ret);
				}
				return(ERR_SUCCESS);
			} else
				break;
		default:
			DEBUG0(socket_debug,
				(0,"ux_connect: already connected"));
			return(unix_err(EISCONN));
		}
	}

	if (addr_ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,
				(0,"ux_connect cannot convert address: %s",
						mach_error_string(ret)));
		return(addr_ret);
	}

	if (! (Local(sostate) & SOSTATE_READY)) {
		ret = uxsock_get_clts_internal();
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
	}

	ret = uxsock_get_cots_internal();
	if (ret != ERR_SUCCESS) {
		return(ret);
	}

	return(ERR_SUCCESS);
}


mach_error_t uxio_socket::ux_shutdown(int how)
{
	mach_error_t		ret;

#ifdef	notdef
	/*
	 * Believe it or not, BSD lets you shutdown() an un-bound,
	 * un-connected socket...
	 */
	if (! (Local(sostate) & SOSTATE_CONNECTED)) {
		return(unix_err(ENOTCONN));
	}
#endif	notdef

	/*
	 * XXX Need to notify the network server even when shutting-down
	 * only one direction?
	 *
	 * Use ns_duplicate() to update the access rights?
	 */

	switch(how) {
	case 0:
		Local(sostate) &= ~SOSTATE_READ_ENABLED;
		break;
	case 1:
		Local(sostate) &= ~SOSTATE_WRITE_ENABLED;
		break;
	case 2:
		Local(sostate) &= 
			~(SOSTATE_READ_ENABLED | SOSTATE_WRITE_ENABLED);
		usNetCOTS *p = usNetCOTS::castdown(obj);
		if (p) {
			ret = p->net_snddis(NULL,0);
		} else {
			ret = MACH_OBJECT_NO_SUCH_OPERATION;
		}
		if (ret != ERR_SUCCESS) {
			us_internal_error("ux_shutdown.net_snddis()",ret);
			/*
			 * XXX Ignore the problem for now.
			 */
		}
		break;
	default:
		return(unix_err(EINVAL));
	}

	return(ERR_SUCCESS);
}


mach_error_t uxio_socket::ux_listen(int backlog)
{
	mach_error_t		ret;

	if (Local(sostate) & SOSTATE_CONNECTED) {
		return(unix_err(EISCONN));
	}

	if ((Local(sostate) & SOSTATE_BOUND) == 0) {
		return(unix_err(EINVAL));
	}

	Local(sostate) |= SOSTATE_LISTENING;
	Local(sostate) &= ~(SOSTATE_READ_ENABLED | SOSTATE_WRITE_ENABLED);

	usNetConnector *p = usNetConnector::castdown(obj);
	if (p) {
		ret = p->net_set_connect_qmax(backlog);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,
				(0,"ux_listen:net_set_connect_qmax(): %s",
				mach_error_string(ret)));
		return(ret);
	}

	return(ERR_SUCCESS);
}


mach_error_t 
uxio_socket::ux_accept(struct sockaddr *name, /* OUT */
		       int *namelen,	/* OUT */
		       uxio **newobj	/* OUT */)
{
	mach_error_t		ret;
	net_addr_t		peeraddr;
	net_options_t		options;
	unsigned int		udatalen;
	int			seqno;
	usItem		*endpt =0;
	ns_type_t		type;

#define	ABORT(_ret) {					\
	mach_object_dereference(endpt);			\
	net_addr_destroy(&peeraddr);			\
	net_options_destroy(&options);			\
	return(_ret);					\
}

	if ((Local(sostate) & SOSTATE_LISTENING) == 0) {
		return(unix_err(EINVAL));
	}

	net_addr_null_init(&peeraddr);
	net_options_null_init(&options);

	/*
	 * First wait to get a connection request.
	 */
	udatalen = 0;
	usNetConnector *p = usNetConnector::castdown(obj);
	if (p) {
		ret = p->net_listen(IOM_WAIT,&peeraddr,&options,
				    NULL,&udatalen,&seqno);
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,(0,"ux_accept:net_listen(): %s",
						mach_error_string(ret)));
		return(ret);
	}

	/*
	 * Unconditionally accept the connection.
	 */
	ret = p->net_accept(seqno,&options,NULL,0,
			    Local(prot),Local(protlen),
			    NSR_READ | NSR_WRITE | NSR_GETATTR,
//			    NSR_READ | NSR_WRITE | NSR_GETATTR | NSR_ADMIN,
			    &endpt,&type);
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,(0,"ux_accept:net_accept(): %s",
						mach_error_string(ret)));
		ABORT(ret);
	}

	/*
	 * Translate the new peer address.
	 */
	ret = uxsock_cvt_netaddr_internal(&peeraddr,name,namelen);
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,(0,"ux_accept:cvt_netaddr(): %s",
						mach_error_string(ret)));
		ABORT(ret);
	}

	/*
	 * Create a new socket object.
	 */
	uxio_socket *s = new uxio_socket(Local(dir_proxy),Local(dir_path),
					 Local(sotype),Local(sofamily),
					 Local(prot),Local(protlen), &ret);
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,(0,"ux_accept:uxio_socket(): %s",
						mach_error_string(ret)));
		ABORT(ret);
	}
	ret = s->uxsock_accept_internal(endpt,&Local(localaddr),&peeraddr);
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,
				(0,"ux_accept:uxsock_accept_internal(): %s",
				mach_error_string(ret)));
		ABORT(ret);
	}
	*newobj = s;

	mach_object_dereference(endpt);
	net_addr_destroy(&peeraddr);
	net_options_destroy(&options);

	return(ERR_SUCCESS);

#undef	ABORT
}


mach_error_t 
uxio_socket::ux_getsockname(struct sockaddr *name, int 	*namelen)
{
	mach_error_t		ret;

	ret = uxsock_cvt_netaddr_internal(&Local(localaddr),name,namelen);
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,
			(0,"ux_getsockname cannot convert address: %s",
					mach_error_string(ret)));
		return(ret);
	}

	return(ERR_SUCCESS);
}


mach_error_t 
uxio_socket::ux_getpeername(struct sockaddr *name, int *namelen)
{
	mach_error_t		ret;

	if (! (Local(sostate) & SOSTATE_CONNECTED)) {
		return(unix_err(ENOTCONN));
	}

	ret = uxsock_cvt_netaddr_internal(&Local(peeraddr),name,namelen);
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,
			(0,"ux_getpeername cannot convert address: %s",
					mach_error_string(ret)));
		return(ret);
	}

	return(ERR_SUCCESS);
}


mach_error_t 
uxio_socket::ux_setsockopt(int level, int name, char *val, int valsize)
{
	char			errmsg[200];

#if SET_SOCK_OPT_FAIL
	switch(level) {
	case SOL_SOCKET:
		switch(name) {
		default:
			sprintf(errmsg,
			"ux_setsockopt(level=SOL_SOCKET,name=%d)",name);
			us_internal_error(errmsg,US_NOT_IMPLEMENTED);
			return(US_NOT_IMPLEMENTED);
		}
		break;
	}

	sprintf(errmsg,"ux_setsockopt(level=%d,name=%d)",level,name);
	us_internal_error(errmsg,US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
#else SET_SOCK_OPT_FAIL
	return(ERR_SUCCESS);
#endif SET_SOCK_OPT_FAIL
}


mach_error_t 
uxio_socket::ux_getsockopt(int level, int name, char *val, int *valsize)
{
	char			errmsg[200];

	switch(level) {
	case SOL_SOCKET:
		switch(name) {
		case SO_TYPE:
			if (*valsize < sizeof(int)) {
				return(US_INVALID_BUFFER_SIZE);
			}
			bcopy(&Local(sotype),val,sizeof(int));
			*valsize = sizeof(int);
		default:
			sprintf(errmsg,
			"ux_getsockopt(level=SOL_SOCKET,name=%d)",name);
			us_internal_error(errmsg,US_NOT_IMPLEMENTED);
			return(US_NOT_IMPLEMENTED);
		}
		break;
	}

	sprintf(errmsg,"ux_getsockopt(level=%d,name=%d)",level,name);
	us_internal_error(errmsg,US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
}


mach_error_t 
uxio_socket::ux_sendto(char *buf, int *len,/* INOUT */
		       int flags, struct sockaddr *to, int tolen)
{
	mach_error_t		ret;
	unsigned int		mode;
	io_recnum_t		recnum;

	if (! (Local(sostate) & SOSTATE_WRITE_ENABLED)) {
		return(IO_REJECTED);
	}

	if (Local(sostate) & SOSTATE_CONNECTED) {
		return(unix_err(EISCONN));
	}

	if (! (Local(sostate) & SOSTATE_READY)) {
		ret = uxsock_get_clts_internal();
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
	}

	if (flags != 0) {
		char			errmsg[200];
		sprintf(errmsg,"ux_sendto: invalid flags: 0x%x",flags);
		us_internal_error(errmsg,US_NOT_IMPLEMENTED);
		return(US_NOT_IMPLEMENTED);
	}

	ret = uxsock_cvt_sockaddr_internal(to,tolen,&Local(peeraddr));
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,
				(0,"ux_sendto cannot convert address: %s",
						mach_error_string(ret)));
		return(ret);
	}

	mode = ((flags & FILE_NDELAY) ? 0 : IOM_WAIT);

	usNetCLTS *p = usNetCLTS::castdown(obj);
	if (p) {
		ret = p->net_clts_write1rec(mode,buf,*len,&recnum,
					&Local(peeraddr),&Local(null_options));
	} else {
		ret = MACH_OBJECT_NO_SUCH_OPERATION;
	}
	if (ret != ERR_SUCCESS) {
		DEBUG0(socket_debug,
				(0,"ux_sendto:net_clts_write1rec(): %s",
						mach_error_string(ret)));
		return(ret);
	}

	return(ERR_SUCCESS);
}


mach_error_t uxio_socket::ux_send(char *buf, int *len,	/* INOUT */
				  int flags)
{
	mach_error_t		ret;
	unsigned int		mode;
	io_recnum_t		recnum;

	if (! (Local(sostate) & SOSTATE_WRITE_ENABLED)) {
		return(IO_REJECTED);
	}

	if (! (Local(sostate) & SOSTATE_CONNECTED)) {
		return(unix_err(EDESTADDRREQ));
	}

	if (flags != 0) {
		char			errmsg[200];
		sprintf(errmsg,"ux_send: invalid flags: 0x%x",flags);
		us_internal_error(errmsg,US_NOT_IMPLEMENTED);
		return(US_NOT_IMPLEMENTED);
	}

	switch(Local(sotype)) {
	case SOCK_DGRAM:
		mode = ((flags & FILE_NDELAY) ? 0 : IOM_WAIT);

		usRecIO *p = usRecIO::castdown(obj);
		if (p) {
			ret = p->io_write1rec_seq(mode,buf,*len,&recnum);
		} else {
			ret = MACH_OBJECT_NO_SUCH_OPERATION;
		}
		if (ret != ERR_SUCCESS) {
			DEBUG0(socket_debug,
					(0,"ux_send:io_write1rec_seq(): %s",
						mach_error_string(ret)));
			return(ret);
		}
		break;
	case SOCK_STREAM:
		return(uxio::ux_write(buf,len));
	}

	return(ERR_SUCCESS);
}


mach_error_t 
uxio_socket::ux_sendmsg(struct msghdr *msg, int flags)
{
	us_internal_error("ux_sendmsg",US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
}


mach_error_t 
uxio_socket::ux_recvfrom(char			*buf,
			 int			*len,	/* INOUT */
			 int			flags,
			 struct sockaddr	*from,	/* OUT */
			 int			*fromlen)	/* OUT */
{
	mach_error_t		ret;
	unsigned int		mode;
	io_recnum_t		recnum;
	net_addr_t		fromaddr;
	net_options_t		options;

	net_addr_null_init(&fromaddr);
	net_options_null_init(&options);

	if (! (Local(sostate) & SOSTATE_READ_ENABLED)) {
		return(IO_REJECTED);
	}

	if (! (Local(sostate) & SOSTATE_READY)) {
		ret = uxsock_get_clts_internal();
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
	}

	if (flags != 0) {
		/* XXX PEEK */

		char			errmsg[200];
		sprintf(errmsg,"ux_recvfrom: invalid flags: 0x%x",flags);
		us_internal_error(errmsg,US_NOT_IMPLEMENTED);
		return(US_NOT_IMPLEMENTED);
	}

	mode = ((flags & FILE_NDELAY) ? 0 : IOM_WAIT);

	if (Local(sostate) & SOSTATE_CONNECTED) {
		switch(Local(sotype)) {
		case SOCK_DGRAM:
			usRecIO *p = usRecIO::castdown(obj);
			if (p) {
				ret = p->io_read1rec_seq(mode,buf,len,&recnum);
			} else {
				ret = MACH_OBJECT_NO_SUCH_OPERATION;
			}
			if (ret != ERR_SUCCESS) {
				DEBUG0(socket_debug,
				(0,"ux_recvfrom:io_read1rec_seq(): %s",
						mach_error_string(ret)));
				return(ret);
			}
			break;
		case SOCK_STREAM:
			ret = uxio::ux_read(buf,len);
			if (ret != ERR_SUCCESS) {
				DEBUG0(socket_debug,
				(0,"ux_recvfrom:ux_read(): %s",
						mach_error_string(ret)));
				return(ret);
			}
			break;
		}

		net_addr_copy(&Local(peeraddr),&fromaddr);
	} else {
		usNetCLTS *p = usNetCLTS::castdown(obj);
		if (p) {
			ret = p->net_clts_read1rec(mode,
					buf,len,&recnum,&fromaddr,&options);
		} else {
			ret = MACH_OBJECT_NO_SUCH_OPERATION;
		}
		if (ret != ERR_SUCCESS) {
			DEBUG0(socket_debug,
				(0,"ux_recvfrom:net_clts_read1rec(): %s",
						mach_error_string(ret)));
			return(ret);
		}
	}

	/* XXX buffer too small */

	if (from != NULL) {
		ret = uxsock_cvt_netaddr_internal(&fromaddr,from,fromlen);
		if (ret != ERR_SUCCESS) {
			DEBUG0(socket_debug,
				(0,"ux_recvfrom cannot convert address: %s",
						mach_error_string(ret)));
			return(ret);
		}
	}

	return(ERR_SUCCESS);
}


mach_error_t 
uxio_socket::ux_recv(char *buf, int *len,/* INOUT */
		     int flags)
{
	mach_error_t		ret;
	unsigned int		mode;
	io_recnum_t		recnum;

	if (! (Local(sostate) & SOSTATE_READ_ENABLED)) {
		return(IO_REJECTED);
	}

	if (! (Local(sostate) & SOSTATE_READY)) {
		ret = uxsock_get_clts_internal();
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
	}

	if (flags != 0) {
		/* XXX PEEK */

		char			errmsg[200];
		sprintf(errmsg,"ux_recv: invalid flags: 0x%x",flags);
		us_internal_error(errmsg,US_NOT_IMPLEMENTED);
		return(US_NOT_IMPLEMENTED);
	}

	mode = ((flags & FILE_NDELAY) ? 0 : IOM_WAIT);

	if (Local(sostate) & SOSTATE_CONNECTED) {
		switch(Local(sotype)) {
		case SOCK_DGRAM:
			usRecIO *p = usRecIO::castdown(obj);
			if (p) {
				ret = p->io_read1rec_seq(mode,buf,len,&recnum);
			} else {
				ret = MACH_OBJECT_NO_SUCH_OPERATION;
			}
			if (ret != ERR_SUCCESS) {
				DEBUG0(socket_debug,
					(0,"ux_recv:io_read1rec_seq(): %s",
						mach_error_string(ret)));
				return(ret);
			}
			break;
		case SOCK_STREAM:
			return(uxio::ux_read(buf,len));
		}
	} else {
		net_addr_t		fromaddr;
		net_options_t		options;

		net_addr_null_init(&fromaddr);
		net_options_null_init(&options);

		usNetCLTS *p = usNetCLTS::castdown(obj);
		if (p) {
			ret = p->net_clts_read1rec(mode,
					buf,len,&recnum,&fromaddr,&options);
		} else {
			ret = MACH_OBJECT_NO_SUCH_OPERATION;
		}
		if (ret != ERR_SUCCESS) {
			DEBUG0(socket_debug,
				(0,"ux_recv:net_clts_read1rec(): %s",
						mach_error_string(ret)));
			return(ret);
		}
	}

	/* XXX buffer too small */

	return(ERR_SUCCESS);
}


mach_error_t uxio_socket::ux_recvmsg(struct msghdr *msg, int flags)
{
	us_internal_error("ux_recvmsg",US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
}


mach_error_t uxio_socket::ux_read(char			* buf,
				  unsigned int		* len)
{
	char			errmsg[200];

	if (! (Local(sostate) & SOSTATE_READ_ENABLED)) {
		return(IO_REJECTED);
	}

	switch(Local(sotype)) {
	case SOCK_DGRAM:
		return(ux_recv(buf,len,0));
	case SOCK_STREAM:
		return(uxio::ux_read(buf,len));
	}

	sprintf("ux_read() -- socket type = %d",Local(sotype));
	us_internal_error(errmsg,US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
}


mach_error_t uxio_socket::ux_write (char			* buf,
				    unsigned int		* len)
{
	char			errmsg[200];

	if (! (Local(sostate) & SOSTATE_WRITE_ENABLED)) {
		return(IO_REJECTED);
	}

	switch(Local(sotype)) {
	case SOCK_DGRAM:
		return(ux_send(buf,len,0));
	case SOCK_STREAM:
		return(uxio::ux_write(buf,len));
	}

	sprintf("ux_write() -- socket type = %d",Local(sotype));
	us_internal_error(errmsg,US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
}


mach_error_t uxio_socket::ux_readv(struct iovec		* iov,
				   int			iovcnt,
				   int			* len)
{
	char			errmsg[200];

	if (! (Local(sostate) & SOSTATE_READ_ENABLED)) {
		return(IO_REJECTED);
	}

	switch(Local(sotype)) {
	case SOCK_DGRAM:
		us_internal_error("ux_readv on SOCK_DGRAM socket",
							US_NOT_IMPLEMENTED);
		return(US_NOT_IMPLEMENTED);
	case SOCK_STREAM:
		return(uxio::ux_readv(iov,iovcnt,len));
	}

	sprintf(errmsg,"ux_readv() -- socket type = %d",Local(sotype));
	us_internal_error(errmsg,US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
}


mach_error_t uxio_socket::ux_writev(struct iovec		* iov,
				    int			iovcnt,
				    unsigned int		* len)
{
	char			errmsg[200];

	if (! (Local(sostate) & SOSTATE_WRITE_ENABLED)) {
		return(IO_REJECTED);
	}

	switch(Local(sotype)) {
	case SOCK_DGRAM:
		us_internal_error("ux_writev on SOCK_DGRAM socket",
							US_NOT_IMPLEMENTED);
		return(US_NOT_IMPLEMENTED);
	case SOCK_STREAM:
		return(uxio::ux_writev(iov,iovcnt,len));
	}

	sprintf(errmsg,"ux_writev() -- socket type = %d",Local(sotype));
	us_internal_error(errmsg,US_NOT_IMPLEMENTED);
	return(US_NOT_IMPLEMENTED);
}


mach_error_t uxio_socket::ux_ftruncate(unsigned int		len)
{
	return(US_UNSUPPORTED);
}


mach_error_t uxio_socket::ux_lseek(int			* pos,
				   unsigned int		mode)
{
        return(US_UNSUPPORTED);
}


mach_error_t uxio_socket::ux_modify_protection(int			uid,
					       int			gid,
					       int			mode)
{
        return(US_UNSUPPORTED);
}


uxio_socket::ux_map(task_t			task,
		    vm_address_t		*addr,
		    vm_size_t		size,
		    vm_offset_t		mask,
		    boolean_t		anywhere,
		    vm_offset_t		paging_offset,
		    boolean_t		copy,
		    vm_prot_t		cprot,
		    vm_prot_t		mprot,
		    vm_inherit_t		inherit)
{
        return(US_UNSUPPORTED);
}

/*
 * Select support
 */

/*
 * return the function to be used by cthread_fork to correctly castdown
 * the "this" to the right type after the fork.
 */
cthread_fn_t 
uxio_socket::uxio_fork_probe_routine_internal()
{
	return(uxio_socket_forked_probe);
}

any_t
uxio_socket_forked_probe(any_t arg)
{
	uxio_socket		*sobj;
	int			sindex;
	mach_error_t		ret = ERR_SUCCESS;
	
	/* Copy to shrink the memory leak window if there is a ux_fork before
		this probe is done XXX */
	sobj = uxio_socket::castdown(((uxio_fork_rec_t)arg)->self_obj);
	sindex = ((uxio_fork_rec_t)arg)->active_select_index;
	free((uxio_fork_rec_t)arg);

	ret = sobj->uxio_probe_internal(sindex);

	/* We have done our thing, time to die (by just returning)*/
	return(ERR_SUCCESS);
}

/*
 * uxio_probe_internal
 *  Overrides the default definition from uxio to handle waiting on
 *  connectors.  Code should be kept in sync with that from "uxio.cc"
 */
mach_error_t
uxio_socket::uxio_probe_internal(int active_select_index)
{
	usNetConnector *connector;
	mach_error_t	ret = ERR_SUCCESS;
	net_addr_t		peeraddr;
	net_options_t		options;
	unsigned int		udatalen;
	int			seqno;

	io_mode_t		mode;
	ux_select_type_t	select_type;

	mode = ((Local(active_selects))[active_select_index]).mode;
	select_type = ((Local(active_selects))[active_select_index]).select_type;
	mode |= IOM_PROBE;
	connector = usNetConnector::castdown(obj);
	if (! connector) {
	    /* We are not a connector, just call the default */
	    ret = uxio::uxio_probe_internal(active_select_index);
	    return(ret);
	}
	/* XXX no wait mode? Crap? */

	/*
	 * Wait for the connection.
	 * Note: unix defines a select on a connector to be done when
	 *	the connection is established, not when there is
	 *	a ready "read/write/except"
	 */
	ret = connector->net_listen(mode,&peeraddr,&options,
				    NULL,&udatalen,&seqno);
	DEBUG0(TRUE,(0, "uxio_socket::uxio_probe_internal, probe_done %d\n", active_select_index));
	ux_select_one_done(active_select_index, ret);
	DEBUG0(TRUE,(0, "uxio_socket::uxio_probe_internal, sel_one_done %d\n", active_select_index));
	return(ret);
}

/*
 * Cloning support.
 */
mach_error_t uxio_socket::clone_init(mach_port_t child)
{
	if (Local(dir_proxy) != NULL) {
		if (0 == usClone::castdown(dir_proxy)) {
			us_internal_error("uxio_socket::clone_init: null dir proxy castdown: suspending",
							US_INTERNAL_ERROR);
			task_suspend(mach_task_self());
		}
		(void) usClone::castdown(dir_proxy)->clone_init(child);
	}

	if (Local(af_unix).basedir != NULL){
		(void) usClone::castdown(Local(af_unix).basedir)->clone_init(child);
	}

	return uxio::clone_init(child);
}

mach_error_t uxio_socket::clone_complete()
{
	if (Local(dir_proxy) != NULL){
		(void) usClone::castdown(dir_proxy)->clone_complete();
	}

	if (Local(af_unix).basedir != NULL){
		(void) usClone::castdown(Local(af_unix).basedir)->clone_complete();
	}

	return uxio::clone_complete();
}

mach_error_t uxio_socket::clone_abort(mach_port_t child)
{
	if (Local(dir_proxy) != NULL){
		(void) usClone::castdown(dir_proxy)->clone_abort(child);
	}

	if (Local(af_unix).basedir != NULL){
		(void) usClone::castdown(Local(af_unix).basedir)->clone_abort(child);
	}

	return uxio::clone_abort(child);
}

