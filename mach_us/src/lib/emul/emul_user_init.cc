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
 * File:  emul_usr_init.c
 */

/*
 * 
 * Purpose:  Program to create and become a "first" user process.
 * 
 * HISTORY: 
 * $Log:	emul_user_init.cc,v $
 * Revision 2.8  94/07/08  16:57:28  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.7  94/06/16  17:16:11  mrt
 * 	Changed default name server name from root_server to
 * 	pathname_server.
 * 	[94/06/02            mrt]
 * 
 * Revision 2.6  94/01/11  17:49:11  jms
 * 	Remove "ustty_proxy" special translation. Now default.
 * 	[94/01/09  18:49:40  jms]
 * 
 * Revision 2.5  92/07/05  23:25:41  dpj
 * 	tm_job_group_cat -> tm_tgrp_proxy
 * 	tm_task_cat -> tm_task_proxy
 * 	[92/06/24  14:40:47  jms]
 * 	Added include of usint_mf_proxy_ifc.
 * 	[92/06/24  15:55:36  dpj]
 * 
 * 	Eliminated diag_format().
 * 	[92/05/10  00:39:12  dpj]
 * 
 * Revision 2.4  92/03/05  14:56:07  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.
 * 	[92/02/26  17:25:52  jms]
 * 
 * Revision 2.3  91/12/20  17:43:25  jms
 * 	Update to new ns_set_system_prefix args. (from DPJ)
 * 	[91/12/20  14:49:22  jms]
 * 
 * Revision 2.2  91/11/06  11:33:16  jms
 * 	Upgraded to US41.
 * 	[91/09/26  19:39:08  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:31:41  pjg]
 * 
 * Revision 1.24  91/10/06  22:26:59  jjc
 * 	[91/10/01  17:28:41  jjc]
 * 
 *	Changed emul_user_init_io() and login() to only print out
 *	NS_ROOT_PORT and PUBLIC_ AS_PORT if debugging is on.
 *	Changed public_ns_port to public_config_port.
 *	Changed emul_user_init_io() to get port for Configuration Server
 *	instead of the nameserver.
 *	Added "-s" option to process_args() to specify name of Configuration
 *	Server.
 *	Modified emul_user_name_space_init() to get initial prefix table from
 *	the Configuration Server.
 *	Changed NS_ROOT_PORT to CONFIG_PORT.
 * 	[91/04/25            jjc]
 * 
 * Revision 1.23  91/07/01  14:07:03  jms
 * 	Remove some ns_reference stuff
 * 	[91/06/24  16:29:12  jms]
 * 
 * Revision 1.22  90/11/27  18:18:42  jms
 * 	Misc MACH3_US changes.  Minor.
 * 	[90/11/20  13:49:38  jms]
 * 
 * Revision 1.21  90/03/21  17:21:30  jms
 * 	Mods for useing the objectified Task Master and TM->emul signal support.
 * 	[90/03/16  16:39:54  jms]
 * 
 * Revision 1.20  90/01/02  21:55:18  dorr
 * 	get rid of the explicitly named token port.
 * 	identity is taken by an unforgeable ident object
 * 	now.
 * 
 * Revision 1.19.2.1  90/01/02  14:13:39  dorr
 * 	forget about renaming token port.
 * 
 * Revision 1.19  89/06/30  18:31:43  dpj
 * 	Added setup of an initial Current Working Directory, identical
 * 	to the root. Without it, getwd() does not work until the user has
 * 	done a first chdir().
 * 	[89/06/21  22:16:20  dpj]
 * 
 * 	Fixed one diagnostics message.
 * 	[89/05/31  17:39:00  dpj]
 * 
 * Revision 1.18  89/05/17  16:16:53  dorr
 * 	include file cataclysm
 * 
 * Revision 1.17  89/05/04  17:25:36  mbj
 * 	Merge up to U3.
 * 	[89/04/17  15:16:34  mbj]
 * 
 * Revision 1.16  89/04/04  18:19:51  dorr
 * 	pass out the root agent generated when creating the name space.
 * 	[89/04/02  20:45:56  dorr]
 * 
 * 	initialize name space from the user level.
 * 	[89/03/31  10:24:49  dorr]
 * 
 * Revision 1.15  89/03/21  14:13:52  mbj
 * 	Merge mbj_pgrp branch onto mainline.
 * 
 * Revision 1.14  89/03/17  12:24:27  sanzi
 * 	condense history.
 * 	[89/02/25  13:55:24  dorr]
 * 
 * Revision 1.16.1.1  89/05/15  12:07:40  dorr
 * 	return root agent from user_init()
 * 
 * Revision 1.15.2.1  89/03/31  16:04:06  mbj
 * 	Create mbj_signal branch
 * 
 * Revision 1.13.2.1  89/03/30  16:44:58  mbj
 * 	TM_NOTIFICATION_PORT changes and some cleanup.
 * 
 */

