/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991 Carnegie Mellon University
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
 * HISTORY
 * $Log:	cthread_data.c,v $
 * Revision 2.3  94/07/08  14:14:25  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  91/03/25  14:14:45  jjc
 * 	For compatibility with cthread_data:
 * 		1) Added routines, cthread_data and cthread_set_data,
 * 		   which use the new routines in here.
 * 		2) Defined CTHREAD_KEY_RESERVED as the key used to
 * 		   access cthread_data.
 * 		3) Changed the first free key from CTHREAD_KEY_NULL
 * 		   to CTHREAD_KEY_FIRST.
 * 	[91/03/18            jjc]
 * 	Made simple implementation from POSIX threads specification for
 * 	thread specific data.
 * 	[91/03/07            jjc]
 * 
 */
#include <stdio.h>
#include <cthreads.h>


#ifdef	CTHREAD_DATA
#define	CTHREAD_KEY_FIRST	(cthread_key_t)1	/* first free key */
#else	CTHREAD_DATA
#define	CTHREAD_KEY_FIRST	CTHREAD_KEY_NULL	/* first free key */
#endif	CTHREAD_DATA
#define	CTHREAD_KEY_MAX		(cthread_key_t)8	/* max. no. of keys */
#define	CTHREAD_KEY_NULL	(cthread_key_t)0

#ifdef	CTHREAD_DATA
/*
 *	Key reserved for cthread_data
 */
#define	CTHREAD_KEY_RESERVED	CTHREAD_KEY_NULL
#endif	CTHREAD_DATA


/* lock protecting key creation */
struct mutex	cthread_data_lock = MUTEX_INITIALIZER;

/* next free key */
cthread_key_t	cthread_key = CTHREAD_KEY_FIRST;


/*
 *	Create key to private data visible to all threads in task.
 *	Different threads may use same key, but the values bound to the key are
 *	maintained on a thread specific basis.
 *	Returns 0 if successful and returns -1 otherwise.
 */
cthread_keycreate(key)
cthread_key_t	*key;
{
	if (cthread_key >= CTHREAD_KEY_FIRST && cthread_key < CTHREAD_KEY_MAX) {
		mutex_lock((mutex_t)&cthread_data_lock);
		*key = cthread_key++;
		mutex_unlock((mutex_t)&cthread_data_lock);
		return(0);
	}
	else {	/* out of keys */
		*key = CTHREAD_KEY_INVALID;
		return(-1);
	}
}


/*
 *	Get private data associated with given key
 *	Returns 0 if successful and returns -1 if the key is invalid.
 *	If the calling thread doesn't have a value for the given key,
 *	the value returned is CTHREAD_DATA_VALUE_NULL.
 */
cthread_getspecific(key, value)
cthread_key_t	key;
any_t		*value;
{
	register cthread_t	self;
	register any_t		*thread_data;

	*value = CTHREAD_DATA_VALUE_NULL;
	if (key < CTHREAD_KEY_NULL || key >= cthread_key)
		return(-1);

	self = cthread_self();
	thread_data = (any_t *)(self->private_data);
	if (thread_data != (any_t *)0)
		*value = thread_data[key];

	return(0);
}


/*
 *	Set private data associated with given key
 *	Returns 0 if successful and returns -1 otherwise.
 */
cthread_setspecific(key, value)
cthread_key_t	key;
any_t		value;
{
	register int		i;
	register cthread_t	self;
	register any_t		*thread_data;

	if (key < CTHREAD_KEY_NULL || key >= cthread_key)
		return(-1);

	self = cthread_self();
	thread_data = (any_t *)(self->private_data);
	if (thread_data != (any_t *)0)
		thread_data[key] = value;
	else {
		/*
		 *	Allocate and initialize thread data table,
		 *	point cthread_data at it, and then set the
		 *	data for the given key with the given value.
		 */
		thread_data = (any_t *)malloc(CTHREAD_KEY_MAX * sizeof(any_t));
		if (thread_data == (any_t *)0) {
			fprintf(stderr,	"cthread_setspecific: malloc failed\n");
			return(-1);
		}
		self->private_data = (any_t)thread_data;

		for (i = 0; i < CTHREAD_KEY_MAX; i++)
			thread_data[i] = CTHREAD_DATA_VALUE_NULL;

		thread_data[key] = value;
	}
	return(0);
}


#ifdef	CTHREAD_DATA
/*
 *	Set thread specific "global" variable,
 *	using new POSIX routines.
 *	Crash and burn if the thread given isn't the calling thread.
 *	XXX For compatibility with old cthread_set_data() XXX
 */
cthread_set_data(t, x)
cthread_t	t;
any_t		x;
{
	register cthread_t	self;

	self = cthread_self();
	if (t == self)
		return(cthread_setspecific(CTHREAD_KEY_RESERVED, x));
	else {
		fprintf(stderr,
			"cthread_set_data: thread argument must be self\n");
		ASSERT(t == self);
	}
}


/*
 *	Get thread specific "global" variable,
 *	using new POSIX routines.
 *	Crash and burn if the thread given isn't the calling thread.
 *	XXX For compatibility with old cthread_data() XXX
 */
any_t
cthread_data(t)
cthread_t	t;
{
	register cthread_t	self;
	any_t			value;

	self = cthread_self();
	if (t == self) {
		(void)cthread_getspecific(CTHREAD_KEY_RESERVED, &value);
		return(value);
	}
	else {
		fprintf(stderr,"cthread_data: thread argument must be self\n");
		ASSERT(t == self);
	}
}
#endif	CTHREAD_DATA
