#include "LinkedList.h"

#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Debug.h"

// PRIVATE BEGIN
typedef struct LinkedListNode {
    void* value;
    size_t valueSize;
    struct LinkedListNode* next;
} LinkedListNode;

typedef struct LinkedListHeader {
    uint64_t length;
    LinkedListNode* first;
    LinkedListNode* last;
} LinkedListHeader;

static inline LinkedListNode* _NewEmptyNode() {
    LinkedListNode* node = malloc(sizeof(LinkedListNode));
    memset(node, 0, sizeof(LinkedListNode));
    return node;
}

static void _SetNodeValue(LinkedListNode* node,
                          const void* value, size_t v_size) {
    if (node->value == NULL) {
        node->value = malloc(v_size);
    } else if (node->valueSize != v_size) {
        void* temp = realloc(node->value, v_size);
        if (temp) {
            node->value = temp;
        } else {
            DEBUG_LOG_FATAL("Memory reallocation error on LinkedList.!");
        }
    }
    memcpy(node->value, value, v_size);
    node->valueSize = v_size;
}

static uint64_t _CalculateListLength(LinkedList* list) {
    LinkedListHeader* header = list->data;
    LinkedListNode* node = header->first;
    uint64_t current_index = 0;
    while (node) {
        node = node->next;
        current_index++;
    }
    return current_index;
}

static void _RaiseOutOfBounds(LinkedList* list, uint64_t index) {
    LinkedListHeader* header = list->data;
    uint64_t calculated = _CalculateListLength(list);
    ASSERT_BREAK(calculated == header->length);
    DEBUG_LOG_ERROR("Index out of bounds. Index: %I64u, LinkedList length: %I64u",
                    index, header->length);
}

static LinkedListNode* _GetNodeAt(LinkedList* list, uint64_t index,
                                  bool needPrevNode, LinkedListNode** prevNode) {
    LinkedListHeader* header = list->data;
    LinkedListNode* node = header->first;
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
// PRIVATE END

LinkedList* LinkedListCreate() {
    LinkedList* list = malloc(sizeof(LinkedList));
    LinkedListNode* first = _NewEmptyNode();
    LinkedListHeader* header = malloc(sizeof(LinkedListHeader));
    header->length = 0;
    header->first = first;
    header->last = first;
    list->data = header;
    return list;
}

void LinkedListClear(LinkedList* list) {
    LinkedListHeader* header = list->data;
    if (header->first) {
        LinkedListNode* node = header->first;
        while (node) {
            if (node->value) {
                free(node->value);
            }
            LinkedListNode* next = node->next;
            free(node);
            node = next;
        }
    }
    header->length = 0;
    header->first = NULL;
    header->last = NULL;
}

void LinkedListFree(LinkedList* list) {
    if (list->data) {
        LinkedListClear(list);
        free(list->data);
    }
    free(list);
}

void LinkedListSetElementValue(LinkedList* list, uint64_t index,
                               const void* value, size_t value_size) {
    LinkedListHeader* header = list->data;
    if (header->first == NULL) {
        _RaiseOutOfBounds(list, index);
        return;
    }
    LinkedListNode* node = _GetNodeAt(list, index, false, NULL);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return;
    }
    _SetNodeValue(node, value, value_size);
}

void* LinkedListGetElementValue(LinkedList* list, uint64_t index) {
    LinkedListHeader* header = list->data;
    if (header->first == NULL) {
        _RaiseOutOfBounds(list, index);
        return NULL;
    }
    LinkedListNode* node = _GetNodeAt(list, index, false, NULL);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return NULL;
    }
    return node->value;
}

size_t LinkedListGetElementSize(LinkedList* list, uint64_t index) {
    LinkedListHeader* header = list->data;
    if (header->first == NULL) {
        _RaiseOutOfBounds(list, index);
        return 0;
    }
    LinkedListNode* node = _GetNodeAt(list, index, false, NULL);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return 0;
    }
    return node->valueSize;
}

uint64_t LinkedListGetSize(LinkedList* list) {
    LinkedListHeader* header = list->data;
    return header->length;
}

void LinkedListPush(LinkedList* list, const void* value, size_t value_size) {
    LinkedListHeader* header = list->data;
    if (header->first == NULL) {
        LinkedListNode* first = _NewEmptyNode();
        _SetNodeValue(first, value, value_size);
        first->next = NULL;
        header->first = first;
        header->last = first;
        header->length = 1;
    } else {
        LinkedListNode* node = header->last;
        if (node->value == NULL) {
            _SetNodeValue(node, value, value_size);
            header->length = _CalculateListLength(list);
        } else {
            LinkedListNode* newNode = _NewEmptyNode();
            _SetNodeValue(newNode, value, value_size);
            newNode->next = NULL;
            node->next = newNode;
            header->last = newNode;
            header->length++;
        }
    }
}

void LinkedListPushAt(LinkedList* list, const void* value,
                      size_t value_size, uint64_t index) {
    LinkedListHeader* header = list->data;
    if (header->first == NULL) {
        if (index != 0) {
            _RaiseOutOfBounds(list, index);
            return;
        }
        LinkedListNode* first = _NewEmptyNode();
        _SetNodeValue(first, value, value_size);
        header->first = first;
        header->last = first;
        header->length = 1;
    } else {
        LinkedListNode* prevNode = NULL;
        LinkedListNode* node = _GetNodeAt(list, index, true, &prevNode);
        if (node == NULL) {
            // TODO add new node to the end
            _RaiseOutOfBounds(list, index);
            return;
        }
        if (node->value == NULL) {
            _SetNodeValue(node, value, value_size);
            header->length = _CalculateListLength(list);
        } else {
            LinkedListNode* newNode = _NewEmptyNode();
            _SetNodeValue(newNode, value, value_size);
            if (prevNode) {
                prevNode->next = newNode;
            } else {
                header->first = newNode;
            }
            newNode->next = node;
            header->length++;
        }
    }
}

void* LinkedListPop(LinkedList* list, size_t* outValueSize) {
    return LinkedListPopAt(list, LinkedListGetSize(list) - 1, outValueSize);
}

void* LinkedListPopAt(LinkedList* list, uint64_t index, size_t* outValueSize) {
    LinkedListHeader* header = list->data;
    if (header->first == NULL) {
        _RaiseOutOfBounds(list, index);
        return NULL;
    }
    LinkedListNode* prevNode = NULL;
    LinkedListNode* node = _GetNodeAt(list, index, true, &prevNode);
    if (node == NULL) {
        _RaiseOutOfBounds(list, index);
        return NULL;
    }
    void* value = malloc(node->valueSize);
    memcpy(value, node->value, node->valueSize);
    if (outValueSize) {
        *outValueSize = node->valueSize;
    }
    if (prevNode) {
        prevNode->next = node->next;
    } else {
        header->first = node->next;
    }
    if (node->value) {
        free(node->value);
        node->value = NULL;
    }
    free(node);
    header->length--;
    return value;
}
