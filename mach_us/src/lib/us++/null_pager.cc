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
 * $Log:	null_pager.cc,v $
 * Revision 2.3  94/07/07  17:23:54  mrt
 * 	Updated copyright.
 * 
 * Revision 2.2  93/01/20  17:38:00  jms
 * 	First release.
 * 	[93/01/18  16:46:11  jms]
 * 
 */

#include	<null_pager_ifc.h>

extern "C" {
#include	<io_types.h>
#include	<errno.h>
}


/* initial size of backing story memory of the null pager */
#define NULL_PAGER_BACKING_SIZE (vm_page_size * 0x10)

/*
 * Debugging control.
 */
int	null_pager_debug = 1;

/*
 * Backing Pool Definition: Someplace to keep inactive allocated backing stores.
 */
class backing_pool;

class backing_puddle {
	friend class backing_pool;

    private:
	backing_puddle		*next;
	vm_size_t		size;
	vm_address_t		addr;

	backing_puddle(vm_address_t, vm_size_t);
};

class backing_pool {
	mutex		bp_lock;
	backing_puddle	*free_list;
	backing_puddle	*rec_list;

    public:
	backing_pool(int init_dummy);
	~backing_pool();
	mach_error_t get_space(vm_address_t *, vm_size_t *);
	mach_error_t free_space(vm_address_t, vm_size_t);
};

backing_pool::backing_pool(int init_dummy) {
	mutex_init(&bp_lock);
	free_list = NULL;
	rec_list = NULL;
}

backing_pool::~backing_pool(){
}

mach_error_t backing_pool::get_space(vm_address_t *_addr, vm_size_t *_size)
{
	mach_error_t		err = ERR_SUCCESS;
	backing_puddle *	puddle;

	mutex_lock(&bp_lock);
	if (NULL != free_list) {
		puddle = free_list;
		free_list = puddle->next;
		puddle->next = rec_list;
		rec_list = puddle;
		*_addr = puddle->addr;
		*_size = puddle->size;
	}
	else {
		err = vm_allocate(mach_task_self(), _addr,
			NULL_PAGER_BACKING_SIZE, TRUE);
		*_size = NULL_PAGER_BACKING_SIZE;
	}
	mutex_unlock(&bp_lock);
	return(err);
}

mach_error_t backing_pool::free_space(vm_address_t addr, vm_size_t size)
{
	mach_error_t		err = ERR_SUCCESS;
	backing_puddle *	puddle;

	mutex_lock(&bp_lock);
	if (NULL != rec_list) {
		puddle = rec_list;
		rec_list = puddle->next;
	}
	else {
		puddle = new backing_puddle(addr,size);
	}
	puddle->next = free_list;
	free_list = puddle;
	mutex_unlock(&bp_lock);
	return(ERR_SUCCESS);
}

backing_puddle::backing_puddle(vm_address_t _addr, vm_size_t _size) {
	addr = _addr;
	size = _size;
}

static backing_pool pool(0);

/*
 * NULL Pager Methods
 */

null_pager::null_pager()
	:
	started(FALSE)
{}

null_pager::~null_pager()
{
	mach_error_t	err;

	if (! started) return;
	pool.free_space(backing_space, backing_size);
}


mach_error_t null_pager::null_pager_start()
{
	mach_error_t	err;

	DEBUG1(null_pager_debug,(Diag,"null_pager::start\n"));

	mutex_init(&(this->lock));

	started = TRUE;
	backing_size = NULL_PAGER_BACKING_SIZE;
	written_size = 0;
	unclean_size = 0;

	err = pool.get_space(&backing_space, &backing_size);
	if (err != ERR_SUCCESS) {
	    return(err);
	}

	return(pager_base_start(FALSE));
}

/*
 * null_pagein()
 *
 * 	Read 'count' bytes from the 'backing_space'
 *	beginning at 'offset.'  offset must be on a vm_page_size
 *      boundary and count an integral multiple of vm_page_size.
 *	
 *	If any part of the requested data had not been created, zero-filled
 *	memory will be returned for that part of the data.
 *
 *      The caller of this routine is responsible for freeing the 'data'
 *    	VM region.
 */
mach_error_t null_pager::io_pagein(
	vm_offset_t		offset,
	vm_address_t*		data,
	vm_size_t*		count,
	boolean_t*		deallocate)
{
        register int request_len, wash_len, start, remainder;
        vm_address_t tempbuf;
	mach_error_t ret = ERR_SUCCESS;
	
        DEBUG1(null_pager_debug,(Diag,"null_pager::io_pagein, offset=%d, size=%d\n",
			     offset,*count));
        request_len = *count;   /* amount requested by caller */
        *count = 0;             /* nothing read, yet */
        *data = NULL;
	*deallocate = FALSE;

        if ((request_len == 0) ||
	    ((offset + request_len) > backing_size)) {
	        return(EINVAL);
	}

	mutex_lock(&(this->lock));

	/*
         * If desired offset is beyond the "end of the file"
	 * in the file, then bzero the "new" area if it is unclean.
         */
	start = backing_space+offset;
	remainder = start+request_len;

	wash_len = 0;
	if (remainder <= written_size) {
		wash_len = 0;
	}
	else {
		if (remainder > unclean_size) {
			wash_len = unclean_size - written_size;
		}
		else {
			wash_len = remainder - written_size;
		}
	}
	if (wash_len) {
		bzero(backing_space+written_size, wash_len);
	}
	mutex_unlock(&(this->lock));

        *data = start;              /* set return values */
        *count = request_len;

        return(ERR_SUCCESS);
}


/*
 * null_pageout()
 *
 * 	Write 'count' bytes to the 'backing_space'
 *	beginning at 'offset.'  The data to write is contained
 *	in the VM region 'data.'  
 *	
 *	The file will be grown as necessary to satisfy the request.
 *	On return, the amount written is specified by the return value
 *	of 'count.'
 *
 *      Note that this routine "consumes" the 'data' VM region.
 */
mach_error_t null_pager::io_pageout(
	vm_offset_t		offset,
	vm_address_t		data,
	vm_size_t*		count)	/* inout */
{
        register int request_len, next_len;
	mach_error_t ret = ERR_SUCCESS;
	
	DEBUG1(null_pager_debug,(Diag,"io_pageout, offset=%d, size=%d\n",
			     offset,*count));

        request_len = *count;   /* amount requested by caller */
        *count = 0;             /* nothing written, yet */

        if ((request_len == 0) ||
	    ((offset + request_len) > backing_size)) {
	        return(EINVAL);
	}

	mutex_lock(&(this->lock));
	
	written_size = ((written_size > (next_len = offset + request_len )) ?
				written_size : next_len);
	bcopy((char *)(backing_space+offset), (char *)data, request_len);
	mutex_unlock(&(this->lock));
	
	(void) vm_deallocate(mach_task_self(), data, request_len);
	return(ret);

}


mach_error_t null_pager::io_get_size(
	io_size_t*		size)
{

	mach_error_t		ret;

	DEBUG1(null_pager_debug,(Diag,"io_get_size"));

	INT_TO_IO_SIZE(written_size, size);
	return(ERR_SUCCESS);
}


mach_error_t null_pager::io_set_size(
	io_size_t		newsize)
{
	DEBUG1(null_pager_debug,(Diag,"io_set_size"));

	IO_SIZE_TO_INT(newsize, &written_size);
	return(ERR_SUCCESS);
}


