
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
 * fsadmin.c
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
 * $Log:	fsadmin.cc,v $
 * Revision 2.3  94/10/27  12:01:10  jms
 * 	Set some return values to VOID
 * 	Add tm_task_id_to_task_info_cmd to get the exec info for a given task
 * 	Add tm_ps_cmd to get the exec info for all tasks
 * 	[94/10/26  14:28:05  jms]
 * 
 * Revision 2.2  94/06/16  17:08:33  mrt
 * 	Moved from utils to bin.
 * 	[94/06/01            mrt]
 * 
 * Revision 2.8  94/05/17  14:11:11  jms
 * 	Add "-s" switch to be able to stop it to attach with a debugger
 * 	[94/04/29  14:03:02  jms]
 * 
 * Revision 2.7  94/01/11  18:13:13  jms
 * 	Noise
 * 	[94/01/10  13:59:58  jms]
 * 
 * Revision 2.6  93/01/20  17:40:57  jms
 * 	newfsadmin => fsadmin. yet again.
 * 	[93/01/18  18:05:56  jms]
 * 
 * Revision 2.5  92/07/05  23:37:26  dpj
 * 	Added tm_get_tgrp_id command.
 * 	[92/07/05  19:04:19  dpj]
 * 
 * 	Add some taskmaster test stuff
 * 	[92/06/24  18:23:47  jms]
 * 	Converted for new C++ RPC package.
 * 	[92/06/24  17:47:42  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:34:59  dpj]
 * 
 * Revision 2.4  92/03/05  15:16:44  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  19:54:35  jms]
 * 
 * Revision 2.3  91/12/20  17:45:49  jms
 * 	Update to new ns_ prefix table signatures. (dpj)
 * 	Add tsymlinks (dpj)
 * 	[91/12/20  16:36:26  jms]
 * 
 * Revision 2.2  91/11/06  14:25:34  jms
 * 	Copied from fsadmin of pjg.
 * 	[91/11/04  18:08:24  jms]
 * 
 * 	Initial C++ revision. Derived from fsadmin.c, but does not implement
 * 	everything in fsadmin.c
 * 	[91/10/03  14:59:28  pjg]
 * 
 * Revision 1.14  90/03/14  17:31:04  orr
 * 	don't use writeable strings for standard login.
 * 	[90/03/14  17:10:23  orr]
 * 
 * Revision 1.13  89/10/30  16:40:55  dpj
 * 	Fixed error reporting.
 * 	Added authid_map specification in nfsmount.
 * 	[89/10/27  19:57:05  dpj]
 * 
 * Revision 1.12  89/07/09  14:22:59  dpj
 * 	Added "-d<debug-level>" switch.
 * 	[89/07/08  15:47:44  dpj]
 * 
 * 	Added UsePrintf, so that there is output on the tty.
 * 	Added include of base.h.
 * 	Added variable definitions for us_debug_level and friends.
 * 	[89/07/08  13:27:59  dpj]
 * 
 * Revision 1.11  89/06/30  18:40:00  dpj
 * 	Added the nsfmount command.
 * 	Added NSF_MOUNT where needed.
 * 	Added a call to diag_startup().
 * 	[89/06/29  01:01:56  dpj]
 * 
 * Revision 1.10  89/05/18  10:54:47  dorr
 * 	ns_login -> ns_set_token
 * 	[89/05/15  12:34:35  dorr]
 * 
 * Revision 1.9  89/03/17  13:09:57  sanzi
 * 	Dereference dir_cat after ns_create()-ing the directory.
 * 	[89/03/17  09:43:43  sanzi]
 * 	
 * 	Added a ns_reference for cat's that are looked-up in the network
 * 	server.
 * 	Added static loading of mf_cat.
 * 	[89/03/11  21:21:16  dpj]
 * 	
 * 	Change open modes in calls to ns_resolve().
 * 	[89/03/11  16:09:06  sanzi]
 * 	
 * 	Fixed.
 * 	[89/03/10  14:55:34  sanzi]
 * 	
 * 	fix default protection again.  bad doug!  bad doug!
 * 	[89/03/02  20:53:06  dorr]
 * 	
 * 	Set file size when done writing in copyto_cmd().  Someday, this
 * 	will be taken care of in mf_mem_terminate.  But not today.
 * 	[89/03/02  14:27:54  sanzi]
 * 	
 * 	fix default prot structure.
 * 	[89/03/01  17:30:11  dorr]
 * 	
 * 	Added setup_mfobj().
 * 	[89/03/01  16:45:14  sanzi]
 * 	
 * 	switch to server side mach objects.
 * 	[89/02/28  00:17:24  dorr]
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
usItem* fsadmin_root_agent = 0;

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

	if (fsadmin_root_agent == NULL) {
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

	fprintf(stdout,"  User %d logged-in with groups:",user_id);
	for (i = 0; i < count; i++) {
		fprintf(stdout,"  %4d",out_group_ids[i]);
	}
	fprintf(stdout,"\n");

	ret = vm_deallocate(mach_task_self(),out_group_ids,count * sizeof(group_id_t));
	if (ret != KERN_SUCCESS) {
		mach_error("vm_deallocate",ret);
		return;
	}

	ret = prefix_obj->ns_set_system_prefix("/",fsadmin_root_agent,
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

	fprintf(stdout,"  CWD = /\n");

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

void ls_cmd(char *arglist)
{
	char			*p = arglist;
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

	strarg(&p,BRK,"Path: ","",path);

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

	fprintf(stdout,"Found %d entries: \n",count);
	for (i = 0; i < count; i++) {
		fprintf(stdout,"  %-40s     0x%08x\n",names[i],ids[i].obj_id);
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


void mkdir_cmd(char *arglist)
{
	char			*p = arglist;
	mach_error_t		ret;
	ns_path_t		pathname;
	ns_path_t		dir_name;
	ns_name_t		name;
	usItem*		a_obj =0;
	usName*		cat =0;
	ns_type_t		type;

	strarg(&p,BRK,"Path: ","",pathname);
	path(pathname,dir_name,name);
	ret = prefix_obj->ns_resolve_fully(dir_name, resolve_mode, NSR_INSERT,
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
	cat = usName::castdown(a_obj);
	ret = cat->ns_create(name, NST_DIRECTORY, dft_prot, dft_protlen,
			      NSR_REFERENCE, &a_obj);
	if (ret != NS_SUCCESS) {
		mach_object_dereference(cat);
		mach_error("ns_create",ret);
		return;
	}
	mach_object_dereference(cat);
	mach_object_dereference(a_obj);	
	fflush(stdout);
	return;
}


void rm_cmd(char *arglist)
{
	char			*p = arglist;
	mach_error_t		ret;
	ns_path_t		pathname;
	ns_path_t		dir;
	ns_name_t		name;
	usItem*		a_obj =0;
	usName*		cat =0;
	ns_type_t		type;

	strarg(&p,BRK,"Path: ","",pathname);

	path(pathname,dir,name);

	ret = prefix_obj->ns_resolve_fully(dir, resolve_mode, NSR_DELETE,
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

	cat = usName::castdown(a_obj);

	ret = cat->ns_remove_entry(name);

	if (ret != NS_SUCCESS) {
		mach_object_dereference(cat);
		mach_error("ns_remove_entry",ret);
		return;
	}

	mach_object_dereference(cat);
	fflush(stdout);
	return;
}


void symlink_cmd(char *arglist)
{
	char			*p = arglist;
	mach_error_t		ret;
	ns_path_t		linkname;
	ns_path_t		pathname;
	ns_path_t		dir_name;
	ns_name_t		name;
	usItem*		a_obj =0;
	usName*		cat =0;
	ns_type_t		out_type;

	strarg(&p,BRK,"New Path: ","",pathname);
	strarg(&p,BRK,"Link Name: ","",linkname);

	path(linkname,dir_name,name);

	ret = prefix_obj->ns_resolve_fully(dir_name, NSF_FOLLOW | NSF_MOUNT,
					    NSR_INSERT, &a_obj, &out_type, 0);
	if (ret != NS_SUCCESS) {
		mach_error("ns_resolve",ret);
		return;
	}
	if (out_type != NST_DIRECTORY) {
		mach_object_dereference(a_obj);
		mach_error("",NS_NOT_DIRECTORY);
		return;
	}
	cat = usName::castdown(a_obj);
	ret = cat->ns_insert_forwarding_entry(name, dft_prot, dft_protlen,
					       0, pathname);
	if (ret != NS_SUCCESS) {
		mach_object_dereference(cat);
		mach_error("ns_insert_forwarding_entry",ret);
		return;
	}
	mach_object_dereference(cat);
	fflush(stdout);
	return;
}


void tsymlink_cmd(char *arglist)
{
	char			*p = arglist;
	mach_error_t		ret;
	ns_path_t		linkname;
	ns_path_t		pathname;
	ns_path_t		dir_name;
	ns_name_t		name;
	usItem*		a_obj =0;
	usName*		cat =0;
	ns_type_t		out_type;

	strarg(&p,BRK,"New Path: ","",pathname);
	strarg(&p,BRK,"Link Name: ","",linkname);

	path(linkname,dir_name,name);

	ret = prefix_obj->ns_resolve_fully(dir_name, NSF_FOLLOW | NSF_MOUNT,
					    NSR_INSERT, &a_obj, &out_type, 0);
	if (ret != NS_SUCCESS) {
		mach_error("ns_resolve",ret);
		return;
	}
	if (out_type != NST_DIRECTORY) {
		mach_object_dereference(a_obj);
		mach_error("",NS_NOT_DIRECTORY);
		return;
	}
	cat = usName::castdown(a_obj);
	ret = cat->ns_create_transparent_symlink(name, dft_prot, dft_protlen,
					      pathname);
	if (ret != NS_SUCCESS) {
		mach_object_dereference(cat);
		mach_error("ns_create_transparent_symlink",ret);
		return;
	}
	mach_object_dereference(cat);
	fflush(stdout);
	return;
}


void mount_cmd(char *arglist)
{
	char			*p = arglist;
	mach_error_t		ret;
	ns_path_t		mountname;
	ns_path_t		mountdir;
	char			servername[100];
	ns_name_t		name;
	usItem*			a_obj =0;
	usName*			base_cat =0;
	usItem*			end_cat =0;
	ns_type_t		out_type;
	mach_port_t		server_port;

	strarg(&p,BRK,"Mount Point Name: ","",mountname);
	strarg(&p,BRK,"New Server Name: ","",servername);
	/*
	 * Locate the mount point.
	 */
	path(mountname,mountdir,name);
	ret = prefix_obj->ns_resolve_fully(mountdir, resolve_mode, NSR_INSERT,
					    &a_obj, &out_type, 0);
	if (ret != NS_SUCCESS) {
		mach_error("ns_resolve (mount base)",ret);
		return;
	}
	if (out_type != NST_DIRECTORY) {
		mach_object_dereference(a_obj);
		mach_error("Invalid mount point",NS_NOT_DIRECTORY);
		return;
	}
	base_cat = usName::castdown(a_obj);

	ret = prefix_obj->ns_resolve_fully(mountname, resolve_mode,
					    NSR_REFERENCE, &end_cat,
					    &out_type, 0);
	if (ret == NS_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_object_dereference(end_cat);
		printf("Cannot mount over an existing entry (for now)\n");
		return;
	} else if (ret != NS_NOT_FOUND) {
		mach_object_dereference(base_cat);
		mach_error("ns_resolve (mount end)",ret);
		return;
	}
	/*
	 * Locate the file server.
	 */
	ret = netname_look_up(name_server_port,"",servername,&server_port);
	if (ret != KERN_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_error("netname_look_up",ret);
		return;
	}
	usName_proxy* server_proxy = new usName_proxy;
	server_proxy->set_object_port(server_port);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_object_dereference(server_proxy);
		return;
	}
	/*
	 * Now do the mount.
	 */
	ret = base_cat->ns_insert_forwarding_entry(name, dft_prot,dft_protlen,
						    server_proxy, "");
	if (ret != NS_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_object_dereference(server_proxy);
		mach_error("ns_insert_forwarding_entry",ret);
		return;
	}
	mach_object_dereference(base_cat);
	mach_object_dereference(server_proxy);
	fflush(stdout);
	return;
}

void enter_cmd(char *arglist)
{
#ifdef notdef
	char			*p = arglist;
	mach_error_t		ret;
	ns_path_t		entryname;
	char			servername[100];
	ns_name_t		name;
	mach_object_t		base_cat;
	mach_object_t		end_cat;
	struct ns_spec		in_spec;
	struct ns_spec		out_spec;
	ns_type_t		out_type;
	mach_port_t		server_port;
	usItem*			server_cat;

	strarg(&p,BRK,"Entry Name: ","",entryname);
	strarg(&p,BRK,"New Server Name: ","",servername);

	/*
	 * Locate the entry point.
	 */
	path(entryname,&in_spec.path,name);

	in_spec.specdata.spec_function = NSS_SIMPLE;

	ret = ns_resolve_obj(&in_spec,resolve_mode,&base_cat,&out_spec,&out_type);

	if (ret != NS_SUCCESS) {
		mach_error("ns_resolve (entry base)",ret);
		return;
	}

	if (out_type != NST_DIRECTORY) {
		mach_object_dereference(base_cat);
		mach_error("Invalid entry point",NS_NOT_DIRECTORY);
		return;
	}

	strcpy(&in_spec.path,name);
	in_spec.specdata.spec_function = NSS_SIMPLE;
	ret = ns_resolve_obj(&in_spec,resolve_mode,&end_cat,&out_spec,&out_type);
	if (ret == NS_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_object_dereference(end_cat);
		fprintf("Cannot entry over an existing entry (for now)\n");
		return;
	} else if (ret != NS_NOT_FOUND) {
		mach_object_dereference(base_cat);
		mach_error("ns_resolve (entry end)",ret);
		return;
	}

	/*
	 * Locate the file server.
	 */
	ret = netname_look_up(name_server_port,"",servername,&server_port);
	if (ret != KERN_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_error("netname_look_up",ret);
		return;
	}
	ret = ns_export_cat(server_port,&server_cat);
	if (ret != ERR_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_error("ns_export_cat",ret);
		return;
	}
	/*
	 * Now do the enter.
	 */
	ret = ns_add_entry(base_cat,name,server_cat,"",NST_EXT_TERM);
	if (ret != NS_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_object_dereference(server_cat);
		mach_error("ns_add_entry",ret);
		return;
	}

	mach_object_dereference(base_cat);
	mach_object_dereference(server_cat);

	fflush(stdout);
	return;
#endif notdef
}

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
	if (fsadmin_root_agent != NULL) {
		mach_object_dereference(fsadmin_root_agent);
		fsadmin_root_agent = NULL;
	}
	fsadmin_root_agent = new usItem_proxy;
	fsadmin_root_agent->set_object_port(server_port);
	fprintf(stdout,"Connected to %s, CWD = /\n",servername);
	fflush(stdout);
	return;
}

void cd_cmd(char *arglist)
{
	char			*p = arglist;
	mach_error_t		ret;
	ns_path_t		path;

	strarg(&p,BRK,"Path: ","",path);

	ret = prefix_obj->ns_set_user_prefix("",path);
	if (ret != NS_SUCCESS) {
		mach_error("ns_define_user_prefix",ret);
		return;
	}
	fflush(stdout);
	return;
}


void copyto_cmd(char *arglist)
{
	char			*p = arglist;
	ns_path_t		uxpath;
	ns_path_t		fspath;
	int			uxfd;
	struct stat		statbuf;
	ns_name_t		name;
	ns_type_t		out_type;
	usItem*		a_obj =0;
	usByteIO*		file_cat =0;
	usName*		dir_cat =0;
	char			buf[BUFSIZE];
	int			resid;
	int			len;
	mach_error_t		ret;
	io_size_t		file_size;
	io_offset_t		startoff;
	ns_access_t		access = NSR_READ|NSR_WRITE|NSR_GETATTR;
	

	strarg(&p,BRK,"UNIX file: ","",uxpath);
	strarg(&p,BRK,"FS file: ","",fspath);
	/*
	 * Get the UNIX file.
	 */
	uxfd = open(uxpath,O_RDONLY,0);
	if (uxfd < 0) {
		perror("Cannot open UNIX file");
		return;
	}
	if (fstat(uxfd,&statbuf) < 0) {
		perror("Cannot stat UNIX file");
		close(uxfd);
		return;
	}
	/*
	 * Get the FS file.
	 */
	ret = prefix_obj->ns_resolve_fully(fspath, resolve_mode, access,
					    &a_obj, &out_type, 0);
	if (ret == NS_NOT_FOUND) {
		ns_path_t		dir;
		/*
		 * Create the file.
		 */
		path(fspath,dir,name);
		ret =prefix_obj->ns_resolve_fully(dir,resolve_mode,NSR_INSERT,
						   &a_obj, &out_type, 0);
		if (ret != NS_SUCCESS) {
			mach_error("Cannot find the containing directory",ret);
			close(uxfd);
			return;
		}
		if (out_type != NST_DIRECTORY) {
			mach_error("Invalid containing directory",
							NS_NOT_DIRECTORY);
			close(uxfd);
			mach_object_dereference(a_obj);
			return;
		}
		dir_cat = usName::castdown(a_obj);
		ret = dir_cat->ns_create(name, NST_FILE, dft_prot,
					  dft_protlen, access, &a_obj);
		if (ret != NS_SUCCESS) {
			mach_error("ns_create",ret);
			close(uxfd);
			mach_object_dereference(dir_cat);
			return;
		}
		mach_object_dereference(dir_cat);
	} 
	else if ((ret != NS_SUCCESS) || (out_type != NST_FILE)) {
		mach_error("Specified FS path name does not designate a file",ret);
		close(uxfd);
		if (ret == NS_SUCCESS) {
			mach_object_dereference(a_obj);
		}
		return;
	}

	/*
	 * Do the copy.
	 */
	file_cat = usByteIO::castdown(a_obj);
	resid = statbuf.st_size;
	INT_TO_DLONG(&file_size, 0);
	ret = file_cat->io_set_size(file_size);

	if (ret != ERR_SUCCESS) {
		mach_error("Cannot truncate the I/O object",ret);
		close(uxfd);
		mach_object_dereference(file_cat);
		return;
	}
	INT_TO_DLONG(&startoff, 0);
	INT_TO_DLONG(&file_size,resid);
	
	while (resid > 0) {
		len = ((resid > BUFSIZE) ? BUFSIZE : resid);

		len = read(uxfd,buf,len);
		if (len < 0) {
			perror("Error reading UNIX file");
			close(uxfd);
			mach_object_dereference(file_cat);
			return;
		}

		ret=file_cat->io_write(IOM_WAIT,startoff,(pointer_t)buf,&len);
		if (ret != ERR_SUCCESS) {
			mach_error("Error writing FS file",ret);
			close(uxfd);
			mach_object_dereference(file_cat);
			return;
		}

		resid -= len;
		ADD_INT_TO_DLONG(&startoff, len);
	}

	/*
	 * Close everything.
	 */
	close(uxfd);

	ret = file_cat->io_set_size(file_size);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot io_set_size the I/O object",ret);
		close(uxfd);
		mach_object_dereference(file_cat);
		return;
	}
	
	mach_object_dereference(file_cat);
	
	fprintf(stdout,"Copied %d bytes\n",statbuf.st_size);

	fflush(stdout);
	return;
}

void copyfrom_cmd(char *arglist)
{
	char			*p = arglist;
	ns_path_t		uxpath;
	ns_path_t		fspath;
	int			uxfd;
	ns_type_t		out_type;
	usItem*		a_obj =0;
	usByteIO*		file_cat =0;
	io_offset_t		startoff;	
	char			buf[BUFSIZE];
	int			resid;
	int			len;
	io_size_t		file_size;
	ns_access_t 		access = NSR_READ|NSR_GETATTR;
	mach_error_t		ret;

	strarg(&p,BRK,"FS file: ","",fspath);
	strarg(&p,BRK,"UNIX file: ","",uxpath);
	/*
	 * Get the UNIX file.
	 */
	uxfd = open(uxpath,O_WRONLY | O_CREAT | O_TRUNC,0644);
	if (uxfd < 0) {
		perror("Cannot open UNIX file");
		return;
	}
	/*
	 * Get the FS file.
	 */
	ret = prefix_obj->ns_resolve_fully(fspath, resolve_mode, access,
					    &a_obj, &out_type, 0);
	if ((ret != NS_SUCCESS) || (out_type != NST_FILE)) {
		mach_error("Specified FS path name does not designate a file",ret);
		close(uxfd);
		if (ret == NS_SUCCESS) {
			mach_object_dereference(a_obj);
		}
		return;
	}

	/*
	 * Do the copy.
	 */
	file_cat = usByteIO::castdown(a_obj);
	ret = file_cat->io_get_size(&file_size);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot stat I/O object",ret);
		close(uxfd);
		mach_object_dereference(file_cat);
		return;
	}

	INT_TO_DLONG(&startoff,0);
	resid = DLONG_TO_INT(file_size);
	
	while (resid > 0) {
		len = ((resid > BUFSIZE) ? BUFSIZE : resid);

		ret =file_cat->io_read(IOM_WAIT,startoff,(pointer_t)buf,&len);
		if (ret != ERR_SUCCESS) {
			mach_error("Error reading FS file",ret);
			close(uxfd);
			mach_object_dereference(file_cat);
			return;
		}

		len = write(uxfd,buf,len);
		if (len < 0) {
			perror("Error writing UNIX file");
			close(uxfd);
			mach_object_dereference(file_cat);
			return;
		}

		resid -= len;
		ADD_INT_TO_DLONG(&startoff,len);
	}

	/*
	 * Close everything.
	 */
	close(uxfd);
	mach_object_dereference(file_cat);

	fprintf(stdout,"Copied %d bytes\n", DLONG_TO_INT(file_size));

	fflush(stdout);
	return;
}


void type_cmd(char *arglist)
{
	char			*p = arglist;
	ns_path_t		fspath;
	FILE			*uxstream;
	ns_type_t		out_type;
	usItem*		a_obj =0;
	usByteIO*		file_cat =0;
	char			buf[BUFSIZE];
	int			resid;
	int			len;
	mach_error_t		ret;
	io_offset_t		startoff;
	io_size_t		file_size;
	ns_access_t		access = NSR_READ|NSR_GETATTR;
	
	strarg(&p,BRK,"FS file: ","",fspath);
	/*
	 * Get the pipe.
	 */
	uxstream = popen("/usr/ucb/more","w");
	if (uxstream == NULL) {
		perror("Cannot open pipe");
		return;
	}
	/*
	 * Get the FS file.
	 */
	ret = prefix_obj->ns_resolve_fully(fspath, resolve_mode, access,
					    &a_obj, &out_type, 0);
							
	if ((ret != NS_SUCCESS) || (out_type != NST_FILE)) {
		mach_error("Specified FS path name does not designate a file",ret);
		pclose(uxstream);
		if (ret == NS_SUCCESS) {
			mach_object_dereference(a_obj);
		}
		return;
	}
	/*
	 * Do the copy.
	 */
	file_cat = usByteIO::castdown(a_obj);
	ret = file_cat->io_get_size(&file_size);
	if (ret != ERR_SUCCESS) {
		mach_error("Cannot stat I/O file",ret);
		pclose(uxstream);
		mach_object_dereference(file_cat);
		return;
	}
	INT_TO_DLONG(&startoff,0);

	resid = DLONG_TO_INT(file_size);
	
	while (resid > 0) {
		len = ((resid > BUFSIZE) ? BUFSIZE : resid);

		ret = file_cat->io_read(IOM_WAIT, startoff, (pointer_t)buf,
					(unsigned int*)&len);
		if (ret != ERR_SUCCESS) {
			mach_error("Error reading FS file",ret);
			pclose(uxstream);
			mach_object_dereference(file_cat);
			return;
		}

		len = fwrite(buf,1,len,uxstream);
		if (len < 0) {
			perror("Error writing pipe");
			pclose(uxstream);
			mach_object_dereference(file_cat);
			return;
		}

		resid -= len;
		ADD_INT_TO_DLONG(&startoff,len);
	}

	/*
	 * Close everything.
	 */
	pclose(uxstream);
	mach_object_dereference(file_cat);
	
	fprintf(stdout,"Copied %d bytes\n", DLONG_TO_INT(file_size) );

	fflush(stdout);
	return;
}


void uxcd_cmd(char *arglist)
{
	char			*p = arglist;
	ns_path_t		uxpath;

	strarg(&p,BRK,"Path: ","",uxpath);
	if (chdir(uxpath) < 0) {
		perror("chdir");
	}
	fflush(stdout);
	return;
}


void nfsmount_cmd(char *arglist)
{
#ifdef notdef
	char			*p = arglist;
	mach_error_t		ret;
	char			hostname[256];
	ns_path_t		filesystem;
	ns_path_t		mountname;
	ns_path_t		mountdir;
	ns_name_t		entryname;
	ns_path_t		authid_map;
	usItem*			base_cat;
	usItem*			end_cat;
	ns_type_t		out_type;
	int			authid_fd;
	struct stat		authid_statbuf;
	char			*authid_addr;

	strarg(&p,BRK,"Host Name: ","",hostname);
	strarg(&p,BRK,"File System Name: ","",filesystem);
	strarg(&p,BRK,"Mount Point Name: ","",mountname);
	strarg(&p,BRK,"Authid Map: ","NONE",authid_map);

	/*
	 * Locate the mount point.
	 */
	path(mountname,mountdir,entryname);

	ret = ns_resolve_obj(prefix_obj,mountdir,resolve_mode,NSR_INSERT,
			     &base_cat,&out_type);

	if (ret != NS_SUCCESS) {
		mach_error("ns_resolve (mount base)",ret);
		return;
	}

	if (out_type != NST_DIRECTORY) {
		mach_object_dereference(base_cat);
		mach_error("Invalid mount point",NS_NOT_DIRECTORY);
		return;
	}

	ret = ns_resolve_obj(prefix_obj,mountname,resolve_mode,NSR_REFERENCE,
			     &end_cat,&out_type);
	if (ret == NS_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_object_dereference(end_cat);
		printf("Cannot mount over an existing entry (for now)\n");
		return;
	} else if (ret != NS_NOT_FOUND) {
		mach_object_dereference(base_cat);
		mach_error("ns_resolve (mount end)",ret);
		return;
	}

	/*
	 * Find the authid map.
	 */
	if (! strcmp(authid_map,"NONE")) {
		authid_map[0] = '\0';
	}
	if (authid_map[0] != '\0') {
		authid_fd = open(authid_map,O_RDONLY);
		if (authid_fd < 0) {
			mach_object_dereference(base_cat);
			perror("Cannot open authid map file");
			return;
		}
		if (fstat(authid_fd,&authid_statbuf) < 0) {
			mach_object_dereference(base_cat);
			perror("Cannot stat authid map file");
			return;
		}
		authid_addr = 0;
		if (map_fd(authid_fd,0,&authid_addr,TRUE,
					authid_statbuf.st_size) < 0) {
			mach_object_dereference(base_cat);
			perror("Cannot map authid map file");
			return;
		}
	} else {
		authid_addr = 0;
		authid_statbuf.st_size = 0;
	}

	/*
	 * Perform the mount.
	 */
	ret = nfs_mount(base_cat,hostname,filesystem,entryname,
					authid_addr,authid_statbuf.st_size);
	if (ret != KERN_SUCCESS) {
		mach_object_dereference(base_cat);
		mach_error("nfs_mount",ret);
		return;
	}

	mach_object_dereference(base_cat);
	if (authid_map[0] != '\0') {
		(void) vm_deallocate(mach_task_self(),
					authid_addr,authid_statbuf.st_size);
		close(authid_fd);
	}

	fflush(stdout);
	return;
#endif notdef
}

#include <us_tm_root_proxy_ifc.h>
#include <uxsignal_ifc.h>

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
		printf("error = %d, '%s', taskid = %d, task_item = 0x%x, tgrp = 0x%x\n",
			err, mach_error_string(err), task_id, 
			tm_task_item, tm_tgrp_item);
	fflush(stdout);
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

//	fprintf(stdout,"Found %d entries: \n",count);
	for (i = 0; i < count; i++) {
//		fprintf(stdout,"  %-40s     0x%08x\n",names[i],ids[i].obj_id);
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

		printf("id = %d	'%s'\n", id, exec_string);
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


/*
 * Main command list.
 */
CIENTRY		ci_main[] = {
	CICMD("login",login_cmd),
	CICMD("logout",logout_cmd),
	CICMD("connect",connect_cmd),
	CICMD("cd",cd_cmd),
	CICMD("ls",ls_cmd),
	CICMD("mkdir",mkdir_cmd),
	CICMD("rm",rm_cmd),
	CICMD("symlink",symlink_cmd),
	CICMD("tsymlink",tsymlink_cmd),
	CICMD("mount",mount_cmd),
	CICMD("enter",enter_cmd),
	CICMD("copyto",copyto_cmd),
	CICMD("cpto",copyto_cmd),
	CICMD("copyfrom",copyfrom_cmd),
	CICMD("cpfrom",copyfrom_cmd),
	CICMD("type",type_cmd),
	CICMD("more",type_cmd),
	CICMD("uxcd",uxcd_cmd),
	CICMD("nfsmount",nfsmount_cmd),
	CICMD("tm_ps",tm_ps_cmd),	
	CICMD("tm_register_initial_task",tm_register_initial_task_cmd),
	CICMD("tm_task_id_to_task",tm_task_id_to_task_cmd),	
	CICMD("tm_task_id_to_task_info",tm_task_id_to_task_info_cmd),	
	CICMD("tm_kernel_port_to_task",tm_kernel_port_to_task_cmd),
	CICMD("tm_get_tgrp_id",tm_get_tgrp_id_cmd),
	CICMD("quit",exit_cmd),
	CICMD("exit",exit_cmd),
	CIINT("us_debug_level",us_debug_level),
	CIINT("us_debug_force",us_debug_force),
	CIINT("mach_object_debug_level",mach_object_debug_level),
	CIINT("resolve_mode",resolve_mode),
	CIINT("io_mode",io_mode),	
	CIEND
};

void _init_user_proxies(void);
void _print_map(void);

main(int argc, char **argv)
{
	int			new_debug_level = us_debug_level;
	char			*root_name;
	mach_error_t		ret;

	argv++;
	if ( (argc > 1) && strncmp(argv[0],"-s",2) == 0) {
		argc--;
		task_suspend(mach_task_self());		
		argv++;
	}

	if ( (argc > 1) && strncmp(argv[0],"-d",2) == 0) {
		argc--;
		sscanf(argv[0]+2,"%d",&new_debug_level);
		argv++;
	}

	if (argc > 1) {
		argc--;
		root_name = argv[0];
		argv++;
	} else {
		root_name = "pathname_server";
	}

	if (argc > 1) {
		USAGE();
		exit(1);
	}

	cthread_init();
	intr_init();

#if	MACH3_US
	(void) diag_startup("fsadmin");
#else	MACH3_US
	(void) diag_startup_printf("fsadmin");
	(void) mach3_output_unix();
#endif	MACH3_US
	(void) set_diag_level(new_debug_level);

	prefix_obj = new std_name;

	_init_user_proxies();
//	_print_map();
	/*
	 * Find the authentication server.
	 */
	ret = netname_look_up(name_server_port,"",LAS_NAME,&as_port);
	if (ret == ERR_SUCCESS) {
		fprintf(stdout,"Connected to local authentication server\n");
	} else {
		ret = netname_look_up(name_server_port,"",CAS_NAME,&as_port);
		if (ret == ERR_SUCCESS) {
			fprintf(stdout,
				"Connected to central authentication server\n");
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

	/*
	 * Command loop.
	 */
	ci(
		"FSADMIN >",			/* prompt */
		0,				/* file */
		0,				/* depth */
		ci_main,			/* list */
		0,				/* helppath */
		0				/* cmdfpath */
	);

	fprintf(stdout, "Terminated.\n");
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

extern "C" {
void my_task_suspend(void);
}

void my_task_suspend(void)
{
//	task_suspend(mach_task_self());
}
