#pragma once
#include "module.h"

//------------------------------------------------------------------------------

enum { GpucParseRulesArrayLength = 8 };

typedef struct GpucParseRules {
    const GpucParseRule array[GpucParseRulesArrayLength];
} GpucParseRules;

#define GpucParseRules(...)\
        static const GpucParseRules gpucParseRules = {{ __VA_ARGS__ }}

//------------------------------------------------------------------------------

static bool gpucModuleParseTest(
    GpucModule*             const module,
    const GpucParseRules* const rules)
{
    const GpucLexeme*       itr = array_begin(module->lexemes);
    const GpucLexeme* const end = array_end(module->lexemes);
    const GpucNode*   const structType = gpucGetPrimitiveType("void");
    for (unsigned i = 0; i < GpucParseRulesArrayLength; ++i) {
        const GpucParseRule rule = rules->array[i];
        if (not rule) break;
        GpucNode* const node =
            gpucModuleParseNode(module, &itr, end, structType, rule);
        if (not node) return false;
        gpucNodeAddChild(module->root, node);
        if (array_length(module->diagnostics)) return false;
    }
    return true;
}

//------------------------------------------------------------------------------

static bool gpucParseTest(
    const char*           const source,
    const GpucParseRules* const rules)
{
    GpucModule* const module = gpucModuleAlloc("test", source);
    const bool passed =
        gpucModuleLex(module) and
        gpucModuleParseTest(module, rules);
    if (not passed) {
        gpucModulePrintDiagnostics(module);
        gpucModulePrintDebug(module);
    }
    gpucModuleFree(module);
    return passed;
}

//------------------------------------------------------------------------------

/*public*/
int gpucTests(void) {
    #define gpucParseTest_(source) \
        failureCount += !gpucParseTest(source, &gpucParseRules)
    int failureCount = 0;

    {
        GpucParseRules(GpucParseRule_StructDefinition);
        gpucParseTest_("struct Foo {};");
    }

    {
        GpucParseRules(
            GpucParseRule_VariableStatement,
            GpucParseRule_SumExpression,
        );
        gpucParseTest_("int a; a + a");
    }

    {
        GpucParseRules(
            GpucParseRule_VariableStatement,
            GpucParseRule_ExpressionStatement,
        );
        gpucParseTest_("int a; a + a;");
    }

    return failureCount;
    #undef gpucParseTest_
}
