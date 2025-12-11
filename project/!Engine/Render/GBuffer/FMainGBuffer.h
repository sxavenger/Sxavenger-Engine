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
// FMainGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////
class FMainGBuffer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Layout enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Layout : uint8_t {
		Scene,
		UI,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FMainGBuffer()  = default;
	~FMainGBuffer() = default;

	void Init(const Vector2ui& size);

	//* scene transition option *//

	void TransitionBeginRenderTargetScene(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle);

	void TransitionEndRenderTargetScene(const DirectXQueueContext* context);

	void ClearRenderTargetScene(const DirectXQueueContext* context);

	void TransitionBeginUnorderedScene(const DirectXQueueContext* context);

	void TransitionEndUnorderedScene(const DirectXQueueContext* context);

	//* UI transition option *//

	void TransitionBeginRenderTargetUI(const DirectXQueueContext* context, const D3D12_CPU_DESCRIPTOR_HANDLE& depthStencilHandle);

	void TransitionEndRenderTargetUI(const DirectXQueueContext* context);

	void ClearRenderTargetUI(const DirectXQueueContext* context);

	void TransitionBeginUnorderedUI(const DirectXQueueContext* context);

	void TransitionEndUnorderedUI(const DirectXQueueContext* context);

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

	static inline const size_t kLayoutCount_ = magic_enum::enum_count<Layout>();

	static const std::array<DXGI_FORMAT, kLayoutCount_> kFormats_;

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount_> buffers_ = {};

};
