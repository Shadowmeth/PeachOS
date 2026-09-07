#include "include/kernel.h"
#include "include/terminal.h"
#include "include/idt.h"

void kernel_main()
{
    terminal_clear();
    terminal_print("Welcome to PeachOS\n");

    // Initialize the interrupt descriptor table
    idt_init();

    // Enable the system interrupts
    enable_interrupts();
}
