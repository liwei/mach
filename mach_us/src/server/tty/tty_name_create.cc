/*
 **********************************************************************
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
 **********************************************************************
 */
/*
 * tty_name_create.c
 *
 * Namespace initialization code.
 *
 * Michael B. Jones  --  05-Sep-1990
 */

/*
 * HISTORY:
 * $Log:	tty_name_create.cc,v $
 * Revision 2.4  94/07/21  16:15:02  mrt
 * 	Updated copyright
 * 
 * Revision 2.3  92/07/05  23:36:32  dpj
 * 	Removed undef of __cplusplus in extern "C" declarations.
 * 	[92/04/17  16:57:30  dpj]
 * 
 * Revision 2.2  91/11/06  14:24:20  jms
 * 	Update to use C++.
 * 	[91/09/17  14:23:17  jms]
 * 
 * Revision 2.2  90/10/02  11:38:30  mbj
 * 	Moved ns_create code here tty from tty_name.c.
 * 	[90/10/01  15:35:36  mbj]
 * 
 * Revision 1.3  90/09/05  09:46:00  mbj
 * 	Added ns naming code.  Enter ttys backed by the underlying 2.5 system
 * 	with "_htg" appended to their names (e.g. /dev/ttyp0_htg).  Enter
 * 	emulated ptys under normal names.  Dropped indirect /dev/tty support.
 * 	[90/09/04  15:25:42  mbj]
 * 
 * Revision 1.2  88/11/16  11:32:43  mbj
 * Build without -DKERNEL or multitudes of feature .h files.
 * 
 * Revision 1.1  88/10/28  01:28:59  mbj
 * Initial revision
 * 
 * 19-Oct-88  Michael Jones (mbj) at Carnegie-Mellon University
 *	Wrote it.
 */

extern "C" {
#include "tty_name.h"
#include <auth_defs.h>
}
#include "tty_dir_ifc.h"

extern tty_name tty_names[NTTYS];

/*
 * Default protection code stolen from fsadmin.c.
 */

#define	DFT_PROT_LEN	3
int dft_prot_data[NS_PROT_LEN(DFT_PROT_LEN)] = {
	NS_PROT_VERSION, 0, DFT_PROT_LEN,
	0, NSR_ALL,
};
int dft_protlen = NS_PROT_LEN(DFT_PROT_LEN);
ns_prot_t dft_prot = (ns_prot_t)dft_prot_data;

/*
 * ns_create all the tty names in the tty root directory.
 */
void create_tty_names(tty_dir *root)
{
    register int i;
    usItem *newobj;
    mach_error_t ret;

    /*
     * create a default prot entry
     */
    dft_prot->head.version = NS_PROT_VERSION;
    dft_prot->head.generation = 0;
    dft_prot->head.acl_len = DFT_PROT_LEN;

    dft_prot->acl[0].authid = SYS_ADM_ID;
    dft_prot->acl[0].rights = NSR_ALL;
    dft_prot->acl[1].authid = 2;	/* XXX anon group */
    dft_prot->acl[1].rights = NSR_ALL & ~NSR_ADMIN;
    dft_prot->acl[2].authid = DEFAULT_ID;
    dft_prot->acl[2].rights = NSR_ALL & ~NSR_ADMIN;

    for (i = 0; i < NTTYS; i++) {
	ret = root->ns_create(tty_names[i].name, NST_TTY,
		dft_prot, dft_protlen,
		0 /* access 0 means don't return newobj */ , &newobj);
    }
}
