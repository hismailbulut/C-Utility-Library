#include "Hash.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "containers/Array.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t _MD5LeftRotate(uint32_t x, uint32_t c) {
    return (x << c) | (x >> (32 - c));
}

uint8_t* HashMD5(const char* key) {
    // Implemented from Wikipedia MD5 pseudocode
    // clang-format off
    uint32_t s[64] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
    };
    uint32_t K[64] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };
    // clang-format on
    uint32_t a0 = 0x67452301;  //A
    uint32_t b0 = 0xefcdab89;  //B
    uint32_t c0 = 0x98badcfe;  //C
    uint32_t d0 = 0x10325476;  //D

    uint64_t keyLen = strlen(key);
    uint64_t messageLen;
    if (keyLen % 64 < 56) {
        messageLen = (keyLen + (64 - (keyLen % 64)));
    } else {
        messageLen = (keyLen + (64 - (keyLen % 64))) + 64;
    }
    ASSERT_BREAK_MSG(messageLen > keyLen && messageLen % 64 == 0,
                     "KeyLen: %I64u, MessageLen: %I64u", keyLen, messageLen);

    uint8_t* message = CUtilsMalloc(messageLen);
    memcpy(message, key, keyLen);
    uint64_t index = keyLen;
    // append "1" bit to message
    message[index++] = 0x80;
    // append "0" bit until message length in bits ≡ 448 (mod 512)
    while (index % 64 != 56) {
        message[index++] = 0;
    }
    // append original length in bits mod (2 pow 64) to message
    uint64_t lenInBits = keyLen * 8;
    memcpy(message + index, &lenInBits, 8);

    for (uint64_t j = 0; j < messageLen / 64; j++) {
        // break chunk into sixteen 32-bit words M[j], 0 ≤ j ≤ 15
        uint32_t M[16];
        memcpy(M, message + j * messageLen, 64);

        uint32_t A = a0;
        uint32_t B = b0;
        uint32_t C = c0;
        uint32_t D = d0;
        for (uint64_t i = 0; i < 64; i++) {
            uint32_t F;
            uint32_t g;
            if (i < 16) {
                F = (B & C) | (~B & D);
                g = i;
            } else if (i < 32) {
                F = (D & B) | (~D & C);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                F = B ^ C ^ D;
                g = (3 * i + 5) % 16;
            } else {
                F = C ^ (B | ~D);
                g = (7 * i) % 16;
            }
            uint32_t tempD = D;
            D = C;
            C = B;
            B = B + _MD5LeftRotate(A + F + K[i] + M[g], s[i]);
            A = tempD;
        }
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    CUtilsFree(message);
    uint8_t* hash = CUtilsMalloc(16);
    memcpy(hash + 0, &a0, 4);
    memcpy(hash + 4, &b0, 4);
    memcpy(hash + 8, &c0, 4);
    memcpy(hash + 12, &d0, 4);

    return hash;
}

#ifdef __cplusplus
}
#endif
