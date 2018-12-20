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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/diag.cc,v $
 *
 * Purpose: Diagnostic and error messages system.
 *
 * HISTORY:
 * $Log:	diag.cc,v $
 * Revision 2.6  94/07/07  17:23:08  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  94/06/16  17:19:07  mrt
 * 	Add USSTATS stuff from DPJ.
 * 	[94/05/25  13:13:29  jms]
 * 
 * Revision 2.4  92/07/05  23:27:05  dpj
 * 	Re-enabled diag_init_mesg(). I don't know why it was ever disabled.
 * 	[92/07/05  18:53:35  dpj]
 * 
 * 	Added cloning functions under GXXBUG_CLONING1.
 * 	[92/06/24  16:09:32  dpj]
 * 
 * 	Added extern declarations for C++.
 * 	[92/05/10  00:51:38  dpj]
 * 
 * Revision 2.3  92/03/05  15:05:27  jms
 * 	Upgrade to use no MACH_IPC_COMPAT features.  New IPC only!
 * 	[92/02/26  18:29:55  jms]
 * 
 * Revision 2.2  91/11/06  13:45:48  jms
 * 	Upgraded to US41 and fixed some bugs introduced by the
 * 	conversion to C++.
 * 	[91/10/03  15:06:25  pjg]
 * 
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  11:28:37  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:35:20  pjg]
 * 
 * Revision 2.6  91/07/01  14:11:45  jms
 * 	Fixed for MACH3_UNIX, lazy initialization, etc.
 * 	Added mgr_dump_machobj_statistics(), mgr_reset_machobj_statistics().
 * 	[91/06/21  17:20:18  dpj]
 * 
 * 	Re-worked to allow lazy initialization of diag-server connection.
 * 	Removed cloning methods. We now use diag_post_fork() instead.
 * 	[91/06/16  21:06:44  dpj]
 * 
 * Revision 2.5  91/05/05  19:25:57  dpj
 * 	Merged up to US39
 * 	[91/05/04  09:53:09  dpj]
 * 
 * 	Added diag_us_internal_error() and support variables.
 * 	[91/04/28  10:01:42  dpj]
 * 
 * Revision 2.4  90/10/29  17:32:15  dpj
 * 	Merged-up to U25
 * 	[90/09/02  20:02:13  dpj]
 * 
 * 	Use vsprintf() instead of _doprnt() for compatibility
 * 	with new versions of libc.a.
 * 	[90/08/02  10:25:53  dpj]
 * 
 * Revision 2.3  90/08/22  18:12:25  roy
 * 	Use vsprintf() to fill diag messages.
 * 	[90/08/14  11:59:53  roy]
 * 
 * Revision 2.2  89/07/09  14:18:46  dpj
 * 	Initial revision.
 * 	[89/07/08  12:53:08  dpj]
 * 
 */


#include <diag_ifc.h>

extern "C" {

#include <stdio.h>
#include <base.h>
#include <debug.h>
#include <us_statistics.h>
#include <us_error.h>

void vsprintf();
void diag_mesg();

}

/*
 * Global variables exported by the DIAG system.
 */
int	us_debug_level = 0;
int	us_debug_force = 0;
int	us_debug_unimplemented_functions =1;

diag 	*_us_diag_object;
char 	*us_diag_object =(char*)_us_diag_object;

#undef Diag
#define Diag (_us_diag_object)

/*
 * Global variables used only within the DIAG system.
 */
static int			diag_ready = 0;
static struct mutex		diag_lock = MUTEX_INITIALIZER;
static int			diag_mach3_ready = 0;
static int			diag_use_printf = 0;


/*
 * Things imported from the libmach3 output system.
 */
extern "C" {
extern mach_port_t		mach3_diag_port;
extern mach_error_t		mach3_output_diag();
#if	! MACH3_UNIX
extern mach_error_t		mach3_diag_fork_child();
#endif	! MACH3_UNIX

}


/*
 * Internal macros.
 */
#define	DIAG_BODY(_obj,_mesg,_level) {				\
	char			string[1024];			\
	char			*str = string;			\
	mach_error_t		ret;				\
								\
	/*							\
	 * Check initialization.				\
	 */							\
	if (diag_ready == 0) {					\
		us_init_diag();					\
	}							\
	if (diag_mach3_ready == 0) {				\
		diag_mach3_init();				\
	}							\
								\
	/*							\
	 * Copy in the name.					\
	 */							\
	*str++ = '[';						\
	ret = _obj->diag_init_mesg(&str,_level);		\
	if (ret == US_ACCESS_DENIED) return;			\
	if (ret != ERR_SUCCESS) {				\
		*str++ = '?';					\
		*str++ = '?';					\
		*str++ = '?';					\
	}							\
	*str++ = ']';						\
	*str++ = ' ';						\
								\
	/*							\
	 * Prepare the rest of the message.			\
	 */							\
	vsprintf(str, _mesg, &((int *)&_mesg)[1]);		\
								\
	if (mach3_diag_port != MACH_PORT_NULL) {			\
		diag_mesg(mach3_diag_port,_level,string);	\
	} else {						\
		fprintf(stderr,"<%d> %s\n",_level,string);	\
	}							\
}



