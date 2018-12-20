/*
 * las_main.c
 *
 * Main routines for the Local Authentication Server.
 *
 */

/*
 * HISTORY:
 * $Log:	las_main.c,v $
 * Revision 1.2  89/05/18  09:43:19  dorr
 * 	include file cataclysm
 * 
 *  4-Aug-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Only set AS_SM_INIT to false if not already defined.
 *
 *  9-Jul-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added start up code which gets the service port via sm_init_get_port.
 *
 * 10-Mar-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#define LAS_DEBUG	0

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
#define	MACH_INIT_SLOTS	1
#endif	AS_SM_INIT

#include <mach.h>
#include <mach_init.h>
#include <msg_type.h>
#include <stdio.h>
#include <mach/message.h>
#include <mach/notify.h>

#include "auth_defs.h"
#include "sm_init_defs.h"

extern void las_handle_port_death();

port_t		cas_port;
static port_t	las_service_port;
#if	AS_SM_INIT
static port_t	private_service_port;
#endif	AS_SM_INIT


#if	AS_SM_INIT
/*
 * as_init_cas_ports
 *	receives the central authentication server port (takes the first one provided).
 *
 */
kern_return_t as_init_cas_ports(server_port, cas_ports, cas_ports_cnt)
port_t		server_port;
port_array_t	cas_ports;
unsigned int	cas_ports_cnt;
{
    if (server_port != private_service_port) {
	fprintf(stderr, "as_init_cas_ports: server port (%x) is not private service port (%x).\n",
				server_port, private_service_port);
	return KERN_FAILURE;
    }
    if (cas_ports_cnt < 1) {
	fprintf(stderr, "as_init_cas_ports: no central name server port.\n");
	return KERN_FAILURE;
    }
    cas_port = cas_ports[0];
#if LAS_DEBUG
    printf("as_init_cas_ports: central authentication server port = %x.\n", cas_port);
#endif
    return KERN_SUCCESS;
}


/*
 * as_init_get_as_port
 *	no meaning for the local authentication server.
 *
 */
/* ARGSUSED */
kern_return_t as_init_get_as_port(server_port, as_port_ptr)
port_t	server_port;
port_t	*as_port_ptr;
{
    fprintf(stderr, "as_init_get_as_port called unexpectedly.\n");
    return KERN_FAILURE;
}

#endif	AS_SM_INIT

/*
 * las_init
 *	initialises the local authentication server.
 *
 * Results:
 *	TRUE if all went well, FALSE otherwise.
 *
 * Design:
 *	Either get hold of the service port using sm_init_get_port
 *	or allocate and check in the service port.
 *	May look up the central authentication server port.
 *	May start up the network server.
 *	May register our private service port for use by the KDS.
 *
 */
