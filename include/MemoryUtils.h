#pragma once

#include <stdbool.h>
#include <stdint.h>

// COMMONS

typedef enum CUtilsDataType {
    DATA_TYPE_STRING,  //char*
    DATA_TYPE_NUMBER,  //int64_t
    DATA_TYPE_FLOAT,   //float
    DATA_TYPE_BOOL,    //bool
    DATA_TYPE_LIST,    //List.h
    DATA_TYPE_OBJECT,  //Dictionary.h
} CUtilsDataType;

/* Compares two values and returns true if they are equals. False otherwise. */
bool MemoryEquals(const void* buf1, const void* buf2, size_t size);

/* Swaps two given memory blocks. */
void MemorySwap(void* buf1, void* buf2, size_t size);

// MEMORY ALLOCATIONS

typedef struct CUtilsMemory {
    uint64_t* pointers;
} CUtilsMemory;

void* CUtilsMalloc(size_t size);

void* CUtilsRealloc(void* buf, size_t newSize);

void CUtilsFree(void* buf);