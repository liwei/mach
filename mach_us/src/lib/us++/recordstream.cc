/* 
 * Mach Operating System
 * Copyright (c) 1994,1993,1992,1991,1990 Carnegie Mellon University
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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/recordstream.cc,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: General-purpose queue for undifferentiated records.
 *
 * HISTORY:
 * $Log:	recordstream.cc,v $
 * Revision 2.5  94/07/07  17:24:04  mrt
 * 	Updated copyright.
 * 
 * Revision 2.4  92/07/05  23:28:24  dpj
 * 	Define as an abstract class instead of a concrete class.
 * 	[92/06/24  17:01:48  dpj]
 * 
 * 	Eliminated ambiguity with local member "lock".
 * 	Added DESTRUCTOR_GUARD.
 * 	[92/05/10  00:56:32  dpj]
 * 
 * Revision 2.3  91/11/13  17:17:55  dpj
 * 	condition_wait() -> intr_cond_wait()
 * 	[91/11/13  14:50:08  dpj]
 * 
 * Revision 2.2  91/11/06  13:47:25  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:03:03  pjg]
 * 
 * Revision 2.3  91/07/01  14:12:31  jms
 * 	Code from dan to make record streams interruptable
 * 	[91/06/24  17:26:37  jms]
 * 
 * Revision 2.2  91/05/05  19:27:15  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:57:53  dpj]
 * 
 * 	Reorganized for split between basic interface (io_methods) and
 * 	extended interface (io_methods2).
 * 	[91/04/28  10:17:37  dpj]
 * 
 * 	First version.
 * 	[91/02/25  10:30:11  dpj]
 * 
 */

#ifndef lint
char * recordstream_rcsid = "$Header: recordstream.cc,v 2.5 94/07/07 17:24:04 mrt Exp $";
#endif	lint

#include	<recordstream_ifc.h>

extern "C" {
#include <interrupt.h>
}

/*
 * XXX This code should be able to rendez-vous between a writer and
 * reader that is currently asleep, to avoid extraneous copies when
 * possible.
 */

DEFINE_ABSTRACT_CLASS_MI(recordstream)
DEFINE_CASTDOWN2(recordstream, stream_base, usRecIO)

void recordstream::init_class(usClass* class_obj)
{
	usRecIO::init_class(class_obj);

	BEGIN_SETUP_METHOD_WITH_ARGS(recordstream);
	SETUP_METHOD_WITH_ARGS(recordstream,io_read1rec_seq);
	SETUP_METHOD_WITH_ARGS(recordstream,io_write1rec_seq);
	SETUP_METHOD_WITH_ARGS(recordstream,io_get_record_count);
	END_SETUP_METHOD_WITH_ARGS;
}

recordstream::recordstream(default_iobuf_mgr *mgr, int qsize, 
			   io_strategy_t readstrat, io_strategy_t writestrat)
	: stream_base(mgr, readstrat, writestrat),
	  qused(0), qmax(qsize), first_record(NULL), last_record(NULL)
{
	mutex_init(&Local(lock));
	condition_init(&put_cond);
	condition_init(&get_cond);
	INT_TO_IO_RECNUM(0,&first_recnum);
	INT_TO_IO_RECNUM(0,&next_recnum);
}


recordstream::~recordstream()
{
	io_record_t		currec;
	io_record_t		newrec;

	DESTRUCTOR_GUARD();

	currec = Local(first_record);
	while (currec != NULL) {
		newrec = currec->next;
		currec->next = NULL;
//		(void) io_free_record(Base,currec);
		(void) this->io_free_record(currec);
		currec = newrec;
	}
	condition_clear(&Local(put_cond));
	condition_clear(&Local(get_cond));
}


