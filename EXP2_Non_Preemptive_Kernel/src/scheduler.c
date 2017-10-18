/* scheduler.c */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "util.h"
#include "queue.h"

extern int NUM_TASKS;
int scheduler_count;
// process or thread runs time
uint64_t cpu_time;
extern volatile pcb_t *current_running;
pcb_t *unblock_pcb;

extern queue task_queue;
extern queue_t ready_queue, blocked_queue;

void printstr(char *s);
void printnum(unsigned long long n);
void scheduler(void)
{
	++scheduler_count;

	current_running = queue_pop(ready_queue);	
	if ( !current_running )
	  return ; // no process can run
	current_running->state = PROCESS_RUNNING;
	// pop new pcb off ready queue
	/* need student add */ 
}

void do_yield(void)
{
	save_pcb();
	current_running->state = PROCESS_READY;
	/* push the qurrently running process on ready queue */
	/* need student add */
	queue_push(ready_queue, (pcb_t *)current_running);

	// call scheduler_entry to start next task
	scheduler_entry();

	// should never reach here
	ASSERT(0);
}

void do_exit(void)
{
	/* need student add */
	current_running->state = PROCESS_EXITED;
	scheduler_entry();
}

void block(void)
{
	save_pcb();
	/* need student add */
	current_running->state = PROCESS_BLOCKED;
	queue_push(blocked_queue, (pcb_t *)current_running);
	//blocked_queue->isEmpty = FALSE;
	// should never reach here
	scheduler_entry();
	ASSERT(0);
}

int unblock(void)
{
	if (blocked_tasks()){
		unblock_pcb = queue_pop(blocked_queue);
		unblock_pcb->state = PROCESS_READY;
		queue_push(ready_queue, (pcb_t *)unblock_pcb);
		return 1;
	}
	return 0;
	/* need student add */
}

bool_t blocked_tasks(void)
{
	return !blocked_queue->isEmpty;
}
