#include "include/heap.h"

static const void *heap_start = NULL;
static struct heap_block *start_block = NULL;

static struct list_head all_blocks;
static struct list_head free_blocks;

/* Useful for debugging. */
size_t heap_blocks_count = 0;
size_t heap_allocated_memory = 0;

/* Prototypes */
static struct heap_block *insert_heap_block(void *addr, size_t size);
static struct heap_block *find_free_block(const size_t needed);
static void mark_block_free(struct heap_block *heap_block);
static void mark_block_used(struct heap_block *heap_block);
static struct heap_block *grow_heap(const size_t needed);
static void split_block(struct heap_block *block, const size_t needed);
static struct heap_block *next_block(struct heap_block *block);
static struct heap_block *prev_block(struct heap_block *block);
static void merge_blocks(struct heap_block *keep, struct heap_block *gone);

/* Mark the given block as freed. */
static void mark_block_free(struct heap_block *heap_block)
{
    heap_block->free = 1;
}

/* Mark the given block as in-use. */
static void mark_block_used(struct heap_block *heap_block)
{
    heap_block->free = 0;
}

void kheap_init(void *heap_beginning)
{
    heap_start = (void *)align_up_ptr((uintptr_t)heap_beginning, HEAP_ALIGNMENT);
    const size_t first_block_size = align_up_size(1024, HEAP_ALIGNMENT);
    
    list_init(&all_blocks);
    list_init(&free_blocks);

    /* Forcefully insert a small block right at heap_start. */
    /* Go to the heap start and insert a block. */
    start_block = insert_heap_block((void *)heap_start, first_block_size);
    mark_block_free(start_block);

    list_add(&start_block->all, &all_blocks);
    list_add(&start_block->free_list, &free_blocks);

    heap_blocks_count++;
    heap_allocated_memory += first_block_size;
}

/* Insert a new heap block of aligned size at addr.
 * Return the address of the heap_block. */
static struct heap_block *insert_heap_block(void *addr, size_t size)
{
    const size_t _size = align_up_size(size, HEAP_ALIGNMENT);
    struct heap_block *heap_block = (struct heap_block *)addr;
    heap_block->free = 1;
    heap_block->size = _size;
    heap_block->magic = 0xDEADBEEF;
    return heap_block;
}

/* Find the first block that is free and can hold the requested size.
 * Return NULL if no such block found. */
static struct heap_block *find_free_block(const size_t needed)
{
    struct list_head *pos;

    for (pos = free_blocks.next; pos != &free_blocks; pos = pos->next)
    {
        struct heap_block *candidate = list_entry(pos, struct heap_block, free_list);

        if (candidate->size >= needed)
            return candidate;
    }

    return NULL;
}

/* Split the given block if we need less memory than blocks' size.
 * Make a block in the remaining free space and add it to free list
 * also. */
static void split_block(struct heap_block *block, const size_t needed)
{
    const size_t leftover = block->size - needed;

    /* The remaining space won't be big enough to hold a block.
     * So don't split it. */
    if (leftover < sizeof(struct heap_block) + HEAP_ALIGNMENT)
    {
        return;
    }

    void *new_addr = (char *)block + sizeof(struct heap_block) + needed;
    struct heap_block *rest = insert_heap_block(new_addr, leftover - sizeof(struct heap_block));
    mark_block_free(rest);

    list_add(&rest->all, &block->all);
    list_add(&rest->free_list, &free_blocks);

    block->size = needed;
    heap_blocks_count++;
    heap_allocated_memory += block->size;
}

/* Grow the heap if no block on free_list was big enough to
 * the requested size. There is a special case where the last
 * block is free but not big enough to hold the requested size.
 * In this case just grow the the last block instead of creating
 * a new block because kfree will be merging consective free blocks
 * to deal with external fragmentation. */
static struct heap_block *grow_heap(const size_t needed)
{
    struct heap_block *last = list_entry(all_blocks.prev, struct heap_block, all);
    
    /* If the last block is free grow that and return it. */
    if (last->free)
    {
        const size_t extra = needed - last->size;
        last->size = needed;
        heap_allocated_memory+= extra;
        return last;
    }

    /* Last block is not free. Extend the heap and create a new block. */
    void *new_addr = (char *)last + sizeof(struct heap_block) + last->size;
    struct heap_block *block = insert_heap_block(new_addr, needed);
    mark_block_free(block);

    list_add_tail(&block->all, &all_blocks);
    list_add(&block->free_list, &free_blocks);

    heap_blocks_count++;
    heap_allocated_memory += block->size;

    return block;
}

void *kmalloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    const size_t needed = align_up_size(size, HEAP_ALIGNMENT);
    struct heap_block *first_fit_block = find_free_block(needed);

    /* Grow the heap if we don't find a block big enough in free list. */
    if (!first_fit_block)
    {
        first_fit_block = grow_heap(needed);
        if (!first_fit_block)
            return NULL;
    }

    split_block(first_fit_block, needed);

    mark_block_used(first_fit_block);
    list_del(&first_fit_block->free_list);

    /* Return the address of the actual memory excluding the header. */
    return (void *)((char *)first_fit_block + sizeof(struct heap_block));
}

static struct heap_block *next_block(struct heap_block *block)
{
    if (block->all.next == &all_blocks)
    {
        return NULL;
    }

    return list_entry(block->all.next, struct heap_block, all);
}

static struct heap_block *prev_block(struct heap_block *block)
{
    if (block->all.prev == &all_blocks)
    {
        return NULL;
    }

    return list_entry(block->all.prev, struct heap_block, all);
}

/* Keep must sit right before gone in memory, and both must be free.
 * gone disappears into keep.
 */
static void merge_blocks(struct heap_block *keep, struct heap_block *gone)
{
    keep->size += sizeof(struct heap_block) + gone->size;

    list_del(&gone->all);
    list_del(&gone->free_list);

    gone->magic = 0;
    
    heap_blocks_count--;
}

void kfree(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    struct heap_block *block = (struct heap_block *)((char *)ptr - sizeof(struct heap_block));

    if (block->magic != 0xDEADBEEF)
    {
        /* Wrong pointer or header got corrupted. */
        /* Make a panic handler and call it here. */
        return;
    }

    if (block->free)
    {
        /* Can't free an already freed block. */
        /* No need to panic but inform user that double free was performed. */
        return;
    }

    mark_block_free(block);
    list_add(&block->free_list, &free_blocks);

    /* Merge right first. Merging left after right would change blocks position. */
    struct heap_block *right = next_block(block);
    if (right && right->free)
    {
        merge_blocks(block, right);
    }

    struct heap_block *left = prev_block(block);
    if (left && left->free)
    {
        merge_blocks(left, block);
    }
}
