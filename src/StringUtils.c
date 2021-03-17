#include "StringUtils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Array.h"
#include "Debug.h"
#include "LinkedList.h"

#define CHAR_STRIDE sizeof(char)

static inline void _CheckCapacity(String* string) {
    uint64_t size = ArrayGetSize(string->c_str);
    uint64_t capacity = ArrayGetCapacity(string->c_str);
    if (size >= capacity) {
        ArrayReserve(string->c_str, capacity * 2);
    }
}

String StringCreate(uint64_t len) {
    String string;
    string.c_str = _ArrayCreate(CHAR_STRIDE, len + 1);
    string.length = 0;
    return string;
}

String StringCreateCStr(const char* str) {
    uint64_t strLen = strlen(str);
    String string = StringCreate(strLen);
    ArrayInsert(string.c_str, str, strLen);
    return string;
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

char StringCharAt(const String* string, uint64_t index) {
    return string->c_str[index];
}

bool StringFind(const String* string, const char* toFind,
                uint64_t start, uint64_t end, uint64_t* outIndex) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    if (end == 0) {
        end = stringLen;
    }
    uint64_t toFindLen = strlen(toFind);
    bool firstFounded = false;
    uint64_t firstMatchedIndex;
    uint64_t toFindIndex = 0;
    for (uint64_t i = start; i < end; i++) {
        if (toFind[toFindIndex] == string->c_str[i]) {
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

LinkedList* StringSplit(const String* string, const char* delim) {
    uint64_t stringLen = ArrayGetSize(string->c_str);
    uint64_t delimLen = strlen(delim);
    ASSERT_BREAK(stringLen > delimLen && delimLen > 0);
    LinkedList* stringList = LinkedListCreate();
    uint64_t currentIndex = 0;
    uint64_t foundedIndex = 0;
    while (StringFind(string, delim, currentIndex, 0, &foundedIndex)) {
        if (currentIndex < foundedIndex) {
            String s = StringSubString(string, currentIndex, foundedIndex);
            LinkedListPush(stringList, s.c_str, s.length + 1);
            StringFree(&s);
        }
        currentIndex = foundedIndex + delimLen;
    }
    if (currentIndex < stringLen) {
        String s = StringSubString(string, currentIndex, stringLen);
        LinkedListPush(stringList, s.c_str, s.length + 1);
        StringFree(&s);
    }
    return stringList;
}

void StringAppend(String* string, const String* appendString) {
    StringAppendCStr(string, appendString->c_str);
}

void StringAppendCStr(String* string, const char* appendString) {
    uint64_t appendStringLen = strlen(appendString);
    ArrayInsert(string->c_str, appendString, appendStringLen);
    _CheckCapacity(string);
    string->length += appendStringLen;
    ASSERT_BREAK(string->length == ArrayGetSize(string->c_str));
}

void StringAppendChar(String* string, const char c) {
    ArrayPush(string->c_str, c);
    _CheckCapacity(string);
    string->length++;
    ASSERT_BREAK(string->length == ArrayGetSize(string->c_str));
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
