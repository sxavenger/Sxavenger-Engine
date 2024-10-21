#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxBufferResource.h>
#include <Engine/Game/Camera/Camera3D.h>
#include <Engine/Game/SxavengerPipeline/MultiViewTexture.h>
#include <Engine/Game/SxavengerPipeline/DepthStencilTexture.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <array>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// SystematicRenderingFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class SystematicRenderingFrame {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// GBuffer enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum GBuffer : uint32_t {
		GBUFFER_ALBEDO,
		GBUFFER_NORMAL,
		GBUFFER_POSITION,
		kCountOfGBuffer
	};
	static_assert(kCountOfGBuffer < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, "[Systematic frame GBuffer]: The number of GBuffer exceeds <D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>.");

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using GBufferArray = std::array<T, GBuffer::kCountOfGBuffer>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SystematicRenderingFrame()  = default;
	~SystematicRenderingFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* option *//

	void BeginRenderingBuffer(bool isClearRenderTerget = false);
	void EndRenderingBuffer();

	//* getter *//

	const GBufferArray<std::unique_ptr<MultiViewTexture>>& GetBuffers() const { return buffers_; }
	MultiViewTexture* GetTexture(GBuffer type) const { return buffers_.at(type).get(); }

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

////////////////////////////////////////////////////////////////////////////////////////////
// XclipseProcessFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class XclipseProcessFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	XclipseProcessFrame()  = default;
	~XclipseProcessFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* option *//

	void BeginUnorderedAccess();
	void EndUnorderedAccess();

	//* getter *//

	MultiViewTexture* GetTexture() const { return buffer_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	std::unique_ptr<MultiViewTexture> buffer_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// AdaptiveRenderingFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class AdaptiveRenderingFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AdaptiveRenderingFrame()  = default;
	~AdaptiveRenderingFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* option *//

	void BeginRenderingBuffer();
	void EndRenderingBuffer();

	//* getter *//

	const MultiViewTexture* GetTexture() const { return buffer_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<MultiViewTexture> buffer_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// VisualProcessFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class VisualProcessFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	VisualProcessFrame()  = default;
	~VisualProcessFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* index option *//

	void AdvanceResultBufferIndex();

	void ResetResultBufferIndex();

	//* getter *//

	MultiViewTexture* GetResultBuffer() const { return buffers_.at(resultBufferIndex_).get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	static const uint32_t kProcessBufferNum_ = 2;
	uint32_t resultBufferIndex_ = 0;

	//* buffer *//

	std::array<std::unique_ptr<MultiViewTexture>, kProcessBufferNum_> buffers_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetReferenceTextureHandle(uint32_t prevIndex);

private:
	static_assert(kProcessBufferNum_ >= 2, "Process Texture must be at least 2.");
	//!< process bufferは2つ以上で作成する
};

////////////////////////////////////////////////////////////////////////////////////////////
// ScreenPresenter class
////////////////////////////////////////////////////////////////////////////////////////////
class ScreenPresenter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ScreenPresenter()  = default;
	~ScreenPresenter() { Term(); }

	void Init();

	void Term();

	void Present(const D3D12_GPU_DESCRIPTOR_HANDLE& handle);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ScreenPresentIA structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ScreenPresentIA {
		Vector4f position;
		Vector2f texcoord;
	};

	//=========================================================================================
	// public methods
	//=========================================================================================

	std::unique_ptr<DxObject::BufferResource<ScreenPresentIA>> ia_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SxavengerFrame()  = default;
	~SxavengerFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* rendering option *//

	void BeginSystematic(bool isDepthClear = false, bool isRenderTargetClear = false);
	void EndSystematic();

	void BeginAdaptive(bool isDepthClear = false);
	void EndAdaptive();

	//* transition methods *//
	
	void TransitionSystematicToXclipse();
	void TransitionXclipseToAdaptive();
	void TransitionAdaptiveToVisual();
	void TransitionVisualToAdaptive();

	//* present methods *//

	void PresentAdaptiveToScreen();

	//* getter *//

	const MultiViewTexture* GetAdaptiveTexture() const { return adaptive_->GetTexture(); }

	const Camera3D* GetCamera() const { return camera_; }

	//* setter *//

	void SetCamera(const Camera3D* camera) { camera_ = camera; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// SxavengerFrameConfig structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SxavengerFrameConfig {
		Vector2ui size;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	const Camera3D* camera_ = nullptr;

	//* sxavenger frames *//

	std::unique_ptr<SystematicRenderingFrame> systematic_; //!< deferred rendering
	std::unique_ptr<XclipseProcessFrame>      xclipse_;    //!< deferred process
	std::unique_ptr<AdaptiveRenderingFrame>   adaptive_;   //!< forward rendering
	std::unique_ptr<VisualProcessFrame>       visual_;     //!< visual frame process

	//* depth texture *//

	std::unique_ptr<DepthStencilTexture> depthStencilTexture_;

	//* config buffer *//

	std::unique_ptr<DxObject::BufferResource<SxavengerFrameConfig>> config_;

	//* presenter *//

	std::unique_ptr<ScreenPresenter> presenter_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CopyTexture(const MultiViewTexture* dst, const MultiViewTexture* src);

};

