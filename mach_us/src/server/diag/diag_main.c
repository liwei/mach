/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989 Carnegie Mellon University
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
 *  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they made and grant Carnegie Mellon the
 * rights to redistribute these changes.
 */

/*
 * HISTORY:
 * $Log:	diag_main.c,v $
 * Revision 1.15  94/07/13  16:44:37  mrt
 * 	Updated copyright
 * 
 * Revision 1.14  92/03/05  15:12:32  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:27:13  jms]
 * 
 * Revision 1.13  91/10/07  00:12:52  jjc
 * 	Changed to only print out "DIAG: server started" message
 * 	when debugging is on.
 * 	[91/04/25            jjc]
 * 
 * Revision 1.12  90/10/29  18:12:31  dpj
 * 	Do not lookup remote netmsgserver in the local case. This allows
 * 	the use of the "snames" server instead of the netmsgserver.
 * 	[90/08/29  06:10:54  dpj]
 * 
 * Revision 1.11  90/08/22  18:14:11  roy
 * 	Use mach_error().
 * 	[90/08/15  16:03:36  roy]
 * 
 * Revision 1.10  89/07/09  14:20:53  dpj
 * 	Output messages if their level is smaller than the client's
 * 	global debug level, and not the opposite.
 * 	Initialize the client's debug level to Dbg_Level_Max, so
 * 	that everything is output.
 * 	[89/07/08  13:10:42  dpj]
 * 
 * Revision 1.9  89/05/18  10:27:29  dorr
 * 	include file cataclysm
 * 
 * Revision 1.8  89/05/04  17:55:18  mbj
 * 	Don't print an extra newline after messages already ending with them.
 * 
 */

#include <mach/mach_types.h>

#include <stdio.h>
#include <mach_error.h>
#include <debug.h>
#include <cthreads.h>

#include "diag_defs.h"
#include "hash.h"

/* mach_error is #defined to be a macro which results in access 
 *   to the Diag server.  We can't allow this here because this is
 *   the Diag server.
 */
#undef mach_error   

extern void mach_diag_server();
void diag_notify();

int 	current_debug_level = 0;
	
#define	DEFAULT_TSIZE 	16

typedef struct client_data {
    char *name;
    int	 debug_level;
} *client_data_t;

/* XXX mutex lock */
#define	client_lookup(ht,k)	((client_data_t) hash_lookup(ht,k))

/* XXX mutex lock */
#define	client_enter(ht,k,v)	hash_enter(ht,k,v)	

/* must be a power of two */

hash_table_t 	client_ports;
int		client_table_size = DEFAULT_TSIZE;

static mach_port_t	receive_port_set = MACH_PORT_NULL;

int client_hash(c1)
client_data_t c1;
{
    int c = (int) c1;
    return( c ^ (client_table_size - 1));
}

client_data_t client_init(name,level)
char *name;
int level;
{
    client_data_t c = (client_data_t) malloc(sizeof(struct client_data));
    char *n;

    c->name = (char *) malloc( strlen(name));
    strcpy(/* to */ c->name, /* from */ name);
    
    c->debug_level = level;
    return(c);
}

boolean_t client_compare(c1,c2)
client_data_t c1, c2;
{
    return (c1 == c2);
}

void XXX_hack_cthread_link_hack_XXX(){
	cthread_detach( cthread_fork(0,0));
}	

#define USAGE()	\
	fprintf(stderr,"Usage: %s [-d<debug-level>] -h[host-name]\n",program)
main(argc, argv)
	int  		argc;
	char		* argv[];
{
	mach_error_t		err;
	char 			*program, *host_name = "";
	mach_port_t			anyuser_port, server_port;

	program = argv[0];
	for (argc--,argv++; (argc > 0) && (argv[0][0] == '-'); argc--,argv++) {
		if (!strncmp(argv[0],"-d",2)) {
			sscanf(argv[0]+2,"%d",&current_debug_level);
			continue;
		}
		if (!strncmp(argv[0],"-h",2)) {
			host_name = argv[0]+2;
			continue;
		}
	}
	if (argc > 1) {
		USAGE();
		exit(1);
	} 


	/* initialize port --> client_data hash table */
	client_ports = hash_init(client_hash,client_compare,client_table_size);

	/* global server port */
	err = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
				 &anyuser_port);
	if (err) {
		mach_error("port_allocate", err);
		exit(1);
	}

	err = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET, 
					&receive_port_set);
	if (err) {
		mach_error("port_set_allocate", err);
		exit(1);
	}
    	err = mach_port_move_member(mach_task_self(), anyuser_port, 
					receive_port_set);
	if (err) {
		mach_error("port_enable", err);
		exit(1);
	}

	/*
	 * Check-in to the netname database on the appropriate host.
	 * The name_server_port on that host is obtained with the
	 * following call.
	 */
	if (!strcmp(host_name,"")) {
		server_port = name_server_port;
	} else {
		err = netname_look_up(name_server_port, host_name,
			      "new_network_name_server", &server_port);
		if (err) {
			mach_error(
			"netname_look_up(new_network_name_server)", err);
			exit(1);
		}
	}

	err = netname_check_in(server_port, "mach-diag", MACH_PORT_NULL,
				anyuser_port);
	if (err) {
		mach_error("netname_check_in", err);
		exit(1);
	}

	/* greeting */
	if (current_debug_level > 0)
		fprintf( stderr, "DIAG: server started\n" );

	/* initialize name for global port */
	client_enter(client_ports,anyuser_port,
			client_init("???",Dbg_Level_Max));

	/* loop, waiting for requests */
	err = server_loop(NULL,
			1, mach_diag_server, diag_notify, receive_port_set);
	if (err) {
		mach_error("server_loop", err);
		exit(1);
	}
}


mach_error_t
diag_mesg(port, level, mesg)
	mach_port_t		port;
	int		level;
	diag_mesg_t	mesg;
{
    	client_data_t c;
	char lvl[20];

	c = client_lookup(client_ports,port);
			
 	if (level >= c->debug_level) return(ERR_SUCCESS);

	sprintf( lvl, "%d", level );
	fprintf(stderr,"<%s (%s)>: %s%s",
		c->name, (level == Dbg_Level_Max) ? "Max" : lvl,
		mesg,
		((strlen(mesg) && mesg[strlen(mesg) - 1] == '\n') ? "" : "...\n")
		);

        return(ERR_SUCCESS);	
}

mach_error_t
diag_level(port, level)
	mach_port_t		port;
	int		level;
{
    client_data_t c;

    c = client_lookup(client_ports,port);

    c->debug_level = level;
   
    return(ERR_SUCCESS);
}

void
diag_notify(msg)
{
}

mach_error_t
diag_checkin(port,name,new_port)
mach_port_t	port;
diag_mesg_t name;
mach_port_t *new_port;
{
    mach_error_t err;
    client_data_t c;

    /*
     * allocate the port, but leave it in the DEFAULT set so the
     * server loop will pick it up.
     */

    err = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			     new_port);
    if (err) return(err);
    err = mach_port_move_member(mach_task_self(), *new_port, 
					receive_port_set);
    if (err) return(err);    
    
    c = client_init(name,Dbg_Level_Max);
    client_enter(client_ports,*new_port,c);
    return(ERR_SUCCESS);
}