mach_error_t 
recordstream::io_getrec_seq(io_mode_t mode, io_record_t *rec, 
			    unsigned int *count,
			    io_recnum_t *recnum)
{
	mach_error_t		ret;
	unsigned int		mincount;
	io_record_t		nxtrec, prvrec;
	io_record_t		newrec;
	int			i;
	boolean_t		wakeup_readers;

	if (mode & ~(IOM_WAIT | IOM_PROBE | IOM_TRUNCATE)) {
		return(IO_INVALID_MODE);
	}

	mutex_lock(&Local(lock));

	if (! (Local(read_strategy) & IOS_ENABLED)) {
		mutex_unlock(&Local(lock));
		return(IO_REJECTED);
	}

	if (mode & IOM_TRUNCATE) {
		mincount = 1;
	} else {
		mincount = *count;
	}

	/*
	 * Wait until there are enough records on the queue.
	 */
	while (Local(qused) < mincount) {
		if (! (Local(read_strategy) & IOS_WAIT_ALLOWED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
		if (mode & IOM_WAIT) {
			ret = intr_cond_wait(&Local(get_cond),&Local(lock));
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&Local(lock));
				return(EXCEPT_SOFTWARE);
			}
		} else {
			mutex_unlock(&Local(lock));
			return(US_OBJECT_BUSY);
		}
		if (! (Local(read_strategy) & IOS_ENABLED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
	}
	*count = MIN(*count,Local(qused));
	*recnum = Local(first_recnum);

	if (mode & IOM_PROBE) {
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	/*
	 * Dequeue the records.
	 */
	nxtrec = Local(first_record);
	for (i = 0; i < *count; i++) {
		if (nxtrec == NULL) {
			mutex_unlock(&Local(lock));
			us_internal_error("io_getrec(): not enough records",
							US_INTERNAL_ERROR);
			return(US_INTERNAL_ERROR);
		}
		prvrec = nxtrec;
		nxtrec = nxtrec->next;
	}
	newrec = Local(first_record);
	prvrec->next = NULL;
	Local(first_record) = nxtrec;
	Local(qused) -= *count;
	if (Local(qused) == 0) {
		Local(last_record) = NULL;
	}
	ADD_LONG_TO_DLONG(&Local(first_recnum),*count);

	/*
	 * Decide if we should enable other readers.
	 */
	if (Local(qused) > 0) {
		wakeup_readers = TRUE;
	} else {
		wakeup_readers = FALSE;
	}

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up possible writers.
	 */
	condition_signal(&Local(put_cond));

	/*
	 * Wake-up possible readers.
	 */
	if (wakeup_readers) {
		condition_signal(&Local(get_cond));
	}

	*rec = newrec;

	return(ERR_SUCCESS);
}


mach_error_t 
recordstream::io_putrec_seq(io_mode_t mode, io_record_t *rec, 
			    unsigned int *count,
			    io_recnum_t *recnum)
{
	unsigned int		mincount;
	io_record_t		nxtrec, prvrec;
	int			i;
	boolean_t		wakeup_writers;
	mach_error_t		ret;

	if (mode & ~(IOM_WAIT | IOM_PROBE | IOM_TRUNCATE)) {
		return(IO_INVALID_MODE);
	}

	mutex_lock(&Local(lock));

	if (! (Local(write_strategy) & IOS_ENABLED)) {
		mutex_unlock(&Local(lock));
		return(IO_REJECTED);
	}

	if (mode & IOM_TRUNCATE) {
		mincount = 1;
	} else {
		mincount = *count;
	}

	/*
	 * Wait until there is enough space on the queue.
	 */
	while ((Local(qmax) - Local(qused)) < mincount) {
		if (! (Local(write_strategy) & IOS_WAIT_ALLOWED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
		if (mode & IOM_WAIT) {
			ret = intr_cond_wait(&Local(put_cond),&Local(lock));
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&Local(lock));
				return(EXCEPT_SOFTWARE);
			}
		} else {
			mutex_unlock(&Local(lock));
			return(US_OBJECT_BUSY);
		}
		if (! (Local(write_strategy) & IOS_ENABLED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
	}
	*count = MIN(*count,Local(qmax) - Local(qused));
	*recnum = Local(next_recnum);

	if (mode & IOM_PROBE) {
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	/*
	 * Extract the input records.
	 */
	nxtrec = *rec;
	for (i = 0; i < *count; i++) {
		if (nxtrec == NULL) {
			mutex_unlock(&Local(lock));
			return(IO_NOT_ENOUGH_DATA);
		}
		prvrec = nxtrec;
		nxtrec = nxtrec->next;
	}

	/*
	 * Enqueue the records.
	 */
	if (Local(qused) > 0) {
		Local(last_record)->next = *rec;
	} else {
		Local(first_record) = *rec;
		Local(first_recnum) = *recnum;
	}
	Local(last_record) = prvrec;
	prvrec->next = NULL;
	*rec = nxtrec;
	Local(qused) += *count;
	ADD_LONG_TO_DLONG(&Local(next_recnum),*count);

	/*
	 * Decide if we should enable other writers.
	 */
	if (Local(qused) < Local(qmax)) {
		wakeup_writers = TRUE;
	} else {
		wakeup_writers = FALSE;
	}

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up possible readers.
	 */
	condition_signal(&Local(get_cond));

	/*
	 * Wake-up possible writers.
	 */
	if (wakeup_writers) {
		condition_signal(&Local(put_cond));
	}

	return(ERR_SUCCESS);
}


mach_error_t recordstream::io_get_record_count(io_recnum_t *count)
{
	mutex_lock(&Local(lock));
	UINT_TO_IO_RECNUM(Local(qused),count);
	mutex_unlock(&Local(lock));

	return(ERR_SUCCESS);
}


mach_error_t 
recordstream::io_read1rec_seq(io_mode_t mode, char *buf, unsigned int *len,
			      io_recnum_t *recnum)
{
	mach_error_t		ret;

//	ret = io_read1rec_seq_with_info(Base,mode,buf,len,recnum,NULL);
	ret = this->io_read1rec_seq_with_info(mode,buf,len,recnum,NULL); // XXX C++

	return(ret);
}


mach_error_t 
recordstream::io_write1rec_seq(io_mode_t mode, char *buf, unsigned int len,
			       io_recnum_t *recnum)
{
	mach_error_t		ret;

//	ret = io_write1rec_seq_with_info(Base,mode,buf,len,recnum,NULL);
	ret = this->io_write1rec_seq_with_info(mode,buf,len,recnum,NULL); //XXX C++

	return(ret);
}


mach_error_t 
recordstream::io_read1rec_seq_with_info(io_mode_t mode, char *buf, 
					unsigned int *len,
					io_recnum_t *recnum, 
					io_recinfo_t *recinfo)
{
	mach_error_t		ret;
	io_record_t		rec;
	boolean_t		wakeup_readers;

	if (mode & ~(IOM_WAIT | IOM_PROBE)) {
		return(IO_INVALID_MODE);
	}

	mutex_lock(&Local(lock));

	if (! (Local(read_strategy) & IOS_ENABLED)) {
		mutex_unlock(&Local(lock));
		return(IO_REJECTED);
	}

	/*
	 * Wait until there is at least one record on the queue.
	 */
	while (Local(qused) < 1) {
		if (! (Local(read_strategy) & IOS_WAIT_ALLOWED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
		if (mode & IOM_WAIT) {
			ret = intr_cond_wait(&Local(get_cond),&Local(lock));
			if (ret != ERR_SUCCESS) {
				mutex_unlock(&Local(lock));
				return(EXCEPT_SOFTWARE);
			}
		} else {
			mutex_unlock(&Local(lock));
			return(US_OBJECT_BUSY);
		}
		if (! (Local(read_strategy) & IOS_ENABLED)) {
			mutex_unlock(&Local(lock));
			return(IO_REJECTED);
		}
	}

	/*
	 * Setup the OUT parameters.
	 */
	*recnum = Local(first_recnum);
	rec = Local(first_record);
	ret = iorec_read(rec,mode,buf,len);
	if (ret != ERR_SUCCESS) {
		mutex_unlock(&Local(lock));
		return(ret);
	}

	if (mode & IOM_PROBE) {
		mutex_unlock(&Local(lock));
		return(ERR_SUCCESS);
	}

	/*
	 * XXX Should we return the recinfo even in IOM_PROBE mode?
	 */
	if (recinfo != NULL) {
		io_recinfo_copy(&rec->recinfo,recinfo);
	}

	/*
	 * Dequeue the record.
	 */
	Local(first_record) = rec->next;
	Local(qused)--;
	if (Local(qused) == 0) {
		Local(last_record) = NULL;
	}
//	(void) io_free_record(Base,rec);
	(void) this->io_free_record(rec);
	ADD_LONG_TO_DLONG(&Local(first_recnum),1);

	/*
	 * Decide if we should enable other readers.
	 */
	if (Local(qused) > 0) {
		wakeup_readers = TRUE;
	} else {
		wakeup_readers = FALSE;
	}

	mutex_unlock(&Local(lock));

	/*
	 * Wake-up possible writers.
	 */
	condition_signal(&Local(put_cond));

	/*
	 * Wake-up possible readers.
	 */
	if (wakeup_readers) {
		condition_signal(&Local(get_cond));
	}

	return(ERR_SUCCESS);
}


mach_error_t 
recordstream::io_write1rec_seq_with_info(io_mode_t mode, char *buf, 
					 unsigned int len,
					 io_recnum_t *recnum, 
					 io_recinfo_t *recinfo)
{
	mach_error_t		ret;
	int			infosize;
	io_record_t		rec;
	io_block_t		blk;
	io_count_t		count = 1;

	if (mode & ~(IOM_WAIT | IOM_PROBE)) {
		return(IO_INVALID_MODE);
	}

	if ((mode & IOM_PROBE) == 0) {
//		ret = io_alloc_record(Base,len,&rec);
		ret = this->io_alloc_record(len,&rec);
		if (ret != ERR_SUCCESS) {
			return(ret);
		}
		if (recinfo != NULL) {
			if (rec->infosize < io_recinfo_size(recinfo)) {
				us_internal_error(
		"io_write1rec_seq_with_info(): rec->infosize too small",
							US_INTERNAL_ERROR);
//				(void) io_free_record(Base,rec);
				(void) this->io_free_record(rec);
				return(US_INTERNAL_ERROR);
			}
			io_recinfo_copy(recinfo,&rec->recinfo);
		}
		blk = rec->first_block;
		bcopy(buf,ioblk_start(blk),len);
		blk->end_offset += len;
	} else {
		rec = NULL;
	}

//	ret = io_putrec_seq(Base,mode,&rec,&count,recnum);
	ret = this->io_putrec_seq(mode,&rec,&count,recnum); // XXX C++

	if ((ret != ERR_SUCCESS) && ((mode & IOM_PROBE) == 0)) {
//		(void) io_free_record(Base,rec);
		(void) this->io_free_record(rec); // XXX C++
	}

	return(ret);
}


mach_error_t recordstream::io_set_read_strategy(io_strategy_t strategy)
{
	if (strategy & ~(IOS_ENABLED | IOS_WAIT_ALLOWED)) {
		return(IO_INVALID_STRATEGY);
	}

	mutex_lock(&Local(lock));
	Local(read_strategy) = strategy;
	mutex_unlock(&Local(lock));

	condition_broadcast(&Local(get_cond));

	return(ERR_SUCCESS);
}


mach_error_t recordstream::io_set_write_strategy(io_strategy_t strategy)
{
	if (strategy & ~(IOS_ENABLED | IOS_WAIT_ALLOWED)) {
		return(IO_INVALID_STRATEGY);
	}

	mutex_lock(&Local(lock));
	Local(write_strategy) = strategy;
	mutex_unlock(&Local(lock));

	condition_broadcast(&Local(put_cond));

	return(ERR_SUCCESS);
}


mach_error_t recordstream::io_flush_stream(void)
{
	io_record_t		currec;
	io_record_t		newrec;

	mutex_lock(&Local(lock));

	currec = Local(first_record);
	while (currec != NULL) {
		newrec = currec->next;
		currec->next = NULL;
//		(void) io_free_record(Base,currec);
		(void) this->io_free_record(currec); // XXX C++
		currec = newrec;
	}

	Local(qused) = 0;
	Local(first_record) = NULL;
	Local(last_record) = NULL;
	Local(first_recnum) = Local(next_recnum);

	mutex_unlock(&Local(lock));

	condition_broadcast(&Local(put_cond));
	condition_broadcast(&Local(get_cond));

	return(ERR_SUCCESS);
}

mach_error_t recordstream::io_read1rec(io_mode_t, io_recnum_t, char*, unsigned int*)
{
	return _notdef();
}

mach_error_t recordstream::io_write1rec(io_mode_t, io_recnum_t, char*, unsigned int)
{
	return _notdef();
}

