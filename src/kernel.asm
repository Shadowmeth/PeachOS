[BITS 32]
global _start
extern kernel_main

        CODE_SEL equ 0x08
        DATA_SEL equ 0x10

_start:
        mov ax, DATA_SEL
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov ebp, 0x00200000
        mov esp, ebp

        ; Enable the A20 line
        in al, 0x92
        or al, 2
        out 0x92, al

        ; Remap the master PIC
        ; See http://wiki.osdev.org/8259_PIC
        mov al, 00010001b       ; ICW1_INIT | ICW1_ICW4
        out 0x20, al            ; IO base address for master PIC is 0x20

        mov al, 0x20            ; Interrupt 0x20 is where the master ISR should start
        out 0x21, al            ; Data address for master PIC is 0x21

        mov al, 00000001b       ; Have the master PIC use 8086 mode (and not 8080 mode)
        out 0x21, al
        ; End of remap of the master PIC

        call kernel_main
        jmp $

        times 512 - ($ - $$) db 0
