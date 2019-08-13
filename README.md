# GPUC

A generic shading language compiler that writes metal, HLSL, and GLSL

***GPUC is a work in progress, not ready for prime time.***

The primary motivation was to create a header-only shader language translator that has negligible effect on compile times.  Including this should not add more than a fraction of a second to your compile time, unlike other shader translators I tried.

Only the metal backend is actually useful right now, and probably only for simple shaders.  The HLSL and GLSL backends have rotted a bit due to some recent changes in the source language.

I initially wrote this over a two week holiday vacation in December of 2017.  I was amazed how easy it was to continue building upon when I returned to it many months later.  For me, this was the first reasonably useful thing I implemented in C, and since then I have switched to C for the majority of my hobby projects.

## Usage

### Header-only library

```c
// include API declarations as needed
#include "gpuc/gpuc.h"
```

```c
// include implementation in one translation unit
#include "gpuc/gpuc.inl"
```

### Stand-alone compiler

```sh
» sh gpuc.c --help
usage:

  gpuc [options] <source>

options:

  --help           display this usage summary
  --ast            print AST to stdout
  --debug          print debug summary to stdout
  --test           run tests
  --glsl           translate GPUC to GLSL
  --hlsl           translate GPUC to HLSL
  --metal          translate GPUC to Metal
  --frag <file>    write output to <file>, or '-' for stdout
  --vert <file>    write output to <file>, or '-' for stdout
```

```sh
» sh gpuc.c sample.gpuc --vert - --frag - --metal
```
```c
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

float4 sample(const texture2d<float> t, float2 uv) {
    constexpr sampler s(filter::nearest);
    return t.sample(s, uv);
}

float4 sample(const texture2d<float> t, float2 uv, const sampler s) {
    return t.sample(s, uv);
}

struct gpuc {

    // float4 sample(texture2d, float2);

    // float4 sample(texture2d, float2, sampler);

    struct Camera {
        float4x4 mvp;
        float4x4 mvn;
    };

    struct Vertex {
        float3 position [[attribute(0)]];
        float4 color [[attribute(1)]];
        float4 texcoords [[attribute(2)]];
    };

    struct Fragment {
        float4 position [[position]];
        float4 color;
        float4 texcoords;
    };

    struct Sample {
        float4 color;
    };

    constant const Camera& cam;

    const texture2d<float> color;

    const sampler samp;

    Fragment vert(const Vertex v) const {
        Fragment f;
        f.position = cam.mvp * float4(v.position, 1);
        f.color = v.color;
        f.texcoords = v.texcoords;
        return f;
    }

};

vertex gpuc::Fragment vert(
    constant const gpuc::Camera& cam [[buffer(0)]],
    texture2d<float> color [[texture(0)]],
    sampler samp [[sampler(0)]],
    const gpuc::Vertex v [[stage_in]]
) {
    return gpuc{cam, color, samp}.vert(v);
}
#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

float4 sample(const texture2d<float> t, float2 uv) {
    constexpr sampler s(filter::nearest);
    return t.sample(s, uv);
}

float4 sample(const texture2d<float> t, float2 uv, const sampler s) {
    return t.sample(s, uv);
}

struct gpuc {

    // float4 sample(texture2d, float2);

    // float4 sample(texture2d, float2, sampler);

    struct Camera {
        float4x4 mvp;
        float4x4 mvn;
    };

    struct Vertex {
        float3 position;
        float4 color;
        float4 texcoords;
    };

    struct Fragment {
        float4 position [[position]];
        float4 color;
        float4 texcoords;
    };

    struct Sample {
        float4 color;
    };

    constant const Camera& cam;

    const texture2d<float> color;

    const sampler samp;

    Sample frag(const Fragment f) const {
        Sample s;
        s.color = sample(color, f.texcoords.st, samp);
        return s;
    }

};

fragment gpuc::Sample frag(
    constant const gpuc::Camera& cam [[buffer(0)]],
    texture2d<float> color [[texture(0)]],
    sampler samp [[sampler(0)]],
    const gpuc::Fragment f [[stage_in]]
) {
    return gpuc{cam, color, samp}.frag(f);
}
```