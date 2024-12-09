#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Texture/MultiViewTexture.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AdaptiveRenderFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class AdaptiveRenderFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AdaptiveRenderFrame()  = default;
	~AdaptiveRenderFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* adaptive option *//

	void TransitionBeginRenderTarget(const DirectXThreadContext* context);

	void TransitionEndRenderTarget(const DirectXThreadContext* context);

	void ClearRenderTarget(const DirectXThreadContext* context);

	//* getter *//

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandleRTV() const { return buffer_->GetCPUHandleRTV(); }

	const MultiViewTexture* GetTexture() const { return buffer_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<MultiViewTexture> buffer_;

};