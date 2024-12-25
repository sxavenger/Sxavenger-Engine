#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxGraphicsPipelineState.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Geometry/Vector2.h>

//* c++
#include <array>

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPipelineType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum RenderPipelineType {
	//* behavior
	kDefaultVS_AlbedoPS_Deferred,
	kDefaultMS_AlbedoPS_Deferred,
	kDefaultVS_AlbedoPS,
	kDefaultMS_AlbedoPS,

	kPresentToScreen,
	kCountOfRenderPipelineType
};

////////////////////////////////////////////////////////////////////////////////////////////
// RenderPipelineCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderPipelineCollection {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using RenderArray = std::array<T, RenderPipelineType::kCountOfRenderPipelineType>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderPipelineCollection()  = default;
	~RenderPipelineCollection() = default;

	void Init();

	void Term();

	//* pipeline option *//

	void SetPipeline(RenderPipelineType type, const DirectXThreadContext* context, const Vector2ui& size);

	void BindGraphicsBuffer(RenderPipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RenderArray<std::unique_ptr<DxObject::ReflectionGraphicsPipelineState>> pipelines_;

	DxObject::GraphicsPipelineDesc defferedDefaultDesc_ = {};
	DxObject::GraphicsPipelineDesc forwardDefaultDesc_  = {};

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDefaultDesc();

	void CreateDeffered();

	void CreateForward();

	void CreatePresent();

};
