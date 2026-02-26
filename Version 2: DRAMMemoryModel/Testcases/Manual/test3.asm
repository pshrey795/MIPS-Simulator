main:
	addi $s0, $zero, 1000
	addi $s1, $zero, 0
	addi $s2, $zero, 10
	addi $t1, $zero, 0

loop:	
	addi $t1, $t1, 1
	sw $t1, 0($s0)
	addi $s0, $s0, 4
	addi $s1, $s1, 1
	slt $s3, $s1, $s2
	bne $s3, $zero, loop
	
	addi $s0, $zero, 1000
	addi $s1, $zero, 0
	addi $s3, $zero, 0
	addi $s2, $zero, 9

loop2:
	lw $t0, 0($s0)
	addi $s0, $s0, 4
	lw $t1, 0($s0)
	add $t2, $t0, $t1
	sw $t2, 0($s0)
	addi $s1, $s1, 1
	slt $s3, $s1, $s2
	bne $s3, $zero, loop2

exit:
	
