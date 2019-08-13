#pragma once
#include <ctype.h>
#include <iso646.h>
#include <stdbool.h>
#include <string.h>

static bool gpucIsChar(
    const char* const itr,
    const char* const end,
    const char        c)
{
    if (itr < end) {
        return *itr == c;
    }
    return false;
}

static bool gpucIsCharType(
    const char* const itr,
    const char* const end,
    int (*ischar)(int))
{
    if (itr < end) {
        return ischar(*itr);
    }
    return false;
}

static bool gpucIsString(
    const char* const itr,
    const char* const end,
    const char*       s)
{
    if (itr < end) {
        for (const char* p = itr; p < end and *s and *s == *p; ++p, ++s);
        return *s == '\0';
    }
    return false;
}

static bool gpucSkipChar(
    const char**      itr,
    const char* const end,
    const char        c)
{
    if (gpucIsChar(*itr, end, c)) {
        *itr += 1;
        return true;
    }
    return false;
}

static bool gpucSkipCharType(
    const char**      itr,
    const char* const end,
    int (*ischar)(int))
{
    if (gpucIsCharType(*itr, end, ischar)) {
        *itr += 1;
        return true;
    }
    return false;
}

#define gpucSkipAlphaChar(itr, end) gpucSkipCharType(itr, end, isalpha)
#define gpucSkipAlnumChar(itr, end) gpucSkipCharType(itr, end, isalnum)
#define gpucSkipDigitChar(itr, end) gpucSkipCharType(itr, end, isdigit)
#define gpucSkipSpaceChar(itr, end) gpucSkipCharType(itr, end, isspace)

static bool gpucSkipString(
    const char**      itr,
    const char* const end,
    const char*       s)
{
    if (gpucIsString(*itr, end, s)) {
        *itr += strlen(s);
        return true;
    }
    return false;
}

static bool gpucSkipIdentifier(
    const char**      itr,
    const char* const end,
    const char*       s)
{
    const char* p = *itr;
    if (gpucSkipString(&p, end, s)) {
        if (p < end and (*p == '_' or isalnum(*p))) {
            return false;
        }
        *itr = p;
        return true;
    }
    return false;
}

//static bool gpucSeekChar(
//    const char**      itr,
//    const char* const end,
//    const char        c)
//{
//    for (const char* p = *itr; p < end; ++p) {
//        if (gpucIsChar(p, end, c)) {
//            *itr = p;
//            return true;
//        }
//    }
//    return false;
//}

static bool gpucSeekString(
    const char**      itr,
    const char* const end,
    const char* const s)
{
    for (const char* p = *itr; p < end; ++p) {
        if (gpucIsString(p, end, s)) {
            *itr = p;
            return true;
        }
    }
    return false;
}

static bool gpucSkipLineContinuation(const char** itr, const char* const end) {
    return gpucSkipString(itr, end, "\\\n");
}

static bool gpucSeekNewLine(const char** itr, const char* const end) {
    if (*itr < end) {
        const char* p = *itr;
        for (char c = *p; p < end and c != '\n';) {
            if (not gpucSkipLineContinuation(&p, end)) {
                c = *++p;
            }
        }
        gpucSkipChar(&p, end, '\n');
        *itr = p;
        return true;
    }
    return false;
}

static bool gpucSkipBlockComment(
    const char**      itr,
    const char* const end)
{
    if (gpucSkipString(itr, end, "/*")) {
        if (gpucSeekString(itr, end, "*/"))
            gpucSkipString(itr, end, "*/");
        else
            *itr = end;
        return true;
    }
    return false;
}

static bool gpucSkipLineComment(
    const char**      itr,
    const char* const end)
{
    const char* p = *itr;
    if (gpucSkipString(&p, end, "//")) {
        gpucSeekNewLine(&p, end);
        *itr = p;
        return true;
    }
    return false;
}

static bool gpucIsWhitespace(
    const char*       itr,
    const char* const end)
{
    return
        gpucSkipSpaceChar(&itr, end) or
        gpucSkipLineContinuation(&itr, end) or
        gpucSkipBlockComment(&itr, end) or
        gpucSkipLineComment(&itr, end);
}

static bool gpucSkipWhitespace(
    const char**      itr,
    const char* const end)
{
    if (gpucSkipSpaceChar(itr, end) or
        gpucSkipLineContinuation(itr, end) or
        gpucSkipBlockComment(itr, end) or
        gpucSkipLineComment(itr, end))
    {
        while (
            gpucSkipSpaceChar(itr, end) or
            gpucSkipLineContinuation(itr, end) or
            gpucSkipBlockComment(itr, end) or
            gpucSkipLineComment(itr, end)
        );
        return true;
    }
    return false;
}

static bool gpucIsDirective(const char* itr, const char* const end) {
    return gpucSkipChar(&itr, end, '#');
}

static bool gpucSkipDirectiveName(
    const char**      itr,
    const char* const end,
    const char* const directive)
{
    const char* p = *itr;
    if (gpucSkipChar(&p, end, '#')) {
        gpucSkipWhitespace(&p, end);
        if (gpucSkipIdentifier(&p, end, directive)) {
            *itr = p;
            return true;
        }
    }
    return false;
}

static bool gpucSkipDirective(
    const char**      itr,
    const char* const end)
{
    return gpucIsDirective(*itr, end) and gpucSeekNewLine(itr, end);
}

static bool gpucSkipNonTokens(const char** itr, const char* const end) {
    if (gpucIsWhitespace(*itr, end) or gpucIsDirective(*itr, end)) {
        while (gpucSkipWhitespace(itr, end) or gpucSkipDirective(itr, end));
        return true;
    }
    return false;
}

static bool gpucScanUIntBase10(
    unsigned*         value,
    const char**      itr,
    const char* const end)
{
    unsigned result = 0;
    const char* p = *itr;
    char c = *p;
    if (isdigit(c)) {
        for (; p < end and isdigit(c); c = *++p) {
            const unsigned digit = c - '\0';
            result *= 10;
            result += digit;
        }
        *value = result;
        *itr = p;
        return true;
    }
    return false;
}
