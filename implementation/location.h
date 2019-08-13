#pragma once
#include <string.h>
#include "assert.h"
#include "lexeme.h"
#include "scan.h"
#include "stream.h"

//------------------------------------------------------------------------------

static inline
unsigned gpucLocationLineDirectiveFileLength(const char* file) {
    unsigned i = 0;
    for (char c = file[i]; c and c != '"' and c != '\n'; c = file[++i]);
    return i;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
unsigned gpucLocationContextLength(
    const char* const itr,
    const char* const end)
{
    const char* p = itr;
    gpucSeekNewLine(&p, end);
    if (p >= itr and p[-1] == '\n') p -= 1;
    return (unsigned)(p - itr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucLocationFromLineDirective(
    GpucLocation*     location,
    const char**      itr,
    const char* const end)
{
    // # line <digit-sequence> ["filename"]
    if (gpucSkipDirectiveName(itr, end, "line")) {
        gpucSkipWhitespace(itr, end);
        unsigned line = 0;
        if (gpucScanUIntBase10(&line, itr, end)) {
            location->line = line;
            gpucSkipWhitespace(itr, end);
            if (gpucSkipChar(itr, end, '"')) {
                location->file.head = *itr;
                location->file.length =
                    gpucLocationLineDirectiveFileLength(*itr);
            }
        }
        return true;
    }
    return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucLocationFromLexeme(
    GpucLocation*              location,
    const GpucSubstring* const file,
    const GpucSubstring* const source,
    const GpucLexeme*    const lexeme)
{
    const char*       itr = source->head;
    const char* const end = itr + source->length;
    // gpucAssert(lexeme->head >= itr, "!!!");
    // gpucAssert(lexeme->head + lexeme->length < end, "!!!");

    location->file.head   = file->head;
    location->file.length = file->length;
    location->site.head   = lexeme->head;
    location->site.length = lexeme->length;
    location->line = 1;
    location->column = 1;
    location->context.head   = source->head;
    location->context.length = gpucLocationContextLength(itr, end);

    while (itr < end and itr < lexeme->head) {
        if (gpucLocationFromLineDirective(location, &itr, end)) {
            continue;
        }
        if (gpucSkipChar(&itr, end, '\n')) {
            location->line += 1;
            location->column = 1;
            location->context.head   = itr;
            location->context.length = gpucLocationContextLength(itr, end);
            continue;
        }
        ++location->column;
        ++itr;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
size_t gpucLocationStringWrite(char** out, const GpucLocation* const location) {
    size_t length = 0;
    length += gpucStreamSubstring(out, &location->file);
    length += gpucStreamFormat(out, ":%u:%u:", location->line, location->column);
    return length;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
size_t gpucLocationStringLength(const GpucLocation* const location) {
    char* out = NULL;
    return gpucLocationStringWrite(&out, location);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
const char* gpucLocationStringAlloc(const GpucLocation* const location) {
    const size_t length = gpucLocationStringLength(location);
    char* const mem = (char*)calloc(length + 1, sizeof(char));
    char* out = mem;
    gpucLocationStringWrite(&out, location);
    return mem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void gpucLocationPrint(const GpucLocation* const location) {
    const char* const mem = gpucLocationStringAlloc(location);
    fputs(mem, stdout);
    free((void*)mem);
}
