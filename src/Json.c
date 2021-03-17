#include "Json.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Array.h"
#include "Debug.h"
#include "Dictionary.h"
#include "LinkedList.h"
#include "StringUtils.h"

#define TAB "    "

// PRIVATE BEGIN

static inline char* _NumberToString(int64_t numberValue) {
    const int n = snprintf(NULL, 0, "%I64i", numberValue);
    assert(n > 0);
    char* str = malloc(n + 1);
    int c = snprintf(str, n + 1, "%I64i", numberValue);
    assert(c == n && str[c] == 0);
    return str;
}

static inline char* _FloatToString(float floatValue) {
    const int n = snprintf(NULL, 0, "%f", floatValue);
    assert(n > 0);
    char* str = malloc(n + 1);
    int c = snprintf(str, n + 1, "%f", floatValue);
    assert(c == n && str[c] == 0);
    return str;
}

static String _ArrayToString(void* arrayValue, DictValueDataTypes type) {
    String s = StringCreate(1);
    StringAppendCStr(&s, "[ ");
    for (uint64_t i = 0; i < ArrayGetSize(arrayValue); i++) {
        if (type == TYPE_NUMBER_ARRAY) {
            char* numberString = _NumberToString(((int64_t*)arrayValue)[i]);
            StringAppendCStr(&s, numberString);
            free(numberString);
        } else if (type == TYPE_FLOAT_ARRAY) {
            char* floatString = _FloatToString(((float*)arrayValue)[i]);
            StringAppendCStr(&s, floatString);
            free(floatString);
        }
        if (i < ArrayGetSize(arrayValue) - 1) {
            StringAppendCStr(&s, ", ");
        }
    }
    StringAppendCStr(&s, " ]");
    return s;
}

static String _LinkedListToString(LinkedList* list) {
    String s = StringCreate(1);
    StringAppendCStr(&s, "[ ");
    uint64_t listSize = LinkedListGetSize(list);
    for (uint64_t i = 0; i < listSize; i++) {
        StringAppendChar(&s, '\"');
        char* str = LinkedListGetElementValue(list, i);
        size_t strLength = LinkedListGetElementSize(list, i);
        if (str[strLength - 1] == '\0') {
            StringAppendCStr(&s, str);
        } else {
            for (uint64_t j = 0; j < strLength; j++) {
                StringAppendChar(&s, str[j]);
            }
        }
        StringAppendChar(&s, '\"');
        if (i < listSize - 1) {
            StringAppendCStr(&s, ", ");
        }
    }
    StringAppendCStr(&s, " ]");
    return s;
}

String _JsonCreate(Dictionary* dict, uint32_t indentLevel);

static String _GetValueString(DictValue* value, uint32_t indentLevel) {
    String s = StringCreate(1);
    if (value->type == TYPE_STRING && value->CString) {
        StringAppendChar(&s, '\"');
        StringAppendCStr(&s, value->CString);
        StringAppendChar(&s, '\"');
        return s;
    } else if (value->type == TYPE_NUMBER && value->Number) {
        char* numberString = _NumberToString(*value->Number);
        StringAppendCStr(&s, numberString);
        free(numberString);
    } else if (value->type == TYPE_FLOAT && value->Float) {
        char* floatString = _FloatToString(*value->Float);
        StringAppendCStr(&s, floatString);
        free(floatString);
    } else if (value->type == TYPE_BOOL && value->Boolean) {
        if (value->Boolean) {
            StringAppendCStr(&s, "true");
        } else {
            StringAppendCStr(&s, "false");
        }
    } else if ((value->type == TYPE_NUMBER_ARRAY || value->type == TYPE_FLOAT_ARRAY) && value->Array) {
        StringFree(&s);
        return _ArrayToString(value->Array, value->type);
    } else if (value->type == TYPE_STRING_LIST && value->StringList) {
        StringFree(&s);
        return _LinkedListToString(value->StringList);
    } else if (value->type == TYPE_OBJECT && value->Object) {
        StringFree(&s);
        return _JsonCreate(value->Object, indentLevel + 1);
    } else {
        StringAppendCStr(&s, "null");
    }
    return s;
}

