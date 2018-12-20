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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/clone_master.cc,v $
 *
 * Purpose: Maintain a master list of objects to be cloned
 *
 * HISTORY: 
 * $Log:	clone_master.cc,v $
 * Revision 2.4  94/07/07  17:23:02  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:26:58  dpj
 * 	Introduced abstract class usClone to define cloning functions
 * 	previously defined in usTop.
 * 	[92/06/24  16:08:09  dpj]
 * 
 * Revision 2.2  91/11/06  13:45:37  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:27:17  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:23:32  pjg]
 * 
 * 	initial checkin
 * 	[89/12/19  17:11:57  dorr]
 * 
 * Revision 2.3  90/07/09  17:09:10  dorr
 * 	add some debug output.
 * 	[90/03/01  14:59:02  dorr]
 * 
 * 	fix clone_abort().  don't crap out on null objects.
 * 	[90/01/11  11:44:08  dorr]
 * 	No Further Changes
 * 	[90/07/06  17:37:11  jms]
 * 
 * Revision 2.2  90/01/02  22:09:28  dorr
 * 	Clone me!
 * 
 */

#ifndef lint
char * clone_master_rcsid = "$Header: clone_master.cc,v 2.4 94/07/07 17:23:02 mrt Exp $";
#endif	lint

#include <clone_master_ifc.h>

#include <clone_ifc.h>

/*
 * Debugging control.
 */
int	clone_master_debug = 1;


clone_master::clone_master()
{
	dll_init(&_Local(q));
}

clone_master::~clone_master()
{
	register dll_t		q;

	/* free all elements on the list */
	DEBUG0(clone_master_debug, (0, "~clone_master (TERMINATE)\n"));

	for(q=dll_first(&_Local(q)); !dll_end(&_Local(q),q); q = dll_next(q)) {
		clone_t		clone = (clone_t)q;
		dll_remove(&_Local(q), clone, clone_t, qp);

		mach_object_dereference(clone->obj);
		free(clone);
	}
}

/*
 * clone_init:  executed in the parent, does parent-side
 *	operations needed to initiate cloning
 */
mach_error_t clone_master::clone_init(mach_port_t child)
{
	register dll_t		q;
	mach_error_t		err;
	/* clone all elements on the list */

	for(q=dll_first(&_Local(q)); !dll_end(&_Local(q),q); q = dll_next(q)) {
		clone_t		clone = (clone_t)q;

		DEBUG1(clone_master_debug,(0,"clone_master->%s::clone_init\n", 
					   clone->obj->class_name()));

		err = clone->obj->clone_init(child);
		if (err && err != MACH_OBJECT_NO_SUCH_OPERATION) {
			dll_t	qp;

			/* on error, abort everybody you've cloned */
			for( qp = dll_first(&_Local(q)); qp != q; qp = dll_next(qp) ) {
				clone = (clone_t)qp;
				(void)clone->obj->clone_abort(child);
			}
			return err;
		}
	}
	return(ERR_SUCCESS);
}

/*
 * clone_complete:  executed in the child, does any child-side
 *	operations needed to finish cloning
 */
mach_error_t clone_master::clone_complete()
{
	mach_error_t		err, ret;
	register dll_t		q;

	ret = ERR_SUCCESS;

	/* finish cloning everyone on the list */

	for(q=dll_first(&_Local(q)); !dll_end(&_Local(q),q); q = dll_next(q)) {
		clone_t		clone = (clone_t)q;

		DEBUG1(clone_master_debug,(0, "clone_master->%s::_clone_complete\n", 
					   clone->obj->class_name()));

		err = clone->obj->clone_complete();
		if (err && err != MACH_OBJECT_NO_SUCH_OPERATION) {
			/* save the error and complete the rest of the list */
			DEBUG1(clone_master_debug,(0,
					      "%s::_clone_complete error %s\n",
					      clone->obj->class_name(),
					      mach_error_string(err)));
			ret = err;
		}
	}
	return(ret);
}


/*
 * list_add: add an element to the list
 */
mach_error_t clone_master::list_add(usClone* obj)
{
	clone_t			clone;

	if (obj == 0)
		return ERR_SUCCESS;

	clone = New(struct clone);
	clone->obj = obj;
	mach_object_reference(obj);

	DEBUG1(clone_master_debug,(0, "%s add to clone list\n", 
				   clone->obj->class_name()));

	dll_enter(&_Local(q), clone, clone_t, qp);
	return(ERR_SUCCESS);
}


/*
 * list_delete:  remove an element from the list 
 */
mach_error_t clone_master::list_delete(usClone* obj)
{
	dll_t			q;

	DEBUG1(clone_master_debug,(0, "clone DELETE %s\n", 
				   obj->class_name()));

	/* free this element */
	for(q=dll_first(&_Local(q)); !dll_end(&_Local(q),q); q = dll_next(q)) {
		clone_t		clone = (clone_t)q;

		if ( clone->obj == obj ) {
			dll_remove(&_Local(q), clone, clone_t, qp);
			mach_object_dereference(obj);
			free(clone);
			return(ERR_SUCCESS);
		}

	}

	/* XXX error code? */

	return(ERR_SUCCESS);
}
