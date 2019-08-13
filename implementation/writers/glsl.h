#pragma once
#include "../writer.h"

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteGlslNodeType(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    const GpucNode* const type = node->type;
    const GpucLexeme* const name = type->lexeme;
    const GpucToken token = name->token;
    switch (token) {
        case GpucToken_Bool2:       gpucWriteString("bvec2"); return;
        case GpucToken_Bool3:       gpucWriteString("bvec3"); return;
        case GpucToken_Bool4:       gpucWriteString("bvec4"); return;

        case GpucToken_Float2:      gpucWriteString("vec2"); return;
        case GpucToken_Float3:      gpucWriteString("vec3"); return;
        case GpucToken_Float4:      gpucWriteString("vec4"); return;

        case GpucToken_Float2x2:    gpucWriteString("mat2x2"); return;
        case GpucToken_Float2x3:    gpucWriteString("mat2x3"); return;
        case GpucToken_Float2x4:    gpucWriteString("mat2x4"); return;
        case GpucToken_Float3x2:    gpucWriteString("mat3x2"); return;
        case GpucToken_Float3x3:    gpucWriteString("mat3x3"); return;
        case GpucToken_Float3x4:    gpucWriteString("mat3x4"); return;
        case GpucToken_Float4x2:    gpucWriteString("mat4x2"); return;
        case GpucToken_Float4x3:    gpucWriteString("mat4x3"); return;
        case GpucToken_Float4x4:    gpucWriteString("mat4x4"); return;

        case GpucToken_Int2:        gpucWriteString("ivec2"); return;
        case GpucToken_Int3:        gpucWriteString("ivec3"); return;
        case GpucToken_Int4:        gpucWriteString("ivec4"); return;

        case GpucToken_UInt2:       gpucWriteString("uvec2"); return;
        case GpucToken_UInt3:       gpucWriteString("uvec3"); return;
        case GpucToken_UInt4:       gpucWriteString("uvec4"); return;

        case GpucToken_TEXTURE1D:   gpucWriteString("texture1D"); return;
        case GpucToken_TEXTURE2D:   gpucWriteString("texture2D"); return;
        case GpucToken_TEXTURE3D:   gpucWriteString("texture3D"); return;
        case GpucToken_TEXTURECUBE: gpucWriteString("textureCube"); return;

        default: gpucWriteLexeme(name); return;
    }
}

#define gpucWriteGlslNodeType(node)\
        gpucWriterWriteGlslNodeType(writer, node);

//------------------------------------------------------------------------------

static inline
bool gpucNodeIsGlslUniform(const GpucNode* const node) {
    if (node->semantic == GpucSemantic_GlobalParameterDeclaration)
        return true;

    const GpucNode* const type = node->type;
    const GpucLexeme* const name = type->lexeme;
    const GpucToken token = name->token;
    switch (token) {
        case GpucToken_TEXTURE1D:
        case GpucToken_TEXTURE2D:
        case GpucToken_TEXTURE3D:
        case GpucToken_TEXTURECUBE: return true;

        default: return false;
    }
}

static inline
void gpucTranslateParameterToGlslUniform(
    GpucWriter* const writer,
    const GpucNode* const node,
    unsigned*       const uniformCount)
{
    *uniformCount += 1;
    gpucWriteString("\nuniform ");
    gpucWriteGlslNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    gpucWriteString(";\n\n");
}

static inline
void gpucTranslateParameterToGlslInput(
    GpucWriter*   const writer,
    const GpucNode*   const node,
    const GpucLexeme* const prefix,
    unsigned*         const inputCount)
{
    const GpucLexeme* const name = node->lexeme;
    const GpucNode* const type = node->type;
    if (gpucNodeIsStructure(type)) {
        array_for(const GpucNode*, field, type->children) {
            gpucTranslateParameterToGlslInput(
                writer,
                field,
                name,
                inputCount);
        }
        return;
    }

    char layout[32];
    snprintf(
        layout, sizeof(layout),
        "layout(location = %u) in ",
        *inputCount);
    *inputCount += 1;
    gpucWriteString(layout);

    gpucWriteGlslNodeType(node);
    gpucWriteChar(' ');
    gpucWriteString("_gpuc_in_");
    if (prefix) {
        gpucWriteLexeme(prefix);
        gpucWriteChar('_');
    }
    gpucWriteNodeLexeme(node);
    gpucWriteString(";\n");
}

static inline
void gpucTranslateParameterListToGlslInputs(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    const GpucNode* const parameterList = gpucNodeFindFunctionOrCallParameterList(node);
    array_t(const GpucNode*) const parameters = parameterList->children;
    unsigned inputCount = 0;
    unsigned uniformCount = 0;
    array_for(const GpucNode*, parameter, parameters) {
        if (gpucNodeIsGlslUniform(parameter))
            gpucTranslateParameterToGlslUniform(
                writer,
                parameter,
                &uniformCount);
        else
            gpucTranslateParameterToGlslInput(
                writer,
                parameter,
                NULL,
                &inputCount);
    }
}

//------------------------------------------------------------------------------

