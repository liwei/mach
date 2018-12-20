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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/ux/uxio_dir.cc,v $
 *
 * Purpose:
 *
 * HISTORY: 
 * $Log:	uxio_dir.cc,v $
 * Revision 2.4  94/07/08  16:02:00  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.3  92/03/05  15:07:32  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:47:27  jms]
 * 
 * Revision 2.2  91/11/06  14:11:53  jms
 * 	Upgraded to US41
 * 	[91/09/26  20:15:47  pjg]
 * 
 * 	Initial C++ revision.
 * 	[91/04/15  10:09:38  pjg]
 * 
 * Revision 2.2  90/11/10  00:38:46  dpj
 * 	Subclass of uxio representing directories.
 * 	[90/10/17  12:57:23  neves]
 * 
 *
 */

#ifndef lint
char * uxio_dir_rcsid = "$Header: uxio_dir.cc,v 2.4 94/07/08 16:02:00 mrt Exp $";
#endif	lint

#include <uxio_dir_ifc.h>
#include <us_name_ifc.h>

extern "C" {
#include <sys/dir.h>
#include <sys/errno.h>
}

#define BASE uxio
DEFINE_LOCAL_CLASS(uxio_dir)

static struct dlong_t dlong_zero = { 0, };


uxio_dir::uxio_dir() : uxio(), dir_list(0), dir_cnt(0), 
		       dir_entries(0), dir_entries_cnt(0)
{}


uxio_dir::~uxio_dir()
{
	if (dir_list)
		(void)vm_deallocate(mach_task_self(), (vm_address_t)dir_list,
				    dir_cnt*sizeof(dir_list)[0]);
	if (dir_entries)
		(void)vm_deallocate(mach_task_self(), (vm_address_t)dir_entries,
				    dir_entries_cnt*sizeof(dir_entries)[0]);
}

mach_error_t
uxio_dir::ux_ftruncate(unsigned int len)
{
	return MACH_OBJECT_NO_SUCH_OPERATION;
}

mach_error_t 
uxio_dir::io_read_internal(int mode, io_offset_t io_start, pointer_t buf, 
			   unsigned int* num)
{
	mach_error_t		err;
	register struct direct	* dp;
	struct direct		* ndp;
	struct direct		* dp_end;
	struct direct		dir;
	unsigned int		off;
	unsigned int	        len;
	unsigned int		cur;
	unsigned int		start;

	DEBUG2(TRUE,(0, "uxio_dir::_io_read_internal\n"));
	if (dir_list == (ns_name_list_t)0) {
		/*
		 * XXX C++ Delegate ??
		 */
/*
		err = ns_list_entries(Delegate, mode, &_Local(dir_list), &_Local(dir_cnt),
				      &_Local(dir_entries), &_Local(dir_entries_cnt));
*/
		usName* p;
		if ((p = usName::castdown(obj)) == 0) {
			DEBUG1(TRUE,(0, "uxio_dir::_io_read_internal\n"));
			return MACH_OBJECT_NO_SUCH_OPERATION;
		}
		err = p->ns_list_entries(mode, &dir_list, &dir_cnt,
					  &dir_entries, &dir_entries_cnt);
		DEBUG2(TRUE,(0,"uxio_dir::_io_read_internal: ns_list_entries()->err=0x%0x,dir_entries_cnt=%d\n", err, dir_entries_cnt));
		if (err) return err;
	}

	/* figure out how many names you want */
	dp_end = (struct direct *)((char *)buf + *num);

	/* 
	 * figure out where you are currently and
	 * convert that into a directory position 
	 */
	/*
	 * assume directories are less than 4 gigabytes
	 */
	start = DLONG_TO_U_INT(io_start);

	for(cur=0, off=0; (off < start) && (cur < _Local(dir_cnt)); cur++) {
		/* advance the offset until you hit the current location */
		dir.d_namlen = strlen(_Local(dir_list)[cur]);
		off += DIRSIZ(&dir);
	}

	DEBUG1(TRUE,(0, "uxio_dir::_io_read_internal: start at off=%d\n", off));

	/* cons up some struct direct entries */
	for( dp=(struct direct *)buf; dp < dp_end; cur++ ) {
		if (cur >= _Local(dir_cnt)) break;

		len = strlen(_Local(dir_list)[cur]);
		if (len > MAXNAMLEN) len = MAXNAMLEN;

		dir.d_namlen = len;
		dir.d_reclen = DIRSIZ(&dir);

		/* calculate the end of this record.  make sure it's in bounds */
		ndp = (struct direct *)((char *)dp + dir.d_reclen);
		if (ndp > dp_end) break;

		dp->d_ino = _Local(dir_entries)[cur].obj_id;
		dp->d_namlen = dir.d_namlen;
		dp->d_reclen = dir.d_reclen;
		bcopy(_Local(dir_list)[cur],dp->d_name, len + 1);

		dp = ndp;
	}

	*num = (char *)dp - (char *)buf;

	DEBUG1(TRUE,(0, "uxio_dir::_io_read_internal: returning %d bytes\n", *num));
	if (*num == 0)
		return IO_INVALID_OFFSET;
	else
		return ERR_SUCCESS;
}


mach_error_t 
uxio_dir::ns_get_attributes_internal(ns_attr_t attrs, int *attrlen)
{
	unsigned int		cur;
	struct direct		dir;
	mach_error_t		err;
	int			mode = 0;

	DEBUG2(TRUE,(0,"uxio_dir::_ns_get_attributes\n"));
	/* get the underlying attributes */
		/*
		 * XXX C++ Delegate ??
		 */
/*  	if( err = ns_get_attributes(Delegate,attrs,attrlen) ) */

	if( err = obj->ns_get_attributes(attrs, attrlen) )
		return err;

	if (_Local(dir_list) == (ns_name_list_t)0) {
		/*
		 * XXX C++ Delegate ??
		 */
/*
		err = ns_list_entries(Delegate, mode, &_Local(dir_list), 
				      &_Local(dir_cnt), &_Local(dir_entries), 
				      &_Local(dir_entries_cnt));
*/
		usName* n_obj;
		if ((n_obj = usName::castdown(obj)) == 0) {
			DEBUG1(TRUE,(0,"uxdir::_ns_get_attributes_internal\n"));
			return MACH_OBJECT_NO_SUCH_OPERATION;
		}
		err = n_obj->ns_list_entries(mode, &dir_list, &dir_cnt,
					     &dir_entries, &dir_entries_cnt);
		if (err) return err;
	}

	/* and mung the size */
	for(cur=0, attrs->size=dlong_zero; (cur < _Local(dir_cnt)); cur++) {
		/* advance the offset until you hit the current location */
		dir.d_namlen = strlen(_Local(dir_list)[cur]);
		ADD_U_INT_TO_DLONG(&(attrs->size),DIRSIZ(&dir));
	}

	attrs->valid_fields |= NS_ATTR_SIZE;

	return err;
}



