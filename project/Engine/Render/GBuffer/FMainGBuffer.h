#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Common/FBaseTexture.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* external
#include <magic_enum.hpp>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FMainGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FMainGBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Scene,
		Canvas,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Vector2ui& size);

	//* transition option *//

	void TransitionBeginRenderTarget(const DirectXQueueContext* context, Layout layout, const D3D12_CPU_DESCRIPTOR_HANDLE& handleDSV);

	void TransitionEndRenderTarget(const DirectXQueueContext* context, Layout layout);

	void ClearRenderTarget(const DirectXQueueContext* context, Layout layout);

	void TransitionBeginUnordered(const DirectXQueueContext* context, Layout layout);

	void TransitionEndUnordered(const DirectXQueueContext* context, Layout layout);

	//* getter *//

	FBaseTexture* GetGBuffer(Layout layout) const;

	static DXGI_FORMAT GetFormat(Layout layout);

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const DXGI_FORMAT kColorFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

	static const std::array<DXGI_FORMAT, kLayoutCount> kFormats;

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount> buffers_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