#include <emul_user_init_ifc.h>
#include <us_name_ifc.h>
#include <us_item_proxy_ifc.h>
#include <us_sys_proxy_ifc.h>

extern "C" {
#include <mach/mach_types.h>
#include <mach_error.h>
#include <debug.h>

#include <sys/syscall.h>
#include <syscall_val.h>
#include <auth_defs.h>

#include <ns_types.h>
#include <tm_types.h>

#include "us_ports.h"
}


/*
 * Currently given to us by the mach_init call
 */

extern 	mach_port_t name_server_port;

/*
 * Global variables holding ports which are inherited
 */

extern mach_port_t	public_as_port;
extern mach_port_t	public_config_port;
/*
 * Default server names
 */

char	* ufs_serv = "pathname_server";
char	* config_serv = "config_server";
char	* tm_serv = "Task_Master";
char	* las_serv = LAS_NAME;
char	* cas_serv = CAS_NAME;

/*
 * Default login info
 */
char *login_string = "ufsroot";
char *password_string = "ufsroot";

extern "C" char* index(char*, char*);

#define	STDIN	0
#define	STDOUT	1
#define	STDERR	2

mach_error_t
emul_user_init_io(void)
{
	mach_error_t	err;
	char		servername[1024];
	char		* hp;
	char		* config;

	/* change host:name into two separate args */
	strcpy( config = servername, config_serv );
	if ( hp = index( servername, ':' ) ) {
		config = hp;
		*config++ = '\0';
		hp = servername;
	} else
		hp = "";
	
	/*
	 *  set up our bootstrap port set.  
	 */
	
	/*  find the root name server and give it a well known name */
	err = netname_look_up(name_server_port, hp, config, &public_config_port);
	if (err) {
		ERROR((Diag,"Running without nameserver: error %#x %s\n", 
			   err, mach_error_string(err)));
		public_config_port = MACH_PORT_NULL;
	} else {
		err = mach_port_rename( mach_task_self(), public_config_port, CONFIG_PORT );
		if (err)
			ERROR((Diag,"mach_port_rename CONFIG_PORT %d -> %d: %#x %s\n",
				public_config_port, CONFIG_PORT,
				err, mach_error_string(err)));
		else
			ERROR((Diag,"CONFIG_PORT %d -> %d\n", public_config_port, CONFIG_PORT));
		public_config_port = CONFIG_PORT;
	}

	return ERR_SUCCESS;
}

mach_error_t
emul_user_init_tm(void)
{
	return ERR_SUCCESS;
}

#if (defined(MACH3_UNIX) || defined(MACH3_VUS))
extern int errno;

real_read(int fd, char* buf, int cnt)
{
	struct syscall_val	rv;
	char			* argv[3];
	
	argv[0] = (char *)fd;
	argv[1] = (char *)buf;
	argv[2] = (char *)cnt;

	if ( htg_unix_syscall( SYS_read, argv, &rv ) == 0 )
		return rv.rv_val1;
	else {
		errno = rv.rv_val1;
		return -1;
	}
}

real_write(int fd, char* buf, int cnt)
{
	struct syscall_val	rv;
	char			* argv[3];
	
	argv[0] = (char *)fd;
	argv[1] = (char *)buf;
	argv[2] = (char *)cnt;

	if ( htg_unix_syscall( SYS_write, argv, &rv ) == 0 )
		return rv.rv_val1;
	else {
		errno = rv.rv_val1;
		return -1;
	}
}

real_exit(int ec)
{
	struct syscall_val	rv;
	char			* argv[1];

	argv[0] = (char *)ec;

	if ( htg_unix_syscall( SYS_exit, argv, &rv ) == 0 )
		return rv.rv_val1;
	else {
		errno = rv.rv_val1;
		return -1;
	}

}
#endif

