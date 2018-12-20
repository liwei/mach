/*
 * cas_procs.c
 *
 *	Main routines for the central authentication server.
 *
 */

/*
 * HISTORY:
 * $Log:	cas_procs.c,v $
 * Revision 1.5  92/03/05  15:11:16  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:15:34  jms]
 * 
 * Revision 1.4  91/10/07  00:12:32  jjc
 * 	Removed some debugging statements from cas_login_create().
 * 	[91/04/25            jjc]
 * 
 * Revision 1.3  90/12/19  11:06:24  jjc
 * 	Created cas_create_group_rec() to make a group record and
 * 	insert it into the hash table.
 * 	Changed cas_login_create() to use it to create the user 
 * 	and/or group.
 * 	Changed cas_register_token() to take an additional argument
 * 	that says whether to create any nonexistent groups in the
 * 	given group list rather than give up.
 * 	[90/12/05            jjc]
 * 	Added cas_login_create() to login a user and create the user
 * 	and group if either does not exist yet.
 * 	[90/10/09            jjc]
 * 
 * Revision 1.2  89/05/18  09:41:54  dorr
 * 	add cas_login_priv()
 * 	[89/05/15  12:23:55  dorr]
 * 
 *
 * 26-Oct-87  Robert Sansom (rds) at Carnegie Mellon University
 *	No longer need to remove ourself from a transitive closure list.
 *
 * 21-Oct-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added cas_change_owner.  Added owner_id to translate routines.
 *	Gave the system administrator more power.
 *
 *  3-Aug-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Fixed up handling of the registered port map.
 *	Made sure all names are converted to lower case.
 *
 * 14-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added optional handling of encrypted passwords.
 *
 *  2-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Improved port hashing.  Added local constant PORT_MAP_MAX.
 *
 *  9-Apr-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Replaced cas_verify_token by cas_verify_token_ids and cas_verify_token_names.
 *
 *  2-Mar-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#include <ctype.h>
#include <mach.h>
#include <stdio.h>
#include <strings.h>
#include <mach/mach_param.h>
#include <mach/message.h>
#include <mach/notify.h>
#include "auth_defs.h"
#include "cas_defs.h"

#define to_lower(name) {			\
    char *cp;					\
    for (cp = name; *cp; cp++)			\
	if ((isalpha(*cp)) && (isupper(*cp)))	\
	    *cp = tolower(*cp);			\
}

/*
 * Arrays used to:
 *	map private ports to group records,
 *	map registered to private ports, so that all registered ports associated with a login
 *		can be removed when the user logs out (either explicitly or implicitly); and
 *	map registered ports to verification information.
 *
 */
#define PORT_MAP_MAX	512

static struct {
    mach_port_t		mapped_port;
    group_record_ptr_t	group_record_ptr;
} priv_port_map[PORT_MAP_MAX];

static struct {
    mach_port_t		mapped_port, private_port;
    group_id_list_t	group_ids;
    int			group_ids_count;
    group_id_t		user_id;
} reg_port_map[PORT_MAP_MAX];

#define HASH_PORT(port, map, index) {						\
    (index) = ((((port) >> 8) & 0xff) ^ ((port) & 0xff)) & (PORT_MAP_MAX - 1);	\
    while (((map)[(index)].mapped_port != (port))				\
	&& ((map)[(index)].mapped_port != MACH_PORT_NULL))				\
    (index) = ++(index) & (PORT_MAP_MAX - 1);					\
}


/* Port to request notifications be sent to */
extern mach_port_t	task_notify_port;

#define request_notify(port) { \
	mach_port_t	previous_port_dummy = MACH_PORT_NULL;	\
	mach_port_request_notification(mach_task_self(),	\
	    (port), MACH_NOTIFY_DEAD_NAME, 1,	\
	    task_notify_port, MACH_MSG_TYPE_MAKE_SEND_ONCE,	\
	    &previous_port_dummy);	\
	    if (previous_port_dummy != MACH_PORT_NULL) {	\
		    mach_port_deallocate(mach_task_self(),	\
			    previous_port_dummy);	\
	    }	\
	}

/*
 * cas_login
 *	login a user with the central authentication server.
 *
 * Parameters:
 *	server_port	: ignored.
 *	user_name	: the user trying to log in.
 *	pass_word	: the clear-text password for the user.
 *	private_port	: a private port representing the user to the authentication service.
 *
 * Side effects:
 *	Make a mapping entry between the private port and the group record.
 *
 * Design:
 *	Looks up the group name.
 *	Checks the password.
 *	Computes the transitive closure of the group memberships.
 *
 */
/*ARGSUSED*/
cas_login(server_port, user_name, pass_word, private_port)
mach_port_t		server_port;
group_name_t	user_name;
pass_word_t	pass_word;
mach_port_t		private_port;
{
    group_record_ptr_t	login_rec_ptr;
    kern_return_t	kr;
    int			port_index;

    to_lower(user_name);
    if ((login_rec_ptr = lookup_name(user_name)) == GROUP_RECORD_NULL) {
	return AS_BAD_NAME;
    }
    if (login_rec_ptr->group_type != AS_PRIMARY) {
	return AS_NOT_PRIMARY;
    }
#if	ENCRYPT_PASSWORDS
    encrypt_password(pass_word);
#endif	ENCRYPT_PASSWORDS
    if (!(PW_EQUAL(pass_word, login_rec_ptr->pass_word))) {
	return AS_BAD_PASS_WORD;
    }

    if (login_rec_ptr->tr_memberships_count > 0) {
	/*
	 * Deallocate the old transitive closure information.
	 */
	vm_size_t	size = login_rec_ptr->tr_memberships_count * sizeof(group_id_t);
	if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)login_rec_ptr->tr_memberships, size))
	    != KERN_SUCCESS)
	{
	    fprintf(stderr, "cas_login.vm_deallocate fails, kr = %d.\n", kr);
	}
    }

    do_transitive_closure(login_rec_ptr, TRUE,
			&(login_rec_ptr->tr_memberships), &(login_rec_ptr->tr_memberships_count));

    HASH_PORT(private_port, priv_port_map, port_index);
    priv_port_map[port_index].mapped_port = private_port;
    request_notify(private_port);
    priv_port_map[port_index].group_record_ptr = login_rec_ptr;

    return AS_SUCCESS;

}

