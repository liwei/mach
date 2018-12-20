/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log:	snames.c,v $
 * Revision 2.3  94/07/07  16:38:13  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  91/10/07  00:12:47  jjc
 * 	Incorporated snames name server into the Configuration Server,
 * 	splitting main() into snames_setup() and snames().
 * 	[91/09/22            jjc]
 * 
 * Revision 2.4  91/03/27  17:29:44  mrt
 * 	Changed mach.h include
 * 
 * Revision 2.3  91/03/19  12:40:09  mrt
 * 	Changed to new copyright
 * 
 * Revision 2.2  90/09/12  16:33:55  rpd
 * 	Initial check-in.
 * 	[90/09/12  15:53:26  rpd]
 * 
 */

#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <mach.h>
#include <mach/message.h>
#include <mach/notify.h>

#define streql(a, b)	(strcmp((a), (b)) == 0)

extern int errno;
extern char *sys_errlist[];
extern int sys_nerr;

void netname_init();

char *program = "snames";
boolean_t Debug = FALSE;

mach_port_t notify;
mach_port_t service;

static boolean_t snames_demux();

struct mach_msg_server_args {
	boolean_t (*demux)();
	mach_msg_size_t max_size;
	mach_port_t rcv_name;
};


static char *
unix_error_string(errno)
    int errno;
{
    if ((0 <= errno) && (errno < sys_nerr))
	return sys_errlist[errno];
    else
	return "unknown errno";
}

snames_setup(debug_level)
    int	debug_level;
{
    mach_port_t *rports;
    unsigned int rportCnt;
    kern_return_t kr;
    struct mach_msg_server_args	args;
    extern mach_msg_return_t mach_msg_server();

    if (debug_level > 1)
	Debug = TRUE;

    /* Allocate our notification port. */

    kr = mach_port_allocate(mach_task_self(),
			    MACH_PORT_RIGHT_RECEIVE, &notify);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: mach_port_allocate: %s\n",
	     program, mach_error_string(kr));

    kr = service_checkin(service_port, name_server_port, &service);
    if (kr == KERN_SUCCESS) {
	/* We are the official name server. */

	if (Debug)
	    printf("%s: I'm the official name server!\n", program);
    } else {
	mach_port_t previous;

	if (Debug)
	    printf("%s: service_checkin: %s\n",
		   program, mach_error_string(kr));

	/* Allocate the netname service port. */

	kr = mach_port_allocate(mach_task_self(),
				MACH_PORT_RIGHT_RECEIVE, &service);
	if (kr != KERN_SUCCESS)
	    quit(1, "%s: mach_port_allocate: %s\n",
		 program, mach_error_string(kr));

	kr = mach_port_insert_right(mach_task_self(), service,
				    service, MACH_MSG_TYPE_MAKE_SEND);
	if (kr != KERN_SUCCESS)
	    quit(1, "%s: mach_port_insert_right: %s\n",
		 program, mach_error_string(kr));

	/* Register the new service port. */

	kr = mach_ports_lookup(mach_task_self(), &rports, &rportCnt);
	if (kr != KERN_SUCCESS)
	    quit(1, "%s: mach_ports_lookup: %s\n",
		 program, mach_error_string(kr));

	if (rportCnt == 0) {
	    kr = mach_ports_register(mach_task_self(), &service, 1);
	    if (kr != KERN_SUCCESS)
		quit(1, "%s: mach_ports_register: %s\n",
		     program, mach_error_string(kr));
	} else {
	    rports[0] = service;

	    kr = mach_ports_register(mach_task_self(), rports, rportCnt);
	    if (kr != KERN_SUCCESS)
		quit(1, "%s: mach_ports_register: %s\n",
		     program, mach_error_string(kr));

	    /* too bad we can't use the deallocate bit on the register call */

	    kr = vm_deallocate(mach_task_self(), (vm_address_t) rports,
			       (vm_size_t) (rportCnt * sizeof(mach_port_t)));
	    if (kr != KERN_SUCCESS)
		quit(1, "%s: vm_deallocate: %s\n",
		     program, mach_error_string(kr));
	}
    }
    /* 
     *	Set name server port to our service port
     *	since we're the new name server.
     */
    name_server_port = service;
}

snames()
{
    mach_port_t pset;
    kern_return_t kr;

    /*
     *	Prepare the name service.
     *	The three do_netname_check_in calls will consume user-refs
     *	for their port args, so we have to generate the refs.
     *	Note that mach_task_self() is just a macro;
     *	it doesn't return a ref.
     */

    netname_init();

    kr = mach_port_mod_refs(mach_task_self(), mach_task_self(),
			    MACH_PORT_RIGHT_SEND, 3);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: mach_port_mod_refs: %s\n",
	     program, mach_error_string(kr));

    kr = mach_port_mod_refs(mach_task_self(), service,
			    MACH_PORT_RIGHT_SEND, 3);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: mach_port_mod_refs: %s\n",
	     program, mach_error_string(kr));

    kr = do_netname_check_in(service, "NameServer",
			     mach_task_self(), service);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: netname_check_in: %s\n",
	     program, mach_error_string(kr));

    kr = do_netname_check_in(service, "NMMonitor",
			     mach_task_self(), service);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: netname_check_in: %s\n",
	     program, mach_error_string(kr));

    kr = do_netname_check_in(service, "NMControl",
			     mach_task_self(), service);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: netname_check_in: %s\n",
	     program, mach_error_string(kr));

    /* Prepare our port set. */

    kr = mach_port_allocate(mach_task_self(),
			    MACH_PORT_RIGHT_PORT_SET, &pset);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: mach_port_allocate: %s\n",
	     program, mach_error_string(kr));

    kr = mach_port_move_member(mach_task_self(), service, pset);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: mach_port_move_member: %s\n",
	     program, mach_error_string(kr));

    kr = mach_port_move_member(mach_task_self(), notify, pset);
    if (kr != KERN_SUCCESS)
	quit(1, "%s: mach_port_move_member: %s\n",
	     program, mach_error_string(kr));

    /* Enter service loop. */

    if (Debug)
	printf("%s: entering service loop\n", program);

    kr = mach_msg_server(snames_demux, 256, pset);
    quit(1, "%s: mach_msg_server: %s\n", program, mach_error_string(kr));
}

static boolean_t
snames_demux(request, reply)
    mach_msg_header_t *request, *reply;
{
    if (request->msgh_local_port == service)
	return netname_server(request, reply);
    else if (request->msgh_local_port == notify)
	return notify_server(request, reply);
    else
	quit(1, "%s: snames_demux: port = %x\n",
	     program, request->msgh_local_port);
}

/*
 *	XXX This is a ho-ho, but I couldn't get varargs 
 *	    to work correctly on the 386 XXX
 */
quit(exitcode, a1, a2, a3, a4, a5, a6, a7, a8, a9)
int	exitcode;
char	*a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9;
{
	printf("quit(%d)\n", exitcode);
	fprintf(stderr, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	exit(exitcode);
}
