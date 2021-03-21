#include "containers/List.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Defines.h"
#include "containers/Array.h"
#include "containers/Dictionary.h"

static ListNode* _ListCreateNode(CUtilsDataType type, void* value) {
    ListNode* node = malloc(sizeof(ListNode));
    memset(node, 0, sizeof(ListNode));
    node->dataType = type;
    switch (type) {
        case DATA_TYPE_STRING:;
            uint64_t len = strlen(value);
            node->value = malloc(len + 1);
            memcpy(node->value, value, len + 1);
            break;
        case DATA_TYPE_NUMBER:
            node->value = malloc(sizeof(int64_t));
            memcpy(node->value, value, sizeof(int64_t));
            break;
        case DATA_TYPE_FLOAT:
            node->value = malloc(sizeof(float));
            memcpy(node->value, value, sizeof(float));
            break;
        case DATA_TYPE_BOOL:
            node->value = malloc(sizeof(bool));
            memcpy(node->value, value, sizeof(bool));
            break;
        case DATA_TYPE_LIST:
            node->value = value;
            break;
        case DATA_TYPE_OBJECT:
            node->value = value;
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
    return list;
}

void ListFree(List* list) {
    if (list && list->data) {
        for (uint64_t i = 0; i < ArrayGetSize(list->data); i++) {
            ListNode* node = (ListNode*)list->data[i];
            ListFreeNode(node);
        }
        ArrayFree(list->data);
        list->data = NULL;
        free(list);
    }
}

void ListFreeNode(ListNode* node) {
    if (node->value) {
        switch (node->dataType) {
            case DATA_TYPE_LIST:
                ListFree(node->value);
                break;
            case DATA_TYPE_OBJECT:
                DictionaryFree(node->value);
                break;
            default:
                free(node->value);
                break;
        }
    }
    free(node);
}

void ListSetValue(List* list, uint64_t index, CUtilsDataType type, void* value) {
    if (index >= ArrayGetSize(list->data)) {
        // TODO raise error.
        return;
    }
    ListNode* old = (ListNode*)list->data[index];
    ListFreeNode(old);
    ListNode* new = _ListCreateNode(type, value);
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
    ListNode* new = _ListCreateNode(type, value);
    uint64_t pointer = (uint64_t) new;
    ArrayPush(list->data, pointer);
}

void ListPushAt(List* list, uint64_t index, CUtilsDataType type, void* value) {
    ListNode* new = _ListCreateNode(type, value);
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
