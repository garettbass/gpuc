#pragma once
#ifdef GPUC_H_25A109C0_B38B_4E2F_BFDD_8B6B28467F01
    #if GPUC_H_25A109C0_B38B_4E2F_BFDD_8B6B28467F01 < 100 ||\
        GPUC_H_25A109C0_B38B_4E2F_BFDD_8B6B28467F01 > 199
        #error "expected gpuc v1"
    #endif
#else
#define GPUC_H_25A109C0_B38B_4E2F_BFDD_8B6B28467F01 100
#include <stdbool.h>

//------------------------------------------------------------------------------

#ifdef __cplusplus
#define GPUC_EXTERN_C_BEGIN extern "C" {
#define GPUC_EXTERN_C_END   }
#else
#define GPUC_EXTERN_C_BEGIN /* extern "C" { */
#define GPUC_EXTERN_C_END   /* } */
#endif // __cplusplus

//------------------------------------------------------------------------------

#define GPUC(GPUC) _GPUC(GPUC)
#define _GPUC(GPUC) #GPUC

//------------------------------------------------------------------------------

GPUC_EXTERN_C_BEGIN

//------------------------------------------------------------------------------

typedef struct GpucSubstring {
    const char* head;
    unsigned    length;
} GpucSubstring;

//------------------------------------------------------------------------------

typedef struct GpucLocation {
    GpucSubstring file;
    GpucSubstring site;
    GpucSubstring context;
    unsigned      line;
    unsigned      column;
} GpucLocation;

size_t gpucLocationStringWrite(char** out, const GpucLocation*);

size_t gpucLocationStringLength(const GpucLocation*);

const char* gpucLocationStringAlloc(const GpucLocation*);

void gpucLocationPrint(const GpucLocation*);

//------------------------------------------------------------------------------

typedef enum GpucDiagnosticLevel {
    GpucDiagnosticLevel_Note,
    GpucDiagnosticLevel_Warning,
    GpucDiagnosticLevel_Error,
} GpucDiagnosticLevel;

typedef struct GpucDiagnostic {
    GpucDiagnosticLevel level;
    GpucLocation        location;
    char                message[];
} GpucDiagnostic;

size_t gpucDiagnosticStringWrite(char** out, const GpucDiagnostic*);

size_t gpucDiagnosticStringLength(const GpucDiagnostic*);

const char* gpucDiagnosticStringAlloc(const GpucDiagnostic*);

void gpucDiagnosticPrint(const GpucDiagnostic*);

void gpucDiagnosticPrintLine(const GpucDiagnostic*);


//------------------------------------------------------------------------------

typedef enum GpucLanguage {
    GpucLanguage_Default,

    #define GpucLanguage(A,B,C,D)\
            ((unsigned)(\
                (((unsigned)(A)) << 24)|\
                (((unsigned)(B)) << 16)|\
                (((unsigned)(C)) <<  8)|\
                (((unsigned)(D)) <<  0)))

    GpucLanguage_GPUC     = GpucLanguage('G','P', 0,0),
    GpucLanguage_GLSL_330 = GpucLanguage('G','L', 3,3),
    GpucLanguage_HLSL_11  = GpucLanguage('H','L',11,0),
    GpucLanguage_Metal_1  = GpucLanguage('M','E', 1,0),

    #undef GpucLanguage
} GpucLanguage;

typedef enum GpucStage {
    GpucStage_None,
    GpucStage_Compute,
    GpucStage_Vertex,
    GpucStage_Fragment,
} GpucStage;

//------------------------------------------------------------------------------

typedef struct GpucModule GpucModule;

GpucModule* gpucModuleAlloc(const char* file, const char* source);

void gpucModuleFree(GpucModule*);

bool gpucModuleCompile(GpucModule*);

void gpucModulePrintDebug(const GpucModule*);

void gpucModulePrintDiagnostics(const GpucModule* const module);

void gpucModulePrintLexemes(const GpucModule* const module);

void gpucModulePrintAST(const GpucModule* const module);

const GpucDiagnostic* gpucModuleGetDiagnostic(const GpucModule*, unsigned index);

const char* gpucModuleGetTranslation(
    GpucModule*,
    GpucStage,
    GpucLanguage);

//------------------------------------------------------------------------------

int gpucTests(void);

//------------------------------------------------------------------------------

GPUC_EXTERN_C_END

#endif // GPUC_H_25A109C0_B38B_4E2F_BFDD_8B6B28467F01
