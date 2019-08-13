#pragma once
#include "module.h"

//------------------------------------------------------------------------------

#define GPUC_PARSE_RULES(GPUC_PARSE_RULE)\
        GPUC_PARSE_RULE(TopLevelDeclaration)\
        GPUC_PARSE_RULE(StructDefinition)\
        GPUC_PARSE_RULE(StructFieldDefinition)\
        GPUC_PARSE_RULE(Function)\
        GPUC_PARSE_RULE(FunctionParameter)\
        GPUC_PARSE_RULE(GlobalParameterDeclaration)\
        GPUC_PARSE_RULE(VariableDeclaration)\
        GPUC_PARSE_RULE(CompoundStatement)\
        GPUC_PARSE_RULE(CompoundStatementInScope)\
        GPUC_PARSE_RULE(Statement)\
        GPUC_PARSE_RULE(EmptyStatement)\
        GPUC_PARSE_RULE(IfStatement)\
        GPUC_PARSE_RULE(SwitchStatement)\
        GPUC_PARSE_RULE(WhileStatement)\
        GPUC_PARSE_RULE(DoStatement)\
        GPUC_PARSE_RULE(ForStatement)\
        GPUC_PARSE_RULE(GotoStatement)\
        GPUC_PARSE_RULE(ContinueStatement)\
        GPUC_PARSE_RULE(BreakStatement)\
        GPUC_PARSE_RULE(ReturnStatement)\
        GPUC_PARSE_RULE(VariableStatement)\
        GPUC_PARSE_RULE(ExpressionStatement)\
        GPUC_PARSE_RULE(Expression)\
        GPUC_PARSE_RULE(VariableExpression)\
        GPUC_PARSE_RULE(AssignmentExpression)\
        GPUC_PARSE_RULE(PrefixExpression)\
        GPUC_PARSE_RULE(MemberReference)\
        GPUC_PARSE_RULE(MemberExpression)\
        GPUC_PARSE_RULE(MemberSubmemberExpression)\
        GPUC_PARSE_RULE(MemberSubscriptExpression)\
        GPUC_PARSE_RULE(MemberPostfixExpression)\
        GPUC_PARSE_RULE(SubscriptExpression)\
        GPUC_PARSE_RULE(CallOrPostfixExpression)\
        GPUC_PARSE_RULE(CallExpression)\
        GPUC_PARSE_RULE(PostfixExpression)\
        GPUC_PARSE_RULE(PrimaryExpression)\
        GPUC_PARSE_RULE(LiteralExpression)\
        GPUC_PARSE_RULE(VariableReference)\
        GPUC_PARSE_RULE(NestedExpression)\
        GPUC_PARSE_RULE(TernaryExpression)\
        GPUC_PARSE_RULE(LogicalOrExpression)\
        GPUC_PARSE_RULE(LogicalAndExpression)\
        GPUC_PARSE_RULE(BitwiseOrExpression)\
        GPUC_PARSE_RULE(BitwiseXorExpression)\
        GPUC_PARSE_RULE(BitwiseAndExpression)\
        GPUC_PARSE_RULE(EqualityExpression)\
        GPUC_PARSE_RULE(RelationalExpression)\
        GPUC_PARSE_RULE(ShiftExpression)\
        GPUC_PARSE_RULE(SumExpression)\
        GPUC_PARSE_RULE(ProductExpression)\

typedef GpucNode*(*GpucParseRule)(
    GpucModule*         const module,
    const GpucLexeme**      itr,
    const GpucLexeme* const end,
    const GpucNode*   const structType);

#define GPUC_PARSE_RULE_DEFINITION(NAME)\
        static inline\
        GpucNode* GpucParseRule_##NAME(\
            GpucModule*         const module,\
            const GpucLexeme**      itr,\
            const GpucLexeme* const end,\
            const GpucNode*   const structType)

#define GPUC_PARSE_RULE_DECLARATION(NAME)\
        GPUC_PARSE_RULE_DEFINITION(NAME);
GPUC_PARSE_RULES(GPUC_PARSE_RULE_DECLARATION)
#undef GPUC_PARSE_RULE_DECLARATION

