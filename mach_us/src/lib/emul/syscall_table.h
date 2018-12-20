/*
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 * HISTORY:
 * $Log:	syscall_table.h,v $
 * Revision 2.6  94/07/08  17:01:40  mrt
 * 	Updtated copyright
 * 
 * Revision 2.5  90/11/27  18:19:36  jms
 * 	New "fork" call mechinism
 * 	[90/08/20  17:29:32  jms]
 * 
 * Revision 2.4  90/07/09  17:03:40  dorr
 * 	Fix sysW to be same format as other sysX macros
 * 	[90/07/06  15:17:08  jms]
 * 
 * Revision 2.3  90/01/02  22:00:54  dorr
 * 	add definitions of emul_entry and sys* macros.
 * 
 * Revision 2.2.1.1  89/12/18  15:58:05  dorr
 * 	add definitions of emul_entry_t and sys* macros.
 * 
 * Revision 2.2  89/10/06  13:46:10  mbj
 * 	Hacked BSD-server version to make multi-server version.
 * 	[89/10/05  16:36:22  mbj]
 * 
 * 	Version for the multi-server emulator.
 * 	[89/09/14  10:04:39  mbj]
 * 
 */


/*
 * Definition of system call table.
 */
struct sysent {
	int	nargs;		/* number of arguments, or special code */
	int	(*routine)();
};

/*
 * emul entry (8 bytes)  (temp)
 */
typedef struct	emul_entry {
	char	code[8];
} emul_entry_t;

extern emul_entry_t	emul_vector_base[];
extern int		emul_vec[];
extern char		* emul_names[];

/*
 * Special arguments:
 */
#define	E_GENERIC	(-1)
				/* no specialized routine */
#define	E_CHANGE_REGS	(-2)
				/* may change registers */
#define	syss(routine, nargs)	{ nargs, routine }
#define	sysB(routine, nargs)	{ nargs, routine }
#define	sysg			{ E_GENERIC, emul_generic }
#define	sysW(routine, dummy)	{ E_CHANGE_REGS, routine }
#define	sysF(routine, nargs)	{ nargs, routine }
#ifdef ECACHE
#define syse(routine)		{ E_GENERIC, routine }
#endif ECACHE

/*
 * Exported system call table
 */
extern struct sysent	sysent[];	/* normal system calls */
extern int		nsysent;

#if	XXX
extern struct sysent	cmusysent[];	/* CMU extensions */
extern int		ncmusysent;

extern struct sysent	sysent_task_by_pid;
extern struct sysent	sysent_htg_ux_syscall;
#endif	XXX
