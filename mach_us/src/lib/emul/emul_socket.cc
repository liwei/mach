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
 * File:       emul_socket.c
 * $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/emul/emul_socket.cc,v $
 *
 * Purpose:
 *	user space emulation of unix networking primitives
 *
 * HISTORY: 
 * $Log:	emul_socket.cc,v $
 * Revision 2.5  94/07/08  16:57:22  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  94/01/11  17:49:09  jms
 * 	Add tcp support.
 * 	Misc bug fixes.
 * 	[94/01/09  18:41:47  jms]
 * 
 * Revision 2.3  92/07/05  23:25:33  dpj
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  00:37:07  dpj]
 * 
 * Revision 2.2  91/11/06  11:32:48  jms
 * 	Initial C++ revision.
 * 	[91/09/26  19:36:40  pjg]
 * 
 * Revision 1.15  91/10/06  22:26:56  jjc
 * 	Changed uses of /pipenet to /server/pipenet.
 * 	Changed uses of /net to /server/net.
 * 	[91/07/22            jjc]
 * 
 * Revision 1.14  91/05/05  19:24:47  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:51:12  dpj]
 * 
 * 	Completely redone to use the standard "uxio" base objects and the new
 * 	network and I/O interfaces.
 * 	[91/04/28  09:48:45  dpj]
 * 
 */

#include <emul_base.h>
#include <uxio_socket_ifc.h>

extern "C" {
#include <mach.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <debug.h>
#include <emul_io.h>
}



/*
 * Translate error code returned from ux_* method on file/socket object.
 */
#define	emulsock_error_to_unix(_err)				\
	(((_err) == MACH_OBJECT_NO_SUCH_OPERATION)		\
		? ENOTSOCK					\
		: emul_error_to_unix(_err))


/*
 *	Initialize the package
 */
mach_error_t emul_socket_init()
{
	return(ERR_SUCCESS);
}


/*
 *	Entry points for emulated Unix syscalls
 */
mach_error_t emul_socket(int			domain,
			 int			type,
			 int			protocol,
			 syscall_val_t		*rv)
{
	char			*path = NULL;
	mach_error_t		err;
	usNetName		*dir_proxy = NULL;
	uxio			*sock_obj = NULL;
	int			sofamily;
	ns_type_t		nstype;
	int			prot_data[DEFAULT_NS_PROT_LEN];
	ns_prot_t		prot = (ns_prot_t)prot_data;
	int			protlen;

	SyscallTrace("socket");
	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"socket(%x %x %x)",
							domain,type,protocol));

	switch (domain) {
	case PF_INET:
		sofamily = AF_INET;
		switch (type) {
		case SOCK_DGRAM:
			switch (protocol) {
			case 0:
				path = "/server/net/udp";
				break;
			}
			break;
		case SOCK_STREAM:
			switch (protocol) {
			case 0:
				path = "/server/net/tcp";
				break;
			}
			break;
		}
		break;
	case PF_UNIX:
		sofamily = AF_UNIX;
		switch (type) {
		case SOCK_DGRAM:
			switch (protocol) {
			case 0:
				path = "/server/pipenet/CLTS_RECS";
				break;
			}
			break;
		case SOCK_STREAM:
			switch (protocol) {
			case 0:
				path = "/server/pipenet/COTS_BYTES";
				break;
			}
			break;
		}
		break;
	}
	if (path == NULL) {
		rv->rv_val1 = EPROTONOSUPPORT;
		err = US_UNSUPPORTED;
		goto finish;
	}

	usItem *aobj;
	err = prefix_obj->ns_resolve_fully(path,NSF_FOLLOW_ALL,
			NSR_INSERT | NSR_LOOKUP,&aobj,&nstype,NULL);
	if (err != ERR_SUCCESS) {
		ERROR((Diag,"Cannot find %s directory: %s\n",path,
						mach_error_string(err)));
		rv->rv_val1 = emul_error_to_unix(err);
		goto finish;
	}
	if ((dir_proxy = usNetName::castdown(aobj)) == 0) {
		ERROR((Diag,"emul_socket: castdown error: %s->%s\n", aobj->is_a()->class_name(), "usNetName"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

//	new_object(sock_obj,uxio_socket);
	(void) emul_io_umask(0700,prot,&protlen);
	prot->acl[0].rights = NSR_READ | NSR_WRITE | NSR_GETATTR | NSR_ADMIN;
//	err = setup_uxio_socket(sock_obj,dir_proxy,path,type,sofamily,
//								prot,protlen);
	sock_obj = new uxio_socket(dir_proxy,path,type,sofamily,prot,protlen,&err);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = emul_error_to_unix(err);
		goto finish;
	}

	err = ftab_obj->ftab_add_obj(sock_obj,&rv->rv_val1);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = emul_error_to_unix(err);
		goto finish;
	}

finish:
	mach_object_dereference(dir_proxy);
	mach_object_dereference(sock_obj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"socket() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_accept(int			s,
			 struct sockaddr	*name,
			 int			*namelen,
			 syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	uxio		*newobj = NULL;
	mach_error_t 		err;
	struct sockaddr		*iname = NULL;
	int			*inamelen = NULL;
	struct sockaddr		iname_dummy;
	int			inamelen_dummy;

	SyscallTrace("accept");

	if ((NULL != name) && (NULL != namelen)) {
		COPYIN(namelen, inamelen, 1, int, rv, EFAULT);
		COPYOUT_INIT(iname, name, *inamelen, char, rv, EFAULT);
	}
	else {
		iname = &iname_dummy;
		inamelen = &inamelen_dummy;
	}

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"accept(%x %x %x)",
							s,iname,inamelen));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"emul_accept: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_accept(iname,inamelen,&newobj);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}

	err = ftab_obj->ftab_add_obj(newobj,&rv->rv_val1);
	if (err) {
		rv->rv_val1 = emul_error_to_unix(err);
	}

