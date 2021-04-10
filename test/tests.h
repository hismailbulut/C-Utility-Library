#pragma once

#include "tests.h"
#define TEST_START             \
    bool _test_status_ = true; \
    DEBUG_LOG_INFO("Test '%s' started.", __func__)

#ifdef CUTILS_DEBUG_BUILD
#define TEST_CHECK(cond) \
    ASSERT_BREAK(cond)
#else
#define TEST_CHECK(cond) \
    if (!(cond)) _test_status_ = false
#endif  // CUTILS_DEBUG_BUILD

#define TEST_ASSERT(cond) \
    assert(cond)

#define TEST_END                                            \
    {                                                       \
        if (_test_status_) {                                \
            DEBUG_LOG_INFO("Test '%s' passed.", __func__);  \
        } else {                                            \
            DEBUG_LOG_ERROR("Test '%s' failed.", __func__); \
        }                                                   \
    }

void sandbox();
void test_array();
void test_array_performance();
void test_strings();
void test_linkedlist();
void test_linkedlist_performance();
void test_dictionary_and_json();
void test_unique_array();
void test_unique_array_performance();
void test_hash_map();
void test_hash_map_performance();
void test_file_write_read_string();
void test_file_write_read_binary();
