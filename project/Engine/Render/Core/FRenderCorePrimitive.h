#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Module/Pipeline/CustomGraphicsPipeline.h>

//* lib
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCorePrimitive class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCorePrimitive {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics pipeline *//

	std::unique_ptr<CustomReflectionGraphicsPipeline> pipeline_;

	DxObject::GraphicsPipelineDesc defferedDesc_ = {};
	DxObject::GraphicsPipelineDesc forwardDesc_ = {};

	//* directory *//

	static const std::filesystem::path kDirectory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreateDeferred();

	void CreateForward();

	

};
