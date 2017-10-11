    .data
	str: .asciiz "Welcome to OS!\n"
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

	la $8, str
	li $10, 0xbfe48000

putstr:
	lb $9, ($8)
	beq $9, $0, end
    sb $9, ($10)
	addiu $8, 1
	j putstr

end:
    nop
	
	
	#need add code
	# call printstr print a string