/*
 * cas_login_priv
 *	login a user with the central authentication server.
 *	no password is required, but only the supreme user is allowed to issue this call
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: a private port representing the supreme user.
 *      user_id         : the user trying to log in.
 *	new_private_port: the private port representing the new user to the authentication service.
 *
 * Side effects:
 *	Make a mapping entry between the private port and the group record.
 *
 * Design:
 *	Verifies the identity of the supreme user.
 *	Looks up the user id.
 *	Computes the transitive closure of the group memberships.
 *
 */
/*ARGSUSED*/
cas_login_priv(server_port, private_port, user_id, new_private_port)
mach_port_t		server_port;
mach_port_t		private_port;
group_id_t      user_id;
mach_port_t		new_private_port;
{
    group_record_ptr_t	login_rec_ptr;
    group_record_ptr_t	caller_rec_ptr;
    kern_return_t	kr;
    int			port_index;

    /*
     * verify that the caller is a member of SYS_ADM
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL) {
	printf("cas_login_priv: bad_private_port\n");
	return AS_BAD_PRIVATE_PORT;
    }

    if (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)) {
	printf("cas_login_priv: not member\n");
	return AS_NOT_ALLOWED;
    }

    if ((login_rec_ptr = lookup_id(user_id)) == GROUP_RECORD_NULL) {
	printf("cas_login_priv: %d--bad name\n", user_id) ;
	return AS_BAD_NAME;
    }
    if (login_rec_ptr->group_type != AS_PRIMARY) {
	printf("cas_login_priv: %d--not primary\n", user_id) ;
	return AS_NOT_PRIMARY;
    }

    if (login_rec_ptr->tr_memberships_count > 0) {
	/*
	 * Deallocate the old transitive closure information.
	 */
	vm_size_t	size = login_rec_ptr->tr_memberships_count * sizeof(group_id_t);
	if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)login_rec_ptr->tr_memberships, size))
	    != KERN_SUCCESS)
	{
	    fprintf(stderr, "cas_login.vm_deallocate fails, kr = %d.\n", kr);
	}
    }

    do_transitive_closure(login_rec_ptr, TRUE,
			&(login_rec_ptr->tr_memberships), &(login_rec_ptr->tr_memberships_count));

    HASH_PORT(new_private_port, priv_port_map, port_index);
    priv_port_map[port_index].mapped_port = new_private_port;
    request_notify(new_private_port);
    priv_port_map[port_index].group_record_ptr = login_rec_ptr;

    printf("login_priv succeeds\n");
    return AS_SUCCESS;

}

/*
 * cas_create_group_rec
 *	Create group record for given user or group and insert it into
 *	the hash table.
 *
 * Parameters:
 *	group_id	: user ID or group ID
 *	memberships	: list of groups that this user/group belongs to
 *	memberships_count	: how many groups this user/group belongs to
 *	is_user		: whether we're creating a group record for a user
 *			  (not a group)
 *
 * Side effects:
 *	Create group record for new user or group and enter it into hash table.
 *	Add user to groups.
 *
 * Design:
 */
group_record_ptr_t cas_create_group_rec(group_id, memberships,
					 memberships_count, is_user)
    group_id_t		group_id;
    group_id_list_t	memberships;
    int			memberships_count;
    boolean_t		is_user;
{
    group_record_ptr_t	group_rec_ptr;
    int			i;
    kern_return_t	kr;
	
    /*
     * Make a new group record and put it into the hash table.
     */
    group_rec_ptr = GROUP_RECORD_NULL;
    kr = vm_allocate(mach_task_self(), (vm_address_t *)&group_rec_ptr, 
    		      sizeof(group_record_t), TRUE);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr,"cas_create_group_rec.vm_allocate fails, kr = %d.\n", kr);
        return(GROUP_RECORD_NULL);
    }
    group_rec_ptr->tr_memberships_count = 0;
    /*
     * Set group name, ID, type, full name, owner ID, password, 
     * and membership list.
     */
    (void)sprintf(group_rec_ptr->group_name, "%d", group_id);
    group_rec_ptr->group_id = group_id;
    group_rec_ptr->group_type = AS_PRIMARY;
    if (is_user) {
	(void)sprintf(group_rec_ptr->full_name, "User %d", group_id);
	(void)sprintf(group_rec_ptr->pass_word, "User %d", group_id);
    }
    else {
	(void)sprintf(group_rec_ptr->full_name, "Group %d", group_id);
	(void)sprintf(group_rec_ptr->pass_word, "Group %d", group_id);
    }
    group_rec_ptr->owner_id = SYS_ADM_ID;
    group_id_map[group_rec_ptr->group_id] = group_rec_ptr;
#if	ENCRYPT_PASSWORDS
    encrypt_password(group_rec_ptr->pass_word);
#endif	ENCRYPT_PASSWORDS

    group_rec_ptr->members_count = 0;
    group_rec_ptr->memberships_count = 0;
    if (memberships_count)
	for (i = 1; i < memberships_count; i++) {
	     group_rec_ptr->memberships[i-1] = memberships[i];
	     group_rec_ptr->memberships_count++;
	}

    enter_name(group_id_map[group_rec_ptr->group_id]);

    return(group_rec_ptr);
}



/*
 * cas_login_create
 *	login a user with the central authentication server.
 *	no password is required, but only the supreme user is allowed to issue
 *	this call.
 *	create the user if he/she doesn't exist yet.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: a private port representing the supreme user.
 *	id		: user ID, group ID, and list of group IDs.
 *	id_count	: how many IDs.
 *	new_private_port: the private port representing the new user to the
 *			  authentication service.
 *
 * Side effects:
 *	Create group record for new user and enter it into hash table.
 *	Add user to groups.
 *	Make a mapping entry between the private port and the group record.
 *
 * Design:
 *	Verifies the identity of the supreme user.
 *	Looks up the user id.
 *	Computes the transitive closure of the group memberships.
 *
 */
