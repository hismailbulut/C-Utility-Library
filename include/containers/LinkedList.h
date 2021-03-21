#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct LinkedList {
    void* data;
} LinkedList;

LinkedList* LinkedListCreate();

void LinkedListClear(LinkedList* list);

void LinkedListFree(LinkedList* list);

void LinkedListSetElementValue(LinkedList* list, uint64_t index,
                               const void* value, size_t value_size);

void* LinkedListGetElementValue(LinkedList* list, uint64_t index);

size_t LinkedListGetElementSize(LinkedList* list, uint64_t index);

uint64_t LinkedListGetSize(LinkedList* list);

void LinkedListPush(LinkedList* list, const void* value, size_t valueSize);
#define LinkedListPushLV(list, value) \
    LinkedListPush(list, &value, sizeof(value))
#define LinkedListPushRV(list, type, value)        \
    {                                              \
        type temp = value;                         \
        LinkedListPush(list, &temp, sizeof(type)); \
    }

void LinkedListPushAt(LinkedList* list, const void* value,
                      size_t valueSize, uint64_t index);
#define LinkedListPushAtLV(list, value, index) \
    LinkedListPushAt(list, &value, sizeof(value), index)
#define LinkedListPushAtRV(list, type, value, index)        \
    {                                                       \
        type temp = value;                                  \
        LinkedListPushAt(list, &temp, sizeof(type), index); \
    }

void* LinkedListPop(LinkedList* list, size_t* outValueSize);

void* LinkedListPopAt(LinkedList* list, uint64_t index, size_t* outValueSize);
