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
// FDeferredGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FDeferredGBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Albedo,
		Normal,
		MaterialARM,
		Position, //!< Hack: depthから復元できるように調整
		Velocity
	};

	static inline const size_t kLayoutCount = magic_enum::enum_count<Layout>();

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Vector2ui& size);

	//* transition option *//

	void TransitionBeginRenderTarget(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle);

	void TransitionEndRenderTarget(const DirectXQueueContext* context);

	void ClearRenderTarget(const DirectXQueueContext* context);

	void TransitionBeginUnordered(const DirectXQueueContext* context);

	void TransitionEndUnordered(const DirectXQueueContext* context);

	//* option *//

	void ForEach(const std::function<void(FBaseTexture*)>& funciton);
	void ForEach(const std::function<void(size_t, FBaseTexture*)>& funciton);

	//* getter *//

	FBaseTexture* GetGBuffer(Layout layout) const;

	static DXGI_FORMAT GetFormat(Layout layout);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const std::array<DXGI_FORMAT, kLayoutCount> kFormats;

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount> buffers_ = {};

};

SXAVENGER_ENGINE_NAMESPACE_END
