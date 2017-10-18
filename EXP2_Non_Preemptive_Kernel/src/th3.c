#include "common.h"
#include "scheduler.h"
#include "util.h"
uint32_t time_TtoT;
uint32_t time_t4, time_t5,time_t5_off, time_ptot4;

void thread4(void)
{
	time_t4 = get_timer();
	do_yield();
	time_ptot4 = get_timer();
	unsigned int sw_ticks = time_t5_off - time_ptot4;
	unsigned int time = ((unsigned int) sw_ticks ) / ( 2 * MHZ);
	print_location(1, 15);
	printstr("The switch time from thread to process: ");
	printint(41, 4, time);
	do_exit();
}

void thread5(void)
{
	time_t5 = get_timer();
	unsigned int sw_ticks = time_t5 - time_t4;
	unsigned int time = ((unsigned int) sw_ticks ) / MHZ;
	print_location(1,15);
	printstr("The switch time from thread to thread: ");
	printint(40, 3, time);
	time_t5_off = get_timer();
	do_yield();
	do_exit();
}
