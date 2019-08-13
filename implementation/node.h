#pragma once
#include <stdlib.h>
#include "../gpuc.h"
#include "array/array.h"
#include "lexeme.h"
#include "semantic.h"

typedef struct GpucNode GpucNode;

typedef array_t(const GpucNode*) GpucNodeArray;

struct GpucNode {
    GpucSemantic      semantic;
    const GpucLexeme* lexeme;
    const GpucNode*   type;
    const GpucNode*   parent;
    GpucNodeArray     children;
};

const GpucNode GpucNodeDefaults = { GpucSemantic_None };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
GpucNode* gpucNodeAllocCopy(const GpucNode* node) {
    GpucNode* copy = (GpucNode*)calloc(1, sizeof(GpucNode));
    memcpy(copy, node, sizeof(*node));
    return copy;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
GpucNode* gpucNodeAlloc(
    const GpucSemantic      semantic,
    const GpucLexeme* const lexeme,
    const GpucNode*   const type)
{
    GpucNode node = GpucNodeDefaults;
    node.semantic = semantic;
    node.lexeme = lexeme;
    node.type = type;
    return gpucNodeAllocCopy(&node);
}

#define gpucNodeAlloc_(semantic, lexeme, type) \
        gpucNodeAlloc(GpucSemantic_##semantic, lexeme, type)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucNodeFree(const GpucNode* const node) {
    gpucAssert(node, "");
    array_free(node->children);
    free((void*)node);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucNodeArrayDestructor(
    const GpucNode**       itr,
    const GpucNode** const end)
{
    while (itr < end) gpucNodeFree(*itr++);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const GpucNode* gpucNodeConcreteType(const GpucNode* node) {
    gpucAssert(node, "");
    const GpucNode* type = node->type;
    if (type) {
        while (type->type != type) {
            gpucAssert(type->type, "");
            type = type->type;
        }
    }
    return type;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeHasSemantic(
    const GpucNode* const node,
    const GpucSemantic    semantic)
{
    return node and node->semantic == semantic;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeHasLexeme(
    const GpucNode*   const node,
    const GpucLexeme* const lexeme)
{
    return node and gpucLexemesAreEqual(node->lexeme, lexeme);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsAssignable(const GpucNode* const node) {
    gpucAssert(node, "");
    switch (node->semantic) {
        case GpucSemantic_PrefixExpression:
        case GpucSemantic_MemberReference:
        case GpucSemantic_VariableReference:
            return true;
        case GpucSemantic_NestedExpression:
            return gpucNodeIsAssignable(node->children[0]);
        default:
            return false;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsFunctionDeclaration(const GpucNode* const node) {
    return gpucNodeHasSemantic(node, GpucSemantic_FunctionDeclaration);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsFunctionDefinition(const GpucNode* const node) {
    return gpucNodeHasSemantic(node, GpucSemantic_FunctionDefinition);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsFunction(const GpucNode* const node) {
    return gpucNodeIsFunctionDeclaration(node)
        or gpucNodeIsFunctionDefinition(node);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsStageEntryPoint(
    const GpucNode* const node,
    const GpucStage stage)
{
    return
        (gpucNodeIsFunctionDefinition(node))
        and
        (gpucTokenStage(node->lexeme->token) == stage);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsCallExpression(const GpucNode* const node) {
    return gpucNodeHasSemantic(node, GpucSemantic_CallExpression);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsFunctionOrCall(const GpucNode* const node) {
    return gpucNodeIsFunction(node)
        or gpucNodeIsCallExpression(node);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsType(const GpucNode* const node) {
    return node and gpucSemanticIsType(node->semantic);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsMatrixType(const GpucNode* const node) {
    return gpucNodeIsType(node) and gpucTokenIsMatrix(node->lexeme->token);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsVariable(const GpucNode* const node) {
    return node and gpucSemanticIsVariable(node->semantic);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeIsStructure(const GpucNode* const node) {
    return node and node->semantic == GpucSemantic_StructDefinition;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodeAddChild(GpucNode* parent, GpucNode* child) {
    gpucAssert(parent, "");
    if (not child)
        return false;

    if (not parent->children)
        array_alloc(parent->children, 6, gpucNodeArrayDestructor);

    child->parent = parent;
    array_append(parent->children, child);
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const int gpucNodeGetChildIndex(const GpucNode* const node) {
    const GpucNode* const parent = node->parent;
    if (parent)
    {
        int index = 0;
        array_t(const GpucNode* const) children = parent->children;
        array_for(const GpucNode*, child, children) {
            if (child == node)
                return index;
            index += 1;
        }
        gpucFatal("node not found in parent->children");
    }
    return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const GpucNode* gpucNodeFindChildBySemantic(
    const GpucNode* const parent,
    const GpucSemantic    semantic)
{
    gpucAssert(parent, "");
    if (parent->children) {
        const GpucNode** itr = array_begin(parent->children);
        const GpucNode** end = array_end(parent->children);
        for (; itr < end; ++itr) {
            const GpucNode* child = *itr;
            if (gpucNodeHasSemantic(child, semantic)) {
                return child;
            }
        }
    }
    return NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const GpucNode* gpucNodeFindChildByLexeme(
    const GpucNode*   const parent,
    const GpucLexeme* const lexeme)
{
    gpucAssert(parent, "");
    gpucAssert(lexeme, "");
    if (parent->children) {
        const GpucNode** itr = array_begin(parent->children);
        const GpucNode** end = array_end(parent->children);
        for (; itr < end; ++itr) {
            const GpucNode* child = *itr;
            if (gpucNodeHasLexeme(child, lexeme)) {
                return child;
            }
        }
    }
    return NULL;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const GpucNode* gpucNodeFindFunctionOrCallParameterList(
    const GpucNode* const node)
{
    gpucAssert(node, "");
    const GpucSemantic FParameterList = GpucSemantic_FunctionParameterList;
    const GpucSemantic CParameterList = GpucSemantic_CallParameterList;

    const GpucNode* list = NULL;
    (list = gpucNodeFindChildBySemantic(node, FParameterList)) or
    (list = gpucNodeFindChildBySemantic(node, CParameterList));
    return list;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodesHaveSameType(
    const GpucNode* const a,
    const GpucNode* const b)
{
    return gpucNodeConcreteType(a) == gpucNodeConcreteType(b);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool gpucNodesHaveSameFunctionOrCallParameterTypes(
    const GpucNode* const f,
    const GpucNode* const g)
{
    gpucAssert(gpucNodeIsFunctionOrCall(f), "");
    gpucAssert(gpucNodeIsFunctionOrCall(g), "");

    const GpucNode* const fParameterList = gpucNodeFindFunctionOrCallParameterList(f);
    const GpucNode* const gParameterList = gpucNodeFindFunctionOrCallParameterList(g);
    gpucAssert(fParameterList, "");
    gpucAssert(gParameterList, "");

    array_t(const GpucNode*) const fParameters = fParameterList->children;
    array_t(const GpucNode*) const gParameters = gParameterList->children;

    const size_t fParameterCount = array_length(fParameters);
    const size_t gParameterCount = array_length(gParameters);

    if (fParameterCount != gParameterCount)
        return false;

    for (size_t i = 0; i < fParameterCount; ++i) {
        const GpucNode* fParameter = fParameters[i];
        const GpucNode* gParameter = gParameters[i];
        if (not gpucNodesHaveSameType(fParameter, gParameter))
            return false;
    }

    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
size_t gpucNodeParameterTypeStringWrite(
    char** out,
    array_t(const GpucNode*) parameters)
{
    unsigned parameterCount = 0;
    size_t length = 0;
    length += gpucStreamChar(out, '(');
    array_for(const GpucNode*, parameter, parameters) {
        if (parameterCount++) {
            length += gpucStreamChar(out, ',');
        }
        const GpucNode* parameterType = parameter->type;
        const GpucLexeme* parameterTypeName = parameterType->lexeme;
        length += gpucStreamLexeme(out, parameterTypeName);
    }
    length += gpucStreamChar(out, ')');
    return length;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const char* gpucNodeParameterTypeStringAlloc(const GpucNode* node) {
    gpucAssert(gpucNodeIsFunctionOrCall(node), "");
    gpucAssert(node->type, "");

    const GpucNode* const parameterList = gpucNodeFindFunctionOrCallParameterList(node);
    gpucAssert(parameterList, "");

    char* out = NULL;

    array_t(const GpucNode*) const parameters = parameterList->children;
    const size_t length = gpucNodeParameterTypeStringWrite(&out, parameters);

    char* const mem = (char*)calloc(length + 1, sizeof(char));
    out = mem;

    gpucNodeParameterTypeStringWrite(&out, parameters);

    return mem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
size_t gpucNodeFunctionSignatureStringWrite(
    char** out,
    const GpucLexeme* returnTypeName,
    const GpucLexeme* functionName,
    array_t(const GpucNode*) parameters)
{
    size_t length = 0;
    length += gpucStreamLexeme(out, returnTypeName);
    length += gpucStreamChar(out, ' ');
    length += gpucStreamLexeme(out, functionName);
    length += gpucNodeParameterTypeStringWrite(out, parameters);
    return length;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
const char* gpucNodeFunctionSignatureStringAlloc(const GpucNode* node) {
    gpucAssert(gpucNodeIsFunctionOrCall(node), "");
    gpucAssert(node->type, "");

    const GpucLexeme* returnTypeName = node->type->lexeme;
    const GpucLexeme* functionName = node->lexeme;
    const GpucNode* parameterList = gpucNodeFindFunctionOrCallParameterList(node);
    gpucAssert(parameterList, "");

    char* out = NULL;

    const size_t length = gpucNodeFunctionSignatureStringWrite(
        &out,
        returnTypeName,
        functionName,
        parameterList->children);

    char* const mem = (char*)calloc(length + 1, sizeof(char));
    out = mem;

    gpucNodeFunctionSignatureStringWrite(
        &out,
        returnTypeName,
        functionName,
        parameterList->children);

    return mem;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucNodeDebugPrint(const GpucNode* const node) {
    const char* const semanticName = gpucSemanticName(node->semantic);
    const GpucLexeme* const lexeme = node->lexeme;
    if (lexeme) {
        printf("%s \"%.*s\"", semanticName, lexeme->length, lexeme->head);
    } else {
        fputs(semanticName, stdout);
    }
    const GpucNode* const type = node->type;
    if (type) {
        const GpucLexeme* const typeName = type->lexeme;
        printf(" : %.*s", typeName->length, typeName->head);
        if (gpucNodeIsFunction(node)) {
            const char* const parameterString =
                gpucNodeParameterTypeStringAlloc(node);
            fputs(parameterString, stdout);
            free((void*)parameterString);
        }
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void gpucNodeDebugPrintLine(const GpucNode* const node) {
    gpucNodeDebugPrint(node);
    fputc('\n', stdout);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucNodeDebugPrintTreeLines(
    const GpucNode* const root,
    const GpucNode* const node,
    const int depth)
{
    const bool isRoot = node == root;
    if (isRoot)
        return;

    const GpucNode* const parent = node->parent;
    if (parent) {
        gpucNodeDebugPrintTreeLines(root, parent, depth + 1);

        const GpucNode* const lastNode = array_back(parent->children);
        const bool isLastNode = node == lastNode;
        if (isLastNode) {
            if (depth) {
                fputs("   ", stdout);
                return;
            }
            fputs(" \u2514\u2574", stdout);
            return;
        }

        if (depth) {
            fputs(" \u2502 ", stdout);
            return;
        }

        fputs(" \u251C\u2574", stdout);
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucNodeDebugPrintTreeInternal(
    const GpucNode* const root,
    const GpucNode* const node,
    const int rootIndentDepth,
    const int depth)
{
    for (int i = 0; i < rootIndentDepth; ++i) {
        fputs("    ", stdout);
    }

    gpucNodeDebugPrintTreeLines(root, node, 0);

    gpucNodeDebugPrintLine(node);

    if (array_length(node->children)) {
        const int childDepth = depth + 1;
        array_for(const GpucNode*, child, node->children) {
            gpucNodeDebugPrintTreeInternal(
                root,
                child,
                rootIndentDepth,
                childDepth);
        }
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucNodeDebugPrintTree(
    const GpucNode* const node,
    const int rootIndentDepth)
{
    gpucNodeDebugPrintTreeInternal(
        node,
        node,
        rootIndentDepth,
        0);
}
