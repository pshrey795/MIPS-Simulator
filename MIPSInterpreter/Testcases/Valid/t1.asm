addi $s1,$s0,4
li $s0,8
sub $t1,$s0,$s1
add $t2,$t1,$t0

sw $s0,44
lw $s2,44
mul $s3,$s2,$t2

sw $s3,8($t0)
lw $t3,8
slt $s4,$t0,$s2