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
 * HISTORY
 * $Log:	null_pager_ifc.h,v $
 * Revision 2.3  94/07/07  17:23:56  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  93/01/20  17:38:02  jms
 * 	First version.
 * 	[93/01/18  16:46:39  jms]
 * 
 */
#ifndef	_null_pager_ifc_h
#define	_null_pager_ifc_h

#include	<pager_base_ifc.h>

extern "C" {
#include	<ns_types.h>
#include	<io_types.h>
}




class null_pager: public pager_base {
	boolean_t		started;
	vm_address_t		backing_space;	/* location of backing mem */
	vm_size_t		backing_size;	/* Its size */
	vm_size_t		written_size;	/* The formal items "eof" */
	vm_size_t		unclean_size;	/* Edge of "virgin" space */
	struct mutex		lock;

      public:
				null_pager();
	virtual			~null_pager();

	mach_error_t		null_pager_start();

	virtual mach_error_t	io_pagein(
					vm_offset_t,
					vm_address_t*,
					vm_size_t*,
					boolean_t*);
	virtual mach_error_t	io_pageout(
					vm_offset_t,
					vm_address_t,
					vm_size_t*);
	virtual mach_error_t	io_get_size(io_size_t*);
	virtual mach_error_t	io_set_size(io_size_t);
};

#endif	_null_pager_ifc_h
