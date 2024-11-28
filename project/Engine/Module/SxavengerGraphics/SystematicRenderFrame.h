#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/Texture/MultiViewTexture.h>

//* c++
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////////////////
// SystematicRenderFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class SystematicRenderFrame {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBuffer enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class GBuffer : uint8_t {
		kAlbedo,
		kNormal,
		kPosition,
		kMaterial
	};
	static_assert(
		static_cast<uint8_t>(GBuffer::kMaterial) + 1 < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT,
		"[Systematic frame GBuffer]: The number of GBuffer exceeds <D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>."
	);

public:



};