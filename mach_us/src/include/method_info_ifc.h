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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/method_info_ifc.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Keeper of all information for methods used by the RPC system.
 *
 * HISTORY
 * $Log:	method_info_ifc.h,v $
 * Revision 2.3  94/07/08  15:51:23  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:23:12  dpj
 * 	Pre-compute the arg sizes for input and output.
 * 	[92/06/24  13:17:17  dpj]
 * 
 * 	First version.
 * 	[92/05/10  00:17:32  dpj]
 * 
 */

#ifndef	_method_info_ifc_h
#define	_method_info_ifc_h

extern "C" {
#include	<mach/message.h>
#include	<hash.h>
}

static const int	method_info_OBJ_HASH_SIZE = 1024;

#define	METHOD_ID_NULL	0

#define	MAXARGS		16	/* max args parse_arg_info can handle */

typedef struct arg_info {
	mach_msg_type_t	info;
	char*		class_name;
	class usClass*	class_desc;
	int		name;
	int		size;
	int		number;
	int		unit_number;
	int		indirect_cnt;
	boolean_t	input;
	boolean_t	output;
	boolean_t	variable;
	boolean_t	dealloc;
	boolean_t	copy;
	boolean_t	object;
	int		arg_size;
	int		out_size;
} arg_info_t;

typedef struct arg_type {
	boolean_t		inited;
	mach_msg_id_t		msgid;
	int			num_args;
	int			size_args;
	boolean_t		rpc;
	boolean_t		interruptible;
	int			timeout;
	boolean_t		in_simple;
	boolean_t		out_simple;
	arg_info_t		args[MAXARGS];
} * arg_type_t;

typedef struct method_descriptor {
	char*			method_name;
	char*			method_args;
	struct arg_type		arg_type;
} * mach_method_id_t;


class method_info {
public:
	/*
	 * Global instance.
	 */
	static class method_info*	GLOBAL;

private:
	hash_table_t		name_table;
	hash_table_t		msgid_table;

	boolean_t		_parse_arg_info(char*,arg_type_t);
	int			_parse_number(char**);
	void			_parse_rest(char**);
	char*			_parse_class(char**);
	int			_parse_port_right(char**);

public:
				method_info();
	virtual			~method_info();

	void			_define_method(mach_method_id_t);
	mach_method_id_t	_lookup_msgid(mach_msg_id_t);
	mach_method_id_t	_lookup_name(char*);
};

#endif	_method_info_ifc_h
