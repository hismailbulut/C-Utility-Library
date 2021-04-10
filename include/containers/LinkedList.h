#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "MemoryUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LinkedListNode {
    void* value;
    struct LinkedListNode* next;
} LinkedListNode;

typedef struct LinkedList {
    size_t stride;
    uint64_t size;
    LinkedListNode* first;
    LinkedListNode* last;
} LinkedList;

/* Creates an empty LinkedList. Stride is the size of the each element. */
LinkedList* LinkedListCreate(size_t stride);

/* Deletes all nodes. */
void LinkedListClear(LinkedList* list);

void LinkedListFree(LinkedList* list);

/* Sets value at index. */
void LinkedListSetValue(LinkedList* list, const void* value, uint64_t index);

/* Returns a pointer to value at index. */
void* LinkedListGetValue(LinkedList* list, uint64_t index);

/* Recalculates and returns the linkedlist size. */
uint64_t LinkedListGetSize(LinkedList* list);

/* Adds the value to end of the linkedlist. */
void LinkedListPush(LinkedList* list, const void* value);
#define LinkedListPushRV(list, type, value) \
    {                                       \
        type temp = value;                  \
        LinkedListPush(list, &temp);        \
    }

/* Adds the value to index of the linkedlist. */
void LinkedListPushAt(LinkedList* list, const void* value, uint64_t index);
#define LinkedListPushAtRV(list, type, value, index) \
    {                                                \
        type temp = value;                           \
        LinkedListPushAt(list, &temp, index);        \
    }

/* Pops last element from linkedlist and returns its value.
 * Don't forget to free the value. */
void* LinkedListPop(LinkedList* list);

/* Pops element at index and returns its value.
 * Don't forget to free the value. */
void* LinkedListPopAt(LinkedList* list, uint64_t index);

#ifdef __cplusplus
}
#endif
