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
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Module/Camera/Camera3d.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SystematicRenderFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavGraphicsFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SxavGraphicsFrame()  = default;
	~SxavGraphicsFrame() = default;

	void Create(const Vector2ui& size);

	void Term();

	//* render option *//

	void BeginSystematic(const DirectXThreadContext* context);

	void EndSystematic(const DirectXThreadContext* context);

	void ClearSystematic(const DirectXThreadContext* context);

	void BeginAdaptive(const DirectXThreadContext* context);

	void EndAdaptive(const DirectXThreadContext* context);

	void ClearRasterizerDepth(const DirectXThreadContext* context);

	//* transition *//

	//void TransitionSystematicToXclipse(const DirectXThreadContext* context);

	void TransitionXclipseToAdaptive(const DirectXThreadContext* context);

	//* getter *//

	const Vector2ui& GetSize() const { return (*config_)[0].size; }

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

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CopyTexture(const DirectXThreadContext* context, const MultiViewTexture* dst, const MultiViewTexture* src);
	// HACK: Dispatch式に変更

};