boolean_t las_init()
{
    kern_return_t	kr;
#if	AS_SM_INIT
    port_array_t	reg_ports;
    unsigned int	reg_ports_cnt;
    port_t		sm_init_port;
    int			pid;
#endif	AS_SM_INIT

#if	AS_SM_INIT
    if ((kr = mach_ports_lookup(task_self(), &reg_ports, &reg_ports_cnt)) != KERN_SUCCESS) {
	fprintf(stderr, "las_init.mach_ports_lookup fails, kr = %d.\n", kr);
	return FALSE;
    }
    if (reg_ports_cnt < SM_INIT_SLOTS_USED) {
	fprintf(stderr, "las_init: too few (%d) registered ports.\n", reg_ports_cnt);
	return KERN_FAILURE;
    }
    sm_init_port = reg_ports[SM_INIT_SLOT];
#if LAS_DEBUG
    printf("las_init: sm_init port is %x.\n", sm_init_port);
#endif
    if ((kr = sm_init_get_port(sm_init_port, AUTH_SERVER_INDEX, &las_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "las_init.sm_init_get_port fails, kr = %d.\n", kr);
	return FALSE;
    }
    (void)port_enable(task_self(), las_service_port);
#if LAS_DEBUG
    printf("las_init: las service port is %x.\n", las_service_port);
#endif
    /*
     * Tell the name server about it.
     */
    if ((kr = ns_init_as_port(reg_ports[NAME_PRIVATE_SLOT], las_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "las_init.ns_init_as_port fails, kr = %d.\n", kr);
	return FALSE;
    }

    /*
     * Allocate a private port for the  KDS to use and register it.
     */
    if ((kr = port_allocate(task_self(), &private_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "las_init.port_allocate fails, kr = %d.\n", kr);
	return FALSE;
    }
    (void)port_enable(task_self(), private_service_port);
#if LAS_DEBUG
    printf("las_init: private service port is %x.\n", private_service_port);
#endif
    reg_ports[AUTH_PRIVATE_SLOT] = private_service_port;
    if ((kr = mach_ports_register(task_self(), reg_ports, reg_ports_cnt)) != KERN_SUCCESS) {
	fprintf(stderr, "las_init.mach_ports_register fails, kr = %d.\n", kr);
	return FALSE;
    }

    /*
     * Now start up the secure network server.
     */
    pid = fork();
    if (pid < 0) {
	perror("las_init.fork");
	return -1;
    }
    else if (pid == 0) {
	if (execl(NETWORK_SERVER_NAME, NETWORK_SERVER_NAME, (char *)0) < 0) {
	    perror("las_init.exec");
	    exit(-1);
	}
	/*
	 * Should not return from here.
	 */
    }
#if	LAS_DEBUG
    printf("las_init: network server forked, pid = %d.\n", pid);
#endif	LAS_DEBUG

#else	AS_SM_INIT

    if ((kr = port_allocate(task_self(), &las_service_port)) != KERN_SUCCESS) {
	fprintf(stderr, "las_init.port_allocate fails, kr = %d.\n", kr);
	return FALSE;
    }
    (void)port_enable(task_self(), las_service_port);
    if ((kr = netname_check_in(name_server_port, LAS_NAME, task_self(), las_service_port))
	!= KERN_SUCCESS)
    {
	fprintf(stderr, "las_init.netname_check_in fails, kr = %d.\n", kr);
	return FALSE;
    }

    do {
	if ((kr = netname_look_up(name_server_port, "*", CAS_NAME, &cas_port)) != KERN_SUCCESS) {
	    fprintf(stderr, "las_init.netname_look_up of %s fails - trying again.\n", CAS_NAME);
	}
    } while (kr != KERN_SUCCESS);
#if LAS_DEBUG
    printf("las_init: found CAS port (%d).\n", cas_port);
#endif LAS_DEBUG

#endif	AS_SM_INIT

    (void)port_enable(task_self(), task_notify());
    return TRUE;
}



/*
 * las_main
 *	receives messages and processes them accordingly.
 *
 * Results:
 *	none.
 *
 * Design:
 *	waits for a message and either calls auth_server or las_handle_port_death
 *	depending on which port the message was received.
 *
 */
void las_main()
{
    kern_return_t	kr;
    notification_t	*notify_msg_ptr;
    struct msg_buffer {
	msg_header_t	header;
	char		data[1000];
    } rcv_buffer, send_buffer;

    while (1) {
	rcv_buffer.header.msg_size = sizeof(struct msg_buffer);
	rcv_buffer.header.msg_local_port = PORT_ENABLED;
	if ((kr = msg_receive(&(rcv_buffer.header), MSG_OPTION_NONE, 0)) == KERN_SUCCESS) {
#if	LAS_DEBUG
	    printf("las_main: received message on port %x with id %d.\n", rcv_buffer.header.msg_local_port,
			rcv_buffer.header.msg_id);
#endif	LAS_DEBUG
	    if (rcv_buffer.header.msg_local_port == las_service_port) {
		if (!(auth_server(&rcv_buffer.header, &send_buffer.header))) {
		    fprintf(stderr, "las_main.auth_server fails, msg_id = %d.\n", rcv_buffer.header.msg_id);
		}
		if ((kr = msg_send(&(send_buffer.header), MSG_OPTION_NONE, 0)) != KERN_SUCCESS) {
		    fprintf(stderr, "las_main.msg_send fails, kr = %d.\n", kr);
		}
	    }
#if	AS_SM_INIT
	    else if (rcv_buffer.header.msg_local_port == private_service_port) {
		if (!(as_init_server(&rcv_buffer.header, &send_buffer.header))) {
		    fprintf(stderr, "las_main.as_init_server fails, msg_id = %d.\n",
				rcv_buffer.header.msg_id);
		}
		if ((kr = msg_send(&(send_buffer.header), MSG_OPTION_NONE, 0)) != KERN_SUCCESS) {
		    fprintf(stderr, "las_main.msg_send fails, kr = %d.\n", kr);
		}
	    }
#endif	AS_SM_INIT
	    else if (rcv_buffer.header.msg_local_port == task_notify()) {
		if (rcv_buffer.header.msg_id == NOTIFY_PORT_DELETED) {
		    notify_msg_ptr = (notification_t *)&rcv_buffer;
		    if (notify_msg_ptr->notify_port == cas_port) {
#if	AS_SM_INIT
			fprintf(stderr, "las_main: cas port died.\n");
#else	AS_SM_INIT
			fprintf(stderr, "las_main: cas port died - trying to find it again.\n");
			do {
			    if ((kr = netname_look_up(name_server_port, "*", CAS_NAME, &cas_port))
				!= KERN_SUCCESS)
			    {
				fprintf(stderr, "las_main.netname_look_up of %s fails - trying again.\n",
							CAS_NAME);
			    }
			} while (kr != KERN_SUCCESS);
#if LAS_DEBUG
			printf("las_main: found CAS port (%d).\n", cas_port);
#endif LAS_DEBUG
#endif	AS_SM_INIT
		    }
		    else {
			las_handle_port_death(notify_msg_ptr->notify_port);
		    }
		}
		else {
		    fprintf(stderr, "las_main: received unexpected notification, msg_id = %d.\n",
					rcv_buffer.header.msg_id);
		}
	    }
	    else {
		fprintf(stderr, "las_main: received message on unexpected port %x.\n",
				rcv_buffer.header.msg_local_port);
	    }
	}
	else {
	    fprintf(stderr, "las_main.msg_receive fails, kr = %d.\n", kr);
	}
    }
}



/*
 * main
 *	calls las_init followed by las_main.
 *
 */
main()
{
    if (las_init()) {
 	printf("LAS initialised.\n");
	las_main();
    }
    else {
	fprintf(stderr, "las_init fails - exiting.\n");
	exit(-1);
    }
}
