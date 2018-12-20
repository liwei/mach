/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988 Carnegie Mellon University
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
 * tm_mapping.c
 *
 * Map ports to data structures for the Mach Task Master.
 *
 * Michael B. Jones
 *
 * 25-Jul-1988
 */

/*
 * HISTORY:
 * $Log:	tm_mapping.c,v $
 * Revision 1.10  94/07/13  17:33:17  mrt
 * 	Updated copyright
 * 
 * Revision 1.9  92/03/05  15:13:06  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:32:21  jms]
 * 
 * Revision 1.8  91/11/13  17:21:17  dpj
 * 	Removed the deallocation of the port in remove_mapping. This is now taken
 * 	care of by tm_deregister_task() itself.
 * 	[91/07/13  22:26:57  dpj]
 * 
 * Revision 1.7  91/07/01  14:15:18  jms
 * 	Deallocate the task_port dead name when the task has died.
 * 	[91/06/16  21:10:51  dpj]
 * 
 * Revision 1.6  90/03/21  17:29:23  jms
 * 	Strip out everything but the port mapping stuff.  Add some locking and cleanup
 * 	modularity some.
 * 	[90/03/16  17:19:49  jms]
 * 
 * Revision 1.5  89/05/18  10:31:42  dorr
 * 	include file cataclysm
 * 
 * Revision 1.4  89/03/21  14:38:46  mbj
 * 	Merge mbj_pgrp branch onto mainline.
 * 
 * Revision 1.3  89/03/17  13:04:03  sanzi
 * 	Merge mainline onto MOSERVER.
 * 	[89/02/24  15:52:45  mbj]
 * 
 * Revision 1.2  89/02/17  18:05:05  mbj
 * 	Merged task_master -> tm name changes.
 * 
 * Revision 1.1.1.3  89/03/07  13:44:22  mbj
 * 	Remove unnecessary *_record_compare routines since the hash functions
 * 	now default to comparing the pointers for the key equality test.
 * 
 * Revision 1.1.1.2  89/03/02  10:41:50  mbj
 * 	Added job_group mapping routines.
 * 
 * Revision 1.1.1.1  89/02/16  23:55:15  mbj
 * 	Removed unused declarations.
 * 
 * 25-Jul-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

#include "tm_mapping.h"

extern char *malloc();

unsigned int	port_mapping_table_size = 128;
/*
 * Port table stuff
 */

typedef struct port_record {
    port_types_t	port_type;	/* The kind of port recorded here */
    tm_task_id_t	task_id;	/* taskid for the task */
} *port_record_t;

int port_record_hash(pr1)
port_record_t pr1;
{
    int c = (int) pr1;
    return( c ^ (port_mapping_table_size - 1));
}

port_mapping_table_t new_port_mapping_table()
{
    port_mapping_table_t pt = (port_mapping_table_t) malloc(sizeof(struct port_mapping_table_st));
    
    /* initialize port --> port_record hash table */

    mutex_init(&(pt->tlock));
    pt->ports = hash_init(
	port_record_hash, (boolean_t (*)()) 0, port_mapping_table_size);
    return(pt);
}

port_record_t new_port_record(port_type, task_id)
    port_types_t port_type;
    tm_task_id_t task_id;
{
    port_record_t pr = (port_record_t) malloc(sizeof(struct port_record));

    pr->port_type = port_type;
    pr->task_id = task_id;

    return(pr);
}

void enter_port_mapping(port_mapping_table, port, port_type, task_id)
    port_mapping_table_t port_mapping_table;
    mach_port_t port;
    port_types_t port_type;
    tm_task_id_t task_id;
{
    mutex_lock(&(port_mapping_table->tlock));
    hash_enter(port_mapping_table->ports, port, new_port_record(port_type, task_id));
    mutex_unlock(&(port_mapping_table->tlock));
}

boolean_t lookup_port_mapping(port_mapping_table, port, port_type, task_id)
    port_mapping_table_t port_mapping_table;
    mach_port_t port;
    port_types_t *port_type;
    tm_task_id_t *task_id;
{
    port_record_t port_ptr;

    mutex_lock(&(port_mapping_table->tlock));
    port_ptr = ((port_record_t) hash_lookup(port_mapping_table->ports, port));
    mutex_unlock(&(port_mapping_table->tlock));
    
    if (! port_ptr) return FALSE;

    *port_type = port_ptr->port_type;
    *task_id = port_ptr->task_id;
    return TRUE;
}

boolean_t remove_port_mapping(port_mapping_table, port)
    port_mapping_table_t port_mapping_table;
    mach_port_t	port;
{
    port_record_t port_ptr;

    mutex_lock(&(port_mapping_table->tlock));
    port_ptr = ((port_record_t) hash_lookup(port_mapping_table->ports, port));
    if (! port_ptr) {
	mutex_unlock(&(port_mapping_table->tlock));
	return FALSE;
    }
        
    if (! hash_remove(port_mapping_table->ports, port)) {
	mutex_unlock(&(port_mapping_table->tlock));
	return FALSE;
    }
    mutex_unlock(&(port_mapping_table->tlock));
    
    free(port_ptr);
#ifdef	nope
    /*
     * XXX XXX -- dpj 7/13/91
     * The kernel_port normally gets deallocated in
     * tm_deregister_task.tm_task_death.
     * What about other calling sequences???
     */
    (void) mach_port_deallocate(mach_task_self(), port);
#endif	nope
    return TRUE;
}
