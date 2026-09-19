#ifndef HEAP_H
#define HEAP_H

#include "include/list.h"
#include "include/config.h"
#include <stddef.h>
#include <stdint.h>

#define HEAP_ALIGNMENT 8

struct heap_block
{
    size_t size;
    uint32_t magic;
    uint8_t free;

    struct list_head all;
    struct list_head free_list;
};

/**
 * Aligns an address upward to the next boundary.
 * @param address   The unaligned address/integer.
 * @param alignment The alignment boundary (must be a power of 2, e.g., 4, 8, 16, 64).
 * @return          The aligned address.
 */
static inline uintptr_t align_up_ptr(uintptr_t address, size_t alignment) {
    return (address + (alignment - 1)) & ~(alignment - 1);
}

/**
 * Aligns a size upward to the next boundary.
 * @param address   The unaligned size.
 * @param alignment The alignment boundary (must be a power of 2, e.g., 4, 8, 16, 64).
 * @return          The aligned size.
 */
static inline uint32_t align_up_size(uint32_t size, size_t alignment) {
    return (size + (alignment - 1)) & ~(alignment - 1);
}

void kheap_init(void *heap_start);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif /* HEAP_H */
