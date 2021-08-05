# This code has been designed by :
# 1) Shrey Patel (2019CS10400)
# 2) Aayush Goyal (2019CS10452)

# Assembly program to evaluate postfix expression

.text
.globl main

main:
           
    #current size of stack
    li $s0,0        

    #ASCII values for permissible character
    li $s3,48   # 0
    li $s4,57   # 9
    li $s5,42   # *
    li $s6,43   # +
    li $s7,45   # -

loop:

    #Taking string as input, v0=8 is used for inputting string at syscall
    li $v0,8       

    # Allocating maximum space to string, which is 52 characters
    # a0 stores the input string, a1 stores the length of the string
    la $a0,buffer
    li $a1,1      
    syscall

    #Copying the string onto a global variable s1, so s1 is the string iterator
    move $s1,$a0

    # Exit condition of the loop: The loop is exited once the user presses "Enter" key
    li $t9,10
    lb $s2,($s1)
    beq $s2,$t9,loop_exit

    # Detecting operators: +, * and -
    beq $s2,$s5,multiply
    beq $s2,$s6,addition
    beq $s2,$s7,subtraction

    # Raising error if the character entered is not valid (valid characters: digits in 0 to 9)
    slt $t4,$s2,$s3
    bne $t4,$zero,char_error
    sgt $t4,$s2,$s4
    bne $t4,$zero,char_error

    # Converting the ASCII value in to the integer number between 0 to 9
    addi $s2,$s2,-48

    # We change the position of this pointer for the push operation
    addi $sp,$sp,-4
    sw $s2, ($sp)    

    addi $s0,$s0,1      # Increasing the size of stack by 1 

    j loop              #Jumping back to the loop

multiply:

    # Checking if the stack contains at least 2 elements, because the binary operation requires two arguments
    slti $t0,$s0,2
    bne $t0,$zero,exp_error

    # Popping the top two elements from the stack and storing them in t1 and t2. 
    lw $t1,($sp)
    lw $t2,4($sp)
    addi $sp,$sp,4      # The stack pointer is $sp is updated accordingly (i.e. increased by 4 bytes)

    # Pushing the product stored in t3 on the top of the stack 
    mult $t1, $t2			
    mflo $t3
    sw $t3,($sp)

    #Updating the size of the stack(decreased by 1)
    addi $s0,$s0,-1

    j loop	            #Jumping back to the loop				


addition:
    
    # Checking if the stack contains at least 2 elements, because the binary operation requires two arguments
    slti $t0,$s0,2
    bne $t0,$zero,exp_error

    # Popping the top two elements from the stack and storing them in t1 and t2.
    lw $t1,($sp)
    lw $t2,4($sp)
    addi $sp,$sp,4       # The stack pointer is $sp is updated accordingly (i.e. increased by 4 bytes)

    # Pushing the sum stored in t3 on the top of the stack
    add	$t3, $t1, $t2
    sw $t3,($sp)

    # Updating the size of the stack(decreased by 1)
    addi $s0,$s0,-1

    j loop              #Jumping back to the loop              

subtraction:

    # Checking if the stack contains at least 2 elements, because the binary operation requires two arguments
    slti $t0,$s0,2
    bne $t0,$zero,exp_error
    
    # Popping the top two elements from the stack and storing them in t1 and t2.
    lw $t1,($sp)
    lw $t2,4($sp)
    addi $sp,$sp,4      # The stack pointer is $sp is updated accordingly (i.e. increased by 4 bytes)

    # Pushing the difference of t2 and t1 stored in t3 on the top of the stack
    sub	$t3, $t2, $t1
    sw $t3,($sp)

    #Updating the size of the stack(decreased by 1)
    addi $s0,$s0,-1
    
    j loop    

# Raising error in case an invalid character is entered
char_error:

    li $v0,4
    la $a0,error
    syscall

    li $v0,10
    syscall

# Raising error when the postfix expression is incorrect
exp_error:

    li $v0,4
    la $a0,error2
    syscall

    li $v0,10
    syscall

# Raising error when the input string is empty
empty_error:

    li $v0,4
    la $a0,error3
    syscall

    li $v0,10
    syscall    

loop_exit:

    # Exception handling
    li $t5,1
    beq $s0,$zero,empty_error
    bne	$s0,$t5,exp_error

    # Printing the final result which is stored on the top of the stack
    li $v0,1
    lw $a0, ($sp)
    syscall

    li $v0,4
    la $a0,newline
    syscall

    li $v0,10
    syscall                 #exit

#Error messages to be printed on console
.data
buffer: .space 1
error: .asciiz "\nInvalid character\n"
error2: .asciiz "\nInvalid expression\n"
error3: .asciiz "Empty expression\n"
newline: .asciiz "\n"