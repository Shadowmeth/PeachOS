#include "include/config.h"
#include "include/idt.h"
#include "include/memory.h"
#include "include/terminal.h"

struct idt_entry idt_descriptors[PEACHOS_TOTAL_INTERRUPTS];
struct idt_table idt;

void idt_init(void)
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idt.base = (uint32_t)idt_descriptors;
    idt.limit = sizeof(idt_descriptors) - 1;

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
