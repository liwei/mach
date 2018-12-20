/*
 * clas_procs.c
 *	Routines to map local authentication requests into central authentication requests.
 *	Used by the CAS to handle such requests.
 */

/*
 * HISTORY:
 * $Log:	clas_procs.c,v $
 * Revision 1.4  92/03/05  15:11:29  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:17:39  jms]
 * 
 * Revision 1.3  90/12/19  11:06:34  jjc
 * 	Changed as_create_token() to call cas_register_token() with
 * 	an extra argument saying not to create nonexistent groups.
 * 	Created as_create_token_group() which is like as_create_token()
 * 	except that it will create any nonexistent groups that it
 * 	finds in the group list.
 * 	[90/12/05            jjc]
 * 	Added as_login_create() to login a user and create the user
 * 	if the user does not exist yet.
 * 	[90/10/09            jjc]
 * 
 * Revision 1.2  89/05/18  09:42:17  dorr
 * 	add as_login_priv()
 * 	[89/05/15  12:25:19  dorr]
 * 
 * 21-Oct-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added as_change_owner.  Added owner_id to translate routines.
 *
 *  2-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#include <mach.h>
#include <stdio.h>

#include "auth_defs.h"


/*
 * as_login
 *	login a user with the central authentication server.
 *
 * Parameters:
 *	server_port	: ignored.
 *	user_name	: the user trying to log in.
 *	pass_word	: the clear-text password for the user.
 *
 * Returns:
 *	private_port	: a private, registered port for the user to use
 *				for all further communication with the AS.
 */
/*ARGSUSED*/
as_login(server_port, user_name, pass_word, private_port_ptr)
mach_port_t		server_port;
group_name_t	user_name;
pass_word_t	pass_word;
mach_port_t		*private_port_ptr;
{
    kern_return_t	kr;
    mach_port_t		private_port;

    *private_port_ptr = MACH_PORT_NULL;

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			&private_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_login(MACH_PORT_NULL, user_name, pass_word, private_port)) == AS_SUCCESS) {
	*private_port_ptr = private_port;
    }

    return kr;

}
/*
 * as_login
 *	login a user with the central authentication server.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: current user id (must be a member of sys_adm)
 *	user_id		: the user to log in as.
 *
 * Returns:
 *	new_port	: a private, registered port for the user to use
 *				for all further communication with the AS.
 */
/*ARGSUSED*/
as_login_priv(server_port, private_port, user_id, new_port_ptr)
mach_port_t		server_port;
mach_port_t		private_port;
group_id_t	user_id;
mach_port_t		*new_port_ptr;
{
    kern_return_t	kr;
    mach_port_t		new_port;

    *new_port_ptr = MACH_PORT_NULL;

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
		&new_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login_priv.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_login_priv(MACH_PORT_NULL, private_port, user_id, new_port)) == AS_SUCCESS) {
	*new_port_ptr = new_port;
    }

    return kr;

}
/*
 * as_login_create
 *	login a user with the central authentication server.
 *	create user if he/she does not exist.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: current user id (must be a member of sys_adm)
 *	id		: the user and group(s) to log in as.
 *	id_count	: how many IDs.
 *	new_port_ptr	: new private port
 *
 * Returns:
 *	new_port_ptr	: a private, registered port for the user to use
 *				for all further communication with the AS.
 */
/*ARGSUSED*/
as_login_create(server_port, private_port, id, id_count, new_port_ptr)
mach_port_t		server_port;
mach_port_t		private_port;
group_id_list_t	id;
unsigned int	id_count;
mach_port_t		*new_port_ptr;
{
    kern_return_t	kr;
    mach_port_t		new_port;

    *new_port_ptr = MACH_PORT_NULL;

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			     &new_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login_priv.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_login_create(MACH_PORT_NULL, private_port, id, id_count, 
				new_port)) == AS_SUCCESS) {
	*new_port_ptr = new_port;
    }

    return kr;

}


/*
 * as_logout
 *	Logs out a user.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing user who wants to be logged out.
 *
 */
/*ARGSUSED*/
as_logout(server_port, private_port)
mach_port_t		server_port;
mach_port_t		private_port;
{
    return cas_logout(MACH_PORT_NULL, private_port);
}



/*
 * as_create_token
 *	creates a new token which can be later used to verify the user.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing user to authentication service.
 *	group_ids	: list of groups to which the token is to be restricted
*				(empty implies all groups of which user is a member).
 *	group_ids_count	: number of group ids in the list.
 *
 * Results:
 *	token_ptr	: pointer to a new token that the user can pass around.
 *
 */
