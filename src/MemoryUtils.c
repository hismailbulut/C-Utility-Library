#include "MemoryUtils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"

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
// TODO
void* CUtilsMalloc(size_t size) {
    return malloc(size);
}

void* CUtilsRealloc(void* buf, size_t newSize) {
    return realloc(buf, newSize);
}

void CUtilsFree(void* buf) {
    free(buf);
}
