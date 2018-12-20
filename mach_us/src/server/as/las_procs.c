/*
 * las_procs.c
 *
 *	Main routines for the local authentication server:
 *		pass requests to the central authentication server,
 *		map private ports,
 *		maybe cache verification information, and
 *		create login codes.
 *
 *	Before these routines are called, there should already be a secure
 *	connection between the local host and the central host.
 *
 */

/*
 * HISTORY:
 * $Log:	las_procs.c,v $
 * Revision 1.3  90/12/19  11:06:41  jjc
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
 * Revision 1.2  89/05/18  09:43:27  dorr
 * 	add as_login_priv()
 * 	[89/05/15  12:25:53  dorr]
 * 
 * 21-Oct-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added as_change_owner.  Added owner_id to translate routines.
 *
 *  2-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Improved port hashing.  Added local constant PORT_MAP_MAX.
 *
 *  9-Apr-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Replaced as_verify_token by as_verify_token_ids and as_verify_token_names.
 *	Added user_name and group_names to the cache of verification information.
 *
 * 23-Feb-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#include <mach.h>
#include <stdio.h>
#include <strings.h>
#include <mach/mach_param.h>
#include <mach/message.h>

#include "auth_defs.h"

extern port_t cas_port;


/*
 * Arrays used to:
 *	map local to central private ports, so that we can logout a user
 *		when we get a no-senders notification on the local private port;
 *	map registered to private ports, so that all registered ports associated with a login
 *		can be removed when the user logs out; and
 *	map registered ports to cached verification information,
 *		so that some verification requests can be handled locally.
 *
 */
#define PORT_MAP_MAX	256	/* Must be a power of 2 */

static struct {
    port_t		mapped_port, central_pport;
} priv_port_map[PORT_MAP_MAX];

static struct {
    port_t		mapped_port, local_pport;
    group_id_t		user_id;
    group_id_list_t	group_ids;
    int			group_ids_count;
    group_name_t	user_name;
    group_name_list_t	group_names;
    int			group_names_count;
} reg_port_map[PORT_MAP_MAX];

#define HASH_PORT(port, map, index) {						\
    (index) = ((((port) >> 8) & 0xff) ^ ((port) & 0xff)) & (PORT_MAP_MAX - 1);	\
    while (((map)[(index)].mapped_port != (port))				\
	&& ((map)[(index)].mapped_port != PORT_NULL))				\
    (index) = ++(index) % PORT_MAP_MAX;						\
}



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
 *
 * Side effects:
 *	Make a mapping entry between the local and central private port.
 *
 * Design:
 *	Create a central private port for the user.
 *	Call cas_login to actually log the user in.
 *	If the login is successful, create a new local private port for the user.
 *
 * Note:
 *	the local and central private ports are distinct so that we can retain receive
 *	rights for the central port whilst passing back receive rights to the user for
 *	the local port.  In this way we can tell when the user dies before logging out
 *	without having the central AS have to deal with the same port death.
 *
 *	Alternatively we should rely on receiving a no-senders notification on the
 *	local private port for detecting when the user has implicitly logged out.
 *
 *	Basically we want to be able to call logout explicitly on the central server
 *	in the normal case - however if we die then the central server will have to
 *	handle the port death of the central private port.
 *
 */
