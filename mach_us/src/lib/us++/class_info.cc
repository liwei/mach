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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/class_info.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Class dictionary.
 *
 * HISTORY
 * $Log:	class_info.cc,v $
 * Revision 2.3  94/07/07  17:22:53  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:26:50  dpj
 * 	Removed "static" keyword in global definition.
 * 	[92/06/24  16:06:51  dpj]
 * 
 * 	First version.
 * 	[92/05/10  00:51:19  dpj]
 * 
 *
 */

#ifndef lint
char * class_info_rcsid = "$Header: class_info.cc,v 2.3 94/07/07 17:22:53 mrt Exp $";
#endif	lint

#include	<class_info_ifc.h>


extern "C" {
#include	<base.h>

static int _c_str_hash(char* str)
{
	register int ret = 0;
	register char c;
	while (c = *str++) {
		ret += c;
	}
	return (ret);
}

static boolean_t _c_str_cmp(char* str1, char* str2)
{
	register char c;
	while (c = *str1++) {
		if (c != *str2++) return (FALSE);
	}
	return (TRUE);	
}
}


/*
 * Global instance.
 */
/*static*/ class class_info*		class_info::GLOBAL = 0;


class_info::class_info()
{
	class_map = hash_init(_c_str_hash,_c_str_cmp,class_info_OBJ_HASH_SIZE);
}


class_info::~class_info()
{
	hash_free(class_map);
}


void* class_info::_lookup_class(char* name)
{
	return (void*)(hash_lookup(class_map, (hash_key_t)name));
}


boolean_t class_info::_insert_class(char* name, void* obj)
{
	boolean_t ret;

	if (ret = hash_enter(class_map, (hash_key_t)name,(hash_value_t)obj)) {
		return ret;
	} else {
		(void)hash_remove(class_map, (hash_key_t)name);
		return hash_enter(class_map, (hash_key_t)name,
							(hash_value_t)obj);
	}
}

