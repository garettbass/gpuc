#pragma once
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "token.h"

//------------------------------------------------------------------------------

typedef struct GpucLexeme {
    const char*  head;
    unsigned     length;
    GpucToken    token;
} GpucLexeme;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucLexemeIsEqualToSubstring(
    const GpucLexeme*    const lexeme,
    const GpucSubstring* const substring)
{
    return
        lexeme->length == substring->length
        and not strncmp(lexeme->head, substring->head, substring->length);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucLexemeIsEqualToString(
    const GpucLexeme* const lexeme,
    const char*       const string)
{
    const GpucSubstring substring = {
        .head   = string,
        .length = (unsigned)strlen(string),
    };
    return gpucLexemeIsEqualToSubstring(lexeme, &substring);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucLexemesAreEqual(
    const GpucLexeme* const a,
    const GpucLexeme* const b)
{
    const GpucSubstring substring = {
        .head   = b->head,
        .length = b->length,
    };
    return gpucLexemeIsEqualToSubstring(a, &substring);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const char* gpucLexemeStringAlloc(const GpucLexeme* const lexeme) {
    char* const mem = (char*)calloc(lexeme->length + 1, sizeof(char));
    strncpy(mem, lexeme->head, lexeme->length);
    return mem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucLexemeDebugPrint(const GpucLexeme* const lexeme) {
    const char* const tokenName = gpucTokenName(lexeme->token);
    printf("%-12s \"%.*s\"", tokenName, lexeme->length, lexeme->head);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucLexemeDebugPrintLine(const GpucLexeme* const lexeme) {
    gpucLexemeDebugPrint(lexeme);
    fputc('\n', stdout);
}
