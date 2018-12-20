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
 * File:  tm_session_ifc.h
 *
 * Purpose:  Mach Task Master task session object definition
 *
 * Manish Modh
 *
 * 28-Mar-1994
 */

/*
 * HISTORY:
 * $Log:	tm_session_ifc.h,v $
 * Revision 2.3  94/07/13  17:33:31  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  94/05/17  13:36:08  jms
 * 	first checkin.
 * 	[94/05/11  15:04:39  modh]
 * 
 */

#ifndef	_tm_session_ifc_h
#define	_tm_session_ifc_h

#include	<mach/mach_types.h>
#include	<vol_agency_ifc.h>


class tm_session: public vol_agency {
 private:

  /*
   * Instance variables
   */
  int           num_of_tasks;                /* Number of tasks in session */
  struct mutex  lock;                        /* lock for above var */

  /***********************
   *
   * EXTERNALLY AVAILABLE METHODS
   *
   ***********************/
 public:
 
  tm_session();                              /* Standard constructor */
 
  /*
   * Increment count of tasks.  If #of tasks is greater than MAXUPRC
   * (as defined in sys/param.h) then return TM_TOO_MANY_TASKS_IN_SESSION.
   */
  mach_error_t tasks_increment();

  /*
   * Decrement count of tasks 
   */
  mach_error_t tasks_decrement();
};

#endif _tm_session_ifc_h




