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
 * File: bsd_syscalls.c
 *
 * Purpose: define all possible bsd syscall entries
 *
 * HISTORY: 
 * $Log:	bsd_syscalls.cpp,v $
 * Revision 2.4  94/07/08  16:56:08  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  94/06/01  18:18:01  mrt
 * 	dumped the call/CAT macro stuff.
 * 	[94/05/27            mrt]
 * 
 * Revision 2.2  90/11/27  18:17:28  jms
 * 	Fix args to getpagesize
 * 	[90/11/20  17:02:56  jms]
 * 
 * Revision 2.1  90/08/20  17:11:54  jms
 * Created.
 * 
 * Revision 2.3  90/07/09  17:01:37  dorr
 * 	[90/02/23  14:38:38  dorr]
 * 
 *	add special invocations for execv, sigreturn, osigcleanup
 *
 * Revision 2.2  90/01/02  21:31:40  dorr
 *	initial checkin.
 *
 * Revision 2.1.1.1  89/12/18  15:38:18  dorr
 *	Initial checkin.
 *
 */

#include <sys/syscall.h>

/*
 * Define a syscall whose syscall number is given
 */
#define ncall(num, name, type, argcount) \
num name type argcount

ncall(0, indirect, s, 7)	/*   0 = indir */
ncall(1, rexit, s, 1)		/*   1 = exit */
ncall(2, fork, F, 1000)		/*   2 = fork */
ncall(3, read, s, 3)		/*   3 = read */
ncall(4, write, s, 3)		/*   4 = write */
ncall(5, open, s, 3)		/*   5 = open */
ncall(6, close, s, 1)		/*   6 = close */
ncall(8, creat, s, 2)		/*   8 = creat */
ncall(9, link, s, 2)		/*   9 = link */
ncall(10, unlink, s, 1)		/*  10 = unlink */
ncall(11, execv, s, 1000)	/*  11 = execv */
ncall(12, chdir, s, 1)		/*  12 = chdir */
ncall(13, otime, s, 0)		/*  13 = old time */
ncall(14, mknod, s, 3)		/*  14 = mknod */
ncall(15, chmod, s, 2)		/*  15 = chmod */
ncall(16, chown, s, 3)		/*  16 = chown; now 3 args */
ncall(17, obreak, B, 1)		/*  17 = old brk */
ncall(18, ostat, s, 2)		/*  18 = old stat */
ncall(19, lseek, s, 3)		/*  19 = lseek */
ncall(20, getpid, s, 0)		/*  20 = getpid */
ncall(21, smount, s, 3)		/*  21 = mount */
ncall(22, umount, s, 1)		/*  22 = umount */
ncall(23, osetuid, s, 1)	/*  23 = old setuid */
ncall(24, getuid, s, 0)		/*  24 = getuid */
ncall(25, ostime, s, 1)		/*  25 = old stime */
ncall(26, ptrace, s, 4)		/*  26 = ptrace */
ncall(27, oalarm, s, 1)		/*  27 = old alarm */
ncall(28, ofstat, s, 2)		/*  28 = old fstat */
ncall(29, opause, s, 0)		/*  29 = opause */
ncall(30, outime, s, 2)		/*  30 = old utime */
ncall(33, access, s, 2)		/*  33 = access */
ncall(34, onice, s, 1)		/*  34 = old nice */
ncall(35, oftime, s, 1)		/*  35 = old ftime */
ncall(36, sync, s, 0)		/*  36 = sync */
ncall(37, kill, s, 2)		/*  37 = kill */
ncall(38, stat, s, 2)		/*  38 = stat */
ncall(39, osetpgrp, s, 2)	/*  39 = old setpgrp */
ncall(40, lstat, s, 2)		/*  40 = lstat */
ncall(41, dup, s, 2)			/*  41 = dup */
ncall(42, pipe, s, 0)		/*  42 = pipe */
ncall(43, otimes, s, 1)		/*  43 = old times */
ncall(44, profil, s, 4)		/*  44 = profil */
ncall(47, getgid, s, 0)		/*  47 = getgid */
ncall(48, ossig, s, 2)		/*  48 = old sigsys */
ncall(51, sysacct, s, 1)	/*  51 = turn acct off/on */
ncall(54, ioctl, s, 3)		/*  54 = ioctl */
ncall(55, reboot, s, 1)		/*  55 = reboot */
ncall(57, symlink, s, 2)	/*  57 = symlink */
ncall(58, readlink, s, 3)	/*  58 = readlink */
ncall(59, execve, s, 1000)	/*  59 = execve */
ncall(60, umask, s, 1)		/*  60 = umask */
ncall(61, chroot, s, 1)		/*  61 = chroot */
ncall(62, fstat, s, 2)		/*  62 = fstat */
ncall(64, getpagesize, s, 0)	/*  64 = getpagesize */
ncall(65, mremap, s, 5)		/*  65 = mremap */
ncall(66, vfork, F, 1000)	/*  66 = vfork */
ncall(69, sbrk, s, 1)		/*  69 = sbrk */
ncall(70, sstk, s, 1)		/*  70 = sstk */
ncall(71, smmap, s, 6)		/*  71 = mmap */
ncall(72, ovadvise, s, 1)	/*  72 = old vadvise */
ncall(73, munmap, s, 2)		/*  73 = munmap */
ncall(74, mprotect, s, 3)	/*  74 = mprotect */
ncall(75, madvise, s, 3)	/*  75 = madvise */
ncall(76, vhangup, s, 1)	/*  76 = vhangup */
ncall(77, ovlimit, s, 2)	/*  77 = old vlimit */
ncall(78, mincore, s, 3)	/*  78 = mincore */
ncall(79, getgroups, s, 2)	/*  79 = getgroups */
ncall(80, setgroups, s, 2)	/*  80 = setgroups */
ncall(81, getpgrp, s, 1)	/*  81 = getpgrp */
ncall(82, setpgrp, s, 2)	/*  82 = setpgrp */
ncall(83, setitimer, s, 3)	/*  83 = setitimer */
ncall(84, wait, W, 1000)	/*  84 = wait */
ncall(85, swapon, s, 1)		/*  85 = swapon */
ncall(86, getitimer, s, 2)	/*  86 = getitimer */
ncall(87, gethostname, s, 2)	/*  87 = gethostname */
ncall(88, sethostname, s, 2)	/*  88 = sethostname */
ncall(89, getdtablesize, s, 0)	/*  89 = getdtablesize */
ncall(90, dup2, s, 2)		/*  90 = dup2 */
ncall(91, getdopt, s, 2)	/*  91 = getdopt */
ncall(92, fcntl, s, 3)		/*  92 = fcntl */
ncall(93, select, s, 5)		/*  93 = select */
ncall(94, setdopt, s, 2)	/*  94 = setdopt */
ncall(95, fsync, s, 1)		/*  95 = fsync */
ncall(96, setpriority, s, 3)	/*  96 = setpriority */
ncall(97, socket, s, 3)		/*  97 = socket */
ncall(98, connect, s, 3)	/*  98 = connect */
ncall(99, accept, s, 3)		/*  99 = accept */
ncall(100, getpriority, s, 2)	/* 100 = getpriority */
ncall(101, send, s, 4)		/* 101 = send */
ncall(102, recv, s, 4)		/* 102 = recv */
ncall(103, sigreturn, s, 1000)	/* 103 = sigreturn */
ncall(104, bind, s, 3)		/* 104 = bind */
ncall(105, setsockopt, s, 5)	/* 105 = setsockopt */
ncall(106, listen, s, 2)	/* 106 = listen */
ncall(107, ovtimes, s, 2)	/* 107 = old vtimes */
ncall(108, sigvec, s, 3)	/* 108 = sigvec */
ncall(109, sigblock, s, 1)	/* 109 = sigblock */
ncall(110, sigsetmask, s, 1)	/* 110 = sigsetmask */
ncall(111, sigpause, s, 1)	/* 111 = sigpause */
ncall(112, sigstack, s, 2)	/* 112 = sigstack */
ncall(113, recvmsg, s, 3)	/* 113 = recvmsg */
ncall(114, sendmsg, s, 3)	/* 114 = sendmsg */
ncall(115, vtrace, s, 2)	/* 115 = vtrace */
ncall(116, gettimeofday, s, 2)	/* 116 = gettimeofday */
ncall(117, getrusage, s, 2)	/* 117 = getrusage */
ncall(118, getsockopt, s, 5)	/* 118 = getsockopt */
				/* 119 = old resuba */
