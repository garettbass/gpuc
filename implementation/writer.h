#pragma once
#include "module.h"

//------------------------------------------------------------------------------

typedef struct GpucWriter GpucWriter;

//------------------------------------------------------------------------------

typedef void (*GpucWriterWriteNode)(GpucWriter* const, const GpucNode* const);

#define GPUC_WRITER_WRITE(SEMANTIC)\
        static inline\
        void gpucWriterWrite##SEMANTIC##Node(\
            GpucWriter*     const writer,\
            const GpucNode* const node)

#define GPUC_WRITER_WRITE_DECLARATION(SEMANTIC)\
        GPUC_WRITER_WRITE(SEMANTIC);
        GPUC_SEMANTICS(GPUC_WRITER_WRITE_DECLARATION)
#undef GPUC_WRITER_WRITE_DECLARATION

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteNodeLexeme(
    GpucWriter*     const writer,
    const GpucNode* const node);

static inline
void gpucWriterWriteNodeTypeLexeme(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    gpucWriterWriteNodeLexeme(writer, node->type);
}

//------------------------------------------------------------------------------

struct GpucWriter {
    GpucModule*         module;
    GpucStage           stage;
    const GpucNode*     stageEntryPoint;
    const GpucNode*     stageInputStruct;
    const GpucNode*     stageOutputStruct;
    GpucWriterWriteNode writeNode[GpucSemanticCount];
    GpucWriterWriteNode writeNodeType;
    unsigned            indentDepth;
    bool                indentPending;
};

static const GpucWriter gpucWriterDefaults = { NULL };

//------------------------------------------------------------------------------

static inline
void gpucWriterSetup(
    GpucWriter* const writer,
    GpucModule* const module,
    const GpucStage   stage)
{
    *writer = gpucWriterDefaults;
    writer->module = module;
    writer->stage  = stage;
    writer->stageEntryPoint = gpucModuleFindStageEntryPoint(module, stage);
    gpucAssert(writer->stageEntryPoint, "expected entry point");
    if (writer->stageEntryPoint)
    {
        const GpucNode* const parameterList =
            gpucNodeFindFunctionOrCallParameterList(writer->stageEntryPoint);
        array_t(const GpucNode*) const parameters = parameterList->children;
        array_for(const GpucNode*, parameter, parameters) {
            if (parameter->semantic == GpucSemantic_VariableDeclaration) {
                gpucAssert(
                    not writer->stageInputStruct,
                    "redundant attributes");
                const GpucNode* const paramType = parameter->type;
                gpucAssert(gpucNodeIsStructure(paramType), "expected struct");
                writer->stageInputStruct = paramType;
            }
        }
        gpucAssert(
            writer->stageInputStruct,
            "expected attributes");

        const GpucNode* const resultType = writer->stageEntryPoint->type;
        gpucAssert(gpucNodeIsStructure(resultType), "expected struct");
        writer->stageOutputStruct = resultType;
    }

    #define GPUC_TRANSLATE(SEMANTIC)\
            writer->writeNode[GpucSemantic_##SEMANTIC] =\
                gpucWriterWrite##SEMANTIC##Node;
    GPUC_SEMANTICS(GPUC_TRANSLATE)
    #undef GPUC_TRANSLATE

    writer->writeNodeType = gpucWriterWriteNodeTypeLexeme;

    const size_t sourceLength = module->source.length;
    array_free(module->translation);
    array_alloc(module->translation, sourceLength * 2, NULL);
}

//------------------------------------------------------------------------------

static inline
void gpucWriterIndentBegin(GpucWriter* const writer) {
    writer->indentDepth += 1;
    writer->indentPending = true;
}

static inline
void gpucWriterIndentEnd(GpucWriter* const writer) {
    assert(writer->indentDepth > 0);
    writer->indentDepth -= 1;
}

#define gpucIndentBegin() gpucWriterIndentBegin(writer)
#define gpucIndentEnd()   gpucWriterIndentEnd(writer)

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteChar(
    GpucWriter* const writer,
    const char        c)
{
    GpucModule* const module = writer->module;
    if (c == '\n') {
        array_append(module->translation, c);
        writer->indentPending = true;
        return;
    }
    if (writer->indentPending) {
        writer->indentPending = false;
        const unsigned indentDepth = writer->indentDepth;
        for (unsigned i = 0; i < indentDepth; ++i) {
            array_append(module->translation, ' ');
            array_append(module->translation, ' ');
            array_append(module->translation, ' ');
            array_append(module->translation, ' ');
        }
    }
    array_append(module->translation, c);
}

#define gpucWriteChar(c) gpucWriterWriteChar(writer, c)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteLexeme(
    GpucWriter*       const writer,
    const GpucLexeme* const lexeme)
{
    const char* const head = lexeme->head;
    const unsigned length = lexeme->length;
    for (unsigned i = 0; i < length; ++i) {
        gpucWriterWriteChar(writer, head[i]);
    }
}