finish:
	if (iname != &iname_dummy) {
		COPYOUT(inamelen, namelen, 1, int, rv, EFAULT);
		COPYOUT(iname, name, *inamelen, char, rv, EFAULT);
	}
	mach_object_dereference(tobj);
	mach_object_dereference(newobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"accept() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_bind(int			s,
		       struct sockaddr		*name,
		       int			namelen,
		       syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t		err;
	struct sockaddr		*iname;

	SyscallTrace("bind");

	COPYIN(name, iname, namelen, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"bind(%x %x %x)",
							s,iname,namelen));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"bind: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_bind(iname,namelen);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = 0;

finish:
	COPYIN_DONE(name, iname, namelen, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"bind() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_listen(int			s,
			 int			backlog,
			 syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;

	SyscallTrace("listen");
	DEBUG0(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"listen(%x %x)",
								s,backlog));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"listen: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_listen(backlog);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = 0;

finish:
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"listen() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_connect(int			s,
			  struct sockaddr	*name,
			  int			namelen,
			  syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	struct sockaddr		*iname;

	SyscallTrace("connect");

	COPYIN(name, iname, namelen, char, rv, EFAULT);

	DEBUG0(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"connect(%x %x %x)",
							s,iname,namelen));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"connect: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_connect(iname,namelen);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
finish:
	COPYIN_DONE(name, iname, namelen, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"connect() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_shutdown(int			s,
			   int			how,
			   syscall_val_t	*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;

	SyscallTrace("shutdown");
	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"shutdown(%x %x)",
								s,how));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"shutdown: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_shutdown(how);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = 0;

finish:
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"shutdown() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_setsockopt(int			s,
			     int			level,
			     int			name,
			     char			*val,
			     int			valsize,
			     syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	char			*ival;

	SyscallTrace("setsockopt");

	COPYIN(val, ival, valsize, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
		(Diag,"setsockopt(%x %x %x %x %x)",s,level,name,ival,valsize));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"setsockopt: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_setsockopt(level,name,ival,valsize);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = 0;

