#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// alignment 
////////////////////////////////////////////////////////////////////////////////////////////

//!< GPU Bufferに対するアライメント
constexpr size_t kAlignment = 16;
#define _GPU_BUFFER_ALIGNAS alignas(kAlignment)

//!< warning対策
#define _BEGIN_GPU_BUFFER_ALIGNAS __pragma(warning(push)) __pragma(warning(disable: 4324))
#define _END_GPU_BUFFER_ALIGNAS   __pragma(warning(pop))



