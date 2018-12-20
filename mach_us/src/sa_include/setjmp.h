/*	setjmp.h	4.1	83/05/03	*/
/*
 **********************************************************************
 * HISTORY
 * $Log:	setjmp.h,v $
 * Revision 2.2  90/07/26  12:44:46  dpj
 * 	First version.
 * 	[90/07/24  16:54:01  dpj]
 * 
 * Revision 2.6  90/05/03  19:59:41  bww
 * 	Added i386 case.
 * 	[90/05/03  19:58:16  bww]
 * 
 * Revision 2.5  90/04/26  18:10:31  bww
 * 	Added definitions for the Sun4, and
 * 	for sigsetjmp() and siglongjmp().
 * 	[90/04/26  18:09:19  bww]
 * 
 * Revision 2.4  89/08/14  13:44:00  bww
 * 	Add jmp_buf definition for PMAX (mips).
 * 	From "[89/08/03  13:41:22  mja]" at CMU.
 * 	[89/08/14            bww]
 * 
 * Revision 2.3  89/06/30  12:11:58  bww
 * 	Added function prototype declarations and protection for
 * 	recursive includes.
 * 	[89/06/30  12:01:04  bww]
 * 
 * Revision 2.2  89/06/26  20:20:31  bww
 * 	Merged changes for SUN3, IBMRT, Multimax, and Sequent into
 * 	4.3-tahoe version.
 * 	[89/06/26  20:17:09  bww]
 * 
 * Revision 1.2  89/05/26  12:28:43  bww
 * 	CMU CS as of 89/05/15
 * 	[89/06/04  19:14:49  bww]
 * 
 **********************************************************************
 */

#ifndef _SETJMP_H_
#define _SETJMP_H_ 1

#ifdef multimax
typedef int jmp_buf[10];
typedef	int sigjmp_buf[10+1];
#endif /* multimax */
#ifdef	balance
typedef int jmp_buf[11];	/* 4 regs, ... */
typedef	int sigjmp_buf[11+1];
#endif	/* balance */
#ifdef sun3
typedef int jmp_buf[15];	/* pc, sigmask, onsstack, d2-7, a2-7 */
typedef	int sigjmp_buf[15+1];
#endif	/* sun3 */
#ifdef ibmrt
typedef int jmp_buf[16];
typedef	int sigjmp_buf[16+1];
#endif	/* ibmrt */
#ifdef vax
typedef int jmp_buf[10];
typedef	int sigjmp_buf[10+1];
#endif	/* vax */
#ifdef i386
typedef int jmp_buf[21];
typedef	int sigjmp_buf[21+1];
#endif	/* i386 */
#ifdef mips
/*
 * Room for: onstack, sigmask, pc
 *           r0-31,
 *           40 padding words to match length of sigcontext structure
 */
typedef int jmp_buf[3+32+40];
typedef	int sigjmp_buf[3+32+40+1];
#endif	/* mips */
#ifdef	sun4
/*
 * onsstack,sigmask,sp,pc,npc,psr,g1,o0,wbcnt (sigcontext).
 * All else recovered by under/over(flow) handling.
 */
#define	_JBLEN	9

typedef int jmp_buf[_JBLEN];

/*
 * One extra word for the "signal mask saved here" flag.
 */
typedef	int sigjmp_buf[_JBLEN+1];
#endif	/* sun4 */

#if __STDC__
extern int setjmp(jmp_buf);
extern void longjmp(jmp_buf, int);
extern int _setjmp(jmp_buf);
extern void _longjmp(jmp_buf, int);
extern int sigsetjmp(sigjmp_buf, int);
extern void siglongjmp(sigjmp_buf, int);
#else
extern int setjmp();
extern void longjmp();
extern int _setjmp();
extern void _longjmp();
extern int sigsetjmp();
extern void siglongjmp();
#endif

#ifdef	sun4
/*
 * Routines that call setjmp have strange control flow graphs,
 * since a call to a routine that calls resume/longjmp will eventually
 * return at the setjmp site, not the original call site.  This
 * utterly wrecks control flow analysis.
 */
#pragma unknown_control_flow(sigsetjmp, setjmp, _setjmp)
#endif	/* sun4 */

#endif /* _SETJMP_H_ */
