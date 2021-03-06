#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "MemoryUtils.h"
#include "containers/List.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ListNode {
    CUtilsDataType dataType;
    void* value;
} ListNode;

typedef struct List {
    uint64_t* data;
} List;

/* Creates and empty list. */
List* ListCreate();

List* ListCopy(List* list);

/* Deletes list */
void ListFree(List* list);

/* Only free popped nodes. Get function does not copy the node. */
void ListFreeNode(List* list, ListNode* node);

void ListSetValue(List* list, uint64_t index, CUtilsDataType type, void* value);

/* Returns a pointer to value at index. Don't free. */
ListNode* ListGetValue(List* list, uint64_t index);

void ListPush(List* list, CUtilsDataType type, void* value);

#define ListPushNumber(list, val)             \
    {                                         \
        int64_t t = val;                      \
        ListPush(list, DATA_TYPE_NUMBER, &t); \
    }
#define ListPushFloat(list, val)             \
    {                                        \
        float t = val;                       \
        ListPush(list, DATA_TYPE_FLOAT, &t); \
    }
#define ListPushBool(list, val)             \
    {                                       \
        bool t = val;                       \
        ListPush(list, DATA_TYPE_BOOL, &t); \
    }

void ListPushAt(List* list, uint64_t index, CUtilsDataType type, void* value);

#define ListPushAtNumber(list, index, val)             \
    {                                                  \
        int64_t t = val;                               \
        ListPushAt(list, index, DATA_TYPE_NUMBER, &t); \
    }
#define ListPushAtFloat(list, index, val)             \
    {                                                 \
        float t = val;                                \
        ListPushAt(list, index, DATA_TYPE_FLOAT, &t); \
    }
#define ListPushAtBool(list, index, val)             \
    {                                                \
        bool t = val;                                \
        ListPushAt(list, index, DATA_TYPE_BOOL, &t); \
    }

ListNode* ListPop(List* list);
ListNode* ListPopAt(List* list, uint64_t index);

uint64_t ListGetSize(List* list);
uint64_t ListGetCapacity(List* list);

#ifdef __cplusplus
}
#endif
