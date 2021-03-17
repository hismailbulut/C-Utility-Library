#include "Map.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Debug.h"
#include "LinkedList.h"

// PRIVATE BEGIN
typedef struct MapHeader {
    LinkedList* keys;
    LinkedList* values;
} MapHeader;

static inline bool _CompareEquals(const void* v1, const void* v2, size_t size) {
    const char* _v1 = v1;
    const char* _v2 = v2;
    for (uint64_t i = 0; i < size; i++) {
        if (*_v1++ != *_v2++) {
            return false;
        }
    }
    return true;
}

static uint64_t _FindKeyIndex(MapHeader* header, const void* key,
                              size_t keySize) {
    for (uint64_t i = 0; i < LinkedListGetSize(header->keys); i++) {
        size_t listKeySize = LinkedListGetElementSize(header->keys, i);
        if (listKeySize != keySize) {
            continue;
        }
        char* listKey = LinkedListGetElementValue(header->keys, i);
        if (_CompareEquals(key, listKey, keySize)) {
            return i;
        }
    }
    return LinkedListGetSize(header->keys);
}

static uint64_t _FindValueIndex(MapHeader* header, const void* value,
                                size_t valueSize) {
    for (uint64_t i = 0; i < LinkedListGetSize(header->values); i++) {
        size_t listValueSize = LinkedListGetElementSize(header->values, i);
        if (listValueSize != valueSize) {
            continue;
        }
        char* listValue = LinkedListGetElementValue(header->values, i);
        if (_CompareEquals(value, listValue, valueSize)) {
            return i;
        }
    }
    return LinkedListGetSize(header->keys);
}
// PRIVATE END

Map* MapCreate() {
    MapHeader* header = malloc(sizeof(MapHeader));
    header->keys = LinkedListCreate();
    header->values = LinkedListCreate();
    Map* map = malloc(sizeof(Map));
    map->data = header;
    return map;
}

void MapFree(Map* map) {
    MapHeader* header = map->data;
    LinkedListFree(header->keys);
    LinkedListFree(header->values);
    free(header);
    free(map);
}

void MapSet(Map* map, const void* key, size_t keySize,
            const void* value, size_t valueSize) {
    MapHeader* header = map->data;
    uint64_t keyIndex = _FindKeyIndex(header, key, keySize);
    if (keyIndex < LinkedListGetSize(header->keys)) {
        LinkedListSetElementValue(header->values, keyIndex, value, valueSize);
    } else {
        LinkedListPush(header->keys, key, keySize);
        LinkedListPush(header->values, value, valueSize);
    }
}

void MapRemove(Map* map, const void* key, size_t keySize) {
    MapHeader* header = map->data;
    uint64_t keyIndex = _FindKeyIndex(header, key, keySize);
    if (keyIndex < LinkedListGetSize(header->keys)) {
        free(LinkedListPopAt(header->keys, keyIndex, NULL));
        free(LinkedListPopAt(header->values, keyIndex, NULL));
    }
}

void MapChangeKey(Map* map, const void* key, size_t keySize,
                  const void* newKey, size_t newKeySize) {
    MapHeader* header = map->data;
    uint64_t keyIndex = _FindKeyIndex(header, key, keySize);
    if (keyIndex < LinkedListGetSize(header->keys)) {
        LinkedListSetElementValue(header->keys, keyIndex, newKey, newKeySize);
    }
}

void* MapFindValue(Map* map, const void* key, size_t keySize) {
    MapHeader* header = map->data;
    uint64_t keyIndex = _FindKeyIndex(header, key, keySize);
    if (keyIndex < LinkedListGetSize(header->values)) {
        return LinkedListGetElementValue(header->values, keyIndex);
    }
    return NULL;
}

void* MapFindKey(Map* map, const void* value, size_t valueSize) {
    MapHeader* header = map->data;
    uint64_t valueIndex = _FindValueIndex(header, value, valueSize);
    if (valueIndex < LinkedListGetSize(header->keys)) {
        return LinkedListGetElementValue(header->keys, valueIndex);
    }
    return NULL;
}

void* MapGetKeyAt(Map* map, uint64_t index) {
    MapHeader* header = map->data;
    return LinkedListGetElementValue(header->keys, index);
}

uint64_t MapGetSize(Map* map) {
    MapHeader* header = map->data;
    return LinkedListGetSize(header->keys);
}
