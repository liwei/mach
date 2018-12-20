/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992-1989 Carnegie Mellon University
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
 * dlong.h
 *
 * Double length long arithmetic type.
 *
 * Michael B. Jones  --  3-Feb-1989
 */

/*
 * HISTORY:
 * $Log:	dlong.h,v $
 * Revision 1.11  94/07/08  19:37:49  mrt
 * 	Updated copyright
 * 	[94/07/08            mrt]
 * 
 * Revision 1.10  91/11/06  14:13:14  jms
 * 	Got rid of the #if c_plusplus stuff. This file is now a
 * 	C file that can be included in a C++ program using the
 * 	extern "C" construct. The previous #if's didn't work correctly
 * 	if this file was included from a C file that was included from
 * 	a C++ file within a extern "C".
 * 	[91/09/26  18:26:50  pjg]
 * 
 * Revision 1.9  89/10/30  16:37:35  dpj
 * 	Added dlong_cmp* macros.
 * 	[89/10/27  19:32:19  dpj]
 * 
 * Revision 1.8  89/03/17  12:59:18  sanzi
 * 	Check into MOSERVER branch.
 * 	[89/02/27  16:49:17  mbj]
 * 
 * Revision 1.7  89/02/27  16:37:18  mbj
 * 	Added DLONG_TO_{U_,}{LONG,SHORT,INT} macros.  Added correct
 * 	SUB_{U_,}{LONG,INT}_FROM_DLONG macros.  Squeezed a tiny bit more
 * 	speed out of the existing ADD and SUB macros.
 * 
 * Revision 1.6  89/02/24  01:46:03  mbj
 * 	Added string conversion and output routines, plus DLONG_SGN macro.
 * 
 * Revision 1.5  89/02/21  11:59:42  sanzi
 * added macros for subtracting ints from dlongs.
 * 
 * Revision 1.4  89/02/20  03:48:42  mbj
 * Provide macros for adding integers to dlongs.  This this common combination
 * can be done more than twice as fast directly than by converting and adding.
 * 
 * Revision 1.3  89/02/20  02:15:33  mbj
 * Added type conversion macros.
 * 
 * Revision 1.2  89/02/17  15:40:51  mbj
 * Change type names dlong -> dlong_t, dlong_digit -> dlong_digit_t and
 * dlong_ac -> dlong_ac_t as per prevailing naming conventions.
 * 
 * Revision 1.1  89/02/17  11:48:28  mbj
 * Initial revision
 * 
 * 16-Feb-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Added macros for add, neg, sub and u_short_to_dlong.
 *
 * 13-Feb-89  Michael Jones (mbj) at Carnegie-Mellon University
 *	Converted all functions which returned dlongs to accept a *dlong
 *	result parameter, since the Vax and Sun C compilers return
 *	non-scalar values in static memory!
 */

#ifndef _DLONG_H_
#define _DLONG_H_ 1

#ifndef	C_ARG_DECLS
#define	C_ARG_DECLS(arglist)	()
#endif	not C_ARG_DECLS

#define C_TYPED_ARGLIST(arglist)
#define C_ARG_NAMES(argnames) argnames
#define C_ARG_TYPES(argtypes) argtypes


/*
 * Clever macro support
 */

#ifndef MACRO_BEGIN

#ifdef	lint
int	NEVER;
#else	lint
#define	NEVER 0
#endif	lint

#define	MACRO_BEGIN	do {
#define	MACRO_END	} while (NEVER)

#endif	MACRO_BEGIN

#include <stdio.h>

typedef unsigned short dlong_digit_t;	/* A single represented "digit" */
typedef unsigned long dlong_ac_t;	/* Accumulator type holding 2 digits */

#define dlong_base_bits (8 * sizeof(dlong_digit_t))
	/* Bits in representation of dlong_digit_t */
#define dlong_base ((dlong_ac_t) (1<<dlong_base_bits))
	/* Base of representation */

#define FRIEND extern		/* Friend functions become C externs */
typedef struct dlong_t {
	dlong_digit_t d[4];	/* Digits, from most significant to least */
} dlong_t;

