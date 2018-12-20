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
 *
 * Purpose:
 *	small program to generate syscall tables from list of
 *	syscalls to be emulated and a list of all possible syscalls.
 *
 * HISTORY: 
 * $Log:	gen_syscalls.c,v $
 * Revision 2.5  94/07/08  16:57:34  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.4  90/07/09  17:02:45  dorr
 * 	Add explicit sys call numbers (vs order implicit)
 * 	[90/07/06  15:04:52  jms]
 * 
 * Revision 2.3  90/03/14  17:28:43  orr
 * 	explicitly exit(0).  gcc/libc for the 386 doesn't
 * 	exit with 0 if you fall out of a routine implicitly.
 * 	[90/03/14  16:54:06  orr]
 * 
 * Revision 2.2  90/01/02  21:56:01  dorr
 * 	initial checkin.
 * 
 * Revision 2.1.1.1  89/12/18  15:52:57  dorr
 * 	initial checkin.
 * 
 */

#include <stdio.h>
#include <ctype.h>
#include <base.h>

typedef struct syscall_entry {
	int	call_num;
	char	* name;
	char	protocol;
	int	num_args;
	int	mask;
} * syscall_tab_t;

main(argc, argv)
int argc;
char * * argv;
{
	int		i;
	FILE		* fd;
	char 		buf[256];
	char 		b1[256], b2[256];
	int		num_args;
	int		syscall_num;
	int		entry_num = 0;
	int		max_syscall = 200;
	syscall_tab_t	tab;

	if (argc < 3) {
		fprintf(stderr, "usage: %s syscall-table emulated-syscalls\n",
			argv[0]);
		exit(1);
	}

	if ( (fd = fopen(argv[1], "r")) == NULL ) {
		perror("open");
		exit(1);
	}

	/* read in the syscall table */
	tab = NewArray(struct syscall_entry, max_syscall);

	while( fgets(buf, sizeof(buf), fd) != NULL ) {
		if (buf[0] != '#' && buf[0] != '\n') {

			if (entry_num >= max_syscall) {
				max_syscall += 200;

				tab = (syscall_tab_t)realloc(tab,
					     sizeof(struct syscall_entry) * max_syscall);
			}
							     
			sscanf(buf, "%d %s %s %d", &syscall_num, b1, b2, &num_args);
			tab[entry_num].call_num = syscall_num;
			tab[entry_num].name = NewStr(b1);
			tab[entry_num].protocol = b2[0];
			tab[entry_num].num_args = num_args;
			tab[entry_num].mask = 0;
			entry_num++;
		}
	}

	fclose(fd);

	/* process desired syscalls */
	if ( (fd = fopen(argv[2], "r")) == NULL ) {
		perror("open");
		exit(1);
	}

	while( fgets(buf, sizeof(buf), fd) != NULL ) {
		char	name[256];
		int	i;
		int	ret, mask;

		ret = sscanf(buf, "%s %x",name, &mask);
		if ( ret < 1 || name[0] == '#' ) continue;

		if (ret == 1) mask = 0xffffffff;

		/* find it in the table (linear algorithms for linear people) */
		for (i=0; i<entry_num; i++) {
			if (strcmp(name, tab[i].name) == 0) {
				tab[i].mask = mask;
				break;
			}
		}

		if (i == entry_num) {
			fprintf(stderr, ">> Error: unknown syscall '%s'\n",
				name);
		}

	}

	fclose(fd);
	fd = stdout;


	fprintf(fd, "\n\n#include <syscall_table.h>\n\n\n");


	/* generate a mask vector and a syscall entry vector */
	fprintf(fd, "int emul_vec[] = {\n");
	syscall_num = 0;
	for (i=0; i<entry_num; i++) {
		while (syscall_num < tab[i].call_num) {
			fprintf(fd, "\t0,\n");
			syscall_num++;
		}
		fprintf(fd, "\t%#x,\n", tab[i].mask);
		syscall_num++;
	}
	fprintf(fd, "};\n");

	/* generate forward references */
	fprintf(fd, "\n\nextern int emul_generic();\n");
	for (i=0; i<entry_num; i++) {
		if (tab[i].mask != 0) {
			fprintf(fd, "extern int emul_%s();\n", tab[i].name);
		}
	}
	fprintf(fd, "\n\n");

	/* syscall string table */
	fprintf(fd, "static char	UNDEF[] = \"<undefined>\";\n");
	fprintf(fd, "\nchar * emul_names[] = {\n");
	syscall_num = 0;
	for (i=0; i<entry_num; i++) {
		while (syscall_num < tab[i].call_num) {
			fprintf(fd, "\tUNDEF,\n");
			syscall_num++;
		}
		if (tab[i].mask != 0) {
			fprintf(fd, "\t\"emul_%s\",\n", tab[i].name);
		} else {
			fprintf(fd, "\tUNDEF,\n");
		}
		syscall_num++;
	}
	fprintf(fd, "};\n\n");

	/* generate syscall entries */
	fprintf(fd, "struct sysent	sysent[] = {\n");
	syscall_num = 0;
	for (i=0; i<entry_num; i++) {
		while (syscall_num < tab[i].call_num) {
			fprintf(fd, "\tsysg,\n");
			syscall_num++;
		}
		if (tab[i].mask != 0) {
			fprintf(fd, "\tsys%c(emul_%s,%d),\n", 
				tab[i].protocol,
				tab[i].name, tab[i].num_args);
		} else {
			fprintf(fd, "\tsysg,\n");
		}
		syscall_num++;
	}
	fprintf(fd, "};\n");

	fprintf(fd, "\nint nsysent = sizeof(sysent)/sizeof(struct sysent);\n");

#if 0
	/* generate a reference to emul_initialize */
	fprintf(fd, "\nstatic xxx()\n{\n\t/* never called ... */\n\t\
emul_initialize();\n}\n");
#endif
	exit(0);
}