login(ns_identity_t* as_ident, ns_token_t* as_token)
{
	mach_error_t		ret;
	group_id_t		group_ids[128];
	group_id_t		user_id;
	group_id_list_t		out_group_ids;
	unsigned int		count;
	char			login[256];
	char			password[256];
	static char		* p_login = "Mach Login: ";
	static char		* p_password = "Password: ";
	
	*as_token = MACH_PORT_NULL;

	/*
	 * Get and rename public authentication port
	 */
	public_as_port = MACH_PORT_NULL;
	ret = netname_look_up(name_server_port, "", las_serv, &public_as_port);
	if (ret != ERR_SUCCESS) {
		ret = netname_look_up(name_server_port, "", cas_serv,
				      &public_as_port);
		if (ret != ERR_SUCCESS ) {
			ERROR((Diag, "as: running without authentication: %s", mach_error_string(ret)));
			return ret;
		}
	}
	if (public_as_port != MACH_PORT_NULL) {
		ret = mach_port_rename( mach_task_self(), public_as_port, PUBLIC_AS_PORT );
		if (ret) {
			ERROR((Diag,"mach_port_rename PUBLIC_AS_PORT %d -> %d: %#x %s\n",
				public_as_port, PUBLIC_AS_PORT,
				ret, mach_error_string(ret)));
			return ret;
		}
		else
			DEBUG0(1,(Diag,"PUBLIC_AS_PORT %d -> %d\n", public_as_port, PUBLIC_AS_PORT));
		public_as_port = PUBLIC_AS_PORT;
	}

	/* authenticate us */

	if (! login_string) {
#if MACH3_UNIX || MACH3_VUS
		login_string = login;
		real_write( STDOUT, p_login, strlen(p_login) );
		count = real_read( STDIN, login, sizeof(login)-1 );
		if (count > 0 )
			login[count-1] = '\0';
		else
			real_exit(0);
#endif MACH3_UNIX || MACH3_VUS
	}

	if (! password_string) {
#if MACH3_UNIX || MACH3_VUS
		password_string = password;
		real_write( STDOUT, p_password, strlen(p_password) );
		count = real_read( STDIN, password, sizeof(password)-1 );
		if (count > 0)
			password[count-1] = '\0';
		else
			real_exit(0);
#endif MACH3_UNIX || MACH3_VUS
	}

	ret = as_login(public_as_port, login_string, password_string, as_ident);
	if (ret != ERR_SUCCESS) {
		ERROR((Diag, "as: couldn't login ('%s','%s'): %s", 
			   login_string, password_string, mach_error_string(ret)));
		return ret;
	}

	count = 0;
	ret = as_create_token(public_as_port,*as_ident,group_ids,count,as_token);
	if (ret != ERR_SUCCESS) {
		ERROR((Diag, "as: couldn't create token: %s", mach_error_string(ret)));
		return ret;
	}

	ret = as_verify_token_ids(public_as_port,*as_token,&user_id,&out_group_ids,&count);
	if (ret != ERR_SUCCESS) {
		ERROR((Diag, "as: couldn't verify token ids: %s", mach_error_string(ret)));
		return ret;
	}

	ret = vm_deallocate(mach_task_self(),out_group_ids,count * sizeof(group_id_t));
	if (ret != ERR_SUCCESS) {
		ERROR((Diag, "as: couldn't vm_deallocate: %s", mach_error_string(ret)));
		return ret;
	}


	return ret;
}

mach_error_t
emul_user_init(ns_identity_t* as_ident, ns_token_t* as_token)
{
	mach_error_t		ret;

	/*
]	 * login as someone.  get back an object representing that user identity
	 */
	ret = login(as_ident,as_token);
	if (ret != ERR_SUCCESS) return ret;

	ret = emul_user_init_io();
	if (ret != ERR_SUCCESS) return ret;

	ret = emul_user_init_tm();
	return ret;
}


/*
 *  initialize a user name space, using the global token and root ports
 *  Must be called after emul_user_init_io
 *  XXX should take std_ident as an argument...
 */
