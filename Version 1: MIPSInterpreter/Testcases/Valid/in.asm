main:
	addi $t0, $0, 5
	sw $t0, 100
	add $t5, $t0, $0
	beq $t0, $t5, here
	mul $t0, $t1, $t2

here:
	addi $0, $0, 100
	lw $s1, 100