static inline
const char* gpucParseRuleName(const GpucParseRule rule) {
    #define GPUC_PARSE_RULE_NAME(name)\
            if (rule == GpucParseRule_##name)\
                return "GpucParseRule_"#name;
    GPUC_PARSE_RULES(GPUC_PARSE_RULE_NAME)
    #undef GPUC_PARSE_RULE_NAME

    return "<invalid GpucParseRule>";
}

//------------------------------------------------------------------------------

static inline GpucNode* gpucModuleParseNode(
    GpucModule*       const module,
    const GpucLexeme**      itr,
    const GpucLexeme* const end,
    const GpucNode*   const structType,
    const GpucParseRule     rule)
{
    const GpucLexeme* p = *itr;
    GpucNode* const node = rule(module, &p, end, structType);
    if (node) *itr = p;
    return node;
}

#define gpucParseNode_(rule) \
        gpucModuleParseNode(module, itr, end, structType, GpucParseRule_##rule)

#define gpucParseNodeInStruct_(structType, rule) \
        gpucModuleParseNode(module, itr, end, structType, GpucParseRule_##rule)

//------------------------------------------------------------------------------

static inline
bool gpucModuleParse(GpucModule* module) {
    const GpucLexeme*       itr = array_begin(module->lexemes);
    const GpucLexeme* const end = array_end(module->lexemes);
    const GpucNode*   const structType = gpucGetPrimitiveType("void");
    const GpucParseRule     rule = GpucParseRule_TopLevelDeclaration;
    GpucNode* node = NULL;
    while ((node = gpucModuleParseNode(module, &itr, end, structType, rule))) {
        gpucNodeAddChild(module->root, node);
    }
    return array_empty(module->diagnostics);
}

//------------------------------------------------------------------------------

static inline
const GpucLexeme* gpucPeek(
    const GpucLexeme*      itr,
    const GpucLexeme* const end)
{
    if (itr < end) {
        return itr;
    }
    return NULL;
}

static inline
bool gpucPeekToken(
    const GpucLexeme*       itr,
    const GpucLexeme* const end,
    const GpucToken         token)
{
    if (itr < end) {
        return itr->token == token;
    }
    return false;
}

static inline
bool gpucPeekTokenF(
    const GpucLexeme*       itr,
    const GpucLexeme* const end,
    bool (*const func)(GpucToken))
{
    if (itr < end) {
        return func(itr->token);
    }
    return false;
}

static inline
const GpucLexeme* gpucRead(
    const GpucLexeme**      itr,
    const GpucLexeme* const end)
{
    if (*itr < end) {
        const GpucLexeme* lexeme = *itr;
        ++*itr;
        return lexeme;
    }
    return NULL;
}

static inline
const GpucLexeme* gpucReadIfToken(
    const GpucLexeme**      itr,
    const GpucLexeme* const end,
    const GpucToken         token)
{
    if (*itr < end) {
        const GpucLexeme* lexeme = *itr;
        if (lexeme->token == token) {
            ++*itr;
            return lexeme;
        }
    }
    return NULL;
}

static inline
const GpucLexeme* gpucReadIfTokenF(
    const GpucLexeme**      itr,
    const GpucLexeme* const end,
    bool (*const func)(GpucToken))
{
    if (*itr < end) {
        const GpucLexeme* lexeme = *itr;
        if (func(lexeme->token)) {
            ++*itr;
            return lexeme;
        }
    }
    return NULL;
}

static inline
bool gpucSeekToken(
    const GpucLexeme**      itr,
    const GpucLexeme* const end,
    const GpucToken         token)
{
    while (not gpucPeekToken(*itr, end, token) and gpucRead(itr, end));
    return gpucPeekToken(*itr, end, token);
}

static inline
bool gpucSeekTokenF(
    const GpucLexeme**      itr,
    const GpucLexeme* const end,
    bool (*const func)(GpucToken))
{
    while (not gpucPeekTokenF(*itr, end, func) and gpucRead(itr, end));
    return gpucPeekTokenF(*itr, end, func);
}

static inline
bool gpucSkipTokens(
    const GpucLexeme**      itr,
    const GpucLexeme* const end,
    const GpucToken         token)
{
    if (gpucPeekToken(*itr, end, token)) {
        while (gpucReadIfToken(itr, end, token));
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------

#define gpucPeek_()              gpucPeek(*itr, end)
#define gpucPeekToken_(token)    gpucPeekToken(*itr, end, GpucToken_##token)
#define gpucPeekTokenF_(func)    gpucPeekTokenF(*itr, end, gpucTokenIs##func)

#define gpucRead_()              gpucRead(itr, end)
#define gpucReadIfToken_(token)  gpucReadIfToken(itr, end, GpucToken_##token)
#define gpucReadIfTokenF_(func)  gpucReadIfTokenF(itr, end, gpucTokenIs##func)

#define gpucSeekToken_(token)    gpucSeekToken(itr, end, GpucToken_##token)
#define gpucSeekTokenF_(func)    gpucSeekTokenF(itr, end, gpucTokenIs##func)

#define gpucSkipToken_(token)    (gpucReadIfToken_(token) != NULL)
#define gpucSkipTokens_(token)   gpucSkipTokens(itr, end, GpucToken_##token)

//------------------------------------------------------------------------------

static inline
void gpucExpectSemicolon(
    GpucModule*         const module,
    const GpucLexeme**      itr,
    const GpucLexeme* const end)
{
    if (not gpucSkipTokens_(Semicolon))
        gpucError(*itr, "expected ';'");
}

#define gpucExpectSemicolon_() gpucExpectSemicolon(module, itr, end)

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(TopLevelDeclaration) {
    if (*itr < end) {
        GpucNode* node = NULL;
        (node = gpucParseNode_(StructDefinition)) or
        (node = gpucParseNode_(GlobalParameterDeclaration)) or
        (node = gpucParseNode_(Function));
        if (node) return node;
        gpucError(*itr, "expected struct or function");
    }
    return NULL;
}

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(StructDefinition) {
    if (not gpucSkipToken_(Struct))
        return NULL;

    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) {
        gpucError(*itr, "expected identifier");
        return NULL;
    }

    if (gpucTokenIsReservedWord(name->token)) {
        gpucError(name, "invalid identifier");
        return NULL;
    }

    if (not gpucSkipToken_(LBrace)) {
        gpucError(*itr, "expected '{'");
        return NULL;
    }

    GpucNode* const structure = gpucNodeAlloc_(StructDefinition, name, NULL);
    structure->type = structure;
    gpucDefineType(structure);

    while (gpucNodeAddChild(structure, gpucParseNode_(StructFieldDefinition)));

    if (not gpucSkipToken_(RBrace))
        gpucError(*itr, "expected '}'");

    gpucExpectSemicolon_();
    return structure;
}

GPUC_PARSE_RULE_DEFINITION(StructFieldDefinition) {
    if (gpucPeekToken_(RBrace))
        return NULL;

    const GpucLexeme* const typeName = gpucReadIfTokenF_(IdentifierOrTypename);
    if (not typeName) {
        gpucError(*itr, "expected type name");
        return NULL;
    }

    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) {
        gpucError(*itr, "expected identifier");
        return NULL;
    }

    if (gpucTokenIsReservedWord(name->token)) {
        gpucError(name, "invalid identifier");
        return NULL;
    }

    const GpucNode* const type = gpucRequireType(typeName);
    GpucNode* const field = gpucNodeAlloc_(StructFieldDefinition, name, type);

    gpucExpectSemicolon_();
    return field;
}

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(Function) {
    const GpucLexeme* const typeName = gpucReadIfTokenF_(IdentifierOrTypename);
    if (not typeName) return NULL;

    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) return NULL;

    const GpucLexeme* const lparen = gpucReadIfToken_(LParen);
    if (not lparen) return NULL;

    const GpucToken token = name->token;
    if (not gpucTokenIsIdentifierOrEntryPoint(token)) {
        gpucError(name, "invalid identifier");
        return NULL;
    }

    GpucNode* const params = gpucNodeAlloc_(FunctionParameterList, lparen, NULL);
    while (gpucNodeAddChild(params, gpucParseNode_(FunctionParameter))) {
        if (gpucSkipToken_(Comma)) continue;
        if (gpucSkipToken_(RParen)) break;
        gpucError(*itr, "expected ',' or ')'");
    }

    const GpucNode* const type = gpucRequireType(typeName);

    if (gpucSkipTokens_(Semicolon)) {
        GpucNode* const func = gpucNodeAlloc_(FunctionDeclaration, name, type);
        gpucNodeAddChild(func, params);
        gpucDeclareFunction(func);
        return func;
    }

    if (not gpucPeekToken_(LBrace)) {
        gpucError(*itr, "expected ';' or '{'");
        gpucNodeFree(params);
        return NULL;
    }

    GpucNode* const func = gpucNodeAlloc_(FunctionDefinition, name, type);
    gpucNodeAddChild(func, params);
    gpucDefineFunction(func);
    gpucBeginScope();
    {
        array_for(const GpucNode*, parameter, params->children) {
            gpucDefineVariable(parameter);
        }
        GpucNode* const body = gpucParseNode_(CompoundStatementInScope);
        gpucNodeAddChild(func, body);
    }
    gpucEndScope();

    return func;
}

GPUC_PARSE_RULE_DEFINITION(FunctionParameter) {
    if (gpucPeekToken_(RParen))
        return NULL;

    GpucNode* const node = gpucParseNode_(VariableDeclaration);
    if (node) return node;

    const GpucLexeme* const typeName = gpucReadIfTokenF_(IdentifierOrTypename);
    if (typeName) {
        const GpucNode* const type = gpucRequireType(typeName);
        return gpucNodeAlloc_(ParameterDeclaration, typeName, type);
    }

    gpucError(*itr, "expected parameter");
    return NULL;
}

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(GlobalParameterDeclaration) {
    if (not gpucSkipToken_(Param))
        return NULL;

    const GpucLexeme* const typeName = gpucReadIfTokenF_(IdentifierOrTypename);
    if (not typeName) {
        gpucError(*itr, "expected type name");
        return NULL;
    }

    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) {
        gpucError(*itr, "expected identifier");
        return NULL;
    }

    if (gpucTokenIsReservedWord(name->token)) {
        gpucError(name, "invalid identifier");
        return NULL;
    }

    const GpucNode* const type = gpucRequireType(typeName);

    gpucExpectSemicolon_();
    GpucNode* const uniform = gpucNodeAlloc_(GlobalParameterDeclaration, name, type);
    gpucDefineVariable(uniform);
    return uniform;
}

GPUC_PARSE_RULE_DEFINITION(VariableDeclaration) {
    const GpucLexeme* const typeName = gpucReadIfTokenF_(IdentifierOrTypename);
    if (not typeName) return NULL;

    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) return NULL;

    if (gpucTokenIsReservedWord(name->token)) {
        gpucError(name, "invalid identifier");
        return NULL;
    }

    const GpucNode* const type = gpucRequireType(typeName);

    return gpucNodeAlloc_(VariableDeclaration, name, type);
}

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(CompoundStatement) {
    gpucBeginScope();
    GpucNode* const block = gpucParseNode_(CompoundStatementInScope);
    gpucEndScope();
    return block;
}

