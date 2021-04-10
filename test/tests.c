#include "tests.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Debug.h"
#include "FileUtils.h"
#include "Hash.h"
#include "Json.h"
#include "MemoryUtils.h"
#include "StringUtils.h"
#include "Timer.h"
#include "containers/Array.h"
#include "containers/Dictionary.h"
#include "containers/HashMap.h"
#include "containers/LinkedList.h"
#include "containers/List.h"
#include "containers/UniqueArray.h"

char* test_string =
    "Venenatis mauris. Curabitur ornare mollis\n"
    "velit. Sed vitae metus. Morbi posuere mi id odio. Donec elit sem,\n"
    "tempor at, pharetra eu, sodales sit amet, elit.\n"
    "Curabitur urna tellus, aliquam vitae, ultrices eget, vehicula nec, diam.\n"
    "Integer elementum, felis non faucibus euismod, erat massa dictum eros, eu\n"
    "ornare ligula tortor et mauris. Cras molestie magna in nibh. Aenean et tellus.\n"
    "Fusce adipiscing commodo erat. In eu justo. Nulla dictum, erat sed blandit\n"
    "venenatis, arcu dolor molestie dolor, vitae congue orci risus a nulla.\n"
    "Pellentesque sit amet arcu. In mattis laoreet enim. Pellentesque id augue et\n"
    "arcu blandit tincidunt. Pellentesque elit ante, rhoncus quis.\n"
    "0123456789 !'^+%&/()=?_>£#$½6{[]}@/*-|_ üğışçö ÜĞİŞÇÖ æß´~¨ \n";

char* test_string_2 = ":1;2:3;4:,;5:6;9898;i:,jkl:,asd:,asdasd,";

int64_t test_int_array[] = {5, -7, 98, 165, 789, -1, 0, 456, -123, 987};
float test_float_array[] = {5.45, 7.789, -98.145, 165.002, -789.9, 1.0, 0.5, 456.3, -123.7, 987.0};

typedef struct {
    int int_data;
    float float_data;
    char* string_data;
} test_struct;

char* rand_string(uint64_t max_len, char from, char to) {
    uint64_t len = rand() % (max_len - 1) + 1;
    char* str = CUtilsMalloc(len + 1);
    for (uint64_t i = 0; i < len; i++) {
        str[i] = rand() % (to - from) + from;
        ASSERT_BREAK(str[i] >= from);
        ASSERT_BREAK(str[i] <= to);
    }
    str[len] = 0;
    return str;
}

void sandbox() {
    uint8_t* hash = HashMD5("The quick brown fox jumps over the lazy dog");
    printf("HashMD5: ");
    for (int i = 0; i < 16; i++) {
        printf("%x", hash[i]);
    }
    printf("\n");
    CUtilsFree(hash);
}

void test_array() {
    TEST_START;
    uint64_t test_size = 1000;
    float* array = ArrayCreate(float);
    srand(time(0));
    for (uint64_t i = 0; i < test_size; i++) {
        float negative = rand() % 2 == 0 ? 1 : -1;
        float val = (rand() * rand() * 0.0001 * negative) + 1;
        uint64_t size = ArrayGetSize(array);
        size = size == 0 ? 1 : size;
        uint64_t index = rand() % size;
        ArrayPushAt(array, val, index);
        TEST_CHECK(array[index] == val);
    }
    ArrayFree(array);
    TEST_END;
}

void test_array_performance() {
    TEST_START;
    uint64_t test_size = 10000;
    DEBUG_LOG_INFO("Test size: %lu", (unsigned long)test_size);
    Timer t = TimerCreate("test_array_performance", true);
    int64_t* array = ArrayCreate(int64_t);
    for (uint64_t i = 0; i < test_size; i++) {
        int64_t value = rand();
        ArrayPush(array, value);
    }
    for (uint64_t i = 0; i < test_size; i++) {
        int64_t value = rand();
        uint64_t index = rand() % ArrayGetSize(array);
        ArrayPushAt(array, value, index);
    }
    for (uint64_t i = 0; i < test_size / 2; i++) {
        CUtilsFree(ArrayPop(array));
    }
    for (uint64_t i = 0; i < test_size / 2; i++) {
        uint64_t index = rand() % ArrayGetSize(array);
        CUtilsFree(ArrayPopAt(array, index));
    }
    ArrayFree(array);
    TimerLogElapsed(&t);
}