#define gpucWriteLexeme(lexeme) gpucWriterWriteLexeme(writer, lexeme)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteString(
    GpucWriter* const writer,
    const char* const string)
{
    for (const char* p = string; *p; ++p) {
        gpucWriterWriteChar(writer, *p);
    }
}

#define gpucWriteString(string) gpucWriterWriteString(writer, string)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteFormatV(
    GpucWriter* const writer,
    const char* const format,
    va_list           args)
{
    const size_t length = gpucStreamFormatV(NULL, format, args);
    const size_t size = length + sizeof('\0');
    char* const buffer = (char*)calloc(size, sizeof(char));
    char* out = buffer;
    gpucStreamFormatV(&out, format, args);
    for (const char* p = buffer; *p; ++p) {
        gpucWriterWriteChar(writer, *p);
    }
    free(buffer);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteFormat(
    GpucWriter* const writer,
    const char* const format, ...)
{
    va_list args;
    va_start(args, format);
    gpucWriterWriteFormatV(writer, format, args);
    va_end(args);
}

#define gpucWriteFormat(format, ...)\
        gpucWriterWriteFormat(writer, format, __VA_ARGS__)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteNode(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    const GpucSemantic semantic = node->semantic;
    const GpucWriterWriteNode writeNode = writer->writeNode[semantic];
    writeNode(writer, node);
}

#define gpucWriteNode(node)\
        gpucWriterWriteNode(writer, node)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteNodeChildren(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    array_for(const GpucNode*, child, node->children) {
        gpucWriterWriteNode(writer, child);
    }
}

#define gpucWriteNodeChildren(node)\
        gpucWriterWriteNodeChildren(writer, node)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteNodeLexeme(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    gpucWriterWriteLexeme(writer, node->lexeme);
}

#define gpucWriteNodeLexeme(node)\
        gpucWriterWriteNodeLexeme(writer, node)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
void gpucWriterWriteNodeType(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    const GpucWriterWriteNode writeNodeType = writer->writeNodeType;
    writeNodeType(writer, node);
}

#define gpucWriteNodeType(node)\
        gpucWriterWriteNodeType(writer, node)

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteModule(GpucWriter* const writer) {
    gpucWriterWriteNode(writer, writer->module->root);
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteGpuc(GpucWriter* const writer)
{
    gpucWriterWriteModule(writer);
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(None) {
    gpucAssert(false, "invalid node");
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(PrimitiveType) {
    gpucAssert(false, "invalid node");
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(Module) {
    gpucWriteNodeChildren(node);
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(StructDefinition) {
    gpucWriteString("struct ");
    gpucWriteNodeLexeme(node);
    gpucWriteString(" {\n");
    gpucIndentBegin();
    gpucWriteNodeChildren(node);
    gpucIndentEnd();
    gpucWriteString("};\n\n");
}

GPUC_WRITER_WRITE(StructFieldDefinition) {
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    gpucWriteString(";\n");
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(FunctionDeclaration) {
    if (_gpucModuleIsBuiltinSymbol(writer->module, node)) {
        gpucWriteString("// ");
    }
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    gpucWriteNodeChildren(node);
    gpucWriteString(";\n\n");
}

GPUC_WRITER_WRITE(FunctionDefinition) {
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    gpucWriteNodeChildren(node);
    gpucWriteString("\n");
}

GPUC_WRITER_WRITE(FunctionParameterList) {
    gpucWriteChar('(');
    unsigned childCount = 0;
    array_for(const GpucNode*, child, node->children) {
        if (childCount++) gpucWriteString(", ");
        gpucWriteNode(child);
    }
    gpucWriteChar(')');
}

GPUC_WRITER_WRITE(ParameterDeclaration) {
    gpucWriteNodeType(node);
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(GlobalParameterDeclaration) {
    gpucWriteString("param ");
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    gpucWriteString(";\n\n");
}

GPUC_WRITER_WRITE(VariableDeclaration) {
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(EmptyStatement) {
    gpucWriteString(";\n");
}

GPUC_WRITER_WRITE(IfStatement) {
    gpucWriteString("/* TODO: IfStatement */;\n");
}

GPUC_WRITER_WRITE(SwitchStatement) {
    gpucWriteString("/* TODO: SwitchStatement */;\n");
}

GPUC_WRITER_WRITE(WhileStatement) {
    gpucWriteString("/* TODO: WhileStatement */;\n");
}

GPUC_WRITER_WRITE(DoStatement) {
    gpucWriteString("/* TODO: DoStatement */;\n");
}

GPUC_WRITER_WRITE(ForStatement) {
    gpucWriteString("/* TODO: ForStatement */;\n");
}

GPUC_WRITER_WRITE(GotoStatement) {
    gpucWriteString("/* TODO: GotoStatement */;\n");
}

GPUC_WRITER_WRITE(ContinueStatement) {
    gpucWriteString("/* TODO: ContinueStatement */;\n");
}

GPUC_WRITER_WRITE(BreakStatement) {
    gpucWriteString("/* TODO: BreakStatement */;\n");
}

GPUC_WRITER_WRITE(ReturnStatement) {
    gpucWriteNodeLexeme(node);
    if (array_length(node->children)) {
        gpucWriteChar(' ');
        gpucWriteNodeChildren(node);
    }
    gpucWriteString(";\n");
}

GPUC_WRITER_WRITE(CompoundStatement) {
    gpucWriteString(" {\n");
    gpucIndentBegin();
    gpucWriteNodeChildren(node);
    gpucIndentEnd();
    gpucWriteString("}\n");
}

GPUC_WRITER_WRITE(ExpressionStatement) {
    gpucWriteNodeChildren(node);
    gpucWriteString(";\n");
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(AssignmentExpression) {
    gpucWriteNode(node->children[0]);
    gpucWriteString(" = ");
    gpucWriteNode(node->children[1]);
}

GPUC_WRITER_WRITE(PrefixExpression) {
    gpucWriteNodeLexeme(node);
    gpucWriteNode(node->children[0]);
}

GPUC_WRITER_WRITE(PostfixExpression) {
    gpucWriteNode(node->children[0]);
    gpucWriteNodeLexeme(node);
}

GPUC_WRITER_WRITE(NestedExpression) {
    gpucWriteChar('(');
    gpucWriteNodeChildren(node);
    gpucWriteChar(')');
}

GPUC_WRITER_WRITE(MemberExpression) {
    gpucWriteNode(node->children[0]);
    gpucWriteChar('.');
    gpucWriteNode(node->children[1]);
}

GPUC_WRITER_WRITE(SubscriptExpression) {
    gpucWriteString("/* TODO: SubscriptExpression */");
}

GPUC_WRITER_WRITE(CallExpression) {
    gpucWriteNodeLexeme(node);
    gpucWriteNode(node->children[0]);
}

GPUC_WRITER_WRITE(CallParameterList) {
    gpucWriteChar('(');
    unsigned childCount = 0;
    array_for(const GpucNode*, child, node->children) {
        if (childCount++) gpucWriteString(", ");
        gpucWriteNode(child);
    }
    gpucWriteChar(')');
}

GPUC_WRITER_WRITE(TernaryExpression) {
    gpucWriteNode(node->children[0]);
    gpucWriteString(" ? ");
    gpucWriteNode(node->children[1]);
    gpucWriteString(" : ");
    gpucWriteNode(node->children[2]);
}

GPUC_WRITER_WRITE(BinaryExpression) {
    gpucWriteNode(node->children[0]);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    gpucWriteChar(' ');
    gpucWriteNode(node->children[1]);
}

GPUC_WRITER_WRITE(LogicalOrExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(LogicalAndExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(BitwiseOrExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(BitwiseXorExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(BitwiseAndExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(EqualityExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(RelationalExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(ShiftExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(SumExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(ProductExpression) {
    gpucWriterWriteBinaryExpressionNode(writer, node);
}

GPUC_WRITER_WRITE(LiteralExpression) {
    gpucWriteNodeLexeme(node);
}

//------------------------------------------------------------------------------

GPUC_WRITER_WRITE(MemberReference) {
    gpucWriteNodeLexeme(node);
}

GPUC_WRITER_WRITE(ParamReference) {
    gpucWriteNodeLexeme(node);
}

GPUC_WRITER_WRITE(VariableReference) {
    gpucWriteNodeLexeme(node);
}

#include "writers/glsl.h"
#include "writers/hlsl.h"
#include "writers/metal.h"
