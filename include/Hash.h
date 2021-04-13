#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns 64 bit hash.
uint64_t Hash_64(const char* buffer, size_t bufferSize);

// Returns 128 bit (16 byte) MD5 hash.
uint8_t* Hash_MD5_128(const char* buffer, size_t bufferSize);

// Returns 256 bit (32 byte) SHA2 hash.
uint8_t* Hash_SHA2_256(const char* buffer, size_t bufferSize);

// Returns 512 bit (64 byte) SHA2 hash.
uint8_t* Hash_SHA2_512(const char* buffer, size_t bufferSize);

#ifdef __cplusplus
}
#endif