void test_strings() {
    TEST_START;
    // encode & decode
    String s = StringCreateCStr(test_string);
    StringEncode(&s, "some password");
    StringDecode(&s, "some password");
    TEST_CHECK(strcmp(s.c_str, test_string) == 0);
    StringFree(&s);
    // tokenize
    s = StringCreateCStr(test_string_2);
    List* tokenized = StringTokenize(&s, ":;,");
    TEST_CHECK(ListGetSize(tokenized) == 11);
    TEST_CHECK(strcmp((ListGetValue(tokenized, 0)->value), "1") == 0);
    TEST_CHECK(strcmp((ListGetValue(tokenized, 6)->value), "9898") == 0);
    TEST_CHECK(strcmp((ListGetValue(tokenized, 10)->value), "asdasd") == 0);
    ListFree(tokenized);
    StringFree(&s);
    // replace
    s = StringCreateCStr("Leo sapien and pretium elit, a faucibus and sapien dolor vel pede. Sapien and Vestibulum.");
    StringReplace(&s, "and", "ve", true);
    TEST_CHECK(
        strcmp(s.c_str,
               "Leo sapien ve pretium elit, a faucibus ve sapien dolor vel pede. Sapien ve Vestibulum.") == 0);
    StringFree(&s);
    TEST_END;
}

// void print_uint64_linkedlist(LinkedList* list) {
//     printf("LinkedList: ");
//     for (uint64_t i = 0; i < list->size; i++) {
//         printf("%I64u ", *(uint64_t*)LinkedListGetValue(list, i));
//     }
//     printf("\n");
// }

void test_linkedlist() {
    TEST_START;
    LinkedList* list = LinkedListCreate(sizeof(uint64_t));
    LinkedListPushRV(list, uint64_t, 0);
    LinkedListPushRV(list, uint64_t, 1);
    LinkedListPushRV(list, uint64_t, 2);
    LinkedListPushRV(list, uint64_t, 3);
    LinkedListPushRV(list, uint64_t, 4);
    LinkedListPushRV(list, uint64_t, 5);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 2) == 2);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 5) == 5);
    LinkedListPushAtRV(list, uint64_t, 10, 0);
    LinkedListPushAtRV(list, uint64_t, 12, 2);
    LinkedListPushAtRV(list, uint64_t, 14, 4);
    LinkedListPushAtRV(list, uint64_t, 16, 6);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 0) == 10);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 2) == 12);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 6) == 16);
    CUtilsFree(LinkedListPopAt(list, 6));
    CUtilsFree(LinkedListPopAt(list, 4));
    CUtilsFree(LinkedListPopAt(list, 2));
    CUtilsFree(LinkedListPopAt(list, 0));
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 0) == 0);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 2) == 2);
    CUtilsFree(LinkedListPop(list));
    CUtilsFree(LinkedListPop(list));
    CUtilsFree(LinkedListPop(list));
    TEST_CHECK(list->size == 3);
    CUtilsFree(LinkedListPop(list));
    CUtilsFree(LinkedListPop(list));
    CUtilsFree(LinkedListPop(list));
    TEST_CHECK(list->size == 0);
    LinkedListPushRV(list, uint64_t, 0);
    LinkedListPushRV(list, uint64_t, 1);
    LinkedListPushRV(list, uint64_t, 2);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 1) == 1);
    TEST_CHECK(*(uint64_t*)LinkedListGetValue(list, 2) == 2);
    LinkedListFree(list);
    TEST_END;
}

void test_linkedlist_performance() {
    TEST_START;
    uint64_t test_size = 10000;
    DEBUG_LOG_INFO("Test size: %lu", (unsigned long)test_size);
    Timer t = TimerCreate("test_linkedlist_performance", true);
    LinkedList* list = LinkedListCreate(sizeof(int64_t));
    for (uint64_t i = 0; i < test_size; i++) {
        int64_t value = rand();
        LinkedListPush(list, &value);
    }
    for (uint64_t i = 0; i < test_size; i++) {
        int64_t value = rand();
        uint64_t index = rand() % list->size;
        LinkedListPushAt(list, &value, index);
    }
    for (uint64_t i = 0; i < test_size / 2; i++) {
        CUtilsFree(LinkedListPop(list));
    }
    for (uint64_t i = 0; i < test_size / 2; i++) {
        uint64_t index = rand() % list->size;
        CUtilsFree(LinkedListPopAt(list, index));
    }
    LinkedListFree(list);
    TimerLogElapsed(&t);
}

