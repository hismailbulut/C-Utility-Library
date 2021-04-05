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
	DEBUG_LOG_INFO("Total malloc: %u, Total free: %u", (uint32_t)c_utils_total_malloc, (uint32_t)c_utils_total_free);
    return 0;
}
