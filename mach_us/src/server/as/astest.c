/*
 * astest.c
 *
 *	Test program for the authentication service.
 *
 */

/*
 * HISTORY:
 * $Log:	astest.c,v $
 * Revision 1.3  92/03/05  15:11:04  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:09:56  jms]
 * 
 * 21-Oct-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added as_change_owner.  Added owner_id to translate routines.
 *	Added exit/quit command.  Use sm_login.  Added mig_get_token.
 *
 * 31-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Use secure Mach startup from sm_user_init.c.
 *
 * 13-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Use new secure Mach startup.
 *
 *  9-Jun-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Changes to conform with new mig interface to name server.
 *
 * 10-Apr-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added calls to name server.
 *
 * 11-Mar-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

/*
 * If SM_INIT is true, then astest obtains connection ports to the security
 * servers via the secure Mach initialisation environment.
 *
 * If SM_INIT is false, then astest obtains connection ports to the security
 * servers via the insecure name service provided by the Mach netmsgserver.
 */

#define SM_INIT	0

#include <mach.h>
#include <stdio.h>
#include <ci.h>
#include <servers/netname_defs.h>

#include "auth_defs.h"
#include "auth.h"
#include "name_defs.h"
#include "name.h"
#include "sm.h"

#define getport(args, prompt, default) hexarg(&(args), 0, (prompt), 0, 0xffffffff, (default))


static mach_port_t	las_port;	/* Port to local authentication server. */
static mach_port_t	lns_port;	/* Port to local name server. */
static mach_port_t	env_port;	/* Port to local environment manager. */
static mach_port_t	reg_port;


static char	*scope_table[] = {"local", "global", "either", 0};


static char	*as_errors[] = {"failure", "time-out", "bad private port", "bad name", "not primary",
				"bad password", "bad group", "not allowed", "duplicate id",
				"duplicate name", "not secondary"};

static char	as_error_string[32];

char *as_error(error_code)
int		error_code;
{
    if ((error_code < AS_ERROR_MIN) || (error_code > AS_ERROR_MAX)) {
	(void)sprintf(as_error_string, "unknown error %d", error_code);
	return as_error_string;
    }
    else return as_errors[error_code - AS_ERROR_MIN];
}


static char	*ns_errors[] = {"port not found", "no access", "arg error", "name exists",
				"too many ports", "no central server"};

static char	ns_error_string[32];

char *ns_error(error_code)
int		error_code;
{
    if ((error_code < NS_ERROR_BASE) || (error_code > NS_ERROR_MAX)) {
	(void)sprintf(ns_error_string, "unknown error %d", error_code);
	return ns_error_string;
    }
    else return ns_errors[error_code - NS_ERROR_BASE];
}



do_allocate()
{
    kern_return_t	kr;
    mach_port_t		port;

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			&port)) != KERN_SUCCESS) {
	printf("mach_port_allocate failed, kr = %d.\n", kr);
    }
    else printf("mach_port_allocate succeeds, port = %x.\n", port);
}

do_deallocate(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    mach_port_t		port;

    p = arglist;
    port = getport(p, "port:", 0);
    if ((kr = mach_port_deallocate(mach_task_self(), port)) != KERN_SUCCESS) {
	printf("mach_port_deallocate of %x failed, kr = %d.\n", port, kr);
    }
    else printf("mach_port_deallocate of %x succeeds.\n", port);
}

do_look_up(arglist)
char	*arglist;
{
    kern_return_t	kr;
    mach_port_t		port;
    char		*p;
    netname_name_t	host_name;
    netname_name_t	port_name;

    p = arglist;
    (void)strarg(&p, 0, "port_name:", "", port_name);
    (void)strarg(&p, 0, "host_name:", "", host_name);
    if ((kr = netname_look_up(name_server_port, host_name, port_name, &port)) != KERN_SUCCESS) {
	printf("netname_look_up of '%s' at '%s' failed, kr = %d.\n", port_name, host_name, kr);
    }
    else printf("netname_look_up of '%s' at '%s' succeeds, port = %x.\n", port_name, host_name, port);
}

