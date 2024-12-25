#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* graphics
#include "SystematicRenderFrame.h"
#include "XclipseProcessFrame.h"
#include "AdaptiveRenderFrame.h"
#include "VisualProcessFrame.h"

//* depth
#include "DepthBufferController.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Camera3d;

////////////////////////////////////////////////////////////////////////////////////////////
// SxavGraphicsFrameType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class SxavGraphicsFrameType : uint8_t {
	kNone,
	kDebug,
	kGame,
};

////////////////////////////////////////////////////////////////////////////////////////////
// SxavGraphicsFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavGraphicsFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SxavGraphicsFrame()  = default;
	~SxavGraphicsFrame() = default;

	void Create(const Vector2ui& size, SxavGraphicsFrameType type = SxavGraphicsFrameType::kNone);

	void Term();

	//* render option *//

	void BeginSystematic(const DirectXThreadContext* context);

	void EndSystematic(const DirectXThreadContext* context);

	void ClearSystematic(const DirectXThreadContext* context);

	void BeginAdaptive(const DirectXThreadContext* context);

	void EndAdaptive(const DirectXThreadContext* context);

	void ClearRasterizerDepth(const DirectXThreadContext* context);

	//* process option *//

	void BeginXclipse(const DirectXThreadContext* context);

	void EndXclipse(const DirectXThreadContext* context);

	void BeginVisual(const DirectXThreadContext* context);

	void EndVisual(const DirectXThreadContext* context);

	//* transition *//

	//void TransitionSystematicToXclipse(const DirectXThreadContext* context);

	void TransitionXclipseToAdaptive(const DirectXThreadContext* context);
	void TransitionAdaptiveToVisual(const DirectXThreadContext* context);
	void TransitionVisualToAdaptive(const DirectXThreadContext* context);

	//* getter *//

	SystematicRenderFrame* GetSystematic() const { return systematic_.get(); }
	XclipseProcessFrame* GetXclipse() const { return xclipse_.get(); }
	AdaptiveRenderFrame* GetAdaptive() const { return adaptive_.get(); }
	VisualProcessFrame* GetVisual() const { return visual_.get(); }

	DepthBufferController* GetDepthBufferController() const { return depthBuffer_.get(); }


	const Vector2ui& GetSize() const { return (*config_)[0].size; }

	Camera3d* GetCamera() const { return camera_; }

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGetConfigVirtualAddress() const { return config_->GetGPUVirtualAddress(); }

	DxObject::BindBufferDesc GetTransitionSystematicBindDesc() const;

	SxavGraphicsFrameType GetType() const { return type_; }

	//* setter *//

	void SetCamera(Camera3d* camera) { camera_ = camera; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Config structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Config {
		Vector2ui size;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Camera3d* camera_ = nullptr;

	//* sxav graphics frames *//

	std::unique_ptr<SystematicRenderFrame> systematic_;
	std::unique_ptr<XclipseProcessFrame>   xclipse_;
	std::unique_ptr<AdaptiveRenderFrame>   adaptive_;
	std::unique_ptr<VisualProcessFrame>    visual_;

	//* depth

	std::unique_ptr<DepthBufferController> depthBuffer_;

	//* buffer

	std::unique_ptr<DxObject::DimensionBuffer<Config>> config_;

	//* config

	SxavGraphicsFrameType type_ = SxavGraphicsFrameType::kNone;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CopyTexture(const DirectXThreadContext* context, const MultiViewTextureBuffer* dst, const MultiViewTextureBuffer* src);
	// HACK: Dispatch式に変更

};