void test_dictionary_and_json() {  // Create dictionary.
    TEST_START;
    Dictionary* dict = DictionaryCreate();

    // Add some keys.
    DictionarySetString(dict, "stringkey", "Ismail Bulut");
    DictionarySetNumber(dict, "numberkey", -1071);
    DictionarySetFloat(dict, "floatkey", 3.14);
    DictionarySetBool(dict, "boolkey", false);
    DictionarySet(dict, "nullkey", -1, NULL);

    // Reassign a key.
    DictionarySetFloat(dict, "nullkey", 123.456789);
    DictionarySet(dict, "new null key", -1, NULL);

    // Create another dictionaries.
    Dictionary* inner = DictionaryCreate();
    DictionarySetFloat(inner, "posX", 0.7586);
    DictionarySetFloat(inner, "posY", -785.45623);
    DictionarySetFloat(inner, "posZ", 3.789645);
    DictionarySetNumber(inner, "childcount", 2);

    Dictionary* moreinner = DictionaryCreate();
    DictionarySetString(moreinner, "1", "node1234");
    DictionarySetString(moreinner, "2", "node1235");
    DictionarySetNumber(moreinner, "number", 123456789);

    // Add them to dictionaries.
    DictionarySet(dict, "inner object", DATA_TYPE_OBJECT, inner);
    DictionarySet(inner, "more inner", DATA_TYPE_OBJECT, moreinner);

    // Free dictionaries. Root dictionary has its own copy.
    DictionaryFree(inner);

    // Create a list.
    List* list = ListCreate();
    ListPushNumber(list, 65465);
    ListPushFloat(list, 546.789);
    ListPushBool(list, true);

    // you can add recursive
    ListPush(list, DATA_TYPE_LIST, list);  // don't do this

    List* innerList = ListCreate();
    ListPush(innerList, DATA_TYPE_STRING, "string 1");
    ListPush(innerList, DATA_TYPE_STRING, "\"string\" 2");
    ListPush(innerList, DATA_TYPE_STRING, "string 3 \n \t \\");
    ListPushNumber(innerList, 65465);

    ListPush(list, DATA_TYPE_LIST, innerList);

    // Free list. Root list has its own copy.
    ListFree(innerList);

    // Add dictionary to list.
    ListPush(list, DATA_TYPE_OBJECT, moreinner);

    // Free dictionary. List has its own copy.
    DictionaryFree(moreinner);

    // Add list to dictionary.
    DictionarySet(dict, "mylist", DATA_TYPE_LIST, list);

    ListFree(list);

    // Get json string.
    String json = JsonCreate(dict);

    FileUtilsWriteString("test_dictionary_and_json.json", json);

    // Reparse json string.
    Dictionary* repDict = JsonParse(json);

    // Recreate json string from reparsed dictionary.
    String reparsedJson = JsonCreate(repDict);
    DictionaryFree(repDict);

    // Trim whitespaces.
    StringTrim(&json, " \n");
    StringTrim(&reparsedJson, " \n");

    TEST_CHECK(StringEquals(&json, &reparsedJson));

    // Free json string.
    StringFree(&json);
    StringFree(&reparsedJson);

    // Don't free the inner dictionaries and lists. The parent dictionary will delete them.
    DictionaryFree(dict);

    TEST_END;
}

// void print_float_unique_array(UniqueArray* array) {
//     printf("Arr: ");
//     for (uint64_t i = 0; i < ArrayGetSize(array->data); i++) {
//         printf("%f ", *(float*)UniqueArrayValueAt(array, i));
//     }
//     printf("\n");
// }

int test_unique_array_int_comparator(const void* v1, const void* v2) {
    int myval1 = *(int*)v1;
    int myval2 = *(int*)v2;
    if (myval1 > myval2) {
        return 1;
    } else if (myval1 < myval2) {
        return -1;
    }
    return 0;
}

void test_unique_array() {
    TEST_START;
    UniqueArray* u_arr = UniqueArrayCreate(sizeof(int), 1,
                                           test_unique_array_int_comparator);
    UniqueArrayAddRV(u_arr, int, 1);
    UniqueArrayAddRV(u_arr, int, 11);
    UniqueArrayAddRV(u_arr, int, 5);
    UniqueArrayAddRV(u_arr, int, 13);
    UniqueArrayAddRV(u_arr, int, 4);
    UniqueArrayAddRV(u_arr, int, 12);
    UniqueArrayAddRV(u_arr, int, 2);
    UniqueArrayAddRV(u_arr, int, 3);
    UniqueArrayAddRV(u_arr, int, 15);
    UniqueArrayAddRV(u_arr, int, 14);

    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 0) == 1);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 1) == 2);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 2) == 3);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 3) == 4);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 4) == 5);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 5) == 11);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 6) == 12);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 7) == 13);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 8) == 14);
    TEST_CHECK(*(int*)UniqueArrayValueAt(u_arr, 9) == 15);

    UniqueArrayFree(u_arr);
    TEST_END;
}

int test_unique_array_float_comparator(const void* v1, const void* v2) {
    float myval1 = *(float*)v1;
    float myval2 = *(float*)v2;
    if (myval1 > myval2) {
        return 1;
    } else if (myval1 < myval2) {
        return -1;
    }
    return 0;
}

