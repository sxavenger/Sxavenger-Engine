#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// alignment 
////////////////////////////////////////////////////////////////////////////////////////////

//!< GPU Bufferに対するアライメント
constexpr const size_t kAlignment = 16;
#define GPU_BUFFER_ALIGNAS alignas(kAlignment)

//!< warning対策
#define PUSH_GPU_BUFFER_ALIGNAS __pragma(warning(push)) __pragma(warning(disable: 4324))
#define POP_GPU_BUFFER_ALIGNAS  __pragma(warning(pop))