GPUC_PARSE_RULE_DEFINITION(CompoundStatementInScope) {
    const GpucLexeme* const lbrace = gpucReadIfToken_(LBrace);
    if (not lbrace) return NULL;

    GpucNode* const block = gpucNodeAlloc_(CompoundStatement, lbrace, NULL);

    while (gpucNodeAddChild(block, gpucParseNode_(Statement)));

    if (not gpucSkipToken_(RBrace))
        gpucError(*itr, "expected '}'");

    gpucSkipTokens_(Semicolon);
    return block;
}

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(Statement) {
    if (gpucPeekToken_(RBrace))
        return NULL;

    GpucNode* node = NULL;
    (node = gpucParseNode_(EmptyStatement)) or
    (node = gpucParseNode_(IfStatement)) or
    (node = gpucParseNode_(SwitchStatement)) or
    (node = gpucParseNode_(WhileStatement)) or
    (node = gpucParseNode_(DoStatement)) or
    (node = gpucParseNode_(ForStatement)) or
    (node = gpucParseNode_(GotoStatement)) or
    (node = gpucParseNode_(ContinueStatement)) or
    (node = gpucParseNode_(BreakStatement)) or
    (node = gpucParseNode_(ReturnStatement)) or
    (node = gpucParseNode_(CompoundStatement)) or
    (node = gpucParseNode_(VariableStatement)) or
    (node = gpucParseNode_(ExpressionStatement));
    if (node) return node;

    gpucError(*itr, "expected statement");
    return NULL;
}

GPUC_PARSE_RULE_DEFINITION(EmptyStatement) {
    const GpucLexeme* const semicolon = gpucReadIfToken_(Semicolon);
    if (not semicolon) return NULL;

    return gpucNodeAlloc_(EmptyStatement, semicolon, NULL);
}

GPUC_PARSE_RULE_DEFINITION(IfStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(If);
    if (not keyword) return NULL;

    gpucError(*itr, "TODO: IfStatement");
    return NULL;
}

GPUC_PARSE_RULE_DEFINITION(SwitchStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(Switch);
    if (not keyword) return NULL;

    gpucError(*itr, "TODO: SwitchStatement");
    return NULL;
}

GPUC_PARSE_RULE_DEFINITION(WhileStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(While);
    if (not keyword) return NULL;

    gpucError(*itr, "TODO: WhileStatement");
    return NULL;
}

GPUC_PARSE_RULE_DEFINITION(DoStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(Do);
    if (not keyword) return NULL;

    gpucError(*itr, "TODO: DoStatement");
    return NULL;
}

GPUC_PARSE_RULE_DEFINITION(ForStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(For);
    if (not keyword) return NULL;

    gpucError(*itr, "TODO: ForStatement");
    return NULL;
}

GPUC_PARSE_RULE_DEFINITION(GotoStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(Goto);
    if (not keyword) return NULL;

    gpucError(*itr, "TODO: GotoStatement");
    return NULL;
}

GPUC_PARSE_RULE_DEFINITION(ContinueStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(Continue);
    if (not keyword) return NULL;

    GpucNode* const stmt = gpucNodeAlloc_(ContinueStatement, keyword, NULL);

    gpucExpectSemicolon_();
    return stmt;
}

GPUC_PARSE_RULE_DEFINITION(BreakStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(Break);
    if (not keyword) return NULL;

    GpucNode* const stmt = gpucNodeAlloc_(BreakStatement, keyword, NULL);

    gpucExpectSemicolon_();
    return stmt;
}

GPUC_PARSE_RULE_DEFINITION(ReturnStatement) {
    const GpucLexeme* const keyword = gpucReadIfToken_(Return);
    if (not keyword) return NULL;

    const GpucNode* const func = gpucFindEnclosingFunction();
    gpucAssert(func, "???");

    GpucNode* const expr = gpucParseNode_(Expression);

    const GpucNode* const exprType =
        expr ? expr->type : gpucGetPrimitiveType("void");

    if (not gpucNodesHaveSameType(exprType, func->type)) {
        gpucError(expr->lexeme, "return value does not match function type");
        gpucNote(func->lexeme, "see definition here");
    }

    GpucNode* stmt = gpucNodeAlloc_(ReturnStatement, keyword, exprType);
    gpucNodeAddChild(stmt, expr);

    gpucExpectSemicolon_();
    return stmt;
}

