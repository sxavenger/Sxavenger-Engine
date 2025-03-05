#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// alignment 
////////////////////////////////////////////////////////////////////////////////////////////

//!< GPU Bufferに対するアライメント
constexpr size_t kAlignment = sizeof(float) * 4;
#define _GPU_BUFFER_ALIGNAS alignas(kAlignment)

//!< warning対策
#define _PUSH_GPU_BUFFER_ALIGNAS __pragma(warning(push)) __pragma(warning(disable: 4324))
#define _POP_GPU_BUFFER_ALIGNAS  __pragma(warning(pop))



