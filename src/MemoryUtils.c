#include "MemoryUtils.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "containers/HashMap.h"
#include "containers/UniqueArray.h"

#ifdef __cplusplus
extern "C" {
#endif

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

bool MemoryIsNull(const void* buf, size_t size) {
    const char* b = buf;
    for (uint64_t i = 0; i < size; i++) {
        if (*b++ != 0) {
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

#ifdef CUTILS_TESTS_ENABLED
uint64_t c_utils_total_malloc = 0;
uint64_t c_utils_total_free = 0;
uint64_t c_utils_total_realloc = 0;
#endif

void* CUtilsMalloc(size_t size) {
    void* buf = malloc(size);
#ifdef CUTILS_TESTS_ENABLED
    c_utils_total_malloc++;
#endif
    memset(buf, 0, size);
    return buf;
}

void* CUtilsRealloc(void* buf, size_t newSize) {
    void* temp = realloc(buf, newSize);
#ifdef CUTILS_TESTS_ENABLED
    c_utils_total_realloc++;
#endif
    if (temp == NULL) {
        DEBUG_LOG_ERROR("CUtilsRealloc: Memory allocation error! Old Buffer returned.");
        return buf;
    }
    return temp;
}

void* CUtilsFree(void* buf) {
    free(buf);
#ifdef CUTILS_TESTS_ENABLED
    c_utils_total_free++;
#endif
    return NULL;
}

#ifdef __cplusplus
}
#endif
