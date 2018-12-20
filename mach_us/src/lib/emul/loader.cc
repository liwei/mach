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
 * File: us/lib/emul/loader.cc
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Simple program loader -- machine independent portion.
 *
 * Most of the code in this file was copied from the equivalent module
 * in POE, containing the following entries:
 *	Author:	Joseph S. Barrera III, Randall W. Dean
 *	Copyright (c) 1990 Joseph S. Barrera III, Randall W. Dean
 *
 * HISTORY
 * $Log:	loader.cc,v $
 * Revision 2.3  94/07/08  16:57:37  mrt
 * 	Updated copyrights.
 * 
 * Revision 2.2  91/11/13  17:16:57  dpj
 * 	First working version.
 * 	[91/11/12  17:50:27  dpj]
 * 
 */

#include	<base.h>

extern "C" {
#include	<mach.h>
}
#include	<mach/error.h>
#include	<us_byteio_ifc.h>
#include	<loader_info.h>

#include	<errno.h>

int	loader_debug = 1;


vm_prot_t data_prot = (VM_PROT_READ | VM_PROT_WRITE);
vm_prot_t text_prot = (VM_PROT_READ | VM_PROT_EXECUTE);


PRIVATE mach_error_t map_region(
	usByteIO* exec_obj,
	vm_offset_t file_offset,
	vm_offset_t vm_offset,
	vm_size_t size,
	vm_prot_t prot)		/* XXX should protect ! */
{
	mach_error_t		error;
	vm_size_t		rsize;
	unsigned int		resid;
	io_offset_t		io_offset;
	
	DEBUG1(loader_debug,(Diag,"map_region(%x, %x, %x, %x, %x, %x)\n", exec_obj, file_offset, vm_offset, size, prot));
	rsize = trunc_page(size);
	
	if (vm_offset != round_page(vm_offset)) {
		/*
		 *  In this case, need to copy. Yuck.
		 */
		ERROR((Diag,
			"map_region: region start 0x%x not page-alligned\n",
		       vm_offset));
		return KERN_FAILURE;
	}
	if (rsize > 0) {
		error = exec_obj->io_map(mach_task_self(),&vm_offset,rsize,
					0,FALSE,file_offset,TRUE,prot,
					VM_PROT_ALL,VM_INHERIT_COPY);
		if (error) {
			mach_error("map_region.io_map",error);
			return(error);
		}
	}
	vm_offset += rsize;		/* DPJ */
	file_offset += rsize;		/* DPJ */
	size -= rsize;
	if (size == 0) {
		return(ERR_SUCCESS);
	}

	/*
	 * We only get here when the region size is not a multiple of the
	 * page size. This does not happen on PMAX and 386, but may happen
	 * elsewhere.
	 *
	 * XXX Why not just io_map() the real length?
	 */
	INFO((Diag,"xxx how wierd, addr=0x%x, prot=0x%x\n", vm_offset, prot));
	UINT_TO_IO_OFF(file_offset,&io_offset);
	error = exec_obj->io_read(0,io_offset,vm_offset,&resid);
	if (error) {
		if (error == IO_INVALID_SIZE)
			return(unix_err(ENOEXEC));
		else
		       return(error);
	}

	return(ERR_SUCCESS);
}


mach_error_t loader_load_program_file(
	usByteIO* 		exec_obj,
	struct loader_info*	lp,
	vm_address_t*		new_brk)	/* OUT */
{
	mach_error_t		error;
	vm_offset_t		bss_page_start;
	vm_offset_t		bss_page_end;

	/* XXX worry about contiguous text & data! */
	DEBUG1(loader_debug,(Diag,"load_program_file: map text\n"));
	error = map_region(exec_obj, lp->text_offset, lp->text_start,
			    lp->text_size, text_prot);
	if (error) {
		return error;
	}
	DEBUG1(loader_debug,(Diag,"load_program_file: map data\n"));
	error = map_region(exec_obj, lp->data_offset, lp->data_start,
			    round_page(lp->data_size), data_prot);
	if (error) {
		return error;
	}
	*new_brk = round_page(lp->data_start + lp->data_size +
				  lp->bss_size);
	if (lp->bss_size == 0) {
		return ERR_SUCCESS;
	}
	bss_page_start = round_page(lp->data_start + lp->data_size);
	if (bss_page_start > lp->data_start + lp->data_size) {
		bzero((char *)(lp->data_start + lp->data_size),
			bss_page_start - (lp->data_start + lp->data_size));
	}
	bss_page_end = *new_brk;
	if (bss_page_start < bss_page_end) {
	    /*
	     * XXX Should try to map past the end of the file instead...
	     */
	    error = vm_allocate(mach_task_self(), &bss_page_start,
				bss_page_end - bss_page_start, FALSE);
	    if (error) {
		return error;
	    }
	}
	return ERR_SUCCESS;
}
