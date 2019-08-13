#pragma once
#include "../writer.h"

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalVertexAttributeSemantic(
    GpucWriter* const writer,
    const int         attributeIndex)
{
    gpucWriteFormat(" [[attribute(%i)]]", attributeIndex);
}

static inline
void gpucWriterWriteMetalFragmentAttributeSemantic(
    GpucWriter* const writer,
    const int attributeIndex)
{
    if (attributeIndex == 0)
        gpucWriteString(" [[position]]");
}

static inline
void gpucWriterWriteMetalTargetSemantic(
    GpucWriter* const writer,
    const int targetIndex)
{
    // TODO
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalStructFieldDefinitionNode(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    const GpucNode* const structDefinition = node->parent;
    if (structDefinition == writer->stageInputStruct) {
        switch (writer->stage) {
            case GpucStage_Vertex: {
                const int index = gpucNodeGetChildIndex(node);
                gpucWriterWriteMetalVertexAttributeSemantic(writer, index);
            } break;
            case GpucStage_Fragment: {
                const int index = gpucNodeGetChildIndex(node);
                gpucWriterWriteMetalFragmentAttributeSemantic(writer, index);
            } break;
            default: {
                assert(false);
            } break;
        }
    }
    else
    if (structDefinition == writer->stageOutputStruct) {
        switch (writer->stage) {
            case GpucStage_Vertex: {
                const int index = gpucNodeGetChildIndex(node);
                gpucWriterWriteMetalFragmentAttributeSemantic(writer, index);
            } break;
            case GpucStage_Fragment: {
                const int index = gpucNodeGetChildIndex(node);
                gpucWriterWriteMetalTargetSemantic(writer, index);
            } break;
            default: {
                assert(false);
            } break;
        }
    }
    gpucWriteString(";\n");
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalEntryPointStageInParameter(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    gpucWriteString("const gpuc::");
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    gpucWriteString(" [[stage_in]]");
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalEntryPointConstantParameter(
    GpucWriter*     const writer,
    const GpucNode* const node,
    unsigned*       const bufferIndex,
    unsigned*       const textureIndex,
    unsigned*       const samplerIndex)
{
    switch (node->type->lexeme->token) {
        case GpucToken_Identifier: {
            gpucWriteString("constant const gpuc::");
            gpucWriteNodeType(node);
            gpucWriteString("& ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[buffer(%i)]]", (*bufferIndex)++);
        } break;
        case GpucToken_TEXTURE1D: {
            gpucWriteString("texture1d<float> ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[texture(%i)]]", (*textureIndex)++);
        } break;
        case GpucToken_TEXTURE2D: {
            gpucWriteString("texture2d<float> ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[texture(%i)]]", (*textureIndex)++);
        } break;
        case GpucToken_TEXTURE3D: {
            gpucWriteString("texture3d<float> ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[texture(%i)]]", (*textureIndex)++);
        } break;
        case GpucToken_TEXTURECUBE: {
            gpucWriteString("textureCube<float> ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[texture(%i)]]", (*textureIndex)++);
        } break;
        case GpucToken_DEPTH2D: {
            gpucWriteString("depth2d<float> ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[texture(%i)]]", (*textureIndex)++);
        } break;
        case GpucToken_DEPTHCUBE: {
            gpucWriteString("depthCube<float> ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[texture(%i)]]", (*textureIndex)++);
        } break;
        case GpucToken_SAMPLER: {
            gpucWriteString("sampler ");
            gpucWriteNodeLexeme(node);
            gpucWriteFormat(" [[sampler(%i)]]", (*samplerIndex)++);
        } break;
        default: {
            gpucAssert(false, "unsupported resource type");
        } break;
    }
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalEntryPoint(GpucWriter* const writer)
{
    const GpucNode* const entryPoint = writer->stageEntryPoint;

    gpucWriteNodeType(entryPoint);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(entryPoint);
    gpucWriteChar('(');
    {
        unsigned uniformIndex = 0;
        unsigned parameterCount = 0;
        const GpucNode* const parameterList = entryPoint->children[0];
        array_t(const GpucNode*) const parameters = parameterList->children;
        array_for(const GpucNode*, parameter, parameters) {
            if (parameterCount++) gpucWriteString(", ");
            switch (parameter->semantic) {
                case GpucSemantic_VariableDeclaration:
                    gpucWriteString("const ");
                    gpucWriteNodeType(parameter);
                    gpucWriteChar(' ');
                    gpucWriteNodeLexeme(parameter);
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
    gpucWriteString(") const");
    const GpucNode* const functionBody = entryPoint->children[1];
    gpucWriteNode(functionBody);
    gpucWriteChar('\n');
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalFunctionDefinitionNode(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    const GpucLexeme* const name = node->lexeme;
    const GpucToken token = name->token;

    if (gpucTokenIsEntryPoint(token)) {
        if (node == writer->stageEntryPoint) {
            gpucWriterWriteMetalEntryPoint(writer);
        }
        return;
    }

    gpucWriterWriteFunctionDefinitionNode(writer, node);
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalGlobalParameterDeclarationNode(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    switch (node->type->lexeme->token) {
        case GpucToken_Identifier: {
            gpucWriteString("constant const ");
            gpucWriteNodeType(node);
            gpucWriteString("& ");
        } break;
        case GpucToken_TEXTURE1D: {
            gpucWriteString("const texture1d<float> ");
        } break;
        case GpucToken_TEXTURE2D: {
            gpucWriteString("const texture2d<float> ");
        } break;
        case GpucToken_TEXTURE3D: {
            gpucWriteString("const texture3d<float> ");
        } break;
        case GpucToken_TEXTURECUBE: {
            gpucWriteString("const textureCube<float> ");
        } break;
        case GpucToken_DEPTH2D: {
            gpucWriteString("const depth2d<float> ");
        } break;
        case GpucToken_DEPTHCUBE: {
            gpucWriteString("const depthCube<float> ");
        } break;
        case GpucToken_SAMPLER: {
            gpucWriteString("const sampler ");
        } break;
        default: {
            gpucAssert(false, "unsupported resource type");
        } break;
    }
    gpucWriteNodeLexeme(node);
    gpucWriteString(";\n\n");
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteMetalModule(GpucWriter* const writer) {
    writer->writeNode[GpucSemantic_StructFieldDefinition] =
        gpucWriterWriteMetalStructFieldDefinitionNode;

    writer->writeNode[GpucSemantic_FunctionDefinition] =
        gpucWriterWriteMetalFunctionDefinitionNode;

    writer->writeNode[GpucSemantic_GlobalParameterDeclaration] =
        gpucWriterWriteMetalGlobalParameterDeclarationNode;

    gpucWriteString(
        "#include <metal_stdlib>\n"
        "#include <simd/simd.h>\n"
        "\n"
        "using namespace metal;\n"
        "\n"
        "float4 sample(const texture2d<float> t, float2 uv) {\n"
        "    constexpr sampler s(filter::nearest);\n"
        "    return t.sample(s, uv);\n"
        "}\n"
        "\n"
        "float4 sample(const texture2d<float> t, float2 uv, const sampler s) {\n"
        "    return t.sample(s, uv);\n"
        "}\n"
        "\n"
    );

    gpucWriteString("struct gpuc {\n\n");
    gpucIndentBegin();
    gpucWriterWriteModule(writer);
    gpucIndentEnd();
    gpucWriteString("};\n\n");

    const GpucNode* const entryPoint = writer->stageEntryPoint;
    const GpucNode* const parameterList = entryPoint->children[0];
    array_t(const GpucNode*) const parameters = parameterList->children;
    array_t(const GpucNode*) const globals = writer->module->root->children;

    switch (writer->stage) {
        case GpucStage_Vertex:
            gpucWriteString("vertex ");
            break;
        case GpucStage_Fragment:
            gpucWriteString("fragment ");
            break;
        default:
            assert(false);
            break;
    }
    gpucWriteString("gpuc::");
    gpucWriteNodeType(entryPoint);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(entryPoint);
    gpucWriteString("(\n");
    {
        gpucIndentBegin();
        unsigned bufferIndex = 0;
        unsigned textureIndex = 0;
        unsigned samplerIndex = 0;
        unsigned parameterCount = 0;
        array_for(const GpucNode*, global, globals) {
            switch (global->semantic) {
                case GpucSemantic_GlobalParameterDeclaration:
                    if (parameterCount++) gpucWriteString(",\n");
                    gpucWriterWriteMetalEntryPointConstantParameter(
                        writer,
                        global,
                        &bufferIndex,
                        &textureIndex,
                        &samplerIndex);
                    break;
                default:
                    break;
            }
        }
        array_for(const GpucNode*, parameter, parameters) {
            if (parameterCount++) gpucWriteString(",\n");
            switch (parameter->semantic) {
                case GpucSemantic_VariableDeclaration:
                    gpucWriteString("const gpuc::");
                    gpucWriteNodeType(parameter);
                    gpucWriteChar(' ');
                    gpucWriteNodeLexeme(parameter);
                    gpucWriteString(" [[stage_in]]");
                    break;
                default:
                    assert(false);
                    break;
            }
        }
        gpucIndentEnd();
    }
    gpucWriteString("\n) {\n");
    gpucIndentBegin();
        gpucWriteString("return gpuc{");
        {
            unsigned parameterCount = 0;
            array_for(const GpucNode*, global, globals) {
                switch (global->semantic) {
                    case GpucSemantic_GlobalParameterDeclaration:
                        if (parameterCount++) gpucWriteString(", ");
                        gpucWriteNodeLexeme(global);
                        break;
                    default:
                        break;
                }
            }
        }
        gpucWriteString("}.");
        gpucWriteNodeLexeme(entryPoint);
        gpucWriteString("(");
        {
            unsigned parameterCount = 0;
            array_for(const GpucNode*, parameter, parameters) {
                switch (parameter->semantic) {
                    case GpucSemantic_VariableDeclaration:
                        if (parameterCount++) gpucWriteString(", ");
                        gpucWriteNodeLexeme(parameter);
                        break;
                    default:
                        assert(false);
                        break;
                }
            }
        }
        gpucWriteString(");\n");
    gpucIndentEnd();
    gpucWriteString("}\n\n");

}
