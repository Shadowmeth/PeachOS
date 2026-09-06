#include "include/kernel.h"
#include "include/terminal.h"
#include "include/idt.h"
#include "include/io.h"

void kernel_main()
{
    terminal_clear();
    terminal_print("Hello World!\n");
    terminal_print("Welcome to PeachOS\n");

    // initialize the interrupt descriptor table
    idt_init();

    outb(0x60, 0xFF);
}
