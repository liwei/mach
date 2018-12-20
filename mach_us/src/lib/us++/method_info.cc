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
 * File: us/lib/us++/method_info.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Keeper of all information for methods used by the RPC system.
 *
 * HISTORY
 * $Log:	method_info.cc,v $
 * Revision 2.4  94/07/15  15:17:08  mrt
 * 	Fixed so that the interruptable flag is parsed correctly.
 * 	[94/07/08  21:56:09  grm]
 * 
 * Revision 2.3  94/07/07  17:23:33  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  92/07/05  23:27:45  dpj
 * 	Removed "static" keyword for global definitions.
 * 	Pre-compute the size of arguments for input and output.
 * 	[92/06/24  16:26:14  dpj]
 * 
 * 	First version.
 * 	[92/05/10  00:54:30  dpj]
 * 
 *
 */

#include	<method_info_ifc.h>


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

#define		roundup_long(val) 	(((val)+3) & ~3)

/*
 * Global instance.
 */
/*static*/ class method_info*		method_info::GLOBAL = 0;


method_info::method_info()
{
	msgid_table = hash_init(0,0,method_info_OBJ_HASH_SIZE);
	name_table = hash_init(_c_str_hash, _c_str_cmp, 
						method_info_OBJ_HASH_SIZE);
}


method_info::~method_info()
{
	hash_free(msgid_table);
	hash_free(name_table);
}


void method_info::_define_method(
	mach_method_id_t	mid)
{
	if ((mid->method_name == NULL) ||
		(mid->method_args == NULL) ||
		(mid->arg_type.inited)) {
		return;
	}

	if (_parse_arg_info(mid->method_args,&mid->arg_type)) {
		if (mid->arg_type.msgid != -1) {
			if (! hash_enter(msgid_table,
					(hash_key_t)mid->arg_type.msgid,
					(hash_value_t)mid)) {
				ERROR((Diag,
				"RPC duplicate msgid(%d) for method \"%s\"",
				mid->arg_type.msgid,mid->method_name));
			}
		}
		hash_enter(name_table,(hash_key_t)mid->method_name, 
							(hash_value_t)mid);
	}

	mid->arg_type.inited = TRUE;
}


mach_method_id_t method_info::_lookup_msgid(mach_msg_id_t msgid)
{
	return (mach_method_id_t) hash_lookup(msgid_table,(hash_key_t)msgid);
}


mach_method_id_t method_info::_lookup_name(char* name)
{
	return (mach_method_id_t) hash_lookup(name_table,(hash_key_t)name);
}