GPUC_PARSE_RULE_DEFINITION(VariableStatement) {
    const GpucLexeme* const head = gpucPeek_();
    GpucNode* const expr = gpucParseNode_(VariableExpression);
    if (not expr) return NULL;

    GpucNode* const stmt = gpucNodeAlloc_(ExpressionStatement, head, NULL);
    gpucNodeAddChild(stmt, expr);

    gpucExpectSemicolon_();
    return stmt;
}

GPUC_PARSE_RULE_DEFINITION(ExpressionStatement) {
    const GpucLexeme* const head = gpucPeek_();
    GpucNode* const expr = gpucParseNode_(Expression);
    if (not expr) return NULL;

    GpucNode* const stmt = gpucNodeAlloc_(ExpressionStatement, head, NULL);
    gpucNodeAddChild(stmt, expr);

    gpucExpectSemicolon_();
    return stmt;
}

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(Expression) {
    GpucNode* node = NULL;
    (node = gpucParseNode_(AssignmentExpression)) or
    (node = gpucParseNode_(TernaryExpression));
    return node;
}

GPUC_PARSE_RULE_DEFINITION(VariableExpression) {
    GpucNode* const lhs = gpucParseNode_(VariableDeclaration);
    if (not lhs) return NULL;

    gpucDefineVariable(lhs);

    const GpucLexeme* const op = gpucReadIfToken_(Eq);
    if (not op) return lhs;

    GpucNode* const rhs = gpucParseNode_(Expression);
    if (not rhs)
        gpucError(*itr, "expected expression");

    GpucNode* const expr = gpucNodeAlloc_(AssignmentExpression, op, lhs->type);
    gpucNodeAddChild(expr, lhs);
    gpucNodeAddChild(expr, rhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(AssignmentExpression) {
    GpucNode* const lhs = gpucParseNode_(PrefixExpression);
    if (not lhs) return NULL;

    const GpucLexeme* const op = gpucReadIfTokenF_(AssignmentOperator);
    if (not op) {
        gpucNodeFree(lhs);
        return NULL;
    }

    GpucNode* const rhs = gpucParseNode_(Expression);
    if (not rhs)
        gpucError(*itr, "expected expression");

    if (not lhs->type)
        gpucError(lhs->lexeme, "undefined type");

    GpucNode* const expr = gpucNodeAlloc_(AssignmentExpression, op, lhs->type);
    gpucNodeAddChild(expr, lhs);
    gpucNodeAddChild(expr, rhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(PrefixExpression) {
    const GpucLexeme* const op = gpucReadIfTokenF_(PrefixOperator);
    if (not op) return gpucParseNode_(MemberExpression);

    GpucNode* const rhs = gpucParseNode_(PrefixExpression);
    if (not rhs)
        gpucError(*itr, "expected expression");

    if (not gpucNodeIsAssignable(rhs))
        gpucError(*itr, "subsequent expression must be a modifiable lvalue");

    GpucNode* const expr = gpucNodeAlloc_(PrefixExpression, op, rhs->type);
    gpucNodeAddChild(expr, rhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

static inline
const GpucNode* gpucModuleGetVectorElementType(
    const GpucModule* const module,
    const GpucNode*   const vectorType)
{
    const GpucLexeme* const vectorTypeName = vectorType->lexeme;
    const GpucToken vectorTypeToken = vectorTypeName->token;
    switch (vectorTypeToken) {
        case GpucToken_Bool2:
        case GpucToken_Bool3:
        case GpucToken_Bool4: return gpucGetPrimitiveType("bool");

        case GpucToken_Float2:
        case GpucToken_Float3:
        case GpucToken_Float4: return gpucGetPrimitiveType("float");

        case GpucToken_Int2:
        case GpucToken_Int3:
        case GpucToken_Int4: return gpucGetPrimitiveType("int");

        case GpucToken_UInt2:
        case GpucToken_UInt3:
        case GpucToken_UInt4: return gpucGetPrimitiveType("uint");

        default: return NULL;
    }
}

static inline
const GpucNode* gpucModuleGetVectorType(
    GpucModule*     const module,
    const GpucNode* const elementType,
    const unsigned        vectorWidth)
{
    gpucAssert(vectorWidth >= 2, "vector is too narrow");
    gpucAssert(vectorWidth <= 4, "vector is too wide");
    const GpucLexeme* const elementTypeName = elementType->lexeme;
    const GpucToken elementTypeToken = elementTypeName->token;
    switch (elementTypeToken) {
        case GpucToken_Bool:
        case GpucToken_Float:
        case GpucToken_Int:
        case GpucToken_UInt: {
            const GpucToken vectorTypeToken =
                (GpucToken)(elementTypeToken + (vectorWidth - 1));
            const char* const vectorTypeLiteral =
                gpucTokenLiteral(vectorTypeToken);
            return gpucGetPrimitiveType(vectorTypeLiteral);
        }
        default: return NULL;
    }
}

static inline
bool gpucSwizzleNameFormatIs(
    const GpucLexeme* const swizzleName,
    const char*       const formatChars)
{
    gpucAssert(strlen(formatChars) == 4, "!!!");
    for (unsigned i = 0, n = swizzleName->length; i < n; ++i) {
        const char swizzleChar = swizzleName->head[i];
        if (swizzleChar != formatChars[0] and
            swizzleChar != formatChars[1] and
            swizzleChar != formatChars[2] and
            swizzleChar != formatChars[3])
            return false;
    }
    return true;
}

static inline
const GpucNode* gpucModuleGetSwizzleType(
    GpucModule*       const module,
    const GpucNode*   const vectorType,
    const GpucLexeme* const swizzleName)
{
    const GpucNode* const elementType =
        gpucModuleGetVectorElementType(module, vectorType);

    if (not elementType) return NULL;

    if (not gpucSwizzleNameFormatIs(swizzleName, "xyzw") and
        not gpucSwizzleNameFormatIs(swizzleName, "rgba") and
        not gpucSwizzleNameFormatIs(swizzleName, "stpq"))
        gpucError(swizzleName, "invalid swizzle format");

    unsigned swizzleWidth = swizzleName->length;
    if (swizzleWidth > 4) {
        gpucError(swizzleName, "invalid swizzle width");
        swizzleWidth = 4;
    }

    return gpucModuleGetVectorType(module, elementType, swizzleWidth);
}

#define gpucGetSwizzleType(vectorType, swizzleName) \
        gpucModuleGetSwizzleType(module, vectorType, swizzleName)

GPUC_PARSE_RULE_DEFINITION(MemberReference) {
    gpucAssert(structType, "undefined structure type");

    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) return NULL;

    if (gpucTokenIsReservedWord(name->token)) {
        gpucError(name, "invalid identifier");
        return NULL;
    }

    const GpucLexeme* const structName = structType->lexeme;
    if (gpucTokenIsTypename(structName->token)) {
        const GpucNode* const swizzleType =
            gpucGetSwizzleType(structType, name);
        return gpucNodeAlloc_(MemberReference, name, swizzleType);
    }

    const GpucNode* const member = gpucNodeFindChildByLexeme(structType, name);
    if (member) {
        const GpucNode* const memberType = member->type;
        gpucAssert(memberType, "undefined member type");
        return gpucNodeAlloc_(MemberReference, name, memberType);
    }

    gpucError(name, "undeclared member");
    gpucNote(structName, "see definition here");

    const GpucNode* const voidType = gpucGetPrimitiveType("void");
    return gpucNodeAlloc_(MemberReference, name, voidType);
}

GPUC_PARSE_RULE_DEFINITION(MemberExpression) {
    GpucNode* const lhs =  gpucParseNode_(SubscriptExpression);
    if (not lhs) return NULL;

    const GpucLexeme* op;
    (op = gpucReadIfToken_(Dot)) or
    (op = gpucReadIfToken_(Arrow));
    if (not op) return lhs;

    if (op->token == GpucToken_Arrow)
        gpucError(*itr, "expected '.'");
    
    if (lhs->lexeme->head == strstr(lhs->lexeme->head, "cam."))
        (void)0;
    
    GpucNode* const rhs =
        gpucParseNodeInStruct_(lhs->type, MemberSubmemberExpression);
    if (not rhs)
        gpucError(*itr, "expected member name");
    
    if (rhs->type == NULL)
        (void)0;

    const GpucNode* const type = rhs ? rhs->type : NULL;

    GpucNode* const expr = gpucNodeAlloc_(MemberExpression, op, type);
    gpucNodeAddChild(expr, lhs);
    gpucNodeAddChild(expr, rhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(MemberSubmemberExpression) {
    GpucNode* const lhs = gpucParseNode_(MemberSubscriptExpression);
    if (not lhs) return NULL;

    const GpucLexeme* op;
    (op = gpucReadIfToken_(Dot)) or
    (op = gpucReadIfToken_(Arrow));
    if (not op) return lhs;

    if (op->token == GpucToken_Arrow)
        gpucError(*itr, "expected '.'");

    GpucNode* const rhs =
        gpucParseNodeInStruct_(lhs->type, MemberSubmemberExpression);
    if (not rhs)
        gpucError(*itr, "expected member name");

    const GpucNode* const type = rhs ? rhs->type : NULL;

    GpucNode* const expr = gpucNodeAlloc_(MemberExpression, op, type);
    gpucNodeAddChild(expr, lhs);
    gpucNodeAddChild(expr, rhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(MemberSubscriptExpression) {
    GpucNode* const lhs = gpucParseNode_(MemberPostfixExpression);
    if (not lhs) return NULL;

    const GpucLexeme* const op = gpucReadIfToken_(LBrack);
    if (not op) return lhs;

    GpucNode* const rhs = gpucParseNode_(Expression);
    if (not rhs)
        gpucError(*itr, "expected expression");

    if (not gpucSkipToken_(RBrack))
        gpucError(*itr, "expected ']'");

    GpucNode* const expr = gpucNodeAlloc_(SubscriptExpression, op, NULL);
    gpucNodeAddChild(expr, lhs);
    gpucNodeAddChild(expr, rhs);
    while (gpucReadIfToken_(LBrack)) {
        GpucNode* const rhs = gpucParseNode_(Expression);
        if (not rhs)
            gpucError(*itr, "expected expression");

        if (not gpucSkipToken_(RBrack))
            gpucError(*itr, "expected ']'");

        gpucNodeAddChild(expr, rhs);
    }

    gpucError(op, "TODO: determine type of subscript expression");
    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(SubscriptExpression) {
    GpucNode* const lhs = gpucParseNode_(CallOrPostfixExpression);
    if (not lhs) return NULL;

    const GpucLexeme* const op = gpucReadIfToken_(LBrack);
    if (not op) return lhs;

    GpucNode* const rhs = gpucParseNode_(Expression);
    if (not rhs)
        gpucError(*itr, "expected expression");

    if (not gpucSkipToken_(RBrack))
        gpucError(*itr, "expected ']'");

    GpucNode* const expr = gpucNodeAlloc_(SubscriptExpression, op, NULL);
    gpucNodeAddChild(expr, lhs);
    gpucNodeAddChild(expr, rhs);
    while (gpucReadIfToken_(LBrack)) {
        GpucNode* const rhs = gpucParseNode_(Expression);
        if (not rhs)
            gpucError(*itr, "expected expression");

        if (not gpucSkipToken_(RBrack))
            gpucError(*itr, "expected ']'");

        gpucNodeAddChild(expr, rhs);
    }

    gpucError(op, "TODO: determine type of subscript expression");
    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(MemberPostfixExpression) {
    GpucNode* const lhs = gpucParseNode_(MemberReference);
    if (not lhs) return NULL;

    const GpucLexeme* const op = gpucReadIfTokenF_(PostfixOperator);
    if (not op) return lhs;

    if (gpucPeekTokenF_(PostfixOperator))
        gpucError(*itr, "preceding expression must be a modifiable lvalue");

    GpucNode* const expr = gpucNodeAlloc_(PostfixExpression, op, lhs->type);
    gpucNodeAddChild(expr, lhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(PostfixExpression) {
    GpucNode* const lhs = gpucParseNode_(PrimaryExpression);
    if (not lhs) return NULL;

    const GpucLexeme* const op = gpucReadIfTokenF_(PostfixOperator);
    if (not op) return lhs;

    if (gpucPeekTokenF_(PostfixOperator))
        gpucError(*itr, "preceding expression must be a modifiable lvalue");

    GpucNode* const expr = gpucNodeAlloc_(PostfixExpression, op, lhs->type);
    gpucNodeAddChild(expr, lhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(CallExpression) {
    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) return NULL;

    const GpucLexeme* const lparen = gpucReadIfToken_(LParen);
    if (not lparen) return NULL;

    if (gpucTokenIsKeyword(name->token)) {
        gpucError(name, "invalid identifier");
        return NULL;
    }

    GpucNode* const params =
        gpucNodeAlloc_(CallParameterList, lparen, NULL);
    while (gpucNodeAddChild(params, gpucParseNode_(Expression))) {
        if (gpucSkipToken_(Comma)) continue;
        if (gpucSkipToken_(RParen)) break;
        gpucError(*itr, "expected ',' or ')'");
    }

    GpucNode* const expr = gpucNodeAlloc_(CallExpression, name, NULL);
    gpucNodeAddChild(expr, params);

    if (gpucTokenIsTypename(name->token)) {
        expr->type = gpucRequireType(name);
    } else {
        const GpucNode* const func = gpucRequireFunction(expr);
        expr->type = func ? func->type : gpucGetPrimitiveType("void");
    }

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(CallOrPostfixExpression) {
    GpucNode* node = NULL;
    (node = gpucParseNode_(CallExpression)) or
    (node = gpucParseNode_(PostfixExpression));
    return node;
}

GPUC_PARSE_RULE_DEFINITION(PrimaryExpression) {
    GpucNode* node = NULL;
    (node = gpucParseNode_(LiteralExpression)) or
    (node = gpucParseNode_(VariableReference)) or
    (node = gpucParseNode_(NestedExpression));
    return node;
}

GPUC_PARSE_RULE_DEFINITION(LiteralExpression) {
    const GpucLexeme* const literal = gpucReadIfTokenF_(Literal);
    if (not literal) return NULL;

    const GpucToken literalToken = literal->token;

    const char* const typeName =
        (literalToken == GpucToken_DoubleLiteral) ? "double" :
        (literalToken == GpucToken_FalseLiteral)  ? "bool"   :
        (literalToken == GpucToken_FloatLiteral)  ? "float"  :
        (literalToken == GpucToken_IntLiteral)    ? "int"    :
        (literalToken == GpucToken_UIntLiteral)   ? "uint"   :
        (literalToken == GpucToken_TrueLiteral)   ? "bool"   :
        "void";

    const GpucNode* const type = gpucGetPrimitiveType(typeName);

    return gpucNodeAlloc_(LiteralExpression, literal, type);
}

GPUC_PARSE_RULE_DEFINITION(VariableReference) {
    const GpucLexeme* const name = gpucReadIfTokenF_(IdentifierOrReservedWord);
    if (not name) return NULL;

    const GpucNode* const symbol = gpucRequireVariable(name);

    const GpucNode* const type =
        symbol ? symbol->type : gpucGetPrimitiveType("void");

    const GpucSemantic symbolSemantic =
        symbol ? symbol->semantic : GpucSemantic_None;

    if (symbolSemantic == GpucSemantic_GlobalParameterDeclaration)
        return gpucNodeAlloc_(ParamReference, name, type);

    return gpucNodeAlloc_(VariableReference, name, type);
}

GPUC_PARSE_RULE_DEFINITION(NestedExpression) {
    const GpucLexeme* const op = gpucReadIfToken_(LParen);
    if (not op) return NULL;

    GpucNode* const child = gpucParseNode_(Expression);
    if (not child)
        gpucError(*itr, "expected expression");

    if (not gpucSkipToken_(RParen))
        gpucError(*itr, "expected ')'");

    const GpucNode* const type =
        child ? child->type : gpucGetPrimitiveType("void");

    GpucNode* const expr = gpucNodeAlloc_(NestedExpression, op, type);
    gpucNodeAddChild(expr, child);
    return expr;
}

GPUC_PARSE_RULE_DEFINITION(TernaryExpression) {
    GpucNode* const condExpr = gpucParseNode_(LogicalOrExpression);
    if (not condExpr) return NULL;

    const GpucLexeme* const op = gpucReadIfToken_(Question);
    if (not op) return condExpr;

    GpucNode* const trueExpr = gpucParseNode_(LogicalOrExpression);
    if (not trueExpr) {
        gpucError(*itr, "expected expression");
        gpucNodeFree(condExpr);
        return NULL;
    }

    if (not gpucSkipToken_(Colon))
        gpucError(*itr, "expected ':'");

    GpucNode* const falseExpr = gpucParseNode_(LogicalOrExpression);
    if (not falseExpr) {
        gpucError(*itr, "expected expression");
        gpucNodeFree(condExpr);
        gpucNodeFree(trueExpr);
        return NULL;
    }

    const GpucNode* const trueType =
        trueExpr ? trueExpr->type : gpucGetPrimitiveType("void");

    GpucNode* const expr = gpucNodeAlloc_(TernaryExpression, op, trueType);
    gpucNodeAddChild(expr, condExpr);
    gpucNodeAddChild(expr, trueExpr);
    gpucNodeAddChild(expr, falseExpr);
    return expr;
}

//------------------------------------------------------------------------------

typedef struct GpucBinaryOperation {
    const GpucToken ltoken : 8;
    const GpucToken minOp  : 8;
    const GpucToken maxOp  : 8;
    const GpucToken rtoken : 8;
    const GpucToken type   : 8;
} GpucBinaryOperation;

#define GpucBinaryOperation(LTOKEN, MINOP, MAXOP, RTOKEN, TYPE) \
    ((GpucBinaryOperation){\
        GpucToken_##LTOKEN, \
        GpucToken_##MINOP,\
        GpucToken_##MAXOP, \
        GpucToken_##RTOKEN,\
        GpucToken_##TYPE,\
    })

static const GpucBinaryOperation gpucBinaryOperations[] = {

    // Relational Operators
    GpucBinaryOperation(Bool,     EqEq,NotEq,   Bool,      /**/ Bool),
    GpucBinaryOperation(Bool2,    EqEq,NotEq,   Bool2,     /**/ Bool2),
    GpucBinaryOperation(Bool3,    EqEq,NotEq,   Bool3,     /**/ Bool3),
    GpucBinaryOperation(Bool4,    EqEq,NotEq,   Bool4,     /**/ Bool4),
    GpucBinaryOperation(Float,    EqEq,NotEq,   Float,     /**/ Bool),
    GpucBinaryOperation(Float2,   EqEq,NotEq,   Float2,    /**/ Bool2),
    GpucBinaryOperation(Float3,   EqEq,NotEq,   Float3,    /**/ Bool3),
    GpucBinaryOperation(Float4,   EqEq,NotEq,   Float4,    /**/ Bool4),
    GpucBinaryOperation(Int,      EqEq,NotEq,   Int,       /**/ Bool),
    GpucBinaryOperation(Int2,     EqEq,NotEq,   Int2,      /**/ Bool2),
    GpucBinaryOperation(Int3,     EqEq,NotEq,   Int3,      /**/ Bool3),
    GpucBinaryOperation(Int4,     EqEq,NotEq,   Int4,      /**/ Bool4),
    GpucBinaryOperation(UInt,     EqEq,NotEq,   UInt,      /**/ Bool),
    GpucBinaryOperation(UInt2,    EqEq,NotEq,   UInt2,     /**/ Bool2),
    GpucBinaryOperation(UInt3,    EqEq,NotEq,   UInt3,     /**/ Bool3),
    GpucBinaryOperation(UInt4,    EqEq,NotEq,   UInt4,     /**/ Bool4),

    // Logical Operators
    GpucBinaryOperation(Bool,     OrOr,AndAnd,  Bool,      /**/ Bool),
    GpucBinaryOperation(Bool2,    OrOr,AndAnd,  Bool2,     /**/ Bool2),
    GpucBinaryOperation(Bool3,    OrOr,AndAnd,  Bool3,     /**/ Bool3),
    GpucBinaryOperation(Bool4,    OrOr,AndAnd,  Bool4,     /**/ Bool4),
    GpucBinaryOperation(None,     Not,Not,      Bool,      /**/ Bool),
    GpucBinaryOperation(None,     Not,Not,      Bool2,     /**/ Bool2),
    GpucBinaryOperation(None,     Not,Not,      Bool3,     /**/ Bool3),
    GpucBinaryOperation(None,     Not,Not,      Bool4,     /**/ Bool4),

    // Integral Operators
    GpucBinaryOperation(None,     Compl,Compl,   Int,      /**/ Int),
    GpucBinaryOperation(None,     Compl,Compl,   Int2,     /**/ Int2),
    GpucBinaryOperation(None,     Compl,Compl,   Int3,     /**/ Int3),
    GpucBinaryOperation(None,     Compl,Compl,   Int4,     /**/ Int4),
    GpucBinaryOperation(None,     Compl,Compl,   UInt,     /**/ UInt),
    GpucBinaryOperation(None,     Compl,Compl,   UInt2,    /**/ UInt2),
    GpucBinaryOperation(None,     Compl,Compl,   UInt3,    /**/ UInt3),
    GpucBinaryOperation(None,     Compl,Compl,   UInt4,    /**/ UInt4),

    GpucBinaryOperation(Int,      LshEq,And,     Int,      /**/ Int),
    GpucBinaryOperation(Int2,     LshEq,And,     Int2,     /**/ Int2),
    GpucBinaryOperation(Int3,     LshEq,And,     Int3,     /**/ Int3),
    GpucBinaryOperation(Int4,     LshEq,And,     Int4,     /**/ Int4),
    GpucBinaryOperation(UInt,     LshEq,And,     UInt,     /**/ UInt),
    GpucBinaryOperation(UInt2,    LshEq,And,     UInt2,    /**/ UInt2),
    GpucBinaryOperation(UInt3,    LshEq,And,     UInt3,    /**/ UInt3),
    GpucBinaryOperation(UInt4,    LshEq,And,     UInt4,    /**/ UInt4),

    // Arithmetic Operators
    GpucBinaryOperation(Bool,     AddEq,Eq,      Bool,     /**/ Bool),
    GpucBinaryOperation(Bool2,    AddEq,Eq,      Bool2,    /**/ Bool2),
    GpucBinaryOperation(Bool3,    AddEq,Eq,      Bool3,    /**/ Bool3),
    GpucBinaryOperation(Bool4,    AddEq,Eq,      Bool4,    /**/ Bool4),
    GpucBinaryOperation(Float,    AddEq,Eq,      Float,    /**/ Float),
    GpucBinaryOperation(Float2,   AddEq,Eq,      Float2,   /**/ Float2),
    GpucBinaryOperation(Float3,   AddEq,Eq,      Float3,   /**/ Float3),
    GpucBinaryOperation(Float4,   AddEq,Eq,      Float4,   /**/ Float4),
    GpucBinaryOperation(Int,      AddEq,Eq,      Int,      /**/ Int),
    GpucBinaryOperation(Int2,     AddEq,Eq,      Int2,     /**/ Int2),
    GpucBinaryOperation(Int3,     AddEq,Eq,      Int3,     /**/ Int3),
    GpucBinaryOperation(Int4,     AddEq,Eq,      Int4,     /**/ Int4),
    GpucBinaryOperation(UInt,     AddEq,Eq,      UInt,     /**/ UInt),
    GpucBinaryOperation(UInt2,    AddEq,Eq,      UInt2,    /**/ UInt2),
    GpucBinaryOperation(UInt3,    AddEq,Eq,      UInt3,    /**/ UInt3),
    GpucBinaryOperation(UInt4,    AddEq,Eq,      UInt4,    /**/ UInt4),

    // (float, int)
    GpucBinaryOperation(Float,    AddEq,Eq,      Int,      /**/ Float),
    GpucBinaryOperation(Float2,   AddEq,Eq,      Int2,     /**/ Float2),
    GpucBinaryOperation(Float3,   AddEq,Eq,      Int3,     /**/ Float3),
    GpucBinaryOperation(Float4,   AddEq,Eq,      Int4,     /**/ Float4),
    GpucBinaryOperation(Float,    AddEq,Eq,      UInt,     /**/ Float),
    GpucBinaryOperation(Float2,   AddEq,Eq,      UInt2,    /**/ Float2),
    GpucBinaryOperation(Float3,   AddEq,Eq,      UInt3,    /**/ Float3),
    GpucBinaryOperation(Float4,   AddEq,Eq,      UInt4,    /**/ Float4),

    // (int, float)
    GpucBinaryOperation(Int,      AddEq,Eq,      Float,     /**/ Float),
    GpucBinaryOperation(Int2,     AddEq,Eq,      Float2,    /**/ Float2),
    GpucBinaryOperation(Int3,     AddEq,Eq,      Float3,    /**/ Float3),
    GpucBinaryOperation(Int4,     AddEq,Eq,      Float4,    /**/ Float4),
    GpucBinaryOperation(UInt,     AddEq,Eq,      Float,     /**/ Float),
    GpucBinaryOperation(UInt2,    AddEq,Eq,      Float2,    /**/ Float2),
    GpucBinaryOperation(UInt3,    AddEq,Eq,      Float3,    /**/ Float3),
    GpucBinaryOperation(UInt4,    AddEq,Eq,      Float4,    /**/ Float4),

    // (vector, matrix)
    GpucBinaryOperation(Float2,   MulEq,Mul,     Float2x2,  /**/ Float2),
    GpucBinaryOperation(Float3,   MulEq,Mul,     Float3x3,  /**/ Float3),
    GpucBinaryOperation(Float4,   MulEq,Mul,     Float4x4,  /**/ Float4),

    // (matrix, vector)
    GpucBinaryOperation(Float2x2, MulEq,Mul,     Float2,    /**/ Float2),
    GpucBinaryOperation(Float3x3, MulEq,Mul,     Float3,    /**/ Float3),
    GpucBinaryOperation(Float4x4, MulEq,Mul,     Float4,    /**/ Float4),

};

enum {
    gpucBinaryOperations_length =
        sizeof(gpucBinaryOperations) /
        sizeof(gpucBinaryOperations[0])
};

static inline
const GpucNode* gpucFindBinaryExpressionType(
    GpucModule*     const module,
    const GpucNode* const lhs,
    const GpucToken       op,
    const GpucNode* const rhs)
{
    const GpucNode* const ltype = lhs ? lhs->type : NULL;
    const GpucNode* const rtype = rhs ? rhs->type : NULL;
    const GpucToken ltoken = ltype ? ltype->lexeme->token : GpucToken_None;
    const GpucToken rtoken = rtype ? rtype->lexeme->token : GpucToken_None;

    for (int i = 0; i < gpucBinaryOperations_length; ++i) {
        const GpucBinaryOperation operation = gpucBinaryOperations[i];
        if (ltoken != operation.ltoken) continue;
        if (op      < operation.minOp)  continue;
        if (op      > operation.maxOp)  continue;
        if (rtoken != operation.rtoken) continue;
        return gpucGetPrimitiveType(gpucTokenLiteral(operation.type));
    }
    return NULL;
}

//------------------------------------------------------------------------------

enum { GpucTokensArrayLength = 8 };

typedef struct GpucTokens {
    GpucToken array[GpucTokensArrayLength];
} GpucTokens;

#define GpucTokens(...)\
        static const GpucTokens gpucTokens = {{ __VA_ARGS__ }}

static inline
GpucNode* gpucModuleParseBinaryExpression(
    GpucModule*          const module,
    const GpucLexeme**       itr,
    const GpucLexeme*  const end,
    const GpucNode  *  const structType,
    const GpucSemantic       exprSemantic,
    const GpucTokens*  const opTokens,
    const GpucParseRule      subRule)
{
    GpucNode* const lhs =
        gpucModuleParseNode(module, itr, end, structType, subRule);
    if (not lhs) return NULL;

    const GpucLexeme* op = NULL;
    for (unsigned i = 0; i < GpucTokensArrayLength; ++i) {
        const GpucToken opToken = opTokens->array[i];
        if (not opToken) break;
        if ((op = gpucReadIfToken(itr, end, opToken))) break;
    }
    if (not op) return lhs;

    GpucNode* const rhs =
        gpucModuleParseBinaryExpression(
            module, itr, end, structType,
            exprSemantic,
            opTokens,
            subRule);
    if (not rhs)
        gpucError(*itr, "expected expression");

    const GpucNode* const type =
        gpucFindBinaryExpressionType(module, lhs, op->token, rhs);
    if (not type) {
        const GpucNode* const voidType = gpucGetPrimitiveType("void");
        const GpucNode* const lhsType = lhs->type ? lhs->type : voidType;
        const GpucNode* const rhsType = rhs->type ? rhs->type : voidType;
        const GpucLexeme* const lhsTypeName = lhsType->lexeme;
        const GpucLexeme* const rhsTypeName = rhsType->lexeme;
        gpucError(op,
            "invalid operands to binary expression (%.*s %.*s %.*s)",
            lhsTypeName->length, lhsTypeName->head,
            op->length, op->head,
            rhsTypeName->length, rhsTypeName->head);
    }

    GpucNode* const expr = gpucNodeAlloc(exprSemantic, op, type);
    gpucNodeAddChild(expr, lhs);
    gpucNodeAddChild(expr, rhs);

    gpucAssert(expr->type, "undefined expression type");
    return expr;
}

#define gpucParseBinaryExpression_(exprSemantic, subRule) \
        gpucModuleParseBinaryExpression( \
            module, itr, end, structType, \
            GpucSemantic_##exprSemantic, \
            &gpucTokens, \
            GpucParseRule_##subRule)

//------------------------------------------------------------------------------

GPUC_PARSE_RULE_DEFINITION(LogicalOrExpression) {
    GpucTokens(GpucToken_OrOr);
    return gpucParseBinaryExpression_(
        LogicalOrExpression,
        LogicalAndExpression);
}

GPUC_PARSE_RULE_DEFINITION(LogicalAndExpression) {
    GpucTokens(GpucToken_AndAnd);
    return gpucParseBinaryExpression_(
        LogicalAndExpression,
        BitwiseOrExpression);
}

GPUC_PARSE_RULE_DEFINITION(BitwiseOrExpression) {
    GpucTokens(GpucToken_Or);
    return gpucParseBinaryExpression_(
        BitwiseOrExpression,
        BitwiseXorExpression);
}

GPUC_PARSE_RULE_DEFINITION(BitwiseXorExpression) {
    GpucTokens(GpucToken_Xor);
    return gpucParseBinaryExpression_(
        BitwiseXorExpression,
        BitwiseAndExpression);
}

GPUC_PARSE_RULE_DEFINITION(BitwiseAndExpression) {
    GpucTokens(GpucToken_And);
    return gpucParseBinaryExpression_(
        BitwiseAndExpression,
        EqualityExpression);
}

GPUC_PARSE_RULE_DEFINITION(EqualityExpression) {
    GpucTokens(GpucToken_EqEq, GpucToken_NotEq);
    return gpucParseBinaryExpression_(
        EqualityExpression,
        RelationalExpression);
}

GPUC_PARSE_RULE_DEFINITION(RelationalExpression) {
    GpucTokens(GpucToken_Lt, GpucToken_LtEq, GpucToken_Gt, GpucToken_GtEq);
    return gpucParseBinaryExpression_(
        RelationalExpression,
        ShiftExpression);
}

GPUC_PARSE_RULE_DEFINITION(ShiftExpression) {
    GpucTokens(GpucToken_Lsh, GpucToken_Rsh);
    return gpucParseBinaryExpression_(
        ShiftExpression,
        SumExpression);
}

GPUC_PARSE_RULE_DEFINITION(SumExpression) {
    GpucTokens(GpucToken_Add, GpucToken_Sub);
    return gpucParseBinaryExpression_(
        SumExpression,
        ProductExpression);
}

GPUC_PARSE_RULE_DEFINITION(ProductExpression) {
    GpucTokens(GpucToken_Div, GpucToken_Mod, GpucToken_Mul);
    return gpucParseBinaryExpression_(
        ProductExpression,
        PrefixExpression);
}

#undef GPUC_PARSE_RULE_DEFINITION
