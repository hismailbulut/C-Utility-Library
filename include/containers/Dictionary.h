#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "MemoryUtils.h"
#include "StringUtils.h"
#include "containers/Dictionary.h"
#include "containers/LinkedList.h"
#include "containers/UniqueArray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DictPair {
    char* key;
    CUtilsDataType valueType;
    void* value;
} DictPair;

typedef struct Dictionary {
    uint64_t* data;
} Dictionary;

/* Creates an empty dictionary. */
Dictionary* DictionaryCreate();

/* Returns a copy of the given dictionary. */
Dictionary* DictionaryCopy(Dictionary* dict);

/* Frees all copied values and pointed objects. */
void DictionaryFree(Dictionary* dict);

void DictionaryFreePair(Dictionary* dict, DictPair* pair);

/* Adds pair to dictionary. You can create values with DictionaryCreateValue. */
void DictionarySet(Dictionary* dict, char* key, CUtilsDataType valueType, void* value);

void DictionarySetPair(Dictionary* dict, DictPair* pair);

/* A shortcut for setting string value */
#define DictionarySetString(dict, key, value)          \
    {                                                  \
        char* v = value;                               \
        DictionarySet(dict, key, DATA_TYPE_STRING, v); \
    }

/* A shortcut for setting number value */
#define DictionarySetNumber(dict, key, value)           \
    {                                                   \
        int64_t v = value;                              \
        DictionarySet(dict, key, DATA_TYPE_NUMBER, &v); \
    }

/* A shortcut for setting float value */
#define DictionarySetFloat(dict, key, value)           \
    {                                                  \
        float v = value;                               \
        DictionarySet(dict, key, DATA_TYPE_FLOAT, &v); \
    }

/* A shortcut for setting bool value */
#define DictionarySetBool(dict, key, value)           \
    {                                                 \
        bool v = value;                               \
        DictionarySet(dict, key, DATA_TYPE_BOOL, &v); \
    }

/* Finds the key and returns a pointer to its pair. */
DictPair* DictionaryGet(Dictionary* dict, char* key);

/* Removes given key and its value from dictionary and frees them.
 * Dont forget that if value type is object, the pointed dictionary
 * will be freed! */
void DictionaryRemove(Dictionary* dict, char* key);

/* Looks two given dictionaries and compares them. Returns true if
 * two of the dictionaries is the same. False otherwise. */
bool DictionariesAreEquals(Dictionary* dict1, Dictionary* dict2);

#ifdef __cplusplus
}
#endif
