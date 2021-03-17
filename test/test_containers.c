#include "test_containers.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Array.h"
#include "Debug.h"
#include "Dictionary.h"
#include "FileUtils.h"
#include "Json.h"
#include "LinkedList.h"
#include "Map.h"
#include "StringUtils.h"

char* test_string =
    "üğşiçö Venenatis mauris. Curabitur ornare mollis .çöşiğüßDæ~´´:::;;;/*-+\n"
    "velit. Sed vitae metus. Morbi posuere mi id odio. Donec elit sem,\n"
    "tempor at, pharetra eu, sodales sit amet, elit.\n"
    "Curabitur urna tellus, aliquam vitae, ultrices eget, vehicula nec, diam.\n"
    "Integer elementum, felis non faucibus euismod, erat massa dictum eros, eu\n"
    "ornare ligula tortor et mauris. Cras molestie magna in nibh. Aenean et tellus.\n"
    "Fusce adipiscing commodo erat. In eu justo. Nulla dictum, erat sed blandit\n"
    "venenatis, arcu dolor molestie dolor, vitae congue orci risus a nulla.\n"
    "Pellentesque sit amet arcu. In mattis laoreet enim. Pellentesque id augue et\n"
    "arcu blandit tincidunt. Pellentesque elit ante, rhoncus quis.";

int64_t test_int_array[] = {5, -7, 98, 165, 789, -1, 0, 456, -123, 987};
float test_float_array[] = {5.45, 7.789, -98.145, 165.002, -789.9, 1.0, 0.5, 456.3, -123.7, 987.0};

void array_push_and_pop_time() {
}

void simple_test_String() {
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
    DictionarySet(dict, "nullkey", NULL);

    // Reassign a key.
    DictionarySetFloat(dict, "nullkey", 123.456789);
    DictionarySet(dict, "new null key", NULL);

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
    DictionarySetObject(dict, "inner object", inner);
    DictionarySetObject(inner, "more inner", moreinner);

    // Create arrays.
    int64_t* numberArray = ArrayCreate(int64_t);
    float* floatArray = ArrayCreate(float);
    ArrayInsert(numberArray, test_int_array, sizeof(test_int_array) / sizeof(int64_t));
    ArrayInsert(floatArray, test_float_array, sizeof(test_float_array) / sizeof(float));

    // Add arrays to root dictionary.
    DictionarySetNumberArray(dict, "number array", numberArray);
    DictionarySetFloatArray(inner, "float array", floatArray);

    // Free the arrays. Dictionary has it own.
    ArrayFree(numberArray);
    ArrayFree(floatArray);

    // Create a string list. (LinkedList)
    LinkedList* list = LinkedListCreate();
    LinkedListPush(list, "This", 5);
    LinkedListPush(list, "is", 2);
    LinkedListPush(list, "a", 2);
    LinkedListPush(list, "string", 6);
    LinkedListPush(list, "list.", 5);

    // Add string list to the root dictionary.
    DictionarySetStringList(dict, "string list", list);

    // Free the list. Dictionary has its own copy.
    LinkedListFree(list);

    // Get json string.
    String json = JsonCreate(dict);

    // Write json file.
    FileUtilsWriteString("jsontest.json", json);
    StringFree(&json);

    // Don't free the inner dictionaries. The parent dictionary owns it and will frees.
    DictionaryFree(dict);
}

void read_json_file_and_parse() {
    String json;
    if (FileUtilsReadString("jsontest.json", &json)) {
        Dictionary* dict = JsonParse(json);
        String reparsedJson = JsonCreate(dict);
        printf("REPARSED JSON:\n%s\n", reparsedJson.c_str);
        DictionaryFree(dict);
        StringFree(&reparsedJson);
        StringFree(&json);
    } else {
        printf("File read error.\n");
    }
}
