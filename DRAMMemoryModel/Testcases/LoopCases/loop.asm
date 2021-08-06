main:
	addi	$t1, $0, 5
	sw		$t1, 1000
	addi	$t0, $0, 0
loop:
	lw		$s0, 1000
	addi	$t0, $t0, 1
	bne		$t0, $t1, loop