/*
 * Type conversion operators
 */
    FRIEND void int_to_dlong C_ARG_DECLS((dlong_t *result, int x));
    FRIEND void long_to_dlong C_ARG_DECLS((dlong_t *result, long x));
    FRIEND void u_int_to_dlong C_ARG_DECLS((dlong_t *result, unsigned int x));
    FRIEND void u_long_to_dlong C_ARG_DECLS((dlong_t *result, unsigned long x));

    FRIEND int dlong_to_int C_ARG_DECLS((dlong_t x));
    FRIEND long dlong_to_long C_ARG_DECLS((dlong_t x));
    FRIEND unsigned int dlong_to_u_int C_ARG_DECLS((dlong_t x));
    FRIEND unsigned long dlong_to_u_long C_ARG_DECLS((dlong_t x));

/*
 * Actual operators
 */

    FRIEND void dlong_add  C_ARG_DECLS((dlong_t *result, dlong_t x, dlong_t y));
    FRIEND void dlong_sub  C_ARG_DECLS((dlong_t *result, dlong_t x, dlong_t y));
    FRIEND void dlong_neg  C_ARG_DECLS((dlong_t *result, dlong_t x));

    FRIEND void dlong_mul  C_ARG_DECLS((dlong_t *result, dlong_t x, dlong_t y));
    FRIEND void dlong_div  C_ARG_DECLS((dlong_t *result, dlong_t x, dlong_t y));
    FRIEND void dlong_rem  C_ARG_DECLS((dlong_t *result, dlong_t x, dlong_t y));

    FRIEND void dlong_divrem C_ARG_DECLS((dlong_t u, dlong_t v,
			dlong_t *quotient, dlong_t *remainder));

    FRIEND int dlong_cmp  C_ARG_DECLS((dlong_t x, dlong_t y));

    FRIEND void dlong_to_string C_ARG_DECLS((char *str, dlong_t x, int base));
    FRIEND char *string_to_dlong C_ARG_DECLS((dlong_t *x, char *str, int base));

/*
 * Macros for quick common dlong operations.  These macros accept the same
 * calling conventions as their routine counterparts, with the restriction
 * that all input values must actually be lvalues without side effects.
 *
 * For instance, one could add the two dlongs "a" and "b", storing the result
 * in dlong "c" with the macro call.
 *
 *	DLONG_ADD(&c, a, b)
 *
 * A dlong variable "z" can be set to zero with:
 *
 *	DLONG_ZERO(&z)
 */

/*
 * Macros for common dlong_t math operations.
 */

#define DLONG_ADD(result, x, y)						 \
MACRO_BEGIN								 \
    register dlong_ac_t ac;						 \
    (result)->d[3] = ac = (x).d[3] + (y).d[3];				 \
    (result)->d[2] = ac = (x).d[2] + (y).d[2] + (ac >> dlong_base_bits); \
    (result)->d[1] = ac = (x).d[1] + (y).d[1] + (ac >> dlong_base_bits); \
    (result)->d[0] =      (x).d[0] + (y).d[0] + (ac >> dlong_base_bits); \
MACRO_END

#define DLONG_NEG(result, x)						\
MACRO_BEGIN								\
    register dlong_ac_t ac;						\
    (result)->d[3] = ac = - (dlong_ac_t) (x).d[3];			\
    (result)->d[2] = ac = - (dlong_ac_t) (x).d[2]			\
			  - ((ac >> dlong_base_bits) ? 1 : 0);		\
    (result)->d[1] = ac = - (dlong_ac_t) (x).d[1]			\
			  - ((ac >> dlong_base_bits) ? 1 : 0);		\
    (result)->d[0] =      - (dlong_ac_t) (x).d[0]			\
    			  - ((ac >> dlong_base_bits) ? 1 : 0);		\
MACRO_END

