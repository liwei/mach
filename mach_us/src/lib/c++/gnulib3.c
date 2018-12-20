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
 * HISTORY
 * $Log:	gnulib3.c,v $
 * Revision 2.7  94/07/07  17:46:23  mrt
 * 	Added copyright
 * 
 * Revision 2.6  94/06/16  17:17:34  mrt
 * 	Add USSTATS stuff from DPJ.
 * 	[94/05/25  13:12:29  jms]
 * 
 * Revision 2.5  94/05/17  14:06:09  jms
 * 	Change do_global_init =>do_global_ctors, do_global_cleanup=>do_global_dtors
 * 	[94/04/28  18:09:38  jms]
 * 
 * Revision 2.4  94/04/29  15:48:04  jms
 * 	Allocate the first page in memory full of zeros.  Need so that "standalone"
 * 	has zeros as happens when things are started from mach-UX
 * 	[94/04/29  14:56:08  jms]
 * 
 * Revision 2.3  93/01/20  17:36:27  jms
 * 	Implement the GXXBUG_DELETE3 g++ buq workaround to do object memory freeing
 * 	in top::dealloc_proc instead of gnulib3::_buildin_delete_
 * 
 * 	Add some tracing/debugging stuff
 * 	[93/01/18  15:53:01  jms]
 * 
 * Revision 2.2  91/11/06  11:29:03  jms
 * 	Created.
 * 	[91/09/26  18:50:07  pjg]
 * 
 */

#define GXXBUG_DELETE3 1

#include <logging.h>
#include <mach.h>

#include "us_statistics.h"

typedef struct set_vector
{
  int length;
  int vector[1];
  /* struct set_vector *next; */
} set_vector;

extern set_vector __CTOR_LIST__;
extern set_vector __DTOR_LIST__;
set_vector *__dlp;
int __dli;

extern void __do_global_ctors ();
extern void __do_global_dtors ();
extern  void on_exit(void*, void*);
extern  void _cleanup();
extern  void _exit(int);


int
__main ()
{
  /* Gross hack for GNU ld.  This is defined in `builtin.cc'
     from libg++.  */
  extern int __1xyzzy__;

  __dli = __DTOR_LIST__.length;
  __dlp = &__DTOR_LIST__;

    {
	vm_address_t zero_addr = 0;
	(void)vm_allocate(mach_task_self(), &zero_addr, 512, FALSE);
    }

  __do_global_ctors (&__1xyzzy__);
}

void
__do_global_ctors ()
{
  register int i, len;
  register void (**ppf)() = (void (**)())__CTOR_LIST__.vector;

  len = __CTOR_LIST__.length;
  for (i = 0; i < len; i++)
    (*ppf[i])();
}

void
__do_global_dtors ()
{
  while (__dlp)
    {
      while (--__dli >= 0)
	{
	  void (*pf)() = (void (*)())__dlp->vector[__dli];
	  (*pf)();
	}
      __dlp = (struct set_vector *)__dlp->vector[__dlp->length];
      if (__dlp) __dli = __dlp->length;
    }
}


typedef void (*vfp)();

extern vfp __new_handler;

#if CHECK_LASTONE
struct mutex lastone_lock = MUTEX_INITIALIZER;
char * lastone = 0;
int lastone_id;

#endif CHECK_LASTONE

char *
__builtin_new (sz)
     long sz;
{
  char *p;

  USSTATS(USSTATS_RAWOBJ_ALLOC);

#if CHECK_LASTONE
  mutex_lock(&lastone_lock);
  lastone_id++;
  LOG2(TRUE, 4001, lastone_id, lastone);
#endif CHECK_LASTONE

  p = (char *)malloc (sz);
  if (p == 0)
    (*__new_handler) ();

  LOG2(TRUE, 4002, lastone_id, p);
	
#if BUILTIN_TRACE
  printf("__buildin_new:size = 0x%x, ptr = 0x%x\n",sz,p);
#endif BUILTIN_TRACE

#if CHECK_LASTONE
  if (p == lastone) {
	printf("_builtin_new: reused address, suspending\n");
	task_suspend(mach_task_self());
}
  lastone=p;

  LOG2(TRUE, 4003, lastone_id,p);
  mutex_unlock(&lastone_lock);
#endif CHECK_LASTONE

  
  return p;
}

static void
default_new_handler ();

vfp __new_handler = default_new_handler;

char *
__builtin_vec_new (p, maxindex, size, ctor)
     char *p;
     int maxindex, size;
     void (*ctor)();
{
  int i, nelts = maxindex + 1;
  char *rval;

  if (p == 0)
    p = (char *)__builtin_new (nelts * size);

  rval = p;

  for (i = 0; i < nelts; i++)
    {
      (*ctor) (p);
      p += size;
    }

  return rval;
}

vfp
__set_new_handler (handler)
     vfp handler;
{
  vfp prev_handler;

  prev_handler = __new_handler;
  if (handler == 0) handler = default_new_handler;
  __new_handler = handler;
  return prev_handler;
}

vfp
set_new_handler (handler)
     vfp handler;
{
  return __set_new_handler (handler);
}

static void
default_new_handler ()
{
#if CHECK_LASTONE
   LOG1(TRUE, 4005, lastone_id);
#endif CHECK_LASTONE

  /* don't use fprintf (stderr, ...) because it may need to call malloc.  */
/*   write (2, "default_new_handler: out of memory... aaaiiiiiieeeeeeeeeeeeee!\n", 65); */
  /* don't call exit () because that may call global destructors which
     may cause a loop.  */
  _exit (-1);
}


void
__builtin_delete (ptr)
     char *ptr;
{
  USSTATS(USSTATS_RAWOBJ_DEALLOC);
  LOG1(TRUE, 4004, ptr);
#if BUILTIN_TRACE
  printf("__buildin_delete(0x%x)\n",ptr);
#endif BUILTIN_TRACE

#ifndef GXXBUG_DELETE3
  if (ptr)
    free (ptr);
#endif GXXBUG_DELETE3
}

void
__builtin_vec_delete (ptr, maxindex, size, dtor, auto_delete_vec, auto_delete)
     char *ptr;
     int maxindex, size;
     void (*dtor)();
     int auto_delete;
{
  int i, nelts = maxindex + 1;
  char *p = ptr;

  ptr += nelts * size;

  for (i = 0; i < nelts; i++)
    {
      ptr -= size;
      (*dtor) (ptr, auto_delete);
    }

  if (auto_delete_vec)
    free (p);
}