/*ARGSUSED*/
as_login(server_port, user_name, pass_word, private_port_ptr)
port_t		server_port;
group_name_t	user_name;
pass_word_t	pass_word;
port_t		*private_port_ptr;
{
    kern_return_t	kr;
    port_t		central_priv_port, local_priv_port;
    int			port_index;

    *private_port_ptr = PORT_NULL;

    if ((kr = port_allocate(task_self(), &central_priv_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_login(cas_port, user_name, pass_word, central_priv_port)) != AS_SUCCESS) {
	fprintf(stderr, "as_login.cas_login fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    if ((kr = port_allocate(task_self(), &local_priv_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    HASH_PORT(local_priv_port, priv_port_map, port_index);
    priv_port_map[port_index].mapped_port = local_priv_port;
    priv_port_map[port_index].central_pport = central_priv_port;
    *private_port_ptr = local_priv_port;

    return AS_SUCCESS;

}
/*
 * as_login_priv
 *	login a user with the central authentication server.
 *	this call can be issued by a member of the sys_adm group
 *	and requires no password.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: identity of the caller (must be sys_adm member)
 *	user_id		: the user trying to log in.
 *
 * Returns:
 *	new_port	: a private, registered port for the user to use
 *				for all further communication with the AS.
 *
 * Side effects:
 *	Make a mapping entry between the local and central private port.
 *
 * Design:
 *	Create a central private port for the user.
 *	Call cas_login_priv to actually log the user in.
 *	If the login is successful, create a new local private port for the user.
 */
/*ARGSUSED*/
as_login_priv(server_port, private_port, user_id, new_port_ptr)
port_t		server_port;
port_t		private_port;
group_name_t	user_id;
port_t		*new_port_ptr;
{
    kern_return_t	kr;
    port_t		central_priv_port, local_priv_port;
    int			port_index;

    *new_port_ptr = PORT_NULL;

    if ((kr = port_allocate(task_self(), &central_priv_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_login_priv(cas_port, private_port, user_id, central_priv_port)) != AS_SUCCESS) {
	fprintf(stderr, "as_login.cas_login fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    if ((kr = port_allocate(task_self(), &local_priv_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    HASH_PORT(local_priv_port, priv_port_map, port_index);
    priv_port_map[port_index].mapped_port = local_priv_port;
    priv_port_map[port_index].central_pport = central_priv_port;
    *new_port_ptr = local_priv_port;

    return AS_SUCCESS;

}

/*
 * as_login_create
 *	login a user with the central authentication server.
 *	this call can be issued by a member of the sys_adm group
 *	and requires no password.
 *	create user if he/she does not exist.
 *
 * Parameters:
 *	server_port	: ignored.
 *	private_port	: identity of the caller (must be sys_adm member)
 *	id		: the user and group(s) to log in as.
 *	id_count	: how many IDs.
 *
 * Returns:
 *	new_port	: a private, registered port for the user to use
 *				for all further communication with the AS.
 *
 * Side effects:
 *	Make a mapping entry between the local and central private port.
 *
 * Design:
 *	Create a central private port for the user.
 *	Call cas_login_create to actually log the user in.
 *	If the login is successful, create a new local private port for the user.
 */
/*ARGSUSED*/
as_login_create(server_port, private_port, id, id_count, new_port_ptr)
port_t		server_port;
port_t		private_port;
group_id_list_t	id;
unsigned int	id_count;
port_t		*new_port_ptr;
{
    kern_return_t	kr;
    port_t		central_priv_port, local_priv_port;
    int			port_index;

    *new_port_ptr = PORT_NULL;

    if ((kr = port_allocate(task_self(), &central_priv_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_login_create(cas_port, private_port, id, id_count, 
				central_priv_port)) != AS_SUCCESS) {
	fprintf(stderr, "as_login_create.cas_login_create fails, kr = %d.\n",
		kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    if ((kr = port_allocate(task_self(), &local_priv_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_login.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    HASH_PORT(local_priv_port, priv_port_map, port_index);
    priv_port_map[port_index].mapped_port = local_priv_port;
    priv_port_map[port_index].central_pport = central_priv_port;
    *new_port_ptr = local_priv_port;

    return AS_SUCCESS;

}


/*
 * as_logout
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
 *	Deallocate the local private port.
 *	Call cas_logout on the central private port.
 *	Deallocate the central private port.
 *	Deallocate all the registered ports and clean up any cached information.
 *
 * Note:
 *	the call to cas_logout will deregister all the registered ports within the CAS.
 *	i.e. We do not have to make repeated calls to cas_deregister to do this.
 *
 */
/*ARGSUSED*/
as_logout(server_port, private_port)
port_t		server_port;
port_t		private_port;
{
    port_t		central_priv_port;
    kern_return_t	kr;
    int			port_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if ((central_priv_port = priv_port_map[port_index].central_pport) == PORT_NULL)
	return AS_BAD_PRIVATE_PORT;

    priv_port_map[port_index].mapped_port = PORT_NULL;
    priv_port_map[port_index].central_pport = PORT_NULL;
#if	0
    if ((kr = port_deallocate(task_self(), private_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_logout.port_deallocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }
#else
    /*
     * No matter if this fails - we only have send rights to the private port anyway.
     */
    (void)port_deallocate(task_self(), private_port);
#endif
   
    if ((kr = cas_logout(cas_port, central_priv_port)) != AS_SUCCESS) {
	fprintf(stderr, "as_logout.cas_logout fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    if ((kr = port_deallocate(task_self(), central_priv_port)) != KERN_SUCCESS) {
	fprintf(stderr, "as_logout.port_deallocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    /*
     * Check through the registered port map to see if any are associated with this user.
     */
    for (port_index = 0; port_index < PORT_MAP_MAX; port_index ++) {
	if (reg_port_map[port_index].local_pport == private_port) {
	    /*
	     * This registered port should be deallocated.
	     */
	    if ((kr = port_deallocate(task_self(), reg_port_map[port_index].mapped_port)) != KERN_SUCCESS) {
		fprintf(stderr, "as_logout.port_deallocate fails, kr = %d.\n", kr);
	    }
	    reg_port_map[port_index].mapped_port = reg_port_map[port_index].local_pport = PORT_NULL;

	    /*
	     * Deallocate any cached verification information.
	     */
	    if (reg_port_map[port_index].group_ids_count != 0) {
		vm_size_t	size = reg_port_map[port_index].group_ids_count * sizeof(group_id_t);
		if ((kr = vm_deallocate(task_self(),
					(vm_address_t)reg_port_map[port_index].group_ids, size))
			!= KERN_SUCCESS)
		{
		    fprintf(stderr, "as_logout.vm_deallocate fails, kr = %d.\n", kr);
		}
		reg_port_map[port_index].group_ids_count = 0;
		reg_port_map[port_index].group_ids = (group_id_list_t)0;
	    }
	    if (reg_port_map[port_index].group_names_count != 0) {
		vm_size_t	size = reg_port_map[port_index].group_names_count * sizeof(group_name_t);
		if ((kr = vm_deallocate(task_self(),
					(vm_address_t)reg_port_map[port_index].group_names, size))
			!= KERN_SUCCESS)
		{
		    fprintf(stderr, "as_logout.vm_deallocate fails, kr = %d.\n", kr);
		}
		reg_port_map[port_index].group_names_count = 0;
		reg_port_map[port_index].group_names = (group_name_list_t)0;
	    }
	}
    }

    return AS_SUCCESS;
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
 * Design:
 *	Check and map the private port.
 *	Allocate a new token.
 *	Call cas_register_token.
 *	Remember the association between the private port and the new token.
 *
 */
/*ARGSUSED*/
as_create_token(server_port, private_port, group_ids, group_ids_count, token_ptr)
port_t		server_port;
port_t		private_port;
group_id_list_t	group_ids;
unsigned int	group_ids_count;
port_t		*token_ptr;
{
    kern_return_t	kr;
    int			port_index;

    *token_ptr = PORT_NULL;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = port_allocate(task_self(), token_ptr)) != KERN_SUCCESS) {
	fprintf(stderr, "as_create_token.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_register_token(cas_port, priv_port_map[port_index].central_pport,
				group_ids, (unsigned int)group_ids_count, *token_ptr, FALSE)) != AS_SUCCESS)
    {
	fprintf(stderr, "as_create_token.cas_register_token fails, kr = %d.\n", kr);
	(void)port_deallocate(task_self(), *token_ptr);
	*token_ptr = PORT_NULL;
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }
/*
    HASH_PORT(*token_ptr, reg_port_map, port_index);
    reg_port_map[port_index].mapped_port = *token_ptr;
    reg_port_map[port_index].local_pport = private_port;
*/
    return AS_SUCCESS;

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
 * Design:
 *	Check and map the private port.
 *	Allocate a new token.
 *	Call cas_register_token.
 *	Remember the association between the private port and the new token.
 *
 */
/*ARGSUSED*/
as_create_token_group(server_port, private_port, group_ids, group_ids_count, token_ptr)
port_t		server_port;
port_t		private_port;
group_id_list_t	group_ids;
unsigned int	group_ids_count;
port_t		*token_ptr;
{
    kern_return_t	kr;
    int			port_index;

    *token_ptr = PORT_NULL;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = port_allocate(task_self(), token_ptr)) != KERN_SUCCESS) {
	fprintf(stderr, "as_create_token_group.port_allocate fails, kr = %d.\n", kr);
	return AS_FAILURE;
    }

    if ((kr = cas_register_token(cas_port, priv_port_map[port_index].central_pport,
				group_ids, (unsigned int)group_ids_count, *token_ptr, TRUE)) != AS_SUCCESS)
    {
	fprintf(stderr, "as_create_token_group.cas_register_token fails, kr = %d.\n", kr);
	(void)port_deallocate(task_self(), *token_ptr);
	*token_ptr = PORT_NULL;
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }
/*
    HASH_PORT(*token_ptr, reg_port_map, port_index);
    reg_port_map[port_index].mapped_port = *token_ptr;
    reg_port_map[port_index].local_pport = private_port;
*/
    return AS_SUCCESS;

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
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Remove the entries for this token from the reg_port_map.
 *	Call cas_delete_token.
 *	Deallocate the token locally.
 *
 */
/*ARGSUSED*/
as_delete_token(server_port, private_port, token)
port_t		server_port;
port_t		private_port;
port_t		token;
{
    kern_return_t	kr;
    int			pport_index, rport_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, pport_index);
    if (priv_port_map[pport_index].central_pport == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(token, reg_port_map, rport_index);
    if (reg_port_map[rport_index].local_pport != private_port) {
	fprintf(stderr, "as_delete_token: private_port does not match private_port for token.\n");
	return AS_BAD_PRIVATE_PORT;
    }	

    reg_port_map[rport_index].mapped_port = reg_port_map[rport_index].local_pport = PORT_NULL;

    /*
     * Deallocate any cached verification information.
     */
    if (reg_port_map[rport_index].group_ids_count != 0) {
	vm_size_t	size = reg_port_map[rport_index].group_ids_count * sizeof(group_id_t);
	if ((kr = vm_deallocate(task_self(), (vm_address_t)reg_port_map[rport_index].group_ids, size))
	    != KERN_SUCCESS)
	{
	    fprintf(stderr, "as_delete_token.vm_deallocate fails, kr = %d.\n", kr);
	}
	reg_port_map[rport_index].group_ids_count = 0;
	reg_port_map[rport_index].group_ids = (group_id_list_t)0;
    }
    if (reg_port_map[rport_index].group_names_count != 0) {
	vm_size_t	size = reg_port_map[rport_index].group_names_count * sizeof(group_name_t);
	if ((kr = vm_deallocate(task_self(), (vm_address_t)reg_port_map[rport_index].group_names, size))
	    != KERN_SUCCESS)
	{
	    fprintf(stderr, "as_delete_token.vm_deallocate fails, kr = %d.\n", kr);
	}
	reg_port_map[rport_index].group_names_count = 0;
	reg_port_map[rport_index].group_names = (group_name_list_t)0;
    }

    if ((kr = cas_delete_token(cas_port, priv_port_map[pport_index].central_pport, token))
	!= AS_SUCCESS)
    {
	fprintf(stderr, "as_delete_token.cas_delete_token fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) kr = AS_TIMEOUT;
    }

    if ((port_deallocate(task_self(), token)) != KERN_SUCCESS) {
	fprintf(stderr, "as_delete_token.port_deallocate fails, port = %d.\n", token);
    }

    return kr;
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
 * Side effects:
 *	Will cache verification information obtained from the central server.
 *
 * Design:
 *	If there is verification information in the cache, return it directly.
 *	Otherwise call cas_verify_token_ids.
 *
 * Note:
 *	the group ID list returned must not be deallocated by the mig interface.
 *
 */
/*ARGSUSED*/
as_verify_token_ids(server_port, token, user_id_ptr, group_ids_ptr, group_ids_count_ptr)
port_t		server_port;
port_t		token;
group_id_t	*user_id_ptr;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    kern_return_t	kr;
    int			port_index;

    *user_id_ptr = 0;
    *group_ids_count_ptr = 0;
    *group_ids_ptr = 0;

    HASH_PORT(token, reg_port_map, port_index);
    if (reg_port_map[port_index].mapped_port == token) {
	*group_ids_ptr = reg_port_map[port_index].group_ids;
	*group_ids_count_ptr = reg_port_map[port_index].group_ids_count;
	*user_id_ptr = reg_port_map[port_index].user_id;
	return AS_SUCCESS;
    }
    else {
	if ((kr = cas_verify_token_ids(cas_port, token, user_id_ptr, group_ids_ptr,
					(unsigned int *)group_ids_count_ptr)) != AS_SUCCESS)
	{
	    fprintf(stderr, "as_verify_token_ids.cas_verify_token_ids fails, kr = %d.\n", kr);
	    if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	    else return kr;
	}
	else {
	    reg_port_map[port_index].mapped_port = token;
	    reg_port_map[port_index].group_ids = *group_ids_ptr;
	    reg_port_map[port_index].group_ids_count = *group_ids_count_ptr;
	    reg_port_map[port_index].user_id = *user_id_ptr;
	}
    }

    return AS_SUCCESS;
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
 * Side effects:
 *	Will cache verification information obtained from the central server.
 *
 * Design:
 *	If there is verification information in the cache, return it directly.
 *	Otherwise call cas_verify_token_names.
 *
 * Note:
 *	the group name list returned must not be deallocated by the mig interface.
 *
 */
/*ARGSUSED*/
as_verify_token_names(server_port, token, user_name, group_names_ptr, group_names_count_ptr)
port_t			server_port;
port_t			token;
group_name_t		user_name;
group_name_list_t	*group_names_ptr;
unsigned int		*group_names_count_ptr;
{
    kern_return_t	kr;
    int			port_index;

    (void)strcpy(user_name, "");
    *group_names_count_ptr = 0;
    *group_names_ptr = 0;

    HASH_PORT(token, reg_port_map, port_index);
    if (reg_port_map[port_index].mapped_port == token) {
	*group_names_ptr = reg_port_map[port_index].group_names;
	*group_names_count_ptr = reg_port_map[port_index].group_names_count;
	(void)strcpy(user_name, reg_port_map[port_index].user_name);
	return AS_SUCCESS;
    }
    else {
	if ((kr = cas_verify_token_names(cas_port, token, user_name, group_names_ptr,
					(unsigned int *)group_names_count_ptr)) != AS_SUCCESS)
	{
	    fprintf(stderr, "as_verify_token_names.cas_verify_token_names fails, kr = %d.\n", kr);
	    if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	    else return kr;
	}
	else {
	    reg_port_map[port_index].mapped_port = token;
	    reg_port_map[port_index].group_names = *group_names_ptr;
	    reg_port_map[port_index].group_names_count = *group_names_count_ptr;
	    (void)strcpy(reg_port_map[port_index].user_name, user_name);
	}
    }

    return AS_SUCCESS;
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
 * Design:
 *	Check the private port.
 *	Call cas_create_group.
 *
 */
/*ARGSUSED*/
as_create_group(server_port, private_port, group_name, group_type, full_name, pass_word, owner_group, group_id_ptr)
port_t		server_port;
port_t		private_port;
group_name_t	group_name;
group_type_t	group_type;
full_name_t	full_name;
pass_word_t	pass_word;
group_name_t	owner_group;
group_id_t	*group_id_ptr;
{
    kern_return_t	kr;
    int			port_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = cas_create_group(cas_port, priv_port_map[port_index].central_pport, group_name, group_type,
				full_name, pass_word, owner_group, group_id_ptr)) != AS_SUCCESS)
    {
	fprintf(stderr, "as_create_group.cas_create_group fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Call cas_delete_group.
 *
 */
/*ARGSUSED*/
as_delete_group(server_port, private_port, group_name)
port_t		server_port;
port_t		private_port;
group_name_t	group_name;
{
    kern_return_t	kr;
    int			port_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = cas_delete_group(cas_port, priv_port_map[port_index].central_pport, group_name))
	!= AS_SUCCESS)
    {
	fprintf(stderr, "as_delete_group.cas_delete_group fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Call cas_change_password.
 *
 */
/*ARGSUSED*/
as_change_password(server_port, private_port, group_name, old_pass_word, new_pass_word)
port_t		server_port;
port_t		private_port;
group_name_t	group_name;
pass_word_t	old_pass_word;
pass_word_t	new_pass_word;
{
    kern_return_t	kr;
    int			port_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = cas_change_password(cas_port, priv_port_map[port_index].central_pport, group_name,
					old_pass_word, new_pass_word))
	    != AS_SUCCESS)
    {
	fprintf(stderr, "as_change_password.cas_change_password fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Call cas_change_owner.
 *
 */
/*ARGSUSED*/
as_change_owner(server_port, private_port, group_name, new_owner)
port_t		server_port;
port_t		private_port;
group_name_t	group_name;
group_name_t	new_owner;
{
    kern_return_t	kr;
    int			port_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = cas_change_owner(cas_port, priv_port_map[port_index].central_pport, group_name, new_owner))
	!= AS_SUCCESS)
    {
	fprintf(stderr, "as_change_owner.cas_change_owner fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Call cas_add_to_group.
 *
 */
/*ARGSUSED*/
as_add_to_group(server_port, private_port, group_name, add_group_name)
port_t		server_port;
port_t		private_port;
group_name_t	group_name;
group_name_t	add_group_name;
{
    kern_return_t	kr;
    int			port_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = cas_add_to_group(cas_port, priv_port_map[port_index].central_pport,
				group_name, add_group_name)) != AS_SUCCESS)
    {
	fprintf(stderr, "as_add_to_group.cas_add_to_group fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Results:
 *
 * Design:
 *	Check the private port.
 *	Call cas_remove_from_group.
 *
 */
/*ARGSUSED*/
as_remove_from_group(server_port, private_port, group_name, remove_group_name)
port_t		server_port;
port_t		private_port;
group_name_t	group_name;
group_name_t	remove_group_name;
{
    kern_return_t	kr;
    int			port_index;

    /*
     * Check the validity of the private port.
     */
    if (private_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;
    HASH_PORT(private_port, priv_port_map, port_index);
    if (priv_port_map[port_index].mapped_port == PORT_NULL) return AS_BAD_PRIVATE_PORT;

    if ((kr = cas_remove_from_group(cas_port, priv_port_map[port_index].central_pport,
					group_name, remove_group_name)) != AS_SUCCESS)
    {
	fprintf(stderr, "as_remove_from_group.cas_remove_from_group fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Design:
 *	Just call cas_translate_group_id.
 *
 */
/*ARGSUSED*/
as_translate_group_id(server_port, group_id, owner_id_ptr, group_name, group_type_ptr, full_name)
port_t		server_port;
group_id_t	group_id;
group_id_t	*owner_id_ptr;
group_name_t	group_name;
group_type_t	*group_type_ptr;
full_name_t	full_name;
{
    kern_return_t	kr;

    if ((kr = cas_translate_group_id(cas_port, group_id, owner_id_ptr, group_name,
				group_type_ptr, full_name))
	    != AS_SUCCESS)
    {
	fprintf(stderr, "as_translate_group_id.cas_translate_group_id fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Design:
 *	Just call cas_translate_group_name.
 *
 */
/*ARGSUSED*/
as_translate_group_name(server_port, group_name, owner_id_ptr, group_id_ptr, group_type_ptr, full_name)
port_t		server_port;
group_name_t	group_name;
group_id_t	*owner_id_ptr;
group_id_t	*group_id_ptr;
group_type_t	*group_type_ptr;
full_name_t	full_name;
{
    kern_return_t	kr;

    if ((kr = cas_translate_group_name(cas_port, group_name, owner_id_ptr, group_id_ptr,
				group_type_ptr, full_name))
	    != AS_SUCCESS)
    {
	fprintf(stderr, "as_translate_group_name.cas_translate_group_name fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Design:
 *	Just call cas_list_members.
 *
 */
/*ARGSUSED*/
as_list_members(server_port, group_name, trans_closure, group_ids_ptr, group_ids_count_ptr)
port_t		server_port;
group_name_t	group_name;
boolean_t	trans_closure;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    kern_return_t	kr;

    if ((kr = cas_list_members(cas_port, group_name, trans_closure, group_ids_ptr,
				(unsigned int *)group_ids_count_ptr)) != AS_SUCCESS)
    {
	fprintf(stderr, "as_list_members.cas_list_members fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Design:
 *	Just call cas_list_memberships.
 *
 */
/*ARGSUSED*/
as_list_memberships(server_port, group_name, trans_closure, group_ids_ptr, group_ids_count_ptr)
port_t		server_port;
group_name_t	group_name;
boolean_t	trans_closure;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    kern_return_t	kr;

    if ((kr = cas_list_memberships(cas_port, group_name, trans_closure, group_ids_ptr,
					(unsigned int *)group_ids_count_ptr)) != AS_SUCCESS)
    {
	fprintf(stderr, "as_list_memberships.cas_list_memberships fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
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
 * Design:
 *	Just call cas_list_all_groups.
 *
 */
/*ARGSUSED*/
as_list_all_groups(server_port, group_ids_ptr, group_ids_count_ptr)
port_t		server_port;
group_id_list_t	*group_ids_ptr;
unsigned int	*group_ids_count_ptr;
{
    kern_return_t	kr;

    if ((kr = cas_list_all_groups(cas_port, group_ids_ptr, (unsigned int *)group_ids_count_ptr))
	!= AS_SUCCESS)
    {
	fprintf(stderr, "as_list_all_groups.cas_list_all_groups fails, kr = %d.\n", kr);
	if (kr == RCV_TIMED_OUT) return AS_TIMEOUT;
	else return kr;
    }

    return AS_SUCCESS;
}



/*
 * las_handle_port_death
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
 *	If the dead port is a private port then call as_logout.
 *	If the dead port is a registered port then call as_delete_token.
 *
 */
void las_handle_port_death(dead_port)
port_t		dead_port;
{
    kern_return_t	kr;
    int			pport_index, rport_index;

    HASH_PORT(dead_port, priv_port_map, pport_index);
    HASH_PORT(dead_port, reg_port_map, rport_index);
    if (priv_port_map[pport_index].mapped_port == dead_port) {
	if ((kr = as_logout(PORT_NULL, dead_port)) != AS_SUCCESS) {
	    fprintf(stderr, "las_handle_port_death.as_logout fails, kr = %d, port = %x.\n", kr, dead_port);
	}
    }
    else if (reg_port_map[rport_index].mapped_port == dead_port) {
	if ((kr = as_delete_token(PORT_NULL, reg_port_map[rport_index].local_pport, dead_port))
	    != AS_SUCCESS)
	{
	    fprintf(stderr, "las_handle_port_death.as_delete_token fails, kr = %d, port = %x.\n",
				kr, dead_port);
	}
    }
    else {
	fprintf(stderr, "las_handle_port_death: %x is neither a private nor a registered port.\n",
				dead_port);
    }
}
