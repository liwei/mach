/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988 Carnegie Mellon University
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
 * File:        syscalls_prototypes.h
 *
 * Purpose:
 *	
 *	Prototypes of BSD system calls.
 *
 * HISTORY: 
 * $Log:	syscalls_prototypes.h,v $
 * Revision 2.4  94/07/08  16:58:21  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  91/11/13  17:17:07  dpj
 * 	Cleaned-up compiler warnings.
 * 	[91/11/12  17:51:17  dpj]
 * 
 * Revision 2.2  91/11/06  11:33:47  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:46:22  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:35:58  pjg]
 * 
 */


extern emul_getpagesize(syscall_val_t*);
extern emul_kill(int, int, syscall_val_t *);
extern emul_killpg(int, int, syscall_val_t *);
extern emul_sigblock(int, syscall_val_t *);
extern emul_sigpause(int, syscall_val_t *);
extern emul_sigsetmask(int, syscall_val_t *);
extern emul_sigstack(struct sigstack *, struct sigstack *, syscall_val_t *);
extern emul_sigvec(int, struct sigvec *, struct sigvec *, syscall_val_t *);
extern int emul_generic(int, int*, syscall_val_t*);
extern mach_error_t emul__exit(int, int);
extern mach_error_t emul_accept(int, struct sockaddr*, int*, syscall_val_t*);
extern mach_error_t emul_access(char *, int, syscall_val_t *);
extern mach_error_t emul_bind(int, struct sockaddr*, int, syscall_val_t*);
extern mach_error_t emul_chdir(char *, syscall_val_t *);
extern mach_error_t emul_chgrp(char *, int, syscall_val_t *);
extern mach_error_t emul_chmod(char *, int, syscall_val_t *);
extern mach_error_t emul_chown(char *, int, int, syscall_val_t *);
extern mach_error_t emul_close( int, syscall_val_t* );
extern mach_error_t emul_connect(int, struct sockaddr*, int, syscall_val_t *);
extern mach_error_t emul_creat( char*, unsigned int, syscall_val_t* );
extern mach_error_t emul_dup( int, int, syscall_val_t* );
extern mach_error_t emul_dup2( int, int, syscall_val_t* );
extern mach_error_t emul_fchmod(int, int, syscall_val_t *);
extern mach_error_t emul_fchown(int, int, int, syscall_val_t *);
extern mach_error_t emul_fcntl(int, int, int, syscall_val_t*);
extern mach_error_t emul_fork(syscall_val_t *);
extern mach_error_t emul_fstat(int, struct stat*, syscall_val_t*);
extern mach_error_t emul_ftruncate( int, unsigned int, syscall_val_t* );
extern mach_error_t emul_getdirentries(int, char*, unsigned int, long*, syscall_val_t*);
extern mach_error_t emul_getdtablesize( syscall_val_t* );
extern mach_error_t emul_getgid(syscall_val_t *);
extern mach_error_t emul_getgroups(int, int *, syscall_val_t *);
extern mach_error_t emul_getitimer(int, struct itimerval *, syscall_val_t *);
extern mach_error_t emul_getpeername(int, struct sockaddr*, int*, syscall_val_t*);
extern mach_error_t emul_getpgrp(int, syscall_val_t *);
extern mach_error_t emul_getpid(syscall_val_t *);
extern mach_error_t emul_getpid(syscall_val_t *);
extern mach_error_t emul_getrusage(int, struct rusage *, syscall_val_t *);
extern mach_error_t emul_getsockname(int, struct sockaddr*, int*, syscall_val_t*);
extern mach_error_t emul_getsockopt(int, int, int, char*, int*, syscall_val_t*);
extern mach_error_t emul_gettimeofday(struct timeval *, struct timezone *, syscall_val_t *);
extern mach_error_t emul_getuid(syscall_val_t *);
extern mach_error_t emul_ioctl( int, int, vm_address_t, syscall_val_t* );
extern mach_error_t emul_link(char *, char *, syscall_val_t *);
extern mach_error_t emul_listen(int, int, syscall_val_t*);
extern mach_error_t emul_lseek(int, long, int, syscall_val_t*);
extern mach_error_t emul_lstat(char *, struct stat *, syscall_val_t *);
extern mach_error_t emul_mkdir(char *, int, syscall_val_t *);
extern mach_error_t emul_munmap(caddr_t, int, syscall_val_t*);
extern mach_error_t emul_obreak(vm_address_t, syscall_val_t *);
extern mach_error_t emul_open(char*, unsigned int,unsigned int,syscall_val_t*);
extern mach_error_t emul_ostat(char *, struct stat *, syscall_val_t *);
extern mach_error_t emul_pipe(syscall_val_t*);
extern mach_error_t emul_read(int, char*, unsigned int, syscall_val_t*);
extern mach_error_t emul_readlink(char*, char*, unsigned int, syscall_val_t *);
extern mach_error_t emul_readv(int, struct iovec*, int, syscall_val_t*);
extern mach_error_t emul_recv(int, char*, int, int, syscall_val_t*);
extern mach_error_t emul_recvfrom(int, char*, int, int, struct sockaddr*, int*, syscall_val_t*);
extern mach_error_t emul_recvmsg(int, struct msghdr*, int, syscall_val_t*);
extern mach_error_t emul_rename(char *, char *, syscall_val_t *);
extern mach_error_t emul_rexit(int, syscall_val_t *);
extern mach_error_t emul_rmdir(char *, syscall_val_t *);
extern mach_error_t emul_sbrk(unsigned int, syscall_val_t *);
extern mach_error_t emul_select( int, fd_set*, fd_set*, fd_set*, struct timeval*, syscall_val_t* );
extern mach_error_t emul_send(int, char*, int, int, syscall_val_t*);
extern mach_error_t emul_sendmsg(int, struct msghdr*, int, syscall_val_t*);
extern mach_error_t emul_sendto(int, char*, int, int, struct sockaddr*, int, syscall_val_t*);
extern mach_error_t emul_setgroups(int, int *, syscall_val_t *);
extern mach_error_t emul_setitimer(int, struct itimerval *, struct itimerval *, syscall_val_t *);
extern mach_error_t emul_setpgrp(int, int, syscall_val_t *);
extern mach_error_t emul_setregid(int, int, syscall_val_t *);
extern mach_error_t emul_setreuid(int, int, syscall_val_t *);
extern mach_error_t emul_setsockopt(int, int, int, char*, int, syscall_val_t*);
extern mach_error_t emul_shutdown(int, int, syscall_val_t*);
extern mach_error_t emul_smmap(caddr_t, int, int, int, int, off_t, syscall_val_t*);
extern mach_error_t emul_socket(int, int, int, syscall_val_t *);
extern mach_error_t emul_socketpair(int, int, int, int*, syscall_val_t*);
extern mach_error_t emul_stat(char *, struct stat *, syscall_val_t *);
extern mach_error_t emul_symlink(char *, char *, syscall_val_t *);
extern mach_error_t emul_truncate(char *, unsigned int, syscall_val_t *);
extern mach_error_t emul_umask(int, syscall_val_t*);
extern mach_error_t emul_unlink(char *, syscall_val_t *);
extern mach_error_t emul_utimes(char *, struct timeval *, syscall_val_t *);
extern mach_error_t emul_vfork(syscall_val_t *);
extern mach_error_t emul_write( int, char*, int, syscall_val_t* );
extern mach_error_t emul_writev(int, struct iovec*, int, syscall_val_t*);
