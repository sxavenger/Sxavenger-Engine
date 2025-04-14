#pragma once
//!< https://github.com/microsoft/DirectXShaderCompiler/wiki/16-Bit-Scalar-Types

typedef int   int32_t;
typedef uint  uint32_t;
typedef float float32_t;

typedef int64_t  int64_t;
typedef uint64_t uint64_t;

#ifdef _16BIT_SCALAR_TYPES //!< warning: hlsl version 2018 or later (version_6_2 over)
typedef int16_t   int16_t;
typedef uint16_t  uint16_t;
typedef float16_t float16_t;
#endif

