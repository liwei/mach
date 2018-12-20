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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/local_include/dll.h,v $
 *
 * Purpose: doubly-linked list package. Created from sys/queue.h in the Mach
 * kernel sources.
 *
 * HISTORY: 
 * $Log:	dll.h,v $
 * Revision 1.4  94/07/08  18:42:49  mrt
 * 	Updated copyright
 * 
 * Revision 1.3  89/05/17  16:46:50  dorr
 * 	include file cataclysm
 * 
 * Revision 1.2  88/08/29  14:38:02  dpj
 * Disabled rcsid.
 * 
 * Revision 1.1  88/08/27  21:07:06  dpj
 * Initial revision
 * 
 *
 */

#ifndef	_DLL_
#define	_DLL_

#ifdef	0
#ifndef lint
char * dll_rcsid = "$Header: dll.h,v 1.4 94/07/08 18:42:49 mrt Exp $";
#endif	lint
#endif	0

/*
 *	List of abstract objects.  List is maintained
 *	within that object.
 *
 *	Supports fast removal from within the dll.
 *
 *	How to declare a dll of elements of type "foo_t":
 *		In the "*foo_t" type, you must have a field of
 *		type "dll_chain_t" to hold together this dll.
 *		There may be more than one chain through a
 *		"foo_t", for use by different dlls.
 *
 *		Declare the dll as a "dll_t" type.
 *
 *		Elements of the dll (of type "foo_t", that is)
 *		are referred to by reference, and cast to type
 *		"dll_entry_t" within this module.
 */

/*
 *	A generic doubly-linked list (dll).
 */

struct dll_entry {
	struct dll_entry	*next;		/* next element */
	struct dll_entry	*prev;		/* previous element */
};

typedef struct dll_entry	*dll_t;
typedef	struct dll_entry	dll_head_t;
typedef	struct dll_entry	dll_chain_t;
typedef	struct dll_entry	*dll_entry_t;

#define round_dll(size)	(((size)+7) & (~7))

/*
 *	Macro:		dll_init
 *	Function:
 *		Initialize the given dll.
 *	Header:
 *		void dll_init(q)
 *			dll_t		q;	/* MODIFIED *\
 */
#define	dll_init(q)	((q)->next = (q)->prev = q)

/*
 *	Macro:		dll_first
 *	Function:
 *		Returns the first entry in the dll,
 *	Header:
 *		dll_entry_t dll_first(q)
 *			dll_t	q;		/* IN *\
 */
#define	dll_first(q)	((q)->next)

/*
 *	Macro:		dll_next
 *	Header:
 *		dll_entry_t dll_next(qc)
 *			dll_t qc;
 */
#define	dll_next(qc)	((qc)->next)

/*
 *	Macro:		dll_end
 *	Header:
 *		boolean_t dll_end(q, qe)
 *			dll_t q;
 *			dll_entry_t qe;
 */
#define	dll_end(q, qe)	((q) == (qe))

#define	dll_empty(q)		dll_end((q), dll_first(q))

/*
 *	Macro:		dll_enter
 *	Header:
 *		void dll_enter(q, elt, type, field)
 *			dll_t q;
 *			<type> elt;
 *			<type> is what's in our dll
 *			<field> is the chain field in (*<type>)
 */
#define dll_enter(head, elt, type, field)			\
{ 								\
	if (dll_empty((head))) {				\
		(head)->next = (dll_entry_t) elt;		\
		(head)->prev = (dll_entry_t) elt;		\
		(elt)->field.next = head;			\
		(elt)->field.prev = head;			\
	}							\
	else {							\
		register dll_entry_t prev;			\
								\
		prev = (head)->prev;				\
		(elt)->field.prev = prev;			\
		(elt)->field.next = head;			\
		(head)->prev = (dll_entry_t)(elt);		\
		((type)prev)->field.next = (dll_entry_t)(elt);\
	}							\
}

/*
 *	Macro:		dll_field [internal use only]
 *	Function:
 *		Find the dll_chain_t (or dll_t) for the
 *		given element (thing) in the given dll (head)
 */
#define	dll_field(head, thing, type, field)			\
		(((head) == (thing)) ? (head) : &((type)(thing))->field)

/*
 *	Macro:		dll_remove
 *	Header:
 *		void dll_remove(q, qe, type, field)
 *			arguments as in dll_enter
 */
#define	dll_remove(head, elt, type, field)			\
{								\
	register dll_entry_t	next, prev;			\
								\
	next = (elt)->field.next;				\
	prev = (elt)->field.prev;				\
								\
	dll_field((head), next, type, field)->prev = prev;	\
	dll_field((head), prev, type, field)->next = next;	\
}

/*
 *	Macro:		dll_assign
 */
#define	dll_assign(to, from, type, field)			\
{								\
	((type)((from)->prev))->field.next = (to);		\
	((type)((from)->next))->field.prev = (to);		\
	*to = *from;						\
}

#define	dll_remove_first(h, e, t, f)				\
{								\
	e = (t) dll_first((h));				\
	dll_remove((h), (e), t, f);				\
}

/*
 *	Macro:		dll_enter_first
 *	Header:
 *		void dll_enter_first(q, elt, type, field)
 *			dll_t q;
 *			<type> elt;
 *			<type> is what's in our dll
 *			<field> is the chain field in (*<type>)
 */
#define dll_enter_first(head, elt, type, field)			\
{ 								\
	if (dll_empty((head))) {				\
		(head)->next = (dll_entry_t) elt;		\
		(head)->prev = (dll_entry_t) elt;		\
		(elt)->field.next = head;			\
		(elt)->field.prev = head;			\
	}							\
	else {							\
		register dll_entry_t next;			\
								\
		next = (head)->next;				\
		(elt)->field.prev = head;			\
		(elt)->field.next = next;			\
		(head)->next = (dll_entry_t)(elt);		\
		((type)next)->field.prev = (dll_entry_t)(elt);\
	}							\
}

/*
 *	Macro:		dll_last
 *	Function:
 *		Returns the last entry in the dll,
 *	Header:
 *		dll_entry_t dll_last(q)
 *			dll_t	q;		/* IN *\
 */
#define	dll_last(q)	((q)->prev)

/*
 *	Macro:		dll_prev
 *	Header:
 *		dll_entry_t dll_prev(qc)
 *			dll_t qc;
 */
#define	dll_prev(qc)	((qc)->prev)


#endif	_DLL_
