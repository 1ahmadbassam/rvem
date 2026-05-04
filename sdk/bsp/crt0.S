.section .text.init
.global _start

_start:
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    la sp, __stack_top

    la a0, _sidata
    la a1, _sdata
    la a2, _edata
copy_data_loop:
    bge a1, a2, copy_data_done
    lw t0, 0(a0)
    sw t0, 0(a1)
    addi a0, a0, 4
    addi a1, a1, 4
    j copy_data_loop
copy_data_done:

    la a0, _sbss
    la a1, _ebss
zero_bss_loop:
    bge a0, a1, zero_bss_done
    sw zero, 0(a0)
    addi a0, a0, 4
    j zero_bss_loop
zero_bss_done:

    call main

    li a0, 10
    ecall
halt:
    j halt
