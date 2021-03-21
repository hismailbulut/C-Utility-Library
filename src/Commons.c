#include "Commons.h"

bool MemEquals(const void* buf1, const void* buf2, size_t size) {
    const char* _v1 = buf1;
    const char* _v2 = buf2;
    for (uint64_t i = 0; i < size; i++) {
        if (*_v1++ != *_v2++) {
            return false;
        }
    }
    return true;
}
