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
 * File: $Source: /afs/cs.cmu.edu/project/mach/mach3/rcs/us/include/top_ifc.h,v $
 *
 * Purpose: Top class. Replaces base.
 *
 * HISTORY:
 * $Log:	top_ifc.h,v $
 * Revision 2.6  94/07/08  15:51:44  mrt
 * 	Updated copyright.
 * 
 * Revision 2.5  94/06/16  17:13:09  mrt
 * 	Add USSTATS stuff from DPJ.
 * 
 * 	Fix no debug version of the (de)reference stuff to correspond to fixes in the
 * 	debug version.
 * 	[94/05/25  13:11:50  jms]
 * 
 * Revision 2.4  94/05/17  14:06:04  jms
 * 	Change over to new 2.3.3 g++ compiler
 * 	[94/04/28  18:02:59  jms]
 * 
 * Revision 2.3.1.2  94/02/18  14:11:09  modh
 * 	dummy
 * 
 * Revision 2.3.1.1  94/02/18  11:17:50  modh
 * 	Change over to new 2.3.2 g++ compiler
 * 
 * Revision 2.3  93/01/20  17:36:18  jms
 * 	Add GXXBUG_DELETE3 compilation flag to turn on deletion of object memory from
 * 	usTop::dereference_proc instead of .../lib/c++/gnulib.c::_builtin_delete.
 * 	g++ bug workaround.
 * 	[93/01/18  15:41:23  jms]
 * 
 * Revision 2.2  92/07/05  23:23:20  dpj
 * 	Reorganized destructor guard (under GXXBUG_DELETE2) to reduce the amount
 * 	of debugging output. Moved some functions to top.cc to allow easier
 * 	re-compilation.
 * 	[92/07/05  18:50:17  dpj]
 * 
 * 	Added various GXXBUG conditionals to work around compiler problems.
 * 	Re-organized the class hierarchy to remove many virtual base class
 * 	specifications.
 * 	[92/06/24  13:22:45  dpj]
 * 
 * 	New version for C++ RPC package.
 * 	[92/05/10  00:18:49  dpj]
 * 
 * Revision 2.2  91/11/06  14:13:46  jms
 * 	Initial C++ revision.
 * 	[91/09/26  18:33:10  pjg]
 * 
 * 	Upgraded to US38
 * 	[91/04/14  18:39:18  pjg]
 * 
 * 	Initial C++ revision.
 * 	[90/11/14  17:15:09  pjg]
 * 
 */


#ifndef	_top_ifc_h
#define	_top_ifc_h

/*
 * Special controls to workaround compiler bugs.
 */

/*
 * XXX g++-1.37.1 seems broken w.r.t. destructors
 * with virtual base classes.
 *
 * "delete this" in the base class uses the wrong
 * "this" pointer, and we get a malloc() failure.
 *
 * Using virtual destructors eliminates the malloc()
 * failure, but the space is not always freed, and
 * the wrong destructors are sometimes called
 * (e.g. multiple calls to outer destructor).
 */
//#define	GXXBUG_DELETE1		1

/*
 * XXX g++-1.37.1 seems broken w.r.t. destructors
 * with virtual base classes.
 *
 * Some destructors execute multiple times.
 *
 * Solution: use DESTRUCTOR_GUARD macro where appropriate.
 */
#define	GXXBUG_DELETE2		1

/*
 * XXX g++-1.37.1 seems broken w.r.t. destructors
 * with virtual base classes.
 *
 * Some destructors never called or called on a size that is to small
 *
 * Solution: explicitly free the object in usTop::dereference_proc.
 *		and cause the .../lib/c++/gnulib.c::_builtin_delete
 *		to do nothing.
 *
 * Note: This requires strict adherence to our rule that all objects
 *		are derived from usTop or are members of objects which
 *		derive from usTop.  I.E. a "new" cannot be done on any
 *		object that does not derive from usTop.
 */
#define	GXXBUG_DELETE3		1

