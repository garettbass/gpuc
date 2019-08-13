#pragma once
#include "../writer.h"

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteHlslAttributeSemantic(
    GpucWriter* const writer,
    const int         attributeIndex)
{
    if (attributeIndex == 0)
        gpucWriteString(" : SV_Position");
    else
        gpucWriteFormat(" : Attribute%i", attributeIndex);
}

static inline
void gpucWriterWriteHlslTargetSemantic(
    GpucWriter* const writer,
    const int targetIndex)
{
    gpucWriteFormat(" : SV_Target%i", targetIndex);
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteHlslStructFieldDefinitionNode(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    gpucWriteNodeType(node);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(node);
    const GpucNode* const structDefinition = node->parent;
    if (structDefinition == writer->stageInputStruct) {
        const int index = gpucNodeGetChildIndex(node);
        gpucWriterWriteHlslAttributeSemantic(writer, index);
    }
    else
    if (structDefinition == writer->stageOutputStruct) {
        switch (writer->stage)
        {
            case GpucStage_Vertex: {
                const int index = gpucNodeGetChildIndex(node);
                gpucWriterWriteHlslAttributeSemantic(writer, index);
            } break;
            case GpucStage_Fragment: {
                const int index = gpucNodeGetChildIndex(node);
                gpucWriterWriteHlslTargetSemantic(writer, index);
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
void gpucWriterWriteHlslConstantBufferDeclaration(
    GpucWriter*     const writer,
    const GpucNode* const uniformDeclaration,
    unsigned        const cbufferRegisterIndex)
{
    const GpucNode* const type = uniformDeclaration->type;
    gpucWriteString("cbuffer _gpuc_cbuffer_");
    gpucWriteNodeLexeme(type);
    gpucWriteFormat(" : register(b%u) {\n", cbufferRegisterIndex);
    gpucIndentBegin();
    if (gpucNodeIsStructure(type)) {
        const GpucNodeArray fields = type->children;
        array_for(const GpucNode*, field, fields) {
            gpucWriteNodeType(field);
            gpucWriteString(" _gpuc_cbuffer_");
            gpucWriteNodeLexeme(uniformDeclaration);
            gpucWriteChar('_');
            gpucWriteNodeLexeme(field);
            gpucWriteString(";\n");
        }
    } else {
        gpucWriteNodeType(uniformDeclaration);
        gpucWriteChar(' ');
        gpucWriteNodeLexeme(uniformDeclaration);
        gpucWriteString(";\n");
    }
    gpucIndentEnd();
    gpucWriteString("};\n\n");
}

static inline
void gpucWriterWriteHlslConstantBufferDeclarations(
    GpucWriter*              const writer,
    array_t(const GpucNode*) const parameters)
{
    unsigned cbufferRegisterIndex = 0;
    array_for(const GpucNode*, parameter, parameters) {
        if (parameter->semantic == GpucSemantic_GlobalParameterDeclaration) {
            gpucWriterWriteHlslConstantBufferDeclaration(
                writer,
                parameter,
                cbufferRegisterIndex);
            cbufferRegisterIndex += 1;
        }
    }
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteHlslUniformAssignment(
    GpucWriter*     const writer,
    const GpucNode* const uniformDeclaration)
{
    gpucWriteNodeType(uniformDeclaration);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(uniformDeclaration);
    gpucWriteString(";\n");
    const GpucNode* const type = uniformDeclaration->type;
    if (gpucNodeIsStructure(type)) {
        const GpucNodeArray fields = type->children;
        array_for(const GpucNode*, field, fields) {
            gpucWriteNodeLexeme(uniformDeclaration);
            gpucWriteChar('.');
            gpucWriteNodeLexeme(field);
            gpucWriteString(" = _gpuc_cbuffer_");
            gpucWriteNodeLexeme(uniformDeclaration);
            gpucWriteChar('_');
            gpucWriteNodeLexeme(field);
            gpucWriteString(";\n");
        }
    }
}

static inline
void gpucWriterWriteHlslUniformAssignments(
    GpucWriter*              const writer,
    array_t(const GpucNode*) const parameters)
{
    array_for(const GpucNode*, parameter, parameters) {
        if (parameter->semantic == GpucSemantic_GlobalParameterDeclaration) {
            gpucWriterWriteHlslUniformAssignment(writer, parameter);
        }
    }
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteHlslEntryPoint(GpucWriter* const writer)
{
    const GpucNode* const entryPoint = writer->stageEntryPoint;
    const GpucNode* const parameterList = entryPoint->children[0];
    const GpucNode* const functionBody = entryPoint->children[1];
    array_t(const GpucNode*) const parameters = parameterList->children;

    gpucWriterWriteHlslConstantBufferDeclarations(writer, parameters);

    gpucWriteNodeType(entryPoint);
    gpucWriteChar(' ');
    gpucWriteNodeLexeme(entryPoint);
    gpucWriteChar('(');
    array_for(const GpucNode*, parameter, parameters) {
        if (parameter->semantic == GpucSemantic_VariableDeclaration) {
            gpucWriterWriteVariableDeclarationNode(writer, parameter);
            break;
        }
    }
    gpucWriteString(") {\n");
    gpucIndentBegin();
    gpucWriterWriteHlslUniformAssignments(writer, parameters);
    gpucWriteNodeChildren(functionBody);
    gpucIndentEnd();
    gpucWriteString("};\n\n");
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteHlslFunctionDefinitionNode(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    const GpucLexeme* const name = node->lexeme;
    const GpucToken token = name->token;

    if (gpucTokenIsEntryPoint(token)) {
        if (node == writer->stageEntryPoint) {
            gpucWriterWriteHlslEntryPoint(writer);
        }
        return;
    }

    gpucWriterWriteFunctionDefinitionNode(writer, node);
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteHlslProductExpressionNode(
    GpucWriter*     const writer,
    const GpucNode* const node)
{
    const GpucNode* const lhs = node->children[0];
    const GpucNode* const rhs = node->children[1];
    const GpucNode* const lhsType = lhs->type;
    const GpucNode* const rhsType = rhs->type;
    if (gpucNodeIsMatrixType(lhsType) or gpucNodeIsMatrixType(rhsType))
    {
        gpucWriteString("mul(");
        gpucWriteNode(lhs);
        gpucWriteString(", ");
        gpucWriteNode(rhs);
        gpucWriteString(")");
        return;
    }
    gpucWriterWriteProductExpressionNode(writer, node);
}

//------------------------------------------------------------------------------

static inline
void gpucWriterWriteHlslModule(GpucWriter* const writer)
{
    writer->writeNode[GpucSemantic_StructFieldDefinition] =
        gpucWriterWriteHlslStructFieldDefinitionNode;

    writer->writeNode[GpucSemantic_FunctionDefinition] =
        gpucWriterWriteHlslFunctionDefinitionNode;

    writer->writeNode[GpucSemantic_GlobalParameterDeclaration] =
        gpucWriterWriteVariableDeclarationNode;

    writer->writeNode[GpucSemantic_ProductExpression] =
        gpucWriterWriteHlslProductExpressionNode;

    gpucWriterWriteModule(writer);
}
