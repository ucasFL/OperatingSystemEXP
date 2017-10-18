#include "common.h"
#include "syslib.h"
#include "util.h"
unsigned int t1, t2, t3;

void _start(void)
{
	yield();
	t1 = get_timer();
	yield();
	t2 = get_timer();
	t3 = (t2 - t1) / MHZ;
	print_location(1, 3);
	printstr("The switch time from process to process: ");
	printint(42, 3, t3 );
	exit();
	/* need student add */
}
