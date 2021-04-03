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
static bool _FindValue(UniqueArray* uniqueArray, void* value, uint64_t* outIndex) {
    uint64_t size = ArrayGetSize(uniqueArray->data);
    uint64_t stride = ArrayGetStride(uniqueArray->data);
    uint64_t index = size > 1 ? size / 2 : 0;
    uint64_t step = (index / 2 > 1) ? index / 2 : 1;
    uint64_t prevIndex = index;
    int prevResult = 0;  // prevResult is only set if index stepping one by one
    while (true) {
        void* arrayValue = ArrayGetValue(uniqueArray->data, index);
        if (arrayValue == NULL) {
        }
        int comp = uniqueArray->comparator(arrayValue, value);
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
    UniqueArray* uniqueArray = CUtilsMalloc(sizeof(UniqueArray));
    uniqueArray->data = _ArrayCreate(stride, capacity);
    uniqueArray->comparator = comparator;
    return uniqueArray;
}

void UniqueArrayFree(UniqueArray* uniqueArray) {
    ArrayFree(uniqueArray->data);
    CUtilsFree(uniqueArray);
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

bool UniqueArrayForceAdd(UniqueArray* uniqueArray, void* value,
                         uint64_t* outIndex) {
    uint64_t index;
    bool notFound = true;
    if (_FindValue(uniqueArray, value, &index)) {
        ArraySetValue(uniqueArray->data, value, index);
        notFound = false;
    } else {
        uniqueArray->data = _ArrayPushAt(uniqueArray->data, value, index);
    }
    if (outIndex) {
        *outIndex = index;
    }
    return notFound;
}

bool UniqueArrayRemove(UniqueArray* uniqueArray, void* value, uint64_t* outIndex) {
    uint64_t index;
    if (_FindValue(uniqueArray, value, &index)) {
        CUtilsFree(ArrayPopAt(uniqueArray->data, index));
        if (outIndex) {
            *outIndex = index;
        }
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
    }
    return UniqueArrayGetSize(uniqueArray);
}

void* UniqueArrayValueAt(UniqueArray* uniqueArray, uint64_t index) {
    return ArrayGetValue(uniqueArray->data, index);
}

uint64_t UniqueArrayGetSize(UniqueArray* uniqueArray) {
    return ArrayGetSize(uniqueArray->data);
}
