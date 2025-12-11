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

	//- Format
	// Albedo:      [FMainGBuffer::ColorFormat]      float3 albedo, float NOT_USED
	// Normal:      [DXGI_FORMAT_R10G10B10A2_UNORM]  float3 normal, float NOT_USED
	// MaterialARM: [DXGI_FORMAT_R8G8B8A8_UNORM]     float ambient_occlusion, float roughness, float metallic, float NOT_USED
	// Position:    [DXGI_FORMAT_R32G32B32A32_FLOAT] float3 position, float NOT_USED
	// Velocity:    [DXGI_FORMAT_R16G16B16A16_FLOAT] float2 velocity, float2 NOT_USED

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FDeferredGBuffer()  = default;
	~FDeferredGBuffer() = default;

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

	static const std::array<DXGI_FORMAT, kLayoutCount> kFormats_;

	std::array<std::unique_ptr<FBaseTexture>, kLayoutCount> buffers_ = {};

};
