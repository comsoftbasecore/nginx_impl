#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <unistd.h>
#include "shmem.h"
#include "spinlock.h"

//typedef unsigned long       atomic_uint_t;//原子变量操作定义


typedef void (*spawn_proc_pt) (void *data);

static void start_worker_processes(int n);
static void worker_process_cycle(void *data);
static void  worker_process_init( int worker);
void setaffinity(cpu_set_t *cpu_affinity);  

atomic_uint_t * visit_counter;
atomic_t      * spin_lock;
shm_t            shm;
pthread_mutex_t *mtx;


int main(int argc,char **argv){
	u_char              *shared = NULL;
	
	
	shm.size = 4096;
	
    if (shm_alloc(&shm) != NGX_OK) {
        fprintf(stderr,"shm_alloc failed, eixt(-1).\n");
		exit(-1);
    }

    shared = shm.addr;

    visit_counter = (atomic_uint_t *) shared ;
    mtx =(pthread_mutex_t *)(shared+16); 
        //spin_lock = (atomic_t *)(shared +16); 
	//fprintf(stdout,"visit_counter: %lu\n",(atomic_uint_t)*visit_counter);
    *visit_counter = 0;
        //*spin_lock = 0;
  	
	//*visit_counter = 0;
	//__sync_fetch_and_add(*visit_counter,1);
	thread_mutex_create(mtx);
	start_worker_processes(4);
    thread_mutex_destroy(mtx);
}


static void start_worker_processes(int n)
{
    int      i;

    for (i = 0; i < n; i++) {

        spawn_process( worker_process_cycle,
                          (void *) (intptr_t) i, "worker process");
    }

	for(i=0; i<1000; i++){
		printf("parent - visit_counter: %lu\n",(atomic_uint_t)*visit_counter);
        sleep(1);
	}
    wait(NULL);
    printf("master process end.\n");
}

pid_t  spawn_process( spawn_proc_pt proc, void *data,
    char *name)
{
    pid_t  pid;
  
    pid = fork();

    switch (pid) {

    case -1:
        fprintf(stderr,"fork() failed while spawning \"%s\"\n", name);
        return -1;
    case 0:
        proc(data);
        break;

    default:
        break;
    }

    printf("start %s %ld\n", name, (long int)pid);
    return pid;
}

static void worker_process_cycle(void *data)
{
    int worker = (intptr_t) data;

    worker_process_init(worker);

    //进程开始干活。。。。。。
    for(;;)
	{
		int i=0;
		for(i=0;i<20000000;i++)
		{
		    __sync_fetch_and_add(visit_counter, 1);
                    //thread_mutex_lock(mtx);
		    //(*visit_counter)++;
                    //thread_mutex_unlock(mtx);
		    /*lock_spinlock(spin_lock, 1, 64); 
		    (*visit_counter)++;
            unlock_spinlock(spin_lock);
            */
    	    //usleep(10);
                    
		}
		exit(0);
    }
}

static void
worker_process_init( int worker)
{
    cpu_set_t  cpu_affinity;
    
    CPU_ZERO(&cpu_affinity);
    CPU_SET(worker % CPU_SETSIZE, &cpu_affinity);
    setaffinity(&cpu_affinity);
    
    // do other initialization
        
}


/*Free BSD    
 *  #include <sys/cpuset.h>
 * cpuset_setaffinity(CPU_LEVEL_WHICH, CPU_WHICH_PID, -1,
                           sizeof(cpuset_t), cpu_affinity) == -1)
 *
*/
void setaffinity(cpu_set_t *cpu_affinity)
{
    unsigned int  i;

    for (i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, cpu_affinity)) {
            printf("sched_setaffinity(): using cpu #%ui", i);
        }
    }

    if (sched_setaffinity(0, sizeof(cpu_set_t), cpu_affinity) == -1) {
        fprintf(stderr,"sched_setaffinity() failed");
    }
}



