/*
 * cas_utils.c
 *
 * Utilities for the central authentication service:
 *	name hash table;
 *	transitive closure operations; and
 *	database file reading and writing.
 *
 */

/*
 * HISTORY:
 * $Log:	cas_utils.c,v $
 * Revision 1.4  92/03/05  15:11:26  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:16:10  jms]
 * 
 * Revision 1.3  89/05/18  09:42:06  dorr
 * 	add lookup_id() (to look up group record by id)
 * 	[89/05/15  12:24:45  dorr]
 * 
 * 31-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Restructured do_transitive_closure so that the list produced does
 *	not include the group for which the transitive closure is being done.
 *
 * 14-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added encrypt_password function.
 *
 *  2-Mar-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#include <mach.h>
#include <stdio.h>
#include <mach/boolean.h>

#include "auth_defs.h"
#include "cas_defs.h"

#if	ENCRYPT_PASSWORDS
#include <ctype.h> 
#include "key_defs.h"
#include "newdes.h"
#endif	ENCRYPT_PASSWORDS

/*
 * The group ID map.
 */
group_record_ptr_t	group_id_map[GROUP_ID_MAX];


/*
 * Definition of name hash table.
 */
#define INIT_NAME_HASH_TABLE_SIZE	32
#define INIT_NAME_HASH_TABLE_LIMIT	25
#define NAME_HASH_TABLE_ENTRY_SIZE	(sizeof(group_record_ptr_t))

static group_record_ptr_t	*name_hash_table;
static int			name_hash_table_size = INIT_NAME_HASH_TABLE_SIZE;
static int			name_hash_table_limit = INIT_NAME_HASH_TABLE_LIMIT;
static int			name_hash_table_num_entries = 0;

#define HASH_NAME(index, group_name) {				\
    register char *cp;						\
    index = 0;							\
    for (cp = group_name; *cp != 0; cp++) index += (int)*cp;	\
    index %= name_hash_table_size;				\
}



/*
 * cas_utils_init
 *	Initialises the name hash table.
 *
 * Results:
 *	TRUE if success, FALSE otherwise.
 *
 */
boolean_t cas_utils_init()
{
    vm_address_t	data_ptr = (vm_address_t)0;
    vm_size_t		size;
    kern_return_t	kr;

    size = INIT_NAME_HASH_TABLE_SIZE * NAME_HASH_TABLE_ENTRY_SIZE;
    if ((kr = vm_allocate(mach_task_self(), &data_ptr, size, TRUE)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_utils_init.vm_allocate fails, kr = %d.\n", kr);
	return FALSE;
    }
    else name_hash_table = (group_record_ptr_t *)data_ptr;

    return TRUE;
}



/*
 * grow_name_hash_table
 *	doubles the size of the name hash table.
 *
 * Side effects:
 *	a new hash table with the entries from the old one rehashed into it
 *
 * Design:
 *	Create a new hash table that is twice as big as the old one.
 *	Rehash the entries from the old table into the new one.
 *	Deallocate the old table.
 *
 */
void grow_name_hash_table()
{
    vm_address_t	data_ptr = (vm_address_t)0;
    kern_return_t	kr;
    vm_size_t		size;
    int			old_size, new_size, i;
    group_record_ptr_t	*new_hash_table, *old_hash_table;

    old_size = name_hash_table_size;
    new_size = 2 * old_size;
    old_hash_table = name_hash_table;

    /*
     * Now create and initialise the new hash table.
     */
    size = new_size * NAME_HASH_TABLE_ENTRY_SIZE;
    if ((kr = vm_allocate(mach_task_self(), &data_ptr, size, TRUE)) != KERN_SUCCESS) {
	fprintf(stderr, "grow_name_hash_table.vm_allocate fails, kr = %d.\n", kr);
	return;
    }

    name_hash_table = new_hash_table = (group_record_ptr_t *)data_ptr;
    name_hash_table_limit *= 2;
    name_hash_table_size = new_size;

    /*
     * Rehash the entries from the old hash table into the new one.
     */
    for (i = 0; i < old_size; i++) {
	int	index;

	HASH_NAME(index, old_hash_table[i]->group_name);
	while (new_hash_table[index] != GROUP_RECORD_NULL) {
	    index ++;
	    index %= new_size;
	}
	new_hash_table[index] = old_hash_table[i];
    }

    size = old_size * NAME_HASH_TABLE_ENTRY_SIZE;
    if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)old_hash_table, size)) != KERN_SUCCESS) {
	fprintf(stderr, "grow_name_hash_table.vm_deallocate fails, kr = %d.\n", kr);
    }

}



