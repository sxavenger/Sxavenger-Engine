#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxGraphicsPipeline.h>

//* c++
#include <memory>
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderingPipelineType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum RenderingPipelineType {
	//* behavior
	kDefaultVS_AlbedoPS_Deferred,
	kDefaultMS_AlbedoPS_Deferred,
	kDefaultVS_AlbedoPS,
	kDefaultMS_AlbedoPS,

	kPresentToScreen,
	kCountOfRenderingPipelineType
};

////////////////////////////////////////////////////////////////////////////////////////////
// RenderingPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderingPipeline {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using RenderingArray = std::array<T, RenderingPipelineType::kCountOfRenderingPipelineType>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderingPipeline()  = default;
	~RenderingPipeline() { Term(); }

	void Init();

	void Term();

	//* pipeline option *//

	void SetPipeline(RenderingPipelineType type) const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RenderingArray<std::unique_ptr<DxObject::GraphicsBlob>>     blobs_;
	RenderingArray<std::unique_ptr<DxObject::GraphicsPipeline>> pipelines_;

	//* graphics descs *//

	DxObject::GraphicsPipelineDesc deferredPiplineDesc_ = {};
	DxObject::GraphicsPipelineDesc forwardPipelineDesc_ = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();
	void CreateForward();
	void CreateDeferred();
	void CreatePresent();

};