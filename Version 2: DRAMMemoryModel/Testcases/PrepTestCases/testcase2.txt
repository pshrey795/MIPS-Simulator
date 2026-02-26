main:
	addi $t1, $zero, 1
	sw $t1, 1000
	addi $t1, $t1, 1
	sw $t1, 1004
	addi $t1, $t1, 1
	sw $t1, 1008
	addi $t1, $t1, 1
	sw $t1, 1012
	addi $t1, $t1, 1
	sw $t1, 1016
	addi $t1, $t1, 1
	sw $t1, 1020
	addi $t1, $t1, 1
	sw $t1, 1024
	addi $t1, $t1, 1
	sw $t1, 1028
	addi $t1, $t1, 1
	sw $t1, 1032
	addi $t1, $t1, 1
	sw $t1, 1036
	addi $t1, $t1, 1
	lw $t1, 1000
	lw $t2, 1004
	add $t3, $t1, $t2
	sw $t3, 1004	
	lw $t1, 1004
	lw $t2, 1008
	add $t3, $t1, $t2
	sw $t3, 1008	
	lw $t1, 1008
	lw $t2, 1012
	add $t3, $t1, $t2
	sw $t3, 1012	
	lw $t1, 1012
	lw $t2, 1016
	add $t3, $t1, $t2
	sw $t3, 1016	
	lw $t1, 1016
	lw $t2, 1020
	add $t3, $t1, $t2
	sw $t3, 1020	
	lw $t1, 1020
	lw $t2, 1024
	add $t3, $t1, $t2
	sw $t3, 1024	
	lw $t1, 1024
	lw $t2, 1028
	add $t3, $t1, $t2
	sw $t3, 1028	
	lw $t1, 1028
	lw $t2, 1032
	add $t3, $t1, $t2
	sw $t3, 1032	
	lw $t1, 1032
	lw $t2, 1036
	add $t3, $t1, $t2
	sw $t3, 1036	
exit:
