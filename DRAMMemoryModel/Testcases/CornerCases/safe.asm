main:
	addi	$s0, $zero, 1024
	addi	$t0, $0, 0
	addi	$t1, $0, 5

loop:
	sw		$t1, ($s0)
	addi	$t1, $t1, -1
	addi	$s0, $s0, 4
	bne		$t0, $t1, loop
