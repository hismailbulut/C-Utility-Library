#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

/* Adds the given value to the UniqueArray if not contains it. */
bool UniqueArrayAdd(UniqueArray* uniqueArray, void* value);

/* Shortcut for adding rvalues to UniqueArray. 
 * Note: Return bool value is forgetting. */
#define UniqueArrayAddRV(uniqueArray, type, value) \
    {                                              \
        type temp = value;                         \
        UniqueArrayAdd(uniqueArray, &temp);        \
    }

/* Removes the given value from UniqueArray if contains. */
bool UniqueArrayRemove(UniqueArray* uniqueArray, void* value);

/* Shortcut for removing values from UniqueArray. 
 * Note: Return bool value is forgetting. */
#define UniqueArrayRemoveRV(uniqueArray, type, value) \
    {                                                 \
        type temp = value;                            \
        UniqueArrayRemove(uniqueArray, &temp);        \
    }

/* Returns true if given value is in the UniqueArray. False otherwise. */
bool UniqueArrayContains(UniqueArray* uniqueArray, void* value);

/* Returns pointer to specified value at index. Cast to your type. */
void* UniqueArrayValueAt(UniqueArray* uniqueArray, uint64_t index);