boolean_t method_info::_parse_arg_info(
	char*		type_info,
	arg_type_t	tp)
{
	int		cur_arg = 0;
	boolean_t	is_simple;
	boolean_t	has_number;

	if (type_info == (char *)0) return (FALSE);
	if (*type_info == 0) return (FALSE);

	tp->msgid			= -1;
	tp->rpc 			= FALSE;
	tp->interruptible		= FALSE;
	tp->timeout			= MACH_MSG_TIMEOUT_NONE;
	tp->in_simple 			= TRUE;
	tp->out_simple 			= TRUE;
	tp->num_args			= 0;
	tp->size_args			= 0;
	tp->args[cur_arg].indirect_cnt 	= 0;
	tp->args[cur_arg].object	= FALSE;
	tp->args[cur_arg].input  	= FALSE;
	tp->args[cur_arg].copy  	= FALSE;
	tp->args[cur_arg].output 	= FALSE;
	tp->args[cur_arg].dealloc 	= FALSE;
	tp->args[cur_arg].variable 	= FALSE;
	tp->args[cur_arg].size 		= 32;
	tp->args[cur_arg].name 		= MACH_MSG_TYPE_INTEGER_32;
	tp->args[cur_arg].number 	= 1;
	tp->args[cur_arg].unit_number	= 1;
	is_simple = TRUE;
	has_number = FALSE;

	while (TRUE) {
	switch(*type_info) {
		case '*':
			if (tp->args[cur_arg].indirect_cnt >= 2) {
				ERROR((Diag,
				"RPC: Too much indirection in argument."));
				return(FALSE);
			} else {
				tp->args[cur_arg].indirect_cnt++;
			}
			type_info++;
			break;
		case 'I': 
			tp->args[cur_arg].input 	= TRUE;		break;
		case 'C': 
			tp->args[cur_arg].copy	 	= TRUE;		break;
		case 'O': 
			tp->args[cur_arg].output 	= TRUE;		break;
		case 'D':
		case 'd':
			tp->args[cur_arg].dealloc 	= TRUE;		break;
		case 'K':
		case 'k':
			_parse_rest(&type_info);
			while (*type_info == ' ') type_info++;
			if (*type_info != '<') {
				ERROR((Diag,
				"RPC: Invalid argument spec: missing msgid"));
				return(FALSE);
			}
			type_info++;
			tp->msgid = _parse_number(&type_info);
			break;
		case 'R':
		case 'r':
			tp->rpc			 	= TRUE;
			_parse_rest(&type_info);
			while (*type_info == ' ') type_info++;
			if ((*type_info == 'i') || (*type_info == 'I')) {
				tp->interruptible = TRUE;
				_parse_rest(&type_info);
				while (*type_info == ' ') type_info++;
			}
			if (*type_info == '<') {
				type_info++;
				tp->timeout = _parse_number(&type_info);
				_parse_rest(&type_info);
			}
			if ((*type_info == 'i') || (*type_info == 'I')) {
				tp->interruptible = TRUE;
				_parse_rest(&type_info);
				while (*type_info == ' ') type_info++;
			}
			continue;

		case 'c':
			tp->args[cur_arg].name 	= MACH_MSG_TYPE_CHAR;
			tp->args[cur_arg].size 	= 8;			break;
		case 'f':
			tp->args[cur_arg].name 	= MACH_MSG_TYPE_REAL;
			tp->args[cur_arg].size 	= 32;			break;
		case 'i':
			tp->args[cur_arg].name 	= MACH_MSG_TYPE_INTEGER_32;
			tp->args[cur_arg].size 	= 32;			break;
		case 'o':
			is_simple = FALSE;
			tp->args[cur_arg].object= TRUE;
			tp->args[cur_arg].name 	= MACH_MSG_TYPE_MAKE_SEND; /* XXX not here? Per object? */
			tp->args[cur_arg].size 	= 32;	
			_parse_rest(&type_info);
			while (*type_info == ' ') type_info++;
			if (*type_info != '<') {
				ERROR((Diag,"RPC: Missing class name"));
				return(FALSE);
			}
			type_info++;
			tp->args[cur_arg].class_name = 
						_parse_class(&type_info);
			tp->args[cur_arg].class_desc = 0;
			break;
		case 'w':
			tp->args[cur_arg].name 	= MACH_MSG_TYPE_INTEGER_16;
			tp->args[cur_arg].size 	= 16;			break;
		case 'u':
			tp->args[cur_arg].name 	= MACH_MSG_TYPE_UNSTRUCTURED;
			tp->args[cur_arg].size 	= 32;			break;
		case 'b':
			tp->args[cur_arg].name 	= MACH_MSG_TYPE_BYTE;
			tp->args[cur_arg].size 	= 8;			break;
		case '[':
			type_info++;
			if (has_number) {
				tp->args[cur_arg].unit_number =
						_parse_number(&type_info);
				tp->args[cur_arg].number *=
						tp->args[cur_arg].unit_number;
				break;
			}
			has_number = TRUE;
			if (*type_info == '*') {
				tp->args[cur_arg].variable = TRUE;
				type_info++;
				if (*type_info == ':') {
					type_info++;
					tp->args[cur_arg].number = 
						_parse_number(&type_info);
				}
				break;
			} else {
				tp->args[cur_arg].number = 
						_parse_number(&type_info);
			}	break;
		case 's':
			tp->args[cur_arg].indirect_cnt++;
			tp->args[cur_arg].name = MACH_MSG_TYPE_STRING;
			tp->args[cur_arg].size = 8;
			break;
		case 'p':
			is_simple = FALSE;
			tp->args[cur_arg].size = 32;
			tp->args[cur_arg].name =
				MACH_MSG_TYPE_MAKE_SEND;
			break;
		case '(':
			tp->args[cur_arg].name =
				_parse_port_right(&type_info);
			break;
		case 0:
			if (tp->args[cur_arg].input || 
						tp->args[cur_arg].output) {
				tp->args[cur_arg].info.msgt_name = 
					tp->args[cur_arg].name;
				tp->args[cur_arg].info.msgt_number = 
					tp->args[cur_arg].number *
					tp->args[cur_arg].unit_number;
				tp->args[cur_arg].info.msgt_size = 
					tp->args[cur_arg].size;
				tp->args[cur_arg].info.msgt_deallocate = 
					tp->args[cur_arg].dealloc;
				tp->args[cur_arg].info.msgt_inline = TRUE;
				tp->args[cur_arg].info.msgt_longform=FALSE;
				if (!is_simple) {
					if (tp->args[cur_arg].input)
						tp->in_simple = FALSE;
					if (tp->args[cur_arg].output)
						tp->out_simple = FALSE;
				}
				switch (tp->args[cur_arg].indirect_cnt) {
				case 0:
					tp->args[cur_arg].arg_size = roundup_long(
						((tp->args[cur_arg].number *
						tp->args[cur_arg].size)+7)>>3);
					tp->args[cur_arg].out_size = 0;
					break;
				case 1:
					tp->args[cur_arg].arg_size = sizeof(int *);
					tp->args[cur_arg].out_size = roundup_long(
						((tp->args[cur_arg].number *
						tp->args[cur_arg].size)+7)>>3);
					break;
				case 2:
					tp->args[cur_arg].arg_size = sizeof(int *);
					tp->args[cur_arg].out_size = sizeof(int *);
					break;
				}

				tp->num_args++;
				tp->size_args += tp->args[cur_arg].arg_size;
			}
			return (TRUE);
		case ' ':
		case ':':
			type_info++;	
			continue;
		case '%':
		case ';':
		case ',':
			tp->args[cur_arg].info.msgt_name = 
				tp->args[cur_arg].name;
			tp->args[cur_arg].info.msgt_number = 
				tp->args[cur_arg].number *
				tp->args[cur_arg].unit_number;
			tp->args[cur_arg].info.msgt_size = 
				tp->args[cur_arg].size;
			tp->args[cur_arg].info.msgt_deallocate = 
				tp->args[cur_arg].dealloc;
			tp->args[cur_arg].info.msgt_inline = TRUE;
			tp->args[cur_arg].info.msgt_longform=FALSE;
			if (!is_simple) {
				if (tp->args[cur_arg].input)
					tp->in_simple = FALSE;
				if (tp->args[cur_arg].output)
					tp->out_simple = FALSE;
			}
			switch (tp->args[cur_arg].indirect_cnt) {
			case 0:
				tp->args[cur_arg].arg_size = roundup_long(
					((tp->args[cur_arg].number *
					tp->args[cur_arg].size)+7)>>3);
				tp->args[cur_arg].out_size = 0;
				break;
			case 1:
				tp->args[cur_arg].arg_size = sizeof(int *);
				tp->args[cur_arg].out_size = roundup_long(
					((tp->args[cur_arg].number *
					tp->args[cur_arg].size)+7)>>3);
				break;
			case 2:
				tp->args[cur_arg].arg_size = sizeof(int *);
				tp->args[cur_arg].out_size = sizeof(int *);
				break;
			}
			tp->num_args++;
			tp->size_args += tp->args[cur_arg].arg_size;

			cur_arg++;

			tp->args[cur_arg].indirect_cnt 	= 0;
			tp->args[cur_arg].input  	= FALSE;
			tp->args[cur_arg].copy	  	= FALSE;
			tp->args[cur_arg].object	= FALSE;
			tp->args[cur_arg].output 	= FALSE;
			tp->args[cur_arg].dealloc	= FALSE;
			tp->args[cur_arg].variable 	= FALSE;
			tp->args[cur_arg].size 		= 32;
			tp->args[cur_arg].name 		= MACH_MSG_TYPE_INTEGER_32;
			tp->args[cur_arg].number 	= 1;
			tp->args[cur_arg].unit_number 	= 1;
			is_simple = TRUE;
			has_number = FALSE;

			type_info++;
			continue;
		}
		_parse_rest(&type_info);
	}
}


