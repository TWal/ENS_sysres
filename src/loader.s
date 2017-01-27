.global loader

.equ MAGIC_NUMBER, 0x1BADB002
.equ FLAGS, 0x0
.equ CHECKSUM, -MAGIC_NUMBER - FLAGS
# MAGICNUMBER + FLAGS + CHECKSUM = 0

.equ STACKSIZE, 4096

.bss
kernel_stack:
  .space STACKSIZE


.text
.int MAGIC_NUMBER, FLAGS, CHECKSUM

loader:
  mov $(STACKSIZE + kernel_stack),%esp
  call kmain
loop:
  jmp loop
