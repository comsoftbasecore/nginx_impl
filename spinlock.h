



#ifndef _SPINLOCK_H_INCLUDED_
#define _SPINLOCK_H_INCLUDED_

#define NGX_HAVE_SYSVSHM 1 
#define NGX_HAVE_ATOMIC_OPS  1

#define TOTAL_CPU 1
#define __i386__ 1

#include <stdint.h>
#include <sched.h>



typedef long                        atomic_int_t;
typedef unsigned long               atomic_uint_t;
typedef unsigned long               uint_t;


typedef volatile atomic_uint_t  atomic_t;

void
lock_spinlock(atomic_t *lock, atomic_int_t value, uint_t spin);
#define try_spinlock(lock)  (*(lock) == 0 && __sync_bool_compare_and_swap(lock, 0, 1))
#define unlock_spinlock(lock)    *(lock) = 0




#endif /* _SPINLOCK_H_INCLUDED_ */

