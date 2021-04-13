#include "Hash.h"

#include <assert.h>
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

static inline uint32_t _LRot32(uint32_t x, uint32_t c) {
    return (x << c) | (x >> (32 - c));
}

static inline uint32_t _RRot32(uint32_t x, uint32_t c) {
    return (x >> c) | (x << (32 - c));
}

static inline void _SwitchEndian32(uint32_t* v) {
    char* b = (char*)v;
    MemorySwap(b + 0, b + 3, 1);
    MemorySwap(b + 1, b + 2, 1);
}

static inline void _SwitchEndian64(uint64_t* v) {
    char* b = (char*)v;
    MemorySwap(b + 0, b + 7, 1);
    MemorySwap(b + 1, b + 6, 1);
    MemorySwap(b + 2, b + 5, 1);
    MemorySwap(b + 3, b + 4, 1);
}

static uint8_t* _PreProcessMessage(const char* buffer, size_t bufferSize,
                                   uint64_t* outMessageSize, size_t chunkSize, bool switchEndian) {
    uint64_t messageSize;
    messageSize = (bufferSize + (chunkSize - (bufferSize % chunkSize)));
    if (bufferSize % chunkSize >= chunkSize - 8) {
        messageSize += chunkSize;
    }
    uint8_t* message = CUtilsMalloc(messageSize);
    memcpy(message, buffer, bufferSize);
    uint64_t index = bufferSize;
    // append "1" bit to message
    message[index++] = 0x80;
    // append "0" bit until message length in bits ≡ 448 (mod 512)
    while (index % chunkSize != chunkSize - 8) {
        message[index++] = 0;
    }
    // append original length in bits mod (2 pow 64) to message
    uint64_t lenInBits = bufferSize * 8;
    if (switchEndian) {
        _SwitchEndian64(&lenInBits);
    }
    memcpy(message + index, &lenInBits, 8);
    *outMessageSize = messageSize;
    return message;
}

uint64_t Hash_64(const char* buffer, size_t bufferSize) {
    uint32_t A = 0x4048f5c3;
    uint32_t B = 0x3fcf1aa0;
    for (uint64_t i = 0; i < bufferSize; i++) {
        uint32_t t1 = _RRot32(buffer[i], 3) ^ (A >> 5);
        uint32_t t2 = _LRot32(buffer[bufferSize - i - 1], 7) ^ (B << 5);
        A += t1;
        B += t2;
    }
    uint64_t hash;
    memcpy((char*)&hash + 0, &A, 4);
    memcpy((char*)&hash + 4, &B, 4);
    return hash;
}

uint8_t* Hash_MD5_128(const char* buffer, size_t bufferSize) {
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
    uint32_t a = 0x67452301;
    uint32_t b = 0xefcdab89;
    uint32_t c = 0x98badcfe;
    uint32_t d = 0x10325476;
    uint64_t messageLen;
    uint8_t* message = _PreProcessMessage(buffer, bufferSize, &messageLen, 64, false);
    for (uint64_t j = 0; j < messageLen / 64; j++) {
        uint32_t* M = (uint32_t*)(message + j * (messageLen / 64));
        uint32_t A = a;
        uint32_t B = b;
        uint32_t C = c;
        uint32_t D = d;
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
            B = B + _LRot32(A + F + K[i] + M[g], s[i]);
            A = tempD;
        }
        a += A;
        b += B;
        c += C;
        d += D;
    }
    CUtilsFree(message);
    uint8_t* hash = CUtilsMalloc(16);
    memcpy(hash + 0, &a, 4);
    memcpy(hash + 4, &b, 4);
    memcpy(hash + 8, &c, 4);
    memcpy(hash + 12, &d, 4);
    return hash;
}

uint8_t* Hash_SHA2_256(const char* buffer, size_t bufferSize) {
    uint32_t h0 = 0x6a09e667;
    uint32_t h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372;
    uint32_t h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f;
    uint32_t h5 = 0x9b05688c;
    uint32_t h6 = 0x1f83d9ab;
    uint32_t h7 = 0x5be0cd19;

    uint32_t k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    uint64_t messageSize;
    uint8_t* message = _PreProcessMessage(buffer, bufferSize, &messageSize, 64, true);

    for (uint64_t j = 0; j < messageSize / 64; j++) {
        uint32_t w[64];
        memcpy(w, message + j * (messageSize / 64), 16 * 4);
        // This implementation is little-endian and constant values are
        // big-endian. We must switch these bytes.
        for (int i = 0; i < 16; i++) {
            _SwitchEndian32(w + i);
        }

        for (int i = 16; i < 64; i++) {
            uint32_t s0 = _RRot32(w[i - 15], 7) ^
                          _RRot32(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = _RRot32(w[i - 2], 17) ^
                          _RRot32(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;
        uint32_t f = h5;
        uint32_t g = h6;
        uint32_t h = h7;

        for (int i = 0; i < 64; i++) {
            uint32_t S1 = _RRot32(e, 6) ^ _RRot32(e, 11) ^ _RRot32(e, 25);
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t temp1 = h + S1 + ch + k[i] + w[i];
            uint32_t S0 = _RRot32(a, 2) ^ _RRot32(a, 13) ^ _RRot32(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
        h5 += f;
        h6 += g;
        h7 += h;
    }

    CUtilsFree(message);

    _SwitchEndian32(&h0);
    _SwitchEndian32(&h1);
    _SwitchEndian32(&h2);
    _SwitchEndian32(&h3);
    _SwitchEndian32(&h4);
    _SwitchEndian32(&h5);
    _SwitchEndian32(&h6);
    _SwitchEndian32(&h7);

    uint8_t* hash = CUtilsMalloc(32);
    memcpy(hash + 0, &h0, 4);
    memcpy(hash + 4, &h1, 4);
    memcpy(hash + 8, &h2, 4);
    memcpy(hash + 12, &h3, 4);
    memcpy(hash + 16, &h4, 4);
    memcpy(hash + 20, &h5, 4);
    memcpy(hash + 24, &h6, 4);
    memcpy(hash + 28, &h7, 4);

    return hash;
}

uint8_t* Hash_SHA2_512(const char* buffer, size_t bufferSize) {
    // TODO implement
}

#ifdef __cplusplus
}
#endif