do_check_in(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    netname_name_t	port_name;
    mach_port_t		port;

    p = arglist;
    (void)strarg(&p, 0, "port_name:", "", port_name);
    port = getport(p, "port:", 0);
    if (port == MACH_PORT_NULL) {
	if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				&port)) != KERN_SUCCESS) {
	    fprintf(stderr, "port_allocate fails, kr = %d.\n", kr);
	    return;
	}
	else printf("port_allocate succeeds, port = %x.\n", port);
    }
    if ((kr = netname_check_in(name_server_port, port_name, mach_task_self(), port)) != KERN_SUCCESS) {
	printf("netname_check_in of '%s' failed, kr = %d.\n", port_name, kr);
    }
    else printf("netname_check_in of '%s' succeeds, port = %x.\n", port_name, port);
}

do_connect()
{
    kern_return_t	kr;
#if	SM_INIT
    if ((kr = sm_user_init()) != KERN_SUCCESS) {
	printf("sm_user_init fails, kr = %d.\n", kr);
    }
    else {
	las_port = sm_auth_port;
	lns_port = sm_name_port;
	env_port = sm_env_port;
	printf("do_connect: las_port = %x, lns_port = %x, env_port = %x.\n",
		las_port, lns_port, env_port);
    }
#else	SM_INIT
    if ((kr = netname_look_up(name_server_port, "", LAS_NAME, &las_port)) != KERN_SUCCESS) {
	printf("netname_look_up of %s failed, kr = %d.\n", LAS_NAME, kr);
	if ((kr = netname_look_up(name_server_port, "", CAS_NAME, &las_port)) != KERN_SUCCESS) {
		printf("netname_look_up of %s failed, kr = %d.\n", CAS_NAME, kr);
	}
	else printf("netname_look_up of %s succeeds, port = %x.\n", CAS_NAME, las_port);
    }
    else printf("netname_look_up of %s succeeds, port = %x.\n", LAS_NAME, las_port);

    if ((kr = netname_look_up(name_server_port, "", LNS_NAME, &lns_port)) != KERN_SUCCESS) {
	printf("netname_look_up of %s failed, kr = %d.\n", LNS_NAME, kr);
    }
    else printf("netname_look_up of %s succeeds, port = %x.\n", LNS_NAME, lns_port);
#endif	SM_INIT
}

do_login(arglist)
char	*arglist;
{
    char		*p;
    group_name_t	user_name;
#if	!SM_INIT
    pass_word_t		pass_word;
    kern_return_t	kr;
#endif	!SM_INIT

    p = arglist;
    (void)strarg(&p, 0, "user_name:", "", user_name);
#if	SM_INIT
    if (sm_login(user_name)) printf("as_login of %s succeeds.\n", user_name);
    else printf("as_login of %s fails.\n", user_name);
#else	SM_INIT
    (void)strarg(&p, 0, "password:", "", pass_word);
    if ((kr = as_login(las_port, user_name, pass_word, &private_port)) == AS_SUCCESS)
	printf("as_login of %s succeeds.\n", user_name);
    else printf("as_login of %s fails, kr = %d.\n", user_name, kr);
#endif	SM_INIT
}

do_logout()
{
    kern_return_t	kr;

    if ((kr = as_logout(las_port, private_port)) != AS_SUCCESS)
	printf("as_logout fails, %s.\n", as_error(kr));
    else printf("as_logout succeeds.\n");
    (void)mach_port_deallocate(mach_task_self(), private_port);
}

do_create_token(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_id_t		group_ids[128];
    unsigned int	group_ids_count = 0;
    mach_port_t		new_token;

    p = arglist;
    while (*p != '\0') group_ids[group_ids_count++] = intarg(&p, 0, "group_id", NULL_ID, 4096, 0);
    if ((kr = as_create_token(las_port, private_port, group_ids, group_ids_count, &new_token))
	!= AS_SUCCESS)
    {
	printf("as_create_token fails, %s.\n", as_error(kr));
    }
    else printf("as_create_token succeeds, new token = %x.\n", new_token);
    user_token = new_token;
}

do_delete_token(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    mach_port_t		token;

    p = arglist;
    token = getport(p, "token", user_token);
    if ((kr = as_delete_token(las_port, private_port, token)) != AS_SUCCESS) {
	printf("as_delete_token of %x fails, %s.\n", token, as_error(kr));
    }
    else printf("as_delete_token of %x succeeds.\n", token);
}

