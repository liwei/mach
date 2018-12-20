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
 * Author: Daniel P. Julin
 *
 * Mach Network Service Interface.
 *
 * Standard types used in net requests.
 */
/*
 * HISTORY
 * $Log:	net_types.h,v $
 * Revision 2.3  94/07/08  15:51:26  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  91/05/05  19:23:44  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:43:40  dpj]
 * 
 * 	Modified to manipulate all abstract data types by reference.
 * 	Added NET_ADDR_PIPE.
 * 	[91/04/28  09:26:29  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:21:39  dpj]
 * 
 */

#ifndef	_net_types_h
#define	_net_types_h

#include	<macro_help.h>
#include	<mach/error.h>
#include	"net_error.h"

#ifndef	TRUE
#define	TRUE	1
#define	FALSE	0
#endif

typedef char		string_t[80];


/************************************************************************\
 *									*
 *		Simple data types for address manipulation		*
 *									*
\************************************************************************/
/*
 * XXX Should be abstracted inside the generic net_addr_t.
 */

typedef struct {
    unsigned short	high;
    unsigned short	mid;
    unsigned short	low;
} ethaddr_t;

typedef	struct {
  unsigned char		a, b, c, d;
} ipaddr_t;

extern ipaddr_t		NULL_IP_ADDR;

/************************************************************************\
 *									*
 *		Abstract data types representing arguments		*
 *									*
\************************************************************************/

/*
 * net_addr_t: abstract data type for network address.
 */
typedef unsigned int	net_addr_flavor_t;
#define	NET_ADDR_NULL	0
#define	NET_ADDR_INET	1
#define	NET_ADDR_PIPE	2
typedef struct {
	net_addr_flavor_t			flavor;
	union {
		struct {
			union {
				ipaddr_t	ip_struct;
				long		ip_long;
			} 		ipaddr;
			unsigned int	port;
		} inet;
		struct {
			char		str[8];
		} pipe;
	} un;
}			net_addr_t;
#define			net_addr_null_init(_na)			\
MACRO_BEGIN							\
	(_na)->flavor = NET_ADDR_NULL;				\
MACRO_END
#define			net_addr_null_init(_na)			\
MACRO_BEGIN							\
	(_na)->flavor = NET_ADDR_NULL;				\
MACRO_END
#define			net_addr_destroy(_na)		/* */
extern void		net_addr_copy();
extern boolean_t	net_addr_equal();
extern mach_error_t	net_addr_get_stringname();

/*
 * Internet-style addresses.
 */
#define			net_addr_inet_init(_na,_ipaddr,_port)	\
MACRO_BEGIN							\
	(_na)->flavor = NET_ADDR_INET;				\
	(_na)->un.inet.ipaddr.ip_struct = (_ipaddr);		\
	(_na)->un.inet.port = (_port);				\
MACRO_END
#define			net_addr_inet_init_default(_na)		\
MACRO_BEGIN							\
	(_na)->flavor = NET_ADDR_INET;				\
	(_na)->un.inet.ipaddr.ip_struct = NULL_IP_ADDR;		\
	(_na)->un.inet.port = 0;				\
MACRO_END
#define			net_addr_inet_p(_na) 			\
	(((_na)->flavor == NET_ADDR_INET) ? TRUE : FALSE)
#define			net_addr_inet_ipaddr_default_p(_na)	\
	((((_na)->flavor == NET_ADDR_INET) &&			\
		((_na)->un.inet.ipaddr.ip_long == 0)) ? TRUE : FALSE)
#define			net_addr_inet_port_default_p(_na)	\
	((((_na)->flavor == NET_ADDR_INET) &&			\
		((_na)->un.inet.port == 0)) ? TRUE : FALSE)
#define			net_addr_inet_get_ipaddr(_na)		\
	(net_addr_inet_p(_na) ? (_na)->un.inet.ipaddr.ip_struct : NULL_IP_ADDR)
#define			net_addr_inet_get_port(_na) 		\
	(net_addr_inet_p(_na) ? (_na)->un.inet.port : 0)
#define			net_addr_inet_set_ipaddr(_na,_ip) 	\
	(net_addr_inet_p(_na) ?					\
		((_na)->un.inet.ipaddr.ip_struct = (_ip), ERR_SUCCESS) :\
		NET_INVALID_ADDR_FLAVOR)
#define			net_addr_inet_set_port(_na,_pt) 	\
	(net_addr_inet_p(_na) ? ((_na)->un.inet.port = (_pt), ERR_SUCCESS) : \
		NET_INVALID_ADDR_FLAVOR)
extern mach_error_t	net_addr_inet_get_stringname();
extern mach_error_t	net_addr_inet_set_stringname();

/*
 * Pipe-style addresses.
 */
#define			net_addr_pipe_init(_na,_str)		\
MACRO_BEGIN							\
	(_na)->flavor = NET_ADDR_PIPE;				\
	bzero((_na)->un.pipe.str,8);				\
	strncpy((_na)->un.pipe.str,(_str),8);			\
MACRO_END
#define			net_addr_pipe_init_default(_na)		\
MACRO_BEGIN							\
	(_na)->flavor = NET_ADDR_PIPE;				\
	bcopy("DEFAULT*",(_na)->un.pipe.str,8);			\
MACRO_END
#define			net_addr_pipe_init_random(_na,_seed)	\
MACRO_BEGIN							\
	(_na)->flavor = NET_ADDR_PIPE;				\
	sprintf((_na)->un.pipe.str,"%08x",(_seed));		\
MACRO_END
#define			net_addr_pipe_p(_na) 			\
	(((_na)->flavor == NET_ADDR_PIPE) ? TRUE : FALSE)
#define			net_addr_pipe_default_p(_na)		\
	((((_na)->flavor == NET_ADDR_PIPE) &&			\
		((_na)->un.pipe.str[0] == 'D')) ? TRUE : FALSE)
extern mach_error_t	net_addr_pipe_get_stringname();
extern mach_error_t	net_addr_pipe_set_stringname();


/*
 * net_options_t: abstract data type for network service options.
 */
typedef	struct {
	int	dummy;
}			net_options_t;		/* no options yet */
#define			net_options_null_init(_no)		\
MACRO_BEGIN							\
		(_no)->dummy = 0;				\
MACRO_END
#define			net_options_destroy(_no)		/* */
#define			net_options_copy(_from,_to)		\
MACRO_BEGIN							\
		*(_to) = *(_from);				\
MACRO_END


/*
 * net_info_t: abstract data type for transport provider characteristics.
 */
typedef	struct { 
	int	dummy;
}			net_info_t;		/* no info yet */
#define			net_info_null_init(_ni)			\
MACRO_BEGIN							\
		(_ni)->dummy = 0;				\
MACRO_END
#define			net_info_destroy(_ni)		/* */
#define			net_info_copy(_from,_to)		\
MACRO_BEGIN							\
		*(_to) = *(_from);				\
MACRO_END


/************************************************************************\
 *									*
 *		States for a connection-oriented endpoint		*
 *									*
\************************************************************************/

typedef	int		net_costate_t;
#define	NET_COSTATE_CONNECTING		1
#define	NET_COSTATE_CONNECTED		2
#define	NET_COSTATE_DISCONNECTED	3

#endif	_net_types_h
