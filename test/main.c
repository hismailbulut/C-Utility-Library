#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "tests.h"

int main(void) {
    test_array();
    test_strings();
    test_linkedlist();
    test_dictionary_and_json();
    test_unique_array();
    return 0;
}
