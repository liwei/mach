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

struct xxx {
    char xxx[102400];
};

struct xxx src;
struct xxx dst;

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
	dst = src;
    }
}
