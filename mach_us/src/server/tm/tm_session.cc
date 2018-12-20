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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/tm/tm_session.cc,v $
 *
 * Purpose: Session object implementation.  Interface to the task master for 
 *		manipulating / querying specific sessions
 *
 * HISTORY:
 * $Log:	tm_session.cc,v $
 * Revision 2.3  94/07/13  17:33:29  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  94/05/17  13:36:04  jms
 * 	first checkin.
 * 	[94/05/11  15:05:03  modh]
 * 
 */

#include <tm_session_ifc.h>
#include <sys/param.h>

#include "tm_types.h"

/* The global access object for the task master */
extern int tm_debug;

tm_session::tm_session() {
  num_of_tasks = 0;
  mutex_init(&(this->lock));
}

mach_error_t tm_session::tasks_increment() {
  mutex_lock(&(this->lock));
  if (num_of_tasks == MAXUPRC) {
    DEBUG2(tm_debug, (0, "tm_session::tasks_increment: returning TM_TOO_MANY_TASKS_IN_SESSION, num_of_tasks = %d\n",
		      num_of_tasks));
    mutex_unlock(&(this->lock));
    return TM_TOO_MANY_TASKS_IN_SESSION;
  }
  num_of_tasks++;
  DEBUG2(tm_debug, (0, "tm_session::tasks_increment: num_of_tasks = %d\n",
		    num_of_tasks));
  mutex_unlock(&(this->lock));
  return ERR_SUCCESS;
}

mach_error_t tm_session::tasks_decrement() {
  mutex_lock(&(this->lock));
  if (num_of_tasks)
    num_of_tasks--;
  DEBUG2(tm_debug, (0, "tm_session::tasks_decrement: num_of_tasks = %d\n",
		    num_of_tasks));
  mutex_unlock(&(this->lock));
  return ERR_SUCCESS;
}
