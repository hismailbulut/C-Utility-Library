#include "Json.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "StringUtils.h"
#include "containers/Array.h"
#include "containers/Dictionary.h"
#include "containers/LinkedList.h"
#include "containers/List.h"

// PRIVATE BEGIN
#define APPEND_TABS(str, count)                \
    {                                          \
        for (uint32_t j = 0; j < count; j++) { \
            StringAppendCStr(&str, "    ");    \
        }                                      \
    }

String _JsonCreate(Dictionary* dict, uint32_t indentLevel);

static String _ListToString(List* list, uint32_t indentLevel) {
    uint64_t listSize = ListGetSize(list);
    String s = StringCreate(1);
    StringAppendChar(&s, '[');
    for (uint64_t i = 0; i < listSize; i++) {
        ListNode* node = ListGetValue(list, i);
        StringAppendChar(&s, '\n');
        APPEND_TABS(s, indentLevel);
        if (node == NULL) {
            StringAppendCStr(&s, "null");
        } else if (node->value == NULL) {
            StringAppendCStr(&s, "null");
        } else if (node->dataType == DATA_TYPE_STRING) {
            StringAppendChar(&s, '"');
            StringAppendCStr(&s, node->value);
            StringAppendChar(&s, '"');
        } else if (node->dataType == DATA_TYPE_NUMBER) {
            StringAppendFormat(&s, "%I64i", *(int64_t*)node->value);
        } else if (node->dataType == DATA_TYPE_FLOAT) {
            StringAppendFormat(&s, "%f", *(float*)node->value);
        } else if (node->dataType == DATA_TYPE_BOOL) {
            if (*(bool*)node->value) {
                StringAppendCStr(&s, "true");
            } else {
                StringAppendCStr(&s, "false");
            }
        } else if (node->dataType == DATA_TYPE_LIST) {
            String listString = _ListToString(node->value, indentLevel + 1);
            StringAppend(&s, &listString);
            StringFree(&listString);
        } else if (node->dataType == DATA_TYPE_OBJECT) {
            String objectString = _JsonCreate(node->value, indentLevel + 1);
            StringAppend(&s, &objectString);
            StringFree(&objectString);
        } else {
            StringAppendCStr(&s, "null");
        }
        if (i < listSize - 1) {
            StringAppendChar(&s, ',');
        }
    }
    StringAppendChar(&s, '\n');
    APPEND_TABS(s, indentLevel - 1);
    StringAppendChar(&s, ']');
    return s;
}

static String _GetValueString(DictPair* pair, uint32_t indentLevel) {
    String s = StringCreate(1);
    switch (pair->valueType) {
        case DATA_TYPE_STRING:
            StringAppendChar(&s, '"');
            StringAppendCStr(&s, pair->value);
            StringAppendChar(&s, '"');
            break;
        case DATA_TYPE_NUMBER:
            StringAppendFormat(&s, "%I64i", *(int64_t*)pair->value);
            break;
        case DATA_TYPE_FLOAT:
            StringAppendFormat(&s, "%f", *(float*)pair->value);
            break;
        case DATA_TYPE_BOOL:
            if (*(bool*)pair->value) {
                StringAppendCStr(&s, "true");
            } else {
                StringAppendCStr(&s, "false");
            }
            break;
        case DATA_TYPE_LIST:
            StringFree(&s);
            return _ListToString(pair->value, indentLevel + 1);
        case DATA_TYPE_OBJECT:
            StringFree(&s);
            return _JsonCreate(pair->value, indentLevel + 1);
        default:
            StringAppendCStr(&s, "null");
            break;
    }
    return s;
}

