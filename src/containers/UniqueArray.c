#include "containers/UniqueArray.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "containers/Array.h"

// only for _FindValue function
#define SET_INDEX_IF_NOT_NULL(value) \
    {                                \
        if (outIndex != NULL) {      \
            *outIndex = value;       \
        }                            \
    }
// Performs binary search on array. OutIndex always be set if not null.
// If the given value is not founded, outIndex will be the new position
// of the value.
bool _FindValue(UniqueArray* array, void* value, uint64_t* outIndex) {
    uint64_t size = ArrayGetSize(array->data);
    uint64_t stride = ArrayGetStride(array->data);
    uint64_t index = size > 1 ? size / 2 : 0;
    uint64_t step = (index / 2 > 1) ? index / 2 : 1;
    uint64_t prevIndex = index;
    int prevResult = 0;  // prevResult is only set if index stepping one by one
    while (true) {
        char* arrVal = (char*)array->data + index * stride;
        int comp = array->comparator(arrVal, value);
        // DEBUG_LOG_INFO("Value: %f, Index: %I64u", *(float*)value, index);
        if (comp > 0) {  // array value is bigger than the value
            if (index < step) {
                // new smallest value
                // DEBUG_LOG_INFO("New smallest value is %f", *(float*)value);
                ASSERT_BREAK(array->comparator(array->data, value) > 0);
                SET_INDEX_IF_NOT_NULL(0);
                return false;
            } else if (prevResult < 0) {  // previous array value is smaller than value
                // DEBUG_LOG_WARN("comp > 0 && prevResult < 0, value is %f, Index: %I64u, prevIndex: %I64u", *(float*)value, index, prevIndex);
                ASSERT_BREAK(prevIndex + 1 == index);
                SET_INDEX_IF_NOT_NULL(index);
                return false;
            }
            prevIndex = index;
            index -= step;
        } else if (comp < 0) {  // array value is smaller than the value
            if (index + step >= size) {
                ASSERT_BREAK_MSG(step == 1,
                                 "Index: %I64u, PrevIndex: %I64u, Step: %I64u, Size: %I64u",
                                 index, prevIndex, step, size);
                // DEBUG_LOG_INFO("New biggest value is %f", *(float*)value);
                void* arrLast = (char*)array->data + ((size - 1) * stride);
                ASSERT_BREAK_MSG(array->comparator(arrLast, value) < 0,
                                 "Last: %f, value: %f",
                                 *(float*)arrLast, *(float*)value);
                SET_INDEX_IF_NOT_NULL(size);
                return false;
            } else if (prevResult > 0) {  // not contains this value, put it here
                // DEBUG_LOG_WARN("comp < 0 && prevResult > 0, value is %f, Index: %I64u, prevIndex: %I64u", *(float*)value, index, prevIndex);
                ASSERT_BREAK(prevIndex - 1 == index);
                SET_INDEX_IF_NOT_NULL(index + 1);
                return false;
            }
            prevIndex = index;
            index += step;
        } else {  // values are same
            SET_INDEX_IF_NOT_NULL(index);
            return true;
        }
        if (step / 2 > 1) {
            step /= 2;
        } else if (step == 1) {
            prevResult = comp;
        } else {
            step = 1;
        }
    }
}

UniqueArray* UniqueArrayCreate(size_t stride, size_t capacity,
                               int (*comparator)(const void* v1, const void* v2)) {
    UniqueArray* uniqueArray = malloc(sizeof(UniqueArray));
    uniqueArray->data = _ArrayCreate(stride, capacity);
    uniqueArray->comparator = comparator;
    return uniqueArray;
}

void UniqueArrayFree(UniqueArray* uniqueArray) {
    ArrayFree(uniqueArray->data);
    free(uniqueArray);
}

bool UniqueArrayAdd(UniqueArray* uniqueArray, void* value) {
    uint64_t index;
    if (_FindValue(uniqueArray, value, &index)) {
        return false;
    } else {
        uniqueArray = _ArrayPushAt(uniqueArray->data, value, index);
        return true;
    }
}

bool UniqueArrayRemove(UniqueArray* uniqueArray, void* value) {
    uint64_t index;
    if (_FindValue(uniqueArray, value, &index)) {
        ArrayPopAt(uniqueArray->data, index);
        return true;
    } else {
        return false;
    }
}

bool UniqueArrayContains(UniqueArray* uniqueArray, void* value) {
    return _FindValue(uniqueArray, value, NULL);
}

void* UniqueArrayValueAt(UniqueArray* uniqueArray, uint64_t index) {
    return ArrayGetElementPtr(uniqueArray->data, index);
}
