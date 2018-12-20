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
 *
 * HISTORY:
 * $Log:	class_map.cc,v $
 * Revision 2.3  94/07/07  17:22:57  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/11/06  13:45:32  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:26:40  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:23:11  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:34:35  pjg]
 * 
 *
 *
 * class_map.cc
 *
 * Initializes the class dictionary with C++ classes.
 */

#include <us_item_ifc.h>
#include <us_name_ifc.h>
#include <us_item_proxy_ifc.h>

extern "C" {
extern int str_hash(char*);
extern int str_cmp(char*, char*);
}

void _init_user_proxies(void);


static hash_table_t class_map = 0;

#define class_map_table_size 256

extern "C" void mach_object_init(void);

void _init_class_map(void)
{
	extern hash_table_t	mach_method_args_table;

//	DEBUG1((1), (0, "_init_class_map()\n"));
	if (mach_method_args_table == 0) {
		mach_object_init();
	}
	if (class_map == 0) {
		class_map = hash_init(str_hash, str_cmp,class_map_table_size);
//		_init_user_proxies();
	}
}

void* _lookup_class(char* name)
{
	if (class_map == (hash_table_t)0) {
		return (0);
	}
	else {
		return (void*)(hash_lookup(class_map, (hash_key_t)name));
	}
}

boolean_t _insert_class(char* name, void* obj)
{
	if (class_map == 0) {
		_init_class_map();
	} else {
		boolean_t ret;
		if (ret = hash_enter(class_map, (hash_key_t)name,
				     (hash_value_t)obj)) {
			return ret;
		} else {
			(void)hash_remove(class_map, (hash_key_t)name);
			return hash_enter(class_map, (hash_key_t)name, 
					  (hash_value_t)obj);
		}
	}
}

extern "C" int print_func(void*);

int print_func(void* p)
{
	usClass* c = (usClass*) p;
	fprintf (stdout, "class name=%s, remote class name=%s\n",
		 c->class_name(), c->remote_class_name());
}

void _print_map(void)
{
	if (class_map == 0) {
		fprintf (stderr, "Class map not initialized\n");
		return;
	} else {
		fprintf (stdout, "Class map BEGIN\n");
		hash_apply(class_map, print_func);
		fprintf (stdout, "Class map END\n");
	}
}
