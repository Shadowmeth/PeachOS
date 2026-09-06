#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t offset_1;          // offset bits 0..15
    uint16_t selector;          // a code segment selector in GDT or LDT
    uint8_t  zero;              // unused, set to 0
    uint8_t  type_attributes;   // gate type, dpl and p fields
    uint16_t offset_2;          // offset bits 16..31
} __attribute__((packed));

struct idt_table {
    uint16_t limit;             // size of descriptor table - 1
    uint32_t base;              // base address of the start of the table
} __attribute__((packed));

void idt_init(void);
void idt_set_descriptor(uint8_t vector, void *isr);
extern void idt_load(struct idt_table *ptr);

#endif
