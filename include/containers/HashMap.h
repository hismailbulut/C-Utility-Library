#pragma once

#include <stdint.h>

#include "containers/HashMap.h"
#include "containers/UniqueArray.h"

typedef struct HashMap {
    UniqueArray* keys;
    void* values;
} HashMap;

// Stride is the size of the each value.
// This hashmap implementation is not actually hashmap, but similar.
// Hash function generates an integer hash value from keys.
// This integer values stored to UniqueArray (sorted set),
// and every value is stored to an array at same index.
// The value is not found directly, the binary search result is obtained.
HashMap* HashMapCreate(size_t stride);

void HashMapFree(HashMap* hmap);

void HashMapSet(HashMap* hmap, const char* key, void* value);
#define HashMapSetRV(hmap, key, type, value) \
    {                                        \
        type temp = value;                   \
        HashMapSet(hmap, key, &temp);        \
    }

void* HashMapGet(HashMap* hmap, const char* key);

// Removes key and its value. Returns false if removing fails.
bool HashMapRemove(HashMap* hmap, const char* key);

bool HashMapContains(HashMap* hmap, const char* key);
