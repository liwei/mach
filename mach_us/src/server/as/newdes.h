/*
 * newdms.h
 *
 * $Source: /afs/cs.cmu.edu/project/mach3-rcs/us/server/as/newdes.h,v $
 *
 * $Header: /afs/cs.cmu.edu/project/mach3-rcs/us/server/as/newdes.h,v 2.2 92/03/05 15:11:34 jms Exp $
 *
 */

/*
 * External definitions for newdes.c
 */

/*
 * HISTORY:
 * $Log:	newdes.h,v $
 * Revision 2.2  92/03/05  15:11:34  jms
 * 	Grab from sminclude subdir.
 * 	[92/02/26  19:22:31  jms]
 * 
 * 15-Jan-86  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#ifndef	_NEWDES_
#define	_NEWDES_

typedef unsigned char	block_t[8];

/*
 * Exported functions.
 */

extern void newdesencrypt();
/*
key_t		key;
block_t		text;
*/

extern void newdesdecrypt();
/*
key_t		key;
block_t		text;
*/

#endif	_NEWDES_