do_verify_token_ids(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_id_list_t	group_ids;
    unsigned int	group_ids_count;
    int			i;
    mach_port_t		token;
    group_id_t		user_id;

    p = arglist;
    token = getport(p, "token", user_token);
    if ((kr = as_verify_token_ids(las_port, token, &user_id, &group_ids, &group_ids_count)) != AS_SUCCESS) {
	printf("as_verify_token_ids of %x fails, %s.\n", token, as_error(kr));
    }
    else {
	printf("as_verify_token_ids of %x succeeds, user_id = %d.\n", token, user_id);
	printf("        group ids =");
	for (i = 0; i < group_ids_count; i++) printf(" %d", group_ids[i]);
	printf(".\n");
    }
}

do_verify_token_names(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_list_t	group_names;
    unsigned int	group_names_count;
    int			i;
    mach_port_t		token;
    group_name_t	user_name;

    p = arglist;
    token = getport(p, "token", user_token);
    if ((kr = as_verify_token_names(las_port, token, user_name, &group_names, &group_names_count))
	!= AS_SUCCESS)
    {
	printf("as_verify_token of %x fails, %s.\n", token, as_error(kr));
    }
    else {
	printf("as_verify_token of %x succeeds, user_name = %s.\n", token, user_name);
	printf("        group names =");
	for (i = 0; i < group_names_count; i++) printf(" %s", group_names[i]);
	printf(".\n");
    }
}

do_create_group(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name;
    group_type_t	group_type;
    full_name_t		full_name;
    pass_word_t		pass_word;
    group_name_t	owner_name;
    group_id_t		group_id;

    p = arglist;
    (void)strarg(&p, 0, "group_name:", "", group_name);
    if ((chrarg(&p, 0, "group_type:", "PS", 'S')) == 0) group_type = AS_PRIMARY;
    else group_type = AS_SECONDARY;
    (void)strarg(&p, 0, "full_name:", "", full_name);
    if (group_type == AS_PRIMARY) (void)strarg(&p, 0, "pass_word:", "", pass_word);
    (void)strarg(&p, 0, "owner_name:", "", owner_name);
    group_id = intarg(&p, 0, "group_id", NULL_ID, 4096, NULL_ID);
    if ((kr = as_create_group(las_port, private_port, group_name, group_type, full_name, pass_word,
				owner_name, &group_id)) != AS_SUCCESS)
    {
	printf("as_create_group of %s fails, %s.\n", group_name, as_error(kr));
    }
    else printf("as_create_group of %s succeeds, group_id = %d.\n", group_name, group_id);
}

do_delete_group(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name;

    p = arglist;
    (void)strarg(&p, 0, "group_name:", "", group_name);
    if ((kr = as_delete_group(las_port, private_port, group_name)) != AS_SUCCESS)
    {
	printf("as_delete_group of %s fails, %s.\n", group_name, as_error(kr));
    }
    else printf("as_delete_group of %s succeeds.\n", group_name);
}

do_change_password(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name;
    pass_word_t		old_pass_word;
    pass_word_t		new_pass_word;

    p = arglist;
    (void)strarg(&p, 0, "group_name:", "", group_name);
    (void)strarg(&p, 0, "old_pass_word:", "", old_pass_word);
    (void)strarg(&p, 0, "new_pass_word:", "", new_pass_word);
    if ((kr = as_change_password(las_port, private_port, group_name, old_pass_word, new_pass_word)) != AS_SUCCESS)
    {
	printf("as_change_password of %s fails, %s.\n", group_name, as_error(kr));
    }
    else printf("as_change_password of %s succeeds.\n", group_name);
}

do_change_owner(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name, new_owner;

    p = arglist;
    (void)strarg(&p, 0, "group_name:", "", group_name);
    (void)strarg(&p, 0, "new_owner:", "", new_owner);
    if ((kr = as_change_owner(las_port, private_port, group_name, new_owner)) != AS_SUCCESS)
    {
	printf("as_change_owner of %s to %s fails, %s.\n", group_name, new_owner, as_error(kr));
    }
    else printf("as_change_owner of %s to %s succeeds.\n", group_name, new_owner);
}

do_add_to_group(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name;
    group_name_t	add_group_name;

    p = arglist;
    (void)strarg(&p, 0, "add_group_name:", "", add_group_name);
    (void)strarg(&p, 0, "group_name:", "", group_name);
    if ((kr = as_add_to_group(las_port, private_port, group_name, add_group_name)) != AS_SUCCESS)
    {
	printf("as_add_to_group of %s to %s fails, %s.\n", add_group_name, group_name, as_error(kr));
    }
    else printf("as_add_to_group of %s to %s succeeds.\n", add_group_name, group_name);
}

