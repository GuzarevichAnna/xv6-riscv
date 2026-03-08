.section .data
    input_buf: .zero 24
    sum_fmt: .asciz "%lld\n\0"
    gets_error_msg: .asciz "error reading arguments\n"
    atoi_error_msg: .asciz "error parsing arguments\n"
    printf_error_msg: .asciz "error printing sum via printf\n"
    num1: .word 0
    num2: .word 0
    temp_buf: .space 11  # 10 digits + '\0'

.section .text
.global main
main:
    la a0, input_buf
    li a1, 24
    call gets
    li a1, 0
    beq a0, a1, gets_error_exit
    
    # save temp_buf to later be able to check for parsing errors
    la t5, temp_buf

    la t0, input_buf
    li t1, 0          # input_idx
    li t2, 0          # temp_idx
    la t3, temp_buf
    
    parse_first_loop:
    lbu a1, 0(t0)
    li a2, 32  # space
    beq a1, a2, parse_first_done
    sb a1, 0(t3)
    addi t0, t0, 1
    addi t1, t1, 1
    addi t2, t2, 1
    addi t3, t3, 1
    jal parse_first_loop

    parse_first_done:    
    la a0, temp_buf
    call atoi
    # check if atoi resulten in error (i.e. its return value is 0 but temp_buf does not contain zero
    li a1, 0
    bne a0, a1, atoi_1_succeeded
    lbu a1, 0(t5)
    li a2, 48 # "0"
    bne a1, a2, atoi_error_exit
    lbu a1, 1(t5)
    li a2, 0 # '\0'
    bne a1, a2, atoi_error_exit

    atoi_1_succeeded:
    la t4, num1
    sw a0, 0(t4)

    addi t0, t0, 1
    addi t1, t1, 1
    li t2, 0
    la t3, temp_buf
    
    parse_second_loop:
    lbu a1, 0(t0)
    beqz a1, parse_second_done
    sb a1, 0(t3)
    addi t0, t0, 1
    addi t1, t1, 1
    addi t2, t2, 1
    addi t3, t3, 1
    jal parse_second_loop

    parse_second_done:            
    la a0, temp_buf
    call atoi
    li a1, 0
    bne a0, a1, atoi_2_succeeded
    lbu a1, 0(t5)
    li a2, 48 # '0'
    bne a1, a2, atoi_error_exit
    lbu a1, 1(t5)
    li a2, 10 # '\n'
    bne a1, a2, atoi_error_exit

    atoi_2_succeeded:
    la t4, num2
    sw a0, 0(t4)
        
    la a0, sum_fmt
    lw a1, num1
    lw a2, num2
    add a1, a1, a2    
    call printf
    li a1, 0
    blt a0, a1, printf_error_exit
    
    li a0, 0
    call exit

    gets_error_exit:
    li a0, 2
    la a1, gets_error_msg
    call fprintf
    li a0, 1
    call exit

    atoi_error_exit:
    li a0, 2
    la a1, atoi_error_msg
    call fprintf
    li a0, 1
    call exit

    printf_error_exit:
    li a0, 2
    la a1, printf_error_msg
    call fprintf
    li a0, 1
    call exit
    
