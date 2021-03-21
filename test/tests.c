#include "tests.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.h"
#include "Defines.h"
#include "FileUtils.h"
#include "Json.h"
#include "StringUtils.h"
#include "containers/Array.h"
#include "containers/Dictionary.h"
#include "containers/LinkedList.h"
#include "containers/List.h"
#include "containers/Map.h"

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
    "0123456789 !'^+%&/()=?_>£#$½6{[]}@/*-|_ üğışçö ÜĞİŞÇÖ æß´~¨\n";

char* test_string_2 = ":1;2:3;4:,;5:6;9898;i:,jkl:,asd:,asdasd,";

int64_t test_int_array[] = {5, -7, 98, 165, 789, -1, 0, 456, -123, 987};
float test_float_array[] = {5.45, 7.789, -98.145, 165.002, -789.9, 1.0, 0.5, 456.3, -123.7, 987.0};

void array_push_and_pop_time() {
}

void test_strings() {
    TEST_START;
    // encode & decode
    String s = StringCreateCStr(test_string);
    StringEncode(&s, "my powerfull encoding function");
    StringDecode(&s, "my powerfull encoding function");
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

void simple_test_LinkedList() {
}

// MAP.H BEGIN
void print_map(Map* map) {
    printf("------------------------------\n");
    printf("Map Size = %I64u\n", MapGetSize(map));
    for (uint64_t i = 0; i < MapGetSize(map); i++) {
        char* key = MapGetKeyAt(map, i);
        ASSERT_BREAK(key);
        uint32_t* value = MapFindValue(map, key, strlen(key) + 1);
        ASSERT_BREAK(value);
        printf("\tKey: %s\tValue: %u\n", key, *value);
    }
    printf("------------------------------\n");
}
void set_map_key(Map* map, const char* name, uint32_t age) {
    MapSet(map, name, strlen(name) + 1, &age, sizeof(age));
}
void simple_test_Map() {
    TEST_START;
    Map* animals = MapCreate();
    set_map_key(animals, "Dog", 20);
    set_map_key(animals, "Cat", 18);
    set_map_key(animals, "Elephant", 13);
    TEST_CHECK(*(uint32_t*)MapFindValue(animals, "Elephant", 9) == 13);
    set_map_key(animals, "Monkey", 50);
    TEST_CHECK(*(uint32_t*)MapFindValue(animals, "Monkey", 7) == 50);
    MapChangeKey(animals, "Dog", 4, "Lion", 5);
    TEST_CHECK(*(uint32_t*)MapFindValue(animals, "Lion", 5) == 20);
    int v = 18;
    char* cat_key = MapFindKey(animals, &v, sizeof(v));
    TEST_CHECK(strcmp(cat_key, "Cat") == 0);
    set_map_key(animals, "Monkey", 1071);
    TEST_CHECK(*(uint32_t*)MapFindValue(animals, "Monkey", 7) == 1071);
    MapFree(animals);
    TEST_END;
}
// MAP.H END

void simple_test_Files() {
}

void create_dictionary_and_json() {  // Create dictionary.
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

    // Create a list.
    List* list = ListCreate();
    ListPushNumber(list, 65465);
    ListPushFloat(list, 546.789);
    ListPushBool(list, true);

    // ListPush(list, DATA_TYPE_LIST, list); // don't do this

    List* innerList = ListCreate();
    ListPush(innerList, DATA_TYPE_STRING, "string 1");
    ListPush(innerList, DATA_TYPE_STRING, "string 2");
    ListPush(innerList, DATA_TYPE_STRING, "string 3");
    ListPushNumber(innerList, 65465);

    ListPush(list, DATA_TYPE_LIST, innerList);

    // Add dictionary to list.
    ListPush(list, DATA_TYPE_OBJECT, moreinner);

    // Add list to dictionary.
    DictionarySet(dict, "mylist", DATA_TYPE_LIST, list);

    // Get json string.
    String json = JsonCreate(dict);

    // Write json file.
    /* FileUtilsWriteString("test.json", json); */
    printf("JSON:\n%s\n", json.c_str);
    FileUtilsWriteString("test.json", json);

    // Free json string.
    StringFree(&json);

    // Don't free the inner dictionaries and lists. The parent dictionary owns it and will frees.
    DictionaryFree(dict);
}

void read_json_file_and_parse() {
    TEST_START;
    String json;
    if (FileUtilsReadString("test.json", &json)) {
        Dictionary* dict = JsonParse(json);
        String reparsedJson = JsonCreate(dict);

        FileUtilsWriteString("reparsed.json", reparsedJson);

        DictionaryFree(dict);
        StringFree(&reparsedJson);
        StringFree(&json);
    } else {
        DEBUG_LOG_ERROR("Error.");
    }
    TEST_END;
}
