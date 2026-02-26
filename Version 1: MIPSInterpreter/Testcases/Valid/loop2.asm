main:
	addi	$t0, $0, 10
	add		$t1, $0, $0
	addi	$s1, $zero, 320
loop:
	sw		$s0, 12($s1)
	addi	$s0, $s0, 100
	addi	$t1, $t1, 1
	bne		$t0, $t1, loop

exit: