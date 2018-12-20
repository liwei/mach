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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/class_info_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Class dictionary.
 *
 * HISTORY
 * $Log:	class_info_ifc.h,v $
 * Revision 2.3  94/07/08  15:49:28  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:23:05  dpj
 * 	First version.
 * 	[92/05/10  00:15:25  dpj]
 * 
 */

#ifndef	_class_info_ifc_h
#define	_class_info_ifc_h

extern "C" {
#include	<hash.h>
}

static const int	class_info_OBJ_HASH_SIZE = 1024;

class class_info {
public:
	/*
	 * Global instance.
	 */
	static class class_info*	GLOBAL;

private:
	hash_table_t		class_map;

public:
				class_info();
	virtual			~class_info();

	void*			_lookup_class(char*);
	boolean_t		_insert_class(char*,void*);
};

#endif	_class_info_ifc_h
