/* 
 * Mach Operating System
 * Copyright (c) 1994-1987 Carnegie Mellon University
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
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * us_ps.cc
 *
 *
 */

/*
 * Mach User Library.
 *
 * Purpose: Administration program for file systems.
 */

/*
 * 
 * HISTORY: 
 * $Log:	us_ps.cc,v $
 * Revision 2.2  94/10/27  12:01:14  jms
 * 	Program for doing a list of the exec strings of all processes in the system.
 * 	[94/10/26  14:31:34  jms]
 * 
 */


#include <std_name_ifc.h>
#include <us_name_proxy_ifc.h>
#include <us_item_proxy_ifc.h>
#include <us_byteio_ifc.h>
#include <diag_ifc.h>
#include <us_tm_root_ifc.h>
#include <us_tm_task_ifc.h>
#include <us_tm_tgrp_ifc.h>

extern "C" {

#include	<base.h>
#include	<stdio.h>
#include	<ci.h>
#include	<mach.h>
#include	<cthreads.h>
#include	<servers/netname.h>
#include	<sys/time.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/file.h>

#include	"io_types.h"
#include	"ns_types.h"
#include	"auth_defs.h"


extern int	us_debug_level;
extern int	us_debug_force;
extern int	mach_object_debug_level;

extern int	strarg();
extern int	nxtarg();
extern int	getpath();
extern int	as_login();
extern int	as_create_token();
extern int	as_verify_token_ids();
extern int	as_translate_group_id();
extern int	as_translate_group_name();
extern int	as_delete_token();
extern int	as_logout();
extern int	getpass();
extern char*	path();
extern int	open();
extern void	perror();
extern int	close();
extern int	read();
extern int	write();
extern int	chdir();
extern int	exit();
extern int	ci();

}

#define	BUFSIZE		1024

#define	BRK		" ;\n"


int resolve_mode = NSF_FOLLOW | NSF_MOUNT;
int io_mode = IOM_WAIT | IOM_TRUNCATE;

char	*program;
#define USAGE()	fprintf(stderr,"Usage: %s [-d<debug-level>] [-h<hostname>] [<root-name>]\n",program)

#define	DFT_PROT_LEN	3
int 			dft_prot_data[NS_PROT_LEN(DFT_PROT_LEN)] = {
	NS_PROT_VERSION, 0, DFT_PROT_LEN,
	0, NSR_ALL,
};
int			dft_protlen = NS_PROT_LEN(DFT_PROT_LEN);
ns_prot_t		dft_prot = (ns_prot_t)dft_prot_data;

/*
 * Root for the current connected file system.
 */
usItem* us_ps_root_agent = 0;

/*
 * Prefix/naming object
 */
std_name* prefix_obj;


/*
 * Authentication state.
 */
mach_port_t		as_port;
mach_port_t		as_private_port = MACH_PORT_NULL;
ns_token_t		as_token = MACH_PORT_NULL;

mach_port_t		dbg_port;

/*
 * Translate a principal id into a string.
 */
mach_error_t id_to_str(group_id_t id, char *str)
{
	mach_error_t		ret;
	group_id_t		owner_id;
	group_type_t		group_type;
	full_name_t		full_name;

	ret = as_translate_group_id(as_port,id,&owner_id,str,
					&group_type,full_name);

	if (ret != KERN_SUCCESS) {
		sprintf(str,"[%d]",id);
	}

	return(ret);
}


/*
 * Translate a principal name into an id.
 */
mach_error_t str_to_id(char *str, group_id_t *id)
{
	mach_error_t		ret;
	group_id_t		owner_id;
	group_type_t		group_type;
	full_name_t		full_name;

	ret = as_translate_group_name(as_port,str,&owner_id,id,
					&group_type,full_name);

	return(ret);
}



