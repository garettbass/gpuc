#pragma once
#include <assert.h>
#include <iso646.h>
#include <stdbool.h>

#define GPUC_KEYWORD_TOKENS(GPUC_KEYWORD_TOKEN)\
        GPUC_KEYWORD_TOKEN(Attribute,    "attribute")\
        GPUC_KEYWORD_TOKEN(Break,        "break")\
        GPUC_KEYWORD_TOKEN(Case,         "case")\
        GPUC_KEYWORD_TOKEN(Const,        "const")\
        GPUC_KEYWORD_TOKEN(Continue,     "continue")\
        GPUC_KEYWORD_TOKEN(Default,      "default")\
        GPUC_KEYWORD_TOKEN(Discard,      "discard")\
        GPUC_KEYWORD_TOKEN(Do,           "do")\
        GPUC_KEYWORD_TOKEN(Else,         "else")\
        GPUC_KEYWORD_TOKEN(FalseLiteral, "false")\
        GPUC_KEYWORD_TOKEN(For,          "for")\
        GPUC_KEYWORD_TOKEN(Goto,         "goto")\
        GPUC_KEYWORD_TOKEN(If,           "if")\
        GPUC_KEYWORD_TOKEN(Param,        "param")\
        GPUC_KEYWORD_TOKEN(Return,       "return")\
        GPUC_KEYWORD_TOKEN(Struct,       "struct")\
        GPUC_KEYWORD_TOKEN(Switch,       "switch")\
        GPUC_KEYWORD_TOKEN(TrueLiteral,  "true")\
        GPUC_KEYWORD_TOKEN(Typedef,      "typedef")\
        GPUC_KEYWORD_TOKEN(While,        "while")\
        GPUC_KEYWORD_TOKEN(Comp,         "comp")\
        GPUC_KEYWORD_TOKEN(Frag,         "frag")\
        GPUC_KEYWORD_TOKEN(Vert,         "vert")\

#define GPUC_TYPENAME_TOKENS(GPUC_TYPENAME_TOKEN)\
        GPUC_TYPENAME_TOKEN(Void,                "void")\
        GPUC_TYPENAME_TOKEN(Bool,                "bool")\
        GPUC_TYPENAME_TOKEN(Bool2,               "bool2")\
        GPUC_TYPENAME_TOKEN(Bool3,               "bool3")\
        GPUC_TYPENAME_TOKEN(Bool4,               "bool4")\
        GPUC_TYPENAME_TOKEN(Double,              "double")\
        GPUC_TYPENAME_TOKEN(Float,               "float")\
        GPUC_TYPENAME_TOKEN(Float2,              "float2")\
        GPUC_TYPENAME_TOKEN(Float3,              "float3")\
        GPUC_TYPENAME_TOKEN(Float4,              "float4")\
        GPUC_TYPENAME_TOKEN(Float2x2,            "float2x2")\
        GPUC_TYPENAME_TOKEN(Float2x3,            "float2x3")\
        GPUC_TYPENAME_TOKEN(Float2x4,            "float2x4")\
        GPUC_TYPENAME_TOKEN(Float3x2,            "float3x2")\
        GPUC_TYPENAME_TOKEN(Float3x3,            "float3x3")\
        GPUC_TYPENAME_TOKEN(Float3x4,            "float3x4")\
        GPUC_TYPENAME_TOKEN(Float4x2,            "float4x2")\
        GPUC_TYPENAME_TOKEN(Float4x3,            "float4x3")\
        GPUC_TYPENAME_TOKEN(Float4x4,            "float4x4")\
        GPUC_TYPENAME_TOKEN(Int,                 "int")\
        GPUC_TYPENAME_TOKEN(Int2,                "int2")\
        GPUC_TYPENAME_TOKEN(Int3,                "int3")\
        GPUC_TYPENAME_TOKEN(Int4,                "int4")\
        GPUC_TYPENAME_TOKEN(UInt,                "uint")\
        GPUC_TYPENAME_TOKEN(UInt2,               "uint2")\
        GPUC_TYPENAME_TOKEN(UInt3,               "uint3")\
        GPUC_TYPENAME_TOKEN(UInt4,               "uint4")\
        GPUC_TYPENAME_TOKEN(TEXTURE1D,           "texture1d")\
        GPUC_TYPENAME_TOKEN(TEXTURE2D,           "texture2d")\
        GPUC_TYPENAME_TOKEN(TEXTURE3D,           "texture3d")\
        GPUC_TYPENAME_TOKEN(TEXTURECUBE,         "texturecube")\
        GPUC_TYPENAME_TOKEN(DEPTH2D,             "depth2d")\
        GPUC_TYPENAME_TOKEN(DEPTHCUBE,           "depthcube")\
        GPUC_TYPENAME_TOKEN(SAMPLER,             "sampler")\


