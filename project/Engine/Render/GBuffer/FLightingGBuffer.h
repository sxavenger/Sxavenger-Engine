#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FBaseTexture.h"
#include "../Core/FRenderCorePathtracing.h"

//* engine
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// FLightingGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FLightingGBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Direct,
		Indirect_Atlas_Diffuse,
		Indirect_Atlas_Specular, 
		Indirect_Reservoir_Diffuse,
		Indirect_Reservoir_Specular,
		Indirect_Resolution_Diffuse,
		Indirect_Resolution_Specular,
		Indirect_Moment,
		Indirect,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FLightingGBuffer()  = default;
	~FLightingGBuffer() = default;

	void Init(const Vector2ui& size);

	//* direct transition option *//

	void TransitionBeginRenderTargetDirect(const DirectXQueueContext* context);

	void TransitionEndRenderTargetDirect(const DirectXQueueContext* context);

	void ClearRenderTargetDirect(const DirectXQueueContext* context);

	void TransitionBeginUnorderedDirect(const DirectXQueueContext* context);

	void TransitionEndUnorderedDirect(const DirectXQueueContext* context);

	//* indirect transition option *//

	void TransitionBeginRenderTargetIndirect(const DirectXQueueContext* context);

	void TransitionEndRenderTargetIndirect(const DirectXQueueContext* context);

	void ClearRenderTargetIndirect(const DirectXQueueContext* context);

	//* getter *//

	FBaseTexture* GetGBuffer(Layout layout) const;

	static DXGI_FORMAT GetFormat(Layout layout);

	FRenderCorePathtracing::Config& GetConfig() { return config_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const size_t kLayoutCount_ = magic_enum::enum_count<Layout>();

	//* format *//

	static const std::array<DXGI_FORMAT, kLayoutCount_> kFormats_;

	//* buffer *//

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount_> buffers_      = {};
	//std::array<std::unique_ptr<FBaseTexture>, kLayoutCount_> intermediate_ = {};

	//* parameter *//

	FRenderCorePathtracing::Config config_ = {};

	Vector2ui downsize_ = {};
	uint32_t atlas_     = NULL;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static uint32_t GetAtlasSize(uint32_t sampleCount);

};