do_remove_from_group(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name;
    group_name_t	remove_group_name;

    p = arglist;
    (void)strarg(&p, 0, "remove_group_name:", "", remove_group_name);
    (void)strarg(&p, 0, "group_name:", "", group_name);
    if ((kr = as_remove_from_group(las_port, private_port, group_name, remove_group_name)) != AS_SUCCESS)
    {
	printf("as_remove_from_group of %s from %s fails, %s.\n", remove_group_name, group_name, as_error(kr));
    }
    else printf("as_remove_from_group of %s from %s succeeds.\n", remove_group_name, group_name);
}

do_translate_group_id(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_id_t		group_id;
    group_id_t		owner_id;
    group_name_t	group_name;
    full_name_t		full_name;
    group_type_t	group_type;

    p = arglist;
    group_id = intarg(&p, 0, "group_id", NULL_ID, 4096, NULL_ID);
    if ((kr = as_translate_group_id(las_port, group_id, &owner_id, group_name, &group_type, full_name))
		!= AS_SUCCESS)
    {
	printf("as_translate_group_id of %d fails, %s.\n", group_id, as_error(kr));
    }
    else printf("as_translate_group_id of %d succeeds, group_name = %s, owner_id = %d, group_type = %s, full_name = %s.\n",
		group_id, group_name, owner_id, 
		(group_type == AS_PRIMARY) ? "primary" : "secondary", full_name);
}

do_translate_group_name(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_id_t		group_id;
    group_id_t		owner_id;
    group_name_t	group_name;
    full_name_t		full_name;
    group_type_t	group_type;

    p = arglist;
    (void)strarg(&p, 0, "group_name:", "", group_name);
    if ((kr = as_translate_group_name(las_port, group_name, &owner_id, &group_id, &group_type, full_name))
		!= AS_SUCCESS)
    {
	printf("as_translate_group_name of %s fails, %s.\n", group_name, as_error(kr));
    }
    else printf("as_translate_group_name of %s succeeds, group_id = %d, owner_id = %d, group_type = %s, full_name = %s.\n",
		group_name, group_id, owner_id,
		(group_type == AS_PRIMARY) ? "primary" : "secondary", full_name);
}

do_list_members(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name;
    boolean_t		trans_closure;
    group_id_list_t	group_ids;
    unsigned int	group_ids_count;
    int			i;

    p = arglist;
    (void)strarg(&p, 0, "group_name:", "", group_name);
    trans_closure = boolarg(&p, 0, "transitive_closure:", 0);
    if ((kr = as_list_members(las_port, group_name, trans_closure, &group_ids, &group_ids_count))
	!= AS_SUCCESS)
    {
	printf("as_list_members of %s fails, %s.\n", group_name, as_error(kr));
    }
    else {
	printf("as_list_members %sof %s succeeds.\n", (trans_closure) ? "(transitive closure) " : "",
		group_name);
	printf("        members:");
	for (i = 0; i < group_ids_count; i++) printf(" %d", group_ids[i]);
	printf(".\n");
    }
}

do_list_memberships(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    group_name_t	group_name;
    boolean_t		trans_closure;
    group_id_list_t	group_ids;
    unsigned int	group_ids_count;
    int			i;

    p = arglist;
    (void)strarg(&p, 0, "group_name:", "", group_name);
    trans_closure = boolarg(&p, 0, "transitive_closure:", 0);
    if ((kr = as_list_memberships(las_port, group_name, trans_closure, &group_ids, &group_ids_count))
	    != AS_SUCCESS)
    {
	printf("as_list_memberships of %s fails, %s.\n", group_name, as_error(kr));
    }
    else {
	printf("as_list_memberships %sof %s succeeds.\n", (trans_closure) ? "(transitive closure) " : "",
		group_name);
	printf("        memberships:");
	for (i = 0; i < group_ids_count; i++) printf(" %d", group_ids[i]);
	printf(".\n");
    }
}

do_list_all_groups()
{
    kern_return_t	kr;
    group_id_list_t	group_ids;
    unsigned int	group_ids_count;
    int			i;

    if ((kr = as_list_all_groups(las_port, &group_ids, &group_ids_count)) != AS_SUCCESS)
    {
	printf("as_list_all_groups fails, %s.\n", as_error(kr));
    }
    else {
	printf("as_list_all_groups succeeds.\n");
	printf("        groups:");
	for (i = 0; i < group_ids_count; i++) printf(" %d", group_ids[i]);
	printf(".\n");
    }
}