/*ARGSUSED*/
as_create_token(server_port, private_port, group_ids, group_ids_count, token_ptr)
mach_port_t		server_port;
mach_port_t		private_port;
group_id_list_t	group_ids;
unsigned int	group_ids_count;
mach_port_t		*token_ptr;
{
    kern_return_t	kr;

    *token_ptr = MACH_PORT_NULL;

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			     token_ptr)) != KERN_SUCCESS) {
	fprintf(stderr, "as_create_token.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    return cas_register_token(MACH_PORT_NULL, private_port, group_ids,
				(unsigned int)group_ids_count, *token_ptr, 
				FALSE);

}

/*
 * as_create_token_group
 *	creates a new token which can be later used to verify the user.
 *	also, create any nonexistent groups in the group list given.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing user to authentication service.
 *	group_ids	: list of groups to which the token is to be restricted
*				(empty implies all groups of which user is a member).
 *	group_ids_count	: number of group ids in the list.
 *
 * Results:
 *	token_ptr	: pointer to a new token that the user can pass around.
 *
 */
/*ARGSUSED*/
as_create_token_group(server_port, private_port, group_ids, group_ids_count, token_ptr)
mach_port_t		server_port;
mach_port_t		private_port;
group_id_list_t	group_ids;
unsigned int	group_ids_count;
mach_port_t		*token_ptr;
{
    kern_return_t	kr;

    *token_ptr = MACH_PORT_NULL;

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			     token_ptr)) != KERN_SUCCESS) {
	fprintf(stderr, "as_create_token_group.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    return cas_register_token(MACH_PORT_NULL, private_port, group_ids,
				(unsigned int)group_ids_count, *token_ptr, TRUE);

}


/*
 * as_delete_token
 *	deletes a token priviously created for this user.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: the port that represents the user.
 *	token		: the token to be deleted.
 *
 */
/*ARGSUSED*/
as_delete_token(server_port, private_port, token)
mach_port_t		server_port;
mach_port_t		private_port;
mach_port_t		token;
{
    return cas_delete_token(MACH_PORT_NULL, private_port, token);
}



/*
 * as_verify_token_ids
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
 */
/*ARGSUSED*/
as_verify_token_ids(server_port, token, user_id_ptr, group_ids_ptr, group_ids_count_ptr)
mach_port_t		server_port;
mach_port_t		token;
group_id_t	*user_id_ptr;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    return cas_verify_token_ids(MACH_PORT_NULL, token, user_id_ptr, group_ids_ptr,
				(unsigned int *)group_ids_count_ptr);
}


/*
 * as_verify_token_names
 *	returns the group names associated with a token.
 *
 * Parameters:
 *	server_port	: ignored.
 *	token		: the token to be verified.
 *
 * Results:
 *	user_name		: the real user represented by this token.
 *	group_names		: the group names that are associated with this token.
 *	group_names_count	: the number of such groups.
 *
 * Note:
 *	BUG - the list of names created by cas_verify_token_names
 *	will not be deallocated when this call returns.
 *
 */
/*ARGSUSED*/
as_verify_token_names(server_port, token, user_name, group_names_ptr, group_names_count_ptr)
mach_port_t			server_port;
mach_port_t			token;
group_name_t		user_name;
group_name_list_t	*group_names_ptr;
unsigned int		*group_names_count_ptr;
{
    return cas_verify_token_names(MACH_PORT_NULL, token, user_name, group_names_ptr,
					(unsigned int *)group_names_count_ptr);
}



/*
 * as_create_group
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
 */
/*ARGSUSED*/
as_create_group(server_port, private_port, group_name, group_type, full_name, pass_word,
				owner_group, group_id_ptr)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_type_t	group_type;
full_name_t	full_name;
pass_word_t	pass_word;
group_name_t	owner_group;
group_id_t	*group_id_ptr;
{
    return cas_create_group(MACH_PORT_NULL, private_port, group_name, group_type,
				full_name, pass_word, owner_group, group_id_ptr);
}


/*
 * as_delete_group
 *	deletes a group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_name	: the name of the group to be deleted.
 *
 */
/*ARGSUSED*/
as_delete_group(server_port, private_port, group_name)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
{
    return cas_delete_group(MACH_PORT_NULL, private_port, group_name);
}