int method_info::_parse_number(char** numptr)
{
	register char 	c;
	int		result 	= 0;	

	while (TRUE) {
		c = **numptr;
		if (c >= '0' && c <= '9') {
			result = (result*10) + c - '0';
		} else {
			return (result);
		}
		*numptr = (*numptr) + 1;
	}
}

void method_info::_parse_rest(char** str)
{
	register char 	c;

	while (TRUE) {
		c = **str;
		switch (c) {
			case 0:
			case '%':
			case ',':
			case '[':
			case ';':
			case ' ':
			case ':':
			case '<':
			case '*':
			case '(':
				return;
			default:
				*str = (*str) + 1; break;
		}
	}
}

char* method_info::_parse_class(char** str)
{
	char class_str[128];
	int i = 0;

	while (TRUE) {
		class_str[i] = **str;		
		switch(class_str[i]) {
			case 0:
			case '%':
			case ';':
			case ',':
			case '>':
				{
					char * retstr;
					if (i == 0) {
						return ((char *)0);
					}
					retstr = (char *)malloc(i+1);
					class_str[i] = (char) 0;
					strcpy(retstr, class_str);
					*str++;
					return (retstr);
				}

			case '*': 
				*str = (*str) + 1;
				break;

			default:
				*str = (*str) + 1; i++;
				break;
		}
	}
}


