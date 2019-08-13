#pragma once
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------

static inline
void _gpucFatal(const char* file, const int line, const char* msg) {
    printf("%s:%i: %s\n", file, line, msg);
    exit(1);
}

#define gpucFatal(msg) _gpucFatal(__FILE__, __LINE__, msg)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define gpucAssert(expr, msg) \
        ((expr)||(gpucFatal("assert("#expr") failed: " msg),0))
