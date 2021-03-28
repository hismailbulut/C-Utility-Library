#include "containers/LinkedList.h"

#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Debug.h"
#include "MemoryUtils.h"

// PRIVATE BEGIN

static inline LinkedListNode* _NewEmptyNode(LinkedList* list) {
    return CUtilsMalloc(sizeof(LinkedListNode), list->memTracker);
}

static void _SetNodeValue(LinkedList* list, LinkedListNode* node,
                          const void* value) {
    if (node->value == NULL) {
        node->value = CUtilsMalloc(list->stride, list->memTracker);
    }
    memcpy(node->value, value, list->stride);
}

static uint64_t _CalculateListLength(LinkedList* list) {
    LinkedListNode* node = list->first;
    uint64_t current_index = 0;
    while (node) {
        if (node->value) {
            current_index++;
        }
        node = node->next;
    }
    return current_index;
}

static void _RaiseOutOfBounds(LinkedList* list, uint64_t index) {
    uint64_t calculated = _CalculateListLength(list);
    ASSERT_BREAK(calculated == list->size);
    DEBUG_LOG_ERROR("Index out of bounds. Index: %I64u, LinkedList length: %I64u",
                    index, list->size);
}

static LinkedListNode* _GetNodeAt(LinkedList* list, uint64_t index,
                                  bool needPrevNode, LinkedListNode** prevNode) {
    LinkedListNode* node = list->first;
    uint64_t node_index = 0;
    while (node) {
        if (node_index == index && node->value) {
            return node;
        }
        if (needPrevNode) {
            *prevNode = node;
        }
        if (node->value) {
            node_index++;
        }
        node = node->next;
    }
    return NULL;
}
// PRIVATE END

LinkedList* LinkedListCreate(size_t stride) {
    LinkedList* list = malloc(sizeof(LinkedList));
    list->memTracker = MemoryTrackerInit();
    list->first = _NewEmptyNode(list);
    list->last = list->first;
    list->stride = stride;
    list->size = 0;
    return list;
}

void LinkedListClear(LinkedList* list) {
    LinkedListNode* node = list->first;
    while (node) {
        if (node->value) {
            CUtilsFree(node->value, list->memTracker);
        }
        LinkedListNode* next = node->next;
        CUtilsFree(node, list->memTracker);
        node = next;
    }
    list->size = 0;
    list->first = _NewEmptyNode(list);
    list->last = list->first;
}

void LinkedListFree(LinkedList* list) {
    LinkedListClear(list);
    CUtilsFree(list->first, list->memTracker);
    MemoryTrackerClose(list->memTracker);
    free(list);
}

void LinkedListSetValue(LinkedList* list, const void* value, uint64_t index) {
    LinkedListNode* node = _GetNodeAt(list, index, false, NULL);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return;
    }
    _SetNodeValue(list, node, value);
}

void* LinkedListGetValue(LinkedList* list, uint64_t index) {
    LinkedListNode* node = _GetNodeAt(list, index, false, NULL);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return NULL;
    }
    return node->value;
}

uint64_t LinkedListGetSize(LinkedList* list) {
    list->size = _CalculateListLength(list);
    return list->size;
}

void LinkedListPush(LinkedList* list, const void* value) {
    LinkedListNode* node = list->last;
    if (node->value == NULL) {
        _SetNodeValue(list, node, value);
        list->size = _CalculateListLength(list);
    } else {
        LinkedListNode* newNode = _NewEmptyNode(list);
        _SetNodeValue(list, newNode, value);
        newNode->next = NULL;
        node->next = newNode;
        list->last = newNode;
        list->size++;
    }
}

void LinkedListPushAt(LinkedList* list, const void* value, uint64_t index) {
    LinkedListNode* prevNode = NULL;
    LinkedListNode* node = _GetNodeAt(list, index, true, &prevNode);
    if (node == NULL) {
        LinkedListPush(list, value);
        return;
    }
    if (node->value == NULL) {
        _SetNodeValue(list, node, value);
        list->size = _CalculateListLength(list);
    } else {
        LinkedListNode* newNode = _NewEmptyNode(list);
        _SetNodeValue(list, newNode, value);
        if (prevNode) {
            prevNode->next = newNode;
        } else {
            list->first = newNode;
        }
        newNode->next = node;
        list->size++;
    }
}

void* LinkedListPop(LinkedList* list) {
    return LinkedListPopAt(list, LinkedListGetSize(list) - 1);
}

void* LinkedListPopAt(LinkedList* list, uint64_t index) {
    LinkedListNode* prevNode = NULL;
    LinkedListNode* node = _GetNodeAt(list, index, true, &prevNode);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return NULL;
    }
    void* value = malloc(list->stride);
    memcpy(value, node->value, list->stride);
    CUtilsFree(node->value, list->memTracker);
    if (prevNode) {
        prevNode->next = node->next;
        if (node->next == NULL) {
            list->last = prevNode;
        }
        CUtilsFree(node, list->memTracker);
    } else if (node->next == NULL) {
        list->last = list->first;
    }
    list->size--;
    return value;
}