/*ARGSUSED*/
cas_login_create(server_port, private_port, id, id_count, new_private_port)
mach_port_t		server_port;
mach_port_t		private_port;
group_id_list_t	id;
unsigned int	id_count;
mach_port_t		new_private_port;
{
    group_record_ptr_t	caller_rec_ptr;
    int			i;
    kern_return_t	kr;
    group_id_t		group_id;
    group_record_ptr_t	group_rec_ptr;
    group_record_ptr_t	login_rec_ptr;
    int			port_index;
    group_id_t		user_id;


    /*
     * verify that the caller is a member of SYS_ADM
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL) {
	printf("cas_login_create: bad_private_port\n");
	return AS_BAD_PRIVATE_PORT;
    }

    if (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)) {
	printf("cas_login_create: not member\n");
	return AS_NOT_ALLOWED;
    }

    user_id = id[0];

    /*
     * Create new user if he/she doesn't exist
     */
    if ((login_rec_ptr = lookup_id(user_id)) == GROUP_RECORD_NULL) {
	/*
	 * Make a new group record and put it into the hash table.
	 */
	login_rec_ptr = cas_create_group_rec(user_id, id, id_count, TRUE);

	if (login_rec_ptr == GROUP_RECORD_NULL) {
            fprintf(stderr,"cas_login_create: can't create user\n");
	    return AS_BAD_NAME;
        }
    }

    group_id = id[1];

    /*
     * Create new group if it doesn't exist
     */
    if ((group_rec_ptr = lookup_id(group_id)) == GROUP_RECORD_NULL) {
	/*
;	 * Make a new group record and put it into the hash table.
	 */
	group_rec_ptr = cas_create_group_rec(group_id, 0, 0, FALSE);

	if (group_rec_ptr == GROUP_RECORD_NULL) {
            fprintf(stderr,"cas_login_create: can't create group\n");
	    return AS_BAD_NAME;
	}
    }

    if (login_rec_ptr->group_type != AS_PRIMARY) {
	printf("cas_login_create: %d--not primary\n", user_id) ;
	return AS_NOT_PRIMARY;
    }

    if (login_rec_ptr->tr_memberships_count > 0) {
	/*
	 * Deallocate the old transitive closure information.
	 */
	vm_size_t	size = login_rec_ptr->tr_memberships_count * sizeof(group_id_t);
	if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)login_rec_ptr->tr_memberships, size))
	    != KERN_SUCCESS)
	{
	    fprintf(stderr, "cas_login.vm_deallocate fails, kr = %d.\n", kr);
	}
    }

    do_transitive_closure(login_rec_ptr, TRUE,
			&(login_rec_ptr->tr_memberships), &(login_rec_ptr->tr_memberships_count));

    HASH_PORT(new_private_port, priv_port_map, port_index);
    priv_port_map[port_index].mapped_port = new_private_port;
    request_notify(new_private_port);
    priv_port_map[port_index].group_record_ptr = login_rec_ptr;

    return AS_SUCCESS;

}



/*
 * cas_logout
 *	Logs out a user.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing user who wants to be logged out.
 *
 * Side effects:
 *	Deallocates all the registered ports for this user.
 *
 * Design:
 *	Deallocate the private port.
 *	Deallocate all associated registered ports and clean up any cached information.
 *
 */
/*ARGSUSED*/
cas_logout(server_port, private_port)
mach_port_t		server_port;
mach_port_t		private_port;
{
    group_record_ptr_t	logout_rec_ptr;
    kern_return_t	kr;
    int			port_index;
    vm_size_t		size;

    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((logout_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;
		
    priv_port_map[port_index].mapped_port = MACH_PORT_NULL;
    priv_port_map[port_index].group_record_ptr = GROUP_RECORD_NULL;
    /*
     * No matter if this fails - we only have send rights to the private port anyway.
     */
    (void)mach_port_deallocate(mach_task_self(), private_port);

    /*
     * Deallocate the transitive closure information.
     */
    size = logout_rec_ptr->tr_memberships_count * sizeof(group_id_t);
    if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)logout_rec_ptr->tr_memberships, size))
	!= KERN_SUCCESS)
    {
	fprintf(stderr, "cas_logout.vm_deallocate fails, kr = %d.\n", kr);
    }
    logout_rec_ptr->tr_memberships_count = 0;

    /*
     * Check through the registered port map to see if any are associated with this private port.
     */
    for (port_index = 0; port_index < PORT_MAP_MAX; port_index ++) {
	if (reg_port_map[port_index].private_port == private_port) {
	    /*
	     * This registered port should be deallocated.
	     */
	    if ((mach_port_deallocate(mach_task_self(), reg_port_map[port_index].mapped_port)) != KERN_SUCCESS) {
		fprintf(stderr, "cas_logout.mach_port_deallocate fails, registered_port = %x.\n",
				reg_port_map[port_index].mapped_port);
	    }
	    reg_port_map[port_index].mapped_port = MACH_PORT_NULL;
	    reg_port_map[port_index].private_port = MACH_PORT_NULL;
	    reg_port_map[port_index].user_id = NULL_ID;
	    if (reg_port_map[port_index].group_ids_count != 0) {
		/*
		 * Deallocate verification information.
		 */
		size = reg_port_map[port_index].group_ids_count * sizeof(group_id_t);
		if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)reg_port_map[port_index].group_ids,
					size)) != KERN_SUCCESS)
		{
		    fprintf(stderr, "cas_logout.vm_deallocate fails, kr = %d.\n", kr);
		}
		reg_port_map[port_index].group_ids_count = 0;
	    }
	}
    }

    return AS_SUCCESS;
}



/*
 * cas_register_token
 *	registers a token which can be later used to verify the user.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_ids	: list of groups to which the token is to be restricted
*				(empty implies all groups of which user is a member).
 *	group_ids_count	: number of group ids in the list.
 *	token		: the new token.
 *	create_groups	: whether to create any nonexistent groups in list
 *
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Check that the user really is a member of all the groups on the list that is passed in.
 *	Store the resulting list of groups associated with this token.
 *	Remember the association between the token and the private port.
 *
 */
/*ARGSUSED*/
cas_register_token(server_port, private_port, group_ids, group_ids_count, 
			token, create_groups)
