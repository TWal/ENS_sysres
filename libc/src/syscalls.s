    .global _exit
_exit:
    mov $58, %eax
    syscall
    ret

    .global debug
debug:
    mov $42, %eax
    syscall
    ret
