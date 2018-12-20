/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/server/config/sys_agency.cc,v $
 *
 * Purpose: Mach Startup/Admin/Config Server agency
 *
 * HISTORY:
 * $Log:	sys_agency.cc,v $
 * Revision 2.3  94/07/07  16:37:49  mrt
 * 	Updated copyright
 * 
 * Revision 2.2  92/07/05  23:34:14  dpj
 * 	First working version.
 * 	[92/06/24  17:40:10  dpj]
 * 
 * Revision 2.2  91/11/06  14:17:52  jms
 * 	Moved from lib/us
 * 	[91/11/04  17:44:47  jms]
 * 
 * Revision 2.2  91/10/06  22:30:17  jjc
 * 	Fixed up sys_agency_sys_get_prefix_table().
 * 	[91/10/01  19:03:25  jjc]
 * 
 * 	Created.
 * 	[91/06/20            jjc]
 * 
 *
 */

#include	<sys_agency_ifc.h>
#include	<access_table_ifc.h>


ns_mgr_id_t	config_svr_id = { 0, 0x04000001 };


DEFINE_CLASS_MI(sys_agency);
DEFINE_CASTDOWN2(sys_agency,usSys,vol_agency);


sys_agency::sys_agency()
{
	Local(server_objects) = NULL;
	Local(prefix_names) = NULL;
	Local(prefix_count) = 0;
}


sys_agency::sys_agency(
	usItem**	objects,
	ns_name_t*	names,
	int		count,
	mach_error_t*	ret)
	:
	vol_agency(config_svr_id,new access_table(ret))
{
	Local(server_objects) = objects;
	Local(prefix_names) = names;
	Local(prefix_count) = count;
	*ret = ERR_SUCCESS;
}


sys_agency::~sys_agency()
{
#ifdef	notdef
	free((char *)Local(server_objects));
	free((char *)Local(prefix_names));
#endif	notdef
}


void sys_agency::init_class(usClass* class_obj)
{
	usSys::init_class(class_obj);
	vol_agency::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(sys_agency);
	SETUP_METHOD_WITH_ARGS(sys_agency,sys_get_prefix_table);
	END_SETUP_METHOD_WITH_ARGS;
}

mach_error_t sys_agency::sys_get_prefix_table(
	usItem**	objects,		/* out */
	int*		object_count,		/* out */
	ns_name_t**	names,			/* out */
	int*		name_count)		/* out */
{
	int		i, cnt;
	usItem**	new_obj;
	usItem**	old_obj;

	*object_count = *name_count = 0;
	cnt = Local(prefix_count);

	if (cnt > 0) {
		new_obj = objects;
		old_obj = Local(server_objects);
		for (i = 0; i < cnt; i++) {
			new_obj[i] = old_obj[i];
			mach_object_reference(new_obj[i]);
		}

		*names = Local(prefix_names);
		*object_count = *name_count = Local(prefix_count);
	}
	return(ERR_SUCCESS);
}

