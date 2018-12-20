/*
 * cas_db.c
 *
 *	Routines to read in and save the authentication database.
 *
 */

/*
 * HISTORY
 * $Log:	cas_db.c,v $
 * Revision 1.6  92/03/05  15:11:10  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:11:39  jms]
 * 
 * Revision 1.5  89/05/17  16:54:11  dorr
 * 	include file cataclysm
 * 
 * Revision 1.4  89/05/04  17:53:13  mbj
 * 	Allow asdatabase filename to be passed in at runtime.
 * 	[89/04/17  15:26:49  mbj]
 * 
 * Revision 1.3  89/03/30  12:06:58  dpj
 * 	Modified to accept an external specification of the location
 * 	of the authentication database (DATABASEDIR).
 * 	[89/03/26  18:59:19  dpj]
 * 
 */

#include <mach.h>
#include <stdio.h>
#include <mach/boolean.h>
#include <sys/param.h>		/* For MAXPATHLEN */

#include "auth_defs.h"
#include "cas_defs.h"

#ifndef	DATABASEDIR
#define	DATABASEDIR	"/etc"
#endif
#define DATABASE_FILE	"asdatabase"
#define DATABASE_BACKUP_EXTENSION	".BAK"

char	db_name[MAXPATHLEN];
char	db_backup[MAXPATHLEN];

#define INPUT_LINE_MAX	1024

#define	PRIMARY_USAGE	"<user-name>:<group-id>:P:<full-name>:<owner-id>:<encry-password>:<membership-list>"
#define	SECONDARY_USAGE	"<group-name>:<group-id>:S:<full-name>:<owner-id>:<member-list>:<membership-list>"


/*
 * read_database
 *	reads the authentication database in from disk.
 *
 * Parameters:
 *	server_port	: ignored.
 *
 * Results:
 *	TRUE or FALSE.
 *
 * Side effects:
 *	Creates and fills in group record entries read from the database.
 *	Enters each group record into the name table.
 *
 * Design:
 *	Open the file for reading.
 *	Read line by line from the file.
 *	For each line, extrace the relevant fields from it.
 *
 */
boolean_t read_database(filename) char *filename;
{
    FILE		*database_file;
    char		input_line[INPUT_LINE_MAX];
    char		*cp, field[64];
    group_record_ptr_t	group_rec_ptr;
    kern_return_t	kr;
    int			mem_count, i;
    vm_size_t		size;

    if (filename && *filename) {
	strcpy(db_name, filename);
    } else {
	strcpy(db_name,DATABASEDIR);
	strcat(db_name,"/");
	strcat(db_name,DATABASE_FILE);
    }

    strcpy(db_backup, db_name);
    strcat(db_backup, DATABASE_BACKUP_EXTENSION);

    if (!(database_file = fopen(db_name, "r"))) {
	fprintf(stderr, "read_database: cannot open database file '%s'.\n", db_name);
	return FALSE;
    }

    while (fgets(input_line, INPUT_LINE_MAX, database_file)) {
	cp = (char *)input_line;
	if (*cp == '#') {
	    /*
	     * Comment line - ignore.
	     */
	    continue;
	}

	group_rec_ptr = GROUP_RECORD_NULL;
	size = sizeof(group_record_t);
	if ((kr = vm_allocate(mach_task_self(), (vm_address_t *)&group_rec_ptr, size, TRUE)) != KERN_SUCCESS) {
	    fprintf(stderr, "read_database.vm_allocate fails, kr = %d.\n", kr);
	    return FALSE;
	}
	group_rec_ptr->tr_memberships_count = 0;

	/*
	 * First the group name.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) group_rec_ptr->group_name[i++] = *(cp++);
	group_rec_ptr->group_name[i] = '\0';
	cp++;

	/*
	 * Next the group ID.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) field[i++] = *(cp++);
	field[i] = '\0';
	group_rec_ptr->group_id = atoi(field);
	group_id_map[group_rec_ptr->group_id] = group_rec_ptr;
	cp++;

	/*
	 * Next the group type.
	 */
	group_rec_ptr->group_type = (*cp == 'P') ? AS_PRIMARY : AS_SECONDARY;
	cp++; cp++;

	/*
	 * Next the full name.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) group_rec_ptr->full_name[i++] = *(cp++);
	group_rec_ptr->full_name[i] = '\0';
	cp++;

	/*
	 * Next the owner ID.
	 */
	i = 0;
	while ((*cp) && (*cp != ':')) field[i++] = *(cp++);
	field[i] = '\0';
	group_rec_ptr->owner_id = atoi(field);
	cp++;

	/*
	 * Next the password or members depending on the group type.
	 */
	if (group_rec_ptr->group_type == AS_PRIMARY) {
#if	ENCRYPT_PASSWORDS
	    for (i = 0; i < PASS_WORD_SIZE; i++) group_rec_ptr->pass_word[i] = *cp++;
#else	ENCRYPT_PASSWORDS
	    i = 0;
	    while ((*cp) && (*cp != ':')) group_rec_ptr->pass_word[i++] = *(cp++);
	    group_rec_ptr->pass_word[i] = '\0';
#endif	ENCRYPT_PASSWORDS
	    cp++;
	    group_rec_ptr->members_count = 0;
	}
	else {
	    mem_count = 0;
	    while ((*cp) && (*cp != ':')) {
		i = 0;
		while ((*cp) && (*cp != ':') && (*cp != ',')) field[i++] = *(cp++);
		field[i] = '\0';
		group_rec_ptr->members[mem_count++] = atoi(field);
		if (*cp == ',') cp++;
	    }
	    group_rec_ptr->members_count = mem_count;
	    cp++;
	}

	/*
	 * Lastly the memberships.
	 */
	mem_count = 0;
	while ((*cp) && (*cp != ':')) {
	    i = 0;
	    while ((*cp) && (*cp != ':') && (*cp != ',')) field[i++] = *(cp++);
	    field[i] = '\0';
	    group_rec_ptr->memberships[mem_count++] = atoi(field);
	    if (*cp == ',') cp++;
	}
	group_rec_ptr->memberships_count = mem_count;
    }

    /*
     * Lastly construct the name database.
     */
    for (i = 0; i < GROUP_ID_MAX; i++) {
	if (group_id_map[i] != GROUP_RECORD_NULL) enter_name(group_id_map[i]);
    }

    (void)fclose(database_file);
    return TRUE;
}



