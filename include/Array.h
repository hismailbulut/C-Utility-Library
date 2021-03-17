#pragma once

#include <stdint.h>

void* _ArrayCreate(size_t stride, uint64_t capacity);
#define ArrayCreate(type) \
    _ArrayCreate(sizeof(type), 1)

void* _ArrayFree(void* array);
#define ArrayFree(array) \
    array = _ArrayFree(array)

void* _ArrayResize(void* array, uint64_t new_capacity);
#define ArrayPack(array) \
    array = _ArrayResize(array, ArrayGetSize(array))
#define ArrayReserve(array, newCapacity) \
    array = _ArrayResize(array, newCapacity)

void* _ArrayClear(void* array);
#define ArrayClear(array) \
    array = _ArrayClear(array)

void* _ArrayPushAt(void* array, const void* value, uint64_t index);
#define ArrayPushAt(array, value, index) \
    array = _ArrayPushAt(array, &value, index)
#define ArrayPushAtRV(array, type, value, index)   \
    {                                              \
        type temp = value;                         \
        array = _ArrayPushAt(array, &temp, index); \
    }
#define ArrayPush(array, value) \
    array = _ArrayPushAt(array, &value, ArrayGetSize(array))
#define ArrayPushRV(array, type, value)                          \
    {                                                            \
        type temp = value;                                       \
        array = _ArrayPushAt(array, &temp, ArrayGetSize(array)); \
    }

void* ArrayPopAt(void* array, uint64_t index);
#define ArrayPop(array) \
    ArrayPopAt(array, ArrayGetSize(array) - 1)

void* _ArrayInsertAt(void* array, const void* buffer,
                     uint64_t bufferLength, uint64_t index);
#define ArrayInsertAt(array, buffer, bufferLength, index) \
    array = _ArrayInsertAt(array, buffer, bufferLength, index)
#define ArrayInsert(array, buffer, bufferLength)        \
    array = _ArrayInsertAt(array, buffer, bufferLength, \
                           ArrayGetSize(array))

void* _ArrayRemove(void* array, uint64_t startIndex, uint64_t length);
#define ArrayRemove(array, startIndex, length) \
    array = _ArrayRemove(array, startIndex, length)

void* ArrayGetElementPtr(void* array, uint64_t index);

uint64_t ArrayGetCapacity(void* array);

uint64_t ArrayGetSize(void* array);

uint64_t ArrayGetStride(void* array);
