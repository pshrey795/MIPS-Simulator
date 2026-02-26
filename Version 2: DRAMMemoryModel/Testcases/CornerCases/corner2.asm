main:
	sw		$s0, 1000
	addi	$t0, $0, 1
	addi	$t0, $t0, 5
	sw		$s0, 1004
	add		$t0, $s0, $t0
