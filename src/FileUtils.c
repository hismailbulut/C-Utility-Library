#include "FileUtils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "Debug.h"
#include "MemoryUtils.h"
#include "StringUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

static size_t _GetFileSize(const char* path) {
    struct stat info;
    if (stat(path, &info) == 0) {
        return info.st_size;
    }
    DEBUG_LOG_ERROR("File size couldn't be calculated.");
    return 0;
}

static inline void _CheckError(FILE* file) {
    if (ferror(file) != 0) {
        perror("[FILE UTILS ERROR]");
    }
}

bool FileUtilsReadString(const char* path, String* outString) {
    size_t fileSize = _GetFileSize(path);
    if (fileSize <= 0) {
        return false;
    }
    bool success = false;
    char* buffer = CUtilsMalloc(fileSize + 1);
    uint64_t readed;
    FILE* file = fopen(path, "r");
    if (file != NULL) {
        if ((readed = fread(buffer, 1, fileSize, file)) > 0) {
            *outString = StringCreateCStr(buffer);
            // printf("Readed: %I64u, size: %I64u, strlen: %I64u\n", readed, fileSize, outString->length);
            // StringErase(outString, readed, 0);
            success = true;
        }
        fclose(file);
    }
    CUtilsFree(buffer);
    if (!success) {
        outString->c_str = NULL;
        outString->length = 0;
        _CheckError(file);
    }
    return success;
}

bool FileUtilsWriteString(const char* path, String string) {
    bool success = false;
    FILE* file = fopen(path, "w");
    if (file != NULL) {
        if (fwrite(string.c_str, 1, strlen(string.c_str), file) > 0) {
            success = true;
        }
        fclose(file);
    }
    if (!success) {
        _CheckError(file);
    }
    return success;
}

bool FileUtilsAppendString(const char* path, String string) {
    bool success = false;
    FILE* file = fopen(path, "a");
    if (file != NULL) {
        if (fwrite(string.c_str, 1, strlen(string.c_str), file) > 0) {
            success = true;
        }
        fclose(file);
    }
    if (!success) {
        _CheckError(file);
    }
    return success;
}

bool FileUtilsReadBinary(const char* path, void** outBuffer,
                         size_t* outBufferSize) {
    size_t fileSize = _GetFileSize(path);
    if (fileSize <= 0) {
        return false;
    }
    bool success = false;
    FILE* file = fopen(path, "rb");
    if (file != NULL) {
        *outBuffer = CUtilsMalloc(fileSize);
        if (fread(*outBuffer, 1, fileSize, file) > 0) {
            *outBufferSize = fileSize;
            success = true;
        } else {
            CUtilsFree(*outBuffer);
        }
        fclose(file);
    }
    if (!success) {
        *outBufferSize = 0;
        _CheckError(file);
    }
    return success;
}

bool FileUtilsWriteBinary(const char* path, void* buffer, size_t bufferSize) {
    bool success = false;
    FILE* file = fopen(path, "wb");
    if (file != NULL) {
        if (fwrite(buffer, 1, bufferSize, file) > 0) {
            success = true;
        }
        fclose(file);
    }
    if (!success) {
        _CheckError(file);
    }
    return success;
}

#ifdef __cplusplus
}
#endif
