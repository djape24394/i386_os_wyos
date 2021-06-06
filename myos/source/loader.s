.set MAGIC, 0x1badb002 # bootloader requires this magic number to acknowledge kernel.bin as a valid kernel. With .set we make this compiler flags.
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

# add magic numbers and flags to the object file
.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


.section .text
.extern kernelMain
.extern callConstructors
.global loader


loader:
    mov $kernel_stack, %esp

    call callConstructors
    # when bootloder(GRUB) acknowledges our kernel.bin, it creates that multiboot structure with some informations, like ram size, etc. Pointer to this structure 
    # is stored then in eax and MAGIC number is copied to ebx. We pass these to kernelMain function.
    push %eax
    push %ebx
    call kernelMain

# we should never get out of kernelMain, but just to be shure
_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024; # 2 MiB
kernel_stack:
# we will put the address of kernel_stack to the stack pointer. As stack gets filled from right to the left(to lower address),
# we make 2 MB of free space
