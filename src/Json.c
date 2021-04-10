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

#define APPEND_TABS(str, count)                \
    {                                          \
        for (uint32_t j = 0; j < count; j++) { \
            StringAppendCStr(&str, "    ");    \
        }                                      \
    }

#ifdef __cplusplus
extern "C" {
#endif

String _JsonCreate(Dictionary* dict, uint32_t indentLevel);

static String _StringToJsonString(char* str) {
    uint64_t len = strlen(str);
    String s = StringCreate(len);
    StringAppendChar(&s, '"');
    for (uint64_t i = 0; i < len; i++) {
        switch (str[i]) {
            case '\\':
                StringAppendCStr(&s, "\\\\");
                break;
            case '"':
                StringAppendCStr(&s, "\\\"");
                break;
            case '\n':
                StringAppendCStr(&s, "\\n");
                break;
            case '\t':
                StringAppendCStr(&s, "\\t");
                break;
            default:
                if (str[i] < 32) {  // escape sequence
                    DEBUG_LOG_WARN("JsonCreate: Unsupported escape sequence.");
                } else {
                    StringAppendChar(&s, str[i]);
                }
                break;
                // TODO: add other escape sequences
        }
    }
    StringAppendChar(&s, '"');
    return s;
}

static String _ListToString(List* list, uint32_t indentLevel) {
    uint64_t listSize = ListGetSize(list);
    String s = StringCreate(1);
    StringAppendChar(&s, '[');
    for (uint64_t i = 0; i < listSize; i++) {
        ListNode* node = ListGetValue(list, i);
        StringAppendChar(&s, '\n');
        APPEND_TABS(s, indentLevel);
        if (node->value) {
            switch (node->dataType) {
                case DATA_TYPE_STRING: {
                    String jstr = _StringToJsonString(node->value);
                    StringAppend(&s, &jstr);
                    StringFree(&jstr);
                    break;
                }
                case DATA_TYPE_NUMBER:
                    StringAppendFormat(&s, "%ld", (long)*(int64_t*)node->value);
                    break;
                case DATA_TYPE_FLOAT:
                    StringAppendFormat(&s, "%f", *(float*)node->value);
                    break;
                case DATA_TYPE_BOOL:
                    if (*(bool*)node->value) {
                        StringAppendCStr(&s, "true");
                    } else {
                        StringAppendCStr(&s, "false");
                    }
                    break;
                case DATA_TYPE_LIST: {
                    String listString = _ListToString(node->value, indentLevel + 1);
                    StringAppend(&s, &listString);
                    StringFree(&listString);
                    break;
                }
                case DATA_TYPE_OBJECT: {
                    String objectString = _JsonCreate(node->value, indentLevel + 1);
                    StringAppend(&s, &objectString);
                    StringFree(&objectString);
                    break;
                }
                default:
                    StringAppendCStr(&s, "null");
                    break;
            }
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
        case DATA_TYPE_STRING: {
            String jstr = _StringToJsonString(pair->value);
            StringAppend(&s, &jstr);
            StringFree(&jstr);
            break;
        }
        case DATA_TYPE_NUMBER:
            StringAppendFormat(&s, "%ld", (long)*(int64_t*)pair->value);
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
    DEBUG_LOG_ERROR("Json Reader: %s At index: %lu",
                    message, (unsigned long)status->index);
    status->errorCount++;
}

static String _ReadString(_JsonReadStatus* status, uint64_t start, uint64_t* outEnd) {
    String s = StringCreate(1);
    bool escape = false;
    for (uint64_t i = start; i < status->jsonText.length; i++) {
        if (escape) {
            switch (status->jsonText.c_str[i]) {
                case '\\':
                    StringAppendChar(&s, '\\');
                    break;
                case '"':
                    StringAppendChar(&s, '"');
                    break;
                case 'n':
                    StringAppendChar(&s, '\n');
                    break;
                case 't':
                    StringAppendChar(&s, '\t');
                    break;
                default:
                    _RaiseJsonReadError("Unsupported escape sequence.", status);
                    StringAppendChar(&s, status->jsonText.c_str[i]);
                    break;
            }
            escape = false;
        } else {
            switch (status->jsonText.c_str[i]) {
                case '\\':
                    escape = true;
                    break;
                case '"':
                    *outEnd = i;
                    return s;
                default:
                    StringAppendChar(&s, status->jsonText.c_str[i]);
                    break;
            }
        }
    }
    return s;
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
    switch (c) {
            // clang-format off
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '+': case '-': // optional
            // clang-format on
            return 1;  // number
        case '.':
            return 2;  // float
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
        switch (c) {
            case '{': {  // OBJECT
                if (status->level == 0) {
                    status->level++;
                    break;
                }
                status->level++;
                tokenInfo.tokenType = TOKEN_OBJECT;
                if ((tokenEnd = _FindEnd(status, i + 1, '{', '}'))) {
                    tokenInfo.token = StringSubString(&status->jsonText, i, tokenEnd + 1);
                    status->index = tokenEnd + 1;
                    return tokenInfo;
                } else {
                    _RaiseJsonReadError("Can't find end of the object.", status);
                }
                break;
            }
            case '[': {  //LIST
                if ((tokenEnd = _FindEnd(status, i + 1, '[', ']'))) {
                    tokenInfo.tokenType = TOKEN_LIST;
                    tokenInfo.token = StringSubString(&status->jsonText, i + 1, tokenEnd);
                    status->index = tokenEnd + 1;
                    return tokenInfo;
                } else {
                    _RaiseJsonReadError("Can't find end of the array.", status);
                }
                break;
            }
            case '"': {  //STRING
                tokenInfo.tokenType = TOKEN_STRING;
                tokenInfo.token = _ReadString(status, i + 1, &tokenEnd);
                status->index = tokenEnd + 1;
                return tokenInfo;
            }
            case ':':  //END OF A KEY
                status->keyReaded = true;
                break;
            case ',':  //END OF A VALUE
                status->keyReaded = false;
                break;
            case 'n':  //NULL
                tokenInfo.tokenType = TOKEN_NULL;
                status->index = i + 4;
                return tokenInfo;
            case 't':  //TRUE
                tokenInfo.tokenType = TOKEN_TRUE;
                status->index = i + 4;
                return tokenInfo;
            case 'f':  //FALSE
                tokenInfo.tokenType = TOKEN_FALSE;
                status->index = i + 5;
                return tokenInfo;
            default:
                if (_IsNumber(c)) {
                    tokenInfo.tokenType = TOKEN_NUMBER;
                    tokenEnd = i;
                    int number;
                    while ((number = _IsNumber(status->jsonText.c_str[tokenEnd]))) {
                        if (number == 2) {  // FLOAT
                            tokenInfo.tokenType = TOKEN_FLOAT;
                        }
                        tokenEnd++;
                    }
                    tokenInfo.token = StringSubString(&status->jsonText, i, tokenEnd);
                    status->index = tokenEnd;
                    return tokenInfo;
                }
                break;
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
            case TOKEN_STRING:
                ListPush(list, DATA_TYPE_STRING, tokenInfo.token.c_str);
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
            case TOKEN_LIST: {
                List* v = _CreateListFromToken(tokenInfo, &listStatus);
                ListPush(list, DATA_TYPE_LIST, v);
                ListFree(v);
                StringFree(&tokenInfo.token);
                break;
            }
            case TOKEN_OBJECT: {
                Dictionary* v = JsonParse(tokenInfo.token);
                ListPush(list, DATA_TYPE_OBJECT, v);
                DictionaryFree(v);
                StringFree(&tokenInfo.token);
                break;
            }
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
            return;
        case TOKEN_TRUE:
            DictionarySetBool(dict, key, true);
            return;
        case TOKEN_FALSE:
            DictionarySetBool(dict, key, false);
            return;
        case TOKEN_STRING:
            DictionarySetString(dict, key, tokenInfo.token.c_str);
            break;
        case TOKEN_NUMBER: {
            int64_t v = atoll(tokenInfo.token.c_str);
            DictionarySet(dict, key, DATA_TYPE_NUMBER, &v);
            break;
        }
        case TOKEN_FLOAT: {
            float v = atof(tokenInfo.token.c_str);
            DictionarySet(dict, key, DATA_TYPE_FLOAT, &v);
            break;
        }
        case TOKEN_OBJECT: {
            Dictionary* v = JsonParse(tokenInfo.token);
            DictionarySet(dict, key, DATA_TYPE_OBJECT, v);
            DictionaryFree(v);
            break;
        }
        case TOKEN_LIST: {
            List* v = _CreateListFromToken(tokenInfo, status);
            DictionarySet(dict, key, DATA_TYPE_LIST, v);
            ListFree(v);
            break;
        }
    }
    StringFree(&tokenInfo.token);
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
    if (status.errorCount > 0) {
        DEBUG_LOG_ERROR("Json parsing failed. %u total errors.",
                        status.errorCount);
    }
    return dict;
}

#ifdef __cplusplus
}
#endif