do_ns_enter_port(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    mach_port_t		port;
    mach_port_t		token;
    port_namestr_t		port_name;
    group_name_t	prot_group_name;
    group_name_t	read_group_name;
    ns_scope_t		scope;

    p = arglist;
    (void)strarg(&p, 0, "port_name:", "", port_name);
    scope = searcharg(&p, 0, "scope:", scope_table, "local");
    (void)strarg(&p, 0, "prot_group_name:", "", prot_group_name);
    (void)strarg(&p, 0, "read_group_name:", "", read_group_name);
    port = getport(p, "port:", 0);
    if (port == MACH_PORT_NULL) {
	if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				&port)) != KERN_SUCCESS) {
	    fprintf(stderr, "port_allocate fails, kr = %d.\n", kr);
	    return;
	}
	else printf("port_allocate succeeds, port = %x.\n", port);
    }
    token = getport(p, "token:", user_token);
    if ((kr = ns_enter_port(lns_port, port, port_name, scope, prot_group_name, read_group_name, token))
	!= KERN_SUCCESS)
    {
	printf("ns_enter_port fails, %s.\n", ns_error(kr));
    }
    else printf("ns_enter_port succeeds.\n");
}

do_ns_enter_mult_port(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    mach_port_t		port;
    mach_port_t		token;
    port_namestr_t		port_name;
    group_name_t	prot_group_name;
    group_name_t	read_group_name;
    ns_scope_t		scope;

    p = arglist;
    (void)strarg(&p, 0, "port_name:", "", port_name);
    scope = searcharg(&p, 0, "scope:", scope_table, "local");
    (void)strarg(&p, 0, "prot_group_name:", "", prot_group_name);
    (void)strarg(&p, 0, "read_group_name:", "", read_group_name);
    port = getport(p, "port:", user_token);
    if (port == MACH_PORT_NULL) {
	if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				&port)) != KERN_SUCCESS) {
	    fprintf(stderr, "port_allocate fails, kr = %d.\n", kr);
	    return;
	}
	else printf("port_allocate succeeds, port = %x.\n", port);
    }
    token = getport(p, "token:", 0);
    if ((kr = ns_enter_mult_port(lns_port, port, port_name, scope, prot_group_name, read_group_name, token))
	!= KERN_SUCCESS)
    {
	printf("ns_enter_mult_port fails, %s.\n", ns_error(kr));
    }
    else printf("ns_enter_mult_port succeeds.\n");
}

do_ns_remove_port(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    mach_port_t		port;
    mach_port_t		token;
    port_namestr_t		port_name;
    group_name_t	prot_group_name;
    ns_scope_t		scope;

    p = arglist;
    (void)strarg(&p, 0, "port_name:", "", port_name);
    scope = searcharg(&p, 0, "scope:", scope_table, "local");
    (void)strarg(&p, 0, "prot_group_name:", "", prot_group_name);
    port = getport(p, "port:", 0);
    if (port == MACH_PORT_NULL) {
	if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				&port)) != KERN_SUCCESS) {
	    fprintf(stderr, "port_allocate fails, kr = %d.\n", kr);
	    return;
	}
	else printf("port_allocate succeeds, port = %x.\n", port);
    }
    token = getport(p, "token:", user_token);
    if ((kr = ns_remove_port(lns_port, port, port_name, scope, prot_group_name, token))
	!= KERN_SUCCESS)
    {
	printf("ns_remove_port fails, %s.\n", ns_error(kr));
    }
    else printf("ns_remove_port succeeds.\n");
}

do_ns_lookup_port(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    mach_port_t		*port;
    mach_port_t		token;
    port_namestr_t		port_name;
    group_name_t	prot_group_name;
    ns_scope_t		scope;

    p = arglist;
    (void)strarg(&p, 0, "port_name:", "", port_name);
    scope = searcharg(&p, 0, "scope:", scope_table, "either");
    (void)strarg(&p, 0, "prot_group_name:", "", prot_group_name);
    token = getport(p, "token:", user_token);
    if ((kr = ns_lookup_port(lns_port, port_name, &scope, prot_group_name, token, &port))
	!= KERN_SUCCESS)
    {
	printf("ns_lookup_port fails, %s.\n", ns_error(kr));
    }
    else printf("ns_lookup_port succeeds, port = %x.\n", port);
}

