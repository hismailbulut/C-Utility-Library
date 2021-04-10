#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "containers/UniqueArray.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UniqueArray {
    /* Data is the array contains the values. You can access elements with
     * casting it to your type. */
    void* data;
    /* Comparator is a function pointer to perform comparison operation between
     * values. If values are same comparator must return 0. 1 if v1 is bigger 
     * than v2. -1 if v1 is smaller than v2. Cast values to your type, dereference
     * and perform simple if-else operation. */
    int (*comparator)(const void* v1, const void* v2);
} UniqueArray;

/* UniqueArray is just an array but doesn't hold same value twice.
 * It always searches itself and if this value is not here appends it.
 * Otherwise it will not append the value. UniqueArray is always sorted.
 * Because of this value indices changes in operations. Search operation
 * is simle binary search. If the array elements changes from outside,
 * the UniqueArray may will not work properly. Only use the UniqueArray
 * with its functions. Stride is the size of value the UniqueArray will hold.
 * And capacity is the start capacity of container. Comparator is a function
 * pointer to perform comparison operation between values. If values are same 
 * comparator must return 0. 1 if v1 is bigger than v2. -1 if v1 is smaller
 * than v2. Cast values to your type, dereference and perform simple if-else
 * operation. */
UniqueArray* UniqueArrayCreate(size_t stride, size_t capacity,
                               int (*comparator)(const void* v1, const void* v2));

void UniqueArrayFree(UniqueArray* uniqueArray);

// Adds the given value to the UniqueArray if not contains it.
// outIndex is the index where the value pushed or where can it pushed.
bool UniqueArrayAdd(UniqueArray* uniqueArray, void* value, uint64_t* outIndex);

// Shortcut for adding rvalues to UniqueArray.
// Note: Return value is forgotten.
#define UniqueArrayAddRV(uniqueArray, type, value) \
    {                                              \
        type temp = value;                         \
        UniqueArrayAdd(uniqueArray, &temp, NULL);  \
    }

// Removes the given value from UniqueArray if contains.
bool UniqueArrayRemove(UniqueArray* uniqueArray, void* value, uint64_t* outIndex);

// Shortcut for removing values from UniqueArray.
// Note: Return value and outIndex are forgotten.
#define UniqueArrayRemoveRV(uniqueArray, type, value) \
    {                                                 \
        type temp = value;                            \
        UniqueArrayRemove(uniqueArray, &temp, NULL);  \
    }

void UniqueArrayRemoveFrom(UniqueArray* uniqueArray, uint64_t index);

/* Returns true if given value is in the UniqueArray. False otherwise. */
bool UniqueArrayContains(UniqueArray* uniqueArray, void* value, uint64_t* outIndex);

/* Returns pointer to specified value at index. Cast to your type. */
void* UniqueArrayValueAt(UniqueArray* uniqueArray, uint64_t index);

uint64_t UniqueArrayGetSize(UniqueArray* uniqueArray);

#ifdef __cplusplus
}
#endif
