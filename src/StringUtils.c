#include "StringUtils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "containers/Array.h"
#include "containers/List.h"

#define CHAR_STRIDE sizeof(char)

// PRIVATE BEGIN
static void _CheckCapacity(String* string) {
    uint64_t size = ArrayGetSize(string->c_str);
    uint64_t capacity = ArrayGetCapacity(string->c_str);
    if (size >= capacity) {
        ArrayReserve(string->c_str, capacity * 2);
    } else if (size < capacity / 2) {
        ArrayReserve(string->c_str, size + 1);
    }
}

static char* _CreateStrFormat(const char* Format, va_list args) {
    char* str = NULL;
    int n = vsnprintf(NULL, 0, Format, args);
    assert(n > 0);
    str = CUtilsMalloc(n + 1);
    int c = vsnprintf(str, n + 1, Format, args);
    assert(c == n);
    return str;
}

// PRIVATE END

String StringCreate(uint64_t capacity) {
    String string;
    string.c_str = _ArrayCreate(CHAR_STRIDE, capacity);
    string.length = 0;
    return string;
}

String StringCreateCStr(const char* str) {
    uint64_t strLen = strlen(str);
    String string = StringCreate(strLen);
    string.length = strLen;
    ArrayInsert(string.c_str, str, strLen);
    return string;
}

String StringCreateFormat(const char* Format, ...) {
    va_list args;
    va_start(args, Format);
    char* f = _CreateStrFormat(Format, args);
    String s = StringCreateCStr(f);
    CUtilsFree(f);
    va_end(args);
    return s;
}

String StringGetCopy(const String* string) {
    return StringCreateCStr(string->c_str);
}

void StringFree(String* string) {
    ArrayFree(string->c_str);
    string->c_str = NULL;
    string->length = 0;
}

bool StringEquals(const String* string1, const String* string2) {
    return strcmp(string1->c_str, string2->c_str) == 0;
}

bool StringFind(const String* string, const char* what,
                uint64_t start, uint64_t end, uint64_t* outIndex) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    if (end == 0) {
        end = stringLen;
    }
    uint64_t toFindLen = strlen(what);
    bool firstFounded = false;
    uint64_t firstMatchedIndex;
    uint64_t toFindIndex = 0;
    for (uint64_t i = start; i < end; i++) {
        if (what[toFindIndex] == string->c_str[i]) {
            if (!firstFounded) {
                firstMatchedIndex = i;
                firstFounded = true;
            }
            toFindIndex++;
        } else {
            firstFounded = false;
            toFindIndex = 0;
        }
        if (toFindIndex == toFindLen) {
            if (outIndex) {
                *outIndex = firstMatchedIndex;
            }
            return true;
        }
    }
    return false;
}

List* StringSplit(const String* string, const char* delim) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    uint64_t delimLen = strlen(delim);
    bool start = true;
    if (stringLen <= delimLen || delimLen == 0) {
        start = false;
    }
    List* stringList = ListCreate();
    uint64_t currentIndex = 0;
    uint64_t foundedIndex = 0;
    while (start && StringFind(string, delim, currentIndex, 0, &foundedIndex)) {
        if (currentIndex < foundedIndex) {
            String s = StringSubString(string, currentIndex, foundedIndex);
            ListPush(stringList, DATA_TYPE_STRING, s.c_str);
            StringFree(&s);
        }
        currentIndex = foundedIndex + delimLen;
    }
    if (currentIndex < stringLen) {
        String s = StringSubString(string, currentIndex, stringLen);
        ListPush(stringList, DATA_TYPE_STRING, s.c_str);
        StringFree(&s);
    }
    return stringList;
}