/*
 * XXX g++-1.37.1 broken w.r.t. pointers to member functions.
 *
 * "&foo_class::method" does not yield a useful function pointer.
 * Instead, it looks like we get a combination of the constant offset
 * in the vtable and the "this" offset.
 *
 * "&(foo->method)" or "&(foo->foo_class::method)" crashes the
 * compiler if "foo-class" is any of the proxy classes in lib/us++,
 * but not for other classes. Removing the "virtual" keyword in the
 * inheritance specification for "foo_class" fixes the problem.
 * "&(foo.method)" or "&(foo.foo_class::method)" is OK.
 */
#define	GXXBUG_PFUNC1		1

/*
 * g++-1.37.1 broken w.r.t virtual functions with virtual base classes.
 *
 * In certain circumstances, when invoking a virtual function defined both
 * in a virtual base class and in a derived but not outermost class, the
 * implementation from the virtual base class is actually used instead of
 * the overriding implementation from the derived class.
 *
 * Solution: define the virtual function in the outermost class.
 */
#define	GXXBUG_VIRTUAL1


/*
 * g++-1.37.1 broken w.r.t virtual functions overriding.
 *
 * Sometimes, a derived class fails to override a virtual function inherited
 * from a base class. This seems to be related to the base class itself
 * having multiple base classes (some of them virtual), and thus multiple
 * virtual tables.
 *
 * Solution: eliminate multiple-inheritance of abstract interface classes
 * when needed.
 */
#define	GXXBUG_VIRTUAL2


/*
 * g++-137.1. broken w.r.t virtual base classes.
 *
 * All kinds of things go wrong when using virtual bases classes. Destructors
 * get called more than once or never. Virtual tables get corrupted.
 * "delete" breaks. Trying to reduce the number of virtual classes
 * actually makes some things worse.
 *
 * Solution: try different combinations of virtual declarations.
 * The "all virtual solution" appeared to work for a while, but seems
 * too much.
 */
//#define	GXXBUG_VIRTUAL3

/*
 * g++-1.37.1. broken w.r.t virtual functions for cloning.
 *
 * When there are dummy cloning functions in usTop, we are sometime
 * unable to override them in derived classes (for proxies).
 *
 * Solution: define a new usClone abstract class that defines the cloning
 * function, and is only inherited once. THIS IS NOT JUST A WORKAROUND:
 * ITS GOOD CODE.
 *
 * Secondary problem: usClone should really derive from usRemote
 * (or even usTop), but this would force usItem_proxy to use MI to inherit
 * from usItem. To avoid more MI, we make usClone derive from usItem.
 */
#define	GXXBUG_CLONING1


/*
 * g++-1.37.1 broken w.r.t. virtual base classes with usClone.
 *
 * uxsignal does not compile when derived independently from usEvent
 * and usClone.
 *
 * Workaround: put usClone is the SI hierarchy for usEvent.
 */
#define	GXXBUG_CLONING2


/*
 * g++-2.3.2 broken w.r.t. static member values (not funcs).
 *
 * Workaround: declare magic g++ symbol name globally.
 */
#define GXXBUG_STATIC_MEM


/*
 * Special declarations to control the use of virtual base classes.
 */
#ifdef	GXXBUG_VIRTUAL3
#define	VIRTUAL1		virtual
#define	VIRTUAL2		virtual
#define	VIRTUAL3		virtual
#define	VIRTUAL4		virtual
#define	VIRTUAL5		virtual
#else	GXXBUG_VIRTUAL3
#define	VIRTUAL1		/* normal, usRemote */
#define	VIRTUAL2		virtual /* usItem et al. */
#define	VIRTUAL3		/* others */
#define	VIRTUAL4		/* usTop for iobufs */
#define	VIRTUAL5		virtual /* proxies */
#endif	GXXBUG_VIRTUAL3


extern "C" {
#include	<cthreads.h>
#include	"us_statistics.h"
#include	"debug.h"
}

extern "C" {
#include	<base.h>
#include	<hash.h>
#include	<us_error.h>
#include	<mach_object_error.h>
#include	<mach.h>
#include	<interrupt.h>
}

