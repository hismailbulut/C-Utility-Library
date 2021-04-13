#include "containers/HashMap.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "Hash.h"
#include "MemoryUtils.h"
#include "containers/Array.h"
#include "containers/UniqueArray.h"

#define HMAP_DEFAULT_CAPACITY 64

#ifdef __cplusplus
extern "C" {
#endif

// PRIVATE BEGIN
typedef struct {
    uint64_t hash;
    void* value;
#if defined(CUTILS_DEBUG_BUILD) && defined(CUTILS_TESTS_ENABLED)
    char* key;
#endif
} _HashNode;

static uint64_t _Hash(const char* key) {
    uint64_t hash = Hash_64(key, strlen(key));
    return hash;
}

#if defined(CUTILS_DEBUG_BUILD) && defined(CUTILS_TESTS_ENABLED)
static void _CheckCollision(HashMap* hmap, const char* key1, const char* key2) {
    if (strcmp(key1, key2) != 0) {
        DEBUG_LOG_ERROR("Collision! (Key: %s, Hash: %I64u), (Key: %s, Hash: %I64u)",
                        key1, _Hash(key1), key2, _Hash(key2));
        DEBUG_LOG_INFO("HashMap size: %I64u", UniqueArrayGetSize(hmap->data));
        HashMapFree(hmap);
        RAISE_SIGSEGV;
    }
}
#endif

static _HashNode* _CreateNode(const char* key, void* value, size_t stride) {
    _HashNode* node = CUtilsMalloc(sizeof(_HashNode));
    node->hash = _Hash(key);
    node->value = CUtilsMalloc(stride);
    memcpy(node->value, value, stride);
#if defined(CUTILS_DEBUG_BUILD) && defined(CUTILS_TESTS_ENABLED)
    node->key = CUtilsMalloc(strlen(key) + 1);
    memcpy(node->key, key, strlen(key) + 1);
#endif
    return node;
}

static inline void _DeleteNode(_HashNode* node) {
    CUtilsFree(node->value);
#if defined(CUTILS_DEBUG_BUILD) && defined(CUTILS_TESTS_ENABLED)
    CUtilsFree(node->key);
#endif
    CUtilsFree(node);
}

static int _HashNodeComparator(const void* k1, const void* k2) {
    const uint64_t v1 = ((_HashNode*)*(uint64_t*)k1)->hash;
    const uint64_t v2 = ((_HashNode*)*(uint64_t*)k2)->hash;
    if (v1 < v2) {
        return -1;
    } else if (v1 > v2) {
        return 1;
    }
    return 0;
}

static int _HashComparator(const void* k1, const void* k2) {
    const uint64_t v1 = ((_HashNode*)*(uint64_t*)k1)->hash;  // k1 is always array value
    const uint64_t v2 = *(uint64_t*)k2;                      // k2 is always value
    if (v1 < v2) {
        return -1;
    } else if (v1 > v2) {
        return 1;
    }
    return 0;
}

static bool _FindHash(HashMap* hmap, const char* key, uint64_t* outIndex) {
    hmap->data->comparator = _HashComparator;
    uint64_t hash = _Hash(key);
    bool contains = UniqueArrayContains(hmap->data, &hash, outIndex);
    hmap->data->comparator = _HashNodeComparator;
    return contains;
}
// PRIVATE END

HashMap* HashMapCreate(size_t stride) {
    HashMap* hmap = CUtilsMalloc(sizeof(HashMap));
    hmap->data = UniqueArrayCreate(sizeof(uint64_t),
                                   HMAP_DEFAULT_CAPACITY,
                                   _HashNodeComparator);
    hmap->stride = stride;
    return hmap;
}

void HashMapFree(HashMap* hmap) {
    for (uint64_t i = 0; i < UniqueArrayGetSize(hmap->data); i++) {
        _HashNode* node = (_HashNode*)*(uint64_t*)UniqueArrayValueAt(hmap->data, i);
        _DeleteNode(node);
    }
    UniqueArrayFree(hmap->data);
    CUtilsFree(hmap);
}

void HashMapSet(HashMap* hmap, const char* key, void* value) {
    uint64_t index;
    if (_FindHash(hmap, key, &index)) {
        // this array has this hash and value, change it
        _HashNode* node = (_HashNode*)*(uint64_t*)UniqueArrayValueAt(hmap->data, index);
#if defined(CUTILS_DEBUG_BUILD) && defined(CUTILS_TESTS_ENABLED)
        _CheckCollision(hmap, node->key, key);
#endif
        memcpy(node->value, value, hmap->stride);
    } else {
        // this array hasn't got this hash and value, create new
        _HashNode* new = _CreateNode(key, value, hmap->stride);
        uint64_t pointer = (uint64_t) new;
        UniqueArrayAdd(hmap->data, &pointer, NULL);
    }
}

void* HashMapGet(HashMap* hmap, const char* key) {
    uint64_t index;
    if (_FindHash(hmap, key, &index)) {
        _HashNode* node = (_HashNode*)*(uint64_t*)UniqueArrayValueAt(hmap->data, index);
#if defined(CUTILS_DEBUG_BUILD) && defined(CUTILS_TESTS_ENABLED)
        _CheckCollision(hmap, node->key, key);
#endif
        ASSERT_BREAK(node->value);
        return node->value;
    }
    return NULL;
}

bool HashMapRemove(HashMap* hmap, const char* key) {
    uint64_t index;
    if (_FindHash(hmap, key, &index)) {
        _HashNode* node = (_HashNode*)*(uint64_t*)UniqueArrayValueAt(hmap->data, index);
#if defined(CUTILS_DEBUG_BUILD) && defined(CUTILS_TESTS_ENABLED)
        _CheckCollision(hmap, node->key, key);
#endif
        UniqueArrayRemoveFrom(hmap->data, index);
        _DeleteNode(node);
        return true;
    }
    return false;
}

bool HashMapContains(HashMap* hmap, const char* key) {
    uint64_t index;
    return _FindHash(hmap, key, &index);
}

#ifdef __cplusplus
}
#endif
