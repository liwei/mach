#ifndef	_default_pager_user_
#define	_default_pager_user_

/* Module default_pager */

#include <mach/kern_return.h>
#if	(defined(__STDC__) || defined(c_plusplus)) || defined(LINTLIBRARY)
#include <mach/port.h>
#include <mach/message.h>
#endif

#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <mach/default_pager_types.h>

/* Routine default_pager_object_create */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t default_pager_object_create
#if	defined(LINTLIBRARY)
    (default_pager, memory_object, object_size)
	mach_port_t default_pager;
	memory_object_t *memory_object;
	vm_size_t object_size;
{ return default_pager_object_create(default_pager, memory_object, object_size); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t default_pager,
	memory_object_t *memory_object,
	vm_size_t object_size
);
#else
    ();
#endif
#endif

/* Routine default_pager_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t default_pager_info
#if	defined(LINTLIBRARY)
    (default_pager, info)
	mach_port_t default_pager;
	default_pager_info_t *info;
{ return default_pager_info(default_pager, info); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t default_pager,
	default_pager_info_t *info
);
#else
    ();
#endif
#endif

/* Routine default_pager_objects */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t default_pager_objects
#if	defined(LINTLIBRARY)
    (default_pager, objects, objectsCnt, ports, portsCnt)
	mach_port_t default_pager;
	default_pager_object_array_t *objects;
	mach_msg_type_number_t *objectsCnt;
	mach_port_array_t *ports;
	mach_msg_type_number_t *portsCnt;
{ return default_pager_objects(default_pager, objects, objectsCnt, ports, portsCnt); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t default_pager,
	default_pager_object_array_t *objects,
	mach_msg_type_number_t *objectsCnt,
	mach_port_array_t *ports,
	mach_msg_type_number_t *portsCnt
);
#else
    ();
#endif
#endif

/* Routine default_pager_object_pages */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t default_pager_object_pages
#if	defined(LINTLIBRARY)
    (default_pager, memory_object, pages, pagesCnt)
	mach_port_t default_pager;
	mach_port_t memory_object;
	default_pager_page_array_t *pages;
	mach_msg_type_number_t *pagesCnt;
{ return default_pager_object_pages(default_pager, memory_object, pages, pagesCnt); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t default_pager,
	mach_port_t memory_object,
	default_pager_page_array_t *pages,
	mach_msg_type_number_t *pagesCnt
);
#else
    ();
#endif
#endif

#endif	_default_pager_user_