/*
 * delete_name
 *
 * Parameters:
 *	group_name	: group name to be deleted from the hash table.
 *
 */
void delete_name(group_name)
group_name_t	group_name;
{
    int 		index, original_index;
    group_record_ptr_t	rehash_entry;

    HASH_NAME(index, group_name);
    original_index = index;
    while ((name_hash_table[index] != GROUP_RECORD_NULL)
		&& ((strcmp(group_name, name_hash_table[index]->group_name)) != 0))
    {
	index ++;
	index %= name_hash_table_size;
	if (index == original_index) {
	    fprintf(stderr, "delete_name: hash table full.\n");
	    return;
	}
    }

    if (name_hash_table[index] == GROUP_RECORD_NULL) {
	fprintf(stderr, "delete_name: no entry for %s in the name table.\n", group_name);
	return;
    }

    name_hash_table[index] = GROUP_RECORD_NULL;

    /*
     * Must rehash entries after the deleted one.
     */
    index ++;
    while ((rehash_entry = name_hash_table[index]) != GROUP_RECORD_NULL) {
	name_hash_table[index] = GROUP_RECORD_NULL;
	enter_name(rehash_entry);
	index ++;
    }

}



/*
 * enter_name
 *
 * Parameters:
 *	group_record_ptr	: pointer to the group record to be entered in the hash table.
 *
 * Note:
 *	we assume that there does not already exist an entry for this group name in the hash table.
 *
 */
void enter_name(group_record_ptr)
group_record_ptr_t	group_record_ptr;
{
    int index, original_index;

    name_hash_table_num_entries ++;
    if (name_hash_table_num_entries > name_hash_table_limit) {
	grow_name_hash_table();
    }

    HASH_NAME(index, group_record_ptr->group_name);
    original_index = index;
    while (name_hash_table[index] != GROUP_RECORD_NULL) {
	index ++;
	index %= name_hash_table_size;
	if (index == original_index) {
	    fprintf(stderr, "enter_name: hash table full.\n");
	    grow_name_hash_table();
	    /*
	     * Retry this call to enter.
	     */
	    enter_name(group_record_ptr);
	    return;
	}
    }

    /*
     * Fill in the new entry.
     */
    name_hash_table[index] = group_record_ptr;

}



/*
 * lookup_name
 *	looks up a group name in the name hash table.
 *
 * Parameters:
 *	group_name	: the host to look up
 *
 * Results:
 *	A pointer to the record for this host (GROUP_RECORD_NULL if not found).
 *
 */
group_record_ptr_t lookup_name(group_name)
group_name_t	group_name;
{
    int			index, original_index;

    HASH_NAME(index, group_name);
    original_index = index;
    while ((name_hash_table[index] != GROUP_RECORD_NULL)
		&& ((strcmp(group_name, name_hash_table[index]->group_name)) != 0))
    {
	index ++;
	index %= name_hash_table_size;
	if (index == original_index) {
	    fprintf(stderr, "lookup_name: hash table full");
	    return GROUP_RECORD_NULL;
	}
    }

    return name_hash_table[index];
}

/*
 * lookup_id
 *	looks up a group id
 *
 * Parameters:
 *	group_id	: the id to look up
 *
 * Results:
 *	A pointer to the record for this host (GROUP_RECORD_NULL if not found).
 *
 */