void login_cmd(char *arglist)
{
	char			*p;
	group_name_t		user_name;
	pass_word_t		*pass_word;
	mach_error_t		ret;
	group_id_t		group_ids[128];
	group_id_t		user_id;
	group_id_list_t		out_group_ids;
	unsigned int		count;
	int			i;

	if (us_ps_root_agent == NULL) {
		fprintf(stderr,"Not connected\n");
		return;
	}

	p = arglist;
	(void)strarg(&p, BRK, "User Name:", "", user_name);
	pass_word = (pass_word_t *)nxtarg(&p,BRK);
	if ((pass_word == NULL) || ((*((char *)pass_word)) == '\0')) {
		pass_word = (pass_word_t *)getpass("Password:");
	}

	ret = as_login(as_port, user_name, pass_word, &as_private_port);
	if (ret != AS_SUCCESS) {
		mach_error("as_login",ret);
		return;
	}

	count = 0;
	ret = as_create_token(as_port,as_private_port,group_ids,count,&as_token);
	if (ret != AS_SUCCESS) {
		mach_error("as_create_token",ret);
		return;
	}

	ret = as_verify_token_ids(as_port,as_token,&user_id,&out_group_ids,&count);
	if (ret != AS_SUCCESS) {
		mach_error("as_verify_token",ret);
		return;
	}

	/*
	 * create a default prot entry
	 */
	dft_prot->head.version = NS_PROT_VERSION;
	dft_prot->head.generation = 0;
	dft_prot->head.acl_len = DFT_PROT_LEN;

	dft_prot->acl[0].authid = user_id;
	dft_prot->acl[0].rights = NSR_ALL;
	dft_prot->acl[1].authid = out_group_ids[0];
	dft_prot->acl[1].rights = NSR_ALL & ~NSR_ADMIN;
	dft_prot->acl[2].authid = 0;
	dft_prot->acl[2].rights = NSR_ALL & ~NSR_ADMIN;

	/*
	 * gen up a naming/prefix object
	 */

	ret = prefix_obj->ns_set_token(as_token);
	if (ret != NS_SUCCESS) {
		mach_error("ns_set_token",ret);
		return;
	}

#if NOISY
	fprintf(stdout,"  User %d logged-in with groups:",user_id);
	for (i = 0; i < count; i++) {
		fprintf(stdout,"  %4d",out_group_ids[i]);
	}
	fprintf(stdout,"\n");
#endif NOISY

	ret = vm_deallocate(mach_task_self(),out_group_ids,count * sizeof(group_id_t));
	if (ret != KERN_SUCCESS) {
		mach_error("vm_deallocate",ret);
		return;
	}

	ret = prefix_obj->ns_set_system_prefix("/",us_ps_root_agent,
							NST_DIRECTORY,"/");
	if (ret != NS_SUCCESS) {
		mach_error("ns_set_system_prefix (root)",ret);
		return;
	}

	ret = prefix_obj->ns_set_user_prefix("","/");
	if (ret != NS_SUCCESS) {
		mach_error("ns_set_user_prefix (CWD)",ret);
		return;
	}

#if NOISY
	fprintf(stdout,"  CWD = /\n");
#endif NOISY
	fflush(stdout);
	return;
}


void logout_cmd(void)
{
	mach_error_t		ret;

	mach_object_dereference(prefix_obj);
	prefix_obj = NULL;

	ret = as_delete_token(as_port,as_private_port,as_token);
	if (ret != AS_SUCCESS) {
		mach_error("as_delete_token",ret);
		return;
	}

	ret = as_logout(as_port,as_private_port);
	if (ret != AS_SUCCESS) {
		mach_error("as_delete_token",ret);
		return;
	}

	as_token = MACH_PORT_NULL;
	as_private_port = MACH_PORT_NULL;

	fflush(stdout);
	return;
}



#include <us_net_name_proxy_ifc.h>
#include <us_tm_root_proxy_ifc.h>
#include <uxsignal_ifc.h>

void connect_cmd(char *arglist)
{
	char			*p = arglist;
	char			servername[100];
	mach_error_t		ret;
	mach_port_t		server_port;

	strarg(&p,BRK,"Server Name: ","",servername);
	/*
	 * Locate the file server.
	 */
	ret = netname_look_up(name_server_port,"",servername,&server_port);
	if (ret != KERN_SUCCESS) {
		mach_error("netname_look_up",ret);
		return;
	}
	/*
	 * Replace the internal root cat.
	 */
	if (us_ps_root_agent != NULL) {
		mach_object_dereference(us_ps_root_agent);
		us_ps_root_agent = NULL;
	}
	us_ps_root_agent = new usItem_proxy;
	us_ps_root_agent->set_object_port(server_port);
#if NOISY
	fprintf(stdout,"Connected to %s, CWD = /\n",servername);
	fflush(stdout);
#endif NOISY
	return;
}

usTMRoot	*tm_obj = NULL;