#define DLONG_SUB(result, x, y)						\
MACRO_BEGIN								\
    register dlong_ac_t ac;						\
    (result)->d[3] = ac = (x).d[3] - (y).d[3];				\
    (result)->d[2] = ac = (x).d[2] - (y).d[2]				\
    				   - ((ac >> dlong_base_bits) ? 1 : 0);	\
    (result)->d[1] = ac = (x).d[1] - (y).d[1]				\
				   - ((ac >> dlong_base_bits) ? 1 : 0);	\
    (result)->d[0] =      (x).d[0] - (y).d[0]				\
				   - ((ac >> dlong_base_bits) ? 1 : 0);	\
MACRO_END

/*
 * Conversions from scalar types to dlongs.
 */

#define LONG_TO_DLONG(result, x)					\
MACRO_BEGIN								\
    if ((x) < 0)							\
	(result)->d[0] = (result)->d[1] = ~0;				\
    else								\
	(result)->d[0] = (result)->d[1] = 0;				\
    (result)->d[2] = ((x) >> dlong_base_bits);				\
    (result)->d[3] = (x);						\
MACRO_END

#define U_LONG_TO_DLONG(result, x)					\
MACRO_BEGIN								\
    (result)->d[0] = (result)->d[1] = 0;				\
    (result)->d[2] = ((x) >> dlong_base_bits);				\
    (result)->d[3] = (x);						\
MACRO_END

#define SHORT_TO_DLONG(result, x)					\
MACRO_BEGIN								\
    if ((x) < 0)							\
	(result)->d[0] = (result)->d[1] = (result)->d[2] = ~0;		\
    else								\
	(result)->d[0] = (result)->d[1] = (result)->d[2] = 0;		\
    (result)->d[3] = (x);						\
MACRO_END

#define U_SHORT_TO_DLONG(result, x)					\
MACRO_BEGIN								\
    (result)->d[0] = (result)->d[1] = (result)->d[2] = 0;		\
    (result)->d[3] = (x);						\
MACRO_END

#define INT_TO_DLONG(result, x)		LONG_TO_DLONG((result), (x))

#define U_INT_TO_DLONG(result, x)	U_LONG_TO_DLONG((result), (x))

#define DLONG_ZERO(result)		U_SHORT_TO_DLONG((result), 0)

/*
 * Conversions from dlongs to scalar types.  Overflow is not checked.
 */

#define DLONG_TO_LONG(x)						\
	((long) (((x).d[2] << dlong_base_bits) + (x).d[3]))

#define DLONG_TO_U_LONG(x)						\
	((unsigned long) (((x).d[2] << dlong_base_bits) + (x).d[3]))

#define DLONG_TO_SHORT(x)						\
	((short) (x).d[3])

#define DLONG_TO_U_SHORT(x)						\
	((unsigned short) (x).d[3])

#define DLONG_TO_INT(x)			((int) DLONG_TO_LONG(x))
#define DLONG_TO_U_INT(x)		((unsigned int) DLONG_TO_U_LONG(x))

/*
 * Common composite operations which are significantly faster when combined.
 */

#define ADD_LONG_TO_DLONG(result, x)					\
MACRO_BEGIN								\
    register dlong_ac_t ac;						\
									\
    (result)->d[3] = ac = (result)->d[3] +				\
	(dlong_ac_t) ((x) & (dlong_base-1));				\
									\
    (result)->d[2] = ac = (result)->d[2] +				\
	(dlong_ac_t) (((x) >> dlong_base_bits) & (dlong_base-1)) +	\
	(ac >> dlong_base_bits);					\
									\
    if ((x) >= 0) {							\
	if ((ac >> dlong_base_bits) != 0) {				\
	    (result)->d[1] = ac = (result)->d[1] + (dlong_ac_t) 1;	\
									\
	    if ((ac >> dlong_base_bits) != 0)				\
		(result)->d[0] += 1;					\
	}								\
    } else {								\
	if ((ac >> dlong_base_bits) == 0) {				\
	    (result)->d[1] = ac = (result)->d[1] + (dlong_base-1);	\
									\
	    if ((ac >> dlong_base_bits) == 0)				\
		(result)->d[0] += (dlong_base-1);			\
	}								\
    }									\
MACRO_END