void test_unique_array_performance() {
    TEST_START;
    uint64_t test_size = 50000;
    DEBUG_LOG_INFO("Test size: %lu", (unsigned long)test_size);
    Timer t = TimerCreate("test_unique_array_performance", true);
    UniqueArray* u_arr = UniqueArrayCreate(sizeof(float), test_size,
                                           test_unique_array_float_comparator);
    for (uint64_t i = 0; i < test_size; i++) {
        float value = rand() * 0.1;
        UniqueArrayAdd(u_arr, &value, NULL);
    }
    for (uint64_t i = 0; i < test_size; i++) {
        float value = rand() * 0.1;
        UniqueArrayContains(u_arr, &value, NULL);
    }
    for (uint64_t i = 0; i < test_size; i++) {
        float value = rand() * 0.1;
        UniqueArrayRemove(u_arr, &value, NULL);
    }
    UniqueArrayFree(u_arr);
    TimerLogElapsed(&t);
}

// void print_integer_type_hash_map(HashMap* hmap) {
//     DEBUG_LOG_INFO("HashMap: ");
//     for (uint64_t i = 0; i < UniqueArrayGetSize(hmap->keys); i++) {
//         DEBUG_LOG_INFO("Key hash: %I64u \t Value: %i",
//                        *(uint64_t*)UniqueArrayValueAt(hmap->keys, i),
//                        *(int*)ArrayGetValue(hmap->values, i));
//     }
// }

void test_hash_map() {
    TEST_START;
    HashMap* hmap = HashMapCreate(sizeof(int));
    HashMapSetRV(hmap, "ford", int, 15450);
    HashMapSetRV(hmap, "toyota", int, 27499);
    HashMapSetRV(hmap, "renault", int, 18000);
    HashMapSetRV(hmap, "mercedes", int, 120250);
    HashMapSetRV(hmap, "bmw", int, 105499);
    HashMapSetRV(hmap, "audi", int, 109999);
    HashMapSetRV(hmap, "porche", int, 249000);
    HashMapSetRV(hmap, "ferrari", int, 414999);

    TEST_CHECK(*(int*)HashMapGet(hmap, "ford") = 15450);

    // Raise some vehicles
    HashMapSetRV(hmap, "ford", int, 25999);
    HashMapSetRV(hmap, "toyota", int, 30000);
    HashMapSetRV(hmap, "mercedes", int, 150000);

    TEST_CHECK(HashMapContains(hmap, "bmw"));

    TEST_CHECK(*(int*)HashMapGet(hmap, "ford") = 25999);
    TEST_CHECK(*(int*)HashMapGet(hmap, "toyota") == 30000);
    TEST_CHECK(*(int*)HashMapGet(hmap, "bmw") == 105499);
    TEST_CHECK(*(int*)HashMapGet(hmap, "ferrari") == 414999);

    TEST_CHECK(HashMapRemove(hmap, "ferrari"));
    // print_integer_type_hash_map(hmap);
    TEST_CHECK(HashMapGet(hmap, "ferrari") == NULL);

    HashMapFree(hmap);
    TEST_END;
}

void test_hash_map_performance() {
    TEST_START;
    uint64_t test_size = 50000;
    DEBUG_LOG_INFO("Test size: %lu", (unsigned long)test_size);
    Timer t = TimerCreate("test_hash_map_performance", true);
    HashMap* hmap = HashMapCreate(sizeof(int));
    srand(time(0));
    for (uint64_t i = 0; i < test_size; i++) {
        char* key = rand_string(16, 32, 126);
        HashMapSetRV(hmap, key, int, rand());
        CUtilsFree(key);
    }
    HashMapFree(hmap);
    TimerLogElapsed(&t);
}

void test_file_write_read_string() {
    TEST_START;
    String writed = StringCreateCStr(test_string);
    TEST_ASSERT(FileUtilsWriteString("test_file_write_read_string", writed));
    String readed;
    TEST_ASSERT(FileUtilsReadString("test_file_write_read_string", &readed));
    TEST_CHECK(StringEquals(&writed, &readed));
    StringFree(&writed);
    StringFree(&readed);
    TEST_END;
}

void test_file_write_read_binary() {
    TEST_START;
    test_struct writed = {256, 3.14, "Ismail Bulut"};
    TEST_ASSERT(FileUtilsWriteBinary("test_file_write_read_binary", &writed, sizeof(writed)));
    test_struct* readed;
    size_t readed_size;
    TEST_ASSERT(FileUtilsReadBinary("test_file_write_read_binary", (void**)&readed, &readed_size));
    TEST_CHECK(sizeof(writed) == readed_size);
    TEST_CHECK(writed.int_data == readed->int_data);
    TEST_CHECK(writed.float_data == readed->float_data);
    TEST_CHECK(strcmp(writed.string_data, readed->string_data) == 0);
    CUtilsFree(readed);
    TEST_END;
}
