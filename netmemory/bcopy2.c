/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log$
 */
/*
 *	File:	bcopy.c
 *	Author:	Joseph S. Barrera III
 *
 *	Copyright (C) 1989, Joseph S. Barrera III
 *
 *	Test of byte-copying speed.
 *
 */

char src[102400];
char dst[102400];

/*
 *   Does 1000 copies of 100 KB each.
 *   Divide running time in seconds by 100 to get msec/KB.
 */
main()
{
    register unsigned long *l1;
    register unsigned long *l2;
    register i, j;

    for (i = 0; i < 1000; i++) {
	l1 = (unsigned long *) dst;
	l2 = (unsigned long *) src;
	for (j = 0; j < 1600; j++) {
	    l1[ 0] = l2[ 0];
	    l1[ 1] = l2[ 1];
	    l1[ 2] = l2[ 2];
	    l1[ 3] = l2[ 3];
	    l1[ 4] = l2[ 4];
	    l1[ 5] = l2[ 5];
	    l1[ 6] = l2[ 6];
	    l1[ 7] = l2[ 7];
	    l1[ 8] = l2[ 8];
	    l1[ 9] = l2[ 9];
	    l1[10] = l2[10];
	    l1[11] = l2[11];
	    l1[12] = l2[12];
	    l1[13] = l2[13];
	    l1[14] = l2[14];
	    l1[15] = l2[15];
	    l1 += 16;
	    l2 += 16;
	}
    }
}
