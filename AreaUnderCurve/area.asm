# Assembly program to calculate the algebraic area under the curve formed by given cartesian co-ordinates

.text
.globl main

main:

    # s0 = sum, intialized to 0, which will store the value sigma(i=1 to n)(y_i + y_i-1)(x_i - x_i-1) after i iterations. 
    # The actual area is half of this sum value.
    li $s0,0                

    li $v0,5
    syscall
    move $s1,$v0                # s1 = n, the total number of points

    addi $s1,$s1,-1             # because we have to do only n-1 iterations to get the desired result.
    slt $t0,$s1,$zero           # t0=0 if s1>=0
    bne $t0,$zero, n_error
    # assert: n>=0

    li $v0,5
    syscall
    move $s2,$v0                # s2 = x0

    li $v0,5
    syscall
    move $s3,$v0                # s3 = y0

loop:
    beq $s1,$zero,loop_exit     # When the value of n becomes 0 we exit the loop (by jumping to loop_exit)

    li $v0,5
    syscall
    move $s4,$v0                # s4 = x1

    slt $t4,$s4,$s2             #t4 is 0 if s4>=s2
    bne $t4,$zero,not_sorted
    # assert: s4>=s2

    li $v0,5
    syscall
    move $s5,$v0                # s5 = y1

    sub $t1,$s4,$s2             # t1 = x1-x0
    add $t2,$s5,$s3             # t2 = y1+y0

    mult $t1,$t2    
    mflo $t3                    # t3 = (y1+y0)*(x1-x0)
    add $s0,$s0,$t3             # update sum = sum + (y1+y0)*(x1-x0)

    move $s2,$s4                # update x, x0 = x1
    move $s3,$s5                # update y, y0 = y1 

    # decrement the value of 'n' else we will be stuck in an infinite loop
    addi $s1,$s1,-1             

    # jump back and check whether the loop is to be executed again or not, accordingly do the needful
    j loop        

# We will jump to n_error when the input value of n<=0
n_error:
    li $v0,4
    la $a0, n_error_msg
    syscall

    li $v0,10
    syscall                     # exit the code

# We will jump to not_sorted when x-coordinates are not in non-decreasing order
not_sorted:
    li $v0,4
    la $a0, not_sorted_msg
    syscall

    li $v0,10
    syscall                     # exit the code

# We have successfully executed our loop
loop_exit:

    mtc1 $s0, $f1
    cvt.s.w $f1, $f1            # Converted the integer sum into a float value, f1 = sum

    li.s $f2, 2.0;              # f2 = 2.0
    div.s $f12, $f1, $f2        # f12 = sum/2 including decimal, because area is half of the value calculated in sum, 
                                # as noted earlier

    #print on console directly, the value stored in $f12 gets printed just like for integer $a0 gets printed
    li $v0,2                    
    syscall                     

    li $v0,10
    syscall                     # exit the code

.data
n_error_msg: .asciiz "\nInvalid value of n, it should be greater than 0\n"
not_sorted_msg: .asciiz "\nInvalid input, provide x-coordinates in non-decreasing order\n"