/*
 * encrypt_db.c
 *	encrypt the passwords in an AS database.
 *	Reads in the database from a named file, writes it out to stdout.
 *
 */

/*
 * HISTORY:
 * $Log:	encrypt_db.c,v $
 * Revision 1.2  89/05/04  17:53:46  mbj
 * 	Correct error message.
 * 	[89/04/17  15:28:35  mbj]
 * 
 */

#include <stdio.h>

#include "auth_defs.h"
#include "cas_defs.h"
#include "key_defs.h"
#include "newdes.h"

main(argc, argv)
int	argc;
char	**argv;
{
    FILE		*database_file;
    char		input_line[256];
    char		*cp, field[64];
    int			mem_count, i;
    group_name_t	group_name;
    group_id_t		group_id, owner_id;
    group_type_t	group_type;
    full_name_t		full_name;
    pass_word_t		pass_word;

    if (argv[1] == 0) {
	database_file = stdin;
	fprintf(stderr, "database file is stdin.\n");
    }
    else if (!(database_file = fopen(argv[1], "r"))) {
	fprintf(stderr, "encrypt_db: cannot open database file '%s'.\n", argv[1]);
	exit(-1);
    }
    else fprintf(stderr, "database file %s opened for reading.\n", argv[1]);

    while (fgets(input_line, 256, database_file)) {
	cp = (char *)input_line;

	if (*cp == '#') {
	    /*
	     * Comment line - ignore.
	     */
	    fprintf(stdout, "%s", input_line);
	    continue;
	}

	/*
	 * First the group name.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) group_name[i++] = *(cp++);
	group_name[i] = '\0';
	cp++;

	/*
	 * Next the group ID.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) field[i++] = *(cp++);
	field[i] = '\0';
	group_id = atoi(field);
	cp++;

	/*
	 * Next the group type.
	 */
	group_type = (*cp == 'P') ? AS_PRIMARY : AS_SECONDARY;
	cp++; cp++;

	/*
	 * Next the full name.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) full_name[i++] = *(cp++);
	full_name[i] = '\0';
	cp++;

	/*
	 * Next the owner ID.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) field[i++] = *(cp++);
	field[i] = '\0';
	owner_id = atoi(field);
	cp++;

	/*
	 * Next the password.
	 */
	if (group_type == AS_PRIMARY) {
	    i = 0;
	    while ((*cp) && (*cp != ':')) pass_word[i++] = *(cp++);
	    pass_word[i] = '\0';
	    fprintf(stderr, "%8s: unencrypted password = '%8s' - ", group_name, pass_word);
	    encrypt_password(pass_word);
	    fprintf(stderr, "encrypted password = '", pass_word);
	    for (i = 0; i < PASS_WORD_SIZE; i++) fprintf(stderr, "%c", pass_word[i]);
	    fprintf(stderr, "'.\n");
	}

	/*
	 * cp now points to the rest of the input_line.
	 */

	fprintf(stdout, "%s:%d:%s:%s:%d:", group_name, group_id, (group_type == AS_PRIMARY) ? "P" : "S",
			full_name, owner_id);
	if (group_type == AS_PRIMARY) {
	    for (i = 0; i < PASS_WORD_SIZE; i++) fprintf(stdout, "%c", pass_word[i]);
	}
	fprintf(stdout, "%s", cp);
    }
}