String _JsonCreate(Dictionary* dict, uint32_t indentLevel) {
    String json = StringCreate(3);
    StringAppendCStr(&json, "{\n");
    uint64_t dictSize = ArrayGetSize(dict->data);
    for (uint64_t i = 0; i < dictSize; i++) {
        DictPair* pair = (DictPair*)dict->data[i];
        APPEND_TABS(json, indentLevel);
        StringAppendChar(&json, '\"');
        StringAppendCStr(&json, pair->key);
        StringAppendChar(&json, '\"');
        StringAppendCStr(&json, ": ");
        if (pair->value) {
            String valueString = _GetValueString(pair, indentLevel);
            StringAppend(&json, &valueString);
            StringFree(&valueString);
        } else {
            StringAppendCStr(&json, "null");
        }
        if (i < dictSize - 1) {
            StringAppendChar(&json, ',');
        }
        StringAppendChar(&json, '\n');
    }
    APPEND_TABS(json, indentLevel - 1);
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
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_FLOAT,
    TOKEN_LIST,
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
} _JsonReadStatus;

static inline void _RaiseJsonReadError(const char* message, _JsonReadStatus* status) {
    DEBUG_LOG_ERROR("Json Reader : %s At index: %I64u",
                    message, status->index);
    status->errorCount++;
}

static uint64_t _FindStringEnd(_JsonReadStatus* status, uint64_t start) {
    char* str = status->jsonText.c_str;
    for (uint64_t i = start; i < status->jsonText.length; i++) {
        if (str[i] == '"') {
            if (i > 0 && str[i - 1] == '\\') {
                continue;
            }
            return i;
        }
    }
    return 0;
}