String _JsonCreate(Dictionary* dict, uint32_t indentLevel) {
    String json = StringCreate(3);
    StringAppendCStr(&json, "{\n");
    DictPair* pair = dict->first;
    while (pair) {
        for (uint32_t i = 0; i < indentLevel; i++) {
            StringAppendCStr(&json, TAB);
        }
        StringAppendChar(&json, '\"');
        StringAppendCStr(&json, pair->key);
        StringAppendChar(&json, '\"');
        StringAppendCStr(&json, ": ");
        if (pair->value) {
            String valueString = _GetValueString(pair->value, indentLevel);
            StringAppend(&json, &valueString);
            StringFree(&valueString);
        } else {
            StringAppendCStr(&json, "null");
        }
        if (pair->next) {
            StringAppendChar(&json, ',');
        }
        StringAppendChar(&json, '\n');
        pair = pair->next;
    }
    for (uint32_t i = 0; i < indentLevel - 1; i++) {
        StringAppendCStr(&json, TAB);
    }
    StringAppendChar(&json, '}');
    return json;
}
// PRIVATE END

String JsonCreate(Dictionary* dict) {
    return _JsonCreate(dict, 1);
}

// JSON READER

typedef enum {
    TOKEN_NULL,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_KEY,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_FLOAT,
    TOKEN_NUMBER_ARRAY,
    TOKEN_FLOAT_ARRAY,
    TOKEN_STRING_LIST,
    TOKEN_OBJECT
} _TokenTypes;

typedef struct {
    _TokenTypes tokenType;
    String token;
} _TokenInformation;

typedef struct {
    String jsonText;
    uint64_t index;
    bool keyReaded;
    uint32_t errorCount;
    uint32_t level;
    uint64_t lineNumber;
    uint64_t columnNumber;
} _JsonReadStatus;

static inline void _RaiseJsonReadError(const char* message, _JsonReadStatus* status) {
    DEBUG_LOG_ERROR("Json Reader : %s At index: %I64u, Line: %I64u, Column: %I64u",
                    message, status->index, status->lineNumber, status->columnNumber);
    status->errorCount++;
}

static uint64_t _FindTokenEnd(_JsonReadStatus* status, uint64_t startFrom, char endChar) {
    for (uint64_t i = startFrom; i < status->jsonText.length; i++) {
        if (status->jsonText.c_str[i] == endChar) {
            return i;
        }
    }
    return 0;
}

static int _IsNumber(char c) {
    if (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
        c == '5' || c == '6' || c == '7' || c == '8' || c == '9') {
        return 1;
    } else if (c == '.') {  // FLOAT
        return 2;
    } else if (c == '-') {  // NEGATIVE
        return -1;
    }
    return 0;
}

