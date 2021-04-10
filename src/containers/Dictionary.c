#include "containers/Dictionary.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "containers/Array.h"
#include "containers/List.h"

#ifdef __cplusplus
extern "C" {
#endif

// PRIVATE BEGIN
static void _FreePairValue(Dictionary* dict, DictPair* pair) {
    if (pair->value) {
        if (pair->valueType == DATA_TYPE_LIST) {
            ListFree(pair->value);
        } else if (pair->valueType == DATA_TYPE_OBJECT) {
            DictionaryFree(pair->value);
        } else {
            CUtilsFree(pair->value);
        }
    }
    pair->value = NULL;
    pair->valueType = -1;
}

void _DictionarySetPairValue(Dictionary* dict, DictPair* pair,
                             CUtilsDataType valueType, void* value) {
    pair->valueType = valueType;
    switch (valueType) {
        case DATA_TYPE_STRING:
            pair->value = CUtilsMalloc(strlen(value) + 1);
            memcpy(pair->value, value, strlen(value) + 1);
            break;
        case DATA_TYPE_NUMBER:
            pair->value = CUtilsMalloc(sizeof(int64_t));
            memcpy(pair->value, value, sizeof(int64_t));
            break;
        case DATA_TYPE_FLOAT:
            pair->value = CUtilsMalloc(sizeof(float));
            memcpy(pair->value, value, sizeof(float));
            break;
        case DATA_TYPE_BOOL:
            pair->value = CUtilsMalloc(sizeof(bool));
            memcpy(pair->value, value, sizeof(bool));
            break;
        case DATA_TYPE_LIST:
            pair->value = ListCopy(value);
            break;
        case DATA_TYPE_OBJECT:
            pair->value = DictionaryCopy(value);
            break;
        default:
            pair->value = NULL;
            break;
    }
}

DictPair* _DictionaryCreatePair(Dictionary* dict, char* key,
                                CUtilsDataType valueType, void* value) {
    DictPair* pair = CUtilsMalloc(sizeof(DictPair));
    pair->key = CUtilsMalloc(strlen(key) + 1);
    memcpy(pair->key, key, strlen(key) + 1);
    _DictionarySetPairValue(dict, pair, valueType, value);
    return pair;
}
// PRIVATE END

Dictionary* DictionaryCreate() {
    Dictionary* dict = CUtilsMalloc(sizeof(Dictionary));
    dict->data = ArrayCreate(uint64_t);
    return dict;
}

Dictionary* DictionaryCopy(Dictionary* dict) {
    Dictionary* cpy = DictionaryCreate();
    for (uint64_t i = 0; i < ArrayGetSize(dict->data); i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        DictionarySet(cpy, pair->key, pair->valueType, pair->value);
    }
    return cpy;
}

void DictionaryFree(Dictionary* dict) {
    for (uint64_t i = 0; i < ArrayGetSize(dict->data); i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        DictionaryFreePair(dict, pair);
    }
    ArrayFree(dict->data);
    CUtilsFree(dict);
}

void DictionaryFreePair(Dictionary* dict, DictPair* pair) {
    if (pair->key) {
        CUtilsFree(pair->key);
    }
    _FreePairValue(dict, pair);
    CUtilsFree(pair);
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
    DictPair* new = _DictionaryCreatePair(dict, key, valueType, value);
    DictionarySetPair(dict, new);
}

void DictionarySetPair(Dictionary* dict, DictPair* new) {
    for (uint64_t i = 0; i < ArrayGetSize(dict->data); i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        if (pair->key && new->key &&
            strcmp(pair->key, new->key) == 0) {
            DictionaryFreePair(dict, pair);
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
            DictionaryFreePair(dict, pair);
            CUtilsFree(ArrayPopAt(dict->data, i));
            return;
        }
    }
}

#ifdef __cplusplus
}
#endif
