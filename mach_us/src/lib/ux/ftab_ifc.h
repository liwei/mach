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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/ftab_ifc.h,v $:
 *
 * Purpose: unix file table class
 *
 * HISTORY:
 * $Log:	ftab_ifc.h,v $
 * Revision 2.9  94/07/08  16:01:46  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.8  94/05/17  14:08:33  jms
 * 	Needed implementations of virtual methods in class ftab for 2.3.3 g++ -modh
 * 	[94/04/28  18:57:19  jms]
 * 
 * Revision 2.7.1.1  94/02/18  11:32:21  modh
 * 	Need to declare virtual functions in class frab for 2.3.3 g++
 * 
 * Revision 2.7  93/01/20  17:38:54  jms
 * 	Add SHARED_DATA_TIMING_EQUIVALENCE code to setup a shared memory space between
 * 	the task_master and a task.  Used to emulate timing of such sharing.
 * 	[93/01/18  17:20:51  jms]
 * 
 * Revision 2.6  92/07/05  23:32:36  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  17:30:27  dpj]
 * 
 * Revision 2.5  91/12/20  17:45:04  jms
 * 	Fix FD_MASK for Close-on-exec fix.
 * 	[91/12/20  16:15:41  jms]
 * 
 * Revision 2.4  91/11/06  14:10:34  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:11:40  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:05:06  pjg]
 * 
 * Revision 2.3  90/03/14  17:29:34  orr
 * 	fix a syntax error.
 * 	[90/03/14  16:58:04  orr]
 * 
 * Revision 2.2  89/07/19  11:07:52  dorr
 * 	Created.
 * 
 * Revision 2.1.1.1  89/06/21  15:49:31  dorr
 * 	created.
 * 
 */

#ifndef	_ftab_ifc_h
#define	_ftab_ifc_h

#include <top_ifc.h>
#include <clone_ifc.h>
#include <uxio_ifc.h>

extern "C" {
#include <cthreads.h>
#include <dll.h>
}

/*
 * Open file table.
 */
typedef struct fp {
	dll_chain_t	chain;
	int		ref_cnt;
	uxio*		obj;
#if SHARED_DATA_TIMING_EQUIVALENCE
	unsigned int		shared_status;
				/* Is this desc shared and how? */
#define FTSS_NULL	0
#define FTSS_INHERITED	1
#define FTSS_TOUCHED	2
#endif SHARED_DATA_TIMING_EQUIVALENCE
} *fp_t;

#define	FP_NULL	(fp_t)0

#define	MAX_OPEN_FILES	64


#define	FD_FLAG_NONE		(0)
#define	FD_MASK(x)		(1L << (x))

#define	FD_CLOSE_ON_EXEC	(0)


typedef struct ftab_s {
	fp_t			fp;
	unsigned int		flags;
} *ftab_t;

class ftab: public usClone {
	struct ftab_s	files[MAX_OPEN_FILES];
	struct fp	active_file_objects[MAX_OPEN_FILES];
	dll_head_t	free_file_objects;
	dll_head_t	in_use_objects;
	struct mutex	lock;
      public:
	ftab();
	virtual ~ftab();

	mach_error_t ftab_get_obj(int, uxio**);
	mach_error_t ftab_add_obj(uxio*, int*);
	mach_error_t ftab_exec();
	mach_error_t ftab_close(int);
	
	mach_error_t ftab_set_flag(int, int, boolean_t);
	mach_error_t ftab_get_flags(int, int*);

	mach_error_t ftab_cdup(int, int*, boolean_t);

	virtual mach_error_t clone_init(mach_port_t);
	virtual mach_error_t clone_abort(mach_port_t);
	virtual mach_error_t clone_complete();

        virtual mach_error_t ns_authenticate(ns_access_t,ns_token_t,usItem**);
        virtual mach_error_t ns_duplicate(ns_access_t, usItem**);
        virtual mach_error_t ns_get_attributes(ns_attr_t, int*);
        virtual mach_error_t ns_set_times(time_value_t, time_value_t);
        virtual mach_error_t ns_get_protection(ns_prot_t, int*);
        virtual mach_error_t ns_set_protection(ns_prot_t, int);
        virtual mach_error_t ns_get_privileged_id(int*);
        virtual mach_error_t ns_get_access(ns_access_t *, ns_cred_t, int *);
        virtual mach_error_t ns_get_manager(ns_access_t, usItem **);
     private:
	int falloc(int);
	mach_error_t close_internal(ftab_t);
};

#endif _ftab_ifc_h

