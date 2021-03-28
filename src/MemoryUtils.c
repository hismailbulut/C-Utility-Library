#include "MemoryUtils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "containers/Array.h"
#include "containers/UniqueArray.h"

bool MemoryEquals(const void* buf1, const void* buf2, size_t size) {
    const char* b1 = buf1;
    const char* b2 = buf2;
    for (uint64_t i = 0; i < size; i++) {
        if (*b1++ != *b2++) {
            return false;
        }
    }
    return true;
}

void MemorySwap(void* buf1, void* buf2, size_t size) {
    char* b1 = buf1;
    char* b2 = buf2;
    for (size_t i = 0; i < size; i++) {
        *b1 = *b1 + *b2;
        *b2 = *b1 - *b2;
        *b1 = *b1 - *b2;
        b1++;
        b2++;
    }
}

// MEMORY ALLOCATIONS

#ifdef _DEBUG
#ifndef ENABLE_MEMORY_CHECKS
#define ENABLE_MEMORY_CHECKS
#endif
#endif

#ifdef ENABLE_MEMORY_CHECKS
static int _PointerComparator(const void* buf1, const void* buf2) {
    uint64_t p1 = *(uint64_t*)buf1;
    uint64_t p2 = *(uint64_t*)buf2;
    if (p1 > p2) {
        return 1;
    } else if (p1 < p2) {
        return -1;
    }
    return 0;
}
#endif

MemoryTracker MemoryTrackerInit() {
#ifdef ENABLE_MEMORY_CHECKS
    return UniqueArrayCreate(sizeof(uint64_t), 1, _PointerComparator);
#else
    return NULL;
#endif
}

void* CUtilsMalloc(size_t size, MemoryTracker memTracker) {
    void* buf = malloc(size);
    if (buf == NULL) {
        DEBUG_LOG_FATAL("CUtilsMalloc: Can't allocate memory.");
    }
    memset(buf, 0, size);
#ifdef ENABLE_MEMORY_CHECKS
    uint64_t pointer = (uint64_t)buf;
    UniqueArrayAdd(memTracker, &pointer);
#endif
    return buf;
}

void* CUtilsRealloc(void* buf, size_t newSize, MemoryTracker memTracker) {
#ifdef ENABLE_MEMORY_CHECKS
    uint64_t pointer = (uint64_t)buf;
    if (UniqueArrayContains(memTracker, &pointer)) {
        void* temp = realloc(buf, newSize);
        if (temp == NULL) {
            DEBUG_LOG_ERROR("CUtilsRealloc: Memory allocation error! Old Buffer returned.");
            return buf;
        } else if (temp != buf) {
            UniqueArrayRemove(memTracker, &pointer);
            pointer = (uint64_t)temp;
            UniqueArrayAdd(memTracker, &pointer);
        }
        return temp;
    }
    DEBUG_LOG_WARN("CUtilsRealloc: Try to reallocate not allocated memory!");
    return CUtilsMalloc(newSize, memTracker);
#else
    void* temp = realloc(buf, newSize);
    if (temp == NULL) {
        DEBUG_LOG_ERROR("CUtilsRealloc: Memory allocation error! Old Buffer returned.");
        return buf;
    }
    return temp;
#endif
}

void* _CUtilsFree(void* buf, MemoryTracker memTracker) {
#ifdef ENABLE_MEMORY_CHECKS
    uint64_t pointer = (uint64_t)buf;
    if (UniqueArrayRemove(memTracker, &pointer)) {
        free(buf);
    } else {
        DEBUG_LOG_ERROR("CUtilsFree: Try to free non allocated memory: %p",
                        (void*)pointer);
    }
#else
    free(buf);
#endif
    return NULL;
}

void MemoryTrackerClose(MemoryTracker memTracker) {
#ifdef ENABLE_MEMORY_CHECKS
    for (uint64_t i = 0; i < ArrayGetSize(memTracker->data); i++) {
        uint64_t pointer = *(uint64_t*)UniqueArrayValueAt(memTracker, i);
        void* buf = (void*)pointer;
        DEBUG_LOG_WARN("CUtilsFreeAll: Deleting non freed memory block: %p.", buf);
        free(buf);
    }
    UniqueArrayFree(memTracker);
#endif
}
