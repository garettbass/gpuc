#pragma once

#define GPUC_SEMANTICS(GPUC_SEMANTIC)\
        GPUC_SEMANTIC(None)\
        /**/\
        GPUC_SEMANTIC(PrimitiveType)\
        /**/\
        GPUC_SEMANTIC(Module)\
        /**/\
        GPUC_SEMANTIC(StructDefinition)\
        GPUC_SEMANTIC(StructFieldDefinition)\
        /**/\
        GPUC_SEMANTIC(FunctionDeclaration)\
        GPUC_SEMANTIC(FunctionDefinition)\
        GPUC_SEMANTIC(FunctionParameterList)\
        GPUC_SEMANTIC(ParameterDeclaration)\
        /**/\
        GPUC_SEMANTIC(GlobalParameterDeclaration)\
        GPUC_SEMANTIC(VariableDeclaration)\
        /**/\
        GPUC_SEMANTIC(EmptyStatement)\
        GPUC_SEMANTIC(IfStatement)\
        GPUC_SEMANTIC(SwitchStatement)\
        GPUC_SEMANTIC(WhileStatement)\
        GPUC_SEMANTIC(DoStatement)\
        GPUC_SEMANTIC(ForStatement)\
        GPUC_SEMANTIC(GotoStatement)\
        GPUC_SEMANTIC(ContinueStatement)\
        GPUC_SEMANTIC(BreakStatement)\
        GPUC_SEMANTIC(ReturnStatement)\
        GPUC_SEMANTIC(CompoundStatement)\
        GPUC_SEMANTIC(ExpressionStatement)\
        /**/\
        GPUC_SEMANTIC(AssignmentExpression)\
        GPUC_SEMANTIC(PrefixExpression)\
        GPUC_SEMANTIC(PostfixExpression)\
        GPUC_SEMANTIC(NestedExpression)\
        GPUC_SEMANTIC(MemberExpression)\
        GPUC_SEMANTIC(SubscriptExpression)\
        GPUC_SEMANTIC(CallExpression)\
        GPUC_SEMANTIC(CallParameterList)\
        GPUC_SEMANTIC(TernaryExpression)\
        GPUC_SEMANTIC(LogicalOrExpression)\
        GPUC_SEMANTIC(LogicalAndExpression)\
        GPUC_SEMANTIC(BitwiseOrExpression)\
        GPUC_SEMANTIC(BitwiseXorExpression)\
        GPUC_SEMANTIC(BitwiseAndExpression)\
        GPUC_SEMANTIC(EqualityExpression)\
        GPUC_SEMANTIC(RelationalExpression)\
        GPUC_SEMANTIC(ShiftExpression)\
        GPUC_SEMANTIC(SumExpression)\
        GPUC_SEMANTIC(ProductExpression)\
        GPUC_SEMANTIC(LiteralExpression)\
        /**/\
        GPUC_SEMANTIC(MemberReference)\
        GPUC_SEMANTIC(ParamReference)\
        GPUC_SEMANTIC(VariableReference)\
        /**/

typedef enum GpucSemantic {
    #define GPUC_SEMANTIC_ENUM(SEMANTIC)\
            GpucSemantic_##SEMANTIC,
    GPUC_SEMANTICS(GPUC_SEMANTIC_ENUM)
    #undef GPUC_SEMANTIC_ENUM
    GpucSemanticCount,
} GpucSemantic;

static inline const char* gpucSemanticName(GpucSemantic semantic) {
    switch (semantic) {
        #define GPUC_SEMANTIC_NAME(SEMANTIC)\
                case GpucSemantic_##SEMANTIC: return #SEMANTIC;
        GPUC_SEMANTICS(GPUC_SEMANTIC_NAME)
        #undef GPUC_SEMANTIC_NAME
        default: return "<invalid GpucSemantic>";
    }
}

static inline bool gpucSemanticIsFunction(GpucSemantic semantic) {
    switch (semantic) {
        case GpucSemantic_FunctionDeclaration:
        case GpucSemantic_FunctionDefinition:
            return true;
        default: return false;
    }
}

static inline bool gpucSemanticIsType(GpucSemantic semantic) {
    switch (semantic)  {
        case GpucSemantic_PrimitiveType:
        case GpucSemantic_StructDefinition:
            return true;
        default: return false;
    }
}

static inline bool gpucSemanticIsVariable(GpucSemantic semantic) {
    switch (semantic) {
        case GpucSemantic_GlobalParameterDeclaration:
        case GpucSemantic_VariableDeclaration:
            return true;
        default: return false;
    }
}