emul_user_name_space_init(std_name** prefix_obj, ns_token_t token)
{
	mach_error_t	err;

	/*
	 * start up our prefix table object
	 */
	*prefix_obj = new std_name;

	/* somebody should have done gotten a token for us */
	if ( err = (*prefix_obj)->ns_set_token(token) ) {
		DEBUG0(1,(Diag, "ns_set_token failed... %x, %s\n", err,
				  mach_error_string(err)));
		return err;
	}

	/*
	 * create the initial (unauthenticated) sys agent
	 */
	usSys* sys_agent = new usSys_proxy;
	sys_agent->set_object_port(public_config_port);

	usItem			*server_objects[MAX_PREFIX];
	ns_name_t		*prefix_names;
	int			server_count;
	int			prefix_count;

	err = sys_agent->sys_get_prefix_table(server_objects, &server_count,
					&prefix_names, &prefix_count);
	if (err != ERR_SUCCESS) {
		DEBUG0(1,(Diag, "sys_get_prefix_table: %s\n",
				  mach_error_string(err)));
	}

	/*
	 * enter the prefix names and server agents into the prefix table
	 * (authenticating it)
	 */
	DEBUG1(TRUE,(Diag,"emul_init_namespace: prefix count %d\n",
			prefix_count));
	register i;
	for (i = 0; i < prefix_count; i++) {
		DEBUG1(TRUE,(Diag,"emul_init_namespace: prefix '%s' server 0x%x\n",
				prefix_names[i], server_objects[i]));
		err = (*prefix_obj)->ns_set_system_prefix(prefix_names[i],
					   server_objects[i],NST_DIRECTORY,
					   prefix_names[i]);
		if (err != ERR_SUCCESS) {
			DEBUG0(1,(Diag, "ns_set_prefix: couldn't set prefix %s: %x %s\n",
					  prefix_names[i],
					  err,
					  mach_error_string(err)));
		}
	}

	/*
	 * Setup an initial CWD.
	 */
	err = (*prefix_obj)->ns_set_user_prefix("","/");
	if (err != ERR_SUCCESS) {
		DEBUG0(1,(Diag, "ns_set_prefix: couldn't set CWD prefix %x %s\n",
				  err,
				  mach_error_string(err)));
		goto out;
	}

    out:
	return err;
}


fatal( char* str )
{
	fprintf(2, str);
#if MACH3_UNIX || MACH3_VUS
	real_exit( 1 );
#else
	task_terminate(mach_task_self());
#endif MACH3_UNIX || MACH3_VUS
}

process_args( int* p_argc, char*** p_argv, char* usage )
{
	int		argc = *p_argc;
	char		* * argv = *p_argv;
	static char	* sargv[100];
	int		sargc = 0;
	int		i;

	while( --argc ) {
		++argv;
		if ( (*argv)[0] == '-' ) {
			switch( (*argv)[1] ) {
			    case 'u':
				ufs_serv = (*argv)+2;
				break;
			    case 'a':
				las_serv = (*argv)+2;
				break;
			    case 'c':
				cas_serv = (*argv)+2;
				break;
			    case 's':
				config_serv = (*argv)+2;
				break;
			    case 'L':
				login_string = (*argv)+2;
				break;
			    case 'P':
				password_string = (*argv)+2;
				break;
			    default:
				sargv[sargc++] = (*argv);
				break;
			}
		} else {
			break;
		}
	}

	for(i=0; i<argc; i++) {
		sargv[sargc++] = *(argv++);
	}
	sargv[sargc] = (char *)0;

	*p_argc = sargc;
	*p_argv = sargv;
}

#include <us_name_proxy_ifc.h>
#include <mf_user_ifc.h>
#include <tm_task_proxy_ifc.h>
#include <tm_tgrp_proxy_ifc.h>
#include <us_byteio_proxy_ifc.h>

#include <usint_mf_proxy_ifc.h>


boolean_t _insert_class(char*, void*);
void*     _lookup_class(char*);

void _init_user_proxies(void)
{
	INSERT_CLASS_IN_MAP(usName_proxy, "++dir_proxy");
	INSERT_CLASS_IN_MAP(usName_proxy, "cat");
	INSERT_CLASS_IN_MAP(tm_task_proxy, "tm_task_proxy");
	INSERT_CLASS_IN_MAP(tm_tgrp_proxy, "tm_tgrp_proxy");
	INSERT_CLASS_IN_MAP(mf_user, "++mf_user_proxy");
	INSERT_CLASS_IN_MAP(mf_user, "mf_cat");
	INSERT_CLASS_IN_MAP(usByteIO_proxy, "usByteIO_proxy");
}

extern "C" {
void my_task_suspend(void);
}

void my_task_suspend(void)
{
//	task_suspend(mach_mach_task_self());
}
