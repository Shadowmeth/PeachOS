#ifndef LIST_H
#define LIST_H

#include <stddef.h>

struct list_head
{
    struct list_head *next;
    struct list_head *prev;
};

/* Given a pointer to a member inside a struct, get a pointer to the whole struct.
   Works by subtracting the member's offset. */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/* A list is one spare list_head that acts as the head.
 * An emptry list points at itself, so that there are no NULL cases
 * to worry about anywhere else in the code. */
static inline void list_init(struct list_head *head)
{
    head->next = head;
    head->prev = head;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

/* Internal: put item between two known neighbours. */
static inline void __list_add(struct list_head *item,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = item;
    item->next = next;
    item->prev = prev;
    prev->next = item;   
}

/* Add at the front of the list. */
static inline void list_add(struct list_head *item,
                                 struct list_head *head)
{
    __list_add(item, head, head->next);
}

/* Add at the back of the list. */
static inline void list_add_tail(struct list_head *item,
                                 struct list_head *head)
{
    __list_add(item, head->prev, head);
}

/* Insert item just before pos. Use this to keep a sorted list. */
static inline void list_insert_before(struct list_head *item,
                                      struct list_head *pos)
{
    __list_add(item, pos->prev, pos);
}

/* Take item out of whatever list it is in. */
static inline void list_del(struct list_head *item)
{
    item->prev->next = item->next;
    item->next->prev = item->prev;
    item->next = NULL;
    item->prev = NULL;
}

#endif /* LIST_H */