#include	<class_info_ifc.h>
#include	<method_info_ifc.h>

/*
 * Basic definitions for constructing complex names.
 */
#ifdef	__STDC__
#define	_CAT(x,y) x##y
#define	_CAT1(x,y) x##_##y
#define	_CAT2(x,y) x##__##y
#define	_CAT3(x,y,z) x##y##z
#define	_CAT3_(x,y,z) x##_##y##_##z
#define _STR(x) #x
#define _STRCAT(x,y) _STR(x##y)
#else	__STDC__
#define	_CAT(x,y) x/**/y
#define	_CAT1(x,y) x/**/_/**/y
#define	_CAT2(x,y) x/**/__/**/y
#define	_CAT3(x,y,z) x/**/y/**/z
#define	_CAT3_(x,y,z) x/**/_/**/y/**/_/**/z
#define _STR(x) "x"
#define _STRCAT(x,y) _STR(x/**/y)
#endif	__STDC__



/*
 * Locking types and macros.
 */
#ifdef	_NO_OBJECT_LOCKING_
typedef	int				obj_lock_t[2];
#define	obj_state_lock_init(obj)
#define	obj_state_lock(obj)
#define	obj_state_try_lock(obj)
#define	obj_state_unlock(obj)
#else	NO_OBJECT_LOCKING
typedef struct mutex 			obj_lock_t;
#define	obj_state_lock_init(lock) 	mutex_init(lock)
#define	obj_state_lock(lock) 		mutex_lock(lock)
#define	obj_state_try_lock(lock)	mutex_try_lock(lock)
#define	obj_state_unlock(lock)		mutex_unlock(lock)
#endif	_NO_OBJECT_LOCKING_

#define	mach_method_id(method_name)	(&_CAT(method_name,_method_descriptor))

#define mach_object_reference(_obj) if (_obj) (_obj)->reference()
#define mach_object_dereference(_obj) if (_obj) (_obj)->dereference()
#define mach_object_refcount(_obj) (_obj) ? (_obj)->refcount() : 0

#ifdef private
#undef private
#endif private

#define _Local(x) this->x
#define Local(x) this->x


/*
 * Basic types.
 */

/*
 * Method entry for invoke.
 */
typedef int		(*Pftype)(void*, ...);
typedef struct { 
	Pftype			pfunc;
	class usClass*		cl;
	int			offset;
} obj_method_entry, *obj_method_entry_t;


/*
 * Arbitrary argument list for invoke.
 */
typedef	struct arg_list {
	int	args[16];
} obj_arg_list_t, *arg_list_ptr;


/*
 * usClass: class containing the description of classes (name, etc)
 *	    and the method table to dispatch incoming invocations.
 */
class usClass {
	const char*	name;
	char*		rem_class_name;
	hash_table_t	method_table;
	hash_table_t	aux_table;

	void*		(*_virtual_constructor)(mach_port_t, const usClass&);
	void*		(*_converter_to_remote)(void*);
	const usClass*	_class_descriptor;

      public:
			usClass(
				const char*,
				void* (*)(void*, const usClass&),
				void* (*)(void*),
				const usClass*,
				void (*)(usClass*),
				class usTop*);

	const char*	class_name() const
				{ return name; }

	char*		remote_class_name() const
				{ return rem_class_name; }
	void		set_remote_class_name(char* s)
				{ rem_class_name = s; }

	void*		virtual_constructor(mach_port_t, const usClass&);
	void*		converter_to_remote(void*);
	const usClass*	class_descriptor(void);

	const class usTop*	_test_instance;

	boolean_t	is_init(usClass* class_obj);
	obj_method_entry_t	lookup_method(const mach_method_id_t mid) const
				{
					return((obj_method_entry_t)
						hash_lookup(method_table,
							(hash_key_t)mid));
				}
	void		set_method(mach_method_id_t,obj_method_entry_t);
};


#ifdef	GXXBUG_DELETE1
#define	_DECLARE_GXXBUG_DELETE1						\
    protected:								\
	virtual void		_virtual_delete() { delete this; }	\
    public:
#else	GXXBUG_DELETE1
#define	_DECLARE_GXXBUG_DELETE1	/* */
#endif	GXXBUG_DELETE1

#ifdef	GXXBUG_DELETE2

class _destructor_guard {
	static int		verbose;
	int			count;
    public:
				_destructor_guard() { count = 0; };
	boolean_t		check();
#ifdef	notdef
	inline boolean_t	check()
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
							us_internal_error(
		"destructor_guard triggered (possibly more than once)",
							US_INTERNAL_ERROR);
						}
						break;
					case 2:
						us_internal_error(
					"destructor_guard triggered once",
							US_INTERNAL_ERROR);
						break;
				}
				count += 1;
				return(TRUE);
			} else {
				count = 1;
				return(FALSE);
			}
		};
#endif	notdef
};

#define	_DECLARE_GXXBUG_DELETE2						\
    private:								\
	_destructor_guard	_destructor_guard_data;			\
    public:

#define	DESTRUCTOR_GUARD()						\
	if (this->_destructor_guard_data.check()) return;

#else	GXXBUG_DELETE2

#define	_DECLARE_GXXBUG_DELETE2	/* */

#define	DESTRUCTOR_GUARD() /* */

#endif	GXXBUG_DELETE2

#define _DECLARE_MEMBERS(class_name)					\
    private:								\
	static usClass		_class;					\
    public:								\
	virtual usClass*	is_a() const				\
					{ return &class_name::_class; } \
	static usClass*		desc()					\
					{ return &class_name::_class; }	\
	virtual char*		is_where()				\
					{ return((char*)this); }	\
	_DECLARE_GXXBUG_DELETE1						\
	_DECLARE_GXXBUG_DELETE2

#define _DECLARE_CASTDOWN(class_name)					\
	virtual void*		_castdown(const usClass&) const;	\
	static class_name&	castdown(usTop& p)			\
					{				\
					USSTATS(USSTATS_CASTDOWN);	\
					return *(class_name*)		\
					(&p ? p._castdown(*desc()) : 0);\
					}				\
        static const class_name&					\
				castdown(const usTop& p)		\
					{ 				\
					USSTATS(USSTATS_CASTDOWN);	\
					return *(const class_name*)	\
					(&p ? p._castdown(*desc()):0);\
					} 				\
        static class_name*	castdown(usTop* p)			\
					{				\
					USSTATS(USSTATS_CASTDOWN);	\
					return (class_name*)		\
					(p ? p->_castdown(*desc()): 0);\
					} 				\
        static const class_name*					\
				castdown(const usTop* p)		\
					{ 				\
					USSTATS(USSTATS_CASTDOWN);	\
					return (const class_name*)	\
					(p ? p->_castdown(*desc()) :0);\
					}

#define _DECLARE_INITCLASS(class_name)					\
	static void		init_class(usClass*);

#define _DECLARE_INSTANTIATION(class_name)				\
    public:								\
				class_name(void*, const usClass&);	\
	static void*		new_instance(void*, const usClass&);


/*
 * g++ has a bug in pointer-to-members when there are multiple
 * bases classes that crashes the compiler. Cannot use pointer-to-member
 * to dispatch incoming messages until that bug is corrected.
 */
#define _DECLARE_INVOKE(class_name)	/* */


#define _DECLARE_CONVERTER(class_name)					\
	static void*		cast_to_remote(void*);

/*
 * Use the following macros in the declaration of the class
 */

/*
 * DECLARE_MEMBERS: declares the members of a "normal" class whose methods 
 *		may be invoked remotely or passed as parameters in 
 * 		remote invocations.
 */
#define DECLARE_MEMBERS(class_name)					\
	_DECLARE_MEMBERS(class_name)					\
	_DECLARE_INITCLASS(class_name)					\
	_DECLARE_INSTANTIATION(class_name)				\
	_DECLARE_INVOKE(class_name)					\
	_DECLARE_CONVERTER(class_name)					\
	_DECLARE_CASTDOWN(class_name)

