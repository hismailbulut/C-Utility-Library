#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns 128 bit (16 byte) MD5 hash.
uint8_t* HashMD5(const char* key);

#ifdef __cplusplus
}
#endif
