#include "regs.h"
	.text
	.align	2
	.globl  get_timer	
	.ent	get_timer
	.type	get_timer, @function
get_timer:
	mfc0 $2,$9
        nop
        j       ra
        nop
	.align	2
	.globl  kernel_entry	
	.ent	kernel_entry
	.type	kernel_entry, @function
kernel_entry:
	addiu sp, sp, -24
	sw ra, 0(sp)
	bnez $4, 1f
	nop

	jal do_yield
	nop
	beqz $0,2f
	nop

1:
	jal do_exit
	nop
2:
	lw ra, 0(sp)
	addiu sp, sp, 24
	jr ra
	nop

	.align	2
	.globl  scheduler_entry	
	.ent	scheduler_entry
	.type	scheduler_entry, @function
scheduler_entry:
	# call scheduler, which will set new current process
	# need student add
    
    la t0, scheduler
	jal t0

	lw t1, (current_running)
	lw s0, 0(t1)
	lw s1, 4(t1)
	lw s2, 8(t1)
	lw s3, 12(t1)
	lw s4, 16(t1)
	lw s5, 20(t1)
	lw s6, 24(t1)
	lw s7, 28(t1)
	lw sp, 32(t1)
	lw ra, 36(t1)

	jr ra;
	.align	2
	.globl  save_pcb	
	.ent	save_pcb
	.type	save_pcb, @function
save_pcb:
	# save the pcb of the currently running process
	# need student add
    la t0, current_running 
	addiu sp, sp, 24
	lw t1, (current_running)
	sw s0, 0(t1)
	sw s1, 4(t1)
	sw s2, 8(t1)
	sw s3, 12(t1)
	sw s4, 16(t1)
	sw s5, 20(t1)
	sw s6, 24(t1)
	sw s7, 28(t1)
	sw sp, 32(t1)  # save sp
	addiu sp, sp, -24
	lw t2, 16(sp) # save ra
	sw t2, 36(t1)
	jr ra
	