mach_port_t		server_port;
mach_port_t		private_port;
group_id_list_t	group_ids;
unsigned int	group_ids_count;
mach_port_t		token;
boolean_t	create_groups;
{
    kern_return_t	kr;
    group_record_ptr_t	group_record_ptr;
    int			i;
    int			pport_index, rport_index;
    vm_size_t		size;

    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, pport_index);
    if ((group_record_ptr = priv_port_map[pport_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;

    if (group_ids_count != 0) {

	if (check_for_membership(SYS_ADM_ID, group_record_ptr)
		|| create_groups) {
	    /*
	     * sys_adm can construct a token for any group combos.
	     * For each group in group_ids, check that the group exists
	     */
	    for (i = 0; i < group_ids_count; i++) {
		if (lookup_id(group_ids[i]) == GROUP_RECORD_NULL) {
		    if (!create_groups)
			return AS_BAD_GROUP;
		    else if (cas_create_group_rec(group_ids[i], 0, 0, FALSE) ==
				GROUP_RECORD_NULL)
			    return AS_BAD_GROUP;
		}
	    }
	} else {
	  /*
	   * For each group in group_ids check that it is on the tr_memberships list for the user.
	   */
	    for (i = 0; i < group_ids_count; i++) {
	         if (group_record_ptr->tr_memberships_count == 0
		     && !(check_for_membership(group_ids[i],
					group_record_ptr->tr_memberships)))
	         {
	            /*
	             * We did not found this groups in the tr_memberships list.
	             */
	            fprintf(stderr, "cas_register_token: group %d is not in the tr_memberships list.\n",
				group_ids[i]);
	            return AS_BAD_GROUP;
	        }
	    }
        }
    }

    HASH_PORT(token, reg_port_map, rport_index);
    if (reg_port_map[rport_index].group_ids_count != 0) {
	/*
	 * Deallocate the old list.
	 */
	size = reg_port_map[rport_index].group_ids_count * sizeof(group_id_t);
	if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)reg_port_map[rport_index].group_ids, size))
		!= KERN_SUCCESS)
	{
	    fprintf(stderr, "cas_register_token.vm_deallocate fails, kr = %d.\n", kr);
	}
    }

    if (group_ids_count == 0) {
	if ((reg_port_map[rport_index].group_ids_count = group_record_ptr->tr_memberships_count) > 0) {
	    /*
	     * Copy the tr_memberships list.
	     */
	    size = group_record_ptr->tr_memberships_count * sizeof(group_id_t);
	    if ((kr = vm_allocate(mach_task_self(), (vm_address_t *)&(reg_port_map[rport_index].group_ids),
				size, TRUE)) != KERN_SUCCESS)
	    {
		fprintf(stderr, "cas_register_token.vm_allocate fails, kr = %d.\n", kr);
		reg_port_map[rport_index].group_ids_count = 0;
		return AS_FAILURE;
	    }
	    for (i = 0; i < group_record_ptr->tr_memberships_count; i++) {
		reg_port_map[rport_index].group_ids[i] = group_record_ptr->tr_memberships[i];
	    }
	}
    }
    else {
	reg_port_map[rport_index].group_ids = group_ids;
	reg_port_map[rport_index].group_ids_count = group_ids_count;
    }
    reg_port_map[rport_index].mapped_port = token;
    request_notify(token);
    reg_port_map[rport_index].private_port = private_port;
    reg_port_map[rport_index].user_id = group_record_ptr->group_id;

    return AS_SUCCESS;
}



/*
 * cas_delete_token
 *	deletes a token priviously created for this user.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: the port that represents the user.
 *	token		: the token to be deleted.
 *
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Remove the entries for this token from the reg_port_map map.
 *	Deallocate the token locally.
 *
 */
/*ARGSUSED*/
cas_delete_token(server_port, private_port, token)
mach_port_t		server_port;
mach_port_t		private_port;
mach_port_t		token;
{
    kern_return_t	kr;
    int			pport_index, rport_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, pport_index);
    if (priv_port_map[pport_index].group_record_ptr == GROUP_RECORD_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(token, reg_port_map, rport_index);
    if (reg_port_map[rport_index].private_port != private_port) {
	fprintf(stderr, "cas_delete_token: private_port does not match private_port for token.\n");
	return AS_BAD_PRIVATE_PORT;
    }	

    reg_port_map[rport_index].mapped_port = MACH_PORT_NULL;
    reg_port_map[rport_index].private_port = MACH_PORT_NULL;
    reg_port_map[rport_index].user_id = NULL_ID;
    if (reg_port_map[rport_index].group_ids_count != 0) {
	/*
	 * Deallocate cached verification information.
	 */
	vm_size_t	size = reg_port_map[rport_index].group_ids_count * sizeof(group_id_t);
	if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)reg_port_map[rport_index].group_ids, size))
	    != KERN_SUCCESS)
	{
	    fprintf(stderr, "cas_delete_token.vm_deallocate fails, kr = %d.\n", kr);
	}
	reg_port_map[rport_index].group_ids_count = 0;
    }

    if ((mach_port_destroy(mach_task_self(), token)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_delete_token.mach_port_destroy fails, token = %x.\n", token);
    }

    return kr;
}



/*
 * cas_verify_token_ids
 *	returns the group IDs associated with a token.
 *
 * Parameters:
 *	server_port	: ignored.
 *	token		: the token to be verified.
 *
 * Results:
 *	user_id		: the real user represented by this token.
 *	group_ids	: the groups that are associated with this token.
 *	group_ids_count	: the number of such groups.
 *
 * Design:
 *	Just return the associated verification information.
 *
 */
/*ARGSUSED*/
cas_verify_token_ids(server_port, token, user_id_ptr, group_ids_ptr, group_ids_count_ptr)
mach_port_t		server_port;
mach_port_t		token;
group_id_t	*user_id_ptr;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    int		port_index;

    *user_id_ptr = NULL_ID;
    *group_ids_count_ptr = 0;
    *group_ids_ptr = 0;

    HASH_PORT(token, reg_port_map, port_index);

    if (reg_port_map[port_index].mapped_port == token) {
	*user_id_ptr = reg_port_map[port_index].user_id;
	*group_ids_ptr = reg_port_map[port_index].group_ids;
	*group_ids_count_ptr = reg_port_map[port_index].group_ids_count;
	return AS_SUCCESS;
    }
    else {
	return AS_FAILURE;
    }

}



/*
 * cas_verify_token_names
 *	returns the names of groups associated with a token.
 *
 * Parameters:
 *	server_port	: ignored.
 *	token		: the token to be verified.
 *
 * Results:
 *	user_name		: the real user represented by this token.
 *	group_names		: the groups that are associated with this token.
 *	group_names_count	: the number of such groups.
 *
 * Design:
 *	Allocate space to return the names.
 *	For each ID in the associated verification information
 *		place the corresponding name in the returned list.
 *
 * Note:
 *	the group_names list will be deallocated in the reply message.
 *
 */
