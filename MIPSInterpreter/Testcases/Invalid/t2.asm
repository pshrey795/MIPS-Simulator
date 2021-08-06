lw $s0,23
li $s1,4

loop

    slt $s2,$s0,$s1
    beq $s2,$t0,loop_exit
    add $s3,$s3,$s0
    addi $s0,$s0,1
    j loop

loop_exit:

    mul $t2,$s0,$s1
    sw $t2,($t2)
    lw $s4,4