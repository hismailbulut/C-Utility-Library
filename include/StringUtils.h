#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include "LinkedList.h"

typedef struct String {
    char* c_str;
    uint64_t length;
} String;

String StringCreate(uint64_t len);

String StringCreateCStr(const char* str);

String StringGetCopy(const String* string);

void StringFree(String* string);

bool StringEquals(const String* string1, const String* string2);

char StringCharAt(const String* string, uint64_t index);

bool StringFind(const String* string, const char* searchingString,
                uint64_t startIndex, uint64_t endIndex, uint64_t* outFirstIndex);

LinkedList* StringSplit(const String* string, const char* delim);

void StringAppend(String* string, const String* appendString);

void StringAppendChar(String* string, const char c);

void StringAppendCStr(String* string, const char* appendString);

void StringErase(String* string, uint64_t from, uint64_t to);

String StringSubString(const String* string, uint64_t from, uint64_t to);
