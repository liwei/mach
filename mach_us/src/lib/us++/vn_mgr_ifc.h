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
 *
 * Purpose: Manager of Vnode-based active objects.
 *
 * HISTORY
 * $Log:	vn_mgr_ifc.h,v $
 * Revision 2.4  94/07/07  17:25:56  mrt
 * 	Updated copyright.
 * 
 * Revision 2.3  93/01/20  17:38:40  jms
 * 	AOT_NUM_BUCKETS 128 => 4096
 * 	[93/01/18  17:11:15  jms]
 * 
 * Revision 2.2  92/07/05  23:32:06  dpj
 * 	First working version.
 * 	[92/06/24  17:27:14  dpj]
 * 
 * Revision 2.1  91/09/27  15:03:08  pjg
 * Created.
 * 
 * Revision 2.2  90/12/21  14:14:50  jms
 * 	Initial revision.
 * 	[90/12/15  15:16:22  roy]
 * 	merge for roy/neves @osf
 * 	[90/12/20  13:22:03  jms]
 * 
 * Revision 2.1  90/12/15  15:16:10  roy
 * Created.
 * 
 */

#ifndef	_vn_mgr_ifc_h
#define	_vn_mgr_ifc_h

#include 	<top_ifc.h>
#include	<queue.h>

class vn_agency;


/*
 * Key for objects in the active object table.
 */
typedef	int	aot_key_t;

/*
 * Values of 'state' relevant to aot_get_state()/aot_set_state().
 */
typedef int	aot_state_t;
#define AOT_STATE_DIR_ACTIVE		0   /* active non-file object */
#define AOT_STATE_FILE_ACTIVE		1   /* active file */
#define AOT_STATE_FILE_MUST_CLEAN	2   /* active file, must be cleaned */
#define AOT_STATE_INACTIVE		3   /* previously active, now inact */


/*
 * Total number of bucket chains (must be power of 2).
 */
#define AOT_NUM_BUCKETS		4096

/*
 * Queue numbers for queues in the active table.
 */
typedef int	aot_qnum_t;

#define AOT_QUEUE_NUM_QUEUES		5
#define AOT_QUEUE_DIR_ACTIVE		AOT_STATE_DIR_ACTIVE
#define AOT_QUEUE_FILE_ACTIVE		AOT_STATE_FILE_ACTIVE
#define AOT_QUEUE_FILE_MUST_CLEAN	AOT_STATE_FILE_MUST_CLEAN
#define AOT_QUEUE_INACTIVE		AOT_STATE_INACTIVE
#define AOT_QUEUE_FREE			AOT_QUEUE_INACTIVE + 1
#define AOT_QUEUE_NULL			0x99  /* signifies not on a queue */


/*
 * Internal types.
 */
typedef struct aot_queue_head {
	queue_head_t		head;
	unsigned int		count;
} *aot_queue_head_t;

struct aot_entry;
typedef struct aot_entry*	aot_entry_t;


/*
 * Class declaration.
 */
class vn_mgr: public usTop {
    public:
	DECLARE_LOCAL_MEMBERS(vn_mgr);
			vn_mgr();
	virtual		~vn_mgr();

	/*
	 * Scanner control.
	 */
    private:
	unsigned int		cycle_time;
	unsigned int		destroy_interval;
	unsigned int		clean_interval;
	cthread_t		scanner_thread;
    public:
	void		scanner();

	/*
	 * Active Objects Table (AOT) control.
	 */
    private:
	struct mutex		queuel;  /* lock for manipulating queues */
	struct aot_queue_head	aot_queues[AOT_QUEUE_NUM_QUEUES];
	queue_head_t 		aot_buckets[AOT_NUM_BUCKETS];
	void		aot_free_entry_internal(aot_entry_t);
	mach_error_t	aot_find_entry_internal(aot_key_t,
						boolean_t,aot_entry_t*);
	void		aot_move_entry_internal(aot_entry_t,aot_qnum_t);
    public:
	mach_error_t	aot_lock(aot_key_t,void**);
	void		aot_unlock(void*);
	aot_state_t	aot_get_state(void*);
	void		aot_set_state(void*,aot_state_t);
	mach_error_t	aot_lookup_or_reserve(aot_key_t,vn_agency**,int*);
	void		aot_cancel(int);
	void		aot_install(int,vn_agency*,aot_state_t);
	mach_error_t	aot_enter(aot_key_t,vn_agency*,aot_state_t);
	void		aot_clean();
	void		aot_destroy_inactive();
};

#endif	_vn_mgr_ifc_h
