#include "containers/HashMap.h"

#include <stdint.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "containers/Array.h"
#include "containers/UniqueArray.h"

#define HMAP_DEFAULT_CAPACITY 64

static inline uint64_t _Hash(const char* key) {
    uint64_t hash = 0;
    for (uint64_t i = 0; i < strlen(key); i++) {
        hash += (i + 1) * key[i];
    }
    return hash;
}

static int _HashComparator(const void* k1, const void* k2) {
    const uint64_t v1 = *(uint64_t*)k1;
    const uint64_t v2 = *(uint64_t*)k2;
    if (v1 < v2) {
        return -1;
    } else if (v1 > v2) {
        return 1;
    }
    return 0;
}

HashMap* HashMapCreate(size_t stride) {
    HashMap* hmap = CUtilsMalloc(sizeof(HashMap));
    hmap->keys = UniqueArrayCreate(sizeof(uint64_t),
                                   HMAP_DEFAULT_CAPACITY, _HashComparator);
    hmap->values = _ArrayCreate(stride, HMAP_DEFAULT_CAPACITY);
    return hmap;
}

void HashMapFree(HashMap* hmap) {
    UniqueArrayFree(hmap->keys);
    ArrayFree(hmap->values);
    CUtilsFree(hmap);
}

void HashMapSet(HashMap* hmap, const char* key, void* value) {
    uint64_t hash = _Hash(key);
    uint64_t index;
    if (UniqueArrayForceAdd(hmap->keys, &hash, &index)) {
        // this array hasn't got this hash and value, push it
        // DEBUG_LOG_INFO("Hash: %I64u \t Key: %s", hash, key);
        hmap->values = _ArrayPushAt(hmap->values, value, index);
    } else {
        // this array has this hash and value, change it
        // DEBUG_LOG_INFO("Hash: %I64u \t Key: %s \t (override)", hash, key);
        ArraySetValue(hmap->values, value, index);
    }
}

void* HashMapGet(HashMap* hmap, const char* key) {
    uint64_t hash = _Hash(key);
    uint64_t index;
    if ((index = UniqueArrayIndexOf(hmap->keys, &hash)) !=
        UniqueArrayGetSize(hmap->keys)) {
        return ArrayGetValue(hmap->values, index);
    }
    return NULL;
}

bool HashMapRemove(HashMap* hmap, const char* key) {
    uint64_t hash = _Hash(key);
    uint64_t index;
    if (UniqueArrayRemove(hmap->keys, &hash, &index)) {
        CUtilsFree(ArrayPopAt(hmap->values, index));
        return true;
    }
    return false;
}

bool HashMapContains(HashMap* hmap, const char* key) {
    uint64_t hash = _Hash(key);
    return UniqueArrayContains(hmap->keys, &hash);
}
