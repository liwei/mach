/*
 * key_defs.h
 *
 * $Source: /afs/cs.cmu.edu/project/mach3-rcs/us/server/as/key_defs.h,v $
 *
 * $Header: /afs/cs.cmu.edu/project/mach3-rcs/us/server/as/key_defs.h,v 2.2 92/03/05 15:11:32 jms Exp $
 *
 */

/*
 * Definitions of encryption keys etc..
 */

/*
 * HISTORY:
 * $Log:	key_defs.h,v $
 * Revision 2.2  92/03/05  15:11:32  jms
 * 	Move from the sminclude subdir.
 * 	[92/02/26  19:20:19  jms]
 * 
 * 12-Apr-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added KEY_IS_NULL.
 *
 *  2-Feb-87  Robert Sansom (rds) at Carnegie Mellon University
 *	Added KEY_EQUAL.
 *
 *  5-Nov-86  Robert Sansom (rds) at Carnegie-Mellon University
 *	Started.
 *
 */

#ifndef	_KEY_DEFS_
#define	_KEY_DEFS_

/*
 * An encrytion key.
 */
typedef union {
    unsigned char	key_bytes[16];
    unsigned long	key_longs[4];
} key_t, *key_ptr_t;

#define KEY_EQUAL(key1, key2) 					\
    ((key1.key_longs[0] == key2.key_longs[0])			\
	&& (key1.key_longs[1] == key2.key_longs[1])		\
	&& (key1.key_longs[2] == key2.key_longs[2])		\
	&& (key1.key_longs[3] == key2.key_longs[3]))

#define KEY_IS_NULL(key)					\
    (((key).key_longs[0] == 0) && ((key).key_longs[1] == 0)	\
	&& ((key).key_longs[2] == 0) && ((key).key_longs[3] == 0))


/*
 * Structure used to transmit or store a token or a key.
 */
typedef union {
    key_t	si_key;
    key_t	si_token;
} secure_info_t, *secure_info_ptr_t;

/*
 * Security Level of ports and messages.
 */
#define PORT_NOT_SECURE		0
#define MESSAGE_NOT_SECURE	0

#endif	_KEY_DEFS_