/*ARGSUSED*/
cas_verify_token_names(server_port, token, user_name, group_names_ptr, group_names_count_ptr)
mach_port_t			server_port;
mach_port_t			token;
group_name_t		user_name;
group_name_list_t	*group_names_ptr;
unsigned int		*group_names_count_ptr;
{
    int			port_index, count;
    vm_size_t		size;
    group_record_ptr_t	group_record_ptr;
    group_id_t		group_id;
    kern_return_t	kr;

    (void)strcpy(user_name, "");
    *group_names_count_ptr = 0;
    *group_names_ptr = 0;

    HASH_PORT(token, reg_port_map, port_index);

    if (reg_port_map[port_index].mapped_port == token) {
	if ((group_record_ptr = group_id_map[reg_port_map[port_index].user_id]) == GROUP_RECORD_NULL)
	    return AS_BAD_GROUP;
	(void)strcpy(user_name, group_record_ptr->group_name);

	if ((*group_names_count_ptr = reg_port_map[port_index].group_ids_count) > 0) {
	    /*
	     * Allocate space for the group names.
	     */
	    size = (*group_names_count_ptr) * sizeof(group_name_t);
	    if ((kr = vm_allocate(mach_task_self(), (vm_address_t *)group_names_ptr, size, TRUE)) != KERN_SUCCESS)
	    {
		fprintf(stderr, "cas_verify_token_names.vm_allocate fails, kr = %d.\n", kr);
		*group_names_count_ptr = 0;
		return AS_FAILURE;
	    }

	    /*
	     * For each group in the verified group_ids, get its name.
	     */
	    for (count = 0; count < (*group_names_count_ptr); count++) {
		group_id = reg_port_map[port_index].group_ids[count];
		if ((group_record_ptr = group_id_map[group_id]) == GROUP_RECORD_NULL) {
		    fprintf(stderr, "cas_verify_token_names: bad group ID %d in verified list.\n", group_id);
		    (void)strcpy((char *)(*group_names_ptr)[count], "");
		}
		else {
		    (void)strcpy((char *)(*group_names_ptr)[count],  group_record_ptr->group_name);
		}
	    }
	}
    }

    return AS_SUCCESS;

}



/*
 * cas_create_group
 *	creates a new primary or secondary group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_name	: the name of the new group.
 *	group_type	: whether the group is a primary or a secondary group.
 *	full_name	: the full name for the new group.
 *	pass_word	: the clear-text password for the new group (only for a primary group).
 *	owner_group	: the name of the group that is to "own" the new group.
 *	group_id	: the group ID for the new group.
 *
 * Results:
 *	group_id	: the group ID for the new group.
 *
 * Design:
 *	Check the private port.
 *	If the group_type is primary, then only the SYS_ADM_ID user can create the group.
 *	Check that the owner_group exists and that the caller is a member of it.
 *	Check that the group name is not a duplicate name and the group_id, if not null, is not a duplicate.
 *	Actually create the record for the group.
 *	Write out our permanent database.
 *
 */
/*ARGSUSED*/
cas_create_group(server_port, private_port, group_name, group_type, full_name,
			pass_word, owner_group, group_id_ptr)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_type_t	group_type;