/*
 * Global functions exported by the DIAG system.
 */


/*
 * Make sure that the DIAG system has been initialized.
 */

void us_init_diag()
{
	mutex_lock(&diag_lock);
	if (diag_ready == 0) {
		_us_diag_object = new diag;
		diag_ready = 1;
	}
	mutex_unlock(&diag_lock);
}

void diag_mach3_init()
{
	char		name[256];

	mutex_lock(&diag_lock);
	if (diag_mach3_ready == 0) {
		if (diag_use_printf == 0) {
			(void) Diag->get_diag_name(name);
			(void) mach3_output_diag("",name);
		}
		diag_mach3_ready = 1;
	}
	mutex_unlock(&diag_lock);
}

/*
 * Report an internal problem from us_internal_error() macro.
 */

void diag::_us_internal_error(char *str, mach_error_t err, char *filename,
			     int lineno)
{
	if ((err == US_NOT_IMPLEMENTED)
				&& (!us_debug_unimplemented_functions)) {
		return;
	}
	diag_error(Diag,"US INTERNAL ERROR at %s.%d -- %s: %s",
		   filename,lineno,str,mach_error_string(err));
}

void diag_us_internal_error(char *str, mach_error_t err, char *filename,
			     int lineno)
{
	Diag->_us_internal_error(str, err, filename, lineno);
}

#if	_STATISTICS_
void diag::usstats_reset()
{
	int i;

	mutex_lock(&us_stats_lock);
	for (i = 0; i <= USSTATS_MAX; i++)
	    us_stats[i] = 0;
	mutex_unlock(&us_stats_lock);
}


void diag::usstats_dump()
{
	int i;

	mutex_lock(&us_stats_lock);
	diag_info(Diag,"US STATISTICS DUMP BEGIN");
	for (i = 0; i <= USSTATS_MAX; i++)
	    diag_info(Diag,"    US STATISTICS - %s: %d",
		      us_stats_names[i],us_stats[i]);
	diag_info(Diag,"US STATISTICS DUMP END");
	mutex_unlock(&us_stats_lock);
}
#endif	_STATISTICS_

/*
 * Generate a critical message, also written to stderr.
 */
void diag_critical(diag* obj, char* mesg)
{
	char			global_name[1024];
	char			string[1024];
	char			*str = string;
	mach_error_t		ret;

	obj = (diag*) Diag;

	/*
	 * Copy in the name.
	 */
	*str++ = '[';
	ret = obj->diag_init_mesg(&str,Dbg_Level_Critical);
	if (ret != ERR_SUCCESS) {
		*str++ = '?';
		*str++ = '?';
		*str++ = '?';
	}
	*str++ = ']';
	*str++ = ' ';

	/*
	 * Prepare the rest of the message.
	 */
	vsprintf(str, mesg, &((int *)&mesg)[1]);

	Diag->get_diag_name(global_name);
	fprintf(stderr,"*** %s *** %s\n",global_name,string);

	if (mach3_diag_port != MACH_PORT_NULL) {
		diag_mesg(mach3_diag_port,Dbg_Level_Critical,string);
	}
}


/*
 * Generate an error message.
 */
void diag_error(diag* obj, char* mesg)
{
	DIAG_BODY(Diag,mesg,Dbg_Level_Error);
}


/*
 * Generate an information message.
 */
void diag_info(diag* obj, char* mesg)
{	
	DIAG_BODY(Diag,mesg,Dbg_Level_Info);
}


/*
 * Debugging messages.
 */
void diag_debug0(diag* obj, char* mesg)
{	
	DIAG_BODY(Diag,mesg,Dbg_Level_0);
}

void diag_debug1(diag* obj, char* mesg)
{
	DIAG_BODY(Diag,mesg,Dbg_Level_1);
}

void diag_debug2(diag* obj, char* mesg)
{
	DIAG_BODY(Diag,mesg,Dbg_Level_2);
}


/*
 * Replacement for printf().
 */