static _TokenInformation _GetNextToken(_JsonReadStatus* status) {
    _TokenInformation tokenInfo;
    tokenInfo.token.c_str = NULL;
    tokenInfo.token.length = 0;
    uint64_t tokenEnd = 0;
    for (uint64_t i = status->index; i < status->jsonText.length; i++) {
        char c = status->jsonText.c_str[i];
        if (c == ' ' || c == '\t') {  // WHITESPACE
            status->columnNumber++;
            continue;
        } else if (c == '\n') {  // NEWLINE
            status->lineNumber++;
            status->columnNumber = 0;
            continue;
        } else if (c == '{') {  // OBJECT
            if (status->level == 0) {
                status->level++;
                continue;
            }
            status->level++;
            tokenInfo.tokenType = TOKEN_OBJECT;
            if ((tokenEnd = _FindTokenEnd(status, i, '}'))) {
                tokenInfo.token = StringSubString(&status->jsonText, i, tokenEnd + 1);
                status->index = tokenEnd + 1;
                return tokenInfo;
            } else {
                _RaiseJsonReadError("Can't find end of the object.", status);
                continue;
            }
        } else if (c == '"') {
            if (status->keyReaded) {  // STRING
                tokenInfo.tokenType = TOKEN_STRING;
            } else {  // KEY
                tokenInfo.tokenType = TOKEN_KEY;
            }
            if ((tokenEnd = _FindTokenEnd(status, i + 1, '"'))) {
                tokenInfo.token = StringSubString(&status->jsonText, i + 1, tokenEnd);
                status->index = tokenEnd + 1;
                /* if (tokenInfo.tokenType == TOKEN_KEY) { */
                /*     ASSERT_BREAK_MSG(status->jsonText.c_str[status->index] == ':', */
                /*                      "index: %I64u, char: %c, line: %I64u, col: %I64u", */
                /*                      status->index, status->jsonText.c_str[status->index], */
                /*                      status->lineNumber, status->columnNumber); */
                /* } */
                return tokenInfo;
            } else {
                _RaiseJsonReadError("Can't find end of the string.", status);
                continue;
            }
        } else if (c == ':') {  // END OF THE KEY
            status->keyReaded = true;
            status->columnNumber++;
            continue;
        } else if (c == ',') {  // END OF THE VALUE
            status->keyReaded = false;
            status->columnNumber++;
            continue;
        } else if (c == 'n') {  // NULL
            tokenInfo.tokenType = TOKEN_NULL;
            status->index += 4;  // hardcoded
            status->columnNumber += 4;
            return tokenInfo;
        } else if (c == 't') {  // TRUE
            tokenInfo.tokenType = TOKEN_TRUE;
            status->index += 4;  // hardcoded
            status->columnNumber += 4;
            return tokenInfo;
        } else if (c == 'f') {  // FALSE
            tokenInfo.tokenType = TOKEN_FALSE;
            status->index += 5;  // hardcoded
            status->columnNumber += 5;
            return tokenInfo;
        } else if (c == '[') {  // ARRAY OR STRING LIST
            if ((tokenEnd = _FindTokenEnd(status, i + 1, ']'))) {
                tokenInfo.token = StringSubString(&status->jsonText, i + 1, tokenEnd);
                status->index = tokenEnd + 1;
                tokenInfo.tokenType = TOKEN_NUMBER_ARRAY;
                for (uint64_t j = 0; j < tokenInfo.token.length; j++) {
                    if (tokenInfo.token.c_str[j] == '"') {
                        tokenInfo.tokenType = TOKEN_STRING_LIST;
                        break;
                    } else if (tokenInfo.token.c_str[j] == '.') {
                        tokenInfo.tokenType = TOKEN_FLOAT_ARRAY;
                        break;
                    }
                }
                return tokenInfo;
            } else {
                _RaiseJsonReadError("Can't find end of the array.", status);
                continue;
            }
        } else if (_IsNumber(c)) {
            int end = i;
            int number;
            tokenInfo.tokenType = TOKEN_NUMBER;
            while ((number = _IsNumber(status->jsonText.c_str[end]))) {
                if (number == 2) {  // FLOAT
                    tokenInfo.tokenType = TOKEN_FLOAT;
                }
                end++;
            }
            tokenInfo.token = StringSubString(&status->jsonText, i, end);
            status->index = end;
            return tokenInfo;
        }
    }
    return tokenInfo;
}

static LinkedList* _CreateStringListFromToken(_TokenInformation info) {
    LinkedList* list = LinkedListCreate();
    uint64_t counter = 0;
    uint64_t beginIndex;
    for (uint64_t i = 0; i < info.token.length; i++) {
        if (info.token.c_str[i] == '"') {
            if (counter % 2 == 1) {
                String tok = StringSubString(&info.token, beginIndex, i);
                LinkedListPush(list, tok.c_str, tok.length + 1);
                StringFree(&tok);
            } else {
                beginIndex = i + 1;
            }
            counter++;
        }
    }
    return list;
}

static int64_t* _CreateNumberArrayFromToken(_TokenInformation info) {
    int64_t* array = ArrayCreate(int64_t);
    for (uint64_t i = 0; i < info.token.length; i++) {
        if (_IsNumber(info.token.c_str[i])) {
            int end = i;
            while (_IsNumber(info.token.c_str[end])) {
                end++;
            }
            String tok = StringSubString(&info.token, i, end + 1);
            int64_t num = atoll(tok.c_str);
            ArrayPush(array, num);
            StringFree(&tok);
            i = end + 1;
        }
    }
    return array;
}

