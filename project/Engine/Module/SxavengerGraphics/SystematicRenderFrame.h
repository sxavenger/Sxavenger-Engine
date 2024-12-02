#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Texture/MultiViewTexture.h>

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

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using GBufferArray = std::array<T, static_cast<uint8_t>(GBuffer::kMaterial) + 1>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SystematicRenderFrame()  = default;
	~SystematicRenderFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* systematic option *//

	void TransitionBeginRenderTarget(const DirectXThreadContext* context);

	void TransitionEndRenderTarget(const DirectXThreadContext* context);

	void ClearRenderTarget(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV(uint32_t index) const { return buffers_[index]->GetCPUHandleRTV(); }

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const GBufferArray<DXGI_FORMAT> formats_;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	GBufferArray<std::unique_ptr<MultiViewTexture>> buffers_;

};