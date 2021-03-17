#include "Dictionary.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Array.h"
#include "Debug.h"
#include "LinkedList.h"

// PRIVATE BEGIN
static void _FreeDictInnerValue(DictValue* dictValue) {
    if (dictValue->type == TYPE_STRING && dictValue->CString) {
        free(dictValue->CString);
    } else if (dictValue->type == TYPE_NUMBER && dictValue->Number) {
        free(dictValue->Number);
    } else if (dictValue->type == TYPE_FLOAT && dictValue->Float) {
        free(dictValue->Float);
    } else if (dictValue->type == TYPE_BOOL && dictValue->Boolean) {
        free(dictValue->Boolean);
    } else if ((dictValue->type == TYPE_NUMBER_ARRAY || dictValue->type == TYPE_FLOAT_ARRAY) && dictValue->Array) {
        ArrayFree(dictValue->Array);
    } else if (dictValue->type == TYPE_STRING_LIST && dictValue->StringList) {
        LinkedListFree(dictValue->StringList);
    } else if (dictValue->type == TYPE_OBJECT && dictValue->Object) {
        DictionaryFree(dictValue->Object);
    } else {
        /* DEBUG_LOG_INFO("Nothing deleted."); */
        // TODO delete non null value?
    }
    memset(dictValue, 0, sizeof(DictValue));
}

static DictPair* _FindPair(Dictionary* dict, char* key) {
    DictPair* pair = dict->first;
    while (pair) {
        if (strcmp(pair->key, key) == 0) {
            return pair;
        }
        pair = pair->next;
    }
    return NULL;
}
// PRIVATE END

Dictionary* DictionaryCreate() {
    Dictionary* dict = malloc(sizeof(Dictionary));
    dict->first = NULL;
    return dict;
}

void DictionaryFree(Dictionary* dict) {
    DictPair* pair = dict->first;
    while (pair) {
        DictPair* nextPair = pair->next;
        free(pair->key);
        if (pair->value) {
            _FreeDictInnerValue(pair->value);
            free(pair->value);
        }
        free(pair);
        pair = nextPair;
    }
}

DictValue DictionaryCreateValue(DictValueDataTypes dataType, void* value) {
    DictValue dictValue;
    memset(&dictValue, 0, sizeof(dictValue));
    dictValue.type = dataType;
    switch (dataType) {
        case TYPE_STRING:;
            uint64_t stringLen = strlen(value);
            dictValue.CString = malloc(stringLen + 1);
            memcpy(dictValue.CString, value, stringLen + 1);
            break;
        case TYPE_NUMBER:
            dictValue.Number = malloc(sizeof(int64_t));
            memcpy(dictValue.Number, value, sizeof(int64_t));
            break;
        case TYPE_FLOAT:
            dictValue.Float = malloc(sizeof(float));
            memcpy(dictValue.Float, value, sizeof(float));
            break;
        case TYPE_BOOL:
            dictValue.Boolean = malloc(sizeof(bool));
            memcpy(dictValue.Boolean, value, sizeof(bool));
            break;
        case TYPE_NUMBER_ARRAY:
            dictValue.Array = _ArrayCreate(sizeof(int64_t), ArrayGetSize(value));
            ArrayInsert(dictValue.Array, value, ArrayGetSize(value));
            break;
        case TYPE_FLOAT_ARRAY:
            dictValue.Array = _ArrayCreate(sizeof(float), ArrayGetSize(value));
            ArrayInsert(dictValue.Array, value, ArrayGetSize(value));
            break;
        case TYPE_STRING_LIST:;
            dictValue.StringList = LinkedListCreate();
            for (uint64_t i = 0; i < LinkedListGetSize(value); i++) {
                void* v = LinkedListGetElementValue(value, i);
                size_t s = LinkedListGetElementSize(value, i);
                LinkedListPush(dictValue.StringList, v, s);
            }
            break;
        case TYPE_OBJECT:
            dictValue.Object = value;
            break;
    }
    return dictValue;
}

DictValue* DictionaryGet(Dictionary* dict, char* key) {
    DictPair* pair = _FindPair(dict, key);
    if (pair) {
        return pair->value;
    }
    return NULL;
}

void DictionarySet(Dictionary* dict, char* key, DictValue* value) {
    DictPair* founded = _FindPair(dict, key);
    if (founded) {
        if (founded->value) {
            _FreeDictInnerValue(founded->value);
        } else {
            founded->value = malloc(sizeof(DictValue));
        }
        memcpy(founded->value, value, sizeof(DictValue));
    } else {
        DictPair* newPair = malloc(sizeof(DictPair));
        memset(newPair, 0, sizeof(DictPair));
        newPair->key = malloc(strlen(key) + 1);
        memcpy(newPair->key, key, strlen(key) + 1);
        if (value) {
            newPair->value = malloc(sizeof(DictValue));
            memcpy(newPair->value, value, sizeof(DictValue));
        }
        if (dict->first == NULL) {
            dict->first = newPair;
        } else {
            DictPair* pair = dict->first;
            while (pair->next) {
                pair = pair->next;
            }
            pair->next = newPair;
        }
    }
}

void DictionaryRemove(Dictionary* dict, char* key) {
    DictPair* tail = NULL;
    DictPair* pair = dict->first;
    while (pair) {
        if (strcmp(pair->key, key) == 0) {
            break;
        }
        tail = pair;
        pair = pair->next;
    }
    if (pair) {
        DictPair* nextPair = pair->next;
        free(pair->key);
        if (pair->value) {
            _FreeDictInnerValue(pair->value);
            free(pair->value);
        }
        free(pair);
        if (tail) {
            tail->next = nextPair;
        } else {
            dict->first = nextPair;
        }
    } else {
        DEBUG_LOG_WARN("Can't find the given key in dictionary. Nothing removed.");
    }
}
