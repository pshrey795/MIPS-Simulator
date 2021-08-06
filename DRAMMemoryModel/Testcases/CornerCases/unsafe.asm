main:
	addi	$s0, $zero, 10
	sw		$s0, 1024
	lw		$t0, 1024
	addi	$t1, $0, 2
	add		$t2, $t0, $t1