#define ADD_U_LONG_TO_DLONG(result, x)					\
MACRO_BEGIN								\
    register dlong_ac_t ac;						\
									\
    (result)->d[3] = ac = (result)->d[3] +				\
	(dlong_ac_t) ((x) & (dlong_base-1));				\
									\
    (result)->d[2] = ac = (result)->d[2] +				\
	(dlong_ac_t) (((x) >> dlong_base_bits) & (dlong_base-1)) +	\
	(ac >> dlong_base_bits);					\
									\
    if ((ac >> dlong_base_bits) != 0) {					\
	(result)->d[1] = ac = (result)->d[1] + (dlong_ac_t) 1;		\
									\
	if ((ac >> dlong_base_bits) != 0)				\
	    (result)->d[0] += 1;					\
    }									\
MACRO_END

#define ADD_INT_TO_DLONG(result, x)	ADD_LONG_TO_DLONG((result), (x))

#define ADD_U_INT_TO_DLONG(result, x)	ADD_U_LONG_TO_DLONG((result), (x))


#define SUB_LONG_FROM_DLONG(result, x)					\
MACRO_BEGIN								\
    register dlong_ac_t ac;						\
									\
    (result)->d[3] = ac = (result)->d[3] -				\
	(dlong_ac_t) ((x) & (dlong_base-1));				\
									\
    (result)->d[2] = ac = (result)->d[2] -				\
	(dlong_ac_t) (((x) >> dlong_base_bits) & (dlong_base-1)) -	\
	((ac >> dlong_base_bits) ? 1 : 0);				\
									\
    if ((x) >= 0) {							\
	if ((ac >> dlong_base_bits) != 0) {				\
	    (result)->d[1] = ac = (result)->d[1] - (dlong_ac_t) 1;	\
									\
	    if ((ac >> dlong_base_bits) != 0)				\
		(result)->d[0] -= 1;					\
	}								\
    } else {								\
	if ((ac >> dlong_base_bits) == 0) {				\
	    (result)->d[1] = ac = (result)->d[1] - (dlong_base-1);	\
									\
	    if ((ac >> dlong_base_bits) == 0)				\
		(result)->d[0] -= (dlong_base-1);			\
	}								\
    }									\
MACRO_END

#define SUB_U_LONG_FROM_DLONG(result, x)				\
MACRO_BEGIN								\
    register dlong_ac_t ac;						\
									\
    (result)->d[3] = ac = (result)->d[3] -				\
	(dlong_ac_t) ((x) & (dlong_base-1));				\
									\
    (result)->d[2] = ac = (result)->d[2] -				\
	(dlong_ac_t) (((x) >> dlong_base_bits) & (dlong_base-1)) -	\
	((ac >> dlong_base_bits) ? 1 : 0);				\
									\
    if ((ac >> dlong_base_bits) != 0) {					\
	(result)->d[1] = ac = (result)->d[1] - (dlong_ac_t) 1;		\
								 	\
	if ((ac >> dlong_base_bits) != 0)				\
	    (result)->d[0] -= 1;					\
    }									\
MACRO_END

#define SUB_INT_FROM_DLONG(result, x)	SUB_LONG_FROM_DLONG((result), (x))

#define SUB_U_INT_FROM_DLONG(result, x)	SUB_U_LONG_FROM_DLONG((result), (x))


/*
 * Miscelaneous operations.
 */

#define DLONG_SGN(x) (							\
	((x).d[3] == 0 && (x).d[2] == 0 &&				\
	 (x).d[1] == 0 && (x).d[0] == 0) ? 0 :				\
	((x).d[0] & (1 << (dlong_base_bits-1))) ? -1 : 1 )

#define	dlong_cmpGTEQ(x,y)	(dlong_cmp(x,y) >= 0)
#define dlong_cmpGT(x,y)	(dlong_cmp(x,y) > 0)
#define	dlong_cmpEQ(x,y)	(dlong_cmp(x,y) == 0)

/*
 * Input/output operations.
 */

int dlong_print C_ARG_DECLS((FILE *fp, dlong_t x, int base));


#endif _DLONG_H_