/*
 * save_database
 *	write out the authentication database.
 *
 * Side effects:
 *	Move the old database file to a backup file.
 *
 * Design:
 *	For each group known about, construct and write out its entry in the database.
 *	Save the database file.
 *
 */
void save_database()
{
    FILE		*database_file;
    group_record_ptr_t	group_rec_ptr;
    int			i, j, k;
    char		output_line[INPUT_LINE_MAX], mem_id[10], *cp;

    if ((rename(db_name, db_backup)) != 0) {
	fprintf(stderr, "save_database.rename of %s to %s fails.\n", db_name, db_backup);
	return;
    }

    if (!(database_file = fopen(db_name, "w"))) {
	fprintf(stderr, " save_database.fopen of %s fails,\n", db_name);
	return;
    }

    fprintf(database_file, "#\n");
    fprintf(database_file, "# Database format:\n");
    fprintf(database_file, "#     Primary groups:   %s\n", PRIMARY_USAGE);
    fprintf(database_file, "#     Secondary groups: %s\n", SECONDARY_USAGE);
    fprintf(database_file, "#\n");

    for (i = 0; i < GROUP_ID_MAX; i++) {
	if ((group_rec_ptr = group_id_map[i]) != GROUP_RECORD_NULL) {
	    /*
	     * Write out this group record.
	     */
	    if (group_rec_ptr->group_type == AS_PRIMARY) {
#if	ENCRYPT_PASSWORDS
		(void)sprintf(output_line,"%s:%d:%s:%s:%d:",
			group_rec_ptr->group_name, group_rec_ptr->group_id,
			(group_rec_ptr->group_type == AS_PRIMARY) ? "P" : "S",
			group_rec_ptr->full_name, group_rec_ptr->owner_id);
		cp = &(output_line[strlen(output_line)]);
		for (j = 0; j < PASS_WORD_SIZE; j++) *cp++ = group_rec_ptr->pass_word[j];
		*(cp++) = ':';
#else	ENCRYPT_PASSWORDS
		(void)sprintf(output_line,"%s:%d:%s:%s:%d:%s:",
			group_rec_ptr->group_name, group_rec_ptr->group_id,
			(group_rec_ptr->group_type == AS_PRIMARY) ? "P" : "S",
			group_rec_ptr->full_name, group_rec_ptr->owner_id,
			group_rec_ptr->pass_word);
		cp = &(output_line[strlen(output_line)]);
#endif	ENCRYPT_PASSWORDS
	    }
	    else {
		(void)sprintf(output_line,"%s:%d:%s:%s:%d:",
			group_rec_ptr->group_name, group_rec_ptr->group_id,
			(group_rec_ptr->group_type == AS_PRIMARY) ? "P" : "S",
			group_rec_ptr->full_name, group_rec_ptr->owner_id);
		cp = &(output_line[strlen(output_line)]);

		/*
		* Now the members.
		*/
		for (j = 0; j < group_rec_ptr->members_count; j++) {
		    if (j != 0) *(cp++) = ',';
		    (void)sprintf(mem_id, "%d", group_rec_ptr->members[j]);
		    for (k = 0; k < strlen(mem_id); k++) *(cp++) = mem_id[k];
		}
		*(cp++) = ':';
	    }

	    /*
	     * Lastly the memberships.
	     */
	    for (j = 0; j < group_rec_ptr->memberships_count; j++) {
		if (j != 0) *(cp++) = ',';
		(void)sprintf(mem_id, "%d", group_rec_ptr->memberships[j]);
		for (k = 0; k < strlen(mem_id); k++) *(cp++) = mem_id[k];
	    }
	    *(cp++) = ':';
	    *(cp++) = '\n';
	    *(cp++) = '\0';

	    fputs(output_line, database_file);
	}
    }

    (void)fclose(database_file);

}
