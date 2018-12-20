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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/net_types.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Support functions for standard net data types.
 *
 * HISTORY
 * $Log:	net_types.cc,v $
 * Revision 2.4  94/07/07  17:23:52  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  92/07/05  23:28:11  dpj
 * 	Added a few extern declarations to reduce compiler warnings.
 * 	[92/06/24  16:59:36  dpj]
 * 
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:11:18  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:22  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:59:13  pjg]
 * 
 * Revision 2.2  91/05/05  19:27:03  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:38  dpj]
 * 
 * 	Modified to manipulate all abstract data types by reference.
 * 	Added NET_ADDR_PIPE.
 * 	[91/04/28  10:16:34  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:29:49  dpj]
 * 
 *
 */

#ifndef lint
char * net_types_rcsid = "$Header: net_types.cc,v 2.4 94/07/07 17:23:52 mrt Exp $";
#endif	lint

extern "C" {
#include	<base.h>
#include	"net_types.h"
#include	"us_error.h"
#include	"net_error.h"

void net_addr_copy(net_addr_t *, net_addr_t *);
boolean_t net_addr_equal(net_addr_t *, net_addr_t *);
mach_error_t net_addr_get_stringname(net_addr_t	*, char *, int);
mach_error_t net_addr_inet_get_stringname(net_addr_t *, char *, int);
mach_error_t net_addr_inet_set_stringname(net_addr_t *, char *);
mach_error_t net_addr_pipe_get_stringname(net_addr_t *, char *, int);
mach_error_t net_addr_pipe_set_stringname(net_addr_t *, char *);

void sprintf();
int sscanf();
int ntohs();
int htons();
}

ipaddr_t		NULL_IP_ADDR = {0,0,0,0};

/*
 * net_addr_t: abstract data type for network address.
 */

void net_addr_copy(net_addr_t *from, net_addr_t *to)
{
	net_addr_destroy(to);

	to->flavor = from->flavor;

	switch (from->flavor) {
		case NET_ADDR_NULL:
			return;
		case NET_ADDR_INET:
			to->un.inet.ipaddr.ip_long =
						from->un.inet.ipaddr.ip_long;
			to->un.inet.port = from->un.inet.port;
			return;
		case NET_ADDR_PIPE:
			bcopy(from->un.pipe.str,to->un.pipe.str,8);
			return;
		default:
			return;
	}
}

boolean_t net_addr_equal(net_addr_t *na1, net_addr_t *na2)
{
	if (na1->flavor != na2->flavor) return(FALSE);

	switch (na1->flavor) {
		case NET_ADDR_NULL:
			return(TRUE);
		case NET_ADDR_INET:
			if ((na1->un.inet.ipaddr.ip_long
					== na2->un.inet.ipaddr.ip_long)
				&& (na1->un.inet.port == na2->un.inet.port)) {
				return(TRUE);
			} else {
				return(FALSE);
			}
		case NET_ADDR_PIPE:
			if ((na1->un.pipe.str[0] == 'D')
				&& (na2->un.pipe.str[0] == 'D')) {
				return(TRUE);
			} else {
				return(
				! bcmp(na1->un.pipe.str,na2->un.pipe.str,8));
			}
		default:
			return(FALSE);
	}
}


mach_error_t net_addr_get_stringname(net_addr_t	*na, char *buf, int len)
{
	switch (na->flavor) {
		case NET_ADDR_NULL:
			if (len < 5) return(US_INVALID_BUFFER_SIZE);
			strcpy(buf,"NULL");
			return(ERR_SUCCESS);
		case NET_ADDR_INET:
			return(net_addr_inet_get_stringname(na,buf,len));
		case NET_ADDR_PIPE:
			return(net_addr_pipe_get_stringname(na,buf,len));
		default:
			return(NET_INVALID_ADDR_FLAVOR);
	}
}


mach_error_t net_addr_inet_get_stringname(net_addr_t *na, char *buf, int len)
{
	if (!net_addr_inet_p(na)) return(NET_INVALID_ADDR_FLAVOR);

	if (len < 22) return(US_INVALID_BUFFER_SIZE);

	sprintf(buf,"%d.%d.%d.%d,%d",
		na->un.inet.ipaddr.ip_struct.a,
		na->un.inet.ipaddr.ip_struct.b,
		na->un.inet.ipaddr.ip_struct.c,
		na->un.inet.ipaddr.ip_struct.d,
		ntohs(na->un.inet.port));

	return(ERR_SUCCESS);
}


mach_error_t net_addr_inet_set_stringname(net_addr_t *na, char *buf)
{
	unsigned int		a,b,c,d,port,count;

	if (!net_addr_inet_p(na)) return(NET_INVALID_ADDR_FLAVOR);

	count = sscanf(buf,"%d.%d.%d.%d,%d",&a,&b,&c,&d,&port);

	if ((count != 5) || (a > 255) || (b > 255) || (c > 255) ||
					(d > 255) || (port > 65535)) {
		return(NET_INVALID_ADDR_VALUE);
	}

	na->un.inet.ipaddr.ip_struct.a = a;
	na->un.inet.ipaddr.ip_struct.b = b;
	na->un.inet.ipaddr.ip_struct.c = c;
	na->un.inet.ipaddr.ip_struct.d = d;
	na->un.inet.port = htons(port);

	return(ERR_SUCCESS);
}


mach_error_t net_addr_pipe_get_stringname(net_addr_t *na, char *buf, int len)
{
	if (!net_addr_pipe_p(na)) return(NET_INVALID_ADDR_FLAVOR);

	if (len < 11) return(US_INVALID_BUFFER_SIZE);

	sprintf(buf,"PN%08s",na->un.pipe.str);
	buf[10] = '\0';

	return(ERR_SUCCESS);
}


mach_error_t net_addr_pipe_set_stringname(net_addr_t *na, char *buf)
{
	unsigned int	num;

	if (!net_addr_pipe_p(na)) return(NET_INVALID_ADDR_FLAVOR);

	if ((strlen(buf) != 10) || (sscanf(buf,"PN%08x",&num) != 1)) {
		return(NET_INVALID_ADDR_VALUE);
	}

	bcopy(&buf[2],na->un.pipe.str,8);

	return(ERR_SUCCESS);
}


