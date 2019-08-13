#pragma once
#include <stdlib.h>
#include "location.h"

//------------------------------------------------------------------------------

static inline
const GpucDiagnostic* gpucDiagnosticAllocV(
    GpucDiagnosticLevel level,
    GpucLocation        location,
    const char* const   format, va_list args)
{
    char* out = NULL;

    const size_t messageLength = gpucStreamFormatV(&out, format, args);

    const size_t size = sizeof(GpucDiagnostic) + messageLength + 1;
    GpucDiagnostic* const diagnostic = (GpucDiagnostic*)calloc(1, size);
    diagnostic->level = level;
    diagnostic->location = location;

    out = diagnostic->message;
    gpucStreamFormatV(&out, format, args);

    return diagnostic;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const GpucDiagnostic* gpucDiagnosticAlloc(
    GpucDiagnosticLevel level,
    GpucLocation        location,
    const char* const   format, ...)
{
    va_list args;
    va_start(args, format);
    return gpucDiagnosticAllocV(level, location, format, args);
    va_end(args);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucDiagnosticFree(const GpucDiagnostic* const diagnostic) {
    gpucAssert(diagnostic, "diagnostic is NULL");
    free((void*)diagnostic);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucDiagnosticArrayDestructor(
    const GpucDiagnostic* const *       itr,
    const GpucDiagnostic* const * const end)
{
    while (itr < end) gpucDiagnosticFree(*itr++);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
size_t gpucDiagnosticStringWrite(
    char** out,
    const GpucDiagnostic* const diagnostic)
{
    static const char* LevelPrefix[] = { " note: ", " warning: ", " error: " };
    const GpucDiagnosticLevel level = diagnostic->level;
    const GpucLocation* const location = &diagnostic->location;
    const char* const message = diagnostic->message;
    size_t length = 0;
    length += gpucStreamSubstring(out, &location->file);
    length += gpucStreamFormat(out, ":%u:%u:", location->line, location->column);
    length += gpucStreamString(out, LevelPrefix[level]);
    length += gpucStreamString(out, message);
    length += gpucStreamChar(out, '\n');
    length += gpucStreamSubstring(out, &location->context);
    length += gpucStreamChar(out, '\n');
    for (unsigned i = 1; i < location->column; ++i) {
        length += gpucStreamChar(out, ' ');
    }
    for (unsigned i = 0; i < location->site.length; ++i) {
        length += gpucStreamChar(out, '^');
    }
    return length;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
size_t gpucDiagnosticStringLength(
    const GpucDiagnostic* const diagnostic)
{
    char* out = NULL;
    return gpucDiagnosticStringWrite(&out, diagnostic);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
const char* gpucDiagnosticStringAlloc(
    const GpucDiagnostic* const diagnostic)
{
    const size_t length = gpucDiagnosticStringLength(diagnostic);
    char* const mem = (char*)calloc(length + 1, sizeof(char));
    char* out = mem;
    gpucDiagnosticStringWrite(&out, diagnostic);
    return mem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
void gpucDiagnosticPrint(const GpucDiagnostic* const diagnostic) {
    const char* const mem = gpucDiagnosticStringAlloc(diagnostic);
    fputs(mem, stdout);
    free((void*)mem);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
void gpucDiagnosticPrintLine(const GpucDiagnostic* const diagnostic) {
    const char* const mem = gpucDiagnosticStringAlloc(diagnostic);
    puts(mem);
    free((void*)mem);
}