void diag_dprintf(char* mesg)
{
	char			string[1024];
	char			*str = string;

	vsprintf(str, mesg, &((int *)&mesg)[1]);

	fprintf(stderr,"<diag_dprintf> %s\n",string);
}


/*
 * Obsolete translation routine.
 */
void diag_format(char* mesg ...)
{
	char			string[1024];
	char			*str = string;
	mach_error_t		ret;

	/*
	 * Copy in the name.
	 */
	*str++ = '[';
	ret = Diag->diag_init_mesg(&str,Dbg_Level_0);
	if (ret == US_ACCESS_DENIED) return;
	if (ret != ERR_SUCCESS) {
		*str++ = '?';
		*str++ = '?';
		*str++ = '?';
	}
	*str++ = ']';
	*str++ = ' ';

	/*
	 * Prepare the rest of the message.
	 */
	vsprintf(str, mesg, &((int *)&mesg)[1]);

	fprintf(stderr,"<diag_format> %s\n",string);
}


/*
 * Initialize the DIAG system with a given name.
 */
mach_error_t diag_startup(char* name)
{
	mach_error_t		ret;

	if (diag_ready == 0) {
		us_init_diag();
	}

	ret = Diag->set_diag_name(name);

	(void) diag_mach3_init();

	return(ret);
}

mach_error_t diag_startup_lazy(char *name)
{
	mach_error_t		ret;

	if (diag_ready == 0) {
		us_init_diag();
	}

	ret = Diag->set_diag_name(name);

	return(ret);
}

mach_error_t diag_startup_printf(char *name)
{
	diag_use_printf = 1;
	return(diag_startup(name));
}



extern "C" mach_error_t diag_fork_child();

/*
 * Reset the DIAG system after a fork().
 *
 * This cannot be done as part of normal cloning, because we want to
 * have the diag facilities available during cloning itself.
 */
mach_error_t diag_fork_child()
{
	if (diag_ready == 0) {
		us_init_diag();
	}
#if	! MACH3_UNIX
	mach3_diag_fork_child();
#endif	! MACH3_UNIX

	return(ERR_SUCCESS);
}

diag::diag()
{
	mach_error_t		ret;
	static char		*base_name = "DIAG";
	
	global_diag_level = Dbg_Level_Max;/* highest level, by default */
	strcpy(global_diag_name,"NoName");
}


mach_error_t diag::set_diag_level(int level)
{
	global_diag_level = level;
	us_debug_level = level;

	return(ERR_SUCCESS);
}

mach_error_t set_diag_level(int new_debug_level)
{
	return (Diag->set_diag_level(new_debug_level));
}


mach_error_t diag::get_diag_level(int* level)
{
	*level = global_diag_level;
	return(ERR_SUCCESS);
}


mach_error_t diag::set_diag_name(char* name)
{
	mach_error_t		ret;

	strncpy(global_diag_name,name,256);
	global_diag_name[255] = '\0';
	return(ERR_SUCCESS);
}


mach_error_t diag::get_diag_name(char* name)
{
	strcpy(name, global_diag_name);
	return(ERR_SUCCESS);
}


mach_error_t diag::clone_init(mach_port_t child)
{
	return ERR_SUCCESS;
}

#ifdef	GXXBUG_CLONING1
mach_error_t diag::clone_abort(mach_port_t child)
{
	return ERR_SUCCESS;
}

mach_error_t diag::clone_complete()
{
	return ERR_SUCCESS;
}

#endif	GXXBUG_CLONING1

mach_error_t diag::diag_init_mesg(char** ptr,/* INOUT */ int level)
{
#ifdef	notdef
	char			*cp1;
	char			*cp2;
#endif	notdef

//return(ERR_SUCCESS);

	if (_Local(global_diag_level) < level) {
		return(US_ACCESS_DENIED);
	}

	sprintf(*ptr,"%s (0x%x)",_Local(global_diag_name),this);
	*ptr += strlen(*ptr);

#ifdef	notdef
	cp1 = *ptr;
	cp2 = _Local(global_diag_name);
	while (*cp2) {
		*cp1++ = *cp2++;
	}
	*ptr = cp1;
#endif	notdef

	return(ERR_SUCCESS);
}

#if	_STATISTICS_
/*
 * Statistics information.
 */

struct mutex	us_stats_lock = MUTEX_INITIALIZER;

int	us_stats[USSTATS_MAX + 1];

char* 	us_stats_names[] = {
	"usTop object allocate",
	"usTop object deallocate",
	"raw object allocate",
	"raw object deallocate",
	"incoming RPC",
	"outgoing RPC",
	"abort RPC",
	"notification: no senders",
	"notification: port dead",
	"castdown",
	0
    };
#endif	_STATISTICS_
