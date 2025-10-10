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
		Indirect_Reservoir_Diffuse,
		Indirect_Reservoir_Specular,
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

	void CopyIntermediateToGBuffer(const DirectXQueueContext* context, Layout layout);

	void CopyGBufferToIntermediate(const DirectXQueueContext* context, Layout layout);

	//* getter *//

	FBaseTexture* GetGBuffer(Layout layout) const;
	FBaseTexture* GetIntermediate(Layout layout) const;

	static DXGI_FORMAT GetFormat(Layout layout);

	FRenderCorePathtracing::Config& GetConfig() { return config_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const size_t kLayoutCount_ = magic_enum::enum_count<Layout>();

	static const std::array<DXGI_FORMAT, kLayoutCount_> kFormats_;

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount_> buffers_      = {};
	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount_> intermediate_ = {};

	FRenderCorePathtracing::Config config_ = {};

};
