
.set IRQ_BASE, 0x20 # platform specific

.section .text

.extern _ZN4myos22hardware_communication16InterruptManager15handleInterruptEhj # importing symbol


# creating macro for exporting functions
.macro HandleException num
.global _ZN16InterruptManager16handleException\num\()Ev
_ZN16InterruptManager16handleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm # end macro

.macro HandleInterruptRequest num
.global _ZN4myos22hardware_communication16InterruptManager26handleInterruptRequest\num\()Ev
_ZN4myos22hardware_communication16InterruptManager26handleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0  #  need to push it because of CPUstate alignment, for exceptions this is done by cpu
    jmp int_bottom
.endm # end macro

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31
HandleInterruptRequest 0x80


int_bottom:

    # pusha # save all registers before the cpp function call
    # pushl %ds # push data segment
    # pushl %es
    # pushl %fs
    # pushl %gs
    
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # pushed the arguments for the cpp function and call it
    pushl %esp
    push (interruptnumber) 
    call _ZN4myos22hardware_communication16InterruptManager15handleInterruptEhj
    movl %eax, %esp # retrieve back the current stack pointer returned by _ZN4myos22hardware_communication16InterruptManager15handleInterruptEhj

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp

    # popl %gs
    # popl %fs
    # popl %es
    # popl %ds
    # popa

    add $4, %esp

.global _ZN4myos22hardware_communication16InterruptManager22ignoreInterruptRequestEv
_ZN4myos22hardware_communication16InterruptManager22ignoreInterruptRequestEv:

    iret

.data
    interruptnumber: .byte 0