group_record_ptr_t lookup_id(group_id)
group_id_t	group_id;
{
    if (group_id < 0 || group_id >= GROUP_ID_MAX)
	    return GROUP_RECORD_NULL;

    return group_id_map[group_id];
}

/*
 * check_for_membership
 *	check that one group is in the membership list of another group.
 *
 * Parameters:
 *	group_id	: the group of which group_rec_ptr is or is not a member.
 *	group_rec_ptr	: the group to be checked for membership.
 *
 * Results:
 *	TRUE or FALSE as appropriate
 *
 */
boolean_t check_for_membership(group_id, group_rec_ptr)
register group_id_t	group_id;
group_record_ptr_t	group_rec_ptr;
{
    register boolean_t		ok = FALSE;
    register int		i;
    register int		group_id_count = group_rec_ptr->tr_memberships_count;
    register group_id_list_t	group_id_list = group_rec_ptr->tr_memberships;

    if (group_rec_ptr->group_id == group_id) return TRUE;
    for (i = 0; i < group_id_count; i++) {
	if (group_id == group_id_list[i]) {
	    ok = TRUE;
	    break;
	}
    }

    return(ok);
}



/*
 * delete_from_list
 *	delete a group id from a list and close up the gap.
 *
 * Parameters:
 *	group_id_list	: the list to be modified.
 *	count_ptr	: pointer to the current number of groups in the list.
 *	delete_group	: the group to be deleted.
 *
 * Results:
 *	count_ptr	: set to be the resulting number of groups in the list.
 *
 */
void delete_from_list(group_id_list, count_ptr, delete_group)
group_id_list_t	group_id_list;
int		*count_ptr;
group_id_t	delete_group;
{
    int		i;

    for (i = 0; i < *count_ptr; i++) {
	if (group_id_list[i] == delete_group) {
	    if (i == ((*count_ptr) - 1)) {
		/*
		 * This is the last entry in the list - set it to NULL.
		 */
		group_id_list[i] = NULL_ID;
	    }
	    else {
		/*
		 * Move the last entry in the list down to the position of the deleted group.
		 */
		group_id_list[i] = group_id_list[(*count_ptr) - 1];
		group_id_list[(*count_ptr) - 1] = NULL_ID;
	    }
	    *count_ptr = (*count_ptr) - 1;
	    return;
	}
    }

    fprintf(stderr, "delete_from_list: group %d not deleted from list.\n", delete_group);

}



/*
 * do_transitive_closure
 *	calculate the transitive closure of memberships for a group.
 *
 * Parameters:
 *	tr_rec_ptr		: pointer to the record for the group in question.
 *	do_memberships		: do closure for the memberships or members of this group.
 *
 * Returns:
 *	group_ids_ptr		: pointer to the transitive closure list.
 *	group_ids_count_ptr	: pointer to the number of groups in the list.
 *
 * Design:
 *	Mark all the groups as not touched.
 *	In two phases examine recursively all the groups reachable via this group.
 *	In the first phase count the number of such groups.
 *	Allocate enough space to list them.
 *	In the second phase copy such groups into the transitive closure list.
 *
 * Note:
 *	we use the touched entry in the group records to do the recursive transitive closure.
 *
 */
void do_recursive_search();