full_name_t	full_name;
pass_word_t	pass_word;
group_name_t	owner_group;
group_id_t	*group_id_ptr;
{
    kern_return_t	kr;
    group_record_ptr_t	caller_rec_ptr, owner_rec_ptr, new_rec_ptr;
    int			i, port_index;
    vm_size_t		size;
    char		*cp;

    to_lower(group_name);
    to_lower(owner_group);
    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;

    if ((group_type == AS_PRIMARY) && (!check_for_membership(SYS_ADM_ID, caller_rec_ptr))) {
	return AS_NOT_ALLOWED;
    }

    if ((strcmp(owner_group, group_name)) == 0) {
	owner_rec_ptr = GROUP_RECORD_NULL;
    }
    else {
	/*
	 * Check that the caller is a member of the owner group
	 * or is the system adminstrator.
	 */
	if ((owner_rec_ptr = lookup_name(owner_group)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;
	if ((owner_rec_ptr->group_id != caller_rec_ptr->group_id)
	    && (!check_for_membership(owner_rec_ptr->group_id, caller_rec_ptr))
	    && (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)))
	{
	    return AS_NOT_ALLOWED;
	}
    }

    /*
     * Check that the name and ID of the new group are not duplicates.
     */
    if ((new_rec_ptr = lookup_name(group_name)) != GROUP_RECORD_NULL) return AS_DUPLICATE_NAME;
    if (*group_id_ptr == NULL_ID) {
	/*
	 * Assign a new ID.
	 */
	for (i = 2; i < GROUP_ID_MAX; i++) {
	    if (group_id_map[i] == GROUP_RECORD_NULL) break;
	}
	if (i >= GROUP_ID_MAX) {
	    fprintf(stderr, "cas_create_group: no more group IDs to allocate.\n");
	    return AS_FAILURE;
	}
	*group_id_ptr = i;
    }
    else {
	if (group_id_map[*group_id_ptr] != GROUP_RECORD_NULL) return AS_DUPLICATE_ID;
    }

    /*
     * Now actually create the new group record.
     */
    size = sizeof(group_record_t);
    if ((kr = vm_allocate(mach_task_self(), (vm_address_t *)&new_rec_ptr, size, TRUE)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_create_group.vm_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }
    new_rec_ptr->group_id = *group_id_ptr;
    new_rec_ptr->group_type = group_type;
    (void)strcpy(new_rec_ptr->group_name, group_name);
    if (group_type == AS_PRIMARY) {
#if	ENCRYPT_PASSWORDS
	encrypt_password(pass_word);
	for (i = 0; i < PASS_WORD_SIZE; i++) new_rec_ptr->pass_word[i] = pass_word[i];
#else	ENCRYPT_PASSWORDS
	for (cp = pass_word; *cp; cp++) if (*cp == ':') *cp = ';';
	(void)strcpy(new_rec_ptr->pass_word, pass_word);
#endif	ENCRYPT_PASSWORDS
    }
    (void)strcpy(new_rec_ptr->full_name, full_name);
    if (owner_rec_ptr) new_rec_ptr->owner_id = owner_rec_ptr->group_id;
    else new_rec_ptr->owner_id = new_rec_ptr->group_id;
    new_rec_ptr->tr_memberships_count = 0;
    new_rec_ptr->members_count = 0;
    new_rec_ptr->memberships_count = 0;
    enter_name(new_rec_ptr);
    group_id_map[*group_id_ptr] = new_rec_ptr;

    save_database();

    return AS_SUCCESS;
}


/*
 * cas_delete_group
 *	deletes a group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_name	: the name of the group to be deleted.
 *
 * Results:
 *
 * Design:
 *	Check the private port.
 *	If the caller has the necessary access rights delete the group.
 *	Delete the group from all groups of which it is a member.
 *	For all members of this group delete the memberships.
 *	Remove all trace of the group from our records and save our database.
 *
 */
/*ARGSUSED*/
cas_delete_group(server_port, private_port, group_name)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
{
    kern_return_t	kr;
    group_record_ptr_t	caller_rec_ptr, delete_rec_ptr, modify_rec_ptr;
    int			i, port_index;
    vm_size_t		size;

    to_lower(group_name);
    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;

    if ((delete_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    /*
     * Check that the caller is a member of the owner group for the delete group
     * or is the system administrator.
     */
    if ((delete_rec_ptr->owner_id != caller_rec_ptr->group_id)
	&& (!check_for_membership(delete_rec_ptr->owner_id, caller_rec_ptr))
	&& (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)))
    {
	return AS_NOT_ALLOWED;
    }

    /*
     * Now can actually delete the group - delete the memberships first.
     */
    for (i = 0; i < delete_rec_ptr->memberships_count; i++) {
	modify_rec_ptr = group_id_map[delete_rec_ptr->memberships[i]];
	if (modify_rec_ptr != GROUP_RECORD_NULL) {
	    delete_from_list(modify_rec_ptr->members, &(modify_rec_ptr->members_count),
				delete_rec_ptr->group_id);
	}
    }
    /*
     * Now the members.
     */
    for (i = 0; i < delete_rec_ptr->members_count; i++) {
	modify_rec_ptr = group_id_map[delete_rec_ptr->members[i]];
	if (modify_rec_ptr != GROUP_RECORD_NULL) {
	    delete_from_list(modify_rec_ptr->memberships, &(modify_rec_ptr->memberships_count),
				delete_rec_ptr->group_id);
	}
    }

    /*
     * Now delete the group from the group id map and the name tables.
     */
    delete_name(delete_rec_ptr->group_name);
    group_id_map[delete_rec_ptr->group_id] = GROUP_RECORD_NULL;
    size = sizeof(group_record_t);
    if ((kr = vm_deallocate(mach_task_self(), (vm_address_t)delete_rec_ptr, size)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_delete_group.vm_deallocate fails, kr = %d.\n", kr);
    }

    /*
     * Lastly write out the database.
     */
    save_database();
    return AS_SUCCESS;
}


/*
 * cas_change_owner
 *	add a group as a member of a secondary group.
 *
 *	changes the owner of a group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_name	: the name of the group to be changed.
 *	new_owner	: the name of the new owner.
 *
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Check that the caller belongs to the ownership group for this group.
 *	Check that the new owner is a valid group.
 *	Change the owner to be the new owner.
 *
 */
/*ARGSUSED*/
cas_change_owner(server_port, private_port, group_name, new_owner)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_name_t	new_owner;
{
    group_record_ptr_t	caller_rec_ptr, change_rec_ptr, own_rec_ptr;
    int			port_index;

    to_lower(group_name);
    to_lower(new_owner);
    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;

    if ((change_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    /*
     * Check that the caller is a member of the owner group for the group to be changed
     * or is the system administrator.
     */
    if ((change_rec_ptr->owner_id != caller_rec_ptr->group_id)
	&& (!check_for_membership(change_rec_ptr->owner_id, caller_rec_ptr))
	&& (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)))
    {
	return AS_NOT_ALLOWED;
    }

    /*
     * Check the validity of the new owner group.
     */
    if ((own_rec_ptr = lookup_name(new_owner)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    /*
     * Make the change.
     */
    change_rec_ptr->owner_id = own_rec_ptr->group_id;
    save_database();

    return AS_SUCCESS;
}



/*
 * cas_change_password
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: the port representing the user.
 *	group_name	: the name of the group whose password is to be changed.
 *	old_pass_word	: the old pass_word for the group.
 *	new_pass_word	: the old pass_word for the group.
 *
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Check that the group is a primary group.
 *	The change of password is permitted if
 *		either the private port says that the caller is the owner for this group,
 *		or the private port says that the caller is the same as this group
 *			and the old password matches the current password for the group.
 *	If the change is permitted, do it and save the database.
 *
 * Note:
 *	whatever the case the private port should be valid.
 *	Any ':' characters in the password are replaced by ';'.
 *
 */
/*ARGSUSED*/
cas_change_password(server_port, private_port, group_name, old_pass_word, new_pass_word)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
pass_word_t	old_pass_word;
pass_word_t	new_pass_word;
{
    group_record_ptr_t	caller_rec_ptr, change_rec_ptr;
    char		*cp;
    int			port_index, i;

    to_lower(group_name);
    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;

    if ((change_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;
    if (change_rec_ptr->group_type != AS_PRIMARY) return AS_NOT_PRIMARY;

    if (change_rec_ptr->group_id == caller_rec_ptr->group_id) {
	/*
	 * Check that the old password matches.
	 */
#if	ENCRYPT_PASSWORDS
	encrypt_password(old_pass_word);
#endif	ENCRYPT_PASSWORDS
	if (!(PW_EQUAL(old_pass_word, change_rec_ptr->pass_word))) {
	    return AS_BAD_PASS_WORD;
	}
    }
    else {
	/*
	 * Check that the caller is a member of the owner group for the delete group
	 * or is the system administrator.
	 */
	if ((change_rec_ptr->owner_id != caller_rec_ptr->group_id)
	    && (!check_for_membership(change_rec_ptr->owner_id, caller_rec_ptr))
	    && (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)))
	{
	    return AS_NOT_ALLOWED;
	}
    }

    /*
     * If we got here then the change must be permitted.
     */
#if	ENCRYPT_PASSWORDS
    encrypt_password(new_pass_word);
    for (i = 0; i < PASS_WORD_SIZE; i++) change_rec_ptr->pass_word[i] = new_pass_word[i];
#else	ENCRYPT_PASSWORDS
    for (cp = new_pass_word; *cp; cp++) if (*cp == ':') *cp = ';';
    (void)strcpy(change_rec_ptr->pass_word, new_pass_word);
#endif	ENCRYPT_PASSWORDS
    save_database();

    return AS_SUCCESS;
}



/*
 * cas_add_to_group
 *	add a group as a member of a secondary group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_name	: the group to be modified.
 *	add_group_name	: the group to be added.
 *
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Check that the group to be modified is a secondary group.
 *	Check that the caller belongs to the ownership group for this group.
 *	Add the add_group_name to the list of members for group_name.
 *	Add the group_name to the list of memberships for add_group_name.
 *
 */
/*ARGSUSED*/
cas_add_to_group(server_port, private_port, group_name, add_group_name)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_name_t	add_group_name;
{
    group_record_ptr_t	caller_rec_ptr, change_rec_ptr, add_rec_ptr;
    int			port_index;

    to_lower(add_group_name);
    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;

    if ((change_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;
    if (change_rec_ptr->group_type != AS_SECONDARY) return AS_NOT_SECONDARY;

    if ((add_rec_ptr = lookup_name(add_group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    /*
     * Check that the caller is a member of the owner group or is the system administrator.
     */
    if ((change_rec_ptr->owner_id != caller_rec_ptr->group_id)
	&& (!check_for_membership(change_rec_ptr->owner_id, caller_rec_ptr))
	&& (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)))
    {
	return AS_NOT_ALLOWED;
    }

    if ((add_rec_ptr->memberships_count > MEMBERS_MAX) || (change_rec_ptr->members_count > MEMBERS_MAX)) {
	fprintf(stderr, "cas_add_to_group: max membership count exceeded.\n");
    }

    change_rec_ptr->members[change_rec_ptr->members_count] = add_rec_ptr->group_id;
    change_rec_ptr->members_count ++;
    add_rec_ptr->memberships[add_rec_ptr->memberships_count] = change_rec_ptr->group_id;
    add_rec_ptr->memberships_count ++;

    save_database();
    return AS_SUCCESS;
}



/*
 * cas_remove_from_group
 *	remove a group as a member from a secondary group.
 *
 * Parameters:
 *	server_port		: ignored.
 *	private_port		: port representing the user.
 *	group_name		: the group to be modified.
 *	remove_group_name	: the group to be removed.
 *
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Check that the group to be modified is a secondary group.
 *	Check that the caller belongs to the ownership group for this group.
 *	Delete the remove_group_name from the list of members for group_name.
 *	Delete the group_name from the list of memberships for remove_group_name.
 *
 */
/*ARGSUSED*/
cas_remove_from_group(server_port, private_port, group_name, remove_group_name)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_name_t	remove_group_name;
{
    group_record_ptr_t	caller_rec_ptr, change_rec_ptr, remove_rec_ptr;
    int			port_index;

    to_lower(group_name);
    to_lower(remove_group_name);
    /*
     * Check the validity of the private port.
     */
    if (private_port == MACH_PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((caller_rec_ptr = priv_port_map[port_index].group_record_ptr) == GROUP_RECORD_NULL)
	return AS_BAD_PRIVATE_PORT;

    if ((change_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;
    if (change_rec_ptr->group_type != AS_SECONDARY) return AS_NOT_SECONDARY;

    if ((remove_rec_ptr = lookup_name(remove_group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    /*
     * Check that the caller is a member of the owner group or is the system administrator.
     */
    if ((change_rec_ptr->owner_id != caller_rec_ptr->group_id)
	&& (!check_for_membership(change_rec_ptr->owner_id, caller_rec_ptr))
	&& (!check_for_membership(SYS_ADM_ID, caller_rec_ptr)))
    {
	return AS_NOT_ALLOWED;
    }

    delete_from_list(change_rec_ptr->members, &(change_rec_ptr->members_count), remove_rec_ptr->group_id);
    delete_from_list(remove_rec_ptr->memberships, &(remove_rec_ptr->memberships_count),
			change_rec_ptr->group_id);

    save_database();
    return AS_SUCCESS;
}



/*
 * cas_translate_group_id
 *	translates a group id into a group name.
 *
 * Parameters:
 *	server_port	: ignored.
 *	group_id	: the ID to be translated.
 *
 * Results:
 *	owner_id	: the owner of the group.
 *	group_name	: the name of the group.
 *	group_type	: the type of the group (primary or secondary).
 *	full_name	: the full name of the group.
 *
 * Design:
 *	Looks up the group and returns the appropriate values.
 *
 */
/*ARGSUSED*/
cas_translate_group_id(server_port, group_id, owner_id_ptr, group_name, group_type_ptr, full_name)
mach_port_t		server_port;
group_id_t	group_id;
group_id_t	*owner_id_ptr;
group_name_t	group_name;
group_type_t	*group_type_ptr;
full_name_t	full_name;
{
    group_record_ptr_t	tr_rec_ptr;

    group_name[0] = '\0';
    full_name[0] = '\0';
    *owner_id_ptr = NULL_ID;

    if ((tr_rec_ptr = group_id_map[group_id]) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    *owner_id_ptr = tr_rec_ptr->owner_id;
    *group_type_ptr = tr_rec_ptr->group_type;
    (void)strcpy(group_name, tr_rec_ptr->group_name);
    (void)strcpy(full_name, tr_rec_ptr->full_name);

    return AS_SUCCESS;
}


/*
 * cas_translate_group_name
 *	translates a group name into a group id.
 *
 * Parameters:
 *	server_port	: ignored.
 *	group_name	: the name to be translated.
 *
 * Results:
 *	owner_id	: the owner of the group.
 *	group_id	: the ID of the group.
 *	group_type	: the type of the group (primary or secondary).
 *	full_name	: the full name of the group.
 *
 * Design:
 *	Looks up the group and returns the appropriate values.
 *
 */
/*ARGSUSED*/
cas_translate_group_name(server_port, group_name, owner_id_ptr, group_id_ptr, group_type_ptr, full_name)
mach_port_t		server_port;
group_name_t	group_name;
group_id_t	*owner_id_ptr;
group_id_t	*group_id_ptr;
group_type_t	*group_type_ptr;
full_name_t	full_name;
{
    group_record_ptr_t	tr_rec_ptr;

    to_lower(group_name);
    full_name[0] = '\0';
    *group_id_ptr = NULL_ID;
    *owner_id_ptr = NULL_ID;

    if ((tr_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    *owner_id_ptr = tr_rec_ptr->owner_id;
    *group_type_ptr = tr_rec_ptr->group_type;
    *group_id_ptr = tr_rec_ptr->group_id;
    (void)strcpy(full_name, tr_rec_ptr->full_name);

    return AS_SUCCESS;
}



/*
 * cas_list_members
 *	list all the members of the group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	group_name	: the group whose members are to be listed.
 *	trans_closure	: whether to compute the transitive closure of the members.
 *
 * Results:
 *	group_ids	: the groups that are members.
 *	group_ids_count	: the number of such groups.
 *
 * Design:
 *	If trans_closure then call do_transitive_closure for the members of group_name
 *	else just copy the members into new storage.
 *
 * Note:
 *	the transitive closure should include this group so we remove it before returning.
 *
 */
/*ARGSUSED*/
cas_list_members(server_port, group_name, trans_closure, group_ids_ptr, group_ids_count_ptr)
mach_port_t		server_port;
group_name_t	group_name;
boolean_t	trans_closure;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    group_record_ptr_t	list_rec_ptr;
    kern_return_t	kr;
    vm_address_t	data_ptr;
    int			i;

    to_lower(group_name);
    if ((list_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    if (trans_closure) {
	do_transitive_closure(list_rec_ptr, FALSE, group_ids_ptr, (int *)group_ids_count_ptr);
    }
    else {
	vm_size_t	size = list_rec_ptr->members_count * sizeof(group_id_t);
	if ((kr = vm_allocate(mach_task_self(), &data_ptr, size, TRUE)) != KERN_SUCCESS) {
	    fprintf(stderr, "cas_list_members.vm_allocate fails, kr = %d.\n", kr);
	    return AS_FAILURE;
	}
	*group_ids_ptr = (group_id_list_t)data_ptr;
	*group_ids_count_ptr = list_rec_ptr->members_count;
	for (i = 0; i < list_rec_ptr->members_count; i++) (*group_ids_ptr)[i] = list_rec_ptr->members[i];
    }

    return AS_SUCCESS;
}



/*
 * cas_list_memberships
 *	list all the groups of which a group is a member.
 *
 * Parameters:
 *	server_port	: ignored.
 *	group_name	: the group whose memberships are to be listed.
 *	trans_closure	: whether to compute the transitive closure of the memberships.
 *
 * Results:
 *	group_ids	: the groups of which the group_name is a member.
 *	group_ids_count	: the number of such groups.
 *
 * Design:
 *	If trans_closure then call do_transitive_closure for the memberships of group_name
 *	else just copy the members into new storage.
 *
 * Note:
 *	the transitive closure should include this group so we remove it before returning.
 *
 */
/*ARGSUSED*/
cas_list_memberships(server_port, group_name, trans_closure, group_ids_ptr, group_ids_count_ptr)
mach_port_t		server_port;
group_name_t	group_name;
boolean_t	trans_closure;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    group_record_ptr_t	list_rec_ptr;
    kern_return_t	kr;
    vm_address_t	data_ptr;
    int			i;

    to_lower(group_name);
    if ((list_rec_ptr = lookup_name(group_name)) == GROUP_RECORD_NULL) return AS_BAD_GROUP;

    if (trans_closure) {
	do_transitive_closure(list_rec_ptr, TRUE, group_ids_ptr, (int *)group_ids_count_ptr);
    }
    else {
	vm_size_t	size = list_rec_ptr->memberships_count * sizeof(group_id_t);
	if ((kr = vm_allocate(mach_task_self(), &data_ptr, size, TRUE)) != KERN_SUCCESS) {
	    fprintf(stderr, "cas_list_memberships.vm_allocate fails, kr = %d.\n", kr);
	    return AS_FAILURE;
	}
	*group_ids_ptr = (group_id_list_t)data_ptr;
	*group_ids_count_ptr = list_rec_ptr->memberships_count;
	for (i = 0; i < list_rec_ptr->memberships_count; i++)
	    (*group_ids_ptr)[i] = list_rec_ptr->memberships[i];
    }

    return AS_SUCCESS;
}



/*
 * cas_list_all_groups
 *	list all the groups known to the authentication service.
 *
 * Parameters:
 *	server_port	: ignored.
 *
 * Results:
 *	group_ids	: the groups known about.
 *	group_ids_count	: the number of such groups.
 *
 * Design:
 *	Use the group_id_map to count how many groups are in existence.
 *	Create enough new storage to hold that number of groups.
 *	Copy all the group ids into the new storage and return it.
 *
 */
/*ARGSUSED*/
cas_list_all_groups(server_port, group_ids_ptr, group_ids_count_ptr)
mach_port_t		server_port;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    kern_return_t	kr;
    vm_address_t	data_ptr;
    int			group_count, i;
    vm_size_t		size;

    group_count = 0;
    for (i = 0; i < GROUP_ID_MAX; i++) if (group_id_map[i] != GROUP_RECORD_NULL) group_count++;

    size = group_count * sizeof(group_id_t);
    if ((kr = vm_allocate(mach_task_self(), &data_ptr, size, TRUE)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_list_all_groups.vm_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    *group_ids_count_ptr = group_count;
    *group_ids_ptr = (group_id_list_t)data_ptr;
    group_count = 0;
    for (i = 0; i < GROUP_ID_MAX; i++) {
	if (group_id_map[i] != GROUP_RECORD_NULL) {
	    (*group_ids_ptr)[group_count] = i;
	    group_count ++;
	}
    }

    return AS_SUCCESS;
}



/*
 * cas_handle_port_death
 *	does what is necessary if we receive a port death message.
 *
 * Parameters:
 *	dead_port	: the port in question.
 *
 * Results:
 *	none
 *
 * Design:
 *	determine whether this port is a private port or a registered port
 *	by looking at the priv_port_map and the reg_port_map.
 *	If the dead port is a private port then call cas_logout.
 *	If the dead port is a registered port then call cas_delete_token.
 *
 */
void cas_handle_port_death(dead_port)
mach_port_t		dead_port;
{
    kern_return_t	kr;
    int			pport_index, rport_index;

#if	CAS_DEBUG
    printf("cas_handle_port_death: dead port = %x.\n", dead_port);
#endif	CAS_DEBUG
    HASH_PORT(dead_port, priv_port_map, pport_index);
    HASH_PORT(dead_port, reg_port_map, rport_index);
    if (priv_port_map[pport_index].group_record_ptr != GROUP_RECORD_NULL) {
	if ((kr = cas_logout(MACH_PORT_NULL, dead_port)) != AS_SUCCESS) {
	    fprintf(stderr, "cas_handle_port_death.cas_logout fails, kr = %d, port = %x.\n", kr, dead_port);
	}
    }
    else if (reg_port_map[rport_index].mapped_port == dead_port) {
	if ((kr = cas_delete_token(MACH_PORT_NULL, reg_port_map[rport_index].private_port,
					dead_port)) != AS_SUCCESS)
	{
	    fprintf(stderr, "cas_handle_port_death.cas_delete_token fails, kr = %d, port = %x.\n",
				kr, dead_port);
	}
    }
    else {
#if	CAS_DEBUG
	printf("cas_handle_port_death: %x is neither a private nor a registered port.\n",dead_port);
#endif	CAS_DEBUG
    }
}
