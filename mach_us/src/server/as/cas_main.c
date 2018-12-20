/*
 * cas_main.c
 *
 * Main routines for the Central Authentication Server.
 *
 */

/*
 * HISTORY:
 * $Log:	cas_main.c,v $
 * Revision 1.5  92/03/05  15:11:13  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:14:46  jms]
 * 
 * Revision 1.4  91/10/07  00:12:29  jjc
 * 	Removed "CAS initialised." message.
 * 	[91/04/25            jjc]
 * 
 * Revision 1.3  89/05/18  09:41:44  dorr
 * 	include file cataclysm
 * 
 * Revision 1.2  89/05/04  17:53:35  mbj
 * 	Allow asdatabase filename to be passed in at runtime.
 * 	[89/04/17  15:27:41  mbj]
 * 
 *  4-Aug-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Only set AS_SM_INIT to false if not already defined.
 *
 *  9-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added start up code which gets the service port via sm_init_get_port.
 *
 *  2-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added ability to act a local authentication server.
 *
 * 10-Mar-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

/*
 * If AS_SM_INIT is true, then the authentication server obtains and supplies
 * its initial connection ports via the secure Mach initialisation sequence.
 * This sequence is set in motion by executing the program sm_init either as
 * a stand-alone system or as part of the /etc/mach_init sequence.
 *
 * If AS_SM_INIT is false, then the authentication server obtains and supplies
 * its initial connection ports via the insecure name service provided by the
 * Mach netmsgserver.  This mode of operation cannot be guaranteed to be secure
 * and should only be used when testing the server.
 */

#ifndef	AS_SM_INIT
#define AS_SM_INIT	0
#endif	AS_SM_INIT

#if	AS_SM_INIT
#define	MACH_INIT_SLOTS		1
#endif	AS_SM_INIT

#include <mach.h>
#include <mach_init.h>
#include <stdio.h>
#include <mach/message.h>
#include <mach/notify.h>

#include "auth_defs.h"
#include "cas_defs.h"

#if AS_SM_INIT
#include "sm_init_defs.h" 
#endif

static mach_port_t	cas_service_port;
mach_port_array_t	cas_ports;
unsigned int	cas_ports_cnt = 0;
#if	AS_SM_INIT
static mach_port_t	private_service_port;
#endif	AS_SM_INIT


static mach_port_t	receive_port_set = MACH_PORT_NULL;
mach_port_t	task_notify_port = MACH_PORT_NULL;
#define MSG_SIZE_MAX	8192

#if	AS_SM_INIT
/*
 * as_init_cas_ports
 *	receives the central authentication server ports.
 *
 */
kern_return_t as_init_cas_ports(server_port, in_cas_ports, in_cas_ports_cnt)
mach_port_t		server_port;
mach_port_array_t	in_cas_ports;
unsigned int	in_cas_ports_cnt;
{
    if (server_port != private_service_port) {
	fprintf(stderr, "as_init_cas_ports: server port (%x) is not private service port (%x).\n",
				server_port, private_service_port);
	return KERN_FAILURE;
    }
    cas_ports = in_cas_ports;
    cas_ports_cnt = in_cas_ports_cnt;
#if CAS_DEBUG
    {
	int	i;
	printf("as_init_cns_ports: central authentication server ports =");
	for (i = 0; i++; i < cas_ports_cnt) printf(" %x", cas_ports[i]);
	printf(".\n");
    }
#endif CAS_DEBUG
    return KERN_SUCCESS;
}


/*
 * as_init_get_as_port
 *	used by the local CKDS to get our public service port.
 *
 * Returns:
 *	as_port	: the public AS port.
 *
 */
kern_return_t as_init_get_as_port(server_port, as_port_ptr)
mach_port_t	server_port;
mach_port_t	*as_port_ptr;
{
    if (server_port != private_service_port) {
	fprintf(stderr, "as_init_get_as_ports: server port (%x) is not private service port (%x).\n",
				server_port, private_service_port);
	return KERN_FAILURE;
    }
    *as_port_ptr = cas_service_port;
    return KERN_SUCCESS;
}


#endif	AS_SM_INIT


/*
 * cas_init
 *	initialises the central authentication server.
 *
 * Results:
 *	TRUE if all went well, FALSE otherwise.
 *
 * Design:
 *	Call cas_utils_init.
 *	Reads in the database.
 *	Sets up our service port.
 *	Enable the notify port.
 *
 */
