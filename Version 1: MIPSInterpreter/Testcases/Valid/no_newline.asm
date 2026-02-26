main : addi	$t0, $0, 0
	addi	$t1, $0, 10
	addi	$s0, $0, 1
	addi	$v0, $0, 1

loop: slt		$t8, $t0, $t1
	beq		$t8, $0, exit
	mul		$s0, $s0, $t1
	sub		$t1, $t1, $v0
	j		loop

exit:


