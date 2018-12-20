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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/lib/us++/top.cc,v $
 *
 * Purpose: UsTop class. Replaces base.c
 *
 * HISTORY
 * $Log:	top.cc,v $
 * Revision 2.6  94/07/07  17:25:00  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  94/06/16  17:19:14  mrt
 * 	Add USSTATS stuff from DPJ.
 * 	[94/05/25  13:15:36  jms]
 * 
 * Revision 2.4  93/01/20  17:38:24  jms
 * 	Implement the GXXBUG_DELETE3 g++ buq workaround to do object memory freeing
 * 	in top::dealloc_proc instead of gnulib3::_buildin_delete_
 * 
 * 	Add some tracing/debugging stuff
 * 	[93/01/18  17:07:15  jms]
 * 
 * Revision 2.3  92/07/05  23:29:39  dpj
 * 	Reorganized destructor guard (under GXXBUG_DELETE2) to reduce
 * 	the amount of debugging output.
 * 	[92/07/05  18:58:34  dpj]
 * 
 * 	No further changes.
 * 	[92/06/29  23:04:01  dpj]
 * 
 * 	Added compiler workarounds (GXXBUG).
 * 	Made _notdef() print an error instead of a debugging warning.
 * 	[92/06/24  17:16:37  dpj]
 * 
 * 	Converted to new C++ RPC package.
 * 	[92/05/10  01:01:33  dpj]
 * 
 * Revision 2.2  91/11/06  13:48:55  jms
 * 	C++ revision - upgraded to US41
 * 	[91/09/27  12:41:16  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:38:57  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  15:54:59  pjg]
 * 
 */

#include <top_ifc.h>
#include <logging.h>

int	ustrace_alloc = 1;

usClass usTop::_class("usTop", 0, 0, usTop::desc(), 0, 0);


usClass::usClass(const char* class_name,
		 void* (*constructor_func)(void*, const usClass&),
		 void* (*converter_func)(void*),
		 const usClass* class_desc,
		 void(*f)(usClass*),
		 class usTop* instance)
	: name(class_name), rem_class_name("INVALID_CLASS"),
	  _virtual_constructor(constructor_func),
	  _converter_to_remote(converter_func),
	  _class_descriptor(class_desc),
	  _test_instance(instance)
{
	if (class_info::GLOBAL == 0)
		class_info::GLOBAL = new class_info;
	if (method_info::GLOBAL == 0)
		method_info::GLOBAL = new method_info;

	if (f) {
		aux_table = hash_init(0,0,64);
		method_table = hash_init(0,0,256);
		(void) (*f)(this);
		hash_free(aux_table);
		aux_table = 0;
	}
	class_info::GLOBAL->_insert_class((char*)name, (void*)this);
}

boolean_t usClass::is_init(usClass* class_obj)
{
	boolean_t ret;
	if (aux_table) {
		ret = hash_enter(aux_table,(hash_key_t)class_obj,0);
		ret = (ret) ? 0 : 1;
	}
	else
		ret = 1;
	return ret;
}

void* usClass::virtual_constructor(mach_port_t p, const usClass& c)
{
	if (_virtual_constructor) {
		return (*_virtual_constructor)(p, c);
	} else {
		return 0;
	}
}

void* usClass::converter_to_remote(void* p)
{
	if (_converter_to_remote) {
		return (*_converter_to_remote)(p);
	} else {
		return 0;
	}
}

const usClass* usClass::class_descriptor(void)
{
	return _class_descriptor;
}

void usClass::set_method(
	mach_method_id_t	mid,
	obj_method_entry_t	entry)
{
	if (! hash_enter(method_table,(hash_key_t)mid,(hash_value_t)entry)){
		hash_remove(method_table,(hash_key_t)mid);
		hash_enter(method_table,(hash_key_t)mid,(hash_value_t)entry);
	}
}

usTop::usTop() : ref_cnt(1)
{
//	state_lock.lock = 0;
//	state_lock.name = (char*) 0;
	obj_state_lock_init(&state_lock);
//	diag_level = Dbg_Level_Max;
//	diag_name = (char*) className();
	USSTATS(USSTATS_TOP_ALLOC);
	DEBUG0(ustrace_alloc,(0,"USTRACE: usTop object allocate: %s",
			      is_a()->class_name()));
}

usTop::~usTop()
{
#if PRINT_TOP_THIS
    printf("usTop::~usTop:this=0x%x, size=0x%x, class=%s\n",
		this, sizeof(*this), is_a()->class_name());
#endif PRINT_TOP_THIS
	USSTATS(USSTATS_TOP_DEALLOC);
	DEBUG0(ustrace_alloc,(0,"USTRACE: usTop object deallocate: %s",
			      is_a()->class_name()));
}

