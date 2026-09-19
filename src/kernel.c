#include "include/kernel.h"
#include "include/terminal.h"
#include "include/idt.h"
#include "include/config.h"
#include "include/heap.h"

void kernel_main()
{
    terminal_clear();
    terminal_print("Welcome to PeachOS\n");

    // Initialize the interrupt descriptor table.
    idt_init();
    terminal_print("Interrupt Descriptor Table is initialized\n");

    // Enable the system interrupts.
    enable_interrupts();
    terminal_print("Interrupts enabled\n");

    // Initialize the heap.
    kheap_init(PEACHOS_HEAP_START);
    terminal_print("Heap initialized\n");
}
