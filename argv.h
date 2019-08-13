#pragma once
#ifndef ARGV_H_7D43C499_77B8_4062_8F43_245354FD2C4C
#define ARGV_H_7D43C499_77B8_4062_8F43_245354FD2C4C
#include <assert.h>
#include <iso646.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


//------------------------------------------------------------------------------


static inline
bool arg_skip_prefix(const char** arg, const char* const prefix) {
    const char* p = prefix;
    const char* a = *arg;
    while (*p and *a and *p == *a) { ++p; ++a; }
    if (*p == '\0') {
        // string starts with prefix
        *arg = a;
        return true;
    }
    return false;
}


static inline
bool arg_skip_name(const char** arg, const char* const name) {
    if (name) {
        const char* s = *arg;
        if (arg_skip_prefix(&s, "--") and
            arg_skip_prefix(&s, name)) {
            *arg = s;
            return true;
        }
    }
    return false;
}


static inline
bool arg_skip_flag(const char** arg, const char flag) {
    if (flag) {
        const char* s = *arg;
        if (*s++ == '-' and *s++ == flag) {
            *arg = s;
            return true;
        }
    }
    return false;
}


static inline
bool arg_skip_name_or_flag(
    const char**      arg,
    const char* const name,
    const char        flag)
{
    return arg_skip_name(arg, name)
        or arg_skip_flag(arg, flag);
}


//------------------------------------------------------------------------------


static inline
bool arg_to_char(const char* arg, char* out)
{
    if (arg == NULL) return false;
    if (arg[0] == 0) return false;
    if (arg[1] != 0) return false;
    *out = *arg;
    return true;
}


static inline
bool arg_to_int(const char* arg, int* out)
{
    if (arg == NULL) return false;
    char* end = NULL;
    const long l = strtol(arg, &end, 0);
    if (end <= arg) return false;
    if (l < INT_MIN or l > INT_MAX) {
        printf("error: argument out of range: %s\n", arg);
        exit(1);
    }
    *out = (int)l;
    return true;
}


static inline
bool arg_to_unsigned(const char* arg, unsigned* out)
{
    if (arg == NULL) return false;
    char* end = NULL;
    const unsigned long ul = strtoul(arg, &end, 0);
    if (end <= arg) return false;
    if (ul > ~0u) {
        printf("error: argument out of range: %s\n", arg);
        exit(1);
    }
    *out = (unsigned)ul;
    return true;
}
#define arg_to_uint arg_to_unsigned


static inline
bool arg_to_float(const char* arg, float* out)
{
    if (arg == NULL) return false;
    char* end = NULL;
    const float f = strtof(arg, &end);
    if (end <= arg) return false;
    *out = f;
    return true;
}


static inline
bool arg_to_double(const char* arg, double* out)
{
    if (arg == NULL) return false;
    char* end = NULL;
    const double d = strtod(arg, &end);
    if (end <= arg) return false;
    *out = d;
    return true;
}


//------------------------------------------------------------------------------


const char* argv_consume(int* argc, const char** argp) {
    const char* const arg = *argp;
    while (*argp) { *argc -= 1; argp[0] = argp[1]; ++argp; }
    return arg;
}


//------------------------------------------------------------------------------


typedef struct argv_bool_params {
    const char* const name;
    const char        flag;
    bool              init;
} argv_bool_params;


bool argv_bool(int* argc, const char** argv, argv_bool_params p) {
    const char** argp = argv + 1;
    while (*argp) {
        const char* arg = *argp;
        if (arg_skip_name_or_flag(&arg, p.name, p.flag)) {
            if (arg[0] == 0) {
                argv_consume(argc, argp); return true;
            }
            if (arg[0] == '=') {
                printf("error: invalid argument %s\n", *argp);
                exit(1);
            }
        }
        ++argp;
    }
    return p.init;
}


//------------------------------------------------------------------------------


typedef struct argv_string_params {
    const char* const name;
    const char        flag;
    const char*       init;
} argv_string_params;


