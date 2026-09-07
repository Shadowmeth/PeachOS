section .asm

global enable_interrupts
enable_interrupts:
        sti
        ret

global disable_interrupts
disable_interrupts:
        cli
        ret

global idt_load
idt_load:
        push ebp
        mov ebp, esp

        mov ebx, [ebp + 8]
        lidt [ebx]
        
        pop ebp
        ret

extern int21h_handler
global int21h
int21h:
        cli
        pushad

        call int21h_handler

        popad
        sti
        iret

extern no_interrupt_handler
global no_interrupt
no_interrupt:
        cli
        pushad

        call no_interrupt_handler

        popad
        sti
        iret