int method_info::_parse_port_right(char** str)
{
#define PRS_MOVE_RECEIVE_STR "(MOVE_RECEIVE)"
#define PRS_MOVE_SEND_STR "(MOVE_SEND)"
#define PRS_MOVE_SEND_ONCE_STR "(MOVE_SEND_ONCE)"
#define PRS_COPY_SEND_STR "(COPY_SEND)"
#define PRS_MAKE_SEND_STR "(MAKE_SEND)"
#define PRS_MAKE_SEND_ONCE_STR "(MAKE_SEND_ONCE)"
#define PRS_RIGHT(right, str) (! strncmp(right, (*str), strlen(right)))
#define PRS_RIGHT_ADJUST(right, str) ((*(str)) += strlen(right))

	if (PRS_RIGHT(PRS_MAKE_SEND_STR, str)) {
		PRS_RIGHT_ADJUST(PRS_MAKE_SEND_STR, str);
		return(MACH_MSG_TYPE_MAKE_SEND);
	}

	if (PRS_RIGHT(PRS_MAKE_SEND_ONCE_STR, str)) {
		PRS_RIGHT_ADJUST(PRS_MAKE_SEND_ONCE_STR, str);
		return(MACH_MSG_TYPE_MAKE_SEND_ONCE);
	}

	if (PRS_RIGHT(PRS_MOVE_SEND_STR, str)) {
		PRS_RIGHT_ADJUST(PRS_MOVE_SEND_STR, str);
		return(MACH_MSG_TYPE_MOVE_SEND);
	}

	if (PRS_RIGHT(PRS_MOVE_SEND_ONCE_STR, str)) {
		PRS_RIGHT_ADJUST(PRS_MOVE_SEND_ONCE_STR, str);
		return(MACH_MSG_TYPE_MOVE_SEND_ONCE);
	}

	if (PRS_RIGHT(PRS_COPY_SEND_STR, str)) {
		PRS_RIGHT_ADJUST(PRS_COPY_SEND_STR, str);
		return(MACH_MSG_TYPE_COPY_SEND);
	}

	if (PRS_RIGHT(PRS_MOVE_RECEIVE_STR, str)) {
		PRS_RIGHT_ADJUST(PRS_MOVE_RECEIVE_STR, str);
		return(MACH_MSG_TYPE_MOVE_RECEIVE);
	}
	/* XXX if nothing, default to "make_send" */
	return(MACH_MSG_TYPE_MAKE_SEND);
}