do_ns_lookup_mult_port(arglist)
char	*arglist;
{
    kern_return_t	kr;
    char		*p;
    ns_port_array_t	ports;
    unsigned int	count, i;
    mach_port_t		token;
    port_namestr_t		port_name;
    group_name_t	prot_group_name;
    ns_scope_t		scope;

    p = arglist;
    (void)strarg(&p, 0, "port_name:", "", port_name);
    scope = searcharg(&p, 0, "scope:", scope_table, "either");
    (void)strarg(&p, 0, "prot_group_name:", "", prot_group_name);
    token = getport(p, "token:", user_token);
    if ((kr = ns_lookup_mult_port(lns_port, port_name, &scope, prot_group_name, token, ports, &count))
			!= KERN_SUCCESS)
    {
	printf("ns_lookup_mult_port fails, %s.\n", ns_error(kr));
    }
    else {
	printf("ns_lookup_mult_port succeeds, ports =");
	for (i = 0; i < count; i++ ) printf(" %x", ports[i]);
	printf(".\n");
    }
}

do_mig_get_token(arglist)
char	*arglist;
{
    mach_port_t	token;

    token = mig_get_token();
    printf("mig_get_token: token = %x, user_token = %x.\n", token, user_token);
}


do_exit()
{
    exit(0);
}


CIENTRY list[] = {
    CIHEX("las_port", las_port),
    CIHEX("lns_port", lns_port),
    CIHEX("env_port", env_port),
    CIHEX("environment_port", environment_port),
    CIHEX("name_server_port", name_server_port),
    CIHEX("private_port", private_port),
    CIHEX("user_token", user_token),
    CICMD("allocate", do_allocate),
    CICMD("deallocate", do_deallocate),
    CICMD("check_in", do_check_in),
    CICMD("netname_look_up", do_look_up),
    CICMD("nnlookup", do_look_up),
    CICMD("connect", do_connect),
    CICMD("login", do_login),
    CICMD("logout", do_logout),
    CICMD("create_token", do_create_token),
    CICMD("ctoken", do_create_token),
    CICMD("delete_token", do_delete_token),
    CICMD("dtoken", do_delete_token),
    CICMD("verify_token_ids", do_verify_token_ids),
    CICMD("vids", do_verify_token_ids),
    CICMD("verify_token_names", do_verify_token_names),
    CICMD("vnames", do_verify_token_names),
    CICMD("create_group", do_create_group),
    CICMD("cgroup", do_create_group),
    CICMD("delete_group", do_delete_group),
    CICMD("dgroup", do_delete_group),
    CICMD("change_password", do_change_password),
    CICMD("change_owner", do_change_owner),
    CICMD("add_to_group", do_add_to_group),
    CICMD("remove_from_group", do_remove_from_group),
    CICMD("rgroup", do_remove_from_group),
    CICMD("translate_group_id", do_translate_group_id),
    CICMD("trid", do_translate_group_id),
    CICMD("translate_group_name", do_translate_group_name),
    CICMD("trname", do_translate_group_name),
    CICMD("list_members", do_list_members),
    CICMD("lmembers", do_list_members),
    CICMD("list_memberships", do_list_memberships),
    CICMD("lships", do_list_memberships),
    CICMD("list_all_groups", do_list_all_groups),
    CICMD("lall", do_list_all_groups),
    CICMD("enter_port", do_ns_enter_port),
    CICMD("eport", do_ns_enter_port),
    CICMD("enter_mult_port", do_ns_enter_mult_port),
    CICMD("emport", do_ns_enter_mult_port),
    CICMD("remove_port", do_ns_remove_port),
    CICMD("rport", do_ns_remove_port),
    CICMD("lookup_port", do_ns_lookup_port),
    CICMD("lport", do_ns_lookup_port),
    CICMD("lookup_mult_port", do_ns_lookup_mult_port),
    CICMD("lmport", do_ns_lookup_mult_port),
    CICMD("mig_get_token", do_mig_get_token),
    CICMD("exit", do_exit),
    CICMD("quit", do_exit),
    CIEND
};

main() {
    (void)do_connect();
    (void)ci("as_test>", 0, 0, list, "/usr/rds/lib/ci/", 0);
}
