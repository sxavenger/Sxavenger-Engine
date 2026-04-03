#include "FGBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FGBuffer::kLayoutCount> FGBuffer::kFormats = {
	FBaseBuffer::kColorFormat,      //!< Albedo
	DXGI_FORMAT_R10G10B10A2_UNORM,  //!< Normal
	DXGI_FORMAT_R8G8B8A8_UNORM,     //!< MaterialARM
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< MotionVector
	DXGI_FORMAT_R32G32_UINT         //!< Address
};

//- Format
// Albedo:       [FBaseBuffer::ColorFormat]       float3 albedo
// Normal:       [DXGI_FORMAT_R10G10B10A2_UNORM]  float3 normal
// MaterialARM:  [DXGI_FORMAT_R8G8B8A8_UNORM]     float ambient_occlusion, float roughness, float metallic
// MotionVector: [DXGI_FORMAT_R16G16B16A16_FLOAT] float2 motion_vector
// Address:      [DXGI_FORMAT_R32G32_UINT]        uint2 address(= uintptr_tと同等[x: upper, y: lower])

////////////////////////////////////////////////////////////////////////////////////////////
// FGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FGBuffer::Create(const Vector2ui& resolution) {
	for (size_t i = 0; i < kLayoutCount; ++i) {
		buffers_[i].Create({ resolution, kFormats[i] });

		// nameの設定
		std::string name = "FGBuffer | ";
		name += magic_enum::enum_name(static_cast<FGBuffer::Layout>(i));
		buffers_[i].SetName(name);
	}
}
