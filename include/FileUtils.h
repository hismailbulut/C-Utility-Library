#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "StringUtils.h"

bool FileUtilsReadString(const char* path, String* outString);

bool FileUtilsWriteString(const char* path, String string);

bool FileUtilsAppendString(const char* path, String string);

bool FileUtilsReadBinary(const char* path, void** outBuffer, size_t* outBufferSize);

bool FileUtilsWriteBinary(const char* path, void* buffer, size_t bufferSize);