finish:
	COPYIN_DONE(val, ival, valsize, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
				(Diag,"setsockopt() returns %s, rval=%d",
				mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_getsockopt(int			s,
			     int			level,
			     int			name,
			     char			*val,
			     int			*valsize,
			     syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	char			*ival;
	int			*ivalsize;

	SyscallTrace("getsockopt");

	COPYIN(valsize, ivalsize, 1, long, rv, EFAULT);
	COPYOUT_INIT(ival, val, *ivalsize, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
					(Diag,"getsockopt(%x %x %x %x %x)",
					s,level,name,ival,*ivalsize));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"getsockopt: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_getsockopt(level,name,ival,ivalsize);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = 0;

finish:
	COPYOUT(ivalsize, valsize, 1, long, rv, EFAULT);
	COPYOUT(ival, val, *ivalsize, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
				(Diag,"getsockopt() returns %s, rval=%d",
				mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_getsockname(int			s,
			      struct sockaddr		*name,
			      int			*namelen,
			      syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	struct sockaddr		*iname;
	int			*inamelen;

	SyscallTrace("getsockname");

	COPYIN(namelen, inamelen, 1, int, rv, EFAULT);
	COPYOUT_INIT(iname, name, *inamelen, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
			(Diag,"getsockname(%x %x %x)",s,iname,*inamelen));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"getsockname: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_getsockname(iname,inamelen);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = 0;

finish:
	COPYOUT(inamelen, namelen, 1, int, rv, EFAULT);
	COPYOUT(iname, name, *inamelen, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
				(Diag,"getsockname() returns %s, rval=%d",
				mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_getpeername(int			s,
			      struct sockaddr		*name,
			      int			*namelen,
			      syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	struct sockaddr		*iname;
	int			*inamelen;

	SyscallTrace("getpeername");

	COPYIN(namelen, inamelen, 1, int, rv, EFAULT);
	COPYOUT_INIT(iname, name, *inamelen, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
			(Diag,"getpeername(%x %x %x)",s,iname,*inamelen));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"getpeername: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_getpeername(iname,inamelen);
	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = 0;

finish:
	COPYOUT(inamelen, namelen, 1, int, rv, EFAULT);
	COPYOUT(iname, name, *inamelen, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
				(Diag,"getpeername() returns %s, rval=%d",
				mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_socketpair(int			domain,
			     int			type,
			     int			protocol,
			     int			*rsv,
			     syscall_val_t		*rv)
{
	usTop  	*obj1 = NULL;
	usTop  	*obj2 = NULL;
	mach_error_t 		err;
	int			*irsv;

	SyscallTrace("socketpair");

	COPYOUT_INIT(irsv, rsv, 2, int, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
			(Diag,"socketpair(%x %x %x)",domain,type,protocol));

	us_internal_error("emul_socketpair",US_NOT_IMPLEMENTED);
	err = US_NOT_IMPLEMENTED;

	if (err) {
		rv->rv_val1 = emul_error_to_unix(err);
		irsv[0] = -1;
		irsv[1] = -1;
		goto finish;
	}

finish:
	COPYOUT(irsv, rsv, 2, int, rv, EFAULT);
	mach_object_dereference(obj1);
	mach_object_dereference(obj2);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
				(Diag,"socketpair() returns %s, rval=%d",
				mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_sendto(int			s,
			 char			*buf,
			 int			len,
			 int			flags,
			 struct sockaddr	*to,
			 int			tolen,
			 syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	char			*ibuf;
	struct sockaddr		*ito;

	SyscallTrace ("sendto");

	COPYIN(buf, ibuf, len, char, rv, EFAULT);
	COPYIN(to, ito, tolen, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
		(Diag,"sendto(%x %x %x %x %x %x)",s,ibuf,len,flags,ito,tolen));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"sendto: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_sendto(ibuf,&len,flags,ito,tolen);

 	if ((err == IO_INVALID_RECNUM) || (err == IO_REJECTED)) {
		/* XXX generate a sigpipe */
		len = 0;
		err = unix_err(EPIPE);
	}

	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = len;

finish:
	COPYIN_DONE(buf, ibuf, len, char, rv, EFAULT);
	COPYIN_DONE(to, ito, tolen, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"sendto() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_send(int			s,
		       char			*buf,
		       int			len,
		       int			flags,
		       syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	char			*ibuf;

	SyscallTrace ("send");

	COPYIN(buf, ibuf, len, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
				(Diag,"send(%x %x %x %x)",s,ibuf,len,flags));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"send: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_send(ibuf,&len,flags);

 	if ((err == IO_INVALID_RECNUM) || (err == IO_REJECTED)) {
		/* XXX generate a sigpipe */
		len = 0;
		err = unix_err(EPIPE);
	}

	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = len;

finish:
	COPYIN_DONE(buf, ibuf, len, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"send() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_sendmsg(int			s,
			  struct msghdr		*msg,
			  int			flags,
			  syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	int			count;
	struct msghdr		*imsg;

	SyscallTrace ("sendmsg");

	COPYIN(msg, imsg, 1, struct msghdr, rv, EFAULT);
	/* XXX check msg data */

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
				(Diag,"sendmsg(%x %x %x)",s,imsg,flags));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"sendmsg: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

//	err = tobj->ux_sendmsg(imsg,flags,&count); XXX C++
	err = tobj->ux_sendmsg(imsg,flags);

 	if ((err == IO_INVALID_RECNUM) || (err == IO_REJECTED)) {
		/* XXX generate a sigpipe */
		count = 0;
		err = unix_err(EPIPE);
	}

	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = count;

finish:
	COPYIN_DONE(msg, imsg, 1, struct msghdr, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"sendmsg() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_recvfrom(int			s,
			   char			*buf,
			   int			len,
			   int			flags,
			   struct sockaddr	*from,
			   int			*fromlen,
			   syscall_val_t	*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	char			*ibuf;
	struct sockaddr		*ifrom;
	int			*ifromlen;

	SyscallTrace ("recvfrom");

	COPYOUT_INIT(ibuf, buf, len, char, rv, EFAULT);
	COPYIN(fromlen, ifromlen, 1, long, rv, EFAULT);
	COPYOUT_INIT(ifrom, from, *ifromlen, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,
					(Diag,"recvfrom(%x %x %x %x %x %x)",
					s,ibuf,len,flags,ifrom,ifromlen));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"recvfrom: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_recvfrom(ibuf,&len,flags,ifrom,ifromlen);

 	if ((err == IO_INVALID_RECNUM) || (err == IO_REJECTED)) {
		len = 0;
		err = ERR_SUCCESS;
	}

	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = len;

finish:
	COPYOUT(ibuf, buf, len, char, rv, EFAULT);
	COPYOUT(ifromlen, fromlen, 1, long, rv, EFAULT);
	COPYOUT(ifrom, from, *ifromlen, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"recvfrom() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_recv(int			s,
		       char			*buf,
		       int			len,
		       int			flags,
		       syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	char			*ibuf;

	SyscallTrace ("recv");

	COPYOUT_INIT(ibuf, buf, len, char, rv, EFAULT);

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"recv(%x %x %x %x)",
							s,ibuf,len,flags));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"recv: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

	err = tobj->ux_recv(ibuf,&len,flags);

 	if ((err == IO_INVALID_RECNUM) || (err == IO_REJECTED)) {
		len = 0;
		err = ERR_SUCCESS;
	}

	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = len;

finish:
	COPYOUT(ibuf, buf, len, char, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"recv() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


mach_error_t emul_recvmsg(int			s,
			  struct msghdr		*msg,
			  int			flags,
			  syscall_val_t		*rv)
{
	uxio_socket   	*tobj = NULL;
	mach_error_t 		err;
	int			count;
	struct msghdr		*imsg;

	SyscallTrace ("recvmsg");

	COPYOUT_INIT(imsg, msg, 1, struct msghdr, rv, EFAULT);
	/* XXX check msg data */

	DEBUG1(emul_debug & EMUL_DEBUG_SYSENTER,(Diag,"recvmsg(%x %x %x)",
								s,imsg,flags));

	uxio *aobj;
	err = ftab_obj->ftab_get_obj(s,&aobj);
	if (err != ERR_SUCCESS) {
		rv->rv_val1 = EBADF;
		goto finish;
	}
	if ((tobj = uxio_socket::castdown(aobj)) == 0) {
		ERROR((Diag,"recvmsg: castdown error: %s->%s\n", aobj->is_a()->class_name(), "uxio_socket"));
		rv->rv_val1 =emul_error_to_unix(MACH_OBJECT_NO_SUCH_OPERATION);
		goto finish;
	}

//	err = tobj->ux_recvmsg(imsg,flags,&count); XXX C++
	err = tobj->ux_recvmsg(imsg,flags);

 	if ((err == IO_INVALID_RECNUM) || (err == IO_REJECTED)) {
		count = 0;
		err = ERR_SUCCESS;
	}

	if (err) {
		rv->rv_val1 = emulsock_error_to_unix(err);
		goto finish;
	}
	rv->rv_val1 = count;

finish:
	COPYOUT(imsg, msg, 1, struct msghdr, rv, EFAULT);
	mach_object_dereference(tobj);
	DEBUG1(emul_debug & EMUL_DEBUG_SYSEXIT,
					(Diag,"recvmsg() returns %s, rval=%d",
					mach_error_string(err),rv->rv_val1));
	return(err);
}


