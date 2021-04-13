#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "Timer.h"
#include "tests.h"

int main(void) {
    Timer t = TimerCreate("c_utils_test", true);
    // sandbox();
    test_array();
    test_array_performance();
    test_strings();
    test_linkedlist();
    test_linkedlist_performance();
    test_dictionary_and_json();
    test_unique_array();
    test_unique_array_performance();
    test_hash_algorithms();
    test_hash_map();
    test_hash_map_performance();
    test_file_write_read_string();
    test_file_write_read_binary();
    DEBUG_LOG_INFO("Total malloc: %lu, Total free: %lu, Total realloc: %lu",
                   (unsigned long)c_utils_total_malloc,
                   (unsigned long)c_utils_total_free,
                   (unsigned long)c_utils_total_realloc);
    TimerLogElapsed(&t);
    return 0;
}