/*
 * DECLARE_MEMBERS_ABSTRACT_CLASS: same as DECLARE_MEMBERS for 
 *		abstract classes.
 */
#define DECLARE_MEMBERS_ABSTRACT_CLASS(class_name)			\
	_DECLARE_MEMBERS(class_name)					\
	_DECLARE_INITCLASS(class_name)					\
	_DECLARE_INVOKE(class_name)					\
	_DECLARE_CONVERTER(class_name)					\
	_DECLARE_CASTDOWN(class_name)

/*
 * DECLARE_LOCAL_MEMBERS: same as DECLARE_MEMBERS for 
 *		local classes - classes whose methods are 
 *		not accessible remotely.
 */
#define DECLARE_LOCAL_MEMBERS(class_name) 				\
	_DECLARE_MEMBERS(class_name)					\
	_DECLARE_CASTDOWN(class_name)


/*
 * DECLARE_PROXY_MEMBERS: same as DECLARE_MEMBERS for 
 *		proxy classes.
 */
#define DECLARE_PROXY_MEMBERS(class_name) 				\
	_DECLARE_MEMBERS(class_name)					\
	_DECLARE_INITCLASS(class_name)					\
	_DECLARE_INSTANTIATION(class_name)				\
	_DECLARE_CONVERTER(class_name)					\
	_DECLARE_CASTDOWN(class_name)

#define _DEFINE_CLASS(class_name)					\
	usClass class_name::_class(_STR(class_name),			\
				   &class_name::new_instance,		\
				   &class_name::cast_to_remote,		\
				   class_name::desc(),			\
				   &class_name::init_class,		\
				   (class usTop*)new class_name);

#define _DEFINE_ABSTRACT_CLASS(class_name)				\
	usClass class_name::_class(_STR(class_name),			\
				   0, 					\
				   &class_name::cast_to_remote,		\
				   class_name::desc(),			\
				   &class_name::init_class,		\
				   0);

#define _DEFINE_LOCAL_CLASS(class_name)					\
	usClass class_name::_class(_STR(class_name),			\
				   0, 0, 				\
				   class_name::desc(),			\
				   0, 0);

#define _DEFINE_INITCLASS(class_name)					\
	void class_name::init_class(usClass* class_obj) { 		\
	      BASE::init_class(class_obj);				\
	}

#define _DEFINE_CASTDOWN(class_name) 					\
	void* class_name::_castdown(const usClass& target) const {	\
		if (&target == desc()) return (void*)this; 		\
		return BASE::_castdown(target); 			\
	}

#if	_DEBUG_

#define DEFINE_CASTDOWN2(class_name,class1,class2)			\
	void* class_name::_castdown(const usClass& target) const {	\
		if (&target == desc()) return (void*) this;		\
		void* p = class1::_castdown(target);			\
		void* q = p;						\
		if (p = class2::_castdown(target)) ambig_check(p, q, target);\
		return q;						\
	}

// XXX This does not check for class1<>class3 ambiguities
#define DEFINE_CASTDOWN3(class_name,class1,class2,class3)		\
	void* class_name::_castdown(const usClass& target) const {	\
		if (&target == desc()) return (void*) this;		\
		void* p = class1::_castdown(target);			\
		void* q = p;						\
		if (p = class2::_castdown(target)) ambig_check(p, q, target);\
		if (p = class3::_castdown(target)) ambig_check(p, q, target);\
		return q;						\
	}

#else	_DEBUG_

#define DEFINE_CASTDOWN2(class_name,class1,class2)			\
	void* class_name::_castdown(const usClass& target) const {	\
		if (&target == desc()) return (void*) this;		\
		void* p;						\
		if (p = class1::_castdown(target)) return p;		\
		return class2::_castdown(target);			\
	}

#define DEFINE_CASTDOWN3(class_name,class1,class2,class3)		\
	void* class_name::_castdown(const usClass& target) const {	\
		if (&target == desc()) return (void*) this;		\
		void* p;						\
		if (p = class1::_castdown(target)) return p;		\
		if (p = class2::_castdown(target)) return p;		\
		return class3::_castdown(target);			\
	}

