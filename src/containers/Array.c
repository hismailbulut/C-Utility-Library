#include "containers/Array.h"

#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum header_fields {
    CAPACITY = 0,
    SIZE = 1,
    STRIDE = 2,
    TOTAL = 3
} header_fields;

static inline uint64_t *_Header(const void *array) {
    return (uint64_t *)array - TOTAL;
}

static inline uint64_t _FieldGet(const void *array, header_fields field) {
    return _Header(array)[field];
}

static inline void _FieldSet(const void *array, header_fields field, uint64_t val) {
    _Header(array)[field] = val;
}

static inline void _RaiseIndexOutOfBounds(const void *array, uint64_t index) {
    DEBUG_LOG_ERROR("Index out of bounds. Index: %lu, Array Size: %lu.",
                    (unsigned long)index, (unsigned long)ArrayGetSize(array));
    RAISE_SIGSEGV;
}

void *_ArrayCreate(size_t stride, uint64_t capacity) {
    uint64_t create_size = TOTAL * sizeof(uint64_t) + stride * capacity;
    uint64_t *head = CUtilsMalloc(create_size);
    memset(head, 0, create_size);
    head[CAPACITY] = capacity > 0 ? capacity : 1;
    head[SIZE] = 0;
    head[STRIDE] = stride;
    return (void *)(head + TOTAL);
}

void *_ArrayFree(void *array) {
    CUtilsFree(_Header(array));
    return NULL;
}

void *_ArrayResize(void *array, uint64_t new_capacity) {
    uint64_t capacity = ArrayGetCapacity(array);
    uint64_t size = ArrayGetSize(array);
    uint64_t stride = ArrayGetStride(array);
    uint64_t create_size = TOTAL * sizeof(uint64_t) + stride * new_capacity;
    uint64_t *head = CUtilsRealloc(_Header(array), create_size);

    array = (void *)(head + TOTAL);
    _FieldSet(array, CAPACITY, new_capacity);
    uint64_t new_size = (size > new_capacity) ? new_capacity : size;
    _FieldSet(array, SIZE, new_size);

    if (new_size < new_capacity) {
        memset((char *)array + new_size * stride,
               0,
               (new_capacity - new_size) * stride);
    }
    return array;
}

void *_ArrayClear(void *array) {
    if (ArrayGetCapacity(array) > 1) {
        array = _ArrayResize(array, 1);
    }
    _FieldSet(array, CAPACITY, 1);
    _FieldSet(array, SIZE, 0);
    return array;
}

void *_ArrayPushAt(void *array, const void *value, uint64_t index) {
    uint64_t capacity = ArrayGetCapacity(array);
    uint64_t size = ArrayGetSize(array);
    uint64_t stride = ArrayGetStride(array);
    if (capacity <= size) {
        array = _ArrayResize(array, capacity * 2);
    }
    if (index > size) {
        index = size;
    } else if (index < size) {
        memmove((char *)array + (index + 1) * stride,
                (char *)array + index * stride,
                (size - index) * stride);
    }
    void *dest = (char *)array + index * stride;
    memcpy(dest, value, stride);
    _FieldSet(array, SIZE, size + 1);
    return array;
}

void *ArrayPopAt(void *array, uint64_t index) {
    uint64_t capacity = ArrayGetCapacity(array);
    uint64_t size = ArrayGetSize(array);
    uint64_t stride = ArrayGetStride(array);
    if (size <= 0) {
        _RaiseIndexOutOfBounds(array, index);
        return NULL;
    }
    if (index >= size) {
        index = size - 1;
    }
    void *value = CUtilsMalloc(stride);
    void *src = (char *)array + index * stride;
    memcpy(value, src, stride);
    _FieldSet(array, SIZE, size - 1);
    if (index < size - 1) {
        memmove(src,
                (char *)array + (index + 1) * stride,
                (size - 1 - index) * stride);
    }
    return value;
}

void *_ArrayInsertAt(void *array, const void *buffer,
                     uint64_t bufferLength, uint64_t index) {
    uint64_t capacity = ArrayGetCapacity(array);
    uint64_t size = ArrayGetSize(array);
    uint64_t stride = ArrayGetStride(array);
    while (capacity <= size + bufferLength) {
        array = _ArrayResize(array, capacity * 2);
        capacity = ArrayGetCapacity(array);
    }
    if (index > size) {
        index = size;
    } else if (index < size) {
        memmove((char *)array + (index + bufferLength) * stride,
                (char *)array + index * stride,
                (size - index) * stride);
    }
    void *dest = (char *)array + index * stride;
    memcpy(dest, buffer, bufferLength * stride);
    _FieldSet(array, SIZE, size + bufferLength);
    return array;
}

void *_ArrayRemove(void *array, uint64_t startIndex, uint64_t length) {
    uint64_t capacity = ArrayGetCapacity(array);
    uint64_t size = ArrayGetSize(array);
    uint64_t stride = ArrayGetStride(array);
    if (startIndex >= size) {
        _RaiseIndexOutOfBounds(array, startIndex);
        return array;
    }
    if (startIndex + length > size) {
        length = size - startIndex;
    }
    if (startIndex + length < size) {
        memmove((char *)array + startIndex * stride,
                (char *)array + (startIndex + length) * stride,
                (size - (startIndex + length)) * stride);
    }
    _FieldSet(array, SIZE, size - length);
    ArrayPack(array);
    return array;
}

void ArraySetValue(void *array, void *value, uint64_t index) {
    if (index >= ArrayGetSize(array)) {
        _RaiseIndexOutOfBounds(array, index);
        return;
    }
    size_t stride = ArrayGetStride(array);
    void *dest = (char *)array + index * stride;
    memcpy(dest, value, stride);
}

void *ArrayGetValue(const void *array, uint64_t index) {
    uint64_t size = ArrayGetSize(array);
    if (index >= size && size > 0) {
        _RaiseIndexOutOfBounds(array, index);
        return NULL;
    }
    return (char *)array + index * ArrayGetStride(array);
}

uint64_t ArrayGetCapacity(const void *array) {
    return _FieldGet(array, CAPACITY);
}

uint64_t ArrayGetSize(const void *array) {
    return _FieldGet(array, SIZE);
}

uint64_t ArrayGetStride(const void *array) {
    return _FieldGet(array, STRIDE);
}

#ifdef __cplusplus
}
#endif
