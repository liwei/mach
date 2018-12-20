/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990,1989,1988,1987 Carnegie Mellon University
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
 *  hash.c
 *
 *  Generic hash table header declarations.
 *
 * HISTORY
 * $Log:	hash.h,v $
 * Revision 1.5  94/07/08  15:51:14  mrt
 * 	Updated copyright.
 * 
 * Revision 1.4  92/07/05  23:23:07  dpj
 * 	Added prototype for hash_free, needed for C++.
 * 	[92/05/10  00:16:13  dpj]
 * 
 * Revision 1.3  88/10/11  13:32:30  dorr
 * merge in rick's fast path macros.
 * 
 * Revision 1.2  88/08/10  13:32:23  sanzi
 * Added creation date, copyright message.
 * 
 *  05-May-88  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	Created.
 */
 
#ifndef	HASH_H
#define	HASH_H

#include <mach.h>

typedef long int hash_key_t;
typedef	unsigned int hash_value_t;

struct hash_entry {
    hash_key_t	key;
    hash_value_t value;
    struct hash_entry *next, *prev;
};

typedef struct hash_entry *hash_entry_t;

struct hash_table {
    int 	(*hashfun)();	/* hashing function for this table */
    boolean_t 	(*hashcompare)(); /* key comparison function */
    long int 	length;		/* current length of the table */
    struct hash_entry *table;	/* the array of header */
};

typedef struct hash_table *hash_table_t;

#define	HASH_NULL	((struct hash_entry *) 0)
#define	HASH_INVALID	(-1)

#define hash_first_entry(ht,key1)				\
	((ht)->table[(key1)&(ht)->length].next)

#define hash_first_entry_key(ht,key2)				\
	(((ht)->table[(key2)&(ht)->length].next)->key)

#define hash_first_entry_value(ht,key3)				\
	(((ht)->table[(key3)&(ht)->length].next)->value)

#define hash_fast_lookup(ht,key4) 			\
	((hash_first_entry_key(ht,key4) == key4) ?	\
		hash_first_entry_value(ht,key4) :	\
		hash_lookup(ht,key4))

extern hash_table_t hash_init (
#ifdef	DECLARE_PARAMS
	int (*hashfun)();
	boolean_t (*hashcompare)();
	unsigned int size;
#endif	DECLARE_PARAMS
	);

extern void hash_free (
#ifdef	DECLARE_PARAMS
	hash_table_t ht;
#endif	DECLARE_PARAMS
);

extern boolean_t hash_enter (
#ifdef	DECLARE_PARAMS
	hash_table_t ht;
	hash_key_t key;
	hash_value_t value;
#endif	DECLARE_PARAMS
	);

extern hash_value_t hash_lookup (
#ifdef	DECLARE_PARAMS
	hash_table_t ht;
	hash_key_t key;
#endif	DECLARE_PARAMS
	);

extern boolean_t hash_remove (
#ifdef	DECLARE_PARAMS
	hash_table_t ht;
	hash_key_t key;
#endif	DECLARE_PARAMS
);

extern boolean_t hash_apply (
#ifdef	DECLARE_PARAMS
	hash_table_t ht;
	(*fun)();
#endif	DECLARE_PARAMS
);
#endif	HASH_H
