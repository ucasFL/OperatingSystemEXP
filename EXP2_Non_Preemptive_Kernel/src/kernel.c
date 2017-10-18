/*
   kernel.c
   the start of kernel
   */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"
#include "queue.h"

#include "tasks.c"

volatile pcb_t *current_running;

queue task_queue;
queue block_queue;
queue_t ready_queue, blocked_queue;
pcb_t *ready_array[NUM_TASKS];
pcb_t ready_arr[NUM_TASKS];
pcb_t *blocked_arr[NUM_TASKS];

/*
   this function is the entry point for the kernel
   It must be the first function in the file
   */

#define PORT3f8 0xbfe48000

 void printnum(unsigned long long n)
 {
   int i,j;
   unsigned char a[40];
   unsigned long port = PORT3f8;
   i=10000;
   while(i--);

   i = 0;
   do {
   a[i] = n % 16;
   n = n / 16;
   i++;
   }while(n);

  for (j=i-1;j>=0;j--) {
   if (a[j]>=10) {
      *(unsigned char*)port = 'a' + a[j] - 10;
    }else{
	*(unsigned char*)port = '0' + a[j];
   }
  }
  printstr("\r\n");
}

void _stat(void){


	blocked_queue = &block_queue;
	ready_queue = &task_queue;
	queue_init(ready_queue);
	queue_init(blocked_queue);
	/* some scheduler queue initialize */
	/* need student add */
	clear_screen(0, 0, 30, 24);

	/* Initialize the PCBs and the ready queue */
	int i, j;
	ready_queue->capacity = NUM_TASKS;
	ready_queue->pcbs = ready_array;
	for (i = 0; i < NUM_TASKS; i++ ){
		for (j = 0; j < NUM_REGISTERS; j++ )
		  ready_arr[i].reg[j] = 0;
		ready_arr[i].sp = STACK_MAX - i * STACK_SIZE ;
		ready_arr[i].ra = task[i]->entry_point;
		ready_arr[i].state = PROCESS_READY;
	}
	ready_queue->pcbs = ready_array;
	for ( i = 0; i < NUM_TASKS; i++ )
	  queue_push(ready_queue, &ready_arr[i]);
	/* need student add */

	blocked_queue->pcbs = blocked_arr;
	blocked_queue->capacity = NUM_TASKS;
	/*Schedule the first task */
	scheduler_count = 0;
	scheduler_entry();

	/*We shouldn't ever get here */
	ASSERT(0);
}