boolean_t cas_init(db_filename) char *db_filename;
{
    kern_return_t	kr;
#if	AS_SM_INIT
    mach_port_array_t	reg_ports;
    unsigned int	reg_ports_cnt;
    int			i;
    mach_port_t		sm_init_port;
    int			pid;
#endif	AS_SM_INIT

    (void) mach_port_allocate(mach_task_self(), 
		MACH_PORT_RIGHT_PORT_SET, &receive_port_set);

    if (!(cas_utils_init())) {
	fprintf(stderr, "cas_init.cas_utils_init fails.\n");
	return FALSE;
    }

    if (!(read_database(db_filename))) {
	fprintf(stderr, "cas_init.read_database fails.\n");
	return FALSE;
    }

#if	AS_SM_INIT
    if ((kr = mach_ports_lookup(mach_task_self(), &reg_ports, &reg_ports_cnt)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_init.mach_ports_lookup fails, kr = %d.\n", kr);
	return FALSE;
    }
    if (reg_ports_cnt < SM_INIT_SLOTS_USED) {
	fprintf(stderr, "cas_init: too few (%d) registered ports.\n", reg_ports_cnt);
	return KERN_FAILURE;
    }
    sm_init_port = reg_ports[SM_INIT_SLOT];
#if CAS_DEBUG
    printf("cas_init: sm_init port is %x.\n", sm_init_port);
#endif
    if ((kr = sm_init_get_port(sm_init_port, AUTH_SERVER_INDEX, &cas_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_init.sm_init_get_port fails, kr = %d.\n", kr);
	return FALSE;
    }
    (void)mach_port_move_member(mach_task_self(), cas_service_port, receive_port_set);
#if CAS_DEBUG
    printf("cas_init: cas service port is %x.\n", cas_service_port);
#endif
    /*
     * Tell the name server about it.
     */
    if ((kr = ns_init_as_port(reg_ports[NAME_PRIVATE_SLOT], cas_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_init.ns_init_as_port fails, kr = %d.\n", kr);
	return FALSE;
    }

    /*
     * Allocate a private port for the  KDS to use and register it.
     */
    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
		&private_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_init.port_allocate fails, kr = %d.\n", kr);
	return FALSE;
    }
    reg_ports[AUTH_PRIVATE_SLOT] = private_service_port;

    (void)mach_port_move_member(mach_task_self(), private_service_port, receive_port_set);
#if CAS_DEBUG
    printf("cas_init: private service port is %x.\n", private_service_port);
#endif
    for (i = 0; i < reg_ports_cnt; i++) {
	mach_port_t	previous_port_dummy = MACH_PORT_NULL;
	mach_port_request_notification(mach_task_self(),
	    reg_ports[i], MACH_NOTIFY_DEAD_NAME, 1,
	    task_notify_port, MACH_MSG_TYPE_MAKE_SEND_ONCE,
	    &previous_port_dummy);
	    if (previous_port_dummy != MACH_PORT_NULL) {
		    mach_port_deallocate(mach_task_self(),
			    previous_port_dummy);
	    }
    }

    if ((kr = mach_ports_register(mach_task_self(), reg_ports, reg_ports_cnt)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_init.mach_ports_register fails, kr = %d.\n", kr);
	return FALSE;
    }

    /*
     * Now start up the secure network server.
     */
    pid = fork();
    if (pid < 0) {
	perror("cas_init.fork");
	return -1;
    }
    else if (pid == 0) {
	if (execl(NETWORK_SERVER_NAME, NETWORK_SERVER_NAME, (char *)0) < 0) {
	    perror("cas_init.exec");
	    exit(-1);
	}
	/*
	 * Should not return from here.
	 */
    }
#if	CAS_DEBUG
    printf("cas_init: network server forked, pid = %d.\n", pid);
#endif	CAS_DEBUG

#else	AS_SM_INIT

    if ((kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, 
		&cas_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "cas_init.port_allocate fails, kr = %d.\n", kr);
	return FALSE;
    }
    (void)mach_port_move_member(mach_task_self(), cas_service_port, receive_port_set);
    if ((kr = netname_check_in(name_server_port, CAS_NAME, mach_task_self(), cas_service_port))
	!= KERN_SUCCESS)
    {
	fprintf(stderr, "cas_init.netname_check_in fails, kr = %d.\n", kr);
	return FALSE;
    }

#endif	AS_SM_INIT

    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
		&task_notify_port);

    (void)mach_port_move_member(mach_task_self(), task_notify_port, receive_port_set);
    return TRUE;
}



/*
 * cas_main
 *	receives messages and processes them accordingly.
 *
 * Results:
 *	none.
 *
 * Design:
 *	waits for a message and either calls c_auth_server, as_init_server
 *	or cas_handle_port_death depending on which port the message was received.
 *
 */
void cas_main()
{
    kern_return_t	kr;
    mach_dead_name_notification_t	*notify_msg_ptr;
    typedef struct msg_buffer {
	mach_msg_header_t	head;
	char			data[MSG_SIZE_MAX-sizeof(mach_msg_header_t)];
    } msg_buffer_t;

    msg_buffer_t	msg1, msg2;
    msg_buffer_t	*in_msg, *out_msg, *tmp_msg;
    boolean_t		replying;


    /* Setup for first request */
    in_msg = &msg1;
    out_msg = &msg2;
    bzero(&(msg1.head), sizeof(mach_msg_header_t));
    bzero(&(msg2.head), sizeof(mach_msg_header_t));
    replying = FALSE;


    while (1) {
	if (replying) {
	    out_msg->head.msgh_local_port = MACH_PORT_NULL;

	    kr = mach_msg(out_msg, 
		    MACH_RCV_MSG|MACH_SEND_MSG,
		    out_msg->head.msgh_size, sizeof(msg_buffer_t),
		    receive_port_set,
		    MACH_MSG_TIMEOUT_NONE,
		    MACH_PORT_NULL);

	    tmp_msg = out_msg;
	    out_msg = in_msg;
	    in_msg = tmp_msg;
	} else {
	    in_msg->head.msgh_local_port = receive_port_set;
	    in_msg->head.msgh_size = sizeof(msg_buffer_t);
	    in_msg->head.msgh_bits = MACH_MSGH_BITS_ZERO;
	    kr = mach_msg_receive(in_msg);
	}

	/* Belch on error */
	if (kr != MACH_MSG_SUCCESS) {
	    if ((err_get_system(kr) == err_mach_ipc) &&
		(err_get_sub(kr) == 4)) {
		fprintf(stderr, "cas_main.msg_receive fails, kr = %d.\n", kr);
	    }
	    else if ((err_get_system(kr) == err_mach_ipc) &&
		(err_get_sub(kr) == 0)) {
        	/* send_error */
		fprintf(stderr, "cas_main.msg_send fails, kr = %d.\n", kr);
	    }
	    else {
		/* some other error */
		fprintf(stderr, "cas_main.msg fails, kr = %d.\n", kr);
	    }

	    replying = FALSE;
	    continue;
	}
	else {
#if CAS_DEBUG
	    printf("cas_main: received message on port %x with id %d.\n", in_msg->head.msgh_local_port,
			in_msg->head.msgh_id);
#endif CAS_DEBUG
	    replying = FALSE;
	    out_msg->head.msgh_bits = in_msg->head.msgh_bits;

	    if (in_msg->head.msgh_local_port == cas_service_port) {
		if (c_auth_server(&(in_msg->head), &(out_msg->head))) {
		    replying = TRUE;
		    continue;
		}
		else if (auth_server(&(in_msg->head), &(out_msg->head))) {
		    replying = TRUE;
		    continue;
		}
		else {
		    fprintf(stderr, "cas_main: c_auth_server & auth_server fail, msg_id = %d.\n",
					in_msg->head.msgh_id);
		    replying = FALSE;
		    continue;
		}
	    }
#if	AS_SM_INIT
	    else if (in_msg->head.msgh_local_port == private_service_port) {
		if (!(as_init_server(&in_msg->head, &out_msg->head))) {
		    fprintf(stderr, "cas_main.as_init_server fails, msg_id = %d.\n",
				in_msg->head.msgh_id);
		    replying = FALSE;
		    continue;
		}
		replying = TRUE;
		continue;
	    }
#endif	AS_SM_INIT
	    else if (in_msg->head.msgh_local_port == task_notify_port) {
		replying = FALSE;
		if ((in_msg->head.msgh_id == MACH_NOTIFY_DEAD_NAME) ||
		    (in_msg->head.msgh_id == MACH_NOTIFY_PORT_DESTROYED)) {
		    notify_msg_ptr = (mach_dead_name_notification_t *)&in_msg;
		    cas_handle_port_death(notify_msg_ptr->not_port);
		}
		else {
		    fprintf(stderr, "cas_main: received unexpected notification, msg_id = %d.\n",
					in_msg->head.msgh_id);
		}
	    }
	    else {
		fprintf(stderr, "cas_main: received message on unexpected port %x.\n",
				in_msg->head.msgh_local_port);
		replying = FALSE;
		continue;
	    }
	}
    }
}



void usage()
{
    fprintf(stderr, "Usage: cas [-f database_filename]\n");
}

/*
 * main
 *	calls cas_init followed by cas_main.
 *
 */
main(argc, argv) int argc; char *argv[];
{
    char *db_filename;

    db_filename = NULL;

    if (argc > 1) {
	if (strcmp(argv[1], "-f") == 0 && argc == 3)
	    db_filename = argv[2];
	else {
	    usage();
	    exit(1);
	}
    }

    if (cas_init(db_filename)) {
	cas_main();
    }
    else {
	fprintf(stderr, "cas_init fails - exiting.\n");
	exit(-1);
    }
}
