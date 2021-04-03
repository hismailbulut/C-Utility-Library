#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "tests.h"

int main(void) {
    test_array();
    test_array_performance();
    test_strings();
    test_linkedlist();
    test_linkedlist_performance();
    test_dictionary_and_json();
    test_unique_array();
    test_hash_map();
    DEBUG_LOG_INFO("Total malloc: %I64u, Total free: %I64u",
                   c_utils_total_malloc, c_utils_total_free);
    return 0;
}