#endif	_DEBUG_

#define _DEFINE_CONSTRUCTOR(class_name)					\
	class_name::class_name(void* p, const usClass&) { set_object_port((mach_port_t)p); }

#define _DEFINE_INSTANTIATION(class_name)				\
	void* class_name::new_instance(void* p, const usClass& cl) {	\
		class_name* x = new class_name(p, cl);			\
		void* y = x->_castdown(cl);				\
		return y;						\
	}

/*
 * Can't have automatic invoke definition because of compiler problems.
 */
#define _DEFINE_INVOKE(class_name)	/* */

#define _DEFINE_CONVERTER(class_name)					\
	void* class_name::cast_to_remote(void* obj) {			\
		class_name* xobj = (class_name*) obj;			\
		return (void*)((usRemote*) xobj);			\
	}

/*
 * Use the following macros in the implementation file of the class
 */

/*
 * DEFINE_CLASS: defines the methods of a "normal" class whose methods 
 *		may be invoked remotely or passed as parameters in 
 * 		remote invocations.
 */
#define DEFINE_CLASS(class_name)					\
	_DEFINE_CLASS(class_name)					\
	_DEFINE_INSTANTIATION(class_name)				\
	_DEFINE_INVOKE(class_name)					\
	_DEFINE_CONVERTER(class_name)					\
	_DEFINE_CONSTRUCTOR(class_name)					\
	_DEFINE_CASTDOWN(class_name)

/*
 * DEFINE_CLASS_MI: equivalent to DEFINE_CLASS for classes that inherit
 *		from multiple base classes. Method 'castdown' must be
 *		implemented "by hand" with DEFINE_CASTDOWN2
 *		or DEFINE_CASTDOWN3.
 */
#define DEFINE_CLASS_MI(class_name)					\
	_DEFINE_CLASS(class_name)					\
	_DEFINE_INSTANTIATION(class_name)				\
	_DEFINE_INVOKE(class_name)					\
	_DEFINE_CONVERTER(class_name)					\
	_DEFINE_CONSTRUCTOR(class_name)

/*
 * DEFINE_ABSTRACT_CLASS: equivalent to DEFINE_CLASS for abstract classes.
 *		These classes normally define an interface. This macro
 *		defines all the methods of these classes. Their implementation
 *		contains only the definition of the parameters of the methods
 *		(defined with macro DEFINE_METHOD_ARGS).
 */
#define DEFINE_ABSTRACT_CLASS(class_name)				\
	_DEFINE_ABSTRACT_CLASS(class_name)				\
	_DEFINE_INITCLASS(class_name)					\
	_DEFINE_INVOKE(class_name)					\
	_DEFINE_CONVERTER(class_name)					\
	_DEFINE_CASTDOWN(class_name)

/*
 * DEFINE_ABSTRACT_CLASS_MI: same as DEFINE_ABSTRACT_CLASS for abstract classes
 *		with multiple base classes. Methods 'init_class' and 'castdown'
 *		must by defined "by hand". Use DEFINE_CASTDOWN2 or
 *		DEFINE_CASTDOWN3.
 */
#define DEFINE_ABSTRACT_CLASS_MI(class_name)				\
	_DEFINE_ABSTRACT_CLASS(class_name)				\
	_DEFINE_INVOKE(class_name)					\
	_DEFINE_CONVERTER(class_name)

/*
 * DEFINE_PROXY_CLASS: defines the methods for a proxy class.
 */
#define DEFINE_PROXY_CLASS(class_name)					\
	_DEFINE_CLASS(class_name)					\
	_DEFINE_INSTANTIATION(class_name)				\
	_DEFINE_CONVERTER(class_name)					\
	_DEFINE_CONSTRUCTOR(class_name)

/*
 * DEFINE_LOCAL_CLASS: defines the methods for a class whose methods are
 *		not accessible remotely. These classes derive from 'usTop'
 *		and have garbage collection, synchronization, etc.
 */
