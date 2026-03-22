#include "FReflectionGBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/System.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FReflectionGBuffer::kLayoutCount> FReflectionGBuffer::kFormats = {
	FMainGBuffer::kColorFormat,     //!< Reflection
	FMainGBuffer::kColorFormat,     //!< ReflectionRadiance
	DXGI_FORMAT_R16G16B16A16_FLOAT, //!< ReflectionPosition
};

//- Format
// Reflection:         [FMainGBuffer::ColorFormat]      float3 color, float NOT_USED
// ReflectionRadiance: [FMainGBuffer::ColorFormat]      float3 radiance, float NOT_USED
// ReflectionPosition: [DXGI_FORMAT_R16G16B16A16_FLOAT] float3 position, float isHit(= 0.0f or 1.0f)

////////////////////////////////////////////////////////////////////////////////////////////
// FReflectionGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FReflectionGBuffer::Init(const Vector2ui& size) {
	for (size_t j = 0; j < kLayoutCount; ++j) {
		// bufferの作成
		buffers_[j] = std::make_unique<FBaseTexture>(size, kFormats[j], FBaseTexture::Flag::All);

		// nameの設定
		std::string name = "FReflectionGBuffer | ";
		name += magic_enum::enum_name(static_cast<FReflectionGBuffer::Layout>(j));
		buffers_[j]->GetResource()->SetName(ToWString(name).c_str());
	}
}

FBaseTexture* FReflectionGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FReflectionGBuffer::GetFormat(Layout layout) {
	return kFormats[static_cast<size_t>(layout)];
}
