#include "containers/Dictionary.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "containers/Array.h"
#include "containers/List.h"

// PRIVATE BEGIN
static void _FreePairValue(DictPair* pair) {
    if (pair->value) {
        if (pair->valueType == DATA_TYPE_LIST) {
            ListFree(pair->value);
        } else if (pair->valueType == DATA_TYPE_OBJECT) {
            DictionaryFree(pair->value);
        } else {
            free(pair->value);
        }
    }
    pair->value = NULL;
    pair->valueType = -1;
}

void _DictionarySetPairValue(DictPair* pair, CUtilsDataType valueType, void* value) {
    pair->valueType = valueType;
    switch (valueType) {
        case DATA_TYPE_STRING:
            pair->value = malloc(strlen(value) + 1);
            memcpy(pair->value, value, strlen(value) + 1);
            break;
        case DATA_TYPE_NUMBER:
            pair->value = malloc(sizeof(int64_t));
            memcpy(pair->value, value, sizeof(int64_t));
            break;
        case DATA_TYPE_FLOAT:
            pair->value = malloc(sizeof(float));
            memcpy(pair->value, value, sizeof(float));
            break;
        case DATA_TYPE_BOOL:
            pair->value = malloc(sizeof(bool));
            memcpy(pair->value, value, sizeof(bool));
            break;
        default:
            pair->value = value;
            break;
    }
}

DictPair* _DictionaryCreatePair(char* key, CUtilsDataType valueType, void* value) {
    DictPair* pair = malloc(sizeof(DictPair));
    memset(pair, 0, sizeof(DictPair));
    pair->key = malloc(strlen(key) + 1);
    memcpy(pair->key, key, strlen(key) + 1);
    _DictionarySetPairValue(pair, valueType, value);
    return pair;
}
// PRIVATE END

Dictionary* DictionaryCreate() {
    Dictionary* dict = malloc(sizeof(Dictionary));
    dict->data = ArrayCreate(uint64_t);
    return dict;
}

void DictionaryFree(Dictionary* dict) {
    for (uint64_t i = 0; i < ArrayGetSize(dict->data); i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        DictionaryFreePair(pair);
    }
    ArrayFree(dict->data);
    free(dict);
}

void DictionaryFreePair(DictPair* pair) {
    if (pair->key) {
        free(pair->key);
    }
    _FreePairValue(pair);
    free(pair);
}

DictPair* DictionaryGet(Dictionary* dict, char* key) {
    for (uint64_t i = 0; i < ArrayGetSize(dict->data); i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        if (pair->key && strcmp(pair->key, key) == 0) {
            return pair;
        }
    }
    return NULL;
}

void DictionarySet(Dictionary* dict, char* key, CUtilsDataType valueType, void* value) {
    DictPair* new = _DictionaryCreatePair(key, valueType, value);
    DictionarySetPair(dict, new);
}

void DictionarySetPair(Dictionary* dict, DictPair* new) {
    for (uint64_t i = 0; i < ArrayGetSize(dict->data); i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        if (pair->key && new->key &&
            strcmp(pair->key, new->key) == 0) {
            DictionaryFreePair(pair);
            dict->data[i] = (uint64_t) new;
            return;
        }
    }
    ArrayPushRV(dict->data, uint64_t, (uint64_t) new);
}

void DictionaryRemove(Dictionary* dict, char* key) {
    for (uint64_t i = 0; i < ArrayGetSize(dict->data); i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        if (pair->key && strcmp(pair->key, key) == 0) {
            DictionaryFreePair(pair);
            free(ArrayPopAt(dict->data, i));
            return;
        }
    }
}