mach_error_t tm_init(void)
{
	mach_port_t	port = MACH_PORT_NULL;
	mach_error_t	err;

	if (NULL == tm_obj) {
		/*
		 *  set up our global task master connection
		 */

		err = netname_look_up(name_server_port,"","task_master",&port);
		if (err) {
			printf("Can't establish initial Task Master connection %x (%s)\n",
				  err, mach_error_string(err));
			fflush(stdout);
			return(err);
		}
		tm_obj = new usTMRoot_proxy;
		tm_obj->set_object_port(port);
	}
	return(ERR_SUCCESS);
}

void tm_register_initial_task_cmd(void)
{
	mach_error_t	err = ERR_SUCCESS;
	uxsignal*   uxsignal_obj;
	tm_task_id_t	task_id = NULL_TASK_ID;
	vm_address_t	shared_dummy = NULL;

	usItem	*tm_tgrp_item = NULL;
	usItem	*tm_task_item = NULL;
	uxsignal_obj = new uxsignal;

	err = tm_init();	
	if (ERR_SUCCESS != err) return;

	err = tm_obj->tm_register_initial_task(mach_task_self(), uxsignal_obj,
			as_token,
#if SHARED_DATA_TIMING_EQUIVALENCE
			&shared_dummy,
#endif SHARED_DATA_TIMING_EQUIVALENCE
			TM_SELF_ACCESS, TM_SELF_ACCESS,	
			&tm_tgrp_item, &task_id, &tm_task_item);
#if NOISY
	printf("error = %d, '%s', taskid = %d, task_item = 0x%x, tgrp = 0x%x\n",
			err, mach_error_string(err), task_id, 
			tm_task_item, tm_tgrp_item);
	fflush(stdout);
#endif NOISY
	return;
}
/* Crap to make our uxsignal_obj happy */
extern "C" {
signal_ignore(){printf("uxsignal: signal_ignore\n");}
signal_core(){printf("uxsignal: signal_core\n");}
signal_terminate(){printf("uxsignal: signal_terminate\n");}
signal_stop(){printf("uxsignal: signal_stop\n");}
uxsignal_call_md(){printf("uxsignal: uxsignal_call_md\n");}
uxsignal_xlate_exception_md(){printf("uxsignal: uxsignal_xlate_exception_md\n");}
event_is_sensitive_md(){printf("uxsignal: event_is_sensitive_md\n");}
}

void tm_task_id_to_task_cmd(char *arglist)
{
	char			*p = arglist;
	tm_task_id_t		task_id;
	usItem			*task_item;
	usTMTask		*task_obj;
	mach_error_t		err;

	task_id = intarg(&p,BRK,"Task ID: ",1,0xffff,1);

        err = tm_obj->tm_task_id_to_task(task_id,
					 TM_DEFAULT_ACCESS, 
					 &task_item);
	task_obj = usTMTask::castdown(task_item);
	printf("id = %d; usItem = 0x%x; usTMTask = 0x%x; err = %d, '%s'\n",
		task_id, task_item, task_obj, err, mach_error_string(err));
	fflush(stdout);

	mach_object_dereference(task_item);
	return;
}

void tm_task_id_to_task_info_cmd(char *arglist)
{
	char			*p = arglist;
	tm_task_id_t		task_id;
	usItem			*task_item;
	usTMTask		*task_obj = NULL;
	mach_error_t		err;

	/* Shared info data */
	tm_task_id_t	id;
	int		touch;
	tm_task_id_t	parent_id;
	char		exec_string[SHARED_EXEC_STR_MAX];

	err = tm_init();	
	if (ERR_SUCCESS != err) return;

	task_id = intarg(&p,BRK,"Task ID: ",1,0xffff,1);

        err = tm_obj->tm_task_id_to_task(task_id,
					 TM_DEFAULT_ACCESS, 
					 &task_item);
	if (NULL == task_item) {
		printf("No such task: id=%d",task_id);
		return;
	}

	task_obj = usTMTask::castdown(task_item);
	strcpy(exec_string, "DUMMY_1234567890");
	err = task_obj->tm_get_shared_info(
		&id, &touch, &parent_id, exec_string);

	printf("pid=%d, touch=%d, ppid=%d, '%s'\n", id, touch, parent_id, exec_string);
	fflush(stdout);

	mach_object_dereference(task_item);
	return;
}