const char* argv_string(int* argc, const char** argv, argv_string_params p) {
    const char** argp = argv + 1;
    const bool unnamed = not p.name and not p.flag;
    while (*argp) {
        const char* arg = *argp;
        if (unnamed) {
            return argv_consume(argc, argp);
        }
        if (arg_skip_name_or_flag(&arg, p.name, p.flag)) {
            if (arg[0] == 0) {
                argv_consume(argc, argp); return argv_consume(argc, argp);
            }
            if (arg[0] == '=') {
                argv_consume(argc, argp); return ++arg;
            }
        }
        ++argp;
    }
    return p.init;
}
#define argv_string_(argc, argv, name, flag) \
        argv_string(argc, argv, (argv_string_params){ name, flag })


//------------------------------------------------------------------------------


typedef struct argv_char_params {
    const char* const name;
    const char        flag;
    char              init;
} argv_char_params;


char argv_char(int* argc, const char** argv, argv_char_params p) {
    const char* arg = argv_string_(argc, argv, p.name, p.flag);
    if (arg and not arg_to_char(arg, &p.init)) {
        printf("error: expected letter '%s'\n", arg);
        exit(1);
    }
    return p.init;
}


//------------------------------------------------------------------------------


typedef struct argv_int_params {
    const char* const name;
    const char        flag;
    int               init;
} argv_int_params;


int argv_int(int* argc, const char** argv, argv_int_params p) {
    const char* arg = argv_string_(argc, argv, p.name, p.flag);
    if (arg and not arg_to_int(arg, &p.init)) {
        printf("error: expected number: '%s'\n", arg);
        exit(1);
    }
    return p.init;
}


//------------------------------------------------------------------------------


typedef struct argv_unsigned_params {
    const char* const name;
    const char        flag;
    unsigned          init;
} argv_unsigned_params;
#define argv_uint_params argv_unsigned_params


unsigned argv_unsigned(int* argc, const char** argv, argv_unsigned_params p) {
    const char* arg = argv_string_(argc, argv, p.name, p.flag);
    if (arg and not arg_to_unsigned(arg, &p.init)) {
        printf("error: expected number: '%s'\n", arg);
        exit(1);
    }
    return p.init;
}
#define argv_uint argv_unsigned


//------------------------------------------------------------------------------


typedef struct argv_float_params {
    const char* const name;
    const char        flag;
    float             init;
} argv_float_params;


float argv_float(int* argc, const char** argv, argv_float_params p) {
    const char* arg = argv_string_(argc, argv, p.name, p.flag);
    if (arg and not arg_to_float(arg, &p.init)) {
        printf("error: expected number: '%s'\n", arg);
        exit(1);
    }
    return p.init;
}


//------------------------------------------------------------------------------


typedef struct argv_double_params {
    const char* const name;
    const char        flag;
    double            init;
} argv_double_params;


double argv_double(int* argc, const char** argv, argv_double_params p) {
    const char* arg = argv_string_(argc, argv, p.name, p.flag);
    if (arg and not arg_to_double(arg, &p.init)) {
        printf("error: expected number: '%s'\n", arg);
        exit(1);
    }
    return p.init;
}


//------------------------------------------------------------------------------


/// argv(type, name[, flag[, init]])
#define argv(TYPE, NAME, ...) \
        argv_##TYPE(&argc, argv, (argv_##TYPE##_params){ NAME, __VA_ARGS__ })


//------------------------------------------------------------------------------


void argv_print_arguments(const char** argv) {
    while (*argv) puts(*argv++);
}


int argv_print_illegal_arguments(const char** argv) {
    int count = 0;
    const char** argp = argv + 1;
    while (*argp) {
        printf("error: illegal argument: '%s'\n", *argp++);
        ++count;
    }
    return count;
}


//------------------------------------------------------------------------------


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


#endif // ARGV_H_7D43C499_77B8_4062_8F43_245354FD2C4C