List* StringTokenize(const String* string, const char* delimList) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    uint64_t delimListLen = strlen(delimList);
    uint64_t last = 0;
    List* list = ListCreate();
    for (uint64_t i = 0; i < stringLen; i++) {
        for (uint64_t j = 0; j < delimListLen; j++) {
            if (string->c_str[i] == delimList[j]) {
                if (i - last > 0) {
                    String s = StringSubString(string, last, i);
                    ListPush(list, DATA_TYPE_STRING, s.c_str);
                    StringFree(&s);
                }
                last = i + 1;
            }
        }
    }
    if (last < stringLen) {
        String s = StringSubString(string, last, 0);
        ListPush(list, DATA_TYPE_STRING, s.c_str);
        StringFree(&s);
    }
    return list;
}

void StringAppend(String* string, const String* appendString) {
    StringAppendCStr(string, appendString->c_str);
}

void StringAppendCStr(String* string, const char* append) {
    uint64_t appLen = strlen(append);
    ArrayInsert(string->c_str, append, appLen);
    _CheckCapacity(string);
    string->length += appLen;
    ASSERT_BREAK(string->length == ArrayGetSize(string->c_str));
}

void StringAppendChar(String* string, const char c) {
    ArrayPush(string->c_str, c);
    _CheckCapacity(string);
    string->length++;
    ASSERT_BREAK(string->length == ArrayGetSize(string->c_str));
}

void StringAppendFormat(String* string, const char* Format, ...) {
    va_list args;
    va_start(args, Format);
    char* f = _CreateStrFormat(Format, args);
    StringAppendCStr(string, f);
    CUtilsFree(f);
    va_end(args);
}

void StringErase(String* string, uint64_t from, uint64_t to) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    if (to == 0 || to > stringLen || to < from) {
        to = stringLen;
    }
    ArrayRemove(string->c_str, from, to - from);
    _CheckCapacity(string);
    string->length -= (to - from);
    ASSERT_BREAK(string->length == ArrayGetSize(string->c_str));
}

String StringSubString(const String* string, uint64_t from, uint64_t to) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    if (to == 0 || to > stringLen || to < from) {
        to = stringLen;
    }
    String sub = StringCreate(to - from);
    ArrayInsert(sub.c_str, string->c_str + from, to - from);
    sub.length = to - from;
    ASSERT_BREAK(sub.length == ArrayGetSize(sub.c_str));
    return sub;
}

void StringReplace(String* string, const char* from,
                   const char* to, bool replaceAll) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    uint64_t fromLen = strlen(from);
    uint64_t toLen = strlen(to);
    uint64_t index = 0;
    uint64_t foundIndex = 0;
    while (StringFind(string, from, index, 0, &foundIndex)) {
        StringErase(string, foundIndex, foundIndex + fromLen);
        ArrayInsertAt(string->c_str, to, toLen, foundIndex);
        string->length += toLen;
        index = foundIndex + toLen;
        if (!replaceAll) {
            break;
        }
    }
}

void StringTrim(String* string, const char* trimList) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    uint32_t trimListLen = strlen(trimList);
    uint64_t i = 0;
    while (i < stringLen) {
        for (uint64_t j = 0; j < trimListLen; j++) {
            if (string->c_str[i] == trimList[j]) {
                CUtilsFree(ArrayPopAt(string->c_str, i));
                stringLen--;
                i--;
            }
        }
        i++;
    }
    ArrayReserve(string->c_str, ArrayGetSize(string->c_str) + 1);
}

void StringEncode(String* string, const char* password) {
    uint64_t stringLen = strlen(string->c_str);
    uint64_t passLen = strlen(password);
    uint64_t passIdx = 0;
    for (uint64_t i = 0; i < stringLen; i++) {
        string->c_str[i] += password[passIdx++ % passLen];
    }
}

void StringDecode(String* string, const char* password) {
    uint64_t stringLen = strlen(string->c_str);
    uint64_t passLen = strlen(password);
    uint64_t passIdx = 0;
    for (uint64_t i = 0; i < stringLen; i++) {
        string->c_str[i] -= password[passIdx++ % passLen];
    }
}