#define DEFINE_LOCAL_CLASS(class_name)					\
	_DEFINE_LOCAL_CLASS(class_name)					\
	_DEFINE_CASTDOWN(class_name)

/*
 * DEFINE_LOCAL_CLASS_MI: same as DEFINE_LOCAL_CLASS for local classes
 *		with multiple base classes. Method 'castdown' must be defined
 *		"by hand" with DEFINE_CASTDOWN2 or DEFINE_CASTDOWN3.
 */
#define DEFINE_LOCAL_CLASS_MI(class_name)				\
	_DEFINE_LOCAL_CLASS(class_name)


/*
 * usTop: root of the hierarchy.
 */
class usTop {
	int			ref_cnt;
	obj_lock_t		state_lock;

      public:
	_DECLARE_MEMBERS(usTop)
	static void		init_class(usClass*) {}

				usTop();
	virtual			~usTop();

	const char*		class_name() const
					{ return is_a()->class_name(); }

	virtual void*		_castdown(const usClass&) const;
	static usTop&		castdown(usTop& p)
		 			{
						USSTATS(USSTATS_CASTDOWN);
						return p;
					}
	static const usTop&	castdown(const usTop& p)
		 			{
						USSTATS(USSTATS_CASTDOWN);
						return p;
					}
	static usTop*		castdown(usTop* p)
		 			{
						USSTATS(USSTATS_CASTDOWN);
						return p;
					}
	static const usTop*	castdown(const usTop* p)
		 			{
						USSTATS(USSTATS_CASTDOWN);
						return p;
					}
	void			ambig_check(
					void*&,
					void*&,
					const usClass&) const;

	void			reference_proc();
	void			dereference_proc();

	inline void		reference()
#if	_DEBUG_
				{ reference_proc(); }
#else	_DEBUG_
				/*
				 * Copied from reference_proc().
				 */
				{
					obj_state_lock(&state_lock);
					ASSERT_EXPR(
					"usTop::reference() dead object",
							ref_cnt > 0);
					ref_cnt++;
					obj_state_unlock(&state_lock);
				}
#endif	_DEBUG_

	inline void		dereference()
#if	_DEBUG_
				{ dereference_proc(); }
#else	_DEBUG_
				/*
				 * Copied from dereference_proc().
				 */
				{
					char	*mem_addr;
					obj_state_lock(&state_lock);
					ASSERT_EXPR("usTop::dereference() dead object",ref_cnt > 0);
					if (ref_cnt == 1) {
						ref_cnt = 0;
						obj_state_unlock(&state_lock);
#ifdef	GXXBUG_DELETE1
						_virtual_delete();
#else	GXXBUG_DELETE1
						mem_addr = is_where();
						delete this;
#ifdef GXXBUG_DELETE3
						free(mem_addr);
#endif GXXBUG_DELETE3
#endif	GXXBUG_DELETE1
					} else {
						ref_cnt--;
						obj_state_unlock(&state_lock);
					}
				}
#endif	_DEBUG_

	int			refcount();

#ifdef	GXXBUG_CLONING1
//	virtual mach_error_t	clone_init(mach_port_t)=0;
//	virtual mach_error_t	clone_abort(mach_port_t)=0;
//	virtual mach_error_t	clone_complete()=0;
#else	GXXBUG_CLONING1
	virtual mach_error_t	clone_init(mach_port_t);
	virtual mach_error_t	clone_abort(mach_port_t);
	virtual mach_error_t	clone_complete();
#endif	GXXBUG_CLONING1

	virtual mach_error_t	_notdef();
};


/*
 * usRemote: root of all classes whose methods are accessible remotely.
 */
static const int	usRemote_MAX_ACTIVE_INVOKES = 64;

class usRemote: public VIRTUAL4 usTop {
      private:
	struct mutex		lock;
	mach_port_t		object_port_data;
	mach_port_t		external_port_data;
	intr_cthread_id_t	active_invokes[usRemote_MAX_ACTIVE_INVOKES];
	int			next_active_invoke_index;
	unsigned int		mscount;
	boolean_t		no_senders;