void tm_ps_cmd()
{
	/* task dir list stuff */
	mach_error_t		ret;
	usItem*		a_obj =0;
	usName*		cat =0;
	ns_path_t		path;
	ns_type_t		type;
	ns_name_list_t		names;
	ns_entry_t		ids;
	unsigned int		name_count;
	unsigned int		count;
	unsigned int		i;

	/* task_info values */
	tm_task_id_t		task_id;
	usItem			*task_item;
	usTMTask		*task_obj = NULL;
	mach_error_t		err;

	/* Task Shared info data */
	tm_task_id_t	id;
	int		touch;
	tm_task_id_t	parent_id;
	char		exec_string[SHARED_EXEC_STR_MAX];

	strcpy(path, "/tm/TASKS");
	ret = prefix_obj->ns_resolve_fully(path, resolve_mode, NSR_READ,
					    &a_obj, &type, 0);
	if (ret != NS_SUCCESS) {
		mach_error("ns_resolve",ret);
		return;
	}
	if (type != NST_DIRECTORY) {
		mach_object_dereference(a_obj);
		mach_error("",NS_NOT_DIRECTORY);
		return;
	}

	cat = usName::castdown(a_obj); /* XXX castdown */
	ret = cat->ns_list_entries(NSR_REFERENCE, &names, &name_count, &ids,
				    &count);

	if (ret != NS_SUCCESS) {
		mach_object_dereference(cat);
		mach_error("ns_list_entries",ret);
		return;
	}

#if NOISY
	fprintf(stdout,"Found %d entries: \n",count);
#endif NOISY
	printf("  id	Exec String\n");
	printf("  --	-----------\n");
	for (i = 0; i < count; i++) {
#if NOISY
		fprintf(stdout,"  %-40s     0x%08x\n",names[i],ids[i].obj_id);
#endif MOISY
		err = tm_init();	
		if (ERR_SUCCESS != err) return;

		task_id = atoi(names[i]);

	        err = tm_obj->tm_task_id_to_task(task_id,
						 TM_DEFAULT_ACCESS, 
						 &task_item);
		if (NULL == task_item) {
			printf("No such task: id=%d",task_id);
			continue;
		}

		task_obj = usTMTask::castdown(task_item);
		err = task_obj->tm_get_shared_info(
			&id, &touch, &parent_id, exec_string);

		printf("  %d	%s\n", id, exec_string);
		mach_object_dereference(task_item);
	}

	ret = vm_deallocate(mach_task_self(),names,name_count * sizeof(ns_name_t));
	if (ret != KERN_SUCCESS) {
		mach_error("vm_deallocate(names)",ret);
	}
	ret = vm_deallocate(mach_task_self(),ids,count * sizeof(struct ns_entry));
	if (ret != KERN_SUCCESS) {
		mach_error("vm_deallocate(ids)",ret);
	}
	mach_object_dereference(cat);
	fflush(stdout);
	return;
}

void tm_kernel_port_to_task_cmd(char *arglist)
{
	char			*p = arglist;
	mach_port_t		kernel_port;
	usItem			*task_item;
	usTMTask		*task_obj;
	mach_error_t		err;

	kernel_port = intarg(&p,BRK,"Task ID: ",1,0xffff,1);

        err = tm_obj->tm_kernel_port_to_task(mach_task_self(),
					 TM_DEFAULT_ACCESS, 
					 &task_item);
	task_obj = usTMTask::castdown(task_item);
	printf("port = %d; usItem = 0x%x; usTMTask = 0x%x; err = %d, '%s'\n",
		kernel_port, task_item, task_obj, err, mach_error_string(err));
	fflush(stdout);

	mach_object_dereference(task_item);
	return;
}



