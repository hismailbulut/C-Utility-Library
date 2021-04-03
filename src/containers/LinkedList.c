#include "containers/LinkedList.h"

#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Debug.h"
#include "MemoryUtils.h"

// PRIVATE BEGIN

static void _SetNodeValue(LinkedList* list, LinkedListNode* node,
                          const void* value) {
    if (node->value == NULL) {
        node->value = CUtilsMalloc(list->stride);
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

static LinkedListNode* _GetNodeAt(LinkedList* list, uint64_t index,
                                  bool needPrevNode, LinkedListNode** prevNode) {
    LinkedListNode* node = list->first;
    uint64_t node_index = 0;
    while (node) {
        if (node_index == index) {
            return node;
        }
        if (needPrevNode) {
            *prevNode = node;
        }
        node = node->next;
        node_index++;
    }
    return NULL;
}

static void _RaiseOutOfBounds(LinkedList* list, uint64_t index) {
    uint64_t calculated = _CalculateListLength(list);
    ASSERT_BREAK(calculated == list->size);
    DEBUG_LOG_ERROR("Index out of bounds. Index: %I64u, LinkedList length: %I64u",
                    index, list->size);
}
// PRIVATE END

LinkedList* LinkedListCreate(size_t stride) {
    LinkedList* list = CUtilsMalloc(sizeof(LinkedList));
    list->first = CUtilsMalloc(sizeof(LinkedListNode));
    list->last = list->first;
    list->stride = stride;
    list->size = 0;
    return list;
}

void LinkedListClear(LinkedList* list) {
    // Never delete first element of the list.
    // But delete its value.
    LinkedListNode* node = list->first->next;
    while (node) {
        if (node->value) {
            CUtilsFree(node->value);
        }
        LinkedListNode* next = node->next;
        CUtilsFree(node);
        node = next;
    }
    if (list->first->value) {
        CUtilsFree(list->first->value);
        list->first->value = NULL;
    }
    list->size = 0;
    list->last = list->first;
}

void LinkedListFree(LinkedList* list) {
    LinkedListClear(list);
    // Delete the first element now.
    CUtilsFree(list->first);
    CUtilsFree(list);
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
        LinkedListNode* newNode = CUtilsMalloc(sizeof(LinkedListNode));
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
        LinkedListNode* newNode = CUtilsMalloc(sizeof(LinkedListNode));
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
    return LinkedListPopAt(list, list->size - 1);
}

void* LinkedListPopAt(LinkedList* list, uint64_t index) {
    LinkedListNode* prevNode = NULL;
    LinkedListNode* node = _GetNodeAt(list, index, true, &prevNode);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return NULL;
    }
    // just return a pointer to the value
    void* value = node->value;
    node->value = NULL;
    bool deleteNode = true;
    if (0) {
    } else if (prevNode == NULL && node->next != NULL) {
        // first element, size is bigger than 1
        list->first = node->next;
    } else if (prevNode != NULL && node->next == NULL) {
        // last element, size is bigger than 1
        list->last = prevNode;
        prevNode->next = NULL;
    } else if (prevNode != NULL && node->next != NULL) {
        // at middle
        prevNode->next = node->next;
    } else if (prevNode == NULL && node->next == NULL) {
        // first and last element, size is 1
        // don't delete the last node
        deleteNode = false;
    }
    if (deleteNode) {
        CUtilsFree(node);
        list->size--;
    } else {
        list->size = 0;
    }
    return value;
}
