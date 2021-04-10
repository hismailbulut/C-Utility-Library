#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "containers/List.h"

typedef struct String {
    char* c_str;
    uint64_t length;
} String;

#ifdef __cplusplus
extern "C" {
#endif

String StringCreate(uint64_t len);

String StringCreateCStr(const char* str);

String StringCreateFormat(const char* Format, ...);

String StringGetCopy(const String* string);

void StringFree(String* string);

bool StringEquals(const String* string1, const String* string2);

bool StringFind(const String* string, const char* what,
                uint64_t start, uint64_t end, uint64_t* outIndex);

/* Splits string and returns a list. */
List* StringSplit(const String* string, const char* delim);

/* Splits string and returns a list.
 * Splitting is done for each character in the string */
List* StringTokenize(const String* string, const char* delimList);

void StringAppend(String* string, const String* append);

void StringAppendChar(String* string, const char c);

void StringAppendCStr(String* string, const char* append);

void StringAppendFormat(String* string, const char* Format, ...);

void StringErase(String* string, uint64_t from, uint64_t to);

String StringSubString(const String* string, uint64_t from, uint64_t to);

void StringReplace(String* string, const char* from, const char* to, bool replaceAll);

void StringTrim(String* string, const char* trimList);

void StringEncode(String* string, const char* password);

void StringDecode(String* string, const char* password);

#ifdef __cplusplus
}
#endif
