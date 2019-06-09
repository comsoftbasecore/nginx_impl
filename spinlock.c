


#include "spinlock.h"


void
lock_spinlock(atomic_t *lock, atomic_int_t value, uint_t spin)
{

    uint_t  i, n;

    for ( ;; ) {

        if (*lock == 0 && __sync_bool_compare_and_swap(lock, 0, value)) {
            return;
        }

        if (TOTAL_CPU > 1) {

            for (n = 1; n < spin; n <<= 1) {

                for (i = 0; i < n; i++) {
                    #if ( __i386__ || __i386 || __amd64__ || __amd64 )            
					    __asm__ ("pause");
					#endif
                }

                if (*lock == 0 && __sync_bool_compare_and_swap(lock, 0, value)) {
                    return;
                }
            }
        }
		/* 当前进程仍然处于可执行状态，单暂时"让出"处理器，是的处理器有限调度其他可执行状态
		   的进程，目的时都时暂时"让出"处理器*/
        sched_yield();
    }



}