	mach_msg_seqno_t	next_seqno;	/* protected by rpcmgr lock */

	class rpcmgr*		_rpcmgr();

      public:
	DECLARE_MEMBERS_ABSTRACT_CLASS(usRemote);
				usRemote();
	virtual			~usRemote();

	virtual mach_error_t	invoke(mach_method_id_t, ...);
	virtual mach_error_t	invoke(mach_method_id_t);
	virtual mach_error_t	invoke(mach_method_id_t,void*);
	virtual mach_error_t	invoke(mach_method_id_t,void*,void*);

	mach_error_t		outgoing_invoke(mach_method_id_t, ...);

	mach_error_t		_register_incoming_invoke(intr_cthread_id_t);
	mach_error_t		_deregister_incoming_invoke(intr_cthread_id_t);
	mach_error_t		_interrupt_incoming_invokes();

	virtual char*		remote_class_name() const;
	virtual void		set_remote_class_name(char*);

	mach_port_t		object_port()
					 { return object_port_data; }
	void			set_object_port(mach_port_t);

	mach_port_t		get_transfer_port(int*);

	void			_reset_after_clone();

	void			_reference_with_seqno(mach_msg_seqno_t);
	mach_error_t		_no_remote_senders(unsigned int);
};


#define REMOTE

#define EXPORT_METHOD(method_name)					\
	extern struct method_descriptor	_CAT(method_name,_method_descriptor);

#define BEGIN_SETUP_METHOD_WITH_ARGS(class_name) {		       	\
	/* Cannot use virtual functions here because this is		\
	 * called from a constructor.					\
	 */								\
	if (class_obj->is_init(&class_name::_class)) {			\
		return;							\
	}								\
	const class_name*	class_instance =			\
			class_name::castdown(class_obj->_test_instance);

#ifdef	GXXBUG_PFUNC1
#define	GETPTR_GXXBUG_PFUNC1(class_name,method_name)			\
	/*								\
	 * This may force the instantiation of an abstract class --	\
	 * illegal in gcc-2.0.						\
	 */								\
	class_name	gxxbug_pfunc1_instance;				\
	entryp->pfunc =							\
		(Pftype)&(gxxbug_pfunc1_instance.class_name::method_name)
#else	GXXBUG_PFUNC1
#define	GETPTR_GXXBUG_PFUNC1(class_name,method_name)			\
	entryp->pfunc = (Pftype)&(class_instance->class_name::method_name)
#endif	GXXBUG_PFUNC1

#define	SETUP_METHOD_WITH_ARGS(class_name,method_name) {		\
	mach_method_id_t	id;					\
	extern struct method_descriptor	_CAT(method_name,_method_descriptor);\
									\
	id = & _CAT(method_name,_method_descriptor);			\
	method_info::GLOBAL->_define_method(id);			\
	/* Insert the method to be called and the type of the object */ \
	obj_method_entry_t	entryp = New(obj_method_entry);		\
	GETPTR_GXXBUG_PFUNC1(class_name,method_name);			\
	entryp->cl = class_name::desc();				\
	entryp->offset = ((void*)class_instance)			\
				- ((void*)(class_obj->_test_instance));	\
	class_obj->set_method(id, entryp);				\
}

#define END_SETUP_METHOD_WITH_ARGS	}

#define	DEFINE_METHOD_ARGS(method_name,args)				\
	struct method_descriptor _CAT(method_name,_method_descriptor) = \
		{ _STR(method_name), args, 0 };


#define INSERT_CLASS_IN_MAP(_class,_proxy_name)				\
	{								\
		if (class_info::GLOBAL == 0)				\
			class_info::GLOBAL = new class_info;		\
		void* c = class_info::GLOBAL->_lookup_class(_STR(_class));\
		if (c == 0) {						\
			DEBUG0(TRUE,(0,"Class (%s) not in map\n",	\
							_STR(_class)));	\
		} else {						\
			class_info::GLOBAL->_insert_class(_proxy_name, c);\
		}							\
	}

#endif	_top_ifc_h