static uint64_t _FindEnd(_JsonReadStatus* status, uint64_t start, char begin, char end) {
    char* str = status->jsonText.c_str;
    uint32_t in = 0;
    for (uint64_t i = start; i < status->jsonText.length; i++) {
        if (str[i] == begin) {
            in++;
        }
        if (str[i] == end) {
            if (in == 0) {
                return i;
            }
            in--;
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
    tokenInfo.tokenType = -1;
    uint64_t tokenEnd = 0;
    for (uint64_t i = status->index; i < status->jsonText.length; i++) {
        char c = status->jsonText.c_str[i];
        if (0) {
        } else if (c == '{') {  // OBJECT
            if (status->level == 0) {
                status->level++;
                continue;
            }
            status->level++;
            tokenInfo.tokenType = TOKEN_OBJECT;
            if ((tokenEnd = _FindEnd(status, i + 1, '{', '}'))) {
                tokenInfo.token = StringSubString(&status->jsonText, i, tokenEnd + 1);
                status->index = tokenEnd + 1;
                return tokenInfo;
            } else {
                _RaiseJsonReadError("Can't find end of the object.", status);
                continue;
            }
        } else if (c == '[') {  // LIST
            if ((tokenEnd = _FindEnd(status, i + 1, '[', ']'))) {
                tokenInfo.tokenType = TOKEN_LIST;
                tokenInfo.token = StringSubString(&status->jsonText, i + 1, tokenEnd);
                status->index = tokenEnd + 1;
                return tokenInfo;
            } else {
                _RaiseJsonReadError("Can't find end of the array.", status);
                continue;
            }
        } else if (c == '"') {  // STRING
            tokenInfo.tokenType = TOKEN_STRING;
            if ((tokenEnd = _FindStringEnd(status, i + 1))) {
                tokenInfo.token = StringSubString(&status->jsonText, i + 1, tokenEnd);
                status->index = tokenEnd + 1;
                DEBUG_LOG_INFO("Char after string: %c", status->jsonText.c_str[status->index]);
                return tokenInfo;
            } else {
                _RaiseJsonReadError("Can't find end of the string.", status);
                continue;
            }
        } else if (c == ':') {  // END OF THE KEY
            status->keyReaded = true;
            continue;
        } else if (c == ',') {  // END OF THE VALUE
            status->keyReaded = false;
            continue;
        } else if (c == 'n') {  // NULL
            tokenInfo.tokenType = TOKEN_NULL;
            status->index = i + 4;
            return tokenInfo;
        } else if (c == 't') {  // TRUE
            tokenInfo.tokenType = TOKEN_TRUE;
            status->index = i + 4;
            return tokenInfo;
        } else if (c == 'f') {  // FALSE
            tokenInfo.tokenType = TOKEN_FALSE;
            status->index = i + 5;
            return tokenInfo;
        } else if (_IsNumber(c)) {  // NUMBER
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

static List* _CreateListFromToken(_TokenInformation info, _JsonReadStatus* status) {
    _JsonReadStatus listStatus;
    memset(&listStatus, 0, sizeof(_JsonReadStatus));
    listStatus.jsonText = info.token;
    listStatus.level = status->level;
    List* list = ListCreate();
    _TokenInformation tokenInfo = _GetNextToken(&listStatus);
    while (tokenInfo.tokenType != -1) {
        switch (tokenInfo.tokenType) {
            case TOKEN_NULL:
                ListPush(list, -1, NULL);
                break;
            case TOKEN_TRUE:
                ListPushBool(list, true);
                break;
            case TOKEN_FALSE:
                ListPushBool(list, false);
                break;
            case TOKEN_STRING:;
                String s = StringCreate(1);
                StringAppend(&s, &tokenInfo.token);
                ListPush(list, DATA_TYPE_STRING, s.c_str);
                StringFree(&s);
                StringFree(&tokenInfo.token);
                break;
            case TOKEN_NUMBER:
                ListPushNumber(list, atoll(tokenInfo.token.c_str));
                StringFree(&tokenInfo.token);
                break;
            case TOKEN_FLOAT:
                ListPushFloat(list, atof(tokenInfo.token.c_str));
                StringFree(&tokenInfo.token);
                break;
            case TOKEN_LIST:;
                List* l = _CreateListFromToken(tokenInfo, &listStatus);
                ListPush(list, DATA_TYPE_LIST, l);
                ListFree(l);
                StringFree(&tokenInfo.token);
                break;
            case TOKEN_OBJECT:;
                Dictionary* d = JsonParse(tokenInfo.token);
                ListPush(list, DATA_TYPE_OBJECT, d);
                DictionaryFree(d);
                StringFree(&tokenInfo.token);
                break;
        }
        tokenInfo = _GetNextToken(&listStatus);
    }
    return list;
}

static void _SetNextDictValue(Dictionary* dict, char* key,
                              _JsonReadStatus* status) {
    _TokenInformation tokenInfo = _GetNextToken(status);
    switch (tokenInfo.tokenType) {
        case TOKEN_NULL:
            DictionarySet(dict, key, -1, NULL);
            break;
        case TOKEN_TRUE:
            DictionarySetBool(dict, key, true);
            break;
        case TOKEN_FALSE:
            DictionarySetBool(dict, key, false);
            break;
        case TOKEN_STRING:
            DictionarySetString(dict, key, tokenInfo.token.c_str);
            StringFree(&tokenInfo.token);
            break;
        case TOKEN_NUMBER:;
            int64_t num = atoll(tokenInfo.token.c_str);
            DictionarySet(dict, key, DATA_TYPE_NUMBER, &num);
            StringFree(&tokenInfo.token);
            break;
        case TOKEN_FLOAT:;
            float f = atof(tokenInfo.token.c_str);
            DictionarySet(dict, key, DATA_TYPE_FLOAT, &f);
            StringFree(&tokenInfo.token);
            break;
        case TOKEN_OBJECT:;
            Dictionary* d = JsonParse(tokenInfo.token);
            DictionarySet(dict, key, DATA_TYPE_OBJECT, d);
            DictionaryFree(d);
            StringFree(&tokenInfo.token);
            break;
        case TOKEN_LIST:;
            List* l = _CreateListFromToken(tokenInfo, status);
            DictionarySet(dict, key, DATA_TYPE_LIST, l);
            ListFree(l);
            StringFree(&tokenInfo.token);
            break;
    }
}

Dictionary* JsonParse(String jsonString) {
    Dictionary* dict = DictionaryCreate();
    jsonString.length = strlen(jsonString.c_str);
    _JsonReadStatus status;
    memset(&status, 0, sizeof(_JsonReadStatus));
    status.jsonText = jsonString;
    while (true) {
        _TokenInformation keyInfo = _GetNextToken(&status);
        if (keyInfo.tokenType == TOKEN_STRING &&
            keyInfo.token.c_str != NULL) {
            _SetNextDictValue(dict, keyInfo.token.c_str, &status);
            StringFree(&keyInfo.token);
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
