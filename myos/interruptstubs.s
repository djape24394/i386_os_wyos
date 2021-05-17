
.set IRQ_BASE, 0x20 # platform specific

.section .text

.extern _ZN16InterruptManager15handleInterruptEhj # importing symbol


# creating macro for exporting functions
.macro HandleException num
.global _ZN16InterruptManager16handleException\num\()Ev
_ZN16InterruptManager16handleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm # end macro

.macro HandleInterruptRequest num
.global _ZN16InterruptManager26handleInterruptRequest\num\()Ev
_ZN16InterruptManager26handleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm # end macro

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

int_bottom:

    pusha # save all registers before the cpp function call
    pushl %ds # push data segment
    pushl %es
    pushl %fs
    pushl %gs

    # pushed the arguments for the cpp function and call it
    pushl %esp
    push (interruptnumber) 
    call _ZN16InterruptManager15handleInterruptEhj
    movl %eax, %esp # retrieve back the current stack pointer returned by _ZN16InterruptManager15handleInterruptEhj

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

    iret

.global _ZN16InterruptManager22ignoreInterruptRequestEv
_ZN16InterruptManager22ignoreInterruptRequestEv:

    iret

.data
    interruptnumber: .byte 0
