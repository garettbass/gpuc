#pragma once
#include <ctype.h>
#include <iso646.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexeme.h"

static size_t gpucStreamWrite(char** out, const char* head, size_t length) {
    const bool canWrite = (out and *out);
    if (canWrite) {
        strncpy(*out, head, length);
        *out += length;
        **out = '\0';
    }
    return length;
}

static size_t gpucStreamChar(char** out, const char c) {
    return gpucStreamWrite(out, &c, 1);
}

static size_t gpucStreamSubstring(char** out, const GpucSubstring* substring) {
    return gpucStreamWrite(out, substring->head, substring->length);
}

static size_t gpucStreamString(char** out, const char* s) {
    return gpucStreamWrite(out, s, strlen(s));
}

static size_t gpucStreamLexeme(char** out, const GpucLexeme* lexeme) {
    return gpucStreamWrite(out, lexeme->head, lexeme->length);
}

static size_t gpucStreamFormatV(char** out, const char* format, va_list args) {
    const bool canWrite = (out and *out);

    va_list argsCopy;
    va_copy(argsCopy, args);
    int length =
        canWrite
        ? vsprintf(*out, format, argsCopy)
        : vsnprintf(NULL, 0, format, argsCopy);
    va_end(argsCopy);

    if (length < 0)
        length = 0;

    if (canWrite) {
        *out += length;
        **out = '\0';
    }
    return length;
}

static size_t gpucStreamFormat(char** out, const char* format, ...) {
    va_list args;
    va_start(args, format);
    const size_t length = gpucStreamFormatV(out, format, args);
    va_end(args);
    return length;
}