ncall(120, readv, s, 3)		/* 120 = readv */
ncall(121, writev, s, 3)	/* 121 = writev */
ncall(122, settimeofday, s, 2)	/* 122 = settimeofday */
ncall(123, fchown, s, 3)	/* 123 = fchown */
ncall(124, fchmod, s, 2)	/* 124 = fchmod */
ncall(125, recvfrom, s, 6)	/* 125 = recvfrom */
ncall(126, setreuid, s, 2)	/* 126 = setreuid */
ncall(127, setregid, s, 2)	/* 127 = setregid */
ncall(128, rename, s, 2)	/* 128 = rename */
ncall(129, truncate, s, 2)	/* 129 = truncate */
ncall(130, ftruncate, s, 2)	/* 130 = ftruncate */
ncall(131, flock, s, 2)		/* 131 = flock */
				/* 132 = unused */
ncall(133, sendto, s, 6)	/* 133 = sendto */
ncall(134, shutdown, s, 2)	/* 134 = shutdown */
ncall(135, socketpair, s, 4)	/* 135 = socketpair */
ncall(136, mkdir, s, 2)		/* 136 = mkdir */
ncall(137, rmdir, s, 1)		/* 137 = rmdir */
ncall(138, utimes, s, 2)	/* 138 = utimes */
ncall(139, osigcleanup, s, 1000)/* 139 = 4.2 signal cleanup */
ncall(140, adjtime, s, 2)	/* 140 = adjtime */
ncall(141, getpeername, s, 3)	/* 141 = getpeername */
ncall(142, gethostid, s, 0)	/* 142 = gethostid */
ncall(143, sethostid, s, 1)	/* 143 = sethostid */
ncall(144, getrlimit, s, 2)	/* 144 = getrlimit */
ncall(145, setrlimit, s, 2)	/* 145 = setrlimit */
ncall(146, killpg, s, 2)	/* 146 = killpg */
ncall(148, setquota, s, 2)	/* 148 = quota */
ncall(149, qquota, s, 4)	/* 149 = qquota */
ncall(150, getsockname, s, 3)	/* 150 = getsockname */

/*
 * the following values are different on different machines
 */
ncall(SYS_getdirentries, getdirentries, s, 4)

