#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "Dictionary.h"
#include "LinkedList.h"
#include "StringUtils.h"

typedef enum DictValueDataTypes {
    TYPE_STRING,        //char*
    TYPE_NUMBER,        //int64_t
    TYPE_FLOAT,         //float
    TYPE_BOOL,          //bool
    TYPE_NUMBER_ARRAY,  //Array.h int64_t*
    TYPE_FLOAT_ARRAY,   //Array.h float*
    TYPE_STRING_LIST,   //LinkedList
    TYPE_OBJECT,        //Dictionary
} DictValueDataTypes;

typedef struct DictValue {
    DictValueDataTypes type;
    char* CString;           //copy
    int64_t* Number;         //copy
    float* Float;            //copy
    bool* Boolean;           //copy
    void* Array;             //copy
    LinkedList* StringList;  // dictionary
    void* Object;            //pointer
} DictValue;

typedef struct DictPair {
    char* key;
    DictValue* value;
    struct DictPair* next;
} DictPair;

typedef struct Dictionary {
    DictPair* first;
} Dictionary;

/* Creates an empty dictionary. */
Dictionary* DictionaryCreate();

/* Frees all copied values and pointed objects. */
void DictionaryFree(Dictionary* dict);

/* Creates a value struct for adding pair. Param value must be a pointer
 * to given dataType. Objects are other dictionaries. All values will be
 * copied to dictionary but not objects. Object values will point to another
 * dictionary. But dictionary owns and frees them. 
 * Array values must be created with ArrayCreate in Array.h and user must be 
 * free them. Dictionary has its own copy.*/
DictValue DictionaryCreateValue(DictValueDataTypes dataType, void* value);

/* Adds pair to dictionary. You can create values with DictionaryCreateValue.
 * Dont create values yourself. */
void DictionarySet(Dictionary* dict, char* key, DictValue* value);

/* A shortcut for setting string value */
#define DictionarySetString(dict, key, value)                  \
    {                                                          \
        char* tt = value;                                      \
        DictValue tv = DictionaryCreateValue(TYPE_STRING, tt); \
        DictionarySet(dict, key, &tv);                         \
    }
/* A shortcut for setting number value */
#define DictionarySetNumber(dict, key, value)                   \
    {                                                           \
        int64_t tt = value;                                     \
        DictValue tv = DictionaryCreateValue(TYPE_NUMBER, &tt); \
        DictionarySet(dict, key, &tv);                          \
    }
/* A shortcut for setting float value */
#define DictionarySetFloat(dict, key, value)                   \
    {                                                          \
        float tt = value;                                      \
        DictValue tv = DictionaryCreateValue(TYPE_FLOAT, &tt); \
        DictionarySet(dict, key, &tv);                         \
    }
/* A shortcut for setting bool value */
#define DictionarySetBool(dict, key, value)                   \
    {                                                         \
        bool tt = value;                                      \
        DictValue tv = DictionaryCreateValue(TYPE_BOOL, &tt); \
        DictionarySet(dict, key, &tv);                        \
    }
/* A shortcut for setting number array value */
#define DictionarySetNumberArray(dict, key, array)                      \
    {                                                                   \
        DictValue tv = DictionaryCreateValue(TYPE_NUMBER_ARRAY, array); \
        DictionarySet(dict, key, &tv);                                  \
    }
/* A shortcut for setting float array value */
#define DictionarySetFloatArray(dict, key, array)                      \
    {                                                                  \
        DictValue tv = DictionaryCreateValue(TYPE_FLOAT_ARRAY, array); \
        DictionarySet(dict, key, &tv);                                 \
    }
/* A shortcut for setting string array value */
#define DictionarySetStringList(dict, key, list)                      \
    {                                                                 \
        DictValue tv = DictionaryCreateValue(TYPE_STRING_LIST, list); \
        DictionarySet(dict, key, &tv);                                \
    }
/* A shortcut for setting object value */
#define DictionarySetObject(dict, key, dictionary)                     \
    {                                                                  \
        DictValue tv = DictionaryCreateValue(TYPE_OBJECT, dictionary); \
        DictionarySet(dict, key, &tv);                                 \
    }

/* Finds the key and returns a pointer to its value. */
DictValue* DictionaryGet(Dictionary* dict, char* key);

/* Removes given key and its value from dictionary and frees them.
 * Dont forget that if value type is object, the pointed dictionary
 * will be freed! */
void DictionaryRemove(Dictionary* dict, char* key);