void usTop::reference_proc()
{
	obj_state_lock(&state_lock);
	ASSERT_EXPR("usTop::reference() dead object",ref_cnt > 0);
	ref_cnt++;
	LOG3(TRUE, 3002, ref_cnt, this, is_where());
	obj_state_unlock(&state_lock);
}

#if CHECK_LASTONE
extern struct mutex lastone_lock;
extern char * lastone;
extern int lastone_id;
#endif CHECK_LASTONE

#define SHOW_DEAD 1
static int deref_num = 0;
void usTop::dereference_proc(){
	char	*mem_addr;
#if PRINT_TOP_THIS
	int i;
#endif PRINT_TOP_THIS	
	obj_state_lock(&state_lock);
#if SHOW_DEAD
	if (ref_cnt <= 0) {
	    printf("SHOW_DEAD:this=0x%x, size=0x%x, is_where=0x%x, class=%s\n",
		this, sizeof(*this), is_where(), is_a()->class_name());
	    printf("Suspending...\n");
	    task_suspend(mach_task_self());
	}
#endif SHOE_DEAD
	ASSERT_EXPR("usTop::dereference() dead object",ref_cnt > 0);
	if (ref_cnt == 1) {
		ref_cnt = 0;
		obj_state_unlock(&state_lock);
#ifdef	GXXBUG_DELETE1
		_virtual_delete();
#else	GXXBUG_DELETE1
	mem_addr = is_where();
#if PRINT_TOP_THIS
	i = deref_num++;
	printf("pre_delete(%d):this=0x%x, size=0x%x, is_where=0x%x, class=%s\n",
		i, this, sizeof(*this), mem_addr, is_a()->class_name());
#endif PRINT_TOP_THIS
	delete this;

#ifdef GXXBUG_DELETE3
	LOG2(TRUE, 3001, this, mem_addr);
#if CHECK_LASTONE
	mutex_lock(&lastone_lock);
	lastone_id++;
	LOG5(TRUE, 3004, lastone_id, lastone, ref_cnt, this, mem_addr);

	if (mem_addr == lastone) {
		lastone=0;
	}

	free(mem_addr);
	LOG4(TRUE, 3005, lastone_id, lastone, this, mem_addr);
	mutex_unlock(&lastone_lock);
#else CHECK_LASTONE
	free(mem_addr);
#endif CHECK_LASTONE
#endif GXXBUG_DELETE3
#if PRINT_TOP_THIS
	printf("post_delete(%d)\n",i);
#endif PRINT_TOP_THIS

#endif	GXXBUG_DELETE1
	} else {
		ref_cnt--;
		LOG3(TRUE, 3003, ref_cnt, this, is_where());
		obj_state_unlock(&state_lock);
	}

}

int usTop::refcount()
{
	obj_state_lock(&state_lock);
	int i = ref_cnt;
	obj_state_unlock(&state_lock);
	return i;
}

void* usTop::_castdown(const usClass& c) const
{
	if (&c == desc()) return (void*) this;
	return 0;
}

void usTop::ambig_check(void*& p, void*& q, const usClass& c) const
{
        if (p == 0 || p == q) return;
        if (q == 0) { q = p;  return; }
	ERROR(("Ambiguous castdown: 0x%x, %s->%s\n",
				this, class_name(), c.class_name()));
	q = 0;
}

#ifdef	GXXBUG_CLONING1
#else	GXXBUG_CLONING1
mach_error_t usTop::clone_init(mach_port_t p)
{
	return _notdef();
}

mach_error_t usTop::clone_abort(mach_port_t p)
{
	return _notdef();
}

mach_error_t usTop::clone_complete()
{
	return _notdef();
}
#endif	GXXBUG_CLONING1

mach_error_t usTop::_notdef()
{
	ERROR((Diag,"Operation not defined !!\n"));
	return MACH_OBJECT_NO_SUCH_OPERATION;
}


#ifdef	GXXBUG_DELETE2

#if	_DEBUG_
int	_destructor_guard::verbose	= 1;
int	gxxbug_delete2_verbose = 0;
#else	_DEBUG_
int	_destructor_guard::verbose	= 0;
#endif	_DEBUG_

boolean_t _destructor_guard::check()
		{
			/*
			 * No need for a lock because we are only worried
			 * about a bug in a single-threaded run of "delete".
			 */
			if (count != 0) {
				switch (verbose) {
					case 0:	break;
					case 1:
						if (count == 1) {
							DEBUG1(TRUE,(0,
	"destructor_guard triggered (possibly more than once) for 0x%x",
								this));
						}
						break;
					case 2:
						DEBUG1(TRUE,(0,
				"destructor_guard triggered once for 0x%x",
								this));
						break;
				}
				count += 1;
				return(TRUE);
			} else {
				count = 1;
				return(FALSE);
			}
		}

#endif	GXXBUG_DELETE2
