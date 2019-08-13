///usr/bin/env \
    [ -n "${PATHEXT}" ] && ext='.exe'; \
    bin="$(dirname $0)/$(basename ${0%.*})$ext"; \
    cc -std=c11 -Werror -o $bin $0 \
    && \
    $bin "$@"; \
    status=$?; \
    rm $bin; \
    exit $status

#include <iso646.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gpuc.implementation.h"
#include "implementation/array/array_tests.c"
#include "argv.h"

//------------------------------------------------------------------------------

static inline
const char* fload(const char* const path) {
    if (not path)
        return NULL;

    FILE* const file = fopen(path, "r");
    if (not file)
        return NULL;

    fseek(file, 0, SEEK_END);
    const long size = ftell(file);
    rewind(file);

    char* const data = (char*)calloc(size + 1, sizeof(char));
    fread(data, sizeof(char), size, file);
    fclose(file);

    return data;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static inline
bool fsave(const char* const path, const char* const data) {
    FILE* const file = strcmp(path, "-") ? fopen(path, "w") : stdout;
    if (not file)
        return false;

    fwrite(data, sizeof(char), strlen(data), file);

    if (file != stdout)
        fclose(file);

    return true;
}

//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
#endif // __cplusplus
void enableUTF8(void) {
    #ifdef _WIN32
    // enable UTF-8 output on Windows
    extern int SetConsoleOutputCP(unsigned wCodePageID);
    SetConsoleOutputCP(65001);
    #endif
}

//------------------------------------------------------------------------------

int main(int argc, const char* argv[], const char* envp[]) {
    enableUTF8();

    const bool        help  = argv(bool,   "help");
    const bool        ast   = argv(bool,   "ast");
    const bool        debug = argv(bool,   "debug");
    const bool        test  = argv(bool,   "test");
    const bool        glsl  = argv(bool,   "glsl");
    const bool        hlsl  = argv(bool,   "hlsl");
    const bool        metal = argv(bool,   "metal");
    const char* const frag  = argv(string, "frag");
    const char* const vert  = argv(string, "vert");
    const char* const file  = argv(string, NULL);

    if (argv_print_illegal_arguments(argv)) {
        return 1;
    }

    if (help or not file) {
        const char* name = argv[0];
        for (const char* p = name; *p; ++p) {
            if (*p == '\\' or *p == '/')
                name = p + 1;
        }
        puts("usage:\n");
        printf("  %s [options] <source>\n\n", name);
        puts("options:\n");
        puts("  --help           display this usage summary");
        puts("  --ast            print AST to stdout");
        puts("  --debug          print debug summary to stdout");
        puts("  --test           run tests");
        puts("  --glsl           translate GPUC to GLSL");
        puts("  --hlsl           translate GPUC to HLSL");
        puts("  --metal          translate GPUC to Metal");
        puts("  --frag <file>    write output to <file>, or '-' for stdout");
        puts("  --vert <file>    write output to <file>, or '-' for stdout");
        return 1;
    }

    if (test) {
        array_tests();
        if (gpucTests()) {
            return 1;
        }
    }

    const char* const source = fload(file);
    if (not source) {
        printf("file not found: '%s'\n", file?file:"");
        return 1;
    }

    GpucModule* const module = gpucModuleAlloc(file, source);
    if (not gpucModuleCompile(module)) {
        if (debug) {
            gpucModulePrintDebug(module);
        } else {
            gpucModulePrintDiagnostics(module);
        }
        gpucModuleFree(module);
        free((void*)source);
        return 1;
    }

    if (ast) {
        gpucModulePrintAST(module);
    }

    if (debug) {
        gpucModulePrintDebug(module);
    }

    GpucLanguage language = GpucLanguage_GPUC;
    if (glsl)  { language = GpucLanguage_GLSL_330; }
    if (hlsl)  { language = GpucLanguage_HLSL_11;  }
    if (metal) { language = GpucLanguage_Metal_1;  }

    if (vert) {
        const GpucStage stage = GpucStage_Vertex;
        const char* out = gpucModuleGetTranslation(module, stage, language);
        fsave(vert, out);
    }

    if (frag) {
        const GpucStage stage = GpucStage_Fragment;
        const char* out = gpucModuleGetTranslation(module, stage, language);
        fsave(frag, out);
    }

    gpucModuleFree(module);
    free((void*)source);

    return 0;
}