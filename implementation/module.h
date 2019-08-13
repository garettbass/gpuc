#pragma once
#include <assert.h>
#include <iso646.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../gpuc.h"
#include "array/array.h"
#include "diagnostic.h"
#include "node.h"

//------------------------------------------------------------------------------

struct GpucModule {
    GpucSubstring                  file;
    GpucSubstring                  source;
    GpucNode*                      root;
    array_t(const GpucDiagnostic*) diagnostics;
    array_t(GpucLexeme)            lexemes;
    array_t(const GpucNode*)       symbols;
    array_t(unsigned)              scopes;
    array_t(char)                  translation;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
GpucModule*  gpucModuleAlloc(
    const char* file,
    const char* source)
{
    GpucModule* const module = (GpucModule*)calloc(1, sizeof(GpucModule));

    const size_t sourceLength = strlen(source);

    file = file ? file : "?";
    module->file.head     = file;
    module->file.length   = (unsigned)strlen(file);
    module->source.head   = source;
    module->source.length = (unsigned)sourceLength;
    module->root          = gpucNodeAlloc_(Module, NULL, NULL);

    array_alloc(module->diagnostics, 4, gpucDiagnosticArrayDestructor);
    array_alloc(module->lexemes,     4 + sourceLength / 4,  NULL);
    array_alloc(module->symbols,     4 + sourceLength / 16, NULL);
    array_alloc(module->scopes,      4 + sourceLength / 32, NULL);

    void
    _gpucModuleBeginScope(GpucModule* module);
    _gpucModuleBeginScope(module); // global scope

    void
    _gpucModuleDefinePrimitiveTypes(GpucModule* module);
    _gpucModuleDefinePrimitiveTypes(module);

    void
    _gpucModuleDeclareBuiltinFunctions(GpucModule* module);
    _gpucModuleDeclareBuiltinFunctions(module);

    _gpucModuleBeginScope(module); // module scope

    return module;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
void gpucModuleFree(GpucModule* const module) {
    gpucAssert(module, "");
    gpucNodeFree(module->root);
    array_free(module->diagnostics);
    array_free(module->lexemes);
    array_free(module->symbols);
    array_free(module->scopes);
    array_free(module->translation);
    free(module);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
void gpucModulePrintDebug(const GpucModule* const module) {
    fputs("lexemes: [", stdout);
    if (array_length(module->lexemes)) {
        fputc('\n', stdout);
        array_for(GpucLexeme, lexeme, module->lexemes) {
            fputs("    ", stdout);
            gpucLexemeDebugPrintLine(&lexeme);
        }
    }
    puts("]");
    fputs("symbols: [", stdout);
    if (array_length(module->symbols)) {
        fputc('\n', stdout);
        array_for(const GpucNode*, symbol, module->symbols) {
            gpucNodeDebugPrintTree(symbol, 1);
        }
    }
    puts("]");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
void gpucModulePrintDiagnostics(const GpucModule* const module) {
    array_for(const GpucDiagnostic*, diagnostic, module->diagnostics) {
        gpucDiagnosticPrintLine(diagnostic);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
void gpucModulePrintLexemes(const GpucModule* const module) {
    array_for(GpucLexeme, lexeme, module->lexemes) {
        gpucLexemeDebugPrintLine(&lexeme);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
void gpucModulePrintAST(const GpucModule* const module) {
    gpucNodeDebugPrintTree(module->root, 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*public*/
const GpucDiagnostic* gpucModuleGetDiagnostic(
    const GpucModule* module,
    unsigned index)
{
    if (index < array_length(module->diagnostics)) {
        return module->diagnostics[index];
    }
    return NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void gpucModuleAppendDiagnostic(
    GpucModule*         const module,
    const GpucDiagnosticLevel level,
    const GpucLexeme*   const lexeme,
    const char*         const format, ...)
{
    GpucLocation location = {0};
    gpucLocationFromLexeme(
        &location,
        &module->file,
        &module->source,
        lexeme);

    va_list args;
    va_start(args, format);
    const GpucDiagnostic* const diagnostic =
        gpucDiagnosticAllocV(level, location, format, args);
    va_end(args);
    array_append(module->diagnostics, diagnostic);
}

#define gpucAppendDiagnostic(level, lexeme, /*format, args*/...) \
        gpucModuleAppendDiagnostic(\
            module,\
            GpucDiagnosticLevel_##level,\
            lexeme,\
            __VA_ARGS__)

#define gpucNote(lexeme, /*format, args*/...) \
        gpucAppendDiagnostic(Note, lexeme, __VA_ARGS__)

#define gpucWarning(lexeme, /*format, args*/...) \
        gpucAppendDiagnostic(Warning, lexeme, __VA_ARGS__)

#define gpucError(lexeme, /*format, args*/...) \
        gpucAppendDiagnostic(Error, lexeme, __VA_ARGS__)

//------------------------------------------------------------------------------

void
_gpucModuleBeginScope(GpucModule* module) {
    const unsigned symbolOffset = (unsigned)array_length(module->symbols);
    array_append(module->scopes, symbolOffset);
}

#define gpucBeginScope() _gpucModuleBeginScope(module)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void gpucModuleEndScope(GpucModule* module) {
    const unsigned symbolOffset = array_back(module->scopes);
    array_resize(module->symbols, symbolOffset);
    array_pop(module->scopes);
}

#define gpucEndScope() gpucModuleEndScope(module)

//------------------------------------------------------------------------------

static bool gpucModuleDefineType(GpucModule* module, const GpucNode* type) {
    gpucAssert(gpucNodeIsType(type), "expected type");

    const GpucLexeme* const name = type->lexeme;

    array_for_reverse(const GpucNode*, symbol, module->symbols) {
        const GpucLexeme* const symbolName = symbol->lexeme;

        if (not gpucLexemesAreEqual(name, symbolName))
            continue;

        gpucError(name, "redefinition of '%.*s'", name->length, name->head);
        gpucNote(symbolName, "previous definition is here");
        return false;
    }

    array_append(module->symbols, type);
    return true;
}

#define gpucDefineType(type) gpucModuleDefineType(module, type)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void
_gpucModuleDefinePrimitiveTypes(GpucModule* module) {
    gpucAssert(array_empty(module->symbols), "before all other symbols");

    #define GPUC_DEFINE_PRIMITIVE_TYPE(TOKEN, LITERAL)\
            static const GpucLexeme gpucLexeme_##TOKEN = {\
                .head   = LITERAL,\
                .length = sizeof(LITERAL) - 1,\
                .token  = GpucToken_##TOKEN,\
            };\
            static const GpucNode gpucNode_##TOKEN = {\
                .semantic = GpucSemantic_PrimitiveType,\
                .lexeme   = &gpucLexeme_##TOKEN,\
                .type     = &gpucNode_##TOKEN,\
            };\
            gpucModuleDefineType(module, &gpucNode_##TOKEN);
    GPUC_TYPENAME_TOKENS(GPUC_DEFINE_PRIMITIVE_TYPE)
    #undef GPUC_DEFINE_PRIMITIVE_TYPE
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static bool gpucModuleDeclareFunction(
    GpucModule*     const module,
    const GpucNode* const function)
{
    gpucAssert(array_length(module->scopes) <= 2, "parameter or module scope only");
    gpucAssert(gpucNodeIsFunctionDeclaration(function), "expected function");

    const GpucLexeme* const name = function->lexeme;

    array_for_reverse(const GpucNode*, symbol, module->symbols) {
        const GpucLexeme* const symbolName = symbol->lexeme;

        if (not gpucLexemesAreEqual(name, symbolName))
            continue;

        if (gpucNodeIsFunction(symbol)) {

            if (gpucNodesHaveSameFunctionOrCallParameterTypes(function, symbol)
                and not gpucNodesHaveSameType(function, symbol))
            {
                gpucError(name, "declaration differs only in return type");
                if (gpucNodeIsFunctionDeclaration(symbol)) {
                    gpucNote(symbolName, "previous declaration is here");
                } else {
                    gpucNote(symbolName, "previous definition is here");
                }
                return false;
            }

        } else {
            gpucError(name, "redefinition of '%.*s'", name->length, name->head);
            gpucNote(symbolName, "previous definition is here");
            return false;
        }

        // NOTE: multiple identical function declarations are OK
    }

    array_append(module->symbols, function);
    return true;
}

#define gpucDeclareFunction(function)\
        gpucModuleDeclareFunction(module, function)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void
_gpucModuleDeclareBuiltinFunctions(GpucModule* module) {
    static const char builtin_functions[] = GPUC(
        float4 sample(texture2d, float2);
        float4 sample(texture2d, float2, sampler);
    );

    bool _gpucModuleLexString(
        GpucModule* module,
        const char* itr,
        const char* const end);

    const bool did_lex_builtin_functions =
        _gpucModuleLexString(
            module,
            builtin_functions,
            builtin_functions + sizeof(builtin_functions) - 1);
    if (not did_lex_builtin_functions) {
        gpucModulePrintDiagnostics(module);
    }

    gpucAssert(did_lex_builtin_functions, "failed to lex builtin functions");
}

static bool _gpucModuleIsBuiltinSymbol(
    const GpucModule* const module,
    const GpucNode*   const symbol)
{
    return
        symbol->lexeme->head < module->source.head ||
        symbol->lexeme->head > module->source.head + module->source.length;
}

static bool _gpucModuleIsUserDefinedSymbol(
    const GpucModule* const module,
    const GpucNode*   const symbol)
{
    return
        symbol->lexeme->head > module->source.head &&
        symbol->lexeme->head < module->source.head + module->source.length;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static bool gpucModuleDefineFunction(
    GpucModule* const       module,
    const GpucNode* const function)
{
    gpucAssert(array_length(module->scopes) <= 2, "parameter or module scope only");
    gpucAssert(gpucNodeIsFunctionDefinition(function), "expected function definition");

    const GpucLexeme* const name = function->lexeme;

    array_for_reverse(const GpucNode*, symbol, module->symbols) {
        const GpucLexeme* const symbolName = symbol->lexeme;

        if (not gpucLexemesAreEqual(name, symbolName))
            continue;

        if (gpucNodeIsFunctionDeclaration(symbol)) {

            if (gpucNodesHaveSameFunctionOrCallParameterTypes(function, symbol)
                and not gpucNodesHaveSameType(function, symbol))
            {
                gpucError(name, "definition differs only in return type");
                gpucNote(symbolName, "previous declaration is here");
                return false;
            }

        } else {
            gpucError(name, "redefinition of '%.*s'", name->length, name->head);
            gpucNote(symbolName, "previous definition is here");
            return false;
        }
    }

    array_append(module->symbols, function);
    return true;
}

#define gpucDefineFunction(function) gpucModuleDefineFunction(module, function)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static bool gpucModuleDefineVariable(
    GpucModule*       const module,
    const GpucNode* const variable)
{
    gpucAssert(gpucNodeIsVariable(variable), "expected variable");

    const GpucLexeme* const name = variable->lexeme;

    const unsigned scope = array_back(module->scopes);

    const GpucNode**       ritr = array_rbegin(module->symbols);
    const GpucNode** const rend = array_rend(module->symbols) + scope;
    for (; ritr > rend; --ritr) {
        const GpucNode* const symbol = *ritr;
        const GpucLexeme* const symbolName = symbol->lexeme;

        if (gpucLexemesAreEqual(name, symbolName)) {
            gpucError(name, "redefinition of '%.*s'", name->length, name->head);
            gpucNote(symbolName, "previous definition is here");
            return false;
        }
    }

    array_append(module->symbols, variable);
    return true;
}

#define gpucDefineVariable(variable) gpucModuleDefineVariable(module, variable)

//------------------------------------------------------------------------------

static const GpucNode* gpucModuleFindSymbol(
    const GpucModule* const module,
    const GpucLexeme* const name)
{
    array_for_reverse(const GpucNode*, symbol, module->symbols) {
        const GpucLexeme* const symbolName = symbol->lexeme;

        if (gpucLexemesAreEqual(symbolName, name))
            return symbol;
    }
    return NULL;
}

#define gpucFindSymbol(name) gpucModuleFindSymbol(module, name)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const GpucNode* gpucModuleGetPrimitiveType(
    const GpucModule* const module,
    const char*       const name)
{
    array_for(const GpucNode*, symbol, module->symbols) {
        if (symbol->semantic == GpucSemantic_PrimitiveType) {
            const GpucLexeme* const symbolName = symbol->lexeme;
            if (gpucLexemeIsEqualToString(symbolName, name))
                return symbol;
        }
    }
    puts(name);
    gpucFatal("primitive type not found");
    return NULL;
}

#define gpucGetPrimitiveType(name) gpucModuleGetPrimitiveType(module, name)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const GpucNode* gpucModuleRequireType(
    GpucModule*       const module,
    const GpucLexeme* const name)
{
    const GpucNode* const symbol = gpucModuleFindSymbol(module, name);

    if (not symbol) {
        gpucError(name, "undeclared identifier");
        return NULL;
    }

    if (not gpucNodeIsType(symbol)) {
        gpucError(name, "identifier is not a type");
        gpucNote(symbol->lexeme, "see definition here");
        return NULL;
    }

    return symbol;
}

#define gpucRequireType(name) gpucModuleRequireType(module, name)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const GpucNode* gpucModuleRequireFunction(
    GpucModule*       const module,
    const GpucNode* const invocation)
{
    gpucAssert(gpucNodeIsCallExpression(invocation), "");

    const GpucLexeme* const name = invocation->lexeme;

    unsigned overloadCount = 0;

    array_for_reverse(const GpucNode*, symbol, module->symbols) {
        const GpucLexeme* const symbolName = symbol->lexeme;
        if (not gpucLexemesAreEqual(name, symbolName))
            continue;

        if (not gpucNodeIsFunction(symbol)) {
            gpucError(name, "identifier is not a function");
            gpucNote(symbolName, "see definition here");
            return NULL;
        }

        if (gpucNodesHaveSameFunctionOrCallParameterTypes(invocation, symbol))
            return symbol;

        overloadCount += 1;
    }

    if (overloadCount) {
        const char* const parameterTypeString =
            gpucNodeParameterTypeStringAlloc(invocation);
        gpucError(name,
            "no instance of function "
            "matches the argument list %s",
            name, parameterTypeString);
        free((void*)parameterTypeString);

        array_for(const GpucNode*, symbol, module->symbols) {
            const GpucLexeme* const symbolName = symbol->lexeme;
            if (gpucNodeIsFunction(symbol) and
                gpucLexemesAreEqual(name, symbolName))
                gpucNote(symbolName, "see definition here");
        }
    } else {
        gpucError(name, "identifier is undefined");
    }
    return NULL;
}

#define gpucRequireFunction(invocation) \
        gpucModuleRequireFunction(module, invocation)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const GpucNode* gpucModuleFindEnclosingFunction(
    GpucModule* const module)
{
    array_for_reverse(const GpucNode*, symbol, module->symbols) {
        if (gpucNodeIsFunctionDefinition(symbol))
            return symbol;
    }
    return NULL;
}

#define gpucFindEnclosingFunction() \
        gpucModuleFindEnclosingFunction(module)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const GpucNode* gpucModuleFindStageEntryPoint(
    GpucModule* const module,
    const GpucStage   stage)
{
    array_for_reverse(const GpucNode*, symbol, module->symbols) {
        if (gpucNodeIsStageEntryPoint(symbol, stage))
            return symbol;
    }
    return NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static const GpucNode* gpucModuleRequireVariable(
    GpucModule*       const module,
    const GpucLexeme* const name)
{
    const GpucNode* const symbol = gpucModuleFindSymbol(module, name);

    if (not symbol) {
        gpucError(name, "undeclared identifier");
        return NULL;
    }

    if (not gpucNodeIsVariable(symbol)) {
        gpucError(name, "identifier is not a variable");
        gpucNote(symbol->lexeme, "see definition here");
        return NULL;
    }

    return symbol;
}

#define gpucRequireVariable(name) gpucModuleRequireVariable(module, name)

//------------------------------------------------------------------------------

#include "module.lex.h"
#include "module.parse.h"
#include "tests.h"
#include "writer.h"

/*public*/
bool gpucModuleCompile(GpucModule* module) {
    return gpucModuleLex(module) and gpucModuleParse(module);
}


//------------------------------------------------------------------------------

const char* gpucModuleGetTranslation(
    GpucModule*  const module,
    const GpucStage    stage,
    const GpucLanguage language)
{
    GpucWriter writer;
    gpucWriterSetup(&writer, module, stage);
    switch (language) {
        case GpucLanguage_GPUC:
            gpucWriterWriteGpuc(&writer);
            break;
        case GpucLanguage_GLSL_330:
            gpucWriterWriteGlsl(&writer);
            break;
        case GpucLanguage_HLSL_11:
            gpucWriterWriteHlslModule(&writer);
            break;
        case GpucLanguage_Metal_1:
            gpucWriterWriteMetalModule(&writer);
            break;
        default:
            gpucWriterWriteGpuc(&writer);
            break;
    }

    // trim trailing whitespace
    while (
        array_size(module->translation) and
        isspace(array_back(module->translation)))
        array_pop(module->translation);

    array_append(module->translation, '\n');

    // append nul terminator
    array_append(module->translation, '\0');

    return module->translation;
}
