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
// FRenderCoreGeometry class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCoreGeometry {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RenderType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum RenderType {
		Forward,
		Deffered,
	};
	static const size_t kRenderTypeCount = Deffered + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// VertexStage enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum VertexStage {
		DefaultVS,
	};
	static const size_t kVertexStageCount = DefaultVS + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// PixelStage enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum PixelStage {
		Albedo,
	};
	static const size_t kPixelStageCount = Albedo + 1;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCoreGeometry() = default;
	~FRenderCoreGeometry() = default;

	void Init();

	//* pipeline option *//

	void SetPipeline(
		RenderType type, VertexStage vs, PixelStage ps,
		const DirectXThreadContext* context, const Vector2ui& size
	);

	void BindGraphicsBuffer(
		RenderType type, VertexStage vs, PixelStage ps,
		const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics pipeline *//

	std::array<std::array<std::array<std::unique_ptr<CustomReflectionGraphicsPipeline>, kPixelStageCount>, kVertexStageCount>, kRenderTypeCount> graphicsPipelines_;

	DxObject::GraphicsPipelineDesc defferedDesc_ = {};
	DxObject::GraphicsPipelineDesc forwardDesc_  = {};

	//* directory *//

	static const std::filesystem::path kDirectory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateDesc();

	void CreateDeferred();

	void CreateForward();
};
