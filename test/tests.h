#pragma once

#define TEST_START bool _test_status_ = true

#define TEST_CHECK(cond) \
    if (!(cond)) _test_status_ = false

#define TEST_END                                            \
    {                                                       \
        if (_test_status_) {                                \
            DEBUG_LOG_INFO("Test '%s' passed.", __func__);  \
        } else {                                            \
            DEBUG_LOG_ERROR("Test '%s' failed.", __func__); \
        }                                                   \
    }

void test_array();

void test_strings();

void test_linkedlist();

void test_dictionary_and_json();

void test_unique_array();