static float* _CreateFloatArrayFromToken(_TokenInformation info) {
    float* array = ArrayCreate(float);
    for (uint64_t i = 0; i < info.token.length; i++) {
        if (_IsNumber(info.token.c_str[i])) {
            int end = i;
            while (_IsNumber(info.token.c_str[end])) {
                end++;
            }
            String tok = StringSubString(&info.token, i, end + 1);
            float num = atof(tok.c_str);
            ArrayPush(array, num);
            StringFree(&tok);
            i = end + 1;
        }
    }
    return array;
}

static DictValue _GetNextValue(_JsonReadStatus* status) {
    DictValue value;
    memset(&value, 0, sizeof(DictValue));
    _TokenInformation tokenInfo = _GetNextToken(status);
    switch (tokenInfo.tokenType) {
        case TOKEN_NULL:
            return value;
        case TOKEN_TRUE:
            value.type = TYPE_BOOL;
            value.Boolean = malloc(sizeof(bool));
            *value.Boolean = true;
            return value;
        case TOKEN_FALSE:
            value.type = TYPE_BOOL;
            value.Boolean = malloc(sizeof(bool));
            *value.Boolean = false;
            return value;
        case TOKEN_KEY:
            _RaiseJsonReadError("No colon after the key.", status);
            StringFree(&tokenInfo.token);
            return value;
        case TOKEN_STRING:
            value.type = TYPE_STRING;
            value.CString = malloc(tokenInfo.token.length + 1);
            memcpy(value.CString, tokenInfo.token.c_str, tokenInfo.token.length + 1);
            StringFree(&tokenInfo.token);
            return value;
        case TOKEN_NUMBER:
            value.type = TYPE_NUMBER;
            int64_t num = atoll(tokenInfo.token.c_str);
            value.Number = malloc(sizeof(int64_t));
            *value.Number = num;
            StringFree(&tokenInfo.token);
            return value;
        case TOKEN_FLOAT:
            value.type = TYPE_FLOAT;
            float f = atof(tokenInfo.token.c_str);
            value.Float = malloc(sizeof(float));
            *value.Float = f;
            StringFree(&tokenInfo.token);
            return value;
        case TOKEN_OBJECT:
            value.type = TYPE_OBJECT;
            value.Object = JsonParse(tokenInfo.token);
            StringFree(&tokenInfo.token);
            return value;
        case TOKEN_STRING_LIST:
            value.type = TYPE_STRING_LIST;
            value.StringList = _CreateStringListFromToken(tokenInfo);
            StringFree(&tokenInfo.token);
            return value;
        case TOKEN_NUMBER_ARRAY:
            value.type = TYPE_NUMBER_ARRAY;
            value.Array = _CreateNumberArrayFromToken(tokenInfo);
            StringFree(&tokenInfo.token);
            return value;
        case TOKEN_FLOAT_ARRAY:
            value.type = TYPE_FLOAT_ARRAY;
            value.Array = _CreateFloatArrayFromToken(tokenInfo);
            StringFree(&tokenInfo.token);
            return value;
    }
}

static String _GetNextKey(_JsonReadStatus* status) {
    _TokenInformation info = _GetNextToken(status);
    if (status->keyReaded || info.tokenType != TOKEN_KEY) {
        /* _RaiseJsonReadError("Comma not founded after value.", status); */
        /* status->keyReaded = false; */
    }
    return info.token;
}

Dictionary* JsonParse(String jsonString) {
    Dictionary* dict = DictionaryCreate();
    jsonString.length = strlen(jsonString.c_str);
    _JsonReadStatus status;
    memset(&status, 0, sizeof(_JsonReadStatus));
    status.jsonText = jsonString;
    while (true) {
        String key = _GetNextKey(&status);
        if (key.c_str) {
            DictValue value = _GetNextValue(&status);
            DictionarySet(dict, key.c_str, &value);
            StringFree(&key);
        } else {
            break;
        }
    }
    if (status.errorCount) {
        DEBUG_LOG_ERROR("Json parsing failed. %u total errors.",
                        status.errorCount);
    }
    return dict;
}
