#include "include/config.h"
#include "include/idt.h"
#include "include/io.h"
#include "include/memory.h"
#include "include/terminal.h"

struct idt_entry idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];
struct idt_table idt;

void int21h_handler(void)
{
    terminal_print("Keyboard pressed!\n");
    outb(0x20, 0x20);
}

void no_interrupt_handler(void)
{
    outb(0x20, 0x20);
}

void idt_init(void)
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idt.base = (uint32_t)idt_descriptors;
    idt.limit = sizeof(idt_descriptors) - 1;

    for (uint16_t i = 0; i < PEACHOS_TOTAL_INTERRUPTS; i++) {
        idt_set_descriptor(i, no_interrupt);
    }

    idt_set_descriptor(0x21, int21h);
    
    // load the interrupt descriptor table
    idt_load(&idt);
}

void idt_set_descriptor(uint8_t vector, void *isr)
{
    struct idt_entry *descriptor = &idt_descriptors[vector];

    descriptor->zero = 0;
    descriptor->type_attributes = 0xEE;
    descriptor->selector = KERNEL_CODE_SELECTOR;
    descriptor->offset_1 = (uint32_t)isr & 0xFFFF;
    descriptor->offset_2 = (uint32_t)isr >> 16;
}
