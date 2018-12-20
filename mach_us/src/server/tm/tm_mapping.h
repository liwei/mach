/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * tm_mapping.h
 *
 * Map ports to data structures for the Mach Task Master.
 *
 * Michael B. Jones
 *
 * 16-Mar-1990
 */

/*
 * HISTORY:
 * $Log:	tm_mapping.h,v $
 * Revision 2.4  94/07/13  17:33:19  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/03/05  15:13:09  jms
 * 	Switch mach_types.h => mach/mach_types.h
 * 	[92/02/26  19:32:44  jms]
 * 
 * Revision 2.2  90/03/21  17:29:30  jms
 * 	New .h file to access functionality of tm_mapping.c
 * 	[90/03/16  17:21:45  jms]
 * 
 *
 * Created from tm_mapping.c (auth mbj) 16-Mar-1990
 */

#ifndef	_tm_mapping_h
#define	_tm_mapping_h

#include <mach/mach_types.h>
#include <cthreads.h>
#include <hash.h>
#include "tm_types.h"

/*
 * Structure of a table of ports (not to be accessed directly by users)
 */
typedef struct port_mapping_table_st {
	struct mutex    tlock;
	hash_table_t	ports;
} *port_mapping_table_t;

/*
 * The kinds of (task specific) ports we can keep
 */
typedef enum {
    MACH_KERNEL_PORT,
    NOTIFICATION_PORT,
    EXCEPTION_PORT
} port_types_t;

/*
 * Create a new port table
 */
extern port_mapping_table_t new_port_mapping_table();

/*
 * Add a port mapping to a port_mapping_table
 * A port mapping is a port used as a key for the type of the port and the
 * associated task_id.
 */
extern void enter_port_mapping(/* port_mapping_table, port, port_type, task_id */);
/*
 *    port_mapping_table_t port_mapping_table;
 *    port_t port;
 *    port_types_t port_type;
 *    tm_task_id_t task_id;
 */

/*
 * Lookup the port type and task_id for a specific port
 */
extern boolean_t lookup_port_mapping(/* port_mapping_table, port, port_type, task_id */);
/*
 *    port_mapping_table_t port_mapping_table;
 *    port_t port;
 *    port_types_t *port_type;
 *    tm_task_id_t *task_id;
 */

/*
 * Remove an entry form the map.
 */
boolean_t remove_port_mapping(/* port_mapping_table, port */);
/*
 *    port_mapping_table_t port_mapping_table;
 *    port_t	port;
 */

#endif	_tm_mapping_h