static inline
void gpucTranslateReturnTypeToGlslOutput(
    GpucWriter*       const writer,
    const GpucNode*   const node,
    const GpucLexeme* const prefix,
    unsigned*         const outputCount)
{
    char layout[32];
    snprintf(
        layout, sizeof(layout),
        "layout(location = %u) out ",
        *outputCount);
    *outputCount += 1;
    gpucWriteString(layout);

    gpucWriteGlslNodeType(node);
    gpucWriteChar(' ');
    gpucWriteString("_gpuc_out_");
    if (prefix) {
        gpucWriteLexeme(prefix);
        gpucWriteChar('_');
    }
    gpucWriteNodeLexeme(node);
    gpucWriteString(";\n");
}

static inline
void gpucTranslateReturnTypeToGlslOutputs(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    const GpucLexeme* const name = node->lexeme;
    const GpucNode* const type = node->type;
    unsigned outputCount = 0;
    if (gpucNodeIsStructure(type)) {
        array_for(const GpucNode*, field, type->children) {
            gpucTranslateReturnTypeToGlslOutput(
                writer,
                field,
                name,
                &outputCount);
        }
        return;
    }
    gpucTranslateReturnTypeToGlslOutput(
        writer,
        node,
        NULL,
        &outputCount);
}

//------------------------------------------------------------------------------

static inline
void gpucInvokeEntryPointFromGlsl(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    const GpucNode* const parameterList = gpucNodeFindFunctionOrCallParameterList(node);
    array_t(const GpucNode*) const parameters = parameterList->children;
    array_for(const GpucNode*, parameter, parameters) {
        if (gpucNodeIsGlslUniform(parameter))
            continue;

        const GpucNode* const type = parameter->type;
        if (gpucNodeIsStructure(type)) {
            gpucWriteNodeLexeme(type);
            gpucWriteChar(' ');
            gpucWriteNodeLexeme(parameter);
            gpucWriteString(";\n");
            array_for(const GpucNode*, field, type->children) {
                gpucWriteNodeLexeme(parameter);
                gpucWriteChar('.');
                gpucWriteNodeLexeme(field);
                gpucWriteString(" = _gpuc_in_");
                gpucWriteNodeLexeme(parameter);
                gpucWriteChar('_');
                gpucWriteNodeLexeme(field);
                gpucWriteString(";\n");
            }
        }
    }
    gpucWriteChar('\n');

    const GpucNode* const type = node->type;
    gpucWriteNodeLexeme(type);
    gpucWriteString(" result = ");
    gpucWriteNodeLexeme(node);
    gpucWriteChar('(');
    {
        const GpucNode* const parameterList = gpucNodeFindFunctionOrCallParameterList(node);
        array_t(const GpucNode*) const parameters = parameterList->children;
        unsigned parameterCount = 0;
        array_for(const GpucNode*, parameter, parameters) {
            if (parameterCount++) gpucWriteString(", ");
            gpucWriteNodeLexeme(parameter);
        }
    }
    gpucWriteString(");\n\n");

    if (gpucNodeIsStructure(type)) {
        array_for(const GpucNode*, field, type->children) {
            gpucWriteString("_gpuc_out_");
            gpucWriteNodeLexeme(node);
            gpucWriteChar('_');
            gpucWriteNodeLexeme(field);
            gpucWriteString(" = result.");
            gpucWriteNodeLexeme(field);
            gpucWriteString(";\n");
        }
        const bool isVertexStage = writer->stage == GpucStage_Vertex;
        if (isVertexStage and array_length(type->children)) {
            gpucWriteChar('\n');
            const GpucNode* const position = type->children[0];
            gpucWriteString("gl_Position = result.");
            gpucWriteNodeLexeme(position);
            gpucWriteString(";\n");
        }
    } else {
        gpucWriteString("// TODO");
    }
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteGlslFunctionDefinitionNode(
    GpucWriter* const writer,
    const GpucNode* const node)
{
    const GpucLexeme* const name = node->lexeme;
    const GpucToken token = name->token;

    if (not gpucTokenIsEntryPoint(token)) {
        gpucWriterWriteFunctionDefinitionNode(writer, node);
        return;
    }

    if (gpucTokenStage(token) != writer->stage)
        return;

    gpucWriterWriteFunctionDefinitionNode(writer, node);

    gpucTranslateParameterListToGlslInputs(writer, node);
    gpucTranslateReturnTypeToGlslOutputs(writer, node);
    gpucWriteChar('\n');

    gpucWriteString("void main() {\n");
    gpucIndentBegin();
    {
        gpucInvokeEntryPointFromGlsl(writer, node);
    }
    gpucIndentEnd();
    gpucWriteString("}\n\n");
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteGlsl(GpucWriter* const writer)
{
    gpucWriteString("// GLSL 330\n");
    gpucWriteString("#version 330\n\n");

    writer->writeNodeType = gpucWriterWriteGlslNodeType;

    writer->writeNode[GpucSemantic_FunctionDefinition] =
        gpucWriterWriteGlslFunctionDefinitionNode;

    writer->writeNode[GpucSemantic_GlobalParameterDeclaration] =
        gpucWriterWriteVariableDeclarationNode;

    gpucWriterWriteModule(writer);
}

