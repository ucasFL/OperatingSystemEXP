	.text
	.globl main

main:
	# check the offset of main
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	addiu $29, $29, -4
	sw $31, 0($29)
	
	li $4, 0xa0800200
	nop
	li $5, 0x200
	jal 0x8007b1a8
	nop
	jal 0xa080026c

	lw $31, 0($29)
	addiu $29, $29, 4
	jr $31
	#need add code
	#read kernel

