#include "containers/List.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MemoryUtils.h"
#include "containers/Array.h"
#include "containers/Dictionary.h"

static ListNode* _ListCreateNode(List* list, CUtilsDataType type, void* value) {
    ListNode* node = CUtilsMalloc(sizeof(ListNode), list->memTracker);
    memset(node, 0, sizeof(ListNode));
    node->dataType = type;
    switch (type) {
        case DATA_TYPE_STRING:;
            uint64_t len = strlen(value);
            node->value = CUtilsMalloc(len + 1, list->memTracker);
            memcpy(node->value, value, len + 1);
            break;
        case DATA_TYPE_NUMBER:
            node->value = CUtilsMalloc(sizeof(int64_t), list->memTracker);
            memcpy(node->value, value, sizeof(int64_t));
            break;
        case DATA_TYPE_FLOAT:
            node->value = CUtilsMalloc(sizeof(float), list->memTracker);
            memcpy(node->value, value, sizeof(float));
            break;
        case DATA_TYPE_BOOL:
            node->value = CUtilsMalloc(sizeof(bool), list->memTracker);
            memcpy(node->value, value, sizeof(bool));
            break;
        case DATA_TYPE_LIST:
            node->value = ListCopy(value);
            break;
        case DATA_TYPE_OBJECT:
            node->value = DictionaryCopy(value);
            break;
        default:
            node->value = NULL;
            break;
    }
    return node;
}

List* ListCreate() {
    List* list = malloc(sizeof(List));
    list->data = ArrayCreate(uint64_t);
    list->memTracker = MemoryTrackerInit();
    return list;
}

List* ListCopy(List* list) {
    List* cpy = ListCreate();
    for (uint64_t i = 0; i < ArrayGetSize(list->data); i++) {
        ListNode* node = (ListNode*)list->data[i];
        ListPush(cpy, node->dataType, node->value);
    }
    return cpy;
}

void ListFree(List* list) {
    if (list == NULL) {
        return;
    }
    if (list->data) {
        for (uint64_t i = 0; i < ArrayGetSize(list->data); i++) {
            ListNode* node = (ListNode*)list->data[i];
            ListFreeNode(list, node);
        }
        ArrayFree(list->data);
        list->data = NULL;
    }
    free(list);
}

void ListFreeNode(List* list, ListNode* node) {
    if (node->value) {
        switch (node->dataType) {
            case DATA_TYPE_LIST:
                ListFree(node->value);
                break;
            case DATA_TYPE_OBJECT:
                DictionaryFree(node->value);
                break;
            default:
                CUtilsFree(node->value, list->memTracker);
                break;
        }
    }
    CUtilsFree(node, list->memTracker);
}

void ListSetValue(List* list, uint64_t index, CUtilsDataType type, void* value) {
    if (index >= ArrayGetSize(list->data)) {
        // TODO raise error.
        return;
    }
    ListNode* old = (ListNode*)list->data[index];
    ListFreeNode(list, old);
    ListNode* new = _ListCreateNode(list, type, value);
    list->data[index] = (uint64_t) new;
}

ListNode* ListGetValue(List* list, uint64_t index) {
    if (index >= ArrayGetSize(list->data)) {
        // TODO raise error.
        return NULL;
    }
    return (ListNode*)list->data[index];
}

void ListPush(List* list, CUtilsDataType type, void* value) {
    ListNode* new = _ListCreateNode(list, type, value);
    uint64_t pointer = (uint64_t) new;
    ArrayPush(list->data, pointer);
}

void ListPushAt(List* list, uint64_t index, CUtilsDataType type, void* value) {
    ListNode* new = _ListCreateNode(list, type, value);
    uint64_t pointer = (uint64_t) new;
    ArrayPushAt(list->data, pointer, index);
}

ListNode* ListPop(List* list) {
    uint64_t* pointer = ArrayPop(list->data);
    if (pointer == NULL) {
        return NULL;
    }
    ListNode* node = (ListNode*)*pointer;
    free(pointer);
    return node;
}

ListNode* ListPopAt(List* list, uint64_t index) {
    uint64_t* pointer = ArrayPopAt(list->data, index);
    if (pointer == NULL) {
        return NULL;
    }
    ListNode* node = (ListNode*)*pointer;
    free(pointer);
    return node;
}

uint64_t ListGetSize(List* list) {
    return ArrayGetSize(list->data);
}

uint64_t ListGetCapacity(List* list) {
    return ArrayGetCapacity(list->data);
}
