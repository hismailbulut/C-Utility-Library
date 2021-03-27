#include "containers/UniqueArray.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "containers/Array.h"

// Performs binary search on array. OutIndex always be set if not null.
// If the given value is not founded, outIndex will be the new position
// of the value.
static bool _FindValue(UniqueArray* array, void* value, uint64_t* outIndex) {
    uint64_t size = ArrayGetSize(array->data);
    uint64_t stride = ArrayGetStride(array->data);
    uint64_t index = size > 1 ? size / 2 : 0;
    uint64_t step = (index / 2 > 1) ? index / 2 : 1;
    uint64_t prevIndex = index;
    int prevResult = 0;  // prevResult is only set if index stepping one by one
    while (true) {
        void* arrVal = (char*)array->data + (index * stride);
        int comp = array->comparator(arrVal, value);
        if (comp > 0) {  // array value is bigger than the value
            if (index < step) {
                // new smallest value
                *outIndex = 0;
                return false;
            } else if (prevResult < 0) {  // not contains this value
                // put here
                *outIndex = index;
                return false;
            }
            prevIndex = index;
            index -= step;
        } else if (comp < 0) {  // array value is smaller than the value
            if (index + step >= size) {
                // new biggest value
                *outIndex = size;
                return false;
            } else if (prevResult > 0) {  // not contains this value
                // put here
                *outIndex = index + 1;
                return false;
            }
            prevIndex = index;
            index += step;
        } else {  // values are same
            *outIndex = index;
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
        uniqueArray->data = _ArrayPushAt(uniqueArray->data, value, index);
        return true;
    }
}

bool UniqueArrayRemove(UniqueArray* uniqueArray, void* value) {
    uint64_t index;
    if (_FindValue(uniqueArray, value, &index)) {
        free(ArrayPopAt(uniqueArray->data, index));
        return true;
    } else {
        return false;
    }
}

bool UniqueArrayContains(UniqueArray* uniqueArray, void* value) {
    uint64_t index;
    return _FindValue(uniqueArray, value, &index);
}

uint64_t UniqueArrayIndexOf(UniqueArray* uniqueArray, void* value) {
    uint64_t index;
    if (_FindValue(uniqueArray, value, &index)) {
        return index;
    } else {
        return UniqueArrayGetSize(uniqueArray);
    }
}

void* UniqueArrayValueAt(UniqueArray* uniqueArray, uint64_t index) {
    return ArrayGetElementPtr(uniqueArray->data, index);
}

uint64_t UniqueArrayGetSize(UniqueArray* uniqueArray) {
    return ArrayGetSize(uniqueArray->data);
}