#define GPUC_SYMBOL_TOKENS(GPUC_SYMBOL_TOKEN)\
        /* Delimiters */\
        GPUC_SYMBOL_TOKEN(LBrace,        "{")\
        GPUC_SYMBOL_TOKEN(RBrace,        "}")\
        GPUC_SYMBOL_TOKEN(LBrack,        "[")\
        GPUC_SYMBOL_TOKEN(RBrack,        "]")\
        GPUC_SYMBOL_TOKEN(LParen,        "(")\
        GPUC_SYMBOL_TOKEN(RParen,        ")")\
        GPUC_SYMBOL_TOKEN(Comma,         ",")\
        GPUC_SYMBOL_TOKEN(Semicolon,     ";")\
        GPUC_SYMBOL_TOKEN(Arrow,         "->")\
        GPUC_SYMBOL_TOKEN(Ellipsis,      "...")\
        GPUC_SYMBOL_TOKEN(Dot,           ".")\
        /* Ternary Operator */\
        GPUC_SYMBOL_TOKEN(Question,      "?")\
        GPUC_SYMBOL_TOKEN(Colon,         ":")\
        /* Integral Operators */\
        GPUC_SYMBOL_TOKEN(Compl,         "~")\
        GPUC_SYMBOL_TOKEN(LshEq,         "<<=")\
        GPUC_SYMBOL_TOKEN(Lsh,           "<<")\
        GPUC_SYMBOL_TOKEN(RshEq,         "<<=")\
        GPUC_SYMBOL_TOKEN(Rsh,           ">>")\
        GPUC_SYMBOL_TOKEN(OrEq,          "|=")\
        GPUC_SYMBOL_TOKEN(Or,            "|")\
        GPUC_SYMBOL_TOKEN(XorEq,         "^=")\
        GPUC_SYMBOL_TOKEN(Xor,           "^")\
        GPUC_SYMBOL_TOKEN(AndEq,         "&=")\
        GPUC_SYMBOL_TOKEN(And,           "&")\
        /* Relational Operators */\
        GPUC_SYMBOL_TOKEN(EqEq,          "==")\
        GPUC_SYMBOL_TOKEN(LtEq,          "<=")\
        GPUC_SYMBOL_TOKEN(Lt,            "<")\
        GPUC_SYMBOL_TOKEN(GtEq,          ">=")\
        GPUC_SYMBOL_TOKEN(Gt,            ">")\
        GPUC_SYMBOL_TOKEN(NotEq,         "!=")\
        /* Logical Operators */\
        GPUC_SYMBOL_TOKEN(OrOr,          "||")\
        GPUC_SYMBOL_TOKEN(AndAnd,        "&&")\
        GPUC_SYMBOL_TOKEN(Not,           "!")\
        /* Arithmetic Operators */\
        GPUC_SYMBOL_TOKEN(Inc,           "++")\
        GPUC_SYMBOL_TOKEN(Dec,           "--")\
        GPUC_SYMBOL_TOKEN(AddEq,         "+=")\
        GPUC_SYMBOL_TOKEN(Add,           "+")\
        GPUC_SYMBOL_TOKEN(SubEq,         "-=")\
        GPUC_SYMBOL_TOKEN(Sub,           "-")\
        GPUC_SYMBOL_TOKEN(DivEq,         "/=")\
        GPUC_SYMBOL_TOKEN(Div,           "/")\
        GPUC_SYMBOL_TOKEN(ModEq,         "%=")\
        GPUC_SYMBOL_TOKEN(Mod,           "%")\
        GPUC_SYMBOL_TOKEN(MulEq,         "*=")\
        GPUC_SYMBOL_TOKEN(Mul,           "*")\
        GPUC_SYMBOL_TOKEN(Eq,            "=")\

#define GPUC_TOKENS(GPUC_TOKEN)\
        GPUC_TOKEN(None,          "")\
        GPUC_TOKEN(Directive,     "")\
        GPUC_TOKEN(DoubleLiteral, "")\
        GPUC_TOKEN(FloatLiteral,  "")\
        GPUC_TOKEN(IntLiteral,    "")\
        GPUC_TOKEN(UIntLiteral,   "")\
        GPUC_TOKEN(Identifier,    "")\
        GPUC_KEYWORD_TOKENS(GPUC_TOKEN)\
        GPUC_TYPENAME_TOKENS(GPUC_TOKEN)\
        GPUC_SYMBOL_TOKENS(GPUC_TOKEN)\

typedef enum GpucToken {
    #define GPUC_TOKEN(TOKEN,...) GpucToken_##TOKEN,
    GPUC_TOKENS(GPUC_TOKEN)
    #undef GPUC_TOKEN
    GpucTokenCount,
} GpucToken;

static_assert(GpucTokenCount < 255, "NumGpucTokens < 255");

static inline const char* gpucTokenName(GpucToken token) {
    switch (token) {
        #define GPUC_TOKEN_NAME(TOKEN,...) \
            case GpucToken_##TOKEN: return #TOKEN;
        GPUC_TOKENS(GPUC_TOKEN_NAME)
        #undef GPUC_TOKEN_NAME
        default: return "<invalid GpucToken>";
    }
}

