/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)stdio.h	5.4 (Berkeley) 10/22/87
 *
 **********************************************************************
 * HISTORY
 * $Log:	stdio.h,v $
 * Revision 2.2  90/07/26  12:44:49  dpj
 * 	First version.
 * 	[90/07/24  16:54:37  dpj]
 * 
 * Revision 2.3  90/04/14  15:03:19  bww
 * 	Added #if PARALLEL and #if _FMAP conditionals
 * 	for parallel libc and file mapping support.
 * 	From "[90/03/13  13:36:14  mbj]" at CMU.
 * 	[90/04/14  15:02:56  bww]
 * 
 * Revision 2.2  89/06/30  12:12:04  bww
 * 	Added function prototype declarations.
 * 	[89/06/30  12:01:22  bww]
 * 
 * Revision 1.2  89/05/26  12:29:09  bww
 * 	CMU CS as of 89/05/15
 * 	[89/05/26  09:46:50  bww]
 * 
 **********************************************************************
 */

#ifndef FILE
#if	PARALLEL && !defined(_FMAP)
#define	_FMAP	1		/* On by default when parallel libc */
#endif
#if	_FMAP
#define	MAPBUFSIZE	(16*1024)
#endif
#define	BUFSIZ	1024
extern	struct	_iobuf {
	int	_cnt;
	char	*_ptr;		/* should be unsigned char */
	char	*_base;		/* ditto */
	int	_bufsiz;
	short	_flag;
	char	_file;		/* should be short */
} _iob[];

#define	_IOREAD	01
#define	_IOWRT	02
#define	_IONBF	04
#define	_IOMYBUF	010
#define	_IOEOF	020
#define	_IOERR	040
#define	_IOSTRG	0100
#define	_IOLBF	0200
#define	_IORW	0400
#if	_FMAP
#define	_IOMAP	040000		/* for mapped files */
#endif
#define	NULL	0
#define	FILE	struct _iobuf
#define	EOF	(-1)

#define	stdin	(&_iob[0])
#define	stdout	(&_iob[1])
#define	stderr	(&_iob[2])
#ifndef lint
#if	PARALLEL
#define	unlocked_getc(p)	(--(p)->_cnt>=0?\
			(int)(*(unsigned char *)(p)->_ptr++):\
			_filbuf(p))
#define	unlocked_getchar()	unlocked_getc(stdin)
#define	getc(p)			fgetc(p)
#else	/* PARALLEL */
#define	getc(p)			(--(p)->_cnt>=0?\
			(int)(*(unsigned char *)(p)->_ptr++):\
			_filbuf(p))
#endif	/* PARALLEL */
#endif /* not lint */
#define	getchar()	getc(stdin)
#ifndef lint
#if	PARALLEL
#define unlocked_putc(x, p)	(--(p)->_cnt >= 0 ?\
	(int)(*(unsigned char *)(p)->_ptr++ = (x)) :\
	(((p)->_flag & _IOLBF) && -(p)->_cnt < (p)->_bufsiz ?\
		((*(p)->_ptr = (x)) != '\n' ?\
			(int)(*(unsigned char *)(p)->_ptr++) :\
			_flsbuf(*(unsigned char *)(p)->_ptr, p)) :\
		_flsbuf((unsigned char)(x), p)))
#define	unlocked_putchar(x)	unlocked_putc(x,stdout)
#define putc(x, p)		fputc(x, p)
#else	/* PARALLEL */
#define putc(x, p)		(--(p)->_cnt >= 0 ?\
	(int)(*(unsigned char *)(p)->_ptr++ = (x)) :\
	(((p)->_flag & _IOLBF) && -(p)->_cnt < (p)->_bufsiz ?\
		((*(p)->_ptr = (x)) != '\n' ?\
			(int)(*(unsigned char *)(p)->_ptr++) :\
			_flsbuf(*(unsigned char *)(p)->_ptr, p)) :\
		_flsbuf((unsigned char)(x), p)))
#endif	/* PARALLEL */
#endif /* not lint */
#define	putchar(x)	putc(x,stdout)
#define	feof(p)		(((p)->_flag&_IOEOF)!=0)
#define	ferror(p)	(((p)->_flag&_IOERR)!=0)
#define	fileno(p)	((p)->_file)
#if	PARALLEL
#define	unlocked_clearerr(p)	((p)->_flag &= ~(_IOERR|_IOEOF))
#else	/* PARALLEL */
#define	clearerr(p)		((p)->_flag &= ~(_IOERR|_IOEOF))
#endif	/* PARALLEL */

#if __STDC__
extern int _flsbuf(unsigned int, FILE*);
extern int _filbuf(FILE*);
extern int fclose(FILE*);
extern FILE* fdopen(int, const char*);
extern int fflush(FILE*);
extern int fgetc(FILE*);
extern char* fgets(char*, int, FILE*);
extern FILE* fopen(const char*, const char*);
extern int fprintf(FILE*, const char*,...);
extern int fputc(int, FILE*);
extern int fputs(const char*, FILE*);
extern int fread(char*, int, int, FILE*);
extern FILE* freopen(const char*, const char*, FILE*);
extern int fscanf(FILE*, const char*,...);
extern int fseek(FILE*, long, int);
extern long ftell(const FILE*);
extern int fwrite(const char*, int, int, FILE*);
extern char* gets(char*);
extern int getw(FILE*);
extern FILE* popen(const char*, const char*);
extern int pclose(FILE *stream);
extern int printf(const char*,...);
extern int puts(const char*);
extern int putw(int, FILE*);
extern int rewind(FILE*);
extern int scanf(const char*,...);
extern void setbuf(FILE*, char*);
extern void setbuffer(FILE*, char*, int);
extern void setlinebuf(FILE*);
extern int sprintf(char*, const char*,...);
extern int sscanf(char*, const char*,...);
extern int ungetc(int, FILE*);
#if	_FMAP
extern char *fmap(FILE*, int);
extern char *fremap(FILE*, int);
#endif
#else
FILE	*fopen();
FILE	*fdopen();
FILE	*freopen();
FILE	*popen();
long	ftell();
char	*fgets();
char	*gets();
int	sprintf();	/* here until everyone does it right */
#if	_FMAP
char	*fmap();
char    *fremap();
#endif
#endif

#if	PARALLEL
typedef struct rec_mutex *f_buflock;
extern f_buflock f_lockbuf();
#ifndef	lint
#define f_unlockbuf(buflock) do { \
	if ((buflock) != (struct rec_mutex *) NULL) \
		rec_mutex_unlock(buflock); \
	} while (0)
#else
#define f_unlockbuf(buflock) \
		rec_mutex_unlock(buflock)
#endif
#endif	/* PARALLEL */
#endif
