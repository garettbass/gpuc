#pragma once
#include <assert.h>
#include "assert.h"
#include "module.h"

//------------------------------------------------------------------------------

static inline
bool gpucModuleAppendIdentifierLexeme(
    GpucModule* const   module,
    const char**      itr,
    const char* const end)
{
    static_assert(GpucTokenCount < 255, "NumGpucTokens < 255");
    static const unsigned char tokens[] = {
        #define GPUC_IDENTIFIER_TOKEN(TOKEN, LITERAL) GpucToken_##TOKEN,
        GPUC_KEYWORD_TOKENS(GPUC_IDENTIFIER_TOKEN)
        GPUC_TYPENAME_TOKENS(GPUC_IDENTIFIER_TOKEN)
        #undef GPUC_IDENTIFIER_TOKEN
        GpucToken_None,
    };

    static const char* const literals[] = {
        #define GPUC_IDENTIFIER_TOKEN(TOKEN, LITERAL) LITERAL,
        GPUC_KEYWORD_TOKENS(GPUC_IDENTIFIER_TOKEN)
        GPUC_TYPENAME_TOKENS(GPUC_IDENTIFIER_TOKEN)
        #undef GPUC_IDENTIFIER_TOKEN
    };

    const char* p = *itr;

    if (gpucSkipChar(&p, end, '_') or gpucSkipAlphaChar(&p, end)) {
        while (gpucSkipChar(&p, end, '_') or gpucSkipAlnumChar(&p, end));

        GpucLexeme lexeme = {
            .head   = *itr,
            .length = (unsigned)(((size_t)p) - ((size_t)*itr)),
            .token  = GpucToken_Identifier,
        };
        *itr = p;

        array_append(module->lexemes, lexeme);

        for (unsigned i = 0; tokens[i]; ++i) {
            const char* const literal = literals[i];
            if (gpucLexemeIsEqualToString(&lexeme, literal)) {
                array_back(module->lexemes).token = (GpucToken)tokens[i];
                break;
            }
        }

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

static inline
bool gpucModuleAppendNumberLexeme(
    GpucModule* const   module,
    const char**      itr,
    const char* const end)
{
    const char* p = *itr;

    if (gpucSkipDigitChar(&p, end)) {
        GpucLexeme lexeme = { .head = *itr };

        while (gpucSkipDigitChar(&p, end));
        lexeme.token = GpucToken_IntLiteral;

        if (gpucSkipChar(&p, end, '.')) {
            while(gpucSkipDigitChar(&p, end));
            lexeme.token = GpucToken_DoubleLiteral;
        }

        if (gpucSkipChar(&p, end, 'e') or gpucSkipChar(&p, end, 'E')) {
            gpucSkipChar(&p, end, '-') or gpucSkipChar(&p, end, '+');
            while(gpucSkipDigitChar(&p, end));
            lexeme.token = GpucToken_DoubleLiteral;
        }

        if (gpucSkipChar(&p, end, 'f') or gpucSkipChar(&p, end, 'F')) {
            lexeme.token = GpucToken_FloatLiteral;
        }

        if (lexeme.token == GpucToken_IntLiteral) {
            if (gpucSkipChar(&p, end, 'u') or gpucSkipChar(&p, end, 'U')) {
                lexeme.token = GpucToken_UIntLiteral;
            }
        }

        bool hasInvalidSuffix = false;
        if (gpucSkipAlnumChar(&p, end)) {
            while (gpucSkipAlnumChar(&p, end));
            hasInvalidSuffix = true;
        }

        lexeme.length = (unsigned)(p - *itr);
        *itr = p;

        if (hasInvalidSuffix) {
            gpucError(&lexeme, "invalid number literal suffix");
        }

        array_append(module->lexemes, lexeme);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

static inline
bool gpucModuleAppendSymbolLexeme(
    GpucModule* const module,
    const char**      itr,
    const char* const end)
{
    static const unsigned char tokens[] = {
        #define GPUC_SYMBOL_TOKEN(TOKEN, LITERAL) GpucToken_##TOKEN,
        GPUC_SYMBOL_TOKENS(GPUC_SYMBOL_TOKEN)
        #undef GPUC_SYMBOL_TOKEN
        GpucToken_None,
    };

    static const char* const literals[] = {
        #define GPUC_SYMBOL_TOKEN(TOKEN, LITERAL) LITERAL,
        GPUC_SYMBOL_TOKENS(GPUC_SYMBOL_TOKEN)
        #undef GPUC_SYMBOL_TOKEN
    };

    const char* p = *itr;

    GpucLexeme lexeme = { .head = *itr };

    for (unsigned i = 0; tokens[i]; ++i) {
        const char* const literal = literals[i];
        if (gpucSkipString(&p, end, literal)) {
            lexeme.length = (unsigned)(p - *itr);
            lexeme.token = (GpucToken)tokens[i];
            *itr = p;
            array_append(module->lexemes, lexeme);
            return true;
        }
    }

    ++p;
    lexeme.length = (unsigned)(p - *itr);
    *itr = p;
    gpucError(&lexeme, "invalid symbol");

    return false;
}

//------------------------------------------------------------------------------

static inline
bool gpucModuleAppendLexeme(
    GpucModule* const   module,
    const char**      itr,
    const char* const end)
{
    gpucSkipNonTokens(itr, end);
    if (*itr < end) {
        return gpucModuleAppendIdentifierLexeme(module, itr, end)
            or gpucModuleAppendNumberLexeme(module, itr, end)
            or gpucModuleAppendSymbolLexeme(module, itr, end);
    }
    return false;
}

//------------------------------------------------------------------------------

static inline
bool gpucModuleLex(GpucModule* module) {
    bool _gpucModuleLexString(
        GpucModule* module,
        const char* itr,
        const char* const end);
    return
        _gpucModuleLexString(
            module,
            module->source.head,
            module->source.head + module->source.length);
}

bool _gpucModuleLexString(
    GpucModule* module,
    const char* itr,
    const char* const end)
{
    while (gpucModuleAppendLexeme(module, &itr, end));
    return array_empty(module->diagnostics);
}
