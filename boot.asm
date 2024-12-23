; Bootloader is loaded into memory address 0x7c00
ORG 0

; Tell assembler we want 16 bit code
BITS 16

; Look BIOS parameter block in osdev
_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0x7c0:step2

step2:
    cli ; clear interrupts
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax

    ; setup stack
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00

    sti ; enable interrupts

    mov ah, 0x02 ; Read sector command
    mov al, 1 ; one sector to read
    mov ch, 0 ; cylinder low eight bits
    mov cl, 2 ; read sector number two
    mov dh, 0 ; head number
    xor bx, bx ; zero out bx reg
    inc bx ; set bx to 1
    mov bx, buffer
    int 0x13
    jc error
    mov si, buffer
    call print
    jmp $

error:
    mov si, error_message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0x0e
    int 0x10
    ret

error_message: db 'Failed to load sector', 0

times 510 - ($ - $$) db 0
; Boot signature is 0x55AA but Intel is little endian so bytes are reversed
dw 0xAA55

buffer:
