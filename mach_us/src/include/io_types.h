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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/io_types.h,v $
 *
 * Author: Daniel P. Julin
 *
 * Purpose: Standard types for I/O interface -- basic interface.
 *
 */
/*
 * HISTORY:
 * $Log:	io_types.h,v $
 * Revision 1.12  94/07/08  15:51:18  mrt
 * 	Updated copyright.
 * 
 * Revision 1.11  91/05/05  19:23:32  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:43:24  dpj]
 * 
 * 	Reorganized for split between basic interface (io_methods) and
 * 	extended interface (io_methods2).
 * 	[91/04/28  09:21:58  dpj]
 * 
 * 	First pass at unified I/O interface for byte- and record-level operations.
 * 	[91/02/25  10:20:59  dpj]
 * 
 * Revision 1.10  89/11/28  19:08:13  dpj
 * 	Added ioblk_empty().
 * 	[89/11/22            dpj]
 * 
 * 	Added definitions for I/O strategies.
 * 	Removed IOA_* definitions, and io_stat_rec_t.
 * 	[89/11/20  20:23:51  dpj]
 * 
 * Revision 1.9  89/10/30  16:27:44  dpj
 * 	Merged-in IO_SIZE and IO_OFF conversion macros.
 * 	Added definitions for block I/O.
 * 	[89/10/27  16:28:02  dpj]
 * 
 * Revision 1.8  89/05/17  15:56:10  dorr
 * 	include file cataclysm
 * 
 * Revision 1.7  89/03/17  12:18:04  sanzi
 * 	Added io_count_t.
 * 	[89/02/22  14:39:20  dpj]
 * 	
 * 	define 64 bit offset in terms of ns data type
 * 	[89/02/16  11:48:10  dorr]
 * 	
 * 	Added typedefs for io_size_t and io_offset_t.
 * 	Nuked activation structure, recnum_t, and associated
 * 	fields in the stat structure.
 * 	[89/02/10  09:28:37  sanzi]
 * 	
 * 	Use io_attr_t for I/O attributes.
 * 	[89/02/01  11:12:16  dpj]
 * 	
 * 	Changes from dorr_newclasses
 * 	[89/01/12  18:39:25  dpj]
 * 
 * Revision 1.6.2.1  89/01/12  15:43:50  dorr
 * 	dan has returned.  go crazy
 * 
 * Revision 1.6.1.3  88/12/14  20:58:39  dpj
 * 	Last checkin before Xmas break
 * 
 * Revision 1.6.1.2  88/12/06  17:08:41  dpj
 * 	First consistent version
 * 
 * Revision 1.6.1.1  88/12/02  13:38:16  dpj
 * 	Checkin to rebuild the tree
 * 
 * Revision 1.6  88/10/27  16:38:48  mbj
 * Add temporary exclusive and nodelay "access" flags.
 * 
 * Revision 1.5  88/10/17  22:14:47  dpj
 * Added some definitions for use with the new I/O object system.
 * 
 * Revision 1.4  88/08/21  18:01:30  dpj
 * Merged the new I/O definitions into the mainline.
 * 
 * Revision 1.2.1.1  88/07/21  18:27:29  dpj
 * Totally different file for new I/O interface.
 * 
 * Revision 1.11  88/07/20  17:21:36  dpj
 * Added io_stat().
 * 
 * Revision 1.10  88/07/19  18:36:29  dpj
 * Use mach_error_t
 * 
 * Revision 1.9  88/07/19  17:17:06  dpj
 * io_reference/io_release + open/activate/setup
 * 
 * Revision 1.8  88/06/26  14:09:39  dpj
 * Fixed size of actrec.
 * 
 * Revision 1.7  88/06/21  19:00:46  dpj
 * Added buffersize to the activation record.
 * 
 * Revision 1.6  88/06/20  12:31:41  dpj
 * Reorganized RCS history
 * 
 *  09-Jun-88  Daniel Julin (dpj) at Carnegie-Mellon University
 *	Created.
 */

#ifndef	_IO_TYPES_H_
#define	_IO_TYPES_H_

#include	<mach_error.h>
#include	<cthreads.h>

#include	<base.h>
#include	<io_error.h>
#include	<ns_types.h>
#include	<dlong.h>


/*
 * Count of bytes for data transfers.
 */
typedef vm_size_t		io_count_t;


/*
 * I/O modes to control the behavior of individual I/O operations.
 */
typedef	unsigned int	 	io_mode_t;
#define	IOM_PROBE		0x1
#define	IOM_TRUNCATE		0x2
#define	IOM_WAIT		0x4
#define	IOM_APPEND		0x8


/*
 * I/O strategies to control the global behavior of I/O objects (for
 * all clients and all operations).
 *
 * XXX This is mostly an ad-hoc definition to deal with UNIX pipes.
 * See if we can generalize this better.
 */
typedef unsigned int		io_strategy_t;
#define	IOS_ENABLED		0x1
#define	IOS_WAIT_ALLOWED	0x2


/*
 * Various 64-bit quantities used in the specification of data elements.
 *
 * XXX All this should be re-defined with some form of abstract
 * dlong type, and derived types.
 *
 * XXX The macros below specify their args in the same order as they
 * are used in the name of the macro itself. The dlong routines seem
 * to always have the result as the first argument. That is confusing...
 */

/*
 * I/O size.
 *
 * XXX Inherited from the name service interface, because of
 * confusion with the standard attributes structure.
 */
typedef ns_size_t		io_size_t;
#define	UINT_TO_IO_SIZE(i,iosp)	u_int_to_dlong(iosp, i)
#define	INT_TO_IO_SIZE(i,iosp)	int_to_dlong(iosp, i)
#define	IO_SIZE_TO_INT(ios,intp) {*(intp) = dlong_to_int(ios);}
#define	IO_SIZE_TO_UINT(ios,intp) {*(intp) = dlong_to_u_int(ios);} 

/*
 * I/O offset.
 */
typedef struct dlong_t		io_offset_t;
#define	UINT_TO_IO_OFF(i,iosp)	u_int_to_dlong(iosp, i)
#define	INT_TO_IO_OFF(i,iosp)	int_to_dlong(iosp, i)
#define	IO_OFF_TO_INT(ios,intp)	{*(intp) = dlong_to_int(ios);}
#define	IO_OFF_TO_UINT(ios,intp) {*(intp) = dlong_to_u_int(ios);} 

/*
 * I/O record number.
 */
typedef struct dlong_t		io_recnum_t;
#define	UINT_TO_IO_RECNUM(i,iornp)	u_int_to_dlong(iornp, i)
#define	INT_TO_IO_RECNUM(i,iornp)	int_to_dlong(iornp, i)
#define	IO_RECNUM_TO_INT(iorn,intp)	{*(intp) = dlong_to_int(iorn);}
#define	IO_RECNUM_TO_UINT(iorn,intp) {*(intp) = dlong_to_u_int(iorn);}
#define	INCREMENT_IO_RECNUM(iornp)	ADD_LONG_TO_DLONG(iornp,1)


#endif	_IO_TYPES_H_