static inline const char* gpucTokenLiteral(GpucToken token) {
    switch (token) {
        #define GPUC_TOKEN_LITERAL(TOKEN, LITERAL) \
            case GpucToken_##TOKEN: return LITERAL;
        GPUC_TOKENS(GPUC_TOKEN_LITERAL)
        #undef GPUC_TOKEN_LITERAL
        default: return "<invalid GpucToken>";
    }
}

static inline bool gpucTokenIsKeyword(GpucToken token) {
    switch (token) {
        #define GPUC_KEYWORD_TOKEN(TOKEN,...)\
            case GpucToken_##TOKEN: return true;
        GPUC_KEYWORD_TOKENS(GPUC_KEYWORD_TOKEN)
        #undef GPUC_KEYWORD_TOKEN

        default: return false;
    }
}

static inline bool gpucTokenIsTypename(GpucToken token) {
    switch (token) {
        #define GPUC_TYPENAME_TOKEN(TOKEN,...)\
            case GpucToken_##TOKEN: return true;
        GPUC_TYPENAME_TOKENS(GPUC_TYPENAME_TOKEN)
        #undef GPUC_TYPENAME_TOKEN

        default: return false;
    }
}

static inline bool gpucTokenIsReservedWord(GpucToken token) {
    return gpucTokenIsKeyword(token)
        or gpucTokenIsTypename(token);
}

static inline bool gpucTokenIsEntryPoint(GpucToken token) {
    switch (token) {
        case GpucToken_Comp:
        case GpucToken_Frag:
        case GpucToken_Vert:
            return true;

        default: return false;
    }
}

static inline GpucStage gpucTokenStage(GpucToken token) {
    switch (token) {
        case GpucToken_Comp: return GpucStage_Compute;
        case GpucToken_Frag: return GpucStage_Fragment;
        case GpucToken_Vert: return GpucStage_Vertex;

        default: return GpucStage_None;
    }
}

static inline bool gpucTokenIsIdentifierOrEntryPoint(GpucToken token) {
    return token == GpucToken_Identifier
        or gpucTokenIsEntryPoint(token);
}

static inline bool gpucTokenIsIdentifierOrTypename(GpucToken token) {
    return token == GpucToken_Identifier
        or gpucTokenIsTypename(token);
}

static inline bool gpucTokenIsIdentifierOrReservedWord(GpucToken token) {
    return token == GpucToken_Identifier
        or gpucTokenIsReservedWord(token);
}

static inline bool gpucTokenIsAssignmentOperator(GpucToken token) {
    switch (token) {
        case GpucToken_LshEq: // <<=
        case GpucToken_RshEq: // <<=
        case GpucToken_AddEq: // +=
        case GpucToken_AndEq: // &=
        case GpucToken_DivEq: // /=
        case GpucToken_ModEq: // %=
        case GpucToken_MulEq: // *=
        case GpucToken_OrEq:  // |=
        case GpucToken_SubEq: // -=
        case GpucToken_XorEq: // ^=
        case GpucToken_Eq:    // =
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsEqualityOperator(GpucToken token) {
    switch (token) {
        case GpucToken_EqEq:  // ==
        case GpucToken_NotEq: // !=
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsRelationalOperator(GpucToken token) {
    switch (token) {
        case GpucToken_Lt:   // <
        case GpucToken_LtEq: // <=
        case GpucToken_Gt:   // >
        case GpucToken_GtEq: // >=
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsShiftOperator(GpucToken token) {
    switch (token) {
        case GpucToken_Lsh: // <<
        case GpucToken_Rsh: // >>
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsSumOperator(GpucToken token) {
    switch (token) {
        case GpucToken_Add: // +
        case GpucToken_Sub: // -
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsProductOperator(GpucToken token) {
    switch (token) {
        case GpucToken_Div: // /
        case GpucToken_Mod: // %
        case GpucToken_Mul: // *
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsPrefixOperator(GpucToken token) {
    switch (token) {
        case GpucToken_Inc:    // ++
        case GpucToken_Dec:    // --
        case GpucToken_Add:    // +
        case GpucToken_Sub:    // -
        case GpucToken_Not:    // !
        case GpucToken_Compl:  // ~
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsPostfixOperator(GpucToken token) {
    switch (token) {
        case GpucToken_Inc: // ++
        case GpucToken_Dec: // --
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsLiteral(GpucToken token) {
    switch (token) {
        case GpucToken_DoubleLiteral:
        case GpucToken_FalseLiteral:
        case GpucToken_FloatLiteral:
        case GpucToken_IntLiteral:
        case GpucToken_TrueLiteral:
        case GpucToken_UIntLiteral:
            return true;

        default: return false;
    }
}

static inline bool gpucTokenIsMatrix(GpucToken token) {
    switch (token) {
        case GpucToken_Float2x2:
        case GpucToken_Float2x3:
        case GpucToken_Float2x4:
        case GpucToken_Float3x2:
        case GpucToken_Float3x3:
        case GpucToken_Float3x4:
        case GpucToken_Float4x2:
        case GpucToken_Float4x3:
        case GpucToken_Float4x4:
            return true;

        default: return false;
    }
}
