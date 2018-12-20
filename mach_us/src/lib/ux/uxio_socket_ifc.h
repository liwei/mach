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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_socket_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * 
 * Purpose:  Interface for U*X sockets
 * 
 * HISTORY
 * $Log:	uxio_socket_ifc.h,v $
 * Revision 2.5  94/07/08  16:02:11  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  94/01/11  17:50:53  jms
 * 	Add "select" and "probe" logic for "real" select.
 * 	[94/01/09  19:51:12  jms]
 * 
 * Revision 2.3  91/11/06  14:12:36  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:19:27  pjg]
 * 
 * Revision 2.2  91/05/05  19:28:58  dpj
 * 	Merged up to US39
 * 	[91/05/04  10:02:24  dpj]
 * 
 * 	First working version.
 * 	[91/04/28  10:33:30  dpj]
 * 
 */

#ifndef	_uxio_socket_ifc_h
#define	_uxio_socket_ifc_h


#include <uxio_ifc.h>
#include <us_net_name_ifc.h>

extern "C" {
#include <net_types.h>
}


/*
 * Socket state.
 */
typedef int				sostate_t;
#define	SOSTATE_READY			0x1
#define	SOSTATE_BOUND			0x2
#define	SOSTATE_CONNECTED		0x4
#define	SOSTATE_READ_ENABLED		0x8
#define	SOSTATE_WRITE_ENABLED		0x10
#define	SOSTATE_LISTENING		0x20


/*
 * Special info for sockets in the AF_UNIX family.
 */
struct uxsock_af_unix {
	struct {
		char			path[200];
		net_addr_t		addr;
	} 			cache;
	usName			*basedir;
	ns_name_t		leafname;
};


class uxio_socket: public uxio {
	usNetName			*dir_proxy;
	ns_path_t			dir_path;
	unsigned int			dir_pathlen;
	int				sotype;		/* socket type */
	int				sofamily;	/* address family */
	sostate_t			sostate;
	int				prot_data[DEFAULT_NS_PROT_LEN];
	ns_prot_t			prot;
	int				protlen;
	net_addr_t			localaddr;
	net_addr_t			peeraddr;
	net_options_t			null_options;
	struct uxsock_af_unix		af_unix;
      public:
	DECLARE_LOCAL_MEMBERS(uxio_socket);
	uxio_socket(usNetName *dir_proxy, ns_path_t dir_path, int sotype,
		    int sofamily, ns_prot_t prot, int protlen, mach_error_t*);
	~uxio_socket();

      private:
	mach_error_t uxsock_cvt_sockaddr_internal(struct sockaddr *name,
						  int namelen,
						  net_addr_t *addr);
	mach_error_t uxsock_cvt_netaddr_internal(net_addr_t *addr,
						 struct sockaddr *name, 
						 int *namelen);
	mach_error_t uxsock_get_clts_internal();
	mach_error_t uxsock_get_cots_internal();
	mach_error_t uxsock_accept_internal(usItem *, net_addr_t *localaddr,
					    net_addr_t *peeraddr);

      public:
	virtual mach_error_t ux_bind(struct sockaddr *name, int namelen);
	virtual mach_error_t ux_connect(struct sockaddr *name, int namelen);
	virtual mach_error_t ux_shutdown(int how);
	virtual mach_error_t ux_listen(int backlog);
	virtual mach_error_t ux_accept(struct sockaddr *name, 
				       int *namelen,	
				       uxio **newobj	);
	virtual mach_error_t ux_getsockname(struct sockaddr *, int *);
	virtual mach_error_t ux_getpeername(struct sockaddr *, int *);
	virtual mach_error_t ux_setsockopt(int, int, char *, int);
	virtual mach_error_t ux_getsockopt(int, int, char *, int *);
	virtual mach_error_t ux_sendto(char *, int *, int, struct sockaddr *,
				       int);
	virtual mach_error_t ux_send(char *buf, int *len, int flags);
	virtual mach_error_t ux_sendmsg(struct msghdr *msg, int flags);
	virtual mach_error_t ux_recvfrom(char			*buf,
					 int			*len,	
					 int			flags,
					 struct sockaddr	*from,	
					 int			*fromlen);
	virtual mach_error_t ux_recv(char *buf, int *len, int flags);
	virtual mach_error_t ux_recvmsg(struct msghdr *msg, int flags);
	virtual mach_error_t ux_read(char			* buf,
				     unsigned int		* len);
	virtual mach_error_t ux_write (char			* buf,
				       unsigned int		* len);
	virtual mach_error_t ux_readv(struct iovec		* iov,
				      int			iovcnt,
				      int			* len);
	virtual mach_error_t ux_writev(struct iovec		* iov,
				       int			iovcnt,
				       unsigned int		* len);
	virtual mach_error_t ux_ftruncate(unsigned int		len);
	virtual mach_error_t ux_lseek(int			* pos,
				      unsigned int		mode);
	virtual mach_error_t ux_modify_protection(int		uid,
						  int		gid,
						  int		mode);
	virtual mach_error_t ux_map(task_t		task,
				    vm_address_t	*addr,
				    vm_size_t		size,
				    vm_offset_t		mask,
				    boolean_t		anywhere,
				    vm_offset_t		paging_offset,
				    boolean_t		copy,
				    vm_prot_t		cprot,
				    vm_prot_t		mprot,
				    vm_inherit_t		inherit);

	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_complete();
	virtual mach_error_t clone_abort(mach_port_t);

	/* Method to do probe on uxio_obj, may wait */
	virtual mach_error_t uxio_probe_internal(int active_select_index);

      protected:
	/* Return routine for cthread fork to call inorder to call the probe */
	virtual cthread_fn_t uxio_fork_probe_routine_internal();
};

#endif	_uxio_socket_ifc_h
