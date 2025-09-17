#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FBaseTexture.h"

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
		Indirect_Reservoir,
		Indirect_Moment,
		Indirect,
		// Direct Diffuse, Direct Specular, Indirect Diffuse, Indirect Specular に分割するかも
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

	void TransitionBeginUnorderedIndirect(const DirectXQueueContext* context);

	void TransitionEndUnorderedIndirect(const DirectXQueueContext* context);

	//* getter *//

	FBaseTexture* GetGBuffer(Layout layout) const;

	static DXGI_FORMAT GetFormat(Layout layout);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const size_t kLayoutCount_ = magic_enum::enum_count<Layout>();

	static const std::array<DXGI_FORMAT, kLayoutCount_> kFormats_;

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount_> buffers_ = {};

};