void tm_get_tgrp_id_cmd(char *arglist)
{
	char			*p = arglist;
	tm_task_id_t		task_id;
	tm_tgrp_id_t		tgrp_id;
	usItem			*task_item;
	usTMTask		*task_obj;
	usItem			*tgrp_item;
	usTMTgrp		*tgrp_obj;
	mach_error_t		err;

	task_id = intarg(&p,BRK,"Task ID: ",1,0xffff,1);

        err = tm_obj->tm_task_id_to_task(task_id,
					 TM_DEFAULT_ACCESS, 
					 &task_item);
	if (err != ERR_SUCCESS) {
		mach_error("tm_task_id_to_task",err);
		return;
	}
	printf("task_item = 0x%x\n",task_item);
	fflush(stdout);
	task_obj = usTMTask::castdown(task_item);
	if (task_obj == NULL) {
		fprintf(stderr,"Error in castdown for task_obj\n");
		mach_object_dereference(task_item);
		return;
	}
	printf("task_obj = 0x%x\n",task_obj);
	fflush(stdout);

	err = task_obj->tm_get_tgrp(TM_DEFAULT_ACCESS,&tgrp_item);
	if (err != ERR_SUCCESS) {
		mach_error("tm_get_tgrp",err);
		mach_object_dereference(task_item);
		return;
	}
	printf("tgrp_item = 0x%x\n",tgrp_item);
	fflush(stdout);
	tgrp_obj = usTMTgrp::castdown(tgrp_item);
	if (tgrp_obj == NULL) {
		fprintf(stderr,"Error in castdown for tgrp_obj\n");
		mach_object_dereference(task_item);
		mach_object_dereference(tgrp_item);
		return;
	}
	printf("tgrp_obj = 0x%x\n",tgrp_obj);
	fflush(stdout);

	err = tgrp_obj->tm_get_tgrp_id(&tgrp_id);
	if (err != ERR_SUCCESS) {
		mach_error("tm_get_tgrp_id",err);
		mach_object_dereference(task_item);
		mach_object_dereference(tgrp_item);
		return;
	}
	printf("tgrp_id = %d\n",tgrp_id);

	mach_object_dereference(task_item);
	mach_object_dereference(tgrp_item);
	fflush(stdout);
	return;
}

void exit_cmd(void)
{
	exit(0);
}


void _init_user_proxies(void);
void _print_map(void);

main(int argc, char **argv)
{
	int			new_debug_level = us_debug_level;
	char			*root_name;
	mach_error_t		ret;

	root_name = "pathname_server";

	cthread_init();
	intr_init();

#if NOISY
#if	MACH3_US
	(void) diag_startup("us_ps");
#else	MACH3_US
	(void) diag_startup_printf("us_ps");
	(void) mach3_output_unix();
#endif	MACH3_US
	(void) set_diag_level(new_debug_level);
#endif NOISY

	prefix_obj = new std_name;

	_init_user_proxies();
//	_print_map();
	/*
	 * Find the authentication server.
	 */
	ret = netname_look_up(name_server_port,"",LAS_NAME,&as_port);
	if (ret == ERR_SUCCESS) {
#if NOISY
		fprintf(stdout,"Connected to local authentication server\n");
#endif NOISY
	} else {
		ret = netname_look_up(name_server_port,"",CAS_NAME,&as_port);
		if (ret == ERR_SUCCESS) {
#if NOISY
			fprintf(stdout,
				"Connected to central authentication server\n");
#endif NOISY
		} else {
			mach_error(
			      "Cannot find authentication server - continuing",
				ret);
		}
	}

	/*
	 * Connect to the initial file system.
	 */
	connect_cmd(root_name);
	if (!strcmp(root_name,"pathname_server")) {
		char	ufs_cmd[100];
		strcpy(ufs_cmd, "ufsroot ufsroot");
		login_cmd(ufs_cmd);		
	}

	tm_ps_cmd();
	exit(0);
}


#include <us_name_proxy_ifc.h>
#include <us_net_name_proxy_ifc.h>
#include <usint_mf_proxy_ifc.h>
#include <tm_task_proxy_ifc.h>
#include <tm_tgrp_proxy_ifc.h>
#include <us_byteio_proxy_ifc.h>
#include <us_tty_proxy_ifc.h>

boolean_t _insert_class(char*, void*);
void*     _lookup_class(char*);

void _init_user_proxies(void)
{
	INSERT_CLASS_IN_MAP(usName_proxy, "++dir_proxy");
	INSERT_CLASS_IN_MAP(usName_proxy, "cat");
	INSERT_CLASS_IN_MAP(usNetName_proxy, "usNetName_proxy");
	INSERT_CLASS_IN_MAP(tm_task_proxy, "tm_task_proxy");
	INSERT_CLASS_IN_MAP(tm_tgrp_proxy, "tm_tgrp_proxy");
	INSERT_CLASS_IN_MAP(usByteIO_proxy, "usByteIO_proxy");

#undef 	INSERT_CLASS_IN_MAP

}