void do_transitive_closure(tr_rec_ptr, do_memberships, group_ids_ptr, group_ids_count_ptr)
group_record_ptr_t	tr_rec_ptr;
boolean_t		do_memberships;
group_id_list_t		*group_ids_ptr;
int			*group_ids_count_ptr;
{
    int			group_count, i;
    group_id_list_t	group_ids = (group_id_list_t)0;
    kern_return_t	kr;
    vm_size_t		size;

    for (i = 0; i < GROUP_ID_MAX; i++)
	if (group_id_map[i] != GROUP_RECORD_NULL) group_id_map[i]->touched = FALSE;
    group_count = 0;

    if (do_memberships)
	for (i = 0; i < tr_rec_ptr->memberships_count; i++) {
	    do_recursive_search(tr_rec_ptr->memberships[i], TRUE, FALSE, (group_id_list_t)0, &group_count);
	}
    else
	for (i = 0; i < tr_rec_ptr->members_count; i++) {
	    do_recursive_search(tr_rec_ptr->members[i], FALSE, FALSE, (group_id_list_t)0, &group_count);
	}

    /*
     * Allocate the right amount of space.
     */
    size = group_count * sizeof(group_id_t);
    if ((kr = vm_allocate(mach_task_self(), (vm_address_t *)&group_ids, size, TRUE)) != KERN_SUCCESS) {
	fprintf(stderr, "do_transitive_closure.vm_allocate fails, kr = %d.\n", kr);
	*group_ids_ptr = 0;
	*group_ids_count_ptr = 0;
    }
    
    *group_ids_ptr = group_ids;
    *group_ids_count_ptr = group_count;
    group_count = 0;
    for (i = 0; i < GROUP_ID_MAX; i++)
	if (group_id_map[i] != GROUP_RECORD_NULL) group_id_map[i]->touched = FALSE;

    if (do_memberships)
	for (i = 0; i < tr_rec_ptr->memberships_count; i++) {
	    do_recursive_search(tr_rec_ptr->memberships[i], TRUE, TRUE, group_ids, &group_count);
	}
    else
	for (i = 0; i < tr_rec_ptr->members_count; i++) {
	    do_recursive_search(tr_rec_ptr->members[i], FALSE, TRUE, group_ids, &group_count);
	}

}


void do_recursive_search(group_id, do_memberships, copy_group_ids, group_ids, group_count_ptr)
group_id_t		group_id;
boolean_t		do_memberships;
boolean_t		copy_group_ids;
group_id_list_t		group_ids;
int			*group_count_ptr;
{
    int		i;

    if (group_id_map[group_id] == GROUP_RECORD_NULL) return;
    if (group_id_map[group_id]->touched) return;

    /*
     * This group should be examined.
     */
    group_id_map[group_id]->touched = TRUE;
    if (copy_group_ids) group_ids[*group_count_ptr] = group_id;
    (*group_count_ptr)++;
    if (do_memberships) {
	for (i = 0; i < group_id_map[group_id]->memberships_count; i++) {
	    do_recursive_search(group_id_map[group_id]->memberships[i], do_memberships, copy_group_ids,
					group_ids, group_count_ptr);
	}
    }
    else {
	for (i = 0; i < group_id_map[group_id]->members_count; i++) {
	    do_recursive_search(group_id_map[group_id]->members[i], do_memberships, copy_group_ids,
					group_ids, group_count_ptr);
	}
    }
}



#if	ENCRYPT_PASSWORDS
/*
 * encrypt_password
 *	encrypts a password with a one-way function.
 *
 * Design:
 *	Uses the newdes encryption algorithm: the password is both cleartext and key.
 *
 * Note:
 *	Should use the Unix password encryption.
 *
 */
void encrypt_password(password)
pass_word_t	password;
{
    int		length, i;
    key_t	key;    

    /*
     * Construct the clear-text and key from the password.
     */
    length = strlen(password);
    for (i = length; i < PASS_WORD_SIZE; i++) password[i] = password[i - length];
    for (i = 0; i < 16; i++) key.key_bytes[i] = password[PASS_WORD_SIZE - i - 1];

    /*
     * Now do the encryption.
     */
    newdesencrypt(key, &password[0]);
    newdesencrypt(key, &password[8]);

    /*
     * Convert the password to a printable representation.
     */
    for (i = 0; i < PASS_WORD_SIZE; i++) {
	if (!isascii(password[i])) password[i] = toascii(password[i]);
	if (password[i] == 0177) password[i] = 0176;
	if (!isprint(password[i])) password[i] = password[i] + 040;
    }

}
#endif	ENCRYPT_PASSWORDS