/*
 * as_change_password
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: the port representing the user.
 *	group_name	: the name of the group whose password is to be changed.
 *	old_pass_word	: the old pass_word for the group.
 *	new_pass_word	: the old pass_word for the group.
 *
 */
/*ARGSUSED*/
as_change_password(server_port, private_port, group_name, old_pass_word, new_pass_word)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
pass_word_t	old_pass_word;
pass_word_t	new_pass_word;
{
    return cas_change_password(MACH_PORT_NULL, private_port,group_name, old_pass_word, new_pass_word);
}


/*
 * as_change_owner
 *	changes the owner of a group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_name	: the name of the group to be changed.
 *	new_owner	: the name of the new owner.
 *
 */
/*ARGSUSED*/
as_change_owner(server_port, private_port, group_name, new_owner)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_name_t	new_owner;
{
    return cas_change_owner(MACH_PORT_NULL, private_port, group_name, new_owner);
}



/*
 * as_add_to_group
 *	add a group as a member of a secondary group.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: port representing the user.
 *	group_name	: the group to be modified.
 *	add_group_name	: the group to be added.
 *
 */
/*ARGSUSED*/
as_add_to_group(server_port, private_port, group_name, add_group_name)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_name_t	add_group_name;
{
    return cas_add_to_group(MACH_PORT_NULL, private_port, group_name, add_group_name);
}


/*
 * as_remove_from_group
 *	remove a group as a member from a secondary group.
 *
 * Parameters:
 *	server_port		: ignored.
 *	private_port		: port representing the user.
 *	group_name		: the group to be modified.
 *	remove_group_name	: the group to be removed.
 *
 */
/*ARGSUSED*/
as_remove_from_group(server_port, private_port, group_name, remove_group_name)
mach_port_t		server_port;
mach_port_t		private_port;
group_name_t	group_name;
group_name_t	remove_group_name;
{
    return cas_remove_from_group(MACH_PORT_NULL, private_port, group_name, remove_group_name);
}



/*
 * as_translate_group_id
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
 */
/*ARGSUSED*/
as_translate_group_id(server_port, group_id, owner_id_ptr, group_name, group_type_ptr, full_name)
mach_port_t		server_port;
group_id_t	group_id;
group_id_t	*owner_id_ptr;
group_name_t	group_name;
group_type_t	*group_type_ptr;
full_name_t	full_name;
{
    return cas_translate_group_id(MACH_PORT_NULL, group_id, owner_id_ptr, group_name, group_type_ptr, full_name);
}


/*
 * as_translate_group_name
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
 */
/*ARGSUSED*/
as_translate_group_name(server_port, group_name, owner_id_ptr, group_id_ptr, group_type_ptr, full_name)
mach_port_t		server_port;
group_name_t	group_name;
group_id_t	*owner_id_ptr;
group_id_t	*group_id_ptr;
group_type_t	*group_type_ptr;
full_name_t	full_name;
{
    return cas_translate_group_name(MACH_PORT_NULL, group_name, owner_id_ptr,
				group_id_ptr, group_type_ptr, full_name);
}



/*
 * as_list_members
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
 */
/*ARGSUSED*/
as_list_members(server_port, group_name, trans_closure, group_ids_ptr, group_ids_count_ptr)
mach_port_t		server_port;
group_name_t	group_name;
boolean_t	trans_closure;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    return cas_list_members(MACH_PORT_NULL, group_name, trans_closure, group_ids_ptr,
				(unsigned int *)group_ids_count_ptr);
}


/*
 * as_list_memberships
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
 */
/*ARGSUSED*/
as_list_memberships(server_port, group_name, trans_closure, group_ids_ptr, group_ids_count_ptr)
mach_port_t		server_port;
group_name_t	group_name;
boolean_t	trans_closure;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    return cas_list_memberships(MACH_PORT_NULL, group_name, trans_closure, group_ids_ptr,
					(unsigned int *)group_ids_count_ptr);
}


/*
 * as_list_all_groups
 *	list all the groups known to the authentication service.
 *
 * Parameters:
 *	server_port	: ignored.
 *
 * Results:
 *	group_ids	: the groups known about.
 *	group_ids_count	: the number of such groups.
 *
 */
/*ARGSUSED*/
as_list_all_groups(server_port, group_ids_ptr, group_ids_count_ptr)
mach_port_t	server_port;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    return cas_list_all_groups(MACH_PORT_NULL, group_ids_ptr, (unsigned int *)group_ids_count_ptr);
